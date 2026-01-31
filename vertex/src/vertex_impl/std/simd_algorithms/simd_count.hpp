#pragma once

#include "vertex_impl/std/simd_algorithms/simd_find.hpp"

namespace vx {
namespace _simd {

//=============================================================================
// count impl
//=============================================================================

namespace _count {

#if defined(USE_X86)

struct count_traits_8 : _find::find_traits_8
{
    #if defined(USE_AVX2)

    static __m256i sub_avx(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_sub_epi64(lhs, rhs);
    }

    static size_t reduce_avx(const __m256i val) noexcept
    {
        const __m128i _Lo64 = _mm256_extracti128_si256(val, 0);
        const __m128i _Hi64 = _mm256_extracti128_si256(val, 1);
        const __m128i rx8 = _mm_add_epi64(_Lo64, _Hi64);
        return reduce_sse(rx8);
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i sub_sse(const __m128i lhs, const __m128i rhs) noexcept
    {
        return _mm_sub_epi64(lhs, rhs);
    }

    static size_t reduce_sse(const __m128i val) noexcept
    {
        #if defined(VX_ARCH_WORD_BITS_32)
        return static_cast<uint32_t>(_mm_cvtsi128_si32(val)) + static_cast<uint32_t>(_mm_extract_epi32(val, 2));
        #else  // // ^^^ 32-bit / 64-bit vvv
        return _mm_cvtsi128_si64(val) + _mm_extract_epi64(val, 1);
        #endif // ^^^ 64-bit ^^^
    }

    #endif // defined(USE_SSE2)
};

struct count_traits_4 : _find::find_traits_4
{
    // For AVX2, we use hadd_epi32 three times to combine pairs of 32-bit counters into 32-bit results.
    // Therefore, max_count is 0x1FFF'FFFF, which is 0xFFFF'FFF8 when doubled three times; any more would
    // overflow.

    // For SSE4.2, we use hadd_epi32 twice. This would allow a larger limit,
    // but it's simpler to use the smaller limit for both codepaths.

    static constexpr size_t max_count = 0x1FFF'FFFF;

    #if defined(USE_AVX2)

    static __m256i sub_avx(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_sub_epi32(lhs, rhs);
    }

    static size_t reduce_avx(const __m256i val) noexcept
    {
        constexpr auto shuf = _MM_SHUFFLE(3, 1, 2, 0);                      // Cross lane, to reduce further on low lane
        const __m256i rx4 = _mm256_hadd_epi32(val, _mm256_setzero_si256()); // (0+1),(2+3),0,0 per lane
        const __m256i rx5 = _mm256_permute4x64_epi64(rx4, shuf);            // low lane  (0+1),(2+3),(4+5),(6+7)
        const __m256i rx6 = _mm256_hadd_epi32(rx5, _mm256_setzero_si256()); // (0+...+3),(4+...+7),0,0
        const __m256i rx7 = _mm256_hadd_epi32(rx6, _mm256_setzero_si256()); // (0+...+7),0,0,0
        return static_cast<uint32_t>(_mm_cvtsi128_si32(_mm256_castsi256_si128(rx7)));
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i sub_sse(const __m128i lhs, const __m128i rhs) noexcept
    {
        return _mm_sub_epi32(lhs, rhs);
    }

    static size_t reduce_sse(const __m128i val) noexcept
    {
        const __m128i rx4 = _mm_hadd_epi32(val, _mm_setzero_si128()); // (0+1),(2+3),0,0
        const __m128i rx5 = _mm_hadd_epi32(rx4, _mm_setzero_si128()); // (0+...+3),0,0,0
        return static_cast<uint32_t>(_mm_cvtsi128_si32(rx5));
    }

    #endif // defined(USE_SSE2)
};

struct count_traits_2 : _find::find_traits_2
{
    // For both AVX2 and SSE4.2, we use hadd_epi16 once to combine pairs of 16-bit counters into 16-bit results.
    // Therefore, max_count is 0x7FFF, which is 0xFFFE when doubled; any more would overflow.

    static constexpr size_t max_count = 0x7FFF;

    #if defined(USE_AVX2)

    static __m256i sub_avx(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_sub_epi16(lhs, rhs);
    }

    static size_t reduce_avx(const __m256i val) noexcept
    {
        const __m256i rx2 = _mm256_hadd_epi16(val, _mm256_setzero_si256());
        const __m256i rx3 = _mm256_unpacklo_epi16(rx2, _mm256_setzero_si256());
        return count_traits_4::reduce_avx(rx3);
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i sub_sse(const __m128i lhs, const __m128i rhs) noexcept
    {
        return _mm_sub_epi16(lhs, rhs);
    }

    static size_t reduce_sse(const __m128i val) noexcept
    {
        const __m128i rx2 = _mm_hadd_epi16(val, _mm_setzero_si128());
        const __m128i rx3 = _mm_unpacklo_epi16(rx2, _mm_setzero_si128());
        return count_traits_4::reduce_sse(rx3);
    }

    #endif // defined(USE_SSE2)
};

struct count_traits_1 : _find::find_traits_1
{
    // For AVX2, max_portion_size below is max_count * 32 bytes, and we have 1-byte elements.
    // We're using packed 8-bit counters, and 32 of those fit in 256 bits.

    // For SSE4.2, max_portion_size below is max_count * 16 bytes, and we have 1-byte elements.
    // We're using packed 8-bit counters, and 16 of those fit in 128 bits.

    // For both codepaths, this is why max_count is the maximum unsigned 8-bit integer.
    // (The reduction steps aren't the limiting factor here.)

    static constexpr size_t max_count = 0xFF;

    #if defined(VX_USE_AVX2)

