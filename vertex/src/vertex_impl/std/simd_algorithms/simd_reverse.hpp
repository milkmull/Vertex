#pragma once

#include "vertex_impl/std/simd_algorithms/simd_common.hpp"

namespace vx {
namespace _simd {

//=============================================================================
// reverse impl
//=============================================================================

namespace _reverse {

template <typename BidIt>
void reverse_tail(BidIt first, BidIt last) noexcept
{
    for (; first != last && first != --last; ++first)
    {
        const auto temp = *first;
        *first = *last;
        *last = temp;
    }
}

template <typename BidIt, typename _OutIt>
void reverse_copy_tail(const BidIt first, BidIt last, _OutIt dest) noexcept
{
    while (first != last)
    {
        *dest++ = *--last;
    }
}

#if defined(USE_ARM_NEON)

struct traits_1
{
    static uint8x8_t rev(const uint8x8_t val) noexcept
    {
        return vrev64_u8(val);
    }

    static uint8x16_t rev(const uint8x16_t val) noexcept
    {
        const uint8x16_t revval = vrev64q_u8(val);
        return vextq_u8(revval, revval, 8);
    }
};

struct traits_2
{
    static uint8x8_t rev(const uint8x8_t val) noexcept
    {
        return vreinterpret_u8_u16(vrev64_u16(vreinterpret_u16_u8(val)));
    }

    static uint8x16_t rev(const uint8x16_t val) noexcept
    {
        const uint8x16_t revval = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u8(val)));
        return vextq_u8(revval, revval, 8);
    }
};

struct traits_4
{
    static uint8x8_t rev(const uint8x8_t val) noexcept
    {
        return vreinterpret_u8_u32(vrev64_u32(vreinterpret_u32_u8(val)));
    }

    static uint8x16_t rev(const uint8x16_t val) noexcept
    {
        const uint8x16_t revval = vreinterpretq_u8_u32(vrev64q_u32(vreinterpretq_u32_u8(val)));
        return vextq_u8(revval, revval, 8);
    }
};

struct traits_8
{
    static uint8x8_t rev(const uint8x8_t val) noexcept
    {
        return val;
    }

