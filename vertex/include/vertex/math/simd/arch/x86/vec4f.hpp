#pragma once

#include "../config.hpp"
#include "../vec_default.hpp"

namespace vx {
namespace math {
namespace simd {

template <>
struct vec<4, f32>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = f32;
    static constexpr size_t size = 4;

    using data_type = __m128;

    static constexpr size_t calulate_alignment() noexcept
    {
        constexpr size_t a1 = alignof(scalar_type[size]);
        constexpr size_t a2 = alignof(data_type);
        return (a1 > a2) ? a1 : a2;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type cmp_eq(data_type v1, data_type v2) noexcept { return _mm_cmpeq_ps(v1, v2); }
    static VX_FORCE_INLINE data_type cmp_neq(data_type v1, data_type v2) noexcept { return _mm_cmpneq_ps(v1, v2); }
    static VX_FORCE_INLINE data_type cmp_lt(data_type v1, data_type v2) noexcept { return _mm_cmplt_ps(v1, v2); }
    static VX_FORCE_INLINE data_type cmp_le(data_type v1, data_type v2) noexcept { return _mm_cmple_ps(v1, v2); }
    static VX_FORCE_INLINE data_type cmp_gt(data_type v1, data_type v2) noexcept { return _mm_cmpgt_ps(v1, v2); }
    static VX_FORCE_INLINE data_type cmp_ge(data_type v1, data_type v2) noexcept { return _mm_cmpge_ps(v1, v2); }

    static constexpr int HAVE_CMP_EQ = 1;
    static constexpr int HAVE_CMP_NEQ = 1;
    static constexpr int HAVE_CMP_LT = 1;
    static constexpr int HAVE_CMP_LE = 1;
    static constexpr int HAVE_CMP_GT = 1;
    static constexpr int HAVE_CMP_GE = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type add(data_type v1, data_type v2) noexcept { return _mm_add_ps(v1, v2); }
    static VX_FORCE_INLINE data_type add(data_type v, scalar_type scalar) noexcept { return _mm_add_ps(v, _mm_set1_ps(scalar)); }

    static VX_FORCE_INLINE data_type sub(data_type v1, data_type v2) noexcept { return _mm_sub_ps(v1, v2); }
    static VX_FORCE_INLINE data_type sub(data_type v, scalar_type scalar) noexcept { return _mm_sub_ps(v, _mm_set1_ps(scalar)); }
    static VX_FORCE_INLINE data_type sub(scalar_type scalar, data_type v) noexcept { return _mm_sub_ps(_mm_set1_ps(scalar), v); }

    static VX_FORCE_INLINE data_type mul(data_type v1, data_type v2) noexcept { return _mm_mul_ps(v1, v2); }
    static VX_FORCE_INLINE data_type mul(data_type v, scalar_type scalar) noexcept { return _mm_mul_ps(v, _mm_set1_ps(scalar)); }

    static VX_FORCE_INLINE data_type div(data_type v1, data_type v2) noexcept { return _mm_div_ps(v1, v2); }
    static VX_FORCE_INLINE data_type div(data_type v, scalar_type scalar) noexcept { return _mm_div_ps(v, _mm_set1_ps(scalar)); }
    static VX_FORCE_INLINE data_type div(scalar_type scalar, data_type v) noexcept { return _mm_div_ps(_mm_set1_ps(scalar), v); }

    static VX_FORCE_INLINE data_type div_lowp(data_type v1, data_type v2) noexcept { return _mm_mul_ps(v1, _mm_rcp_ps(v2)); }
    static VX_FORCE_INLINE data_type div_lowp(data_type v, scalar_type scalar) noexcept { return div_lowp(v, _mm_set1_ps(scalar)); }
    static VX_FORCE_INLINE data_type div_lowp(scalar_type scalar, data_type v) noexcept { return div_lowp(_mm_set1_ps(scalar), v); }

    static constexpr int HAVE_ADD = 1;
    static constexpr int HAVE_SUB = 1;
    static constexpr int HAVE_MUL = 1;
    static constexpr int HAVE_DIV = 1;

    static constexpr int HAVE_DIV_LOWP = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // bitwise operators
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type and_(data_type v1, data_type v2) noexcept { return _mm_and_ps(v1, v2); }
    static VX_FORCE_INLINE data_type andnot(data_type v1, data_type v2) noexcept { return _mm_andnot_ps(v1, v2); }
    static VX_FORCE_INLINE data_type or_(data_type v1, data_type v2) noexcept { return _mm_or_ps(v1, v2); }
    static VX_FORCE_INLINE data_type xor_(data_type v1, data_type v2) noexcept { return _mm_xor_ps(v1, v2); }

    static constexpr int HAVE_AND = 1;
    static constexpr int HAVE_ANDNOT = 1;
    static constexpr int HAVE_OR = 1;
    static constexpr int HAVE_XOR = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // shuffle
    ///////////////////////////////////////////////////////////////////////////////

    template <int x, int y, int z, int w>
    static VX_FORCE_INLINE data_type shuffle(data_type v1) noexcept
    {
        VX_STATIC_ASSERT(0 <= x && x <= 3, "Shuffle index out of range");
        VX_STATIC_ASSERT(0 <= y && y <= 3, "Shuffle index out of range");
        VX_STATIC_ASSERT(0 <= z && z <= 3, "Shuffle index out of range");
        VX_STATIC_ASSERT(0 <= w && w <= 3, "Shuffle index out of range");

    #if (VX_SIMD_X86 >= VX_SIMD_X86_AVX2_VERSION)
        return _mm_permute_ps(v1, _MM_SHUFFLE(x, y, z, w)); // AVX order: x = lane 3
    #else
        return _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(w, z, y, x)); // SSE order: x = lane 0
    #endif
    }

