#pragma once

#include "vertex_impl/std/simd_algorithms/simd_common.hpp"

namespace vx {
namespace _simd {

//=============================================================================
// find impl
//=============================================================================

namespace _find {

#if defined(USE_ARM_NEON)

struct find_traits_1
{
    static uint8x16_t load_q(const void* const ptr) noexcept
    {
        return vld1q_u8(static_cast<const uint8_t*>(ptr));
    }

    static uint8x8_t load(const void* const ptr) noexcept
    {
        return vld1_u8(static_cast<const uint8_t*>(ptr));
    }

    static uint8x16_t set_neon_q(const uint8_t val) noexcept
    {
        return vdupq_n_u8(val);
    }

    static uint8x8_t set_neon(const uint8_t val) noexcept
    {
        return vdup_n_u8(val);
    }

    static uint8x16_t cmp_neon_q(const uint8x16_t lhs, const uint8x16_t rhs) noexcept
    {
        return vceqq_u8(lhs, rhs);
    }

    static uint8x8_t cmp_neon(const uint8x8_t lhs, const uint8x8_t rhs) noexcept
    {
        return vceq_u8(lhs, rhs);
    }

    // Compresses a 128-bit Mask of 16 8-bit values into a 64-bit Mask of 16 4-bit values.
    static uint64_t mask_q(const uint8x16_t val) noexcept
    {
        const uint8x8_t res = vshrn_n_u16(vreinterpretq_u16_u8(val), 4);
        return vget_lane_u64(vreinterpret_u64_u8(res), 0);
    }

    static uint64_t mask(const uint8x8_t val) noexcept
    {
        return vget_lane_u64(vreinterpret_u64_u8(val), 0);
    }

    static uint64_t match_mask_eq(const uint8x16_t cmp_lo, const uint8x16_t cmp_hi) noexcept
    {
        auto cmp = vreinterpretq_u64_u8(vorrq_u8(cmp_lo, cmp_hi));
        return vgetq_lane_u64(vpaddq_u64(cmp, cmp), 0);
    }

    static uint64_t match_mask_ne(const uint8x16_t cmp_lo, const uint8x16_t cmp_hi) noexcept
    {
        auto cmp = vminq_u8(cmp_lo, cmp_hi);
        auto comb = vreinterpretq_u64_u8(vpminq_u8(cmp, cmp));
        return vgetq_lane_u64(comb, 0) ^ 0xFFFF'FFFF'FFFF'FFFF;
    }
};

struct find_traits_2
{
    static uint16x8_t load_q(const void* const ptr) noexcept
    {
        return vld1q_u16(static_cast<const uint16_t*>(ptr));
    }

    static uint16x4_t load(const void* const ptr) noexcept
    {
        return vld1_u16(static_cast<const uint16_t*>(ptr));
    }

    static uint16x8_t set_neon_q(const uint16_t val) noexcept
    {
        return vdupq_n_u16(val);
    }

    static uint16x4_t set_neon(const uint16_t val) noexcept
    {
        return vdup_n_u16(val);
    }

    static uint16x8_t cmp_neon_q(const uint16x8_t lhs, const uint16x8_t rhs) noexcept
    {
        return vceqq_u16(lhs, rhs);
    }

    static uint16x4_t cmp_neon(const uint16x4_t lhs, const uint16x4_t rhs) noexcept
    {
        return vceq_u16(lhs, rhs);
    }

    // Compresses a 128-bit Mask of 8 16-bit values into a 64-bit Mask of 8 8-bit values.
    static uint64_t mask_q(const uint16x8_t val) noexcept
    {
        const uint16x4_t res = vshrn_n_u32(vreinterpretq_u32_u16(val), 8);
        return vget_lane_u64(vreinterpret_u64_u16(res), 0);
    }

    static uint64_t mask(const uint16x4_t val) noexcept
    {
        return vget_lane_u64(vreinterpret_u64_u16(val), 0);
    }

    static uint64_t match_mask_eq(const uint16x8_t cmp_lo, const uint16x8_t cmp_hi) noexcept
    {
        uint8x8_t cmp = vaddhn_u16(cmp_lo, cmp_hi);
        return vget_lane_u64(vreinterpret_u64_u8(cmp), 0);
    }

    static uint64_t match_mask_ne(const uint16x8_t cmp_lo, const uint16x8_t cmp_hi) noexcept
    {
        auto cmp = vminq_u16(cmp_lo, cmp_hi);
        auto comb = vreinterpretq_u64_u16(vpminq_u16(cmp, cmp));
        return vgetq_lane_u64(comb, 0) ^ 0xFFFF'FFFF'FFFF'FFFF;
    }
};

struct find_traits_4
{
    static uint32x4_t load_q(const void* const ptr) noexcept
    {
        return vld1q_u32(static_cast<const uint32_t*>(ptr));
    }

    static uint32x2_t load(const void* const ptr) noexcept
    {
        return vld1_u32(static_cast<const uint32_t*>(ptr));
    }

