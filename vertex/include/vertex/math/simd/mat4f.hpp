#pragma once

#include "vertex/config/simd.hpp"

#if defined(VX_SIMD_X86) && (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)
#   include "./arch/x86/mat4f.hpp"
#endif // VX_SIMD_X86

#include "./arch/mat_default.hpp"