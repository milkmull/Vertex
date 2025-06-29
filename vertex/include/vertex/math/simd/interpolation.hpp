#pragma once

#include "./types.hpp"
#include "vertex/config/language_config.hpp"
#include "./common.hpp"

namespace vx {
namespace math {
namespace simd {

#if (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)

//////////////////////////////////////////////////////////////////////////////
// lerp
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 lerp(f32x4 v1, f32x4 v2, f32x4 a) noexcept
{
    const f32x4 sub0 = _mm_sub_ps(_mm_set1_ps(1.0f), a);
    const f32x4 mul0 = _mm_mul_ps(v1, sub0);
    const f32x4 mad0 = fma(v2, a, mul0);
    return mad0;
}

//////////////////////////////////////////////////////////////////////////////
// mix
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 mix(f32x4 v1, f32x4 v2, f32x4 a) noexcept
{
    return lerp(v1, v2, a);
}

//////////////////////////////////////////////////////////////////////////////
// step
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 step(f32x4 edge, f32x4 x) noexcept
{
    const f32x4 mask = _mm_cmpge_ps(x, edge);
    return _mm_and_ps(mask, _mm_set1_ps(1.0f));
}

//////////////////////////////////////////////////////////////////////////////
// smoothstep
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 smoothstep(f32x4 edge0, f32x4 edge1, f32x4 x) noexcept
{
    const f32x4 sub0 = _mm_sub_ps(x, edge0);
    const f32x4 sub1 = _mm_sub_ps(edge1, edge0);
    const f32x4 div0 = _mm_div_ps(sub0, sub1);
    const f32x4 clp0 = clamp(div0, _mm_setzero_ps(), _mm_set1_ps(1.0f));
    const f32x4 mul0 = _mm_mul_ps(_mm_set1_ps(2.0f), clp0);
    const f32x4 sub2 = _mm_sub_ps(_mm_set1_ps(3.0f), mul0);
    const f32x4 mul1 = _mm_mul_ps(clp0, clp0);
    const f32x4 mul2 = _mm_mul_ps(mul1, sub2);
    return mul2;
}

#endif // VX_SIMD_X86_SSE2_VERSION

} // namespace simd
} // namespace math
} // namespace vx