#pragma once

#include "vertex_impl/std/simd_algorithms/simd_common.hpp"

namespace vx {
namespace _simd {

//=============================================================================
// find meow of impl
//=============================================================================

namespace _find_meow_of {

enum class predicate
{
    any_of,
    none_of
};

//=============================================================================
// bitmap details
//=============================================================================

namespace _bitmap_details {

#if defined(USE_AVX2)

__m256i bitmap_step(const __m256i bitmap, const __m256i data) noexcept
{
    const __m256i data_high = _mm256_srli_epi32(data, 5);
    const __m256i bitmap_parts = _mm256_permutevar8x32_epi32(bitmap, data_high);
    const __m256i data_low_inv = _mm256_andnot_si256(data, _mm256_set1_epi32(0x1F));
    const __m256i mask = _mm256_sllv_epi32(bitmap_parts, data_low_inv);
    return mask;
}

template <typename T>
__m256i load_avx_256_8(const T* const src) noexcept
{
    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return _mm256_cvtepu8_epi32(_mm_loadu_si64(src));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        return _mm256_cvtepu16_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i*>(src)));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 8)
    {
        const __m256i low = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
        const __m256i high = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src) + 1);
        const __m256i pack = _mm256_packs_epi32(low, high);
        return _mm256_permute4x64_epi64(pack, _MM_SHUFFLE(3, 1, 2, 0));
    }
    else
    {
        VX_STATIC_ASSERT_MSG(false, "Unexpected size");
    }
}

template <typename T>
__m256i load_avx_256_8_last(const T* const src, const size_t count) noexcept
{
    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        uint8_t buf[8];
        ::memcpy(buf, src, count);
        return _mm256_cvtepu8_epi32(_mm_loadu_si64(buf));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        uint8_t buf[16];
        ::memcpy(buf, src, count * 2);
        return _mm256_cvtepu16_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i*>(buf)));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return _mm256_maskload_epi32(reinterpret_cast<const int*>(src), avx2_tail_mask_32(count * 4));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 8)
    {
        const __m256i mask_low = avx2_tail_mask_32((count > 4 ? 4 : count) * 8);
        const __m256i low = _mm256_maskload_epi32(reinterpret_cast<const int*>(src) + 0, mask_low);
        const __m256i mask_high = avx2_tail_mask_32((count > 4 ? count - 4 : 0) * 8);
        const __m256i high = _mm256_maskload_epi32(reinterpret_cast<const int*>(src) + 8, mask_high);
        const __m256i pack = _mm256_packs_epi32(low, high);
        return _mm256_permute4x64_epi64(pack, _MM_SHUFFLE(3, 1, 2, 0));
    }
    else
    {
        VX_STATIC_ASSERT_MSG(false, "Unexpected size");
    }
}

template <typename T>
__m256i mask_out_overflow(const __m256i mask, const __m256i data) noexcept
{
    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return mask;
    }
    else
    {
        const __m256i data_high =
            _mm256_and_si256(data, _mm256_set1_epi32(static_cast<int>(0xFFFF'FF00)));
        const __m256i fit_mask = _mm256_cmpeq_epi32(data_high, _mm256_setzero_si256());
        return _mm256_and_si256(mask, fit_mask);
    }
}

template <typename T>
__m256i make_bitmap_small(const T* needle_ptr, const size_t needle_length) noexcept
{
    __m256i bitmap = _mm256_setzero_si256();

    const T* const stop = needle_ptr + needle_length;

    for (; needle_ptr != stop; ++needle_ptr)
    {
        const T val = *needle_ptr;
        const __m128i count_low = _mm_cvtsi32_si128(val & 0x3F);
        const auto count_high_x8 = static_cast<uint32_t>((val >> 3) & 0x18);
        const __m256i one_1_high = _mm256_cvtepu8_epi64(_mm_cvtsi32_si128(1u << count_high_x8));
        const __m256i one_1 = _mm256_sll_epi64(one_1_high, count_low);
        bitmap = _mm256_or_si256(bitmap, one_1);
    }

    return bitmap;
}

template <typename T>
__m256i make_bitmap_large(const T* needle_ptr, const size_t needle_length) noexcept
{
    alignas(32) uint8_t table[256] = {};

    const T* const stop = needle_ptr + needle_length;

    for (; needle_ptr != stop; ++needle_ptr)
    {
        table[*needle_ptr] = 0xFF;
    }

    const auto table_as_avx = reinterpret_cast<const __m256i*>(table);

    return _mm256_setr_epi32( //
        _mm256_movemask_epi8(_mm256_load_si256(table_as_avx + 0)),
        _mm256_movemask_epi8(_mm256_load_si256(table_as_avx + 1)),
        _mm256_movemask_epi8(_mm256_load_si256(table_as_avx + 2)),
        _mm256_movemask_epi8(_mm256_load_si256(table_as_avx + 3)),
        _mm256_movemask_epi8(_mm256_load_si256(table_as_avx + 4)),
        _mm256_movemask_epi8(_mm256_load_si256(table_as_avx + 5)),
        _mm256_movemask_epi8(_mm256_load_si256(table_as_avx + 6)),
        _mm256_movemask_epi8(_mm256_load_si256(table_as_avx + 7)));
}

template <typename T>
__m256i make_bitmap(const T* const needle_ptr, const size_t needle_length) noexcept
{
    if (needle_length <= 20)
    {
        return make_bitmap_small(needle_ptr, needle_length);
    }
    else
    {
        return make_bitmap_large(needle_ptr, needle_length);
    }
}

#endif // defined(USE_AVX2)

} // namespace _bitmap_details

//=============================================================================
// bitmap impl
//=============================================================================

namespace _bitmap_impl {

#if defined(USE_AVX2)

template <typename T>
bool use_bitmap_avx(const size_t count1, const size_t count2) noexcept
{
    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        if (count2 <= 16)
        {
            return count1 > 1000;
        }
        else if (count2 <= 48)
        {
            return count1 > 80;
        }
        else if (count2 <= 240)
        {
            return count1 > 40;
        }
        else if (count2 <= 1000)
        {
            return count1 > 32;
        }
        else
        {
            return count1 > 16;
        }
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        if (count2 <= 8)
        {
            return count1 > 128;
        }
        else if (count2 <= 48)
        {
            return count1 > 32;
        }
        else if (count2 <= 72)
        {
            return count1 > 24;
        }
        else if (count2 <= 144)
        {
            return count1 > 16;
        }
        else
        {
            return count1 > 8;
        }
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        if (count2 <= 8)
        {
            return count1 > 64;
        }
        else if (count2 <= 24)
        {
            return count1 > 40;
        }
        else if (count2 <= 44)
        {
            return count1 > 24;
        }
        else if (count2 <= 112)
        {
            return count1 > 16;
        }
        else
        {
            return count1 > 8;
        }
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 8)
    {
        if (count2 <= 8)
        {
            return count1 > 40;
        }
        else if (count2 <= 12)
        {
            return count1 > 20;
        }
        else if (count2 <= 48)
        {
            return count1 > 16;
        }
        else if (count2 <= 64)
        {
            return count1 > 12;
        }
        else if (count2 <= 192)
        {
            return count1 > 8;
        }
        else
        {
            return count1 > 4;
        }
    }
    else
    {
        VX_STATIC_ASSERT_MSG(false, "unexpected size");
    }
}

#endif // defined(USE_AVX2)

template <typename T>
bool use_bitmap_scalar(const size_t count1, const size_t count2) noexcept
{
    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        if (count2 <= 32)
        {
            return false;
        }
        else if (count2 <= 48)
        {
            return count1 > 416;
        }
        else if (count2 <= 64)
        {
            return count1 > 224;
        }
        else if (count2 <= 80)
        {
            return count1 > 128;
        }
        else if (count2 <= 540)
        {
            return count1 > 48;
        }
        else
        {
            return count1 > 32;
        }
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        if (count2 <= 8)
        {
            return false;
        }
        else if (count2 <= 80)
        {
            return count1 > 16;
        }
        else
        {
            return count1 > 8;
        }
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        if (count2 <= 32)
        {
            return false;
        }
        else if (count2 <= 112)
        {
            return count1 > 16;
        }
        else
        {
            return count1 > 8;
        }
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 8)
    {
        if (count2 <= 16)
        {
            return false;
        }
        else if (count2 <= 32)
        {
            return count1 > 16;
        }
        else if (count2 <= 112)
        {
            return count1 > 8;
        }
        else
        {
            return count1 > 4;
        }
    }
    else
    {
        VX_STATIC_ASSERT_MSG(false, "unexpected size");
    }
}

