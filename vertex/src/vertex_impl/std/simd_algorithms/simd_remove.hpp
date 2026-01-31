#pragma once

#include "vertex_impl/std/simd_algorithms/simd_find.hpp"

namespace vx {
namespace _simd {

//=============================================================================
// remove impl
//=============================================================================

namespace _remove {

template <typename T>
void* remove_fallback(
    const void* const first,
    const void* const last,
    void* const out,
    const T val) noexcept
{
    const T* src = reinterpret_cast<const T*>(first);
    T* dest = reinterpret_cast<T*>(out);

    while (src != last)
    {
        if (*src != val)
        {
            *dest = *src;
            ++dest;
        }

        ++src;
    }

    return dest;
}

template <typename T>
void* unique_fallback(const void* const first, const void* const last, void* const dest) noexcept
{
    T* out = reinterpret_cast<T*>(dest);
    const T* src = reinterpret_cast<const T*>(first);

    while (src != last)
    {
        if (*src != *out)
        {
            ++out;
            *out = *src;
        }

        ++src;
    }

    ++out;
    return out;
}

template <size_t size_v, size_t size_h>
struct tables
{
    uint8_t shuf[size_v][size_h];
    uint8_t size[size_v];
};

template <size_t size_v, size_t size_h>
constexpr auto make_tables(const uint32_t mul, const uint32_t ew)
{
    tables<size_v, size_h> result = {};

    for (uint32_t vx = 0; vx != size_v; ++vx)
    {
        uint32_t nx = 0;

        // Make shuffle mask for pshufb / vpermd corresponding to vx bit value.
        // Every bit set corresponds to an element skipped.
        for (uint32_t hx = 0; hx != size_h / ew; ++hx)
        {
            if ((vx & (1 << hx)) == 0)
            {
                // Inner loop needed for cases where the shuffle mask operates on element parts rather than
                // whole elements; for whole elements there would be one iteration.
                for (uint32_t ex = 0; ex != ew; ++ex)
                {
                    result.shuf[vx][nx * ew + ex] = static_cast<uint8_t>(hx * ew + ex);
                }
                ++nx;
            }
        }

        // Size of elements that are not removed in bytes.
        result.size[vx] = static_cast<uint8_t>(nx * mul);

        // Fill the remaining with arbitrary elements.
        // It is not possible to leave them untouched while keeping this optimization efficient.
        // This should not be a problem though, as they should be either overwritten by the next step,
        // or left in the removed range.
        for (; nx != size_h / ew; ++nx)
        {
            // Inner loop needed for cases where the shuffle mask operates on element parts rather than whole
            // elements; for whole elements there would be one iteration.
            for (uint32_t ex = 0; ex != ew; ++ex)
            {
                result.shuf[vx][nx * ew + ex] = static_cast<uint8_t>(nx * ew + ex);
            }
        }
    }

    return result;
}

#if defined(USE_AVX2)

constexpr auto tables_4_avx = make_tables<256, 8>(4, 1);
constexpr auto tables_8_avx = make_tables<16, 8>(8, 2);

#endif // defined(USE_AVX2)

#if defined(USE_SSE2)

constexpr auto tables_1_sse = make_tables<256, 8>(1, 1);
constexpr auto tables_2_sse = make_tables<256, 16>(2, 2);
constexpr auto tables_4_sse = make_tables<16, 16>(4, 4);
constexpr auto tables_8_sse = make_tables<4, 16>(8, 8);

#endif // defined(USE_SSE2)

#if defined(USE_AVX2)

struct avx_4
{
    static constexpr size_t elem_size = 4;
    static constexpr size_t step = 32;

    static __m256i set(const uint32_t val) noexcept
    {
        return _mm256_set1_epi32(val);
    }

    static __m256i load(const void* const ptr) noexcept
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
    }

    static uint32_t mask(const __m256i first, const __m256i second) noexcept
    {
        const __m256i mask = _mm256_cmpeq_epi32(first, second);
        return _mm256_movemask_ps(_mm256_castsi256_ps(mask));
    }

    static void* store_masked(void* out, const __m256i src, const uint32_t bingo) noexcept
    {
        const __m256i shuf = _mm256_cvtepu8_epi32(_mm_loadu_si64(tables_4_avx.shuf[bingo]));
        const __m256i dest = _mm256_permutevar8x32_epi32(src, shuf);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(out), dest);
        advance_bytes(out, tables_4_avx.size[bingo]);
        return out;
    }
};

struct avx_8
{
    static constexpr size_t elem_size = 8;
    static constexpr size_t step = 32;

    static __m256i set(const uint64_t val) noexcept
    {
        return _mm256_set1_epi64x(val);
    }

