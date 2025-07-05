#pragma once

#include "../config.hpp"

#if defined(VXM_ENABLE_SIMD)

namespace vx {
namespace math {
namespace simd {

#if (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)

// vector types

using vec4f_t = __m128;
using vec4i_t = __m128i;
using vec4u_t = __m128i;

// matrix types

using mat2x4f_t = vec4f_t[2];
using mat2x4i_t = vec4i_t[2];
using mat2x4u_t = vec4u_t[2];

using mat3x4f_t = vec4f_t[3];
using mat3x4i_t = vec4i_t[3];
using mat3x4u_t = vec4u_t[3];

using mat4x4f_t = vec4f_t[4];
using mat4x4i_t = vec4i_t[4];
using mat4x4u_t = vec4u_t[4];

using mat4f_t = mat4x4f_t;
using mat4i_t = mat4x4i_t;
using mat4u_t = mat4x4u_t;

#endif // VX_SIMD_X86_SSE2_VERSION

} // namespace simd
} // namespace math
} // namespace vx

#endif // VXM_ENABLE_SIMD

#include "../vec_default.hpp"
#include "../mat_default.hpp"