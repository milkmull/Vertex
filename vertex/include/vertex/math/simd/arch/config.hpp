#pragma once

#include <cstdint>

#include "vertex/config/simd.hpp"
#include "vertex/config/language_config.hpp"

///////////////////////////////////////////////////////////////////////////////
// simd headers
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_SIMD_X86)

#   if   (VX_SIMD_X86 >= VX_SIMD_X86_AVX2_VERSION)
#       include <immintrin.h>
#   elif (VX_SIMD_X86 >= VX_SIMD_X86_AVX_VERSION)
#       include <immintrin.h>
#   elif (VX_SIMD_X86 >= VX_SIMD_X86_SSE4_2_VERSION)
#       include <nmmintrin.h>
#   elif (VX_SIMD_X86 >= VX_SIMD_X86_SSE4_1_VERSION)
#       include <smmintrin.h>
#   elif (VX_SIMD_X86 >= VX_SIMD_X86_SSSE3_VERSION)
#       include <tmmintrin.h>
#   elif (VX_SIMD_X86 >= VX_SIMD_X86_SSE3_VERSION)
#       include <pmmintrin.h>
#   elif (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)
#       include <emmintrin.h>
#   elif (VX_SIMD_X86 >= VX_SIMD_X86_SSE_VERSION)
#       include <xmmintrin.h>
#   endif

#elif defined(VX_SIMD_ARM)

#   include <arm_neon.h>

#elif defined(VX_SIMD_PPC)

#   include <altivec.h>
#   undef vector

#endif

///////////////////////////////////////////////////////////////////////////////

namespace vx {
namespace math {
namespace simd {

///////////////////////////////////////////////////////////////////////////////
// scalar types
///////////////////////////////////////////////////////////////////////////////

VX_STATIC_ASSERT(sizeof(float) == 4, "float must be 32 bit size");
using f32 = float;

using i32 = int32_t;
using u32 = uint32_t;

} // namespace simd
} // namespace math
} // namespace vx