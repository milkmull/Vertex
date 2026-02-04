#pragma once

#include "vertex_impl/std/simd_algorithms/simd_common.hpp"

#if defined(_VX_USE_SIMD_ALGORITHMS)

namespace vx {
namespace _simd {

//=============================================================================
// bitset to string impl
//=============================================================================

    #if !defined(USE_ARM_NEON)

namespace _bitset_to_string {

        #if defined(USE_AVX2)

struct traits_avx
{
    static void out(void* const dest, const __m256i elems) noexcept
    {
        _mm256_storeu_si256(static_cast<__m256i*>(dest), elems);
    }

    static void exit_vectorized() noexcept
    {
        _mm256_zeroupper();
    }
};

struct traits_1_avx : traits_avx
{
    using value_type = uint32_t;

    static __m256i set(const char val) noexcept
    {
        return _mm256_broadcastb_epi8(_mm_cvtsi32_si128(val));
    }

    static __m256i VX_FORCE_INLINE step(const uint32_t val, const __m256i px0, const __m256i px1) noexcept
    {
        const __m128i vx0 = _mm_cvtsi32_si128(val);
        const __m128i vx1 =
            _mm_shuffle_epi8(vx0, _mm_set_epi32(0x00000000, 0x01010101, 0x02020202, 0x03030303));
        const __m256i vx2 = _mm256_castsi128_si256(vx1);
        const __m256i vx3 = _mm256_permutevar8x32_epi32(vx2, _mm256_set_epi32(3, 3, 2, 2, 1, 1, 0, 0));
        const __m256i msk = _mm256_and_si256(vx3, _mm256_set1_epi64x(0x0102040810204080));
        const __m256i ex0 = _mm256_cmpeq_epi8(msk, _mm256_setzero_si256());
        const __m256i ex1 = _mm256_blendv_epi8(px1, px0, ex0);
        return ex1;
    }
};

struct traits_2_avx : traits_avx
{
    using value_type = uint16_t;

    static __m256i set(const wchar_t val) noexcept
    {
        return _mm256_broadcastw_epi16(_mm_cvtsi32_si128(val));
    }

    static __m256i VX_FORCE_INLINE step(const uint16_t val, const __m256i px0, const __m256i px1) noexcept
    {
        const __m128i vx0 = _mm_cvtsi32_si128(val);
        const __m128i vx1 =
            _mm_shuffle_epi8(vx0, _mm_set_epi32(0x00000000, 0x00000000, 0x01010101, 0x01010101));
        const __m256i vx2 = _mm256_castsi128_si256(vx1);
        const __m256i vx3 = _mm256_permute4x64_epi64(vx2, _MM_SHUFFLE(1, 1, 0, 0));
        const __m256i msk = _mm256_and_si256(vx3,
            _mm256_set_epi64x(0x0001000200040008, 0x0010002000400080, 0x0001000200040008, 0x0010002000400080));
        const __m256i ex0 = _mm256_cmpeq_epi16(msk, _mm256_setzero_si256());
        const __m256i ex1 = _mm256_blendv_epi8(px1, px0, ex0);
        return ex1;
    }
};

        #else

using traits_1_avx = void;
using traits_2_avx = void;

        #endif // defined(USE_AVX2)

        #if defined(USE_SSE2)

struct traits_sse
{
    static void out(void* const dest, const __m128i elems) noexcept
    {
        _mm_storeu_si128(static_cast<__m128i*>(dest), elems);
    }

    static void exit_vectorized() noexcept
    {}
};

struct traits_1_sse : traits_sse
{
    using value_type = uint16_t;

    static __m128i set(const char val) noexcept
    {
        return _mm_shuffle_epi8(_mm_cvtsi32_si128(val), _mm_setzero_si128());
    }

    static __m128i VX_FORCE_INLINE step(const uint16_t val, const __m128i px0, const __m128i px1) noexcept
    {
        const __m128i vx0 = _mm_cvtsi32_si128(val);
        const __m128i vx1 =
            _mm_shuffle_epi8(vx0, _mm_set_epi32(0x00000000, 0x00000000, 0x01010101, 0x01010101));
        const __m128i msk = _mm_and_si128(vx1, _mm_set1_epi64x(0x0102040810204080));
        const __m128i ex0 = _mm_cmpeq_epi8(msk, _mm_setzero_si128());
        const __m128i ex1 = _mm_blendv_epi8(px1, px0, ex0);
        return ex1;
    }
};

struct traits_2_sse : traits_sse
{
    using value_type = uint8_t;