    static uint32x4_t set_neon_q(const uint32_t val) noexcept
    {
        return vdupq_n_u32(val);
    }

    static uint32x2_t set_neon(const uint32_t val) noexcept
    {
        return vdup_n_u32(val);
    }

    static uint32x4_t cmp_neon_q(const uint32x4_t lhs, const uint32x4_t rhs) noexcept
    {
        return vceqq_u32(lhs, rhs);
    }

    static uint32x2_t cmp_neon(const uint32x2_t lhs, const uint32x2_t rhs) noexcept
    {
        return vceq_u32(lhs, rhs);
    }

    // Compresses a 128-bit Mask of 4 32-bit values into a 64-bit Mask of 4 16-bit values.
    static uint64_t mask_q(const uint32x4_t val) noexcept
    {
        const uint32x2_t res = vshrn_n_u64(vreinterpretq_u64_u32(val), 16);
        return vget_lane_u64(vreinterpret_u64_u32(res), 0);
    }

    static uint64_t mask(const uint32x2_t val) noexcept
    {
        return vget_lane_u64(vreinterpret_u64_u32(val), 0);
    }

    static uint64_t match_mask_eq(const uint32x4_t cmp_lo, const uint32x4_t cmp_hi) noexcept
    {
        uint8x8_t cmp = vaddhn_u16(vreinterpretq_u16_u32(cmp_lo), vreinterpretq_u16_u32(cmp_hi));
        return vget_lane_u64(vreinterpret_u64_u8(cmp), 0);
    }

    static uint64_t match_mask_ne(const uint32x4_t cmp_lo, const uint32x4_t cmp_hi) noexcept
    {
        auto cmp = vminq_u32(cmp_lo, cmp_hi);
        auto comb = vreinterpretq_u64_u32(vpminq_u32(cmp, cmp));
        return vgetq_lane_u64(comb, 0) ^ 0xFFFF'FFFF'FFFF'FFFF;
    }
};

struct find_traits_8
{
    static uint64x2_t load_q(const void* const ptr) noexcept
    {
        return vld1q_u64(static_cast<const uint64_t*>(ptr));
    }

    static uint64x2_t set_neon_q(const uint64_t val) noexcept
    {
        return vdupq_n_u64(val);
    }

    static uint64x2_t cmp_neon_q(const uint64x2_t lhs, const uint64x2_t rhs) noexcept
    {
        return vceqq_u64(lhs, rhs);
    }

    // Compresses a 128-bit Mask of 2 64-bit values into a 64-bit Mask of 2 32-bit values.
    static uint64_t mask_q(const uint64x2_t val) noexcept
    {
        const uint32x2_t res = vmovn_u64(val);
        return vget_lane_u64(vreinterpret_u64_u32(res), 0);
    }

    static uint64_t match_mask_eq(const uint64x2_t cmp_lo, const uint64x2_t cmp_hi) noexcept
    {
        uint8x8_t cmp = vaddhn_u16(vreinterpretq_u16_u64(cmp_lo), vreinterpretq_u16_u64(cmp_hi));
        return vget_lane_u64(vreinterpret_u64_u8(cmp), 0);
    }

    static uint64_t match_mask_ne(const uint64x2_t cmp_lo, const uint64x2_t cmp_hi) noexcept
    {
        return mask_q(vandq_u64(cmp_lo, cmp_hi)) ^ 0xFFFF'FFFF'FFFF'FFFF;
    }
};

unsigned long get_first_h_pos_q(const uint64_t mask) noexcept
{
    return countTrailingZeros64(mask) >> 2;
}

unsigned long get_first_h_pos_d(const uint64_t mask) noexcept
{
    return countTrailingZeros64(mask) >> 3;
}

#elif defined(USE_X86)

struct find_traits_1
{
    #if defined(USE_AVX2)

    static __m256i set_avx(const uint8_t val) noexcept
    {
        return _mm256_set1_epi8(val);
    }

    static __m256i cmp_avx(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_cmpeq_epi8(lhs, rhs);
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i set_sse(const uint8_t val) noexcept
    {
        return _mm_shuffle_epi8(_mm_cvtsi32_si128(val), _mm_setzero_si128());
    }

    static __m128i cmp_sse(const __m128i lhs, const __m128i rhs) noexcept
    {
        return _mm_cmpeq_epi8(lhs, rhs);
    }

    #endif // defined(USE_SSE2)
};

struct find_traits_2
{
    #if defined(USE_AVX2)

    static __m256i set_avx(const uint16_t val) noexcept
    {
        return _mm256_set1_epi16(val);
    }

    static __m256i cmp_avx(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_cmpeq_epi16(lhs, rhs);
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i set_sse(const uint16_t val) noexcept
    {
        return _mm_set1_epi16(val);
    }

    static __m128i cmp_sse(const __m128i lhs, const __m128i rhs) noexcept
    {
        return _mm_cmpeq_epi16(lhs, rhs);
    }

    #endif // defined(USE_SSE2)
};

struct find_traits_4
{
    #if defined(USE_AVX2)