    static uint8x16_t rev(const uint8x16_t val) noexcept
    {
        return vextq_u8(val, val, 8);
    }
};

template <typename Traits, typename T>
VX_NO_ALIAS void VX_CDECL reverse_impl(void* first, void* last) noexcept
{
    if (const size_t length = byte_length(first, last); length >= 64)
    {
        const void* stop_at = first;
        constexpr size_t mask_32 = ~((static_cast<size_t>(1) << 5) - 1);
        advance_bytes(stop_at, (length >> 1) & mask_32);
        do
        {
            advance_bytes(last, -32);

            const uint8x16_t left1 = vld1q_u8(static_cast<uint8_t*>(first) + 0);
            const uint8x16_t left2 = vld1q_u8(static_cast<uint8_t*>(first) + 16);
            const uint8x16_t right1 = vld1q_u8(static_cast<uint8_t*>(last) + 0);
            const uint8x16_t right2 = vld1q_u8(static_cast<uint8_t*>(last) + 16);

            const uint8x16_t left1reversed = Traits::rev(left1);
            const uint8x16_t left2reversed = Traits::rev(left2);
            const uint8x16_t right1reversed = Traits::rev(right1);
            const uint8x16_t right2reversed = Traits::rev(right2);

            vst1q_u8(static_cast<uint8_t*>(first) + 0, right2reversed);
            vst1q_u8(static_cast<uint8_t*>(first) + 16, right1reversed);
            vst1q_u8(static_cast<uint8_t*>(last) + 0, left2reversed);
            vst1q_u8(static_cast<uint8_t*>(last) + 16, left1reversed);

            advance_bytes(first, 32);
        } while (first != stop_at);
    }

    if (const size_t length = byte_length(first, last); length >= 32)
    {
        advance_bytes(last, -16);
        const uint8x16_t left = vld1q_u8(static_cast<uint8_t*>(first));
        const uint8x16_t right = vld1q_u8(static_cast<uint8_t*>(last));

        const uint8x16_t leftreversed = Traits::rev(left);
        const uint8x16_t rightreversed = Traits::rev(right);

        vst1q_u8(static_cast<uint8_t*>(first), rightreversed);
        vst1q_u8(static_cast<uint8_t*>(last), leftreversed);
        advance_bytes(first, 16);
    }

    if (const size_t length = byte_length(first, last); length >= 16)
    {
        advance_bytes(last, -8);
        const uint8x8_t left = vld1_u8(static_cast<uint8_t*>(first));
        const uint8x8_t right = vld1_u8(static_cast<uint8_t*>(last));

        const uint8x8_t leftreversed = Traits::rev(left);
        const uint8x8_t rightreversed = Traits::rev(right);

        vst1_u8(static_cast<uint8_t*>(first), rightreversed);
        vst1_u8(static_cast<uint8_t*>(last), leftreversed);
        advance_bytes(first, 8);
    }

    VX_IF_CONSTEXPR (sizeof(T) < 8)
    {
        if (const size_t length = byte_length(first, last); length >= 8)
        {
            advance_bytes(last, -8);

            // Intentional overlapped loads/stores: read both sides first, then write.
            const uint8x8_t left = vld1_u8(static_cast<uint8_t*>(first));
            const uint8x8_t right = vld1_u8(static_cast<uint8_t*>(last));

            const uint8x8_t leftreversed = Traits::rev(left);
            const uint8x8_t rightreversed = Traits::rev(right);

            vst1_u8(static_cast<uint8_t*>(first), rightreversed);
            vst1_u8(static_cast<uint8_t*>(last), leftreversed);

            // Overlapped stores cover any 8-15B remainder, so do not fall through to scalar tail.
            return;
        }
    }

    VX_IF_CONSTEXPR (sizeof(T) < 4)
    {
        reverse_tail(static_cast<T*>(first), static_cast<T*>(last));
    }
}

template <typename Traits, typename T>
VX_NO_ALIAS void VX_CDECL reverse_copy_impl(
    const void* first,
    const void* last,
    void* dest) noexcept
{
    if (const size_t length = byte_length(first, last); length >= 64)
    {
        const void* stop_at = dest;
        constexpr size_t mask_64 = ~((static_cast<size_t>(1) << 6) - 1);
        advance_bytes(stop_at, length & mask_64);
        do
        {
            advance_bytes(last, -64);
            const uint8x16_t block1 = vld1q_u8(static_cast<const uint8_t*>(last) + 0);
            const uint8x16_t block2 = vld1q_u8(static_cast<const uint8_t*>(last) + 16);
            const uint8x16_t block3 = vld1q_u8(static_cast<const uint8_t*>(last) + 32);
            const uint8x16_t block4 = vld1q_u8(static_cast<const uint8_t*>(last) + 48);

            const uint8x16_t block1reversed = Traits::rev(block1);
            const uint8x16_t block2reversed = Traits::rev(block2);
            const uint8x16_t block3reversed = Traits::rev(block3);
            const uint8x16_t block4reversed = Traits::rev(block4);

            vst1q_u8(static_cast<uint8_t*>(dest) + 0, block4reversed);
            vst1q_u8(static_cast<uint8_t*>(dest) + 16, block3reversed);
            vst1q_u8(static_cast<uint8_t*>(dest) + 32, block2reversed);
            vst1q_u8(static_cast<uint8_t*>(dest) + 48, block1reversed);
            advance_bytes(dest, 64);
        } while (dest != stop_at);
    }

    if (const size_t length = byte_length(first, last); length >= 32)
    {
        advance_bytes(last, -32);
        const uint8x16_t block1 = vld1q_u8(static_cast<const uint8_t*>(last) + 0);
        const uint8x16_t block2 = vld1q_u8(static_cast<const uint8_t*>(last) + 16);

        const uint8x16_t block1reversed = Traits::rev(block1);
        const uint8x16_t block2reversed = Traits::rev(block2);

        vst1q_u8(static_cast<uint8_t*>(dest) + 0, block2reversed);
        vst1q_u8(static_cast<uint8_t*>(dest) + 16, block1reversed);
        advance_bytes(dest, 32);
    }

    if (const size_t length = byte_length(first, last); length >= 16)
    {
        advance_bytes(last, -16);
        const uint8x16_t block = vld1q_u8(static_cast<const uint8_t*>(last));
        const uint8x16_t blockreversed = Traits::rev(block);
        vst1q_u8(static_cast<uint8_t*>(dest), blockreversed);
        advance_bytes(dest, 16);
    }

    if (const size_t length = byte_length(first, last); length >= 8)
    {
        advance_bytes(last, -8);
        const uint8x8_t block = vld1_u8(static_cast<const uint8_t*>(last));
        const uint8x8_t blockreversed = Traits::rev(block);
        vst1_u8(static_cast<uint8_t*>(dest), blockreversed);
        advance_bytes(dest, 8);
    }

    VX_IF_CONSTEXPR (sizeof(T) < 8)
    {
        reverse_copy_tail(
            static_cast<const T*>(first),
            static_cast<const T*>(last),
            static_cast<T*>(dest));
    }
}

#elif defined(USE_X86)

struct traits_1
{
    #if defined(USE_AVX2)