    static constexpr int HAVE_SHUFFLE = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // min/max
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type min(data_type a, data_type b) { return _mm_min_ps(a, b); }
    static VX_FORCE_INLINE data_type max(data_type a, data_type b) { return _mm_max_ps(a, b); }

    static constexpr int HAVE_MIN = 1;
    static constexpr int HAVE_MAX = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // abs
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type abs(data_type x) noexcept
    {
        return _mm_and_ps(x, _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)));
    }

    static constexpr int HAVE_ABS = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // neg
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type neg(data_type x) noexcept
    {
        const data_type sgn0 = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
        const data_type xor0 = _mm_xor_ps(x, sgn0);
        return xor0;
    }

    static constexpr int HAVE_NEG = 1;

    //////////////////////////////////////////////////////////////////////////////
    // round
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type round(data_type x) noexcept
    {
    #if (VX_SIMD_X86 >= VX_SIMD_X86_SSE4_1_VERSION)
        return _mm_round_ps(x, _MM_FROUND_TO_NEAREST_INT);
    #else
        const data_type sgn0 = _mm_castsi128_ps(_mm_set1_epi32(static_cast<int>(0x80000000)));
        const data_type and0 = _mm_and_ps(sgn0, x);
        const data_type or0 = _mm_or_ps(and0, _mm_set_ps1(8388608.0f));
        const data_type add0 = _mm_add_ps(x, or0);
        const data_type sub0 = _mm_sub_ps(add0, or0);
        return sub0;
    #endif
    }

    static constexpr int HAVE_ROUND = 1;