    static __m256i set_avx(const uint32_t val) noexcept
    {
        return _mm256_set1_epi32(val);
    }

    static __m256i cmp_avx(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_cmpeq_epi32(lhs, rhs);
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i set_sse(const uint32_t val) noexcept
    {
        return _mm_set1_epi32(val);
    }

    static __m128i cmp_sse(const __m128i lhs, const __m128i rhs) noexcept
    {
        return _mm_cmpeq_epi32(lhs, rhs);
    }

    #endif // defined(USE_SSE2)
};

struct find_traits_8
{
    #if defined(USE_AVX2)

    static __m256i set_avx(const uint64_t val) noexcept
    {
        return _mm256_set1_epi64x(val);
    }

    static __m256i cmp_avx(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_cmpeq_epi64(lhs, rhs);
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    static __m128i set_sse(const uint64_t val) noexcept
    {
        return _mm_set1_epi64x(val);
    }

    static __m128i cmp_sse(const __m128i lhs, const __m128i rhs) noexcept
    {
        return _mm_cmpeq_epi64(lhs, rhs);
    }

    #endif // defined(USE_SSE2)
};

#endif

enum class predicate
{
    equal,
    not_equal
};

template <predicate Pred, typename T>
const void* find_scalar_tail(const void* const first, const void* const last, const T val) noexcept
{
    auto ptr = static_cast<const T*>(first);
    VX_IF_CONSTEXPR (Pred == predicate::not_equal)
    {
        while (ptr != last && *ptr == val)
        {
            ++ptr;
        }
    }
    else
    {
        while (ptr != last && *ptr != val)
        {
            ++ptr;
        }
    }
    return ptr;
}

// The below functions have exactly the same signature as the extern "C" functions, up to calling convention.
// This makes sure the template specialization can be fused with the extern "C" function.
// In optimized builds it avoids an extra call, as these functions are too large to inline.

#if defined(USE_ARM_NEON)

template <typename Traits, predicate Pred, typename T>
const void* VX_STDCALL find_impl(const void* first, const void* const last, const T val) noexcept
{
    const size_t size_bytes = byte_length(first, last);

    if (const size_t neon_size = size_bytes & ~size_t{ 0x1F }; neon_size != 0)
    {
        const auto comparand = Traits::set_neon_q(val);
        const void* stop_at = first;
        advance_bytes(stop_at, neon_size);

        do
        {
            const auto data_lo = Traits::load_q(static_cast<const uint8_t*>(first) + 0);
            const auto data_hi = Traits::load_q(static_cast<const uint8_t*>(first) + 16);

            auto comparison_lo = Traits::cmp_neon_q(data_lo, comparand);
            auto comparison_hi = Traits::cmp_neon_q(data_hi, comparand);

            // Use a fast check for the termination condition.
            uint64_t any_match = 0;
            VX_IF_CONSTEXPR (Pred == predicate::not_equal)
            {
                any_match = Traits::match_mask_ne(comparison_lo, comparison_hi);
            }
            else
            {
                any_match = Traits::match_mask_eq(comparison_lo, comparison_hi);
            }

            if (any_match != 0)
            {
                auto mask_lo = Traits::mask_q(comparison_lo);
                VX_IF_CONSTEXPR (Pred == predicate::not_equal)
                {
                    mask_lo ^= 0xFFFF'FFFF'FFFF'FFFF;
                }

                if (mask_lo != 0)
                {
                    const auto offset = get_first_h_pos_q(mask_lo);
                    advance_bytes(first, offset);
                    return first;
                }

                auto mask_hi = Traits::mask_q(comparison_hi);
                VX_IF_CONSTEXPR (Pred == predicate::not_equal)
                {
                    mask_hi ^= 0xFFFF'FFFF'FFFF'FFFF;
                }

                const auto offset = get_first_h_pos_q(mask_hi) + 16;
                advance_bytes(first, offset);
                return first;
            }

            advance_bytes(first, 32);
        } while (first != stop_at);
    }

    if ((size_bytes & size_t{ 0x10 }) != 0)
    {
        const auto comparand = Traits::set_neon_q(val);
        const auto data = Traits::load_q(first);

        auto comparison = Traits::cmp_neon_q(data, comparand);

        auto match = Traits::mask_q(comparison);
        VX_IF_CONSTEXPR (Pred == predicate::not_equal)
        {
            match ^= 0xFFFF'FFFF'FFFF'FFFF;
        }

        if (match != 0)
        {
            const auto offset = get_first_h_pos_q(match);
            advance_bytes(first, offset);
            return first;
        }

        advance_bytes(first, 16);
    }

    VX_IF_CONSTEXPR (sizeof(T) < 8)
    {
        if ((size_bytes & size_t{ 0x08 }) != 0)
        {
            const auto comparand = Traits::set_neon(val);
            const auto data = Traits::load(first);

            auto comparison = Traits::cmp_neon(data, comparand);

            auto match = Traits::mask(comparison);
            VX_IF_CONSTEXPR (Pred == predicate::not_equal)
            {
                match ^= 0xFFFF'FFFF'FFFF'FFFF;
            }

            if (match != 0)
            {
                const auto offset = get_first_h_pos_d(match);
                advance_bytes(first, offset);
                return first;
            }

            advance_bytes(first, 8);
        }
    }

    return find_scalar_tail<Pred>(first, last, val);
}

#else

template <typename Traits, predicate Pred, typename T>
const void* VX_STDCALL find_impl(const void* first, const void* const last, const T val) noexcept
{
    const size_t size_bytes = byte_length(first, last);

    #if defined(USE_AVX2)

    if (const size_t avx_size = size_bytes & ~size_t{ 0x1F }; avx_size != 0)
    {
        zeroupper_on_exit guard; // TRANSITION, DevCom-10331414

        const __m256i comparand = Traits::set_avx(val);
        const void* stop_at = first;
        advance_bytes(stop_at, avx_size);

        do
        {
            const __m256i data = _mm256_loadu_si256(static_cast<const __m256i*>(first));
            unsigned int bingo = _mm256_movemask_epi8(Traits::cmp_avx(data, comparand));

            VX_IF_CONSTEXPR (Pred == predicate::not_equal)
            {
                bingo ^= 0xFFFF'FFFF;
            }

            if (bingo != 0)
            {
                const unsigned long offset = _tzcnt_u32(bingo);
                advance_bytes(first, offset);
                return first;
            }

            advance_bytes(first, 32);
        } while (first != stop_at);

        if (const size_t avx_tail_size = size_bytes & 0x1C; avx_tail_size != 0)
        {
            const __m256i tail_mask = avx2_tail_mask_32(avx_tail_size);
            const __m256i data = _mm256_maskload_epi32(static_cast<const int*>(first), tail_mask);
            const __m256i cmp = Traits::cmp_avx(data, comparand);
            unsigned int bingo = _mm256_movemask_epi8(_mm256_and_si256(cmp, tail_mask));

            VX_IF_CONSTEXPR (Pred == predicate::not_equal)
            {
                bingo ^= (1 << avx_tail_size) - 1;
            }

            if (bingo != 0)
            {
                const unsigned long offset = _tzcnt_u32(bingo);
                advance_bytes(first, offset);
                return first;
            }

            advance_bytes(first, avx_tail_size);
        }

        VX_IF_CONSTEXPR (sizeof(T) >= 4)
        {
            return first;
        }
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    if (const size_t sse_size = size_bytes & ~size_t{ 0xF }; sse_size != 0)
    {
        const __m128i comparand = Traits::set_sse(val);
        const void* stop_at = first;
        advance_bytes(stop_at, sse_size);

        do
        {
            const __m128i data = _mm_loadu_si128(static_cast<const __m128i*>(first));
            unsigned int bingo = _mm_movemask_epi8(Traits::cmp_sse(data, comparand));

            VX_IF_CONSTEXPR (Pred == predicate::not_equal)
            {
                bingo ^= 0xFFFF;
            }

            if (bingo != 0)
            {
                unsigned long offset;
                // CodeQL [SM02313] offset is always initialized: we just tested `if (bingo != 0)`.
                _BitScanForward(&offset, bingo);
                advance_bytes(first, offset);
                return first;
            }

            advance_bytes(first, 16);
        } while (first != stop_at);
    }

    #endif // defined(USE_SSE2)

    return find_scalar_tail<Pred>(first, last, val);
}

template <typename Traits, predicate Pred, typename T>
const void* VX_STDCALL find_last_impl(const void* first, const void* last, const T val) noexcept
{
    const void* const real_last = last;
    const size_t size_bytes = byte_length(first, last);

    #if defined(USE_AVX2)

    if (const size_t avx_size = size_bytes & ~size_t{ 0x1F }; avx_size != 0)
    {
        zeroupper_on_exit guard; // TRANSITION, DevCom-10331414

        const __m256i comparand = Traits::set_avx(val);
        const void* stop_at = last;
        rewind_bytes(stop_at, avx_size);

        do
        {
            rewind_bytes(last, 32);
            const __m256i data = _mm256_loadu_si256(static_cast<const __m256i*>(last));
            unsigned int bingo = _mm256_movemask_epi8(Traits::cmp_avx(data, comparand));

            VX_IF_CONSTEXPR (Pred == predicate::not_equal)
            {
                bingo ^= 0xFFFF'FFFF;
            }

            if (bingo != 0)
            {
                const unsigned long offset = _lzcnt_u32(bingo);
                advance_bytes(last, (31 - offset) - (sizeof(T) - 1));
                return last;
            }
        } while (last != stop_at);

        if (const size_t avx_tail_size = size_bytes & 0x1C; avx_tail_size != 0)
        {
            rewind_bytes(last, avx_tail_size);
            const __m256i tail_mask = avx2_tail_mask_32(avx_tail_size);
            const __m256i data = _mm256_maskload_epi32(static_cast<const int*>(last), tail_mask);
            const __m256i cmp = Traits::cmp_avx(data, comparand);
            unsigned int bingo = _mm256_movemask_epi8(_mm256_and_si256(cmp, tail_mask));

            VX_IF_CONSTEXPR (Pred == predicate::not_equal)
            {
                bingo ^= (1 << avx_tail_size) - 1;
            }

            if (bingo != 0)
            {
                const unsigned long offset = _lzcnt_u32(bingo);
                advance_bytes(last, (31 - offset) - (sizeof(T) - 1));
                return last;
            }
        }

        VX_IF_CONSTEXPR (sizeof(T) >= 4)
        {
            return real_last;
        }
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    if (const size_t sse_size = size_bytes & ~size_t{ 0xF }; sse_size != 0)
    {
        const __m128i comparand = Traits::set_sse(val);
        const void* stop_at = last;
        rewind_bytes(stop_at, sse_size);

        do
        {
            rewind_bytes(last, 16);
            const __m128i data = _mm_loadu_si128(static_cast<const __m128i*>(last));
            unsigned int bingo = _mm_movemask_epi8(Traits::cmp_sse(data, comparand));

            VX_IF_CONSTEXPR (Pred == predicate::not_equal)
            {
                bingo ^= 0xFFFF;
            }

            if (bingo != 0)
            {
                unsigned long offset;
                // CodeQL [SM02313] offset is always initialized: we just tested `if (bingo != 0)`.
                _BitScanReverse(&offset, bingo);
                advance_bytes(last, offset - (sizeof(T) - 1));
                return last;
            }
        } while (last != stop_at);
    }

    #endif // defined(USE_SSE2)

    auto ptr = static_cast<const T*>(last);
    for (;;)
    {
        if (ptr == first)
        {
            return real_last;
        }
        --ptr;
        VX_IF_CONSTEXPR (Pred == predicate::not_equal)
        {
            if (*ptr != val)
            {
                return ptr;
            }
        }
        else
        {
            if (*ptr == val)
            {
                return ptr;
            }
        }
    }
}

template <typename Traits, predicate Pred, typename T>
size_t VX_STDCALL find_last_pos_impl(
    const void* const first,
    const void* const last,
    const T val) noexcept
{
    const void* const result = find_last_impl<Traits, Pred>(first, last, val);
    if (result == last)
    {
        return static_cast<size_t>(-1);
    }
    else
    {
        return byte_length(first, result) / sizeof(T);
    }
}

template <typename Traits, typename T>
const void* VX_STDCALL adjacent_find_impl(const void* first, const void* const last) noexcept
{
    if (first == last)
    {
        return last;
    }

    const size_t size_bytes = byte_length(first, last) - sizeof(T);

    #if defined(USE_AVX2)

    if (const size_t avx_size = size_bytes & ~size_t{ 0x1F }; avx_size != 0)
    {
        zeroupper_on_exit guard; // TRANSITION, DevCom-10331414

        const void* stop_at = first;
        advance_bytes(stop_at, avx_size);

        do
        {
            const void* next = first;
            advance_bytes(next, sizeof(T));

            const __m256i data = _mm256_loadu_si256(static_cast<const __m256i*>(first));
            const __m256i comparand = _mm256_loadu_si256(static_cast<const __m256i*>(next));
            const unsigned int bingo = _mm256_movemask_epi8(Traits::cmp_avx(data, comparand));

            if (bingo != 0)
            {
                const unsigned long offset = _tzcnt_u32(bingo);
                advance_bytes(first, offset);
                return first;
            }

            advance_bytes(first, 32);
        } while (first != stop_at);

        if (const size_t avx_tail_size = size_bytes & 0x1C; avx_tail_size != 0)
        {
            const void* next = first;
            advance_bytes(next, sizeof(T));

            const __m256i tail_mask = avx2_tail_mask_32(avx_tail_size);
            const __m256i data = _mm256_maskload_epi32(static_cast<const int*>(first), tail_mask);
            const __m256i comparand = _mm256_maskload_epi32(static_cast<const int*>(next), tail_mask);
            const __m256i cmp = Traits::cmp_avx(data, comparand);
            const unsigned int bingo = _mm256_movemask_epi8(_mm256_and_si256(cmp, tail_mask));

            if (bingo != 0)
            {
                const unsigned long offset = _tzcnt_u32(bingo);
                advance_bytes(first, offset);
                return first;
            }

            advance_bytes(first, avx_tail_size);
        }

        VX_IF_CONSTEXPR (sizeof(T) >= 4)
        {
            return last;
        }
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    if (const size_t sse_size = size_bytes & ~size_t{ 0xF }; sse_size != 0)
    {
        const void* stop_at = first;
        advance_bytes(stop_at, sse_size);

        do
        {
            const void* next = first;
            advance_bytes(next, sizeof(T));

            const __m128i data = _mm_loadu_si128(static_cast<const __m128i*>(first));
            const __m128i comparand = _mm_loadu_si128(static_cast<const __m128i*>(next));
            const unsigned int bingo = _mm_movemask_epi8(Traits::cmp_sse(data, comparand));

            if (bingo != 0)
            {
                unsigned long offset;
                // CodeQL [SM02313] offset is always initialized: we just tested `if (bingo != 0)`.
                _BitScanForward(&offset, bingo);
                advance_bytes(first, offset);
                return first;
            }

            advance_bytes(first, 16);
        } while (first != stop_at);
    }

    #endif // defined(USE_SSE2)

    auto ptr = static_cast<const T*>(first);
    auto next = ptr + 1;
    for (; next != last; ++ptr, ++next)
    {
        if (*ptr == *next)
        {
            return ptr;
        }
    }

    return last;
}

template <typename Traits, typename T>
const void* VX_STDCALL search_n_impl(
    const void* first,
    const void* const last,
    const size_t count,
    const T val) noexcept
{
    if (count == 0)
    {
        return first;
    }
    else if (count == 1)
    {
        return find_impl<Traits, predicate::equal>(first, last, val);
    }

    auto mid1 = static_cast<const T*>(first);
    const size_t length = byte_length(first, last);

    #if defined(USE_AVX2)

    if (count <= 16 / sizeof(T) && length >= 32)
    {
        zeroupper_on_exit guard; // TRANSITION, DevCom-10331414

        const int bytes_count = static_cast<int>(count * sizeof(T));
        const int sh1 = sizeof(T) != 1 ? 0 : (bytes_count < 4 ? bytes_count - 2 : 2);
        const int sh2 = sizeof(T) >= 4    ? 0
                        : bytes_count < 4 ? 0
                                          : (bytes_count < 8 ? bytes_count - 4 : 4);
        const int sh3 = sizeof(T) == 8 ? 0 : (bytes_count < 8 ? 0 : bytes_count - 8);

        const __m256i comparand = Traits::set_avx(val);

        const void* stop_at = first;
        advance_bytes(stop_at, length & ~size_t{ 0x1F });

        uint32_t carry = 0;
        do
        {
            const __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first));

            const __m256i cmp = Traits::cmp_avx(comparand, data);
            const uint32_t mask = _mm256_movemask_epi8(cmp);

            uint64_t mskx = uint64_t{ carry } | (uint64_t{ mask } << 32);

            mskx = (mskx >> sizeof(T)) & mskx;

            VX_IF_CONSTEXPR (sizeof(T) == 1)
            {
                mskx = __ull_rshift(mskx, sh1) & mskx;
            }

            VX_IF_CONSTEXPR (sizeof(T) < 4)
            {
                mskx = __ull_rshift(mskx, sh2) & mskx;
            }

            VX_IF_CONSTEXPR (sizeof(T) < 8)
            {
                mskx = __ull_rshift(mskx, sh3) & mskx;
            }

            if (mskx != 0)
            {
        #if defined(VX_ARCH_WORD_BITS_64)
                const long long shift = static_cast<long long>(_tzcnt_u64(mskx)) - 32;
        #else  // ^^^ 64-bit / 32-bit vvv
                const uint32_t mskLow = static_cast<uint32_t>(mskx);

                const int shift = mskLow != 0
                                      ? static_cast<int>(_tzcnt_u32(mskLow)) - 32
                                      : static_cast<int>(_tzcnt_u32(static_cast<uint32_t>(mskx >> 32)));
        #endif // ^^^ 32-bit ^^^
                advance_bytes(first, shift);
                return first;
            }

            carry = mask;

            advance_bytes(first, 32);
        } while (first != stop_at);

        if (const size_t tail = length & 0x1C; tail != 0)
        {
            const __m256i tail_mask = avx2_tail_mask_32(tail);
            const __m256i data = _mm256_maskload_epi32(reinterpret_cast<const int*>(first), tail_mask);

            const __m256i cmp = Traits::cmp_avx(comparand, data);
            const uint32_t mask = _mm256_movemask_epi8(_mm256_and_si256(cmp, tail_mask));

            const uint64_t msk_with_carry = uint64_t{ carry } | (uint64_t{ mask } << 32);
            uint64_t mskx = msk_with_carry;

            mskx = (mskx >> sizeof(T)) & mskx;

            VX_IF_CONSTEXPR (sizeof(T) == 1)
            {
                mskx = __ull_rshift(mskx, sh1) & mskx;
            }

            VX_IF_CONSTEXPR (sizeof(T) < 4)
            {
                mskx = __ull_rshift(mskx, sh2) & mskx;
            }

            VX_IF_CONSTEXPR (sizeof(T) < 8)
            {
                mskx = __ull_rshift(mskx, sh3) & mskx;
            }

            if (mskx != 0)
            {
        #if defined(VX_ARCH_WORD_BITS_64)
                const long long shift = static_cast<long long>(_tzcnt_u64(mskx)) - 32;
        #else  // ^^^ 64-bit / 32-bit vvv
                const uint32_t mskLow = static_cast<uint32_t>(mskx);

                const int shift = mskLow != 0
                                      ? static_cast<int>(_tzcnt_u32(mskLow)) - 32
                                      : static_cast<int>(_tzcnt_u32(static_cast<uint32_t>(mskx >> 32)));
        #endif // ^^^ 32-bit ^^^
                advance_bytes(first, shift);
                return first;
            }

            carry = static_cast<uint32_t>(__ull_rshift(msk_with_carry, static_cast<int>(tail)));

            advance_bytes(first, tail);
        }

        mid1 = static_cast<const T*>(first);
        rewind_bytes(first, _lzcnt_u32(~carry));
    }

    #endif // defined(USE_AVX2)

    #if defined(USE_SSE2)

    VX_IF_CONSTEXPR (sizeof(T) < 8)
    {
        if (count <= 8 / sizeof(T) && length >= 16)
        {
            const int bytes_count = static_cast<int>(count * sizeof(T));
            const int sh1 = sizeof(T) != 1 ? 0 : (bytes_count < 4 ? bytes_count - 2 : 2);
            const int sh2 = sizeof(T) >= 4    ? 0
                            : bytes_count < 4 ? 0
                                              : (bytes_count < 8 ? bytes_count - 4 : 4);

            const __m128i comparand = Traits::set_sse(val);

            const void* stop_at = first;
            advance_bytes(stop_at, length & ~size_t{ 0xF });

            uint32_t carry = 0;
            do
            {
                const __m128i data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first));

                const __m128i cmp = Traits::cmp_sse(comparand, data);
                const uint32_t mask = _mm_movemask_epi8(cmp);

                uint32_t mskx = carry | (mask << 16);

                mskx = (mskx >> sizeof(T)) & mskx;

                VX_IF_CONSTEXPR (sizeof(T) == 1)
                {
                    mskx = (mskx >> sh1) & mskx;
                }

                VX_IF_CONSTEXPR (sizeof(T) < 4)
                {
                    mskx = (mskx >> sh2) & mskx;
                }

                if (mskx != 0)
                {
                    unsigned long _Pos;
                    // CodeQL [SM02313] _Pos is always initialized: mskx != 0 was checked right above.
                    _BitScanForward(&_Pos, mskx);
                    advance_bytes(first, static_cast<ptrdiff_t>(_Pos) - 16);
                    return first;
                }

                carry = mask;

                advance_bytes(first, 16);
            } while (first != stop_at);

            mid1 = static_cast<const T*>(first);

            unsigned long carry_pos;
            // Here, carry can't be 0xFFFF, because that would have been a match. Therefore:
            // CodeQL [SM02313] carry_pos is always initialized: `(carry ^ 0xFFFF) != 0` is always true.
            _BitScanReverse(&carry_pos, carry ^ 0xFFFF);
            rewind_bytes(first, 15 - static_cast<ptrdiff_t>(carry_pos));
        }
    }