enum class strategy
{
    no_bitmap,
    scalar_bitmap,
    vector_bitmap
};

template <typename T>
strategy pick_strategy(const size_t count1, const size_t count2) noexcept
{
#if defined(USE_AVX2)

    if (count1 > 48)
    {
        return use_bitmap_avx<T>(count1, count2) ? strategy::vector_bitmap : strategy::no_bitmap;
    }
    else

#endif // defined(USE_AVX2)

    {
        return use_bitmap_scalar<T>(count1, count2) ? strategy::scalar_bitmap
                                                    : strategy::no_bitmap;
    }
}

#if defined(USE_SSE2)

template <typename T>
bool can_fit_256_bits_sse(const T* needle_ptr, const size_t needle_length) noexcept
{
    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return true;
    }
    else
    {
        __m128i mask = _mm_undefined_si128();
        VX_IF_CONSTEXPR (sizeof(T) == 2)
        {
            mask = _mm_set1_epi16(static_cast<short>(0xFF00));
        }
        else VX_IF_CONSTEXPR (sizeof(T) == 4)
        {
            mask = _mm_set1_epi32(static_cast<int>(0xFFFF'FF00));
        }
        else VX_IF_CONSTEXPR (sizeof(T) == 8)
        {
            mask = _mm_set1_epi64x(static_cast<long long>(0xFFFF'FFFF'FFFF'FF00));
        }
        else
        {
            VX_STATIC_ASSERT_MSG(false, "Unexpected size");
        }

        const size_t byte_size = needle_length * sizeof(T);

        constexpr size_t vec_size = sizeof(mask);
        constexpr size_t vec_mask = vec_size - 1;
        VX_STATIC_ASSERT((vec_size & vec_mask) == 0);

        const void* stop = needle_ptr;
        advance_bytes(stop, byte_size & ~vec_mask);
        for (; needle_ptr != stop; needle_ptr += vec_size / sizeof(T))
        {
            const __m128i data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(needle_ptr));
            if (!_mm_testz_si128(mask, data))
            {
                return false;
            }
        }

        advance_bytes(stop, byte_size & vec_mask);
        for (; needle_ptr != stop; ++needle_ptr)
        {
            if ((*needle_ptr & ~T{ 0xFF }) != 0)
            {
                return false;
            }
        }

        return true;
    }
}

#endif // defined(USE_SSE2)

#if defined(USE_AVX2)

template <typename T, predicate Pred>
size_t impl_first_avx(const void* const haystack, const size_t haystack_length, const void* const needle, const size_t needle_length) noexcept
{
    zeroupper_on_exit guard; // TRANSITION, DevCom-10331414

    const auto haystack_ptr = static_cast<const T*>(haystack);
    const auto needle_ptr = static_cast<const T*>(needle);

    const __m256i bitmap = _bitmap_details::make_bitmap(needle_ptr, needle_length);

    const size_t haystack_length_vec = haystack_length & ~size_t{ 7 };
    for (size_t ix = 0; ix != haystack_length_vec; ix += 8)
    {
        const __m256i data = _bitmap_details::load_avx_256_8(haystack_ptr + ix);
        const __m256i mask_part = _bitmap_details::bitmap_step(bitmap, data);
        const __m256i mask_full = _bitmap_details::mask_out_overflow<T>(mask_part, data);
        unsigned int bingo = _mm256_movemask_ps(_mm256_castsi256_ps(mask_full));

        VX_IF_CONSTEXPR (Pred == predicate::none_of)
        {
            bingo ^= 0xFF;
        }

        if (bingo != 0)
        {
            return ix + _tzcnt_u32(bingo);
        }
    }

    const size_t haystack_length_tail = haystack_length & 7;
    if (haystack_length_tail != 0)
    {
        const unsigned int tail_bingo_mask = (1 << haystack_length_tail) - 1;
        const auto last_ptr = haystack_ptr + haystack_length_vec;
        const __m256i data = _bitmap_details::load_avx_256_8_last(last_ptr, haystack_length_tail);
        const __m256i mask_part = _bitmap_details::bitmap_step(bitmap, data);
        const __m256i mask_full = _bitmap_details::mask_out_overflow<T>(mask_part, data);
        unsigned int bingo = _mm256_movemask_ps(_mm256_castsi256_ps(mask_full)) & tail_bingo_mask;

        VX_IF_CONSTEXPR (Pred == predicate::none_of)
        {
            bingo ^= tail_bingo_mask;
        }

        if (bingo != 0)
        {
            return haystack_length_vec + _tzcnt_u32(bingo);
        }
    }

    return static_cast<size_t>(-1);
}

template <typename T, predicate Pred>
size_t impl_last_avx(const void* const haystack, size_t haystack_length, const void* const needle, const size_t needle_length) noexcept
{
    zeroupper_on_exit guard; // TRANSITION, DevCom-10331414

    const auto haystack_ptr = static_cast<const T*>(haystack);
    const auto needle_ptr = static_cast<const T*>(needle);

    const __m256i bitmap = _bitmap_details::make_bitmap(needle_ptr, needle_length);

    while (haystack_length >= 8)
    {
        haystack_length -= 8;
        const __m256i data = _bitmap_details::load_avx_256_8(haystack_ptr + haystack_length);
        const __m256i mask_part = _bitmap_details::bitmap_step(bitmap, data);
        const __m256i mask_full = _bitmap_details::mask_out_overflow<T>(mask_part, data);
        unsigned int bingo = _mm256_movemask_ps(_mm256_castsi256_ps(mask_full));

        VX_IF_CONSTEXPR (Pred == predicate::none_of)
        {
            bingo ^= 0xFF;
        }

        if (bingo != 0)
        {
            return haystack_length + 31 - _lzcnt_u32(bingo);
        }
    }

    const size_t haystack_length_tail = haystack_length & 7;
    if (haystack_length_tail != 0)
    {
        const unsigned int tail_bingo_mask = (1 << haystack_length_tail) - 1;
        const __m256i data = _bitmap_details::load_avx_256_8_last(haystack_ptr, haystack_length_tail);
        const __m256i mask_part = _bitmap_details::bitmap_step(bitmap, data);
        const __m256i mask_full = _bitmap_details::mask_out_overflow<T>(mask_part, data);
        unsigned int bingo = _mm256_movemask_ps(_mm256_castsi256_ps(mask_full)) & tail_bingo_mask;

        VX_IF_CONSTEXPR (Pred == predicate::none_of)
        {
            bingo ^= tail_bingo_mask;
        }

        if (bingo != 0)
        {
            return 31 - _lzcnt_u32(bingo);
        }
    }

    return static_cast<size_t>(-1);
}

#endif // defined(USE_AVX2)

using scalar_table_t = bool[256];

template <typename T>
VX_NO_DISCARD bool build_scalar_table(
    const void* const needle,
    const size_t needle_length,
    scalar_table_t& table) noexcept
{
    auto ptr = static_cast<const T*>(needle);
    const auto end = ptr + needle_length;

    for (; ptr != end; ++ptr)
    {
        const T val = *ptr;

        VX_IF_CONSTEXPR (sizeof(val) > 1)
        {
            if (val >= 256)
            {
                return false;
            }
        }

        table[val] = true;
    }

    return true;
}

template <typename T>
void build_scalar_table_no_check(
    const void* const needle,
    const size_t needle_length,
    scalar_table_t& table) noexcept
{
    auto ptr = static_cast<const T*>(needle);
    const auto end = ptr + needle_length;

    for (; ptr != end; ++ptr)
    {
        table[*ptr] = true;
    }
}

template <typename T, predicate Pred>
size_t impl_first_scalar(
    const void* const haystack,
    const size_t haystack_length,
    const scalar_table_t& table) noexcept
{
    const auto haystack_ptr = static_cast<const T*>(haystack);

    for (size_t ix = 0; ix != haystack_length; ++ix)
    {
        const T val = haystack_ptr[ix];

        VX_IF_CONSTEXPR (sizeof(val) > 1)
        {
            if (val >= 256)
            {
                VX_IF_CONSTEXPR (Pred == predicate::any_of)
                {
                    continue;
                }
                else
                {
                    return ix;
                }
            }
        }

        VX_IF_CONSTEXPR (Pred == predicate::any_of)
        {
            if (table[val])
            {
                return ix;
            }
        }
        else
        {
            if (!table[val])
            {
                return ix;
            }
        }
    }

    return static_cast<size_t>(-1);
}

template <typename T, predicate Pred>
size_t impl_last_scalar(
    const void* const haystack,
    size_t haystack_length,
    const scalar_table_t& table) noexcept
{
    const auto haystack_ptr = static_cast<const T*>(haystack);

    while (haystack_length != 0)
    {
        --haystack_length;

        const T val = haystack_ptr[haystack_length];

        VX_IF_CONSTEXPR (sizeof(val) > 1)
        {
            if (val >= 256)
            {
                VX_IF_CONSTEXPR (Pred == predicate::any_of)
                {
                    continue;
                }
                else
                {
                    return haystack_length;
                }
            }
        }

        VX_IF_CONSTEXPR (Pred == predicate::any_of)
        {
            if (table[val])
            {
                return haystack_length;
            }
        }
        else
        {
            if (!table[val])
            {
                return haystack_length;
            }
        }
    }

    return static_cast<size_t>(-1);
}

} // namespace _bitmap_impl

//=============================================================================
// first of impl
//=============================================================================

namespace _first_of {

template <typename T, predicate Pred>
const void* fallback(const void* first1, const void* const last1, const void* const first2, const void* const last2) noexcept
{
    auto ptr_haystack = static_cast<const T*>(first1);
    const auto ptr_haystack_end = static_cast<const T*>(last1);
    const auto ptr_needle = static_cast<const T*>(first2);
    const auto ptr_needle_end = static_cast<const T*>(last2);

    for (; ptr_haystack != ptr_haystack_end; ++ptr_haystack)
    {
        VX_IF_CONSTEXPR (Pred == predicate::any_of)
        {
            for (auto ptr = ptr_needle; ptr != ptr_needle_end; ++ptr)
            {
                if (*ptr_haystack == *ptr)
                {
                    return ptr_haystack;
                }
            }
        }
        else
        {
            bool match = false;
            for (auto ptr = ptr_needle; ptr != ptr_needle_end; ++ptr)
            {
                if (*ptr_haystack == *ptr)
                {
                    match = true;
                    break;
                }
            }

            if (!match)
            {
                return ptr_haystack;
            }
        }
    }

    return ptr_haystack;
}

#if defined(USE_SSE2)

template <typename T, predicate Pred>
const void* impl_pcmpestri(const void* first1, const size_t haystack_length, const void* const first2, const size_t needle_length) noexcept
{
    constexpr int op_base =
        (Pred == predicate::any_of ? _SIDD_POSITIVE_POLARITY : _SIDD_MASKED_NEGATIVE_POLARITY) | (sizeof(T) == 1 ? _SIDD_UBYTE_OPS : _SIDD_UWORD_OPS) | _SIDD_CMP_EQUAL_ANY;
    constexpr int op = op_base | _SIDD_LEAST_SIGNIFICANT;
    constexpr int part_size_el = sizeof(T) == 1 ? 16 : 8;

    const void* stop_at = first1;
    advance_bytes(stop_at, haystack_length & ~size_t{ 0xF });

    if (needle_length <= 16)
    {
        // Special handling of small needle
        // The generic branch could also be modified to handle it, but with slightly worse performance

        const int needle_length_el = static_cast<int>(needle_length / sizeof(T));

        alignas(16) uint8_t tmp2[16];
        ::memcpy(tmp2, first2, needle_length);
        const __m128i data2 = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp2));

        while (first1 != stop_at)
        {
            const __m128i data1 = _mm_loadu_si128(static_cast<const __m128i*>(first1));
            if (_mm_cmpestrc(data2, needle_length_el, data1, part_size_el, op))
            {
                const int pos = _mm_cmpestri(data2, needle_length_el, data1, part_size_el, op);
                advance_bytes(first1, pos * sizeof(T));
                return first1;
            }

            advance_bytes(first1, 16);
        }

        if (const size_t last_part_size = haystack_length & 0xF; last_part_size != 0)
        {
            const int last_part_size_el = static_cast<int>(last_part_size / sizeof(T));

            alignas(16) uint8_t tmp1[16];
            ::memcpy(tmp1, first1, last_part_size);
            const __m128i data1 = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp1));

            if (_mm_cmpestrc(data2, needle_length_el, data1, last_part_size_el, op))
            {
                const int pos = _mm_cmpestri(data2, needle_length_el, data1, last_part_size_el, op);
                advance_bytes(first1, pos * sizeof(T));
                return first1;
            }

            advance_bytes(first1, last_part_size);
        }

        return first1;
    }
    else
    {
        const void* last_needle = first2;
        advance_bytes(last_needle, needle_length & ~size_t{ 0xF });

        const int last_needle_length = static_cast<int>(needle_length & 0xF);

        alignas(16) uint8_t tmp2[16];
        ::memcpy(tmp2, last_needle, last_needle_length);
        const __m128i last_needle_val = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp2));
        const int last_needle_length_el = last_needle_length / sizeof(T);

        constexpr int not_found = 16; // arbitrary value greater than any found value

        // structure was padded due to alignment specifier
        VX_DISABLE_MSVC_WARNING_PUSH()
        VX_DISABLE_MSVC_WARNING(4324)

        const auto test_whole_needle = [=](const __m128i data1, const int size1, const int found_pos_init) noexcept
        {
            VX_IF_CONSTEXPR (Pred == predicate::any_of)
            {
                int found_pos = found_pos_init;

                const auto step = [&found_pos](const __m128i data2, const int size2, const __m128i data1, const int size1) noexcept
                {
                    if (_mm_cmpestrc(data2, size2, data1, size1, op))
                    {
                        const int pos = _mm_cmpestri(data2, size2, data1, size1, op);
                        if (pos < found_pos)
                        {
                            found_pos = pos;
                        }
                    }
                };

                const void* cur_needle = first2;
                do
                {
                    const __m128i data2 = _mm_loadu_si128(static_cast<const __m128i*>(cur_needle));
                    step(data2, part_size_el, data1, size1);
                    advance_bytes(cur_needle, 16);
                } while (cur_needle != last_needle);

                if (last_needle_length_el != 0)
                {
                    step(last_needle_val, last_needle_length_el, data1, size1);
                }

                return found_pos;
            }
            else
            {
                constexpr int op_mask = op_base | _SIDD_BIT_MASK;

                const void* cur_needle = first2;

                const __m128i data2_first = _mm_loadu_si128(static_cast<const __m128i*>(cur_needle));

                __m128i found = _mm_cmpestrm(data2_first, part_size_el, data1, size1, op_mask);
                advance_bytes(cur_needle, 16);

                while (cur_needle != last_needle)
                {
                    const __m128i data2 = _mm_loadu_si128(static_cast<const __m128i*>(cur_needle));
                    const __m128i found_part =
                        _mm_cmpestrm(data2, part_size_el, data1, size1, op_mask);
                    found = _mm_and_si128(found, found_part);
                    advance_bytes(cur_needle, 16);
                }

                if (last_needle_length_el != 0)
                {
                    const __m128i found_part =
                        _mm_cmpestrm(last_needle_val, last_needle_length_el, data1, size1, op_mask);
                    found = _mm_and_si128(found, found_part);
                }

                const unsigned int bingo = _mm_cvtsi128_si32(found);
                int found_pos = found_pos_init;

                if (bingo != 0)
                {
                    unsigned long tmp;
                    // CodeQL [SM02313] tmp is always initialized: we just tested `if (bingo != 0)`.
                    _BitScanForward(&tmp, bingo);
                    if (found_pos > static_cast<int>(tmp))
                    {
                        found_pos = static_cast<int>(tmp);
                    }
                }

                return found_pos;
            }
        };

        VX_DISABLE_MSVC_WARNING_POP()

        while (first1 != stop_at)
        {
            const int found_pos = test_whole_needle(
                _mm_loadu_si128(static_cast<const __m128i*>(first1)),
                part_size_el,
                not_found);

            if (found_pos != not_found)
            {
                advance_bytes(first1, found_pos * sizeof(T));
                return first1;
            }

            advance_bytes(first1, 16);
        }

        if (const size_t last_part_size = haystack_length & 0xF; last_part_size != 0)
        {
            const int last_part_size_el = static_cast<int>(last_part_size / sizeof(T));

            alignas(16) uint8_t tmp1[16];
            ::memcpy(tmp1, first1, last_part_size);
            const __m128i data1 = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp1));

            const int found_pos = test_whole_needle(data1, last_part_size_el, last_part_size_el);

            advance_bytes(first1, found_pos * sizeof(T));
        }

        return first1;
    }
}