    static __m128i set(const wchar_t val) noexcept
    {
        return _mm_set1_epi16(val);
    }

    static __m128i VX_FORCE_INLINE step(const uint8_t val, const __m128i px0, const __m128i px1) noexcept
    {
        const __m128i vx = _mm_set1_epi16(val);
        const __m128i msk = _mm_and_si128(vx, _mm_set_epi64x(0x0001000200040008, 0x0010002000400080));
        const __m128i ex0 = _mm_cmpeq_epi16(msk, _mm_setzero_si128());
        const __m128i ex1 = _mm_blendv_epi8(px1, px0, ex0);
        return ex1;
    }
};

        #else

using traits_1_sse = void;
using traits_2_sse = void;

        #endif // defined(USE_SSE2)

template <typename Traits, typename Elem>
void VX_STDCALL impl(
    Elem* const dest,
    const void* src,
    size_t size_bits,
    const Elem elem0,
    const Elem elem1) noexcept
{
    constexpr size_t step_size_bits = sizeof(typename Traits::value_type) * 8;

    const auto px0 = Traits::set(elem0);
    const auto px1 = Traits::set(elem1);
    if (size_bits >= step_size_bits)
    {
        Elem* pos = dest + size_bits;
        size_bits &= step_size_bits - 1;
        Elem* const stop_at = dest + size_bits;
        do
        {
            typename Traits::value_type val;
            ::memcpy(&val, src, sizeof(val));
            const auto elems = Traits::step(val, px0, px1);
            pos -= step_size_bits;
            Traits::out(pos, elems);
            advance_bytes(src, sizeof(val));
        } while (pos != stop_at);
    }

    if (size_bits > 0)
    {
        typename Traits::value_type val;
        ::memcpy(&val, src, sizeof(val));
        const auto elems = Traits::step(val, px0, px1);
        Elem tmp[step_size_bits];
        Traits::out(tmp, elems);
        const Elem* const tmpd = tmp + (step_size_bits - size_bits);
        ::memcpy(dest, tmpd, size_bits * sizeof(Elem));
    }

    Traits::exit_vectorized(); // TRANSITION, DevCom-10331414
}

template <typename AVX, typename SSE, typename Elem>
void VX_STDCALL dispatch(Elem* const dest, const void* const src, const size_t size_bits, const Elem elem0, const Elem elem1) noexcept
{
        #if defined(USE_X86)

            #if defined(USE_AVX2)

    if (size_bits >= 256)
    {
        impl<AVX>(dest, src, size_bits, elem0, elem1);
    }
    else

            #endif // defined(USE_AVX2)

            #if defined(USE_SSE2)

    {
        impl<SSE>(dest, src, size_bits, elem0, elem1);
    }

            #endif // defined(USE_SSE2)

        #else

    const auto arr = reinterpret_cast<const uint8_t*>(src);
    for (size_t ix = 0; ix < size_bits; ++ix)
    {
        dest[size_bits - 1 - ix] = ((arr[ix >> 3] >> (ix & 7)) & 1) != 0 ? elem1 : elem0;
    }

        #endif // defined(USE_X86)
}

} // namespace _bitset_to_string

//=============================================================================
// bitset to string functions
//=============================================================================

extern "C" {

VX_NO_ALIAS void VX_STDCALL bitset_to_string_1(
    char* const dest,
    const void* const src,
    const size_t size_bits,
    const char elem0,
    const char elem1) noexcept
{
    using namespace _bitset_to_string;
    dispatch<traits_1_avx, traits_1_sse>(dest, src, size_bits, elem0, elem1);
}

VX_NO_ALIAS void VX_STDCALL bitset_to_string_2(wchar_t* const dest, const void* const src, const size_t size_bits, const wchar_t elem0, const wchar_t elem1) noexcept
{
    using namespace _bitset_to_string;
    dispatch<traits_2_avx, traits_2_sse>(dest, src, size_bits, elem0, elem1);
}

} // extern "C"

//=============================================================================
// bitset from string impl
//=============================================================================

namespace _bitset_from_string {

        #if defined(USE_AVX2)

struct traits_avx
{
    using guard = zeroupper_on_exit;
    using vec = __m256i;

    static __m256i load(const void* src) noexcept
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
    }

    static void store(void* dest, const __m256i val) noexcept
    {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(dest), val);
    }

