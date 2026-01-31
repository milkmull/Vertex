#pragma once

#include "vertex_impl/std/simd_algorithms/simd_common.hpp"

namespace vx {
namespace _simd {

//=============================================================================
// rotate impl
//=============================================================================

namespace _rotate {

#if defined(USE_ARM_NEON)

void VX_FORCE_INLINE swap_3_ranges(void* first1, void* const last1, void* first2, void* first3) noexcept
{
    if (byte_length(first1, last1) >= 64)
    {
        constexpr size_t mask_64 = ~((static_cast<size_t>(1) << 6) - 1);
        const void* stop_at = first1;
        advance_bytes(stop_at, byte_length(first1, last1) & mask_64);
        do
        {
            const uint8x16_t val1_lo1 = vld1q_u8(static_cast<uint8_t*>(first1) + 0);
            const uint8x16_t val1_lo2 = vld1q_u8(static_cast<uint8_t*>(first1) + 16);
            const uint8x16_t val1_hi1 = vld1q_u8(static_cast<uint8_t*>(first1) + 32);
            const uint8x16_t val1_hi2 = vld1q_u8(static_cast<uint8_t*>(first1) + 48);
            const uint8x16_t val2_lo1 = vld1q_u8(static_cast<uint8_t*>(first2) + 0);
            const uint8x16_t val2_lo2 = vld1q_u8(static_cast<uint8_t*>(first2) + 16);
            const uint8x16_t val2_hi1 = vld1q_u8(static_cast<uint8_t*>(first2) + 32);
            const uint8x16_t val2_hi2 = vld1q_u8(static_cast<uint8_t*>(first2) + 48);
            const uint8x16_t val3_lo1 = vld1q_u8(static_cast<uint8_t*>(first3) + 0);
            const uint8x16_t val3_lo2 = vld1q_u8(static_cast<uint8_t*>(first3) + 16);
            const uint8x16_t val3_hi1 = vld1q_u8(static_cast<uint8_t*>(first3) + 32);
            const uint8x16_t val3_hi2 = vld1q_u8(static_cast<uint8_t*>(first3) + 48);
            vst1q_u8(static_cast<uint8_t*>(first1) + 0, val2_lo1);
            vst1q_u8(static_cast<uint8_t*>(first1) + 16, val2_lo2);
            vst1q_u8(static_cast<uint8_t*>(first1) + 32, val2_hi1);
            vst1q_u8(static_cast<uint8_t*>(first1) + 48, val2_hi2);
            vst1q_u8(static_cast<uint8_t*>(first2) + 0, val3_lo1);
            vst1q_u8(static_cast<uint8_t*>(first2) + 16, val3_lo2);
            vst1q_u8(static_cast<uint8_t*>(first2) + 32, val3_hi1);
            vst1q_u8(static_cast<uint8_t*>(first2) + 48, val3_hi2);
            vst1q_u8(static_cast<uint8_t*>(first3) + 0, val1_lo1);
            vst1q_u8(static_cast<uint8_t*>(first3) + 16, val1_lo2);
            vst1q_u8(static_cast<uint8_t*>(first3) + 32, val1_hi1);
            vst1q_u8(static_cast<uint8_t*>(first3) + 48, val1_hi2);
            advance_bytes(first1, 64);
            advance_bytes(first2, 64);
            advance_bytes(first3, 64);
        } while (first1 != stop_at);
    }

    if (byte_length(first1, last1) >= 32)
    {
        const uint8x16_t val1_lo = vld1q_u8(static_cast<uint8_t*>(first1) + 0);
        const uint8x16_t val1_hi = vld1q_u8(static_cast<uint8_t*>(first1) + 16);
        const uint8x16_t val2_lo = vld1q_u8(static_cast<uint8_t*>(first2) + 0);
        const uint8x16_t val2_hi = vld1q_u8(static_cast<uint8_t*>(first2) + 16);
        const uint8x16_t val3_lo = vld1q_u8(static_cast<uint8_t*>(first3) + 0);
        const uint8x16_t val3_hi = vld1q_u8(static_cast<uint8_t*>(first3) + 16);
        vst1q_u8(static_cast<uint8_t*>(first1) + 0, val2_lo);
        vst1q_u8(static_cast<uint8_t*>(first1) + 16, val2_hi);
        vst1q_u8(static_cast<uint8_t*>(first2) + 0, val3_lo);
        vst1q_u8(static_cast<uint8_t*>(first2) + 16, val3_hi);
        vst1q_u8(static_cast<uint8_t*>(first3) + 0, val1_lo);
        vst1q_u8(static_cast<uint8_t*>(first3) + 16, val1_hi);
        advance_bytes(first1, 32);
        advance_bytes(first2, 32);
        advance_bytes(first3, 32);
    }

    if (byte_length(first1, last1) >= 16)
    {
        const uint8x16_t val1 = vld1q_u8(static_cast<uint8_t*>(first1));
        const uint8x16_t val2 = vld1q_u8(static_cast<uint8_t*>(first2));
        const uint8x16_t val3 = vld1q_u8(static_cast<uint8_t*>(first3));
        vst1q_u8(static_cast<uint8_t*>(first1), val2);
        vst1q_u8(static_cast<uint8_t*>(first2), val3);
        vst1q_u8(static_cast<uint8_t*>(first3), val1);
        advance_bytes(first1, 16);
        advance_bytes(first2, 16);
        advance_bytes(first3, 16);
    }

    if (byte_length(first1, last1) >= 8)
    {
        const uint8x8_t val1 = vld1_u8(static_cast<uint8_t*>(first1));
        const uint8x8_t val2 = vld1_u8(static_cast<uint8_t*>(first2));
        const uint8x8_t val3 = vld1_u8(static_cast<uint8_t*>(first3));
        vst1_u8(static_cast<uint8_t*>(first1), val2);
        vst1_u8(static_cast<uint8_t*>(first2), val3);
        vst1_u8(static_cast<uint8_t*>(first3), val1);
        advance_bytes(first1, 8);
        advance_bytes(first2, 8);
        advance_bytes(first3, 8);
    }

    if (byte_length(first1, last1) >= 4)
    {
        uint32x2_t val1 = vdup_n_u32(0);
        uint32x2_t val2 = vdup_n_u32(0);
        uint32x2_t val3 = vdup_n_u32(0);
        val1 = vld1_lane_u32(static_cast<uint32_t*>(first1), val1, 0);
        val2 = vld1_lane_u32(static_cast<uint32_t*>(first2), val2, 0);
        val3 = vld1_lane_u32(static_cast<uint32_t*>(first3), val3, 0);
        vst1_lane_u32(static_cast<uint32_t*>(first1), val2, 0);
        vst1_lane_u32(static_cast<uint32_t*>(first2), val3, 0);
        vst1_lane_u32(static_cast<uint32_t*>(first3), val1, 0);
        advance_bytes(first1, 4);
        advance_bytes(first2, 4);
        advance_bytes(first3, 4);
    }

    auto first1c = static_cast<unsigned char*>(first1);
    auto first2c = static_cast<unsigned char*>(first2);
    auto first3c = static_cast<unsigned char*>(first3);
    for (; first1c != last1; ++first1c, ++first2c, ++first3c)
    {
        const unsigned char ch = *first1c;
        *first1c = *first2c;
        *first2c = *first3c;
        *first3c = ch;
    }
}

#else

void swap_3_ranges(void* first1, void* const last1, void* first2, void* first3) noexcept
{
    #if defined(USE_AVX2)

    constexpr size_t mask_32 = ~((static_cast<size_t>(1) << 5) - 1);
    if (byte_length(first1, last1) >= 32)
    {
        const void* stop_at = first1;
        advance_bytes(stop_at, byte_length(first1, last1) & mask_32);
        do
        {
            const __m256i val1 = _mm256_loadu_si256(static_cast<__m256i*>(first1));
            const __m256i val2 = _mm256_loadu_si256(static_cast<__m256i*>(first2));
            const __m256i val3 = _mm256_loadu_si256(static_cast<__m256i*>(first3));
            _mm256_storeu_si256(static_cast<__m256i*>(first1), val2);
            _mm256_storeu_si256(static_cast<__m256i*>(first2), val3);
            _mm256_storeu_si256(static_cast<__m256i*>(first3), val1);
            advance_bytes(first1, 32);
            advance_bytes(first2, 32);
            advance_bytes(first3, 32);

        } while (first1 != stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    constexpr size_t mask_16 = ~((static_cast<size_t>(1) << 4) - 1);
    if (byte_length(first1, last1) >= 16)
    {
        const void* stop_at = first1;
        advance_bytes(stop_at, byte_length(first1, last1) & mask_16);
        do
        {
            const __m128i val1 = _mm_loadu_si128(static_cast<__m128i*>(first1));
            const __m128i val2 = _mm_loadu_si128(static_cast<__m128i*>(first2));
            const __m128i val3 = _mm_loadu_si128(static_cast<__m128i*>(first3));
            _mm_storeu_si128(static_cast<__m128i*>(first1), val2);
            _mm_storeu_si128(static_cast<__m128i*>(first2), val3);
            _mm_storeu_si128(static_cast<__m128i*>(first3), val1);
            advance_bytes(first1, 16);
            advance_bytes(first2, 16);
            advance_bytes(first3, 16);

        } while (first1 != stop_at);
    }

    #endif // defined(USE_SSE2)

    #if defined(VX_ARCH_WORD_BITS_64)

    constexpr size_t mask_8 = ~((static_cast<size_t>(1) << 3) - 1);
    if (byte_length(first1, last1) >= 8)
    {
        const void* stop_at = first1;
        advance_bytes(stop_at, byte_length(first1, last1) & mask_8);
        do
        {
            unsigned long long val1;
            unsigned long long val2;
            unsigned long long val3;
            ::memcpy(&val1, first1, 8);
            ::memcpy(&val2, first2, 8);
            ::memcpy(&val3, first3, 8);
            ::memcpy(first1, &val2, 8);
            ::memcpy(first2, &val3, 8);
            ::memcpy(first3, &val1, 8);
            advance_bytes(first1, 8);
            advance_bytes(first2, 8);
            advance_bytes(first3, 8);
        } while (first1 != stop_at);
    }

    #else // ^^^ 64-bit / 32-bit vvv

    constexpr size_t mask_4 = ~((static_cast<size_t>(1) << 2) - 1);
    if (byte_length(first1, last1) >= 4)
    {
        const void* stop_at = first1;
        advance_bytes(stop_at, byte_length(first1, last1) & mask_4);
        do
        {
            unsigned long val1;
            unsigned long val2;
            unsigned long val3;
            ::memcpy(&val1, first1, 4);
            ::memcpy(&val2, first2, 4);
            ::memcpy(&val3, first3, 4);
            ::memcpy(first1, &val2, 4);
            ::memcpy(first2, &val3, 4);
            ::memcpy(first3, &val1, 4);
            advance_bytes(first1, 4);
            advance_bytes(first2, 4);
            advance_bytes(first3, 4);
        } while (first1 != stop_at);
    }

    #endif // ^^^ 32-bit ^^^

    auto first1c = static_cast<unsigned char*>(first1);
    auto first2c = static_cast<unsigned char*>(first2);
    auto first3c = static_cast<unsigned char*>(first3);
    for (; first1c != last1; ++first1c, ++first2c, ++first3c)
    {
        const unsigned char ch = *first1c;
        *first1c = *first2c;
        *first2c = *first3c;
        *first3c = ch;
    }
}

#endif

constexpr size_t buf_size = 512;

bool use_buffer(const size_t smaller, const size_t larger) noexcept
{
    return smaller <= buf_size && (smaller <= 128 || larger >= smaller * 2);
}

} // namespace _rotate

//=============================================================================

extern "C" {

#if defined(USE_ARM_NEON)

VX_NO_ALIAS void VX_CDECL swap_ranges_trivially_swappable_noalias(
    void* first1,
    void* const last1,
    void* first2) noexcept
{
    if (byte_length(first1, last1) >= 64)
    {
        constexpr size_t mask_64 = ~((static_cast<size_t>(1) << 6) - 1);
        const void* stop_at = first1;
        advance_bytes(stop_at, byte_length(first1, last1) & mask_64);
        do
        {
            const uint8x16_t left1 = vld1q_u8(static_cast<uint8_t*>(first1) + 0);
            const uint8x16_t left2 = vld1q_u8(static_cast<uint8_t*>(first1) + 16);
            const uint8x16_t left3 = vld1q_u8(static_cast<uint8_t*>(first1) + 32);
            const uint8x16_t left4 = vld1q_u8(static_cast<uint8_t*>(first1) + 48);
            const uint8x16_t right1 = vld1q_u8(static_cast<uint8_t*>(first2) + 0);
            const uint8x16_t right2 = vld1q_u8(static_cast<uint8_t*>(first2) + 16);
            const uint8x16_t right3 = vld1q_u8(static_cast<uint8_t*>(first2) + 32);
            const uint8x16_t right4 = vld1q_u8(static_cast<uint8_t*>(first2) + 48);
            vst1q_u8(static_cast<uint8_t*>(first1) + 0, right1);
            vst1q_u8(static_cast<uint8_t*>(first1) + 16, right2);
            vst1q_u8(static_cast<uint8_t*>(first1) + 32, right3);
            vst1q_u8(static_cast<uint8_t*>(first1) + 48, right4);
            vst1q_u8(static_cast<uint8_t*>(first2) + 0, left1);
            vst1q_u8(static_cast<uint8_t*>(first2) + 16, left2);
            vst1q_u8(static_cast<uint8_t*>(first2) + 32, left3);
            vst1q_u8(static_cast<uint8_t*>(first2) + 48, left4);
            advance_bytes(first1, 64);
            advance_bytes(first2, 64);
        } while (first1 != stop_at);
    }

    if (byte_length(first1, last1) >= 32)
    {
        const uint8x16_t left1 = vld1q_u8(static_cast<uint8_t*>(first1) + 0);
        const uint8x16_t left2 = vld1q_u8(static_cast<uint8_t*>(first1) + 16);
        const uint8x16_t right1 = vld1q_u8(static_cast<uint8_t*>(first2) + 0);
        const uint8x16_t right2 = vld1q_u8(static_cast<uint8_t*>(first2) + 16);
        vst1q_u8(static_cast<uint8_t*>(first1) + 0, right1);
        vst1q_u8(static_cast<uint8_t*>(first1) + 16, right2);
        vst1q_u8(static_cast<uint8_t*>(first2) + 0, left1);
        vst1q_u8(static_cast<uint8_t*>(first2) + 16, left2);
        advance_bytes(first1, 32);
        advance_bytes(first2, 32);
    }

    if (byte_length(first1, last1) >= 16)
    {
        const uint8x16_t left = vld1q_u8(static_cast<uint8_t*>(first1));
        const uint8x16_t right = vld1q_u8(static_cast<uint8_t*>(first2));
        vst1q_u8(static_cast<uint8_t*>(first1), right);
        vst1q_u8(static_cast<uint8_t*>(first2), left);
        advance_bytes(first1, 16);
        advance_bytes(first2, 16);
    }

    if (byte_length(first1, last1) >= 8)
    {
        const uint8x8_t left = vld1_u8(static_cast<uint8_t*>(first1));
        const uint8x8_t right = vld1_u8(static_cast<uint8_t*>(first2));
        vst1_u8(static_cast<uint8_t*>(first1), right);
        vst1_u8(static_cast<uint8_t*>(first2), left);
        advance_bytes(first1, 8);
        advance_bytes(first2, 8);
    }

    if (byte_length(first1, last1) >= 4)
    {
        uint32x2_t left = vdup_n_u32(0);
        uint32x2_t right = vdup_n_u32(0);
        left = vld1_lane_u32(static_cast<uint32_t*>(first1), left, 0);
        right = vld1_lane_u32(static_cast<uint32_t*>(first2), right, 0);
        vst1_lane_u32(static_cast<uint32_t*>(first1), right, 0);
        vst1_lane_u32(static_cast<uint32_t*>(first2), left, 0);
        advance_bytes(first1, 4);
        advance_bytes(first2, 4);
    }

    auto first1c = static_cast<unsigned char*>(first1);
    auto first2c = static_cast<unsigned char*>(first2);
    for (; first1c != last1; ++first1c, ++first2c)
    {
        const unsigned char ch = *first1c;
        *first1c = *first2c;
        *first2c = ch;
    }
}

#else

VX_NO_ALIAS void VX_CDECL swap_ranges_trivially_swappable_noalias(
    void* first1,
    void* const last1,
    void* first2) noexcept
{
    #if defined(USE_AVX2)

    constexpr size_t mask_32 = ~((static_cast<size_t>(1) << 5) - 1);

    if (byte_length(first1, last1) >= 32)
    {
        const void* stop_at = first1;
        advance_bytes(stop_at, byte_length(first1, last1) & mask_32);
        do
        {
            const __m256i left = _mm256_loadu_si256(static_cast<__m256i*>(first1));
            const __m256i right = _mm256_loadu_si256(static_cast<__m256i*>(first2));
            _mm256_storeu_si256(static_cast<__m256i*>(first1), right);
            _mm256_storeu_si256(static_cast<__m256i*>(first2), left);
            advance_bytes(first1, 32);
            advance_bytes(first2, 32);
        } while (first1 != stop_at);

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    constexpr size_t mask_16 = ~((static_cast<size_t>(1) << 4) - 1);
    if (byte_length(first1, last1) >= 16)
    {
        const void* stop_at = first1;
        advance_bytes(stop_at, byte_length(first1, last1) & mask_16);
        do
        {
            const __m128i left = _mm_loadu_si128(static_cast<__m128i*>(first1));
            const __m128i right = _mm_loadu_si128(static_cast<__m128i*>(first2));
            _mm_storeu_si128(static_cast<__m128i*>(first1), right);
            _mm_storeu_si128(static_cast<__m128i*>(first2), left);
            advance_bytes(first1, 16);
            advance_bytes(first2, 16);
        } while (first1 != stop_at);
    }

    #endif // defined(USE_SSE2)

    #if defined(VX_ARCH_WORD_BITS_64)

    constexpr size_t mask_8 = ~((static_cast<size_t>(1) << 3) - 1);
    if (byte_length(first1, last1) >= 8)
    {
        const void* stop_at = first1;
        advance_bytes(stop_at, byte_length(first1, last1) & mask_8);
        do
        {
            unsigned long long left;
            unsigned long long right;
            ::memcpy(&left, first1, 8);
            ::memcpy(&right, first2, 8);
            ::memcpy(first1, &right, 8);
            ::memcpy(first2, &left, 8);
            advance_bytes(first1, 8);
            advance_bytes(first2, 8);
        } while (first1 != stop_at);
    }

    #else // ^^^ 64-bit / 32-bit vvv

    constexpr size_t mask_4 = ~((static_cast<size_t>(1) << 2) - 1);
    if (byte_length(first1, last1) >= 4)
    {
        const void* stop_at = first1;
        advance_bytes(stop_at, byte_length(first1, last1) & mask_4);
        do
        {
            unsigned long left;
            unsigned long right;
            ::memcpy(&left, first1, 4);
            ::memcpy(&right, first2, 4);
            ::memcpy(first1, &right, 4);
            ::memcpy(first2, &left, 4);
            advance_bytes(first1, 4);
            advance_bytes(first2, 4);
        } while (first1 != stop_at);
    }

    #endif // ^^^ 32-bit ^^^

    auto first1c = static_cast<unsigned char*>(first1);
    auto first2c = static_cast<unsigned char*>(first2);
    for (; first1c != last1; ++first1c, ++first2c)
    {
        const unsigned char ch = *first1c;
        *first1c = *first2c;
        *first2c = ch;
    }
}

#endif

//=============================================================================
// rotate functions
//=============================================================================

VX_NO_ALIAS void VX_STDCALL rotate(void* first, void* const mid, void* last) noexcept
{
    unsigned char buf[_rotate::buf_size];

    for (;;)
    {
        const size_t left = byte_length(first, mid);
        const size_t right = byte_length(mid, last);

        if (left <= right)
        {
            if (left == 0)
            {
                break;
            }

            if (_rotate::use_buffer(left, right))
            {
                ::memcpy(buf, first, left);
                ::memmove(first, mid, right);
                advance_bytes(first, right);
                ::memcpy(first, buf, left);
                break;
            }

            void* mid2 = last;
            rewind_bytes(mid2, left);
            if (left * 2 > right)
            {
                swap_ranges_trivially_swappable_noalias(mid2, last, first);
                last = mid2;
            }
            else
            {
                void* mid3 = mid2;
                rewind_bytes(mid3, left);
                _rotate::swap_3_ranges(mid2, last, first, mid3);
                last = mid3;
            }
        }
        else
        {
            if (right == 0)
            {
                break;
            }

            if (_rotate::use_buffer(right, left))
            {
                rewind_bytes(last, right);
                ::memcpy(buf, last, right);
                void* mid2 = first;
                advance_bytes(mid2, right);
                ::memmove(mid2, first, left);
                ::memcpy(first, buf, right);
                break;
            }

            if (right * 2 > left)
            {
                swap_ranges_trivially_swappable_noalias(mid, last, first);
                advance_bytes(first, right);
            }
            else
            {
                void* mid2 = first;
                advance_bytes(mid2, right);
                _rotate::swap_3_ranges(mid, last, mid2, first);
                advance_bytes(first, right * 2);
            }
        }
    }
}

} // extern "C"

} // namespace _simd
} // namespace vx
