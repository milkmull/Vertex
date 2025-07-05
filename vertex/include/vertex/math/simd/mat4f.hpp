#pragma once

#include "vertex/config/simd.hpp"

#if defined(VXM_ENABLE_SIMD)

#   if defined(VX_SIMD_X86)
#       include "./arch/x86/mat4f.hpp"
#   endif // VX_SIMD_X86

#endif // VXM_ENABLE_SIMD

#include "./arch/mat_default.hpp"