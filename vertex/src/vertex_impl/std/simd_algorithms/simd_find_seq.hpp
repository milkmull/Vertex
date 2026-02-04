#pragma once

#include "vertex_impl/std/simd_algorithms/simd_find.hpp"

#if defined(_VX_USE_SIMD_ALGORITHMS)

namespace vx {
namespace _simd {

//=============================================================================
// find seq impl
//=============================================================================

#if !defined(USE_ARM_NEON)

namespace _find_seq {

#if defined(USE_AVX2)

struct find_seq_traits_avx
{
    using guard = zeroupper_on_exit;

    static constexpr size_t vec_size = 32;

    static __m256i mask(const size_t count_in_bytes) noexcept
    {
        return _Avx2_tail_mask_32(count_in_bytes);
    }

    static __m256i load(const void* const src) noexcept
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
    }

    static __m256i xor_(const __m256i val1, const __m256i val2) noexcept
    {
        return _mm256_xor_si256(val1, val2);
    }

    static bool test_z(const __m256i val1, const __m256i val2) noexcept
    {
        return _mm256_testz_si256(val1, val2);
    }

    static unsigned int bsf(const unsigned long mask) noexcept
    {
        return _tzcnt_u32(mask);
    }

    static unsigned int bsr(const unsigned long mask) noexcept
    {
        return 31 - _lzcnt_u32(mask);
    }
};

struct find_seq_traits_avx_1_2 : find_seq_traits_avx
{
    static __m256i load_tail(
        const void* const src,
        const size_t size_bytes,
        __m256i = _mm256_undefined_si256()) noexcept
    {
        unsigned char tmp[32];
        ::memcpy(tmp, src, size_bytes);
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(tmp));
    }
};

struct find_seq_traits_avx_4_8 : find_seq_traits_avx
{
    static __m256i load_tail(const void* const src, size_t, const __m256i mask) noexcept
    {
        return _mm256_maskload_epi32(reinterpret_cast<const int*>(src), mask);
    }

    static __m256i load_tail(const void* const src, const size_t size_bytes) noexcept
    {
        const __m256i mask = _Avx2_tail_mask_32(size_bytes);
        return _mm256_maskload_epi32(reinterpret_cast<const int*>(src), mask);
    }
};

struct find_seq_traits_avx_1 : find_seq_traits_avx_1_2
{
    static __m256i broadcast(const __m256i data) noexcept
    {
        return _mm256_broadcastb_epi8(_mm256_castsi256_si128(data));
    }

    static unsigned long cmp(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_movemask_epi8(_mm256_cmpeq_epi8(lhs, rhs));
    }
};

struct find_seq_traits_avx_2 : find_seq_traits_avx_1_2
{
    static __m256i broadcast(const __m256i data) noexcept
    {
        return _mm256_broadcastw_epi16(_mm256_castsi256_si128(data));
    }

    static unsigned long cmp(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_movemask_epi8(_mm256_cmpeq_epi16(lhs, rhs)) & 0x55555555;
    }
};

struct find_seq_traits_avx_4 : find_seq_traits_avx_4_8
{
    static __m256i broadcast(const __m256i data) noexcept
    {
        return _mm256_broadcastd_epi32(_mm256_castsi256_si128(data));
    }

    static unsigned long cmp(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_movemask_epi8(_mm256_cmpeq_epi32(lhs, rhs)) & 0x11111111;
    }
};

struct find_seq_traits_avx_8 : find_seq_traits_avx_4_8
{
    static __m256i broadcast(const __m256i data) noexcept
    {
        return _mm256_broadcastq_epi64(_mm256_castsi256_si128(data));
    }

    static unsigned long cmp(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_movemask_epi8(_mm256_cmpeq_epi64(lhs, rhs)) & 0x01010101;
    }
};

#else

using find_seq_traits_avx_1 = void;
using find_seq_traits_avx_2 = void;
using find_seq_traits_avx_4 = void;
using find_seq_traits_avx_8 = void;

#endif // defined(USE_AVX2)

#if defined(USE_SSE2)

struct find_seq_traits_sse_4_8
{
    using guard = char;

    static constexpr size_t vec_size = 16;

    static __m128i mask(const size_t count_in_bytes) noexcept
    {
        // count_in_bytes must be within [0, 16].
        static constexpr unsigned int tail_masks[8] = { ~0u, ~0u, ~0u, ~0u, 0, 0, 0, 0 };
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(
            reinterpret_cast<const unsigned char*>(tail_masks) + (16 - count_in_bytes)));
    }