#endif // defined(USE_SSE2)

template <typename T>
struct find_first_of_traits;

#if defined(USE_AVX2)

template <>
struct find_first_of_traits<uint32_t>
{
    static __m256i cmp_avx(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_cmpeq_epi32(lhs, rhs);
    }

    template <size_t amount>
    static __m256i spread_avx(__m256i val, const size_t needle_length_el) noexcept
    {
        VX_IF_CONSTEXPR (amount == 0)
        {
            return _mm256_undefined_si256();
        }
        else VX_IF_CONSTEXPR (amount == 1)
        {
            return _mm256_broadcastd_epi32(_mm256_castsi256_si128(val));
        }
        else VX_IF_CONSTEXPR (amount == 2)
        {
            return _mm256_broadcastq_epi64(_mm256_castsi256_si128(val));
        }
        else VX_IF_CONSTEXPR (amount == 4)
        {
            if (needle_length_el < 4)
            {
                val = _mm256_shuffle_epi32(val, _MM_SHUFFLE(0, 2, 1, 0));
            }

            return _mm256_permute4x64_epi64(val, _MM_SHUFFLE(1, 0, 1, 0));
        }
        else VX_IF_CONSTEXPR (amount == 8)
        {
            if (needle_length_el < 8)
            {
                const __m256i mask = avx2_tail_mask_32(needle_length_el * 4);
                // zero unused elements in sequential permutation mask, so will be filled by 1st
                const __m256i perm = _mm256_and_si256(_mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0), mask);
                val = _mm256_permutevar8x32_epi32(val, perm);
            }

            return val;
        }
        else
        {
            VX_STATIC_ASSERT_MSG(false, "Unexpected amount");
        }
    }

    template <size_t amount>
    static __m256i shuffle_avx(const __m256i val) noexcept
    {
        VX_IF_CONSTEXPR (amount == 1)
        {
            return _mm256_shuffle_epi32(val, _MM_SHUFFLE(2, 3, 0, 1));
        }
        else VX_IF_CONSTEXPR (amount == 2)
        {
            return _mm256_shuffle_epi32(val, _MM_SHUFFLE(1, 0, 3, 2));
        }
        else VX_IF_CONSTEXPR (amount == 4)
        {
            return _mm256_permute4x64_epi64(val, _MM_SHUFFLE(1, 0, 3, 2));
        }
        else
        {
            VX_STATIC_ASSERT_MSG(false, "Unexpected amount");
        }
    }
};