    #endif // defined(USE_SSE2)

    auto match_start = static_cast<const T*>(first);
    const auto last_ptr = static_cast<const T*>(last);

    if (static_cast<size_t>(last_ptr - match_start) < count)
    {
        return last_ptr;
    }

    auto match_end = match_start + count;
    auto mid2 = match_end;
    for (;;)
    {
        // Invariants: match_end - match_start == count, [match_start, mid1) and [mid2, match_end) match
        // val:
        //
        // match_start  mid1    mid2    match_end
        // |=============|????????|========|??????????...

        --mid2;
        if (*mid2 == val)
        { // match;
            if (mid1 == mid2)
            { // [mid1, mid2) is empty, so [match_start, match_end) all match
                return match_start;
            }
        }
        else
        { // mismatch; skip past it
            match_start = mid2 + 1;

            if (static_cast<size_t>(last_ptr - match_start) < count)
            { // not enough space left
                return last_ptr;
            }

            mid1 = match_end;
            match_end = match_start + count;
            mid2 = match_end;
        }
    }
}

#endif

} // namespace _find

//=============================================================================
// find functions
//=============================================================================

extern "C" {

const void* VX_STDCALL find_trivial_1(
    const void* const first,
    const void* const last,
    const uint8_t val) noexcept
{
#if defined(USE_X86)

    return _find::find_impl<_find::find_traits_1, _find::predicate::equal>(first, last, val);

#else

    auto result = ::memchr(first, val, byte_length(first, last));
    return result ? result : last;

#endif // defined(USE_X86)
}

const void* VX_STDCALL find_trivial_2(
    const void* const first,
    const void* const last,
    const uint16_t val) noexcept
{
#if defined(USE_X86)

    return _find::find_impl<_find::find_traits_2, _find::predicate::equal>(first, last, val);

#else

    auto result = ::wmemchr(static_cast<const wchar_t*>(first), val, byte_length(first, last) / sizeof(wchar_t));
    return result ? result : last;

#endif // defined(USE_X86)
}

const void* VX_STDCALL find_trivial_4(
    const void* const first,
    const void* const last,
    const uint32_t val) noexcept
{
    return _find::find_impl<_find::find_traits_4, _find::predicate::equal>(first, last, val);
}

const void* VX_STDCALL find_trivial_8(
    const void* const first,
    const void* const last,
    const uint64_t val) noexcept
{
    return _find::find_impl<_find::find_traits_8, _find::predicate::equal>(first, last, val);
}

#if defined(USE_X86)

const void* VX_STDCALL find_last_trivial_1(
    const void* const first,
    const void* const last,
    const uint8_t val) noexcept
{
    return _find::find_last_impl<_find::find_traits_1, _find::predicate::equal>(first, last, val);
}

const void* VX_STDCALL find_last_trivial_2(
    const void* const first,
    const void* const last,
    const uint16_t val) noexcept
{
    return _find::find_last_impl<_find::find_traits_2, _find::predicate::equal>(first, last, val);
}

const void* VX_STDCALL find_last_trivial_4(
    const void* const first,
    const void* const last,
    const uint32_t val) noexcept
{
    return _find::find_last_impl<_find::find_traits_4, _find::predicate::equal>(first, last, val);
}

const void* VX_STDCALL find_last_trivial_8(
    const void* const first,
    const void* const last,
    const uint64_t val) noexcept
{
    return _find::find_last_impl<_find::find_traits_8, _find::predicate::equal>(first, last, val);
}

#endif // defined(USE_X86)

const void* VX_STDCALL find_not_ch_1(
    const void* const first,
    const void* const last,
    const uint8_t val) noexcept
{
    return _find::find_impl<_find::find_traits_1, _find::predicate::not_equal>(first, last, val);
}

const void* VX_STDCALL find_not_ch_2(
    const void* const first,
    const void* const last,
    const uint16_t val) noexcept
{
    return _find::find_impl<_find::find_traits_2, _find::predicate::not_equal>(first, last, val);
}

const void* VX_STDCALL find_not_ch_4(
    const void* const first,
    const void* const last,
    const uint32_t val) noexcept
{
    return _find::find_impl<_find::find_traits_4, _find::predicate::not_equal>(first, last, val);
}

const void* VX_STDCALL find_not_ch_8(
    const void* const first,
    const void* const last,
    const uint64_t val) noexcept
{
    return _find::find_impl<_find::find_traits_8, _find::predicate::not_equal>(first, last, val);
}

#if defined(USE_X86)

VX_NO_ALIAS size_t VX_STDCALL find_last_not_ch_pos_1(
    const void* const first,
    const void* const last,
    const uint8_t val) noexcept
{
    return _find::find_last_pos_impl<_find::find_traits_1, _find::predicate::not_equal>(
        first, last, val);
}

VX_NO_ALIAS size_t VX_STDCALL find_last_not_ch_pos_2(
    const void* const first,
    const void* const last,
    const uint16_t val) noexcept
{
    return _find::find_last_pos_impl<_find::find_traits_2, _find::predicate::not_equal>(
        first, last, val);
}

VX_NO_ALIAS size_t VX_STDCALL find_last_not_ch_pos_4(
    const void* const first,
    const void* const last,
    const uint32_t val) noexcept
{
    return _find::find_last_pos_impl<_find::find_traits_4, _find::predicate::not_equal>(
        first, last, val);
}

VX_NO_ALIAS size_t VX_STDCALL find_last_not_ch_pos_8(
    const void* const first,
    const void* const last,
    const uint64_t val) noexcept
{
    return _find::find_last_pos_impl<_find::find_traits_8, _find::predicate::not_equal>(
        first, last, val);
}

const void* VX_STDCALL adjacent_find_1(const void* const first, const void* const last) noexcept
{
    return _find::adjacent_find_impl<_find::find_traits_1, uint8_t>(first, last);
}

const void* VX_STDCALL adjacent_find_2(const void* const first, const void* const last) noexcept
{
    return _find::adjacent_find_impl<_find::find_traits_2, uint16_t>(first, last);
}

const void* VX_STDCALL adjacent_find_4(const void* const first, const void* const last) noexcept
{
    return _find::adjacent_find_impl<_find::find_traits_4, uint32_t>(first, last);
}

const void* VX_STDCALL adjacent_find_8(const void* const first, const void* const last) noexcept
{
    return _find::adjacent_find_impl<_find::find_traits_8, uint64_t>(first, last);
}

const void* VX_STDCALL search_n_1(
    const void* const first,
    const void* const last,
    const size_t count,
    const uint8_t value) noexcept
{
    return _find::search_n_impl<_find::find_traits_1>(first, last, count, value);
}

const void* VX_STDCALL search_n_2(
    const void* const first,
    const void* const last,
    const size_t count,
    const uint16_t value) noexcept
{
    return _find::search_n_impl<_find::find_traits_2>(first, last, count, value);
}

const void* VX_STDCALL search_n_4(
    const void* const first,
    const void* const last,
    const size_t count,
    const uint32_t value) noexcept
{
    return _find::search_n_impl<_find::find_traits_4>(first, last, count, value);
}

const void* VX_STDCALL search_n_8(
    const void* const first,
    const void* const last,
    const size_t count,
    const uint64_t value) noexcept
{
    return _find::search_n_impl<_find::find_traits_8>(first, last, count, value);
}

#endif // defined(USE_X86)

} // extern "C"

} // namespace _simd
} // namespace vx