    static __m128i load(const void* const src) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
    }

    static __m128i xor_(const __m128i val1, const __m128i val2) noexcept
    {
        return _mm_xor_si128(val1, val2);
    }

    static bool test_z(const __m128i val1, const __m128i val2) noexcept
    {
        return _mm_testz_si128(val1, val2);
    }

    static __m128i load_tail(
        const void* const src,
        const size_t size_bytes,
        __m128i = _mm_undefined_si128()) noexcept
    {
        unsigned char tmp[16];
        ::memcpy(tmp, src, size_bytes);
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(tmp));
    }

    static unsigned int bsf(const unsigned long mask) noexcept
    {
        unsigned long _Index;
        // CodeQL [SM02313] _Index is always initialized: we checked mask != 0 on every call site
        _BitScanForward(&_Index, mask);
        return _Index;
    }

    static unsigned int bsr(const unsigned long mask) noexcept
    {
        unsigned long _Index;
        // CodeQL [SM02313] _Index is always initialized: we checked mask != 0 on every call site
        _BitScanReverse(&_Index, mask);
        return _Index;
    }
};

struct find_seq_traits_sse_4 : find_seq_traits_sse_4_8
{
    static __m128i broadcast(const __m128i data) noexcept
    {
        return _mm_shuffle_epi32(data, _MM_SHUFFLE(0, 0, 0, 0));
    }

    static unsigned long cmp(const __m128i lhs, const __m128i rhs) noexcept
    {
        return _mm_movemask_epi8(_mm_cmpeq_epi32(lhs, rhs)) & 0x1111;
    }
};

struct find_seq_traits_sse_8 : find_seq_traits_sse_4_8
{
    static __m128i broadcast(const __m128i data) noexcept
    {
        return _mm_shuffle_epi32(data, _MM_SHUFFLE(1, 0, 1, 0));
    }

    static unsigned long cmp(const __m128i lhs, const __m128i rhs) noexcept
    {
        return _mm_movemask_epi8(_mm_cmpeq_epi64(lhs, rhs)) & 0x0101;
    }
};

#else

using find_seq_traits_sse_1 = void;
using find_seq_traits_sse_2 = void;
using find_seq_traits_sse_4 = void;
using find_seq_traits_sse_8 = void;

#endif // defined(USE_SSE2)

template <typename Traits, typename T>
const void* search_cmpeq(const void* first1, const void* const last1, const void* const first2, const size_t size_bytes_2) noexcept
{
    VX_MAYBE_UNUSED typename Traits::guard guard; // TRANSITION, DevCom-10331414
    const size_t size_bytes_1 = byte_length(first1, last1);
    constexpr size_t vec_size = Traits::vec_size;
    constexpr size_t vec_mask = vec_size - 1;

    if (size_bytes_2 <= vec_size)
    {
        const auto mask2 = Traits::mask(size_bytes_2);
        const auto data2 = Traits::load_tail(first2, size_bytes_2, mask2);
        const auto start2 = Traits::broadcast(data2);

        const void* stop1 = first1;
        advance_bytes(stop1, size_bytes_1 & ~vec_mask);
        do
        {
            const auto data1 = Traits::load(first1);
            unsigned long bingo = Traits::cmp(data1, start2);

            while (bingo != 0)
            {
                const unsigned int pos = Traits::bsf(bingo);

                const void* match = first1;
                advance_bytes(match, pos);

                decltype(Traits::load(match)) cmp;
                if (const size_t left_match = byte_length(match, last1); left_match >= vec_size)
                {
                    const auto match_val = Traits::load(match);
                    cmp = Traits::xor_(data2, match_val);
                }
                else if (left_match >= size_bytes_2)
                {
                    const auto match_val = Traits::load_tail(match, left_match);
                    cmp = Traits::xor_(data2, match_val);
                }
                else
                {
                    break;
                }

                if (Traits::test_z(cmp, mask2))
                {
                    return match;
                }

                bingo ^= 1 << pos;
            }

            advance_bytes(first1, vec_size);
        } while (first1 != stop1);

        if (const size_t left1 = byte_length(first1, last1); left1 >= size_bytes_2)
        {
            const auto data1 = Traits::load_tail(first1, left1);
            unsigned long bingo = Traits::cmp(data1, start2);

            while (bingo != 0)
            {
                const unsigned int pos = Traits::bsf(bingo);

                if (pos > left1 - size_bytes_2)
                {
                    break;
                }

                const void* match = first1;
                advance_bytes(match, pos);

                const size_t left_match = byte_length(match, last1);
                const auto match_val = Traits::load_tail(match, left_match);
                const auto cmp = Traits::xor_(data2, match_val);

                if (Traits::test_z(cmp, mask2))
                {
                    return match;
                }

                bingo ^= 1 << pos;
            }
        }

        return last1;
    }
    else
    { // size_bytes_2 is greater than vec_size bytes
        const auto data2 = Traits::load(first2);
        const auto start2 = Traits::broadcast(data2);

        const size_t max_pos = size_bytes_1 - size_bytes_2;

        const void* stop1 = first1;
        advance_bytes(stop1, max_pos);

        const void* tail2 = first2;
        advance_bytes(tail2, vec_size);

        do
        {
            const auto data1 = Traits::load(first1);
            unsigned long bingo = Traits::cmp(data1, start2);

            while (bingo != 0)
            {
                const unsigned int pos = Traits::bsf(bingo);

                const void* match = first1;
                advance_bytes(match, pos);

                if (match > stop1)
                {
                    break; // Oops, doesn't fit
                }

                const auto match_val = Traits::load(match);
                const auto cmp = Traits::xor_(data2, match_val);

                if (Traits::test_z(cmp, cmp))
                {
                    const void* tail1 = match;
                    advance_bytes(tail1, vec_size);

                    if (::memcmp(tail1, tail2, size_bytes_2 - vec_size) == 0)
                    {
                        return match;
                    }
                }

                bingo ^= 1 << pos;
            }

            advance_bytes(first1, vec_size);
        } while (first1 <= stop1);

        return last1;
    }
}

