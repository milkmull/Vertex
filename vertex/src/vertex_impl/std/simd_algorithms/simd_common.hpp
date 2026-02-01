#pragma once

// These algorithms are part of MSVC library code:
// https://github.com/microsoft/STL/blob/main/stl/src/vector_algorithms.cpp

#if defined(_M_CEE_PURE)
    #error _M_CEE_PURE should not be defined when compiling vector_algorithms.cpp.
#endif

#include "vertex/config/architecture.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/simd.hpp"
#include "vertex/util/bit/bit.hpp"

#if !defined(VX_DEBUG)

// optimize the functions in this TU for speed

    #if defined(_MSC_VER)
        #pragma optimize("t", on)
    #elif defined(__clang__)
        #pragma clang optimize on
        #pragma clang attribute push(__attribute__((optimize("Ofast"))), apply_to = function)
    #elif defined(_GNUC__)
        #pragma GCC optimize("Ofast")
    #endif

#endif // !defined(VX_DEBUG)

#include <cstdint>
#include <cstring>
#include <cwchar>
#include <type_traits>

//=============================================================================
// simd headers
//=============================================================================

#if defined(VX_SIMD_ARM_NEON)

    #include <arm_neon.h>
    #define USE_ARM_NEON

    #if !defined(SIMD_SELECTED)
        #define SIMD_SELECTED
    #endif

#elif defined(VX_SIMD_X86)

    #define USE_X86

    // Detect AVX2
    #if (VX_SIMD_X86 >= VX_SIMD_X86_AVX2_VERSION)

        #include <immintrin.h>
        #define USE_AVX2

        #if !defined(SIMD_SELECTED)
            #define SIMD_SELECTED
        #endif

    #endif

    // Detect SSE2
    #if (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)

        #include <emmintrin.h>
        #define USE_SSE2

        #if !defined(SIMD_SELECTED)
            #define SIMD_SELECTED
        #endif

    #endif

#endif

//=============================================================================

namespace vx {
namespace _simd {

#if defined(USE_AVX2)

VX_NO_DISCARD zeroupper_on_exit
{
    // TRANSITION, DevCom-10331414
    zeroupper_on_exit() = default;

    zeroupper_on_exit(const zeroupper_on_exit&) = delete;
    zeroupper_on_exit& operator=(const zeroupper_on_exit&) = delete;

    ~zeroupper_on_exit()
    {
        _mm256_zeroupper();
    }
};

inline __m256i avx2_tail_mask_32(const size_t count_in_bytes) noexcept
{
    // count_in_bytes must be within [0, 32].
    static constexpr unsigned int tail_masks[16] = {
        ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, 0, 0, 0, 0, 0, 0, 0, 0
    };
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(
        reinterpret_cast<const unsigned char*>(tail_masks) + (32 - count_in_bytes)));
}

#endif // defined(USE_AVX2)

inline size_t byte_length(const void* const first, const void* const last) noexcept
{
    return static_cast<size_t>(static_cast<const unsigned char*>(last) - static_cast<const unsigned char*>(first));
}

inline void rewind_bytes(void*& target, const size_t offset) noexcept
{
    target = static_cast<unsigned char*>(target) - offset;
}

inline void rewind_bytes(const void*& target, const size_t offset) noexcept
{
    target = static_cast<const unsigned char*>(target) - offset;
}

template <typename Int>
inline void advance_bytes(void*& target, const Int offset) noexcept
{
    target = static_cast<unsigned char*>(target) + offset;
}

template <typename Int>
inline void advance_bytes(const void*& target, const Int offset) noexcept
{
    target = static_cast<const unsigned char*>(target) + offset;
}

} // namespace _simd
} // namespace vx