    static __m256i load(const void* const ptr) noexcept
    {
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
    }

    static uint32_t mask(const __m256i first, const __m256i second) noexcept
    {
        const __m256i mask = _mm256_cmpeq_epi64(first, second);
        return _mm256_movemask_pd(_mm256_castsi256_pd(mask));
    }

    static void* store_masked(void* out, const __m256i src, const uint32_t bingo) noexcept
    {
        const __m256i shuf = _mm256_cvtepu8_epi32(_mm_loadu_si64(tables_8_avx.shuf[bingo]));
        const __m256i dest = _mm256_permutevar8x32_epi32(src, shuf);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(out), dest);
        advance_bytes(out, tables_8_avx.size[bingo]);
        return out;
    }
};

#endif // defined(USE_AVX2)

#if defined(USE_SSE2)

struct sse_1
{
    static constexpr size_t elem_size = 1;
    static constexpr size_t step = 8;

    static __m128i set(const uint8_t val) noexcept
    {
        return _mm_shuffle_epi8(_mm_cvtsi32_si128(val), _mm_setzero_si128());
    }

    static __m128i load(const void* const ptr) noexcept
    {
        return _mm_loadu_si64(ptr);
    }

    static uint32_t mask(const __m128i first, const __m128i second) noexcept
    {
        return _mm_movemask_epi8(_mm_cmpeq_epi8(first, second)) & 0xFF;
    }

    static void* store_masked(void* out, const __m128i src, const uint32_t bingo) noexcept
    {
        const __m128i shuf = _mm_loadu_si64(tables_1_sse.shuf[bingo]);
        const __m128i dest = _mm_shuffle_epi8(src, shuf);
        _mm_storeu_si64(out, dest);
        advance_bytes(out, tables_1_sse.size[bingo]);
        return out;
    }
};

struct sse_2
{
    static constexpr size_t elem_size = 2;
    static constexpr size_t step = 16;

    static __m128i set(const uint16_t val) noexcept
    {
        return _mm_set1_epi16(val);
    }

    static __m128i load(const void* const ptr) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
    }

    static uint32_t mask(const __m128i first, const __m128i second) noexcept
    {
        const __m128i mask = _mm_cmpeq_epi16(first, second);
        return _mm_movemask_epi8(_mm_packs_epi16(mask, _mm_setzero_si128()));
    }

    static void* store_masked(void* out, const __m128i src, const uint32_t bingo) noexcept
    {
        const __m128i shuf = _mm_loadu_si128(reinterpret_cast<const __m128i*>(tables_2_sse.shuf[bingo]));
        const __m128i dest = _mm_shuffle_epi8(src, shuf);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out), dest);
        advance_bytes(out, tables_2_sse.size[bingo]);
        return out;
    }
};

struct sse_4
{
    static constexpr size_t elem_size = 4;
    static constexpr size_t step = 16;

    static __m128i set(const uint32_t val) noexcept
    {
        return _mm_set1_epi32(val);
    }

    static __m128i load(const void* const ptr) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
    }

    static uint32_t mask(const __m128i first, const __m128i second) noexcept
    {
        const __m128i mask = _mm_cmpeq_epi32(first, second);
        return _mm_movemask_ps(_mm_castsi128_ps(mask));
    }

    static void* store_masked(void* out, const __m128i src, const uint32_t bingo) noexcept
    {
        const __m128i shuf = _mm_loadu_si128(reinterpret_cast<const __m128i*>(tables_4_sse.shuf[bingo]));
        const __m128i dest = _mm_shuffle_epi8(src, shuf);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out), dest);
        advance_bytes(out, tables_4_sse.size[bingo]);
        return out;
    }
};

struct sse_8
{
    static constexpr size_t elem_size = 8;
    static constexpr size_t step = 16;

    static __m128i set(const uint64_t val) noexcept
    {
        return _mm_set1_epi64x(val);
    }

    static __m128i load(const void* const ptr) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
    }

    static uint32_t mask(const __m128i first, const __m128i second) noexcept
    {
        const __m128i mask = _mm_cmpeq_epi64(first, second);
        return _mm_movemask_pd(_mm_castsi128_pd(mask));
    }

    static void* store_masked(void* out, const __m128i src, const uint32_t bingo) noexcept
    {
        const __m128i shuf = _mm_loadu_si128(reinterpret_cast<const __m128i*>(tables_8_sse.shuf[bingo]));
        const __m128i dest = _mm_shuffle_epi8(src, shuf);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out), dest);
        advance_bytes(out, tables_8_sse.size[bingo]);
        return out;
    }
};