template <typename Traits, typename T>
const void* find_end_cmpeq(const void* const first1, const void* const last1, const void* const first2, const size_t size_bytes_2) noexcept
{
    VX_MAYBE_UNUSED typename Traits::guard guard; // TRANSITION, DevCom-10331414
    const size_t size_bytes_1 = byte_length(first1, last1);
    constexpr size_t vec_size = Traits::vec_size;
    constexpr size_t vec_mask = vec_size - 1;

    if (size_bytes_2 <= vec_size)
    {
        const unsigned int needle_fit_mask = (1 << (vec_size - size_bytes_2 + sizeof(T))) - 1;

        const void* stop1 = first1;
        advance_bytes(stop1, size_bytes_1 & vec_mask);

        const auto mask2 = Traits::mask(size_bytes_2);
        const auto data2 = Traits::load_tail(first2, size_bytes_2, mask2);
        const auto start2 = Traits::broadcast(data2);

        const void* mid1 = last1;
        rewind_bytes(mid1, vec_size);

        // structure was padded due to alignment specifier
        VX_DISABLE_MSVC_WARNING_PUSH()
        VX_DISABLE_MSVC_WARNING(4324)

        const auto check_first = [=, &mid1](unsigned long match) noexcept
        {
            while (match != 0)
            {
                const unsigned int pos = Traits::bsr(match);

                const void* tmp1 = mid1;
                advance_bytes(tmp1, pos);

                const auto match_data = Traits::load_tail(tmp1, byte_length(tmp1, last1));
                const auto cmp_result = Traits::xor_(data2, match_data);

                if (Traits::test_z(cmp_result, mask2))
                {
                    mid1 = tmp1;
                    return true;
                }

                match ^= 1 << pos;
            }

            return false;
        };

        const auto check = [=, &mid1](unsigned long match) noexcept
        {
            while (match != 0)
            {
                const unsigned int pos = Traits::bsr(match);

                const void* tmp1 = mid1;
                advance_bytes(tmp1, pos);

                const auto match_data = Traits::load(tmp1);
                const auto cmp_result = Traits::xor_(data2, match_data);

                if (Traits::test_z(cmp_result, mask2))
                {
                    mid1 = tmp1;
                    return true;
                }

                match ^= 1 << pos;
            }

            return false;
        };

        VX_DISABLE_MSVC_WARNING_POP()

        // The very last part, for any match needle should fit, otherwise false match
        const auto data1_last = Traits::load(mid1);
        const unsigned long match_last_val = Traits::cmp(data1_last, start2);
        if (check_first(match_last_val & needle_fit_mask))
        {
            return mid1;
        }

        // The middle part, fit and unfit needle
        while (mid1 != stop1)
        {
            rewind_bytes(mid1, vec_size);
            const auto data1 = Traits::load(mid1);
            const unsigned long match_val = Traits::cmp(data1, start2);
            if (check(match_val))
            {
                return mid1;
            }
        }

        // The first part, fit and unfit needle, mask out already processed positions
        if (const size_t tail_bytes_1 = size_bytes_1 & vec_mask; tail_bytes_1 != 0)
        {
            mid1 = first1;
            const auto data1 = Traits::load(mid1);
            const unsigned long match_val = Traits::cmp(data1, start2);
            if (match_val != 0 && check(match_val & ((1 << tail_bytes_1) - 1)))
            {
                return mid1;
            }
        }

        return last1;
    }
    else
    { // size_bytes_2 is greater than vec_size bytes
        const auto data2 = Traits::load(first2);
        const auto start2 = Traits::broadcast(data2);

        const void* tail2 = first2;
        advance_bytes(tail2, vec_size);

        const void* mid1 = last1;
        rewind_bytes(mid1, size_bytes_2);

        const size_t size_diff_bytes = size_bytes_1 - size_bytes_2;
        const void* stop1 = first1;
        advance_bytes(stop1, size_diff_bytes & vec_mask);

        // structure was padded due to alignment specifier
        VX_DISABLE_MSVC_WARNING_PUSH()
        VX_DISABLE_MSVC_WARNING(4324)

        const auto check = [=, &mid1](unsigned long match) noexcept
        {
            while (match != 0)
            {
                const unsigned int pos = Traits::bsr(match);

                const void* tmp1 = mid1;
                advance_bytes(tmp1, pos);

                const auto match_data = Traits::load(tmp1);
                const auto cmp_result = Traits::xor_(data2, match_data);

                if (Traits::test_z(cmp_result, cmp_result))
                {
                    const void* tail1 = tmp1;
                    advance_bytes(tail1, vec_size);

                    if (::memcmp(tail1, tail2, size_bytes_2 - vec_size) == 0)
                    {
                        mid1 = tmp1;
                        return true;
                    }
                }

                match ^= 1 << pos;
            }

            return false;
        };

        VX_DISABLE_MSVC_WARNING_POP()

        // The very last part, just compare, as true match must start with first symbol
        const auto data1_last = Traits::load(mid1);
        const auto match_last = Traits::xor_(data2, data1_last);

        if (Traits::test_z(match_last, match_last))
        {
            // Matched vec_size bytes, check the rest
            const void* tail1 = mid1;
            advance_bytes(tail1, vec_size);

            if (::memcmp(tail1, tail2, size_bytes_2 - vec_size) == 0)
            {
                return mid1;
            }
        }

        // The main part, match all characters
        while (mid1 != stop1)
        {
            rewind_bytes(mid1, vec_size);

            const auto data1 = Traits::load(mid1);
            const unsigned long match_val = Traits::cmp(data1, start2);
            if (check(match_val))
            {
                return mid1;
            }
        }

        // The first part, mask out already processed positions
        if (const size_t tail_bytes_1 = size_diff_bytes & vec_mask; tail_bytes_1 != 0)
        {
            mid1 = first1;
            const auto data1 = Traits::load(mid1);
            const unsigned long match_val = Traits::cmp(data1, start2);
            if (match_val != 0 && check(match_val & ((1 << tail_bytes_1) - 1)))
            {
                return mid1;
            }
        }

        return last1;
    }
}