template <>
struct find_first_of_traits<uint64_t>
{
    static __m256i cmp_avx(const __m256i lhs, const __m256i rhs) noexcept
    {
        return _mm256_cmpeq_epi64(lhs, rhs);
    }

    template <size_t amount>
    static __m256i spread_avx(const __m256i val, const size_t needle_length_el) noexcept
    {
        VX_IF_CONSTEXPR (amount == 0)
        {
            return _mm256_undefined_si256();
        }
        else VX_IF_CONSTEXPR (amount == 1)
        {
            return _mm256_broadcastq_epi64(_mm256_castsi256_si128(val));
        }
        else VX_IF_CONSTEXPR (amount == 2)
        {
            return _mm256_permute4x64_epi64(val, _MM_SHUFFLE(1, 0, 1, 0));
        }
        else VX_IF_CONSTEXPR (amount == 4)
        {
            if (needle_length_el < 4)
            {
                return _mm256_permute4x64_epi64(val, _MM_SHUFFLE(0, 2, 1, 0));
            }

            return val;
        }
        else
        {
            VX_STATIC_ASSERT_MSG(false, "Unexpected amount");
        }
    }

    template <size_t amount>
    static __m256i shuffle_avx(const __m256i val) noexcept
    {
        VX_IF_CONSTEXPR (amount == 1)
        {
            return _mm256_shuffle_epi32(val, _MM_SHUFFLE(1, 0, 3, 2));
        }
        else VX_IF_CONSTEXPR (amount == 2)
        {
            return _mm256_permute4x64_epi64(val, _MM_SHUFFLE(1, 0, 3, 2));
        }
        else
        {
            VX_STATIC_ASSERT_MSG(false, "Unexpected amount");
        }
    }
};

