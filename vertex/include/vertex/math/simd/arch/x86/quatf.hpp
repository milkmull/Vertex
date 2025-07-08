#pragma once

#include "../config.hpp"
#include "../quat_default.hpp"

namespace vx {
namespace math {
namespace simd {

template <>
struct quat_t<f32>
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
    // arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE data_type add(data_type v1, data_type v2) noexcept { return _mm_add_ps(v1, v2); }
    static VX_FORCE_INLINE data_type sub(data_type v1, data_type v2) noexcept { return _mm_sub_ps(v1, v2); }

    static VX_FORCE_INLINE data_type mul(data_type v1, data_type v2) noexcept { return _mm_mul_ps(v1, v2); }
    static VX_FORCE_INLINE data_type mul(data_type v, scalar_type scalar) noexcept { return _mm_mul_ps(v, _mm_set1_ps(scalar)); }

    static VX_FORCE_INLINE data_type div(data_type v, scalar_type scalar) noexcept { return _mm_div_ps(v, _mm_set1_ps(scalar)); }
    static VX_FORCE_INLINE data_type div_lowp(data_type v, scalar_type scalar) noexcept { return _mm_mul_ps(v, _mm_rcp_ps(_mm_set1_ps(scalar))); }

    static constexpr int HAVE_ADD = 1;
    static constexpr int HAVE_SUB = 1;
    static constexpr int HAVE_MUL = 1;
    static constexpr int HAVE_DIV = 1;
    static constexpr int HAVE_DIV_LOWP = 1;

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

};

} // namespace simd
} // namespace math
} // namespace vx