template <typename Traits, typename Traits_avx, typename Traits_sse, typename T>
const void* VX_STDCALL search_impl(
    const void* first1,
    const void* const last1,
    const void* const first2,
    const size_t count2) noexcept
{
    if (count2 == 0)
    {
        return first1;
    }

    if (count2 == 1)
    {
        return _find::find_impl<Traits, _find::predicate::equal>(
            first1,
            last1,
            *static_cast<const T*>(first2));
    }

    const size_t size_bytes_1 = byte_length(first1, last1);
    const size_t size_bytes_2 = count2 * sizeof(T);

    if (size_bytes_1 < size_bytes_2)
    {
        return last1;
    }

    // The AVX2 path for 8-bit elements is not necessarily more efficient than the SSE4.2 cmpestri path
    VX_IF_CONSTEXPR (sizeof(T) != 1)
    {
#if defined(USE_AVX2)
        if (size_bytes_1 >= 32)
        {
            return search_cmpeq<Traits_avx, T>(first1, last1, first2, size_bytes_2);
        }
#endif // defined(USE_AVX2)
    }

#if defined(USE_SSE2)

    if (size_bytes_1 >= 16)
    {
        VX_IF_CONSTEXPR (sizeof(T) >= 4)
        {
            return search_cmpeq<Traits_sse, T>(first1, last1, first2, size_bytes_2);
        }
        else
        {
            constexpr int op =
                (sizeof(T) == 1 ? _SIDD_UBYTE_OPS : _SIDD_UWORD_OPS) | _SIDD_CMP_EQUAL_ORDERED;
            constexpr int part_size_el = sizeof(T) == 1 ? 16 : 8;

            if (size_bytes_2 <= 16)
            {
                const int size_el_2 = static_cast<int>(size_bytes_2 / sizeof(T));

                const int max_full_match_pos = part_size_el - size_el_2;

                alignas(16) uint8_t tmp2[16];
                ::memcpy(tmp2, first2, size_bytes_2);
                const __m128i data2 = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp2));

                const void* stop1 = first1;
                advance_bytes(stop1, size_bytes_1 - 16);

                do
                {
                    const __m128i data1 = _mm_loadu_si128(static_cast<const __m128i*>(first1));

                    if (!_mm_cmpestrc(data2, size_el_2, data1, part_size_el, op))
                    {
                        advance_bytes(first1, 16); // No matches, next.
                    }
                    else
                    {
                        const int pos = _mm_cmpestri(data2, size_el_2, data1, part_size_el, op);
                        advance_bytes(first1, pos * sizeof(T));
                        if (pos <= max_full_match_pos)
                        {
                            // Full match. Return this match.
                            return first1;
                        }
                        // Partial match. Search again from the match start. Will return it if it is full.
                    }
                } while (first1 <= stop1);

                const size_t size_bytes_1_tail = byte_length(first1, last1);
                if (size_bytes_1_tail != 0)
                {
                    const int size_el_1_tail = static_cast<int>(size_bytes_1_tail / sizeof(T));

                    alignas(16) uint8_t tmp1[16];
                    ::memcpy(tmp1, first1, size_bytes_1_tail);
                    const __m128i data1 = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp1));

                    if (_mm_cmpestrc(data2, size_el_2, data1, size_el_1_tail, op))
                    {
                        const int pos = _mm_cmpestri(data2, size_el_2, data1, size_el_1_tail, op);
                        advance_bytes(first1, pos * sizeof(T));
                        // Full match because size is less than 16. Return this match.
                        return first1;
                    }
                }

                return last1;
            }
            else
            { // size_bytes_2 is greater than 16 bytes
                const __m128i data2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first2));
                const size_t max_pos = size_bytes_1 - size_bytes_2;

                const void* stop1 = first1;
                advance_bytes(stop1, max_pos);

                const void* tail2 = first2;
                advance_bytes(tail2, 16);

                do
                {
                    const __m128i data1 = _mm_loadu_si128(static_cast<const __m128i*>(first1));
                    if (!_mm_cmpestrc(data2, part_size_el, data1, part_size_el, op))
                    {
                        advance_bytes(first1, 16); // No matches, next.
                    }
                    else
                    {
                        const int pos = _mm_cmpestri(data2, part_size_el, data1, part_size_el, op);

                        bool match_1st_16 = true;

                        if (pos != 0)
                        {
                            advance_bytes(first1, pos * sizeof(T));

                            if (first1 > stop1)
                            {
                                break; // Oops, doesn't fit
                            }

                            // Match not from the first byte, check 16 symbols
                            const __m128i match1 = _mm_loadu_si128(static_cast<const __m128i*>(first1));
                            const __m128i cmp = _mm_xor_si128(data2, match1);
                            if (!_mm_testz_si128(cmp, cmp))
                            {
                                match_1st_16 = false;
                            }
                        }

                        if (match_1st_16)
                        {
                            const void* tail1 = first1;
                            advance_bytes(tail1, 16);

                            if (::memcmp(tail1, tail2, size_bytes_2 - 16) == 0)
                            {
                                return first1;
                            }
                        }

                        // Start from the next element
                        advance_bytes(first1, sizeof(T));
                    }
                } while (first1 <= stop1);

                return last1;
            }
        }
    }