    static bool check(const __m256i val, const __m256i ex1, const __m256i dx0) noexcept
    {
        return _mm256_testc_si256(ex1, _mm256_xor_si256(val, dx0));
    }
};

struct traits_1_avx : traits_avx
{
    static __m256i set(const char val) noexcept
    {
        return _mm256_set1_epi8(val);
    }

    static uint32_t to_bits(const __m256i ex1) noexcept
    {
        const __m256i shuf = _mm256_set_epi8(                     //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

        const __m256i ex2 = _mm256_shuffle_epi8(ex1, shuf);
        return _rotl(static_cast<uint32_t>(_mm256_movemask_epi8(ex2)), 16);
    }

    static __m256i cmp(const __m256i val, const __m256i dx1) noexcept
    {
        return _mm256_cmpeq_epi8(val, dx1);
    }
};

struct traits_2_avx : traits_avx
{
    static __m256i set(const wchar_t val) noexcept
    {
        return _mm256_set1_epi16(val);
    }

    static uint16_t to_bits(const __m256i ex1) noexcept
    {
        const __m256i shuf = _mm256_set_epi8(                               //
            +0, +2, +4, +6, +8, 10, 12, 14, -1, -1, -1, -1, -1, -1, -1, -1, //
            -1, -1, -1, -1, -1, -1, -1, -1, +0, +2, +4, +6, +8, 10, 12, 14);

        const __m256i ex2 = _mm256_shuffle_epi8(ex1, shuf);
        return static_cast<uint16_t>(_rotl(static_cast<uint32_t>(_mm256_movemask_epi8(ex2)), 8));
    }

    static __m256i cmp(const __m256i val, const __m256i dx1) noexcept
    {
        return _mm256_cmpeq_epi16(val, dx1);
    }
};

        #else

using traits_1_avx = void;
using traits_2_avx = void;

        #endif // defined(USE_AVX2)

        #if defined(USE_SSE2)

struct traits_sse
{
    using guard = char;
    using vec = __m128i;

    static __m128i load(const void* src) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
    }

    static void store(void* dest, const __m128i val) noexcept
    {
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dest), val);
    }

    static bool check(const __m128i val, const __m128i ex1, const __m128i dx0) noexcept
    {
        return _mm_testc_si128(ex1, _mm_xor_si128(val, dx0));
    }
};

struct traits_1_sse : traits_sse
{
    static __m128i set(const char val) noexcept
    {
        return _mm_shuffle_epi8(_mm_cvtsi32_si128(val), _mm_setzero_si128());
    }

    static uint16_t to_bits(const __m128i ex1) noexcept
    {
        const __m128i shuf = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        const __m128i ex2 = _mm_shuffle_epi8(ex1, shuf);
        return static_cast<uint16_t>(_mm_movemask_epi8(ex2));
    }

    static __m128i cmp(const __m128i val, const __m128i dx1) noexcept
    {
        return _mm_cmpeq_epi8(val, dx1);
    }
};

struct traits_2_sse : traits_sse
{
    static __m128i set(const wchar_t val) noexcept
    {
        return _mm_set1_epi16(val);
    }

    static uint8_t to_bits(const __m128i ex1) noexcept
    {
        const __m128i shuf = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 0, 2, 4, 6, 8, 10, 12, 14);
        const __m128i ex2 = _mm_shuffle_epi8(ex1, shuf);
        return static_cast<uint8_t>(_mm_movemask_epi8(ex2));
    }

    static __m128i cmp(const __m128i val, const __m128i dx1) noexcept
    {
        return _mm_cmpeq_epi16(val, dx1);
    }
};

        #else

using traits_1_sse = void;
using traits_2_sse = void;

        #endif // defined(USE_SSE2)

template <typename Traits, typename Elem, typename Out_Fn>
bool loop(const Elem* const src, const Elem* src_end, const typename Traits::vec dx0, const typename Traits::vec dx1, Out_Fn out) noexcept
{
    for (;;)
    {
        typename Traits::vec val;
        constexpr size_t per_vec = sizeof(val) / sizeof(Elem);

        if (const size_t left = src_end - src; left >= per_vec)
        {
            src_end -= per_vec;
            val = Traits::load(src_end);
        }
        else if (left == 0)
        {
            return true;
        }
        else
        {
            src_end = src;
            Elem tmp[per_vec];
            Traits::store(tmp, dx0);
            Elem* const tmpd = tmp + (per_vec - left);
            ::memcpy(tmpd, src_end, left * sizeof(Elem));
            val = Traits::load(tmp);
        }

        const auto ex1 = Traits::cmp(val, dx1);

        if (!Traits::check(val, ex1, dx0))
        {
            return false;
        }

        out(ex1);
    }
}

