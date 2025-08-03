#pragma once

#include "vertex/config/simd.hpp"

#if defined(VX_SIMD_X86) && (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)
#   define VX_IMG_SIMD_SSE2
#elif defined(VX_SIMD_NEON)
#   define VX_IMG_SIMD_NEON
#endif