#endif // defined(USE_SSE2)

    const size_t max_pos = size_bytes_1 - size_bytes_2 + sizeof(T);

    auto ptr1 = static_cast<const T*>(first1);
    const auto ptr2 = static_cast<const T*>(first2);
    const void* stop1 = ptr1;
    advance_bytes(stop1, max_pos);

    for (; ptr1 != stop1; ++ptr1)
    {
        if (*ptr1 != *ptr2)
        {
            continue;
        }

        bool equal = true;

        for (size_t idx = 1; idx != count2; ++idx)
        {
            if (ptr1[idx] != ptr2[idx])
            {
                equal = false;
                break;
            }
        }

        if (equal)
        {
            return ptr1;
        }
    }

    return last1;
}

template <typename Traits, typename Traits_avx, typename Traits_sse, typename T>
const void* VX_STDCALL find_end_impl(const void* const first1, const void* const last1, const void* const first2, const size_t count2) noexcept
{
    if (count2 == 0)
    {
        return last1;
    }

    if (count2 == 1)
    {
        return _find::find_last_impl<Traits, _find::predicate::equal>(
            first1,
            last1,
            *static_cast<const T*>(first2));
    }

    const size_t size_bytes_1 = byte_length(first1, last1);
    const size_t size_bytes_2 = count2 * sizeof(T);

    if (size_bytes_1 < size_bytes_2)
    {
        return last1;
    }

#if defined(USE_AVX2)

    if (size_bytes_1 >= 32)
    {
        return find_end_cmpeq<Traits_avx, T>(first1, last1, first2, size_bytes_2);
    }

#endif // defined(USE_AVX2)

#if defined(USE_SSE2)

    if (size_bytes_1 >= 16)
    {
        VX_IF_CONSTEXPR (sizeof(T) >= 4)
        {
            return find_end_cmpeq<Traits_sse, T>(first1, last1, first2, size_bytes_2);
        }
        else
        {
            constexpr int op =
                (sizeof(T) == 1 ? _SIDD_UBYTE_OPS : _SIDD_UWORD_OPS) | _SIDD_CMP_EQUAL_ORDERED;
            constexpr int part_size_el = sizeof(T) == 1 ? 16 : 8;

            static constexpr int8_t low_part_mask[] = {                         //
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };

            if (size_bytes_2 <= 16)
            {
                const int size_el_2 = static_cast<int>(count2);
                constexpr unsigned int whole_mask = (1 << part_size_el) - 1;
                const unsigned int needle_fit_mask = (1 << (part_size_el - size_el_2 + 1)) - 1;
                const unsigned int needle_unfit_mask = whole_mask ^ needle_fit_mask;

                const void* stop1 = first1;
                advance_bytes(stop1, size_bytes_1 & 0xF);

                alignas(16) uint8_t tmp2[16];
                ::memcpy(tmp2, first2, size_bytes_2);
                const __m128i data2 = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp2));

                const void* mid1 = last1;
                rewind_bytes(mid1, 16);

                const auto check_fit = [&mid1, needle_fit_mask](const unsigned int match) noexcept
                {
                    const unsigned int fit_match = match & needle_fit_mask;
                    if (fit_match != 0)
                    {
                        unsigned long match_last_pos;

                        // CodeQL [SM02313] Result is always initialized: we just tested that fit_match != 0.
                        _BitScanReverse(&match_last_pos, fit_match);

                        advance_bytes(mid1, match_last_pos * sizeof(T));
                        return true;
                    }

                    return false;
                };

                // structure was padded due to alignment specifier
                VX_DISABLE_MSVC_WARNING_PUSH()
                VX_DISABLE_MSVC_WARNING(4324)

                const auto check_unfit = [=, &mid1](const unsigned int match) noexcept
                {
                    unsigned long unfit_match = match & needle_unfit_mask;
                    while (unfit_match != 0)
                    {
                        const void* tmp1 = mid1;
                        unsigned long match_last_pos;

                        // CodeQL [SM02313] Result is always initialized: we just tested that unfit_match != 0.
                        _BitScanReverse(&match_last_pos, unfit_match);

                        advance_bytes(tmp1, match_last_pos * sizeof(T));

                        const __m128i match_data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(tmp1));
                        const __m128i cmp_result = _mm_xor_si128(data2, match_data);
                        const __m128i data_mask = _mm_loadu_si128(
                            reinterpret_cast<const __m128i*>(low_part_mask + 16 - size_bytes_2));

                        if (_mm_testz_si128(cmp_result, data_mask))
                        {
                            mid1 = tmp1;
                            return true;
                        }

                        unfit_match ^= 1 << match_last_pos;
                    }

                    return false;
                };

                VX_DISABLE_MSVC_WARNING_POP()

                // TRANSITION, DevCom-10689455, the code below could test with _mm_cmpestrc,
                // if it has been fused with _mm_cmpestrm.

                // The very last part, for any match needle should fit, otherwise false match
                const __m128i data1_last = _mm_loadu_si128(reinterpret_cast<const __m128i*>(mid1));
                const auto match_last = _mm_cmpestrm(data2, size_el_2, data1_last, part_size_el, op);
                const unsigned int match_last_val = _mm_cvtsi128_si32(match_last);
                if (check_fit(match_last_val))
                {
                    return mid1;
                }

                // The middle part, fit and unfit needle
                while (mid1 != stop1)
                {
                    rewind_bytes(mid1, 16);
                    const __m128i data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(mid1));
                    const auto match = _mm_cmpestrm(data2, size_el_2, data1, part_size_el, op);
                    const unsigned int match_val = _mm_cvtsi128_si32(match);
                    if (match_val != 0 && (check_unfit(match_val) || check_fit(match_val)))
                    {
                        return mid1;
                    }
                }

                // The first part, fit and unfit needle, mask out already processed positions
                if (const size_t tail_bytes_1 = size_bytes_1 & 0xF; tail_bytes_1 != 0)
                {
                    mid1 = first1;
                    const __m128i data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(mid1));
                    const auto match = _mm_cmpestrm(data2, size_el_2, data1, part_size_el, op);
                    const size_t tail_el_1 = tail_bytes_1 / sizeof(T);
                    const unsigned int match_val = _mm_cvtsi128_si32(match) & ((1 << tail_el_1) - 1);
                    if (match_val != 0 && (check_unfit(match_val) || check_fit(match_val)))
                    {
                        return mid1;
                    }
                }

                return last1;
            }
            else
            { // size_bytes_2 is greater than 16 bytes
                const __m128i data2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first2));

                const void* tail2 = first2;
                advance_bytes(tail2, 16);

                const void* mid1 = last1;
                rewind_bytes(mid1, size_bytes_2);

                const size_t size_diff_bytes = size_bytes_1 - size_bytes_2;
                const void* stop1 = first1;
                advance_bytes(stop1, size_diff_bytes & 0xF);

                // structure was padded due to alignment specifier
                VX_DISABLE_MSVC_WARNING_PUSH()
                VX_DISABLE_MSVC_WARNING(4324)

                const auto check = [=, &mid1](unsigned long match) noexcept
                {
                    while (match != 0)
                    {
                        const void* tmp1 = mid1;
                        unsigned long match_last_pos;

                        // CodeQL [SM02313] Result is always initialized: we just tested that match != 0.
                        _BitScanReverse(&match_last_pos, match);

                        bool match_1st_16 = true;

                        if (match_last_pos != 0)
                        {
                            advance_bytes(tmp1, match_last_pos * sizeof(T));

                            const __m128i match_data =
                                _mm_loadu_si128(reinterpret_cast<const __m128i*>(tmp1));
                            const __m128i cmp_result = _mm_xor_si128(data2, match_data);

                            if (!_mm_testz_si128(cmp_result, cmp_result))
                            {
                                match_1st_16 = false;
                            }
                        }

                        if (match_1st_16)
                        {
                            const void* tail1 = tmp1;
                            advance_bytes(tail1, 16);

                            if (::memcmp(tail1, tail2, size_bytes_2 - 16) == 0)
                            {
                                mid1 = tmp1;
                                return true;
                            }
                        }

                        match ^= 1 << match_last_pos;
                    }

                    return false;
                };

                VX_DISABLE_MSVC_WARNING_POP()

                // The very last part, just compare, as true match must start with first symbol
                const __m128i data1_last = _mm_loadu_si128(reinterpret_cast<const __m128i*>(mid1));
                const __m128i match_last = _mm_xor_si128(data2, data1_last);
                if (_mm_testz_si128(match_last, match_last))
                {
                    // Matched 16 bytes, check the rest
                    const void* tail1 = mid1;
                    advance_bytes(tail1, 16);

                    if (::memcmp(tail1, tail2, size_bytes_2 - 16) == 0)
                    {
                        return mid1;
                    }
                }

                // TRANSITION, DevCom-10689455, the code below could test with _mm_cmpestrc,
                // if it has been fused with _mm_cmpestrm.

                // The main part, match all characters
                while (mid1 != stop1)
                {
                    rewind_bytes(mid1, 16);

                    const __m128i data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(mid1));
                    const auto match = _mm_cmpestrm(data2, part_size_el, data1, part_size_el, op);
                    const unsigned int match_val = _mm_cvtsi128_si32(match);
                    if (match_val != 0 && check(match_val))
                    {
                        return mid1;
                    }
                }

                // The first part, mask out already processed positions
                if (const size_t tail_bytes_1 = size_diff_bytes & 0xF; tail_bytes_1 != 0)
                {
                    mid1 = first1;
                    const __m128i data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(mid1));
                    const auto match = _mm_cmpestrm(data2, part_size_el, data1, part_size_el, op);
                    const size_t tail_el_1 = tail_bytes_1 / sizeof(T);
                    const unsigned int match_val = _mm_cvtsi128_si32(match) & ((1 << tail_el_1) - 1);
                    if (match_val != 0 && check(match_val))
                    {
                        return mid1;
                    }
                }

                return last1;
            }
        }
    }