template <typename Traits, size_t needle_length_el_magnitude>
__m256i shuffle_step(const __m256i data1, const __m256i data2s0) noexcept
{
    __m256i eq = _mm256_setzero_si256();
    VX_IF_CONSTEXPR (needle_length_el_magnitude >= 1)
    {
        eq = Traits::cmp_avx(data1, data2s0);
        VX_IF_CONSTEXPR (needle_length_el_magnitude >= 2)
        {
            const __m256i data2s1 = Traits::template shuffle_avx<1>(data2s0);
            eq = _mm256_or_si256(eq, Traits::cmp_avx(data1, data2s1));
            VX_IF_CONSTEXPR (needle_length_el_magnitude >= 4)
            {
                const __m256i data2s2 = Traits::template shuffle_avx<2>(data2s0);
                eq = _mm256_or_si256(eq, Traits::cmp_avx(data1, data2s2));
                const __m256i data2s3 = Traits::template shuffle_avx<1>(data2s2);
                eq = _mm256_or_si256(eq, Traits::cmp_avx(data1, data2s3));
                VX_IF_CONSTEXPR (needle_length_el_magnitude >= 8)
                {
                    const __m256i data2s4 = Traits::template shuffle_avx<4>(data2s0);
                    eq = _mm256_or_si256(eq, Traits::cmp_avx(data1, data2s4));
                    const __m256i data2s5 = Traits::template shuffle_avx<1>(data2s4);
                    eq = _mm256_or_si256(eq, Traits::cmp_avx(data1, data2s5));
                    const __m256i data2s6 = Traits::template shuffle_avx<2>(data2s4);
                    eq = _mm256_or_si256(eq, Traits::cmp_avx(data1, data2s6));
                    const __m256i data2s7 = Traits::template shuffle_avx<1>(data2s6);
                    eq = _mm256_or_si256(eq, Traits::cmp_avx(data1, data2s7));
                }
            }
        }
    }
    return eq;
}

template <typename T, bool large, size_t last2_length_el_magnitude>
const void* shuffle_impl(const void* first1, const size_t haystack_length, const void* const first2, const void* const stop2, const size_t last2_length_el) noexcept
{
    using Traits = find_first_of_traits<T>;
    constexpr size_t _Length_el = 32 / sizeof(T);

    const __m256i last2val = _mm256_maskload_epi32(
        reinterpret_cast<const int*>(stop2),
        avx2_tail_mask_32(last2_length_el * sizeof(T)));
    const __m256i last2s0 =
        Traits::template spread_avx<last2_length_el_magnitude>(last2val, last2_length_el);

    const void* stop1 = first1;
    advance_bytes(stop1, haystack_length & ~size_t{ 0x1F });

    for (; first1 != stop1; advance_bytes(first1, 32))
    {
        const __m256i data1 = _mm256_loadu_si256(static_cast<const __m256i*>(first1));
        __m256i eq = shuffle_step<Traits, last2_length_el_magnitude>(data1, last2s0);

        VX_IF_CONSTEXPR (large)
        {
            for (const void* ptr2 = first2; ptr2 != stop2; advance_bytes(ptr2, 32))
            {
                const __m256i data2s0 = _mm256_loadu_si256(static_cast<const __m256i*>(ptr2));
                eq = _mm256_or_si256(eq, shuffle_step<Traits, _Length_el>(data1, data2s0));
            }
        }

        if (const uint32_t bingo = _mm256_movemask_epi8(eq); bingo != 0)
        {
            const unsigned long _Offset = _tzcnt_u32(bingo);
            advance_bytes(first1, _Offset);
            return first1;
        }
    }

    if (const size_t haystack_tail_length = haystack_length & 0x1C; haystack_tail_length != 0)
    {
        const __m256i tail_mask = avx2_tail_mask_32(haystack_tail_length);
        const __m256i data1 = _mm256_maskload_epi32(static_cast<const int*>(first1), tail_mask);
        __m256i eq = shuffle_step<Traits, last2_length_el_magnitude>(data1, last2s0);

        VX_IF_CONSTEXPR (large)
        {
            for (const void* ptr2 = first2; ptr2 != stop2; advance_bytes(ptr2, 32))
            {
                const __m256i data2s0 = _mm256_loadu_si256(static_cast<const __m256i*>(ptr2));
                eq = _mm256_or_si256(eq, shuffle_step<Traits, _Length_el>(data1, data2s0));
            }
        }

        if (const uint32_t bingo = _mm256_movemask_epi8(_mm256_and_si256(eq, tail_mask)); bingo != 0)
        {
            const unsigned long _Offset = _tzcnt_u32(bingo);
            advance_bytes(first1, _Offset);
            return first1;
        }

        advance_bytes(first1, haystack_tail_length);
    }

    return first1;
}

