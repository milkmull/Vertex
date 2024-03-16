#pragma once

#include "vec_fn_comparison.h"

namespace vx {
namespace math {

// =============== is_zero_approx ===============

template <size_t N, typename T>
inline constexpr bool is_zero_approx(
    const mat<2, N, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.columns[0], epsilon)
        && math::is_zero_approx(x.columns[1], epsilon);
}

template <size_t N, typename T>
inline constexpr bool is_zero_approx(
    const mat<3, N, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.columns[0], epsilon)
        && math::is_zero_approx(x.columns[1], epsilon)
        && math::is_zero_approx(x.columns[2], epsilon);
}

template <size_t N, typename T>
inline constexpr bool is_zero_approx(
    const mat<4, N, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.columns[0], epsilon)
        && math::is_zero_approx(x.columns[1], epsilon)
        && math::is_zero_approx(x.columns[2], epsilon)
        && math::is_zero_approx(x.columns[3], epsilon);
}

// =============== is_equal_approx ===============

template <size_t N, typename T>
inline constexpr bool is_equal_approx(
    const mat<2, N, T>& a,
    const mat<2, N, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.columns[0], b.columns[0], epsilon)
        && math::is_equal_approx(a.columns[1], b.columns[1], epsilon);
}

template <size_t N, typename T>
inline constexpr bool is_equal_approx(
    const mat<3, N, T>& a,
    const mat<3, N, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.columns[0], b.columns[0], epsilon)
        && math::is_equal_approx(a.columns[1], b.columns[1], epsilon)
        && math::is_equal_approx(a.columns[2], b.columns[2], epsilon);
}

template <size_t N, typename T>
inline constexpr bool is_equal_approx(
    const mat<4, N, T>& a,
    const mat<4, N, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.columns[0], b.columns[0], epsilon)
        && math::is_equal_approx(a.columns[1], b.columns[1], epsilon)
        && math::is_equal_approx(a.columns[2], b.columns[2], epsilon)
        && math::is_equal_approx(a.columns[3], b.columns[3], epsilon);
}

// =============== is_not_equal_approx ===============

template <size_t N, typename T>
inline constexpr bool is_not_equal_approx(
    const mat<2, N, T>& a,
    const mat<2, N, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_not_equal_approx(a.columns[0], b.columns[0], epsilon)
        && math::is_not_equal_approx(a.columns[1], b.columns[1], epsilon);
}

template <size_t N, typename T>
inline constexpr bool is_not_equal_approx(
    const mat<3, N, T>& a,
    const mat<3, N, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_not_equal_approx(a.columns[0], b.columns[0], epsilon)
        && math::is_not_equal_approx(a.columns[1], b.columns[1], epsilon)
        && math::is_not_equal_approx(a.columns[2], b.columns[2], epsilon);
}

template <size_t N, typename T>
inline constexpr bool is_not_equal_approx(
    const mat<4, N, T>& a,
    const mat<4, N, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_not_equal_approx(a.columns[0], b.columns[0], epsilon)
        && math::is_not_equal_approx(a.columns[1], b.columns[1], epsilon)
        && math::is_not_equal_approx(a.columns[2], b.columns[2], epsilon)
        && math::is_not_equal_approx(a.columns[3], b.columns[3], epsilon);
}

}
}