#endif // defined(USE_SSE2)

    auto ptr1 = static_cast<const T*>(last1) - count2;
    const auto ptr2 = static_cast<const T*>(first2);

    for (;;)
    {
        if (*ptr1 == *ptr2)
        {
            bool equal = true;

            for (size_t idx = 1; idx != count2; ++idx)
            {
                if (ptr1[idx] != ptr2[idx])
                {
                    equal = false;
                    break;
                }
            }

            if (equal)
            {
                return ptr1;
            }
        }

        if (ptr1 == first1)
        {
            return last1;
        }

        --ptr1;
    }
}

} // namespace _find_seq

//=============================================================================
// find seq functions
//=============================================================================

extern "C" {

const void* VX_STDCALL search_1(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const size_t count2) noexcept
{
    return _find_seq::search_impl<_find::find_traits_1, void, void, uint8_t>(first1, last1, first2, count2);
}

const void* VX_STDCALL search_2(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const size_t count2) noexcept
{
    return _find_seq::search_impl<_find::find_traits_2, _find_seq::find_seq_traits_avx_2, void, uint16_t>(
        first1, last1, first2, count2);
}

const void* VX_STDCALL search_4(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const size_t count2) noexcept
{
    return _find_seq::search_impl<_find::find_traits_4, _find_seq::find_seq_traits_avx_4,
        _find_seq::find_seq_traits_sse_4, uint32_t>(first1, last1, first2, count2);
}

const void* VX_STDCALL search_8(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const size_t count2) noexcept
{
    return _find_seq::search_impl<_find::find_traits_8, _find_seq::find_seq_traits_avx_8,
        _find_seq::find_seq_traits_sse_8, uint64_t>(first1, last1, first2, count2);
}


const void* VX_STDCALL find_end_1(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const size_t count2) noexcept
{
    return _find_seq::find_end_impl<_find::find_traits_1, _find_seq::find_seq_traits_avx_1, void, uint8_t>(
        first1, last1, first2, count2);
}

const void* VX_STDCALL find_end_2(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const size_t count2) noexcept
{
    return _find_seq::find_end_impl<_find::find_traits_2, _find_seq::find_seq_traits_avx_2, void, uint16_t>(
        first1, last1, first2, count2);
}

const void* VX_STDCALL find_end_4(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const size_t count2) noexcept
{
    return _find_seq::find_end_impl<_find::find_traits_4, _find_seq::find_seq_traits_avx_4,
        _find_seq::find_seq_traits_sse_4, uint32_t>(first1, last1, first2, count2);
}

const void* VX_STDCALL find_end_8(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const size_t count2) noexcept
{
    return _find_seq::find_end_impl<_find::find_traits_8, _find_seq::find_seq_traits_avx_8,
        _find_seq::find_seq_traits_sse_8, uint64_t>(first1, last1, first2, count2);
}

} // extern "C"

#endif // !defined(USE_ARM_NEON)

} // namespace _simd
} // namespace vx

#endif // _VX_USE_SIMD_ALGORITHMS
