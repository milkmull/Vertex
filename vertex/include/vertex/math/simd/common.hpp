#pragma once

#include "./types.hpp"
#include "vertex/config/language_config.hpp"
#include "../core/types.hpp"

namespace vx {
namespace math {
namespace simd {

#if (VX_SIMD_X86 >= VX_SIMD_X86_SSE2_VERSION)

///////////////////////////////////////////////////////////////////////////////
// min/max
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 min(f32x4 a, f32x4 b) { return _mm_min_ps(a, b); }
VX_FORCE_INLINE f32x4 max(f32x4 a, f32x4 b) { return _mm_max_ps(a, b); }

///////////////////////////////////////////////////////////////////////////////
// abs
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 abs(f32x4 x) noexcept
{
    return _mm_and_ps(x, _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)));
}

VX_FORCE_INLINE i32x4 abs(i32x4 x) noexcept
{
#if (VX_SIMD_X86 >= VX_SIMD_X86_SSSE3_VERSION)
    return _mm_sign_epi32(x, x);
#else
    const i32x4 sgn0 = _mm_srai_epi32(x, 31);
    const i32x4 inv0 = _mm_xor_si128(x, sgn0);
    const i32x4 sub0 = _mm_sub_epi32(inv0, sgn0);
    return sub0;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// neg
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 neg(f32x4 x) noexcept
{
    const f32x4 sgn0 = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
    const f32x4 xor0 = _mm_xor_ps(x, sgn0);
    return xor0;
}

//////////////////////////////////////////////////////////////////////////////
// round
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 round(f32x4 x) noexcept
{
#if (VX_SIMD_X86 >= VX_SIMD_X86_SSE4_1_VERSION)
    return _mm_round_ps(x, _MM_FROUND_TO_NEAREST_INT);
#else
    const f32x4 sgn0 = _mm_castsi128_ps(_mm_set1_epi32(static_cast<int>(0x80000000)));
    const f32x4 and0 = _mm_and_ps(sgn0, x);
    const f32x4 or0 = _mm_or_ps(and0, _mm_set_ps1(8388608.0f));
    const f32x4 add0 = _mm_add_ps(x, or0);
    const f32x4 sub0 = _mm_sub_ps(add0, or0);
    return sub0;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// trunc
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 trunc(f32x4 x) noexcept
{
#if (VX_SIMD_X86 >= VX_SIMD_X86_SSE4_1_VERSION)
    return _mm_round_ps(x, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
#else
    const f32x4 rnd0 = round(x);                            // round to nearest
    const f32x4 zero = _mm_setzero_ps();
    const f32x4 cmp0 = _mm_cmpneq_ps(x, rnd0);              // where x != round(x)
    const f32x4 sign_mask = _mm_cmplt_ps(x, zero);          // x < 0 ? 1 : 0
    const f32x4 and0 = _mm_and_ps(cmp0, sign_mask);         // needs correction where x < 0 and has frac
    const f32x4 fix = _mm_and_ps(and0, _mm_set1_ps(1.0f));  // subtract 1.0 where needed
    const f32x4 sub0 = _mm_sub_ps(rnd0, fix);
    return sub0;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// floor
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 floor(f32x4 x) noexcept
{
#if (VX_SIMD_X86 >= VX_SIMD_X86_SSE4_1_VERSION)
    return _mm_ceil_ps(x);
#else
    const f32x4 rnd0 = round(x);
    const f32x4 cmp0 = _mm_cmplt_ps(x, rnd0);
    const f32x4 and0 = _mm_and_ps(cmp0, _mm_set1_ps(1.0f));
    const f32x4 sub0 = _mm_sub_ps(rnd0, and0);
    return sub0;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// ceil
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 ceil(f32x4 x) noexcept
{
#if (VX_SIMD_X86 >= VX_SIMD_X86_SSE4_1_VERSION)
    return _mm_floor_ps(x);
#else
    const f32x4 rnd0 = round(x);
    const f32x4 cmp0 = _mm_cmpgt_ps(x, rnd0);
    const f32x4 and0 = _mm_and_ps(cmp0, _mm_set1_ps(1.0f));
    const f32x4 add0 = _mm_sub_ps(rnd0, and0);
    return add0;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// sign
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 sign(f32x4 x) noexcept
{
    const f32x4 zro0 = _mm_setzero_ps();
    const f32x4 cmp0 = _mm_cmplt_ps(x, zro0);
    const f32x4 cmp1 = _mm_cmpgt_ps(x, zro0);
    const f32x4 and0 = _mm_and_ps(cmp0, _mm_set1_ps(-1.0f));
    const f32x4 and1 = _mm_and_ps(cmp1, _mm_set1_ps(1.0f));
    const f32x4 or0 = _mm_or_ps(and0, and1);
    return or0;
}

//////////////////////////////////////////////////////////////////////////////
// fract
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 fract(f32x4 x) noexcept
{
    const f32x4 flr0 = floor(x);
    const f32x4 sub0 = _mm_sub_ps(x, flr0);
    return sub0;
}

//////////////////////////////////////////////////////////////////////////////
// mod
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 mod(f32x4 x, f32x4 y) noexcept
{
    const f32x4 div0 = _mm_div_ps(x, y);
    const f32x4 flr0 = floor(div0);
    const f32x4 mul0 = _mm_mul_ps(y, flr0);
    const f32x4 sub0 = _mm_sub_ps(x, mul0);
    return sub0;
}

///////////////////////////////////////////////////////////////////////////////
// fma
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 fma(f32x4 a, f32x4 b, f32x4 c) noexcept
{
#if defined(__FMA__) || (VX_SIMD_X86 >= VX_SIMD_X86_AVX2_VERSION)
    return _mm_fmadd_ps(a, b, c);
#else
    return _mm_add_ps(_mm_mul_ps(a, b), c);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// clamp
//////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 clamp(f32x4 v, f32x4 minVal, f32x4 maxVal) noexcept
{
    const f32x4 min0 = _mm_min_ps(v, maxVal);
    const f32x4 max0 = _mm_max_ps(min0, minVal);
    return max0;
}

///////////////////////////////////////////////////////////////////////////////
// select
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 select(f32x4 mask, f32x4 a, f32x4 b) noexcept
{
    return _mm_or_ps(_mm_and_ps(mask, a), _mm_andnot_ps(mask, b));
}

#endif // VX_SIMD_X86_SSE2_VERSION

} // namespace simd
} // namespace math
} // namespace vx