    static __m256i rev_avx(const __m256i val) noexcept
    {
        const __m256i reverse_char_lanes_avx = _mm256_set_epi8(   //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

        const __m256i perm = _mm256_permute4x64_epi64(val, _MM_SHUFFLE(1, 0, 3, 2));
        return _mm256_shuffle_epi8(perm, reverse_char_lanes_avx);
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i rev_sse(const __m128i val) noexcept
    {
        const __m128i reverse_char_sse = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        return _mm_shuffle_epi8(val, reverse_char_sse);
    }

    #endif // defined(USE_SSE2)
};

struct traits_2
{
    #if defined(USE_AVX2)

    static __m256i rev_avx(const __m256i val) noexcept
    {
        const __m256i reverse_short_lanes_avx = _mm256_set_epi8(  //
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, //
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);

        const __m256i perm = _mm256_permute4x64_epi64(val, _MM_SHUFFLE(1, 0, 3, 2));
        return _mm256_shuffle_epi8(perm, reverse_short_lanes_avx);
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i rev_sse(const __m128i val) noexcept
    {
        const __m128i reverse_short_sse = _mm_set_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
        return _mm_shuffle_epi8(val, reverse_short_sse);
    }

    #endif // defined(USE_SSE2)
};

struct traits_4
{
    #if defined(USE_AVX2)

    static __m256i rev_avx(const __m256i val) noexcept
    {
        const __m256i _Shuf = _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        return _mm256_permutevar8x32_epi32(val, _Shuf);
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i rev_sse(const __m128i val) noexcept
    {
        return _mm_shuffle_epi32(val, _MM_SHUFFLE(0, 1, 2, 3));
    }

    #endif // defined(USE_SSE2)
};

struct traits_8
{
    #if defined(USE_AVX2)

    static __m256i rev_avx(const __m256i val) noexcept
    {
        return _mm256_permute4x64_epi64(val, _MM_SHUFFLE(0, 1, 2, 3));
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i rev_sse(const __m128i val) noexcept
    {
        return _mm_shuffle_epi32(val, _MM_SHUFFLE(1, 0, 3, 2));
    }

    #endif // defined(USE_SSE2)
};

    #if defined(USE_AVX2)

__m256i avx2_rev_tail_mask_32(const size_t count_in_bytes) noexcept
{
    // _Count_in_bytes must be within [0, 32].
    static constexpr unsigned int tail_masks[16] = {
        0, 0, 0, 0, 0, 0, 0, 0, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u
    };
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(
        reinterpret_cast<const unsigned char*>(tail_masks) + count_in_bytes));
}

    #endif // defined(USE_AVX2)

template <typename Traits, typename T>
VX_NO_ALIAS void VX_CDECL reverse_impl(void* first, void* last) noexcept
{
    #if defined(USE_AVX2)

    if (const size_t length = byte_length(first, last); length >= 64)
    {
        const void* stop_at = first;
        advance_bytes(stop_at, (length >> 1) & ~size_t{ 0x1F });
        do
        {
            advance_bytes(last, -32);
            const __m256i left = _mm256_loadu_si256(static_cast<__m256i*>(first));
            const __m256i right = _mm256_loadu_si256(static_cast<__m256i*>(last));
            const __m256i leftreversed = Traits::rev_avx(left);
            const __m256i rightreversed = Traits::rev_avx(right);
            _mm256_storeu_si256(static_cast<__m256i*>(first), rightreversed);
            _mm256_storeu_si256(static_cast<__m256i*>(last), leftreversed);
            advance_bytes(first, 32);
        } while (first != stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    if (const size_t length = byte_length(first, last); length >= 32)
    {
        const void* stop_at = first;
        advance_bytes(stop_at, (length >> 1) & ~size_t{ 0xF });
        do
        {
            advance_bytes(last, -16);
            const __m128i left = _mm_loadu_si128(static_cast<__m128i*>(first));
            const __m128i right = _mm_loadu_si128(static_cast<__m128i*>(last));
            const __m128i leftreversed = Traits::rev_sse(left);
            const __m128i rightreversed = Traits::rev_sse(right);
            _mm_storeu_si128(static_cast<__m128i*>(first), rightreversed);
            _mm_storeu_si128(static_cast<__m128i*>(last), leftreversed);
            advance_bytes(first, 16);
        } while (first != stop_at);
    }

    #endif // defined(USE_SSE2)

    reverse_tail(static_cast<T*>(first), static_cast<T*>(last));
}

template <typename Traits, typename T>
VX_NO_ALIAS void VX_CDECL reverse_copy_impl(
    const void* first,
    const void* last,
    void* dest) noexcept
{
    #if defined(USE_AVX2)

    if (const size_t length = byte_length(first, last); length >= 32)
    {
        const void* stop_at = dest;
        advance_bytes(stop_at, length & ~size_t{ 0x1F });
        do
        {
            advance_bytes(last, -32);
            const __m256i block = _mm256_loadu_si256(static_cast<const __m256i*>(last));
            const __m256i blockreversed = Traits::rev_avx(block);
            _mm256_storeu_si256(static_cast<__m256i*>(dest), blockreversed);
            advance_bytes(dest, 32);
        } while (dest != stop_at);

        if (const size_t avx_tail = length & 0x1C; avx_tail != 0)
        {
            advance_bytes(last, -32);
            const __m256i mask = avx2_tail_mask_32(avx_tail);
            const __m256i rev_mask = avx2_rev_tail_mask_32(avx_tail);
            const __m256i block = _mm256_maskload_epi32(static_cast<const int*>(last), rev_mask);
            const __m256i blockreversed = Traits::rev_avx(block);
            _mm256_maskstore_epi32(static_cast<int*>(dest), mask, blockreversed);
            VX_IF_CONSTEXPR (sizeof(T) < 4)
            {
                advance_bytes(dest, avx_tail);
                advance_bytes(last, 32 - avx_tail);
            }
        }

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414

        VX_IF_CONSTEXPR (sizeof(T) >= 4)
        {
            return;
        }
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    if (const size_t length = byte_length(first, last); length >= 16)
    {
        const void* stop_at = dest;
        advance_bytes(stop_at, length & ~size_t{ 0xF });
        do
        {
            advance_bytes(last, -16);
            const __m128i block = _mm_loadu_si128(static_cast<const __m128i*>(last));
            const __m128i blockreversed = Traits::rev_sse(block);
            _mm_storeu_si128(static_cast<__m128i*>(dest), blockreversed);
            advance_bytes(dest, 16);
        } while (dest != stop_at);
    }

    #endif // defined(USE_SSE2)

    reverse_copy_tail(
        static_cast<const T*>(first),
        static_cast<const T*>(last),
        static_cast<T*>(dest));
}

#endif // defined(USE_X86)

} // namespace _reverse

//=============================================================================
// reverse functions
//=============================================================================

extern "C" {

VX_NO_ALIAS void VX_CDECL reverse_trivially_swappable_1(void* first, void* last) noexcept
{
    _reverse::reverse_impl<_reverse::traits_1, uint8_t>(first, last);
}

VX_NO_ALIAS void VX_CDECL reverse_trivially_swappable_2(void* first, void* last) noexcept
{
    _reverse::reverse_impl<_reverse::traits_2, uint16_t>(first, last);
}

VX_NO_ALIAS void VX_CDECL reverse_trivially_swappable_4(void* first, void* last) noexcept
{
    _reverse::reverse_impl<_reverse::traits_4, uint32_t>(first, last);
}

VX_NO_ALIAS void VX_CDECL reverse_trivially_swappable_8(void* first, void* last) noexcept
{
    _reverse::reverse_impl<_reverse::traits_8, uint64_t>(first, last);
}

VX_NO_ALIAS void VX_CDECL reverse_copy_trivially_copyable_1(
    const void* first,
    const void* last,
    void* dest) noexcept
{
    _reverse::reverse_copy_impl<_reverse::traits_1, uint8_t>(first, last, dest);
}

VX_NO_ALIAS void VX_CDECL reverse_copy_trivially_copyable_2(
    const void* first,
    const void* last,
    void* dest) noexcept
{
    _reverse::reverse_copy_impl<_reverse::traits_2, uint16_t>(first, last, dest);
}

VX_NO_ALIAS void VX_CDECL reverse_copy_trivially_copyable_4(
    const void* first,
    const void* last,
    void* dest) noexcept
{
    _reverse::reverse_copy_impl<_reverse::traits_4, uint32_t>(first, last, dest);
}

VX_NO_ALIAS void VX_CDECL reverse_copy_trivially_copyable_8(
    const void* first,
    const void* last,
    void* dest) noexcept
{
    _reverse::reverse_copy_impl<_reverse::traits_8, uint64_t>(first, last, dest);
}

} // extern "C"

} // namespace _simd
} // namespace vx