template <typename T, bool large>
const void* shuffle_impl_dispatch_magnitude(const void* const first1, const size_t haystack_length, const void* const first2, const void* const stop2, const size_t last2_length_el) noexcept
{
    if (last2_length_el == 0)
    {
        return shuffle_impl<T, large, 0>(first1, haystack_length, first2, stop2, last2_length_el);
    }
    else if (last2_length_el == 1)
    {
        return shuffle_impl<T, large, 1>(first1, haystack_length, first2, stop2, last2_length_el);
    }
    else if (last2_length_el == 2)
    {
        return shuffle_impl<T, large, 2>(first1, haystack_length, first2, stop2, last2_length_el);
    }
    else if (last2_length_el <= 4)
    {
        return shuffle_impl<T, large, 4>(first1, haystack_length, first2, stop2, last2_length_el);
    }
    else if (last2_length_el <= 8)
    {
        VX_IF_CONSTEXPR (sizeof(T) == 4)
        {
            return shuffle_impl<T, large, 8>(
                first1,
                haystack_length,
                first2,
                stop2,
                last2_length_el);
        }
    }

    VX_UNREACHABLE;
}

template <typename T>
const void* impl_4_8(const void* const first1, const size_t haystack_length, const void* const first2, const size_t needle_length) noexcept
{
    zeroupper_on_exit guard; // TRANSITION, DevCom-10331414

    const size_t last_needle_length = needle_length & 0x1F;
    const size_t last_needle_length_el = last_needle_length / sizeof(T);

    if (const size_t needle_length_large = needle_length & ~size_t{ 0x1F }; needle_length_large != 0)
    {
        const void* stop2 = first2;
        advance_bytes(stop2, needle_length_large);
        return shuffle_impl_dispatch_magnitude<T, true>(
            first1,
            haystack_length,
            first2,
            stop2,
            last_needle_length_el);
    }
    else
    {
        return shuffle_impl_dispatch_magnitude<T, false>(
            first1,
            haystack_length,
            first2,
            first2,
            last_needle_length_el);
    }
}

#endif // defined(USE_AVX2)

template <typename T>
const void* VX_STDCALL dispatch_ptr(const void* const first1, const void* const last1, const void* const first2, const void* const last2) noexcept
{
#if defined(USE_SSE2)

    VX_IF_CONSTEXPR (sizeof(T) <= 2)
    {
        return impl_pcmpestri<T, predicate::any_of>(
            first1,
            byte_length(first1, last1),
            first2,
            byte_length(first2, last2));
    }
    else

#endif // defined(USE_SSE2)

#if defined(USE_AVX2)

        VX_IF_CONSTEXPR (sizeof(T) > 2)
    {
        return impl_4_8<T>(
            first1,
            byte_length(first1, last1),
            first2,
            byte_length(first2, last2));
    }
    else

#endif // defined(USE_AVX2)

    {
        return fallback<T, predicate::any_of>(first1, last1, first2, last2);
    }
}

template <typename T>
size_t pos_from_ptr(
    const void* const result,
    const void* const first1,
    const void* const last1) noexcept
{
    if (result != last1)
    {
        return byte_length(first1, result) / sizeof(T);
    }
    else
    {
        return static_cast<size_t>(-1);
    }
}

#if defined(USE_SSE2)

template <typename T, predicate Pred>
size_t dispatch_pos_sse_1_2(const void* const first1, const size_t count1, const void* const first2, const size_t count2) noexcept
{
    using namespace _bitmap_impl;

    const strategy strat = pick_strategy<T>(count1, count2);

    #if defined(USE_X86)

        #if defined(USE_AVX2)

    if (strat == strategy::vector_bitmap)
    {
        if (can_fit_256_bits_sse(static_cast<const T*>(first2), count2))
        {
            return impl_first_avx<T, Pred>(first1, count1, first2, count2);
        }
    }
    else

        #endif // defined(USE_AVX2)

        #if defined(USE_SSE2)

        if (strat == strategy::scalar_bitmap)
    {
        if (can_fit_256_bits_sse(static_cast<const T*>(first2), count2))
        {
            alignas(32) scalar_table_t table = {};
            build_scalar_table_no_check<T>(first2, count2, table);
            return impl_first_scalar<T, Pred>(first1, count1, table);
        }
    }

        #endif // defined(USE_SSE2)

    #endif // defined(USE_X86)

    const void* const last1 = static_cast<const T*>(first1) + count1;
    const size_t size_bytes_1 = count1 * sizeof(T);
    const size_t size_bytes_2 = count2 * sizeof(T);

    return pos_from_ptr<T>(
        impl_pcmpestri<T, Pred>(first1, size_bytes_1, first2, size_bytes_2),
        first1,
        last1);
}

#endif // defined(USE_SSE2)

#if defined(USE_AVX2)

template <typename T>
size_t dispatch_pos_avx_4_8(const void* const first1, const size_t count1, const void* const first2, const size_t count2) noexcept
{
    using namespace _bitmap_impl;

    const auto strat = pick_strategy<T>(count1, count2);

    if (strat == strategy::vector_bitmap)
    {
        if (can_fit_256_bits_sse(static_cast<const T*>(first2), count2))
        {
            return impl_first_avx<T, predicate::any_of>(first1, count1, first2, count2);
        }
    }
    else if (strat == strategy::scalar_bitmap)
    {
        if (can_fit_256_bits_sse(static_cast<const T*>(first2), count2))
        {
            alignas(32) scalar_table_t table = {};
            build_scalar_table_no_check<T>(first2, count2, table);
            return impl_first_scalar<T, predicate::any_of>(first1, count1, table);
        }
    }

    const void* const last1 = static_cast<const T*>(first1) + count1;
    const size_t size_bytes_1 = count1 * sizeof(T);
    const size_t size_bytes_2 = count2 * sizeof(T);

    return pos_from_ptr<T>(
        impl_4_8<T>(first1, size_bytes_1, first2, size_bytes_2),
        first1,
        last1);
}

#endif // defined(USE_AVX2)

template <typename T, predicate Pred>
size_t dispatch_pos_fallback(const void* const first1, const size_t count1, const void* const first2, const size_t count2) noexcept
{
    using namespace _bitmap_impl;

    scalar_table_t table = {};
    if (build_scalar_table<T>(first2, count2, table))
    {
        return impl_first_scalar<T, Pred>(first1, count1, table);
    }

    const void* const last1 = static_cast<const T*>(first1) + count1;
    const void* const last2 = static_cast<const T*>(first2) + count2;

    return pos_from_ptr<T>(fallback<T, Pred>(first1, last1, first2, last2), first1, last1);
}

template <typename T, predicate Pred>
size_t VX_STDCALL dispatch_pos(const void* const first1, const size_t count1, const void* const first2, const size_t count2) noexcept
{
#if defined(USE_AVX2)

    VX_IF_CONSTEXPR (sizeof(T) > 2)
    {
        return dispatch_pos_avx_4_8<T>(first1, count1, first2, count2);
    }
    else

#endif // defined(USE_AVX2)

#if defined(USE_SSE2)

    VX_IF_CONSTEXPR (sizeof(T) <= 2)
    {
        return dispatch_pos_sse_1_2<T, Pred>(first1, count1, first2, count2);
    }
    else

#endif // defined(USE_SSE2)

    {
        return dispatch_pos_fallback<T, Pred>(first1, count1, first2, count2);
    }
}

} // namespace _first_of