template <typename Traits, typename Elem>
bool impl(void* dest, const Elem* const src, const size_t size_bytes, const size_t size_bits, const size_t size_chars, const Elem elem0, const Elem elem1) noexcept
{
    VX_MAYBE_UNUSED typename Traits::guard guard; // TRANSITION, DevCom-10331414
    const auto dx0 = Traits::set(elem0);
    const auto dx1 = Traits::set(elem1);

    void* dest_end = dest;
    advance_bytes(dest_end, size_bytes);

    auto out = [&dest](const typename Traits::vec ex1)
    {
        const auto val = Traits::to_bits(ex1);
        ::memcpy(dest, &val, sizeof(val));
        advance_bytes(dest, sizeof(val));
    };

    const size_t size_convert = (size_chars <= size_bits) ? size_chars : size_bits;

    // Convert characters to bits
    if (!loop<Traits>(src, src + size_convert, dx0, dx1, out))
    {
        return false;
    }

    // Verify remaining characters, if any
    if (size_convert != size_chars && !loop<Traits>(src + size_convert, src + size_chars, dx0, dx1, [](typename Traits::vec) {}))
    {
        return false;
    }

    // Trim tail (may be padding tail, or too short string, or both)
    if (dest != dest_end)
    {
        ::memset(dest, 0, byte_length(dest, dest_end));
    }

    return true;
}

template <typename Elem>
bool fallback(void* const dest, const Elem* const src, const size_t size_bytes, const size_t size_bits, const size_t size_chars, const Elem elem0, const Elem elem1) noexcept
{
    const auto dest_bytes = static_cast<uint8_t*>(dest);
    size_t size_convert = size_chars;

    if (size_chars > size_bits)
    {
        size_convert = size_bits;

        for (size_t ix = size_bits; ix < size_chars; ++ix)
        {
            if (const Elem cur = src[ix]; cur != elem0 && cur != elem1)
            {
                return false;
            }
        }
    }

    ::memset(dest, 0, size_bytes);

    for (size_t ix = 0; ix != size_convert; ++ix)
    {
        const Elem cur = src[size_convert - ix - 1];

        if (cur != elem0 && cur != elem1)
        {
            return false;
        }

        dest_bytes[ix >> 3] |= static_cast<uint8_t>(cur == elem1) << (ix & 0x7);
    }

    return true;
}

template <typename AVX, typename SSE, typename Elem>
bool dispatch(void* dest, const Elem* src, size_t size_bytes, size_t size_bits, size_t size_chars, Elem elem0, Elem elem1) noexcept
{
        #if defined(USE_X86)

            #if defined(USE_AVX2)

    if (size_bits >= 256)
    {
        return impl<AVX>(dest, src, size_bytes, size_bits, size_chars, elem0, elem1);
    }
    else

            #endif // defined(USE_AVX2)

            #if defined(USE_SSE2)

    {
        return impl<SSE>(dest, src, size_bytes, size_bits, size_chars, elem0, elem1);
    }

            #endif // defined(USE_SSE2)

        #else

    return fallback(dest, src, size_bytes, size_bits, size_chars, elem0, elem1);

        #endif // defined(USE_X86)
}
} // namespace _bitset_from_string

//=============================================================================
// bitset from string functions
//=============================================================================

extern "C" {

VX_NO_ALIAS bool VX_STDCALL bitset_from_string_1(void* const dest, const char* const src, const size_t size_bytes, const size_t size_bits, const size_t size_chars, const char elem0, const char elem1) noexcept
{
    using namespace _bitset_from_string;

    return dispatch<traits_1_avx, traits_1_sse>(dest, src, size_bytes, size_bits, size_chars, elem0, elem1);
}

VX_NO_ALIAS bool VX_STDCALL bitset_from_string_2(void* const dest, const wchar_t* const src, const size_t size_bytes, const size_t size_bits, const size_t size_chars, const wchar_t elem0, const wchar_t elem1) noexcept
{
    using namespace _bitset_from_string;

    return dispatch<traits_2_avx, traits_2_sse>(dest, src, size_bytes, size_bits, size_chars, elem0, elem1);
}

} // extern "C"

    #endif // !defined(USE_ARM_NEON)

#endif // defined(_VX_USE_SIMD_ALGORITHMS)

} // namespace _simd
} // namespace vx
