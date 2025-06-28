#pragma once

#include "./types.hpp"
#include "vertex/config/language_config.hpp"
#include "../core/types.hpp"

namespace vx {
namespace math {
namespace simd {

#if (VX_SIMD_X86 >= VXM_SIMD_SSE2)

///////////////////////////////////////////////////////////////////////////////
// set
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 set0() noexcept
{
    return _mm_setzero_ps();
}

VX_FORCE_INLINE f32x4 set1(f32 x) noexcept
{
    return _mm_set1_ps(x);
}

VX_FORCE_INLINE f32x4 set(f32 x0, f32 x1, f32 x2, f32 x3) noexcept
{
    return _mm_set_ps(x3, x2, x1, x0);
}

///////////////////////////////////////////////////////////////////////////////
// load/store
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 load(const f32* ptr)
{
    return _mm_loadu_ps(ptr);
}

VX_FORCE_INLINE void store(f32* ptr, f32x4 v)
{
    _mm_storeu_ps(ptr, v);
}

///////////////////////////////////////////////////////////////////////////////
// comparison
///////////////////////////////////////////////////////////////////////////////

// equal (==)

VX_FORCE_INLINE f32x4 cmp_eq(f32x4 a, f32x4 b) noexcept
{
    return _mm_cmpeq_ps(a, b);
}

// not equal (!=)

VX_FORCE_INLINE f32x4 cmp_neq(f32x4 a, f32x4 b) noexcept
{
    return _mm_cmpneq_ps(a, b);
}

// less than (<)

VX_FORCE_INLINE f32x4 cmp_lt(f32x4 a, f32x4 b) noexcept
{
    return _mm_cmplt_ps(a, b);
}

// less than or equal (<=)

VX_FORCE_INLINE f32x4 cmp_le(f32x4 a, f32x4 b) noexcept
{
    return _mm_cmple_ps(a, b);
}

// greater than (>)

VX_FORCE_INLINE f32x4 cmp_gt(f32x4 a, f32x4 b) noexcept
{
    return _mm_cmpgt_ps(a, b);
}

// greater than or equal (>=)

VX_FORCE_INLINE f32x4 cmp_ge(f32x4 a, f32x4 b) noexcept
{
    return _mm_cmpge_ps(a, b);
}

///////////////////////////////////////////////////////////////////////////////
// arithmetic operators
///////////////////////////////////////////////////////////////////////////////

// add (+)

VX_FORCE_INLINE f32x4 add(f32x4 a, f32x4 b) noexcept
{
    return _mm_add_ps(a, b);
}

VX_FORCE_INLINE f32x4 add_lane0(f32x4 a, f32x4 b) noexcept
{
    return _mm_add_ss(a, b);
}

// subtract (-)

VX_FORCE_INLINE f32x4 sub(f32x4 a, f32x4 b) noexcept
{
    return _mm_sub_ps(a, b);
}

VX_FORCE_INLINE f32x4 sub_lane0(f32x4 a, f32x4 b) noexcept
{
    return _mm_sub_ss(a, b);
}

// multiply (*)

VX_FORCE_INLINE f32x4 mul(f32x4 a, f32x4 b) noexcept
{
    return _mm_mul_ps(a, b);
}

VX_FORCE_INLINE f32x4 mul_lane0(f32x4 a, f32x4 b) noexcept
{
    return _mm_mul_ss(a, b);
}

// divide (/)

VX_FORCE_INLINE f32x4 div(f32x4 a, f32x4 b) noexcept
{
    return _mm_div_ps(a, b);
}

VX_FORCE_INLINE f32x4 div_lane0(f32x4 a, f32x4 b) noexcept
{
    return _mm_div_ss(a, b);
}

VX_FORCE_INLINE f32x4 div_lowp(f32x4 a, f32x4 b) noexcept
{
    return _mm_mul_ps(a, _mm_rcp_ps(b));
}

///////////////////////////////////////////////////////////////////////////////
// bitwise operators
///////////////////////////////////////////////////////////////////////////////

// and

VX_FORCE_INLINE f32x4 and_(f32x4 a, f32x4 b) noexcept
{
    return _mm_and_ps(a, b);
}

// and not (~a & b)

VX_FORCE_INLINE f32x4 andnot(f32x4 a, f32x4 b) noexcept
{
    return _mm_andnot_ps(a, b);
}

// or

VX_FORCE_INLINE f32x4 or_(f32x4 a, f32x4 b) noexcept
{
    return _mm_or_ps(a, b);
}

// xor

VX_FORCE_INLINE f32x4 xor_(f32x4 a, f32x4 b) noexcept
{
    return _mm_xor_ps(a, b);
}

///////////////////////////////////////////////////////////////////////////////
// shuffle
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE f32x4 shuffle(f32x4 a, int x, int y, int z, int w) noexcept
{
#if (VX_SIMD_X86 >= VX_SIMD_X86_AVX2_VERSION)
    return _mm_permute_ps(a, _MM_SHUFFLE(x, y, z, w)); // AVX order: x = lane 3
#else
    return _mm_shuffle_ps(a, a, _MM_SHUFFLE(w, z, y, x)); // SSE order: x = lane 0
#endif
}

VX_FORCE_INLINE f32x4 movehl(f32x4 a, f32x4 b) { return _mm_movehl_ps(a, b); }
VX_FORCE_INLINE f32x4 movelh(f32x4 a, f32x4 b) { return _mm_movelh_ps(a, b); }

#endif

} // namespace simd
} // namespace math
} // namespace vx