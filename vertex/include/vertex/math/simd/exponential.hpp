#pragma once

#include "./types.hpp"
#include "vertex/config/language_config.hpp"
#include "../core/types.hpp"

namespace vx {
namespace math {
namespace simd {

#if (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)

//////////////////////////////////////////////////////////////////////////////
// inverse
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 inverse(f32x4 x) noexcept
{
    const f32x4 one = _mm_set1_ps(1.0f);
    return _mm_div_ps(one, x);
}

VX_FORCE_INLINE f32x4 inverse_lowp(f32x4 x) noexcept
{
    return _mm_rcp_ps(x);
}

//////////////////////////////////////////////////////////////////////////////
// inverse_sqrt
//////////////////////////////////////////////////////////////////////////////

// Newton-Raphson

VX_FORCE_INLINE f32x4 inverse_sqrt(f32x4 x) noexcept
{
    const f32x4 half = _mm_set1_ps(0.5f);
    const f32x4 three = _mm_set1_ps(3.0f);
    const f32x4 est = _mm_rsqrt_ps(x);                      // initial estimate
    const f32x4 mul0 = _mm_mul_ps(est, est);                // est^2
    const f32x4 mul1 = _mm_mul_ps(mul0, x);                 // est^2 * x
    const f32x4 sub0 = _mm_sub_ps(three, mul1);             // 3 - est^2 * x
    const f32x4 mul2 = _mm_mul_ps(est, half);               // 0.5 * est
    return _mm_mul_ps(mul2, sub0);                          // refined estimate
}

VX_FORCE_INLINE f32x4 inverse_sqrt_lowp(f32x4 x) noexcept
{
    return _mm_rsqrt_ps(x);
}

//////////////////////////////////////////////////////////////////////////////
// sqrt
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 sqrt(f32x4 x) noexcept
{
    return _mm_sqrt_ps(x);
}

VX_FORCE_INLINE f32x4 sqrt_lowp(f32x4 x) noexcept
{
    const f32x4 rsqrt = inverse_sqrt_lowp(x);
    return _mm_mul_ps(rsqrt, x);
}

#endif // VX_SIMD_X86_SSE2_VERSION

} // namespace simd
} // namespace math
} // namespace vx