//=============================================================================
// last of impl
//=============================================================================

namespace _last_of {

template <typename T, predicate Pred>
size_t VX_STDCALL fallback(const void* const haystack, const size_t haystack_length, const void* const needle, const size_t needle_length) noexcept
{
    const auto ptr_haystack = static_cast<const T*>(haystack);
    size_t pos = haystack_length;
    const auto needle_end = static_cast<const T*>(needle) + needle_length;

    while (pos != 0)
    {
        --pos;

        VX_IF_CONSTEXPR (Pred == predicate::any_of)
        {
            for (auto ptr = static_cast<const T*>(needle); ptr != needle_end; ++ptr)
            {
                if (ptr_haystack[pos] == *ptr)
                {
                    return pos;
                }
            }
        }
        else
        {
            bool match = false;
            for (auto ptr = static_cast<const T*>(needle); ptr != needle_end; ++ptr)
            {
                if (ptr_haystack[pos] == *ptr)
                {
                    match = true;
                    break;
                }
            }

            if (!match)
            {
                return pos;
            }
        }
    }

    return static_cast<size_t>(-1);
}

#if defined(USE_SSE2)

template <typename T, predicate Pred>
size_t impl(const void* const haystack, const size_t haystack_length, const void* const needle, const size_t needle_length) noexcept
{
    const size_t haystack_length_bytes = haystack_length * sizeof(T);

    constexpr int op_base =
        (Pred == predicate::any_of ? _SIDD_POSITIVE_POLARITY : _SIDD_MASKED_NEGATIVE_POLARITY) | (sizeof(T) == 1 ? _SIDD_UBYTE_OPS : _SIDD_UWORD_OPS) | _SIDD_CMP_EQUAL_ANY;
    constexpr int op = op_base | _SIDD_MOST_SIGNIFICANT;
    constexpr int part_size_el = sizeof(T) == 1 ? 16 : 8;

    const size_t last_part_size = haystack_length_bytes & 0xF;

    const void* stop_at = haystack;
    advance_bytes(stop_at, last_part_size);

    const void* cur = haystack;
    advance_bytes(cur, haystack_length_bytes);

    const size_t needle_length_bytes = needle_length * sizeof(T);

    if (needle_length_bytes <= 16)
    {
        // Special handling of small needle
        // The generic branch could also be modified to handle it, but with slightly worse performance
        const int needle_length_el = static_cast<int>(needle_length);

        alignas(16) uint8_t tmp2[16];
        ::memcpy(tmp2, needle, needle_length_bytes);
        const __m128i data2 = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp2));

        while (cur != stop_at)
        {
            rewind_bytes(cur, 16);
            const __m128i data1 = _mm_loadu_si128(static_cast<const __m128i*>(cur));
            if (_mm_cmpestrc(data2, needle_length_el, data1, part_size_el, op))
            {
                const int pos = _mm_cmpestri(data2, needle_length_el, data1, part_size_el, op);
                return byte_length(haystack, cur) / sizeof(T) + pos;
            }
        }

        if (last_part_size != 0)
        {
            const int last_part_size_el = static_cast<int>(last_part_size / sizeof(T));
            __m128i data1;

            if (haystack_length_bytes >= 16)
            {
                data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(haystack));
            }
            else
            {
                alignas(16) uint8_t tmp1[16];
                ::memcpy(tmp1, haystack, haystack_length_bytes);
                data1 = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp1));
            }

            if (_mm_cmpestrc(data2, needle_length_el, data1, last_part_size_el, op))
            {
                return _mm_cmpestri(data2, needle_length_el, data1, last_part_size_el, op);
            }
        }

        return static_cast<size_t>(-1);
    }
    else
    {
        const void* last_needle = needle;
        advance_bytes(last_needle, needle_length_bytes & ~size_t{ 0xF });

        const int last_needle_length = static_cast<int>(needle_length_bytes & 0xF);

        alignas(16) uint8_t tmp2[16];
        ::memcpy(tmp2, last_needle, last_needle_length);
        const __m128i last_needle_val = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp2));
        const int last_needle_length_el = last_needle_length / sizeof(T);

        // equal to npos when treated as size_t; also less than any found value
        constexpr int not_found = -1;

        // structure was padded due to alignment specifier
        VX_DISABLE_MSVC_WARNING_PUSH()
        VX_DISABLE_MSVC_WARNING(4324)

        const auto test_whole_needle = [=](const __m128i data1, const int size1) noexcept
        {
            VX_IF_CONSTEXPR (Pred == predicate::any_of)
            {
                int found_pos = not_found;

                const auto step = [&found_pos](const __m128i data2, const int size2, const __m128i data1, const int size1) noexcept
                {
                    if (_mm_cmpestrc(data2, size2, data1, size1, op))
                    {
                        const int pos = _mm_cmpestri(data2, size2, data1, size1, op);
                        if (pos > found_pos)
                        {
                            found_pos = pos;
                        }
                    }
                };

                const void* cur_needle = needle;
                do
                {
                    const __m128i data2 = _mm_loadu_si128(static_cast<const __m128i*>(cur_needle));
                    step(data2, part_size_el, data1, size1);
                    advance_bytes(cur_needle, 16);
                } while (cur_needle != last_needle);

                if (last_needle_length_el != 0)
                {
                    step(last_needle_val, last_needle_length_el, data1, size1);
                }

                return found_pos;
            }
            else
            {
                constexpr int op_mask = op_base | _SIDD_BIT_MASK;

                const void* cur_needle = needle;

                const __m128i data2_first = _mm_loadu_si128(static_cast<const __m128i*>(cur_needle));

                __m128i found = _mm_cmpestrm(data2_first, part_size_el, data1, size1, op_mask);

                while (cur_needle != last_needle)
                {
                    const __m128i data2 = _mm_loadu_si128(static_cast<const __m128i*>(cur_needle));
                    const __m128i found_part =
                        _mm_cmpestrm(data2, part_size_el, data1, size1, op_mask);
                    found = _mm_and_si128(found, found_part);
                    advance_bytes(cur_needle, 16);
                }

                if (last_needle_length_el != 0)
                {
                    const __m128i found_part =
                        _mm_cmpestrm(last_needle_val, last_needle_length_el, data1, size1, op_mask);
                    found = _mm_and_si128(found, found_part);
                    advance_bytes(cur_needle, 16);
                }

                const unsigned int bingo = _mm_cvtsi128_si32(found);
                int found_pos = not_found;

                if (bingo != 0)
                {
                    unsigned long tmp;
                    // CodeQL [SM02313] tmp is always initialized: we just tested `if (bingo != 0)`.
                    _BitScanReverse(&tmp, bingo);
                    found_pos = static_cast<int>(tmp);
                }

                return found_pos;
            }
        };

        VX_DISABLE_MSVC_WARNING_POP()

        while (cur != stop_at)
        {
            rewind_bytes(cur, 16);
            const int found_pos =
                test_whole_needle(_mm_loadu_si128(static_cast<const __m128i*>(cur)), part_size_el);

            if (found_pos != not_found)
            {
                return byte_length(haystack, cur) / sizeof(T) + found_pos;
            }
        }

        if (last_part_size != 0)
        {
            const int last_part_size_el = static_cast<int>(last_part_size / sizeof(T));
            __m128i data1;

            if (haystack_length_bytes >= 16)
            {
                data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(haystack));
            }
            else
            {
                alignas(16) uint8_t tmp1[16];
                ::memcpy(tmp1, haystack, haystack_length_bytes);
                data1 = _mm_load_si128(reinterpret_cast<const __m128i*>(tmp1));
            }

            return test_whole_needle(data1, last_part_size_el);
        }

        return static_cast<size_t>(not_found);
    }
}