#endif // defined(USE_SSE2)

constexpr size_t copy_buffer_size = 512;

template <typename Traits, typename T>
void* remove_impl(void* first, void* const stop, const T val) noexcept
{
    void* out = first;
    const auto match = Traits::set(val);

    do
    {
        const auto src = Traits::load(first);
        const uint32_t bingo = Traits::mask(src, match);
        out = Traits::store_masked(out, src, bingo);
        advance_bytes(first, Traits::step);
    } while (first != stop);

    return out;
}

template <typename Traits, typename T>
void* remove_copy_impl(const void* first, const void* const stop, void* out, const T val) noexcept
{
    unsigned char buffer[copy_buffer_size];
    void* buffer_out = buffer;
    void* const buffer_stop = buffer + copy_buffer_size - Traits::step;

    const auto match = Traits::set(val);

    do
    {
        const auto src = Traits::load(first);
        const uint32_t bingo = Traits::mask(src, match);
        buffer_out = Traits::store_masked(buffer_out, src, bingo);
        advance_bytes(first, Traits::step);

        if (buffer_out >= buffer_stop)
        {
            const size_t fill = byte_length(buffer, buffer_out);
            ::memcpy(out, buffer, fill);
            advance_bytes(out, fill);
            buffer_out = buffer;
        }
    } while (first != stop);

    const size_t fill = byte_length(buffer, buffer_out);
    ::memcpy(out, buffer, fill);
    advance_bytes(out, fill);
    return out;
}

template <typename Traits>
void* unique_impl(void* first, void* const stop) noexcept
{
    void* out = first;

    do
    {
        const auto src = Traits::load(first);
        void* first_d = first;
        rewind_bytes(first_d, Traits::elem_size);
        const auto match = Traits::load(first_d);
        const uint32_t bingo = Traits::mask(src, match);
        out = Traits::store_masked(out, src, bingo);
        advance_bytes(first, Traits::step);
    } while (first != stop);

    rewind_bytes(out, Traits::elem_size);
    return out;
}

template <typename Traits>
void* unique_copy_impl(const void* first, const void* const stop, void* out) noexcept
{
    unsigned char buffer[copy_buffer_size];
    void* buffer_out = buffer;
    void* const buffer_stop = buffer + copy_buffer_size - Traits::step;

    do
    {
        const auto src = Traits::load(first);
        const void* first_d = first;
        rewind_bytes(first_d, Traits::elem_size);
        const auto match = Traits::load(first_d);
        const uint32_t bingo = Traits::mask(src, match);
        buffer_out = Traits::store_masked(buffer_out, src, bingo);
        advance_bytes(first, Traits::step);

        if (buffer_out >= buffer_stop)
        {
            const size_t fill = byte_length(buffer, buffer_out);
            ::memcpy(static_cast<unsigned char*>(out) + Traits::elem_size, buffer, fill);
            advance_bytes(out, fill);
            buffer_out = buffer;
        }
    } while (first != stop);

    const size_t fill = byte_length(buffer, buffer_out);
    ::memcpy(static_cast<unsigned char*>(out) + Traits::elem_size, buffer, fill);
    advance_bytes(out, fill);
    return out;
}

} // namespace _remove

//=============================================================================
// remove functions
//=============================================================================

extern "C" {

void* VX_STDCALL remove_1(void* first, void* const last, const uint8_t val) noexcept
{
    void* out = first;

#if defined(USE_SSE2)

    if (const size_t size_bytes = byte_length(first, last); size_bytes >= 8)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 7 });
        out = _remove::remove_impl<_remove::sse_1>(first, stop, val);
        first = stop;
    }

#endif // defined(USE_SSE2)

    return _remove::remove_fallback(first, last, out, val);
}

void* VX_STDCALL remove_2(void* first, void* const last, const uint16_t val) noexcept
{
    void* out = first;

#if defined(USE_SSE2)

    if (const size_t size_bytes = byte_length(first, last); size_bytes >= 16)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        out = _remove::remove_impl<_remove::sse_2>(first, stop, val);
        first = stop;
    }

#endif // defined(USE_SSE2)

    return _remove::remove_fallback(first, last, out, val);
}

void* VX_STDCALL remove_4(void* first, void* const last, const uint32_t val) noexcept
{
    void* out = first;

#if defined(USE_X86)

    const size_t size_bytes = byte_length(first, last);

    #if defined(USE_AVX2)

    if (size_bytes >= 32)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0x1F });
        out = _remove::remove_impl<_remove::_Avx_4>(first, stop, val);
        first = stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }
    else

    #elif defined(USE_SSE2)

    if (size_bytes >= 16)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        out = _remove::remove_impl<_remove::sse_4>(first, stop, val);
        first = stop;
    }

    #endif