    static __m256i sub_avx(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_sub_epi8(lhs, rhs);
    }

    static size_t reduce_avx(const __m256i val) noexcept
    {
        const __m256i rx1 = _mm256_sad_epu8(val, _mm256_setzero_si256());
        return count_traits_8::reduce_avx(rx1);
    }

    #endif // defined(USE_AVX2

    #if defined(USE_SSE2)

    static __m128i sub_sse(const __m128i lhs, const __m128i rhs) noexcept
    {
        return _mm_sub_epi8(lhs, rhs);
    }

    static size_t reduce_sse(const __m128i val) noexcept
    {
        const __m128i rx1 = _mm_sad_epu8(val, _mm_setzero_si128());
        return count_traits_8::reduce_sse(rx1);
    }

    #endif // defined(USE_SSE2)
};

#endif

template <typename Traits, typename T>
VX_NO_ALIAS size_t VX_STDCALL count_impl(
    const void* first,
    const void* const last,
    const T val) noexcept
{
    size_t result = 0;
    const size_t size_bytes = byte_length(first, last);

#if defined(USE_AVX2)

    if (size_t avx_size = size_bytes & ~size_t{ 0x1F }; avx_size != 0)
    {
        const __m256i comparand = Traits::set_avx(val);
        const void* stop_at = first;

        for (;;)
        {
            VX_IF_CONSTEXPR (sizeof(T) >= sizeof(size_t))
            {
                advance_bytes(stop_at, avx_size);
            }
            else
            {
                constexpr size_t max_portion_size = Traits::max_count * 32;
                const size_t portion_size = avx_size < max_portion_size ? avx_size : max_portion_size;
                advance_bytes(stop_at, portion_size);
                avx_size -= portion_size;
            }

            __m256i count_vector = _mm256_setzero_si256();

            do
            {
                const __m256i data = _mm256_loadu_si256(static_cast<const __m256i*>(first));
                const __m256i mask = Traits::cmp_avx(data, comparand);
                count_vector = Traits::sub_avx(count_vector, mask);
                advance_bytes(first, 32);
            } while (first != stop_at);

            result += Traits::reduce_avx(count_vector);

            VX_IF_CONSTEXPR (sizeof(T) >= sizeof(size_t))
            {
                break;
            }
            else
            {
                if (avx_size == 0)
                {
                    break;
                }
            }
        }

        if (const size_t avx_tail_size = size_bytes & 0x1C; avx_tail_size != 0)
        {
            const __m256i tail_mask = avx2_tail_mask_32(avx_tail_size);
            const __m256i data = _mm256_maskload_epi32(static_cast<const int*>(first), tail_mask);
            const __m256i mask = _mm256_and_si256(Traits::cmp_avx(data, comparand), tail_mask);
            const unsigned int bingo = _mm256_movemask_epi8(mask);
            const size_t tail_count = __popcnt(bingo); // Assume available with SSE4.2
            result += tail_count / sizeof(T);
            advance_bytes(first, avx_tail_size);
        }

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414

        VX_IF_CONSTEXPR (sizeof(T) >= 4)
        {
            return result;
        }
    }

#elif defined(USE_SSE2)

    if (size_t sse_size = size_bytes & ~size_t{ 0xF }; sse_size != 0)
    {
        const __m128i comparand = Traits::set_sse(val);
        const void* stop_at = first;

        for (;;)
        {
            VX_IF_CONSTEXPR (sizeof(T) >= sizeof(size_t))
            {
                advance_bytes(stop_at, sse_size);
            }
            else
            {
                constexpr size_t max_portion_size = Traits::max_count * 16;
                const size_t portion_size = sse_size < max_portion_size ? sse_size : max_portion_size;
                advance_bytes(stop_at, portion_size);
                sse_size -= portion_size;
            }

            __m128i count_vector = _mm_setzero_si128();

            do
            {
                const __m128i data = _mm_loadu_si128(static_cast<const __m128i*>(first));
                const __m128i mask = Traits::cmp_sse(data, comparand);
                count_vector = Traits::sub_sse(count_vector, mask);
                advance_bytes(first, 16);
            } while (first != stop_at);

            result += Traits::reduce_sse(count_vector);

            VX_IF_CONSTEXPR (sizeof(T) >= sizeof(size_t))
            {
                break;
            }
            else
            {
                if (sse_size == 0)
                {
                    break;
                }
            }
        }
    }

#endif

    for (auto ptr = static_cast<const T*>(first); ptr != last; ++ptr)
    {
        if (*ptr == val)
        {
            ++result;
        }
    }
    return result;
}

} // namespace _count

//=============================================================================
// count functions
//=============================================================================

extern "C" {

VX_NO_ALIAS size_t VX_STDCALL count_trivial_1(
    const void* const first,
    const void* const last,
    const uint8_t val) noexcept
{
    return _count::count_impl<_count::count_traits_1>(first, last, val);
}

VX_NO_ALIAS size_t VX_STDCALL count_trivial_2(
    const void* const first,
    const void* const last,
    const uint16_t val) noexcept
{
    return _count::count_impl<_count::count_traits_2>(first, last, val);
}

VX_NO_ALIAS size_t VX_STDCALL count_trivial_4(
    const void* const first,
    const void* const last,
    const uint32_t val) noexcept
{
    return _count::count_impl<_count::count_traits_4>(first, last, val);
}

VX_NO_ALIAS size_t VX_STDCALL count_trivial_8(
    const void* const first,
    const void* const last,
    const uint64_t val) noexcept
{
    return _count::count_impl<_count::count_traits_8>(first, last, val);
}

} // extern "C"

} // namespace _simd
} // namespace vx
