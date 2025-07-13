#pragma once

#include "vertex/config/simd.hpp"

#if defined(VX_SIMD_X86) && (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)
#   include "vertex/math/simd/arch/x86/vec4f.hpp"
#endif // VX_SIMD_X86

#include "vertex/math/simd/arch/vec_default.hpp"