#endif // defined(USE_X86)

        return _remove::remove_fallback(first, last, out, val);
}

void* VX_STDCALL remove_8(void* first, void* const last, const uint64_t val) noexcept
{
    void* out = first;

#if defined(USE_X86)

    const size_t size_bytes = byte_length(first, last);

    #if defined(USE_AVX2)

    if (size_bytes >= 32)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0x1F });
        out = _remove::remove_impl<_remove::_Avx_8>(first, stop, val);
        first = stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }
    else

    #elif defined(USE_SSE2)

    if (size_bytes >= 16)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        out = _remove::remove_impl<_remove::sse_8>(first, stop, val);
        first = stop;
    }

    #endif

#endif // defined(USE_X86)

        return _remove::remove_fallback(first, last, out, val);
}

void* VX_STDCALL remove_copy_1(
    const void* first,
    const void* const last,
    void* out,
    const uint8_t val) noexcept
{
#if defined(USE_SSE2)

    if (const size_t size_bytes = byte_length(first, last); size_bytes >= 8)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 7 });
        out = _remove::remove_copy_impl<_remove::sse_1>(first, stop, out, val);
        first = stop;
    }

#endif // defined(USE_SSE2)

    return _remove::remove_fallback(first, last, out, val);
}

void* VX_STDCALL remove_copy_2(
    const void* first,
    const void* const last,
    void* out,
    const uint16_t val) noexcept
{
#if defined(USE_SSE2)

    if (const size_t size_bytes = byte_length(first, last); size_bytes >= 16)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        out = _remove::remove_copy_impl<_remove::sse_2>(first, stop, out, val);
        first = stop;
    }

#endif // defined(USE_SSE2)

    return _remove::remove_fallback(first, last, out, val);
}

void* VX_STDCALL remove_copy_4(
    const void* first,
    const void* const last,
    void* out,
    const uint32_t val) noexcept
{
#if defined(USE_X86)

    const size_t size_bytes = byte_length(first, last);

    #if defined(USE_AVX2)

    if (size_bytes >= 32)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0x1F });
        out = _remove::remove_copy_impl<_remove::_Avx_4>(first, stop, out, val);
        first = stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }
    else

    #elif defined(USE_SSE2)

    if (size_bytes >= 16)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        out = _remove::remove_copy_impl<_remove::sse_4>(first, stop, out, val);
        first = stop;
    }

    #endif

#endif // defined(USE_X86)

        return _remove::remove_fallback(first, last, out, val);
}

void* VX_STDCALL remove_copy_8(
    const void* first,
    const void* const last,
    void* out,
    const uint64_t val) noexcept
{
#if defined(USE_X86)

    const size_t size_bytes = byte_length(first, last);

    #if defined(USE_AVX2)

    if (size_bytes >= 32)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0x1F });
        out = _remove::remove_copy_impl<_remove::_Avx_8>(first, stop, out, val);
        first = stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }
    else

    #elif defined(USE_SSE2)

    if (size_bytes >= 16)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        out = _remove::remove_copy_impl<_remove::sse_8>(first, stop, out, val);
        first = stop;
    }

    #endif

#endif // defined(USE_X86)

        return _remove::remove_fallback(first, last, out, val);
}

void* VX_STDCALL unique_1(void* first, void* const last) noexcept
{
    first = const_cast<void*>(adjacent_find_1(first, last));

    if (first == last)
    {
        return first;
    }

    void* dest = first;
    advance_bytes(first, 1);

#if defined(USE_SSE2)

    if (const size_t size_bytes = byte_length(first, last); size_bytes >= 8)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 7 });
        dest = _remove::unique_impl<_remove::sse_1>(first, stop);
        first = stop;
    }

#endif // defined(USE_SSE2)

    return _remove::unique_fallback<uint8_t>(first, last, dest);
}

void* VX_STDCALL unique_2(void* first, void* const last) noexcept
{
    first = const_cast<void*>(adjacent_find_2(first, last));

    if (first == last)
    {
        return first;
    }

    void* dest = first;
    advance_bytes(first, 2);

#if defined(USE_SSE2)

    if (const size_t size_bytes = byte_length(first, last); size_bytes >= 16)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        dest = _remove::unique_impl<_remove::sse_2>(first, stop);
        first = stop;
    }

#endif // defined(USE_SSE2)

    return _remove::unique_fallback<uint16_t>(first, last, dest);
}

