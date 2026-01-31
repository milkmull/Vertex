#pragma once

#include "vertex_impl/std/simd_algorithms/simd_find.hpp"

namespace vx {
namespace _simd {

//=============================================================================
// replace impl
//=============================================================================

namespace _replace {

template <typename Traits, typename T>
VX_NO_ALIAS void VX_STDCALL replace_copy_impl(
    const void* first,
    const void* const last,
    void* dest,
    const T old_val,
    const T new_val) noexcept
{
    const size_t size_bytes = byte_length(first, last);

#if defined(USE_AVX2)

    if (const size_t avx_size = size_bytes & ~size_t{ 0x1F }; avx_size != 0)
    {
        const __m256i comparand = Traits::set_avx(old_val);
        const __m256i replacement = Traits::set_avx(new_val);
        const void* stop_at = first;
        advance_bytes(stop_at, avx_size);

        do
        {
            const __m256i data = _mm256_loadu_si256(static_cast<const __m256i*>(first));
            const __m256i mask = Traits::cmp_avx(data, comparand);
            const __m256i val = _mm256_blendv_epi8(data, replacement, mask);

            _mm256_storeu_si256(static_cast<__m256i*>(dest), val);

            advance_bytes(first, 32);
            advance_bytes(dest, 32);
        } while (first != stop_at);

        if (const size_t avx_tail_size = size_bytes & 0x1C; avx_tail_size != 0)
        {
            const __m256i tail_mask = avx2_tail_mask_32(avx_tail_size);
            const __m256i data = _mm256_maskload_epi32(static_cast<const int*>(first), tail_mask);
            const __m256i mask = Traits::cmp_avx(data, comparand);
            const __m256i val = _mm256_blendv_epi8(data, replacement, mask);

            _mm256_maskstore_epi32(static_cast<int*>(dest), tail_mask, val);

            advance_bytes(first, avx_tail_size);
            advance_bytes(dest, avx_tail_size);
        }

        _mm256_zeroupper(); // TRANSITION, DevCom-10331414

        VX_IF_CONSTEXPR (sizeof(T) >= 4)
        {
            return;
        }
    }

#elif defined(USE_SSE2)

    if (const size_t sse_size = size_bytes & ~size_t{ 0xF }; sse_size != 0)
    {
        const __m128i comparand = Traits::set_sse(old_val);
        const __m128i replacement = Traits::set_sse(new_val);
        const void* stop_at = first;
        advance_bytes(stop_at, sse_size);

        do
        {
            const __m128i data = _mm_loadu_si128(static_cast<const __m128i*>(first));
            const __m128i mask = Traits::cmp_sse(data, comparand);
            const __m128i val = _mm_blendv_epi8(data, replacement, mask);

            _mm_storeu_si128(static_cast<__m128i*>(dest), val);

            advance_bytes(first, 16);
            advance_bytes(dest, 16);
        } while (first != stop_at);
    }

#endif

    auto ptr_dest = static_cast<T*>(dest);
    for (auto ptr_src = static_cast<const T*>(first); ptr_src != last; ++ptr_src)
    {
        const T val = *ptr_src;
        *ptr_dest = val == old_val ? new_val : val;
        ++ptr_dest;
    }
}

} // namespace _replace

//=============================================================================
// replace functions
//=============================================================================

extern "C" {

VX_NO_ALIAS void VX_STDCALL replace_4(
    void* first,
    void* const last,
    const uint32_t old_val,
    const uint32_t new_val) noexcept
{
#if defined(USE_AVX2)

    const __m256i comparand = _mm256_broadcastd_epi32(_mm_cvtsi32_si128(old_val));
    const __m256i replacement = _mm256_broadcastd_epi32(_mm_cvtsi32_si128(new_val));
    const size_t full_length = byte_length(first, last);

    void* stop_at = first;
    advance_bytes(stop_at, full_length & ~size_t{ 0x1F });

    while (first != stop_at)
    {
        const __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first));
        const __m256i mask = _mm256_cmpeq_epi32(comparand, data);
        _mm256_maskstore_epi32(reinterpret_cast<int*>(first), mask, replacement);

        advance_bytes(first, 32);
    }