#endif // defined(USE_SSE2)

template <typename T, predicate Pred>
size_t VX_STDCALL dispatch_pos(const void* const first1, const size_t count1, const void* const first2, const size_t count2) noexcept
{
    using namespace _bitmap_impl;

#if defined(USE_SSE2)

    const auto strat = pick_strategy<T>(count1, count2);

    if (strat == strategy::vector_bitmap)
    {
        if (can_fit_256_bits_sse(static_cast<const T*>(first2), count2))
        {
    #if defined(USE_AVX2)
            return impl_last_avx<T, Pred>(first1, count1, first2, count2);
    #endif // defined(USE_AVX2)
        }
    }
    else if (strat == strategy::scalar_bitmap)
    {
        if (can_fit_256_bits_sse(static_cast<const T*>(first2), count2))
        {
            alignas(32) scalar_table_t table = {};
            build_scalar_table_no_check<T>(first2, count2, table);
            return impl_last_scalar<T, Pred>(first1, count1, table);
        }
    }

    return impl<T, Pred>(first1, count1, first2, count2);

#else

    alignas(32) scalar_table_t table = {};
    if (build_scalar_table<T>(first2, count2, table))
    {
        return impl_last_scalar<T, Pred>(first1, count1, table);
    }

    return fallback<T, Pred>(first1, count1, first2, count2);

#endif // defined(USE_SSE2)
}

} // namespace _last_of
} // namespace _find_meow_of

//=============================================================================
// meow of functions
//=============================================================================

extern "C" {

const void* VX_STDCALL find_first_of_trivial_1(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _find_meow_of::_first_of::dispatch_ptr<uint8_t>(first1, last1, first2, last2);
}

const void* VX_STDCALL find_first_of_trivial_2(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _find_meow_of::_first_of::dispatch_ptr<uint16_t>(first1, last1, first2, last2);
}

const void* VX_STDCALL find_first_of_trivial_4(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _find_meow_of::_first_of::dispatch_ptr<uint32_t>(first1, last1, first2, last2);
}

const void* VX_STDCALL find_first_of_trivial_8(
    const void* const first1,
    const void* const last1,
    const void* const first2,
    const void* const last2) noexcept
{
    return _find_meow_of::_first_of::dispatch_ptr<uint64_t>(first1, last1, first2, last2);
}

VX_NO_ALIAS size_t VX_STDCALL find_first_of_trivial_pos_1(const void* const haystack,
    const size_t haystack_length,
    const void* const needle,
    const size_t needle_length) noexcept
{
    return _find_meow_of::_first_of::dispatch_pos<uint8_t, _find_meow_of::predicate::any_of>(
        haystack, haystack_length, needle, needle_length);
}

VX_NO_ALIAS size_t VX_STDCALL find_first_of_trivial_pos_2(const void* const haystack,
    const size_t haystack_length,
    const void* const needle,
    const size_t needle_length) noexcept
{
    return _find_meow_of::_first_of::dispatch_pos<uint16_t, _find_meow_of::predicate::any_of>(
        haystack, haystack_length, needle, needle_length);
}

VX_NO_ALIAS size_t VX_STDCALL find_first_of_trivial_pos_4(const void* const haystack,
    const size_t haystack_length,
    const void* const needle,
    const size_t needle_length) noexcept
{
    return _find_meow_of::_first_of::dispatch_pos<uint32_t, _find_meow_of::predicate::any_of>(
        haystack, haystack_length, needle, needle_length);
}

VX_NO_ALIAS size_t VX_STDCALL find_first_of_trivial_pos_8(const void* const haystack,
    const size_t haystack_length,
    const void* const needle,
    const size_t needle_length) noexcept
{
    return _find_meow_of::_first_of::dispatch_pos<uint64_t, _find_meow_of::predicate::any_of>(
        haystack, haystack_length, needle, needle_length);
}

VX_NO_ALIAS size_t VX_STDCALL find_last_of_trivial_pos_1(const void* const haystack,
    const size_t haystack_length,
    const void* const needle,
    const size_t needle_length) noexcept
{
    return _find_meow_of::_last_of::dispatch_pos<uint8_t, _find_meow_of::predicate::any_of>(
        haystack, haystack_length, needle, needle_length);
}

VX_NO_ALIAS size_t VX_STDCALL find_last_of_trivial_pos_2(const void* const haystack,
    const size_t haystack_length,
    const void* const needle,
    const size_t needle_length) noexcept
{
    return _find_meow_of::_last_of::dispatch_pos<uint16_t, _find_meow_of::predicate::any_of>(
        haystack, haystack_length, needle, needle_length);
}

VX_NO_ALIAS size_t VX_STDCALL find_first_not_of_trivial_pos_1(const void* const haystack,
    const size_t haystack_length,
    const void* const needle,
    const size_t needle_length) noexcept
{
    return _find_meow_of::_first_of::dispatch_pos<uint8_t, _find_meow_of::predicate::none_of>(
        haystack, haystack_length, needle, needle_length);
}

VX_NO_ALIAS size_t VX_STDCALL find_first_not_of_trivial_pos_2(const void* const haystack,
    const size_t haystack_length,
    const void* const needle,
    const size_t needle_length) noexcept
{
    return _find_meow_of::_first_of::dispatch_pos<uint16_t, _find_meow_of::predicate::none_of>(
        haystack, haystack_length, needle, needle_length);
}

VX_NO_ALIAS size_t VX_STDCALL find_last_not_of_trivial_pos_1(const void* const haystack,
    const size_t haystack_length,
    const void* const needle,
    const size_t needle_length) noexcept
{
    return _find_meow_of::_last_of::dispatch_pos<uint8_t, _find_meow_of::predicate::none_of>(
        haystack, haystack_length, needle, needle_length);
}

VX_NO_ALIAS size_t VX_STDCALL find_last_not_of_trivial_pos_2(const void* const haystack,
    const size_t haystack_length,
    const void* const needle,
    const size_t needle_length) noexcept
{
    return _find_meow_of::_last_of::dispatch_pos<uint16_t, _find_meow_of::predicate::none_of>(
        haystack, haystack_length, needle, needle_length);
}

} // extern "C"

} // namespace _simd
} // namespace vx
