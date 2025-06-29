#pragma once

#include "./common.hpp"
#include "./exponential.hpp"

namespace vx {
namespace math {
namespace simd {

#if (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)

///////////////////////////////////////////////////////////////////////////////
// dot
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 dot(f32x4 v1, f32x4 v2) noexcept
{
#if (VX_SIMD_X86 >= VX_SIMD_X86_AVX_VERSION)
    return _mm_dp_ps(v1, v2, 0xff);
#elif (VX_SIMD_X86 >= VX_SIMD_X86_SSE3_VERSION)
    const f32x4 mul0 = _mm_mul_ps(v1, v2);
    const f32x4 hadd0 = _mm_hadd_ps(mul0, mul0);
    const f32x4 hadd1 = _mm_hadd_ps(hadd0, hadd0);
    return hadd1;
#else
    const f32x4 mul0 = _mm_mul_ps(v1, v2);
    const f32x4 swp0 = _mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(2, 3, 0, 1));
    const f32x4 add0 = _mm_add_ps(mul0, swp0);
    const f32x4 swp1 = _mm_shuffle_ps(add0, add0, _MM_SHUFFLE(0, 1, 2, 3));
    const f32x4 add1 = _mm_add_ps(add0, swp1);
    return add1;
#endif
}

VX_FORCE_INLINE f32x4 dot_lane0(f32x4 v1, f32x4 v2) noexcept
{
#if (VX_SIMD_X86 >= VX_SIMD_X86_AVX_VERSION)
    return _mm_dp_ps(v1, v2, 0xff);
#elif (VX_SIMD_X86 >= VX_SIMD_X86_SSE3_VERSION)
    const f32x4 mul0 = _mm_mul_ps(v1, v2);
    const f32x4 had0 = _mm_hadd_ps(mul0, mul0);
    const f32x4 had1 = _mm_hadd_ps(had0, had0);
    return had1;
#else
    const f32x4 mul0 = _mm_mul_ps(v1, v2);
    const f32x4 mov0 = _mm_movehl_ps(mul0, mul0);
    const f32x4 add0 = _mm_add_ps(mov0, mul0);
    const f32x4 swp1 = _mm_shuffle_ps(add0, add0, 1);
    const f32x4 add1 = _mm_add_ss(add0, swp1);
    return add1;
#endif
}


///////////////////////////////////////////////////////////////////////////////
// cross
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 cross(f32x4 v1, f32x4 v2) noexcept
{
    const f32x4 swp0 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1));
    const f32x4 swp1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 1, 0, 2));
    const f32x4 swp2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 0, 2, 1));
    const f32x4 swp3 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 1, 0, 2));
    const f32x4 mul0 = _mm_mul_ps(swp0, swp3);
    const f32x4 mul1 = _mm_mul_ps(swp1, swp2);
    const f32x4 sub0 = _mm_sub_ps(mul0, mul1);
    return sub0;
}

///////////////////////////////////////////////////////////////////////////////
// length
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 length(f32x4 x) noexcept
{
    const f32x4 dot0 = dot(x, x);
    const f32x4 sqt0 = _mm_sqrt_ps(dot0);
    return sqt0;
}

///////////////////////////////////////////////////////////////////////////////
// distance
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 distance(f32x4 p0, f32x4 p1) noexcept
{
    const f32x4 sub0 = _mm_sub_ps(p0, p1);
    const f32x4 len0 = length(sub0);
    return len0;
}

///////////////////////////////////////////////////////////////////////////////
// normalize
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 normalize(f32x4 v) noexcept
{
    const f32x4 dot0 = dot(v, v);
    const f32x4 isr0 = inverse_sqrt(dot0);
    const f32x4 mul0 = _mm_mul_ps(v, isr0);
    return mul0;
}

VX_FORCE_INLINE f32x4 normalize_lowp(f32x4 v) noexcept
{
    const f32x4 dot0 = dot(v, v);
    const f32x4 isr0 = inverse_sqrt_lowp(dot0);
    const f32x4 mul0 = _mm_mul_ps(v, isr0);
    return mul0;
}

///////////////////////////////////////////////////////////////////////////////
// project
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 project(f32x4 a, f32x4 b) noexcept
{
    // dot(a, b)
    const f32x4 ab = dot(a, b);
    // dot(b, b)
    const f32x4 bb = dot(b, b);

    // Compute scalar = dot(a,b) / dot(b,b)
    __m128 scalar = _mm_div_ss(ab, bb);

    // Broadcast scalar to all lanes
    scalar = _mm_shuffle_ps(scalar, scalar, 0);

    // result = b * scalar
    return _mm_mul_ps(b, scalar);
}

///////////////////////////////////////////////////////////////////////////////
// reflect
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 reflect(f32x4 I, f32x4 N) noexcept
{
    const f32x4 dot0 = dot(N, I);
    const f32x4 mul0 = _mm_mul_ps(N, dot0);
    const f32x4 mul1 = _mm_mul_ps(mul0, _mm_set1_ps(2.0f));
    const f32x4 sub0 = _mm_sub_ps(I, mul1);
    return sub0;
}

///////////////////////////////////////////////////////////////////////////////
// refract
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE __m128 refract(f32x4 I, f32x4 N, f32x4 eta) noexcept
{
    const f32x4 dot0 = dot(N, I);
    const f32x4 eta2 = _mm_mul_ps(eta, eta);
    const f32x4 dot02 = _mm_mul_ps(dot0, dot0);
    const f32x4 k = _mm_sub_ps(_mm_set1_ps(1.0f), _mm_mul_ps(eta2, _mm_sub_ps(_mm_set1_ps(1.0f), dot02)));

    // Mask for k < 0 (total internal reflection)
    __m128 mask = _mm_cmplt_ps(k, _mm_setzero_ps());

    // sqrt(k)
    const f32x4 sqrt_k = _mm_sqrt_ps(k);

    // eta * dot0 + sqrt(k)
    const f32x4 term = fma(eta, dot0, sqrt_k);

    // eta * I - N * term
    const f32x4 result = _mm_sub_ps(_mm_mul_ps(eta, I), _mm_mul_ps(N, term));

    // If k < 0, zero out result
    return _mm_andnot_ps(mask, result); // zero where k<0, else result
}

///////////////////////////////////////////////////////////////////////////////
// face_forward
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 faceforward(f32x4 N, f32x4 I, f32x4 Nref) noexcept
{
    const f32x4 dot0 = dot(Nref, I);
    const f32x4 sgn0 = sign(dot0);
    const f32x4 mul0 = _mm_mul_ps(sgn0, _mm_set1_ps(-1.0f));
    const f32x4 mul1 = _mm_mul_ps(N, mul0);
    return mul1;
}

#endif // VX_SIMD_X86_SSE2_VERSION

} // namespace simd
} // namespace math
} // namespace vx