    //////////////////////////////////////////////////////////////////////////////
    // trunc
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type trunc(data_type x) noexcept
    {
    #if (VX_SIMD_X86 >= VX_SIMD_X86_SSE4_1_VERSION)
        return _mm_round_ps(x, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
    #else
        const data_type rnd0 = round(x);                            // round to nearest
        const data_type zero = _mm_setzero_ps();
        const data_type cmp0 = _mm_cmpneq_ps(x, rnd0);              // where x != round(x)
        const data_type sign_mask = _mm_cmplt_ps(x, zero);          // x < 0 ? 1 : 0
        const data_type and0 = _mm_and_ps(cmp0, sign_mask);         // needs correction where x < 0 and has frac
        const data_type fix = _mm_and_ps(and0, _mm_set1_ps(1.0f));  // subtract 1.0 where needed
        const data_type sub0 = _mm_sub_ps(rnd0, fix);
        return sub0;
    #endif
    }

    static constexpr int HAVE_TRUNC = 1;

    //////////////////////////////////////////////////////////////////////////////
    // floor
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type floor(data_type x) noexcept
    {
    #if (VX_SIMD_X86 >= VX_SIMD_X86_SSE4_1_VERSION)
        return _mm_ceil_ps(x);
    #else
        const data_type rnd0 = round(x);
        const data_type cmp0 = _mm_cmplt_ps(x, rnd0);
        const data_type and0 = _mm_and_ps(cmp0, _mm_set1_ps(1.0f));
        const data_type sub0 = _mm_sub_ps(rnd0, and0);
        return sub0;
    #endif
    }

    static constexpr int HAVE_FLOOR = 1;

    //////////////////////////////////////////////////////////////////////////////
    // ceil
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type ceil(data_type x) noexcept
    {
    #if (VX_SIMD_X86 >= VX_SIMD_X86_SSE4_1_VERSION)
        return _mm_floor_ps(x);
    #else
        const data_type rnd0 = round(x);
        const data_type cmp0 = _mm_cmpgt_ps(x, rnd0);
        const data_type and0 = _mm_and_ps(cmp0, _mm_set1_ps(1.0f));
        const data_type add0 = _mm_sub_ps(rnd0, and0);
        return add0;
    #endif
    }

    static constexpr int HAVE_CEIL = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // sign
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type sign(data_type x) noexcept
    {
        const data_type zro0 = _mm_setzero_ps();
        const data_type cmp0 = _mm_cmplt_ps(x, zro0);
        const data_type cmp1 = _mm_cmpgt_ps(x, zro0);
        const data_type and0 = _mm_and_ps(cmp0, _mm_set1_ps(-1.0f));
        const data_type and1 = _mm_and_ps(cmp1, _mm_set1_ps(1.0f));
        const data_type or0 = _mm_or_ps(and0, and1);
        return or0;
    }

    static constexpr int HAVE_SIGN = 1;

    //////////////////////////////////////////////////////////////////////////////
    // fract
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type fract(data_type x) noexcept
    {
        const data_type flr0 = floor(x);
        const data_type sub0 = _mm_sub_ps(x, flr0);
        return sub0;
    }

    static constexpr int HAVE_FRACT = 1;

    //////////////////////////////////////////////////////////////////////////////
    // mod
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type mod(data_type x, data_type y) noexcept
    {
        const data_type div0 = _mm_div_ps(x, y);
        const data_type flr0 = floor(div0);
        const data_type mul0 = _mm_mul_ps(y, flr0);
        const data_type sub0 = _mm_sub_ps(x, mul0);
        return sub0;
    }

    static constexpr int HAVE_MOD = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // fma
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type fma(data_type a, data_type b, data_type c) noexcept
    {
    #if defined(__FMA__) || (VX_SIMD_X86 >= VX_SIMD_X86_AVX2_VERSION)
        return _mm_fmadd_ps(a, b, c);
    #else
        return _mm_add_ps(_mm_mul_ps(a, b), c);
    #endif
    }

    static constexpr int HAVE_FMA = 1;

    //////////////////////////////////////////////////////////////////////////////
    // clamp
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type clamp(data_type v, data_type minVal, data_type maxVal) noexcept
    {
        const data_type min0 = _mm_min_ps(v, maxVal);
        const data_type max0 = _mm_max_ps(min0, minVal);
        return max0;
    }

    static constexpr int HAVE_CLAMP = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // select
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type select(data_type mask, data_type a, data_type b) noexcept
    {
        return _mm_or_ps(_mm_and_ps(mask, a), _mm_andnot_ps(mask, b));
    }

    static constexpr int HAVE_SELECT = 1;

    //////////////////////////////////////////////////////////////////////////////
    // inverse
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type inverse(data_type x) noexcept
    {
        const data_type one = _mm_set1_ps(1.0f);
        return _mm_div_ps(one, x);
    }

    static VX_FORCE_INLINE data_type inverse_lowp(data_type x) noexcept
    {
        return _mm_rcp_ps(x);
    }

    static constexpr int HAVE_INVERSE = 1;
    static constexpr int HAVE_INVERSE_LOWP = 1;

    //////////////////////////////////////////////////////////////////////////////
    // inverse_sqrt
    //////////////////////////////////////////////////////////////////////////////

    // Newton-Raphson

    static VX_FORCE_INLINE data_type inverse_sqrt(data_type x) noexcept
    {
        const data_type half = _mm_set1_ps(0.5f);
        const data_type three = _mm_set1_ps(3.0f);
        const data_type est = _mm_rsqrt_ps(x);                      // initial estimate
        const data_type mul0 = _mm_mul_ps(est, est);                // est^2
        const data_type mul1 = _mm_mul_ps(mul0, x);                 // est^2 * x
        const data_type sub0 = _mm_sub_ps(three, mul1);             // 3 - est^2 * x
        const data_type mul2 = _mm_mul_ps(est, half);               // 0.5 * est
        return _mm_mul_ps(mul2, sub0);                          // refined estimate
    }

    static VX_FORCE_INLINE data_type inverse_sqrt_lowp(data_type x) noexcept
    {
        return _mm_rsqrt_ps(x);
    }

    static constexpr int HAVE_INVERSE_SQRT = 1;
    static constexpr int HAVE_INVERSE_SQRT_LOWP = 1;

    //////////////////////////////////////////////////////////////////////////////
    // sqrt
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type sqrt(data_type x) noexcept
    {
        return _mm_sqrt_ps(x);
    }

    static VX_FORCE_INLINE data_type sqrt_lowp(data_type x) noexcept
    {
        const data_type rsqrt = inverse_sqrt_lowp(x);
        return _mm_mul_ps(rsqrt, x);
    }

    static constexpr int HAVE_SQRT = 1;
    static constexpr int HAVE_SQRT_LOWP = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // dot
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type dot(data_type v1, data_type v2) noexcept
    {
    #if (VX_SIMD_X86 >= VX_SIMD_X86_AVX_VERSION)

        return _mm_dp_ps(v1, v2, 0xff);

    #elif (VX_SIMD_X86 >= VX_SIMD_X86_SSE3_VERSION)

        const data_type mul0 = _mm_mul_ps(v1, v2);
        const data_type hadd0 = _mm_hadd_ps(mul0, mul0);
        const data_type hadd1 = _mm_hadd_ps(hadd0, hadd0);
        return hadd1;

    #else

        const data_type mul0 = _mm_mul_ps(v1, v2);
        const data_type swp0 = _mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(2, 3, 0, 1));
        const data_type add0 = _mm_add_ps(mul0, swp0);
        const data_type swp1 = _mm_shuffle_ps(add0, add0, _MM_SHUFFLE(0, 1, 2, 3));
        const data_type add1 = _mm_add_ps(add0, swp1);
        return add1;

    #endif
    }

