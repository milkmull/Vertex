#pragma once

#include "vertex_impl/std/simd_algorithms/simd_common.hpp"

#if defined(_VX_USE_SIMD_ALGORITHMS)

namespace vx {
namespace _simd {

//=============================================================================
// mismatch impl
//=============================================================================

#if !defined(USE_ARM_NEON)

namespace _mismatch {

template <typename T>
VX_NO_ALIAS size_t VX_STDCALL mismatch_impl(
    const void* const first1,
    const void* const first2,
    const size_t count) noexcept
{
    size_t result = 0;
    const auto first1_ch = static_cast<const char*>(first1);
    const auto first2_ch = static_cast<const char*>(first2);

#if defined(USE_AVX2)

    zeroupper_on_exit guard; // TRANSITION, DevCom-10331414

    const size_t count_bytes = count * sizeof(T);
    const size_t count_bytes_avx_full = count_bytes & ~size_t{ 0x1F };

    for (; result != count_bytes_avx_full; result += 0x20)
    {
        const __m256i elem1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first1_ch + result));
        const __m256i elem2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first2_ch + result));
        const __m256i cmp = _mm256_cmpeq_epi8(elem1, elem2);
        const auto bingo = ~static_cast<unsigned int>(_mm256_movemask_epi8(cmp));
        if (bingo != 0)
        {
            return (result + _tzcnt_u32(bingo)) / sizeof(T);
        }
    }

    const size_t count_tail = count_bytes & size_t{ 0x1C };

    if (count_tail != 0)
    {
        const __m256i tail_mask = avx2_tail_mask_32(count_tail);
        const __m256i elem1 =
            _mm256_maskload_epi32(reinterpret_cast<const int*>(first1_ch + result), tail_mask);
        const __m256i elem2 =
            _mm256_maskload_epi32(reinterpret_cast<const int*>(first2_ch + result), tail_mask);

        const __m256i cmp = _mm256_cmpeq_epi8(elem1, elem2);
        const auto bingo = ~static_cast<unsigned int>(_mm256_movemask_epi8(cmp));
        if (bingo != 0)
        {
            return (result + _tzcnt_u32(bingo)) / sizeof(T);
        }

        result += count_tail;
    }

    result /= sizeof(T);

    if constexpr (sizeof(T) >= 4)
    {
        return result;
    }

#elif defined(USE_SSE2)

    const size_t count_bytes_sse = (count * sizeof(T)) & ~size_t{ 0xF };

    for (; result != count_bytes_sse; result += 0x10)
    {
        const __m128i elem1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first1_ch + result));
        const __m128i elem2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first2_ch + result));
        const __m128i cmp = _mm_cmpeq_epi8(elem1, elem2);
        const auto bingo = static_cast<unsigned int>(_mm_movemask_epi8(cmp)) ^ 0xFFFF;
        if (bingo != 0)
        {
            unsigned long offset;
            // CodeQL [SM02313] offset is always initialized: we just tested `if (bingo != 0)`.
            _BitScanForward(&offset, bingo);
            return (result + offset) / sizeof(T);
        }
    }

    result /= sizeof(T);

#endif

    const auto first1_el = static_cast<const T*>(first1);
    const auto first2_el = static_cast<const T*>(first2);

    for (; result != count; ++result)
    {
        if (first1_el[result] != first2_el[result])
        {
            break;
        }
    }

    return result;
}

} // namespace _mismatch

//=============================================================================
// mismatch functions
//=============================================================================

extern "C" {

VX_NO_ALIAS size_t VX_STDCALL mismatch_1(
    const void* const _First1,
    const void* const _First2,
    const size_t _Count) noexcept
{
    return _mismatch::mismatch_impl<uint8_t>(_First1, _First2, _Count);
}

VX_NO_ALIAS size_t VX_STDCALL mismatch_2(
    const void* const _First1,
    const void* const _First2,
    const size_t _Count) noexcept
{
    return _mismatch::mismatch_impl<uint16_t>(_First1, _First2, _Count);
}

VX_NO_ALIAS size_t VX_STDCALL mismatch_4(
    const void* const _First1,
    const void* const _First2,
    const size_t _Count) noexcept
{
    return _mismatch::mismatch_impl<uint32_t>(_First1, _First2, _Count);
}

VX_NO_ALIAS size_t VX_STDCALL mismatch_8(
    const void* const _First1,
    const void* const _First2,
    const size_t _Count) noexcept
{
    return _mismatch::mismatch_impl<uint64_t>(_First1, _First2, _Count);
}

} // extern "C"

#endif // !defined(USE_ARM_NEON)

} // namespace _simd
} // namespace vx

#endif // _VX_USE_SIMD_ALGORITHMS