void* VX_STDCALL unique_4(void* first, void* const last) noexcept
{
    first = const_cast<void*>(adjacent_find_4(first, last));

    if (first == last)
    {
        return first;
    }

    void* dest = first;
    advance_bytes(first, 4);

#if defined(USE_X86)

    const size_t size_bytes = byte_length(first, last);

    #if defined(USE_AVX2)

    if (size_bytes >= 32)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0x1F });
        dest = _remove::unique_impl<_remove::_Avx_4>(first, stop);
        first = stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }
    else

    #elif defined(USE_SSE2)

    if (size_bytes >= 16)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        dest = _remove::unique_impl<_remove::sse_4>(first, stop);
        first = stop;
    }

    #endif

#endif // defined(USE_X86)

        return _remove::unique_fallback<uint32_t>(first, last, dest);
}

void* VX_STDCALL unique_8(void* first, void* const last) noexcept
{
    first = const_cast<void*>(adjacent_find_8(first, last));

    if (first == last)
    {
        return first;
    }

    void* dest = first;
    advance_bytes(first, 8);

#if defined(USE_X86)

    const size_t size_bytes = byte_length(first, last);

    #if defined(USE_AVX2)

    if (size_bytes >= 32)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0x1F });
        dest = _remove::unique_impl<_remove::_Avx_8>(first, stop);
        first = stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }
    else

    #elif defined(USE_SSE2)

    if (size_bytes >= 16)
    {
        void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        dest = _remove::unique_impl<_remove::sse_8>(first, stop);
        first = stop;
    }

    #endif

#endif // defined(USE_X86)

        return _remove::unique_fallback<uint64_t>(first, last, dest);
}

void* VX_STDCALL unique_copy_1(const void* first, const void* const last, void* dest) noexcept
{
    if (first == last)
    {
        return dest;
    }

    ::memcpy(dest, first, 1);
    advance_bytes(first, 1);

#if defined(USE_SSE2)

    if (const size_t size_bytes = byte_length(first, last); size_bytes >= 8)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 7 });
        dest = _remove::unique_copy_impl<_remove::sse_1>(first, stop, dest);
        first = stop;
    }

#endif // defined(USE_SSE2)

    return _remove::unique_fallback<uint8_t>(first, last, dest);
}

void* VX_STDCALL unique_copy_2(const void* first, const void* const last, void* dest) noexcept
{
    if (first == last)
    {
        return dest;
    }

    ::memcpy(dest, first, 2);
    advance_bytes(first, 2);

#if defined(USE_SSE2)

    if (const size_t size_bytes = byte_length(first, last); size_bytes >= 16)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        dest = _remove::unique_copy_impl<_remove::sse_2>(first, stop, dest);
        first = stop;
    }

#endif // defined(USE_SSE2)

    return _remove::unique_fallback<uint16_t>(first, last, dest);
}

void* VX_STDCALL unique_copy_4(const void* first, const void* const last, void* dest) noexcept
{
    if (first == last)
    {
        return dest;
    }

    ::memcpy(dest, first, 4);
    advance_bytes(first, 4);

#if defined(USE_X86)

    const size_t size_bytes = byte_length(first, last);

    #if defined(USE_AVX2)

    if (size_bytes >= 32)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0x1F });
        dest = _remove::unique_copy_impl<_remove::_Avx_4>(first, stop, dest);
        first = stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }
    else

    #elif defined(USE_SSE2)

    if (size_bytes >= 16)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        dest = _remove::unique_copy_impl<_remove::sse_4>(first, stop, dest);
        first = stop;
    }

    #endif

#endif // defined(USE_X86)

        return _remove::unique_fallback<uint32_t>(first, last, dest);
}

void* VX_STDCALL unique_copy_8(const void* first, const void* const last, void* dest) noexcept
{
    if (first == last)
    {
        return dest;
    }

    ::memcpy(dest, first, 8);
    advance_bytes(first, 8);

#if defined(USE_X86)

    const size_t size_bytes = byte_length(first, last);

    #if defined(USE_AVX2)

    if (size_bytes >= 32)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0x1F });
        dest = _remove::unique_copy_impl<_remove::_Avx_8>(first, stop, dest);
        first = stop;

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414
    }
    else

    #elif defined(USE_SSE2)

    if (size_bytes >= 16)
    {
        const void* stop = first;
        advance_bytes(stop, size_bytes & ~size_t{ 0xF });
        dest = _remove::unique_copy_impl<_remove::sse_8>(first, stop, dest);
        first = stop;
    }

    #endif

#endif // defined(USE_X86)

        return _remove::unique_fallback<uint64_t>(first, last, dest);
}

} // extern "C"

} // namespace _simd
} // namespace vx