    static constexpr int HAVE_DOT = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // cross
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type cross(data_type v1, data_type v2) noexcept
    {
        const data_type swp0 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1));
        const data_type swp1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 1, 0, 2));
        const data_type swp2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 0, 2, 1));
        const data_type swp3 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 1, 0, 2));
        const data_type mul0 = _mm_mul_ps(swp0, swp3);
        const data_type mul1 = _mm_mul_ps(swp1, swp2);
        const data_type sub0 = _mm_sub_ps(mul0, mul1);
        return sub0;
    }

    static constexpr int HAVE_CROSS = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // length
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type length(data_type x) noexcept
    {
        const data_type dot0 = dot(x, x);
        const data_type sqt0 = _mm_sqrt_ps(dot0);
        return sqt0;
    }

    static constexpr int HAVE_LENGTH = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // distance
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type distance(data_type p0, data_type p1) noexcept
    {
        const data_type sub0 = _mm_sub_ps(p0, p1);
        const data_type len0 = length(sub0);
        return len0;
    }

    static constexpr int HAVE_DISTANCE = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // normalize
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type normalize(data_type v) noexcept
    {
        const data_type dot0 = dot(v, v);
        const data_type isr0 = inverse_sqrt(dot0);
        const data_type mul0 = _mm_mul_ps(v, isr0);
        return mul0;
    }

    static VX_FORCE_INLINE data_type normalize_lowp(data_type v) noexcept
    {
        const data_type dot0 = dot(v, v);
        const data_type isr0 = inverse_sqrt_lowp(dot0);
        const data_type mul0 = _mm_mul_ps(v, isr0);
        return mul0;
    }

    static constexpr int HAVE_NORMALIZE = 1;
    static constexpr int HAVE_NORMALIZE_LOWP = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // project
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type project(data_type a, data_type b) noexcept
    {
        const data_type ab = dot(a, b);                         // dot(a, b)
        const data_type bb = dot(b, b);                         // dot(b, b)

        __m128 scalar = _mm_div_ss(ab, bb);                 // Compute scalar = dot(a,b) / dot(b,b)
        scalar = _mm_shuffle_ps(scalar, scalar, 0);         // Broadcast scalar to all lanes
        return _mm_mul_ps(b, scalar);                       // result = b * scalar
    }

    static constexpr int HAVE_PROJECT = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // reflect
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type reflect(data_type I, data_type N) noexcept
    {
        const data_type dot0 = dot(N, I);
        const data_type mul0 = _mm_mul_ps(N, dot0);
        const data_type mul1 = _mm_mul_ps(mul0, _mm_set1_ps(2.0f));
        const data_type sub0 = _mm_sub_ps(I, mul1);
        return sub0;
    }

    static constexpr int HAVE_REFLECT = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // refract
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE __m128 refract(data_type I, data_type N, data_type eta) noexcept
    {
        const data_type dot0 = dot(N, I);
        const data_type eta2 = _mm_mul_ps(eta, eta);
        const data_type dot02 = _mm_mul_ps(dot0, dot0);
        const data_type k = _mm_sub_ps(_mm_set1_ps(1.0f), _mm_mul_ps(eta2, _mm_sub_ps(_mm_set1_ps(1.0f), dot02)));

        const data_type mask = _mm_cmplt_ps(k, _mm_setzero_ps());                       // Mask for k < 0 (total internal reflection)
        const data_type sqrt_k = _mm_sqrt_ps(k);                                        // sqrt(k)
        const data_type term = fma(eta, dot0, sqrt_k);                                  // eta * dot0 + sqrt(k)
        const data_type result = _mm_sub_ps(_mm_mul_ps(eta, I), _mm_mul_ps(N, term));   // eta * I - N * term

        // If k < 0, zero out result
        return _mm_andnot_ps(mask, result); // zero where k<0, else result
    }

    static constexpr int HAVE_REFRACT = 1;

    ///////////////////////////////////////////////////////////////////////////////
    // face_forward
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type faceforward(data_type N, data_type I, data_type Nref) noexcept
    {
        const data_type dot0 = dot(Nref, I);
        const data_type sgn0 = sign(dot0);
        const data_type mul0 = _mm_mul_ps(sgn0, _mm_set1_ps(-1.0f));
        const data_type mul1 = _mm_mul_ps(N, mul0);
        return mul1;
    }

    static constexpr int HAVE_FACEFORWARD = 1;

    //////////////////////////////////////////////////////////////////////////////
    // lerp
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type lerp(data_type v1, data_type v2, data_type a) noexcept
    {
        const data_type sub0 = _mm_sub_ps(_mm_set1_ps(1.0f), a);
        const data_type mul0 = _mm_mul_ps(v1, sub0);
        const data_type mad0 = fma(v2, a, mul0);
        return mad0;
    }

    static constexpr int HAVE_LERP = 1;

    //////////////////////////////////////////////////////////////////////////////
    // mix
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type mix(data_type v1, data_type v2, data_type a) noexcept
    {
        return lerp(v1, v2, a);
    }

    static constexpr int HAVE_MIX = 1;

    //////////////////////////////////////////////////////////////////////////////
    // step
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type step(data_type edge, data_type x) noexcept
    {
        const data_type mask = _mm_cmpge_ps(x, edge);
        return _mm_and_ps(mask, _mm_set1_ps(1.0f));
    }

    static constexpr int HAVE_STEP = 1;

    //////////////////////////////////////////////////////////////////////////////
    // smoothstep
    //////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type smoothstep(data_type edge0, data_type edge1, data_type x) noexcept
    {
        const data_type sub0 = _mm_sub_ps(x, edge0);
        const data_type sub1 = _mm_sub_ps(edge1, edge0);
        const data_type div0 = _mm_div_ps(sub0, sub1);
        const data_type clp0 = clamp(div0, _mm_setzero_ps(), _mm_set1_ps(1.0f));
        const data_type mul0 = _mm_mul_ps(_mm_set1_ps(2.0f), clp0);
        const data_type sub2 = _mm_sub_ps(_mm_set1_ps(3.0f), mul0);
        const data_type mul1 = _mm_mul_ps(clp0, clp0);
        const data_type mul2 = _mm_mul_ps(mul1, sub2);
        return mul2;
    }

    static constexpr int HAVE_SMOOTHSTEP = 1;

}; // vec4f

} // namespace simd
} // namespace math
} // namespace vx