    if (const size_t tail_length = full_length & 0x1C; tail_length != 0)
    {
        const __m256i tail_mask = avx2_tail_mask_32(tail_length);
        const __m256i data = _mm256_maskload_epi32(reinterpret_cast<const int*>(first), tail_mask);
        const __m256i mask = _mm256_and_si256(_mm256_cmpeq_epi32(comparand, data), tail_mask);
        _mm256_maskstore_epi32(reinterpret_cast<int*>(first), mask, replacement);
    }

    _mm256_zeroupper(); // TRANSITION, DevCom-10331414

#else

    for (auto cur = reinterpret_cast<uint32_t*>(first); cur != last; ++cur)
    {
        if (*cur == old_val)
        {
            *cur = new_val;
        }
    }

#endif // defined(USE_AVX2)
}

VX_NO_ALIAS void VX_STDCALL replace_8(
    void* first,
    void* const last,
    const uint64_t old_val,
    const uint64_t new_val) noexcept
{
#if defined(USE_AVX2)

    #if defined(VX_ARCH_WORD_BITS_64)

    const __m256i comparand = _mm256_broadcastq_epi64(_mm_cvtsi64_si128(old_val));
    const __m256i replacement = _mm256_broadcastq_epi64(_mm_cvtsi64_si128(new_val));

    #else

    const __m256i comparand = _mm256_set1_epi64x(old_val);
    const __m256i replacement = _mm256_set1_epi64x(new_val);

    #endif // defined(VX_ARCH_WORD_BITS_64)

    const size_t full_length = byte_length(first, last);

    void* stop_at = first;
    advance_bytes(stop_at, full_length & ~size_t{ 0x1F });

    while (first != stop_at)
    {
        const __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first));
        const __m256i mask = _mm256_cmpeq_epi64(comparand, data);
        _mm256_maskstore_epi64(reinterpret_cast<long long*>(first), mask, replacement);

        advance_bytes(first, 32);
    }

    if (const size_t tail_length = full_length & 0x18; tail_length != 0)
    {
        const __m256i tail_mask = avx2_tail_mask_32(tail_length);
        const __m256i data = _mm256_maskload_epi64(reinterpret_cast<const long long*>(first), tail_mask);
        const __m256i mask = _mm256_and_si256(_mm256_cmpeq_epi64(comparand, data), tail_mask);
        _mm256_maskstore_epi64(reinterpret_cast<long long*>(first), mask, replacement);
    }

    _mm256_zeroupper(); // TRANSITION, DevCom-10331414

#else

    for (auto cur = reinterpret_cast<uint64_t*>(first); cur != last; ++cur)
    {
        if (*cur == old_val)
        {
            *cur = new_val;
        }
    }

#endif // defined(USE_AVX2)
}

VX_NO_ALIAS void VX_STDCALL replace_copy_1(const void* const first, const void* const last, void* const dest, const uint8_t old_val, const uint8_t new_val) noexcept
{
    _replace::replace_copy_impl<_find::find_traits_1>(first, last, dest, old_val, new_val);
}

VX_NO_ALIAS void VX_STDCALL replace_copy_2(const void* const first, const void* const last, void* const dest, const uint16_t old_val, const uint16_t new_val) noexcept
{
    _replace::replace_copy_impl<_find::find_traits_2>(first, last, dest, old_val, new_val);
}

VX_NO_ALIAS void VX_STDCALL replace_copy_4(const void* const first, const void* const last, void* const dest, const uint32_t old_val, const uint32_t new_val) noexcept
{
    _replace::replace_copy_impl<_find::find_traits_4>(first, last, dest, old_val, new_val);
}

VX_NO_ALIAS void VX_STDCALL replace_copy_8(const void* const first, const void* const last, void* const dest, const uint64_t old_val, const uint64_t new_val) noexcept
{
    _replace::replace_copy_impl<_find::find_traits_8>(first, last, dest, old_val, new_val);
}

} // extern "C"

} // namespace _simd
} // namespace vx
