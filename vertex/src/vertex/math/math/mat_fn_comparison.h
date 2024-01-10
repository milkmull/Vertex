#pragma once

#include "vec_fn_comparison.h"

namespace vx {
namespace math {

// =============== is_equal_approx ===============

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::mat<2, 2, T>& m1,
    const detail::mat<2, 2, T>& m2,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(m1.columns[0], m2.columns[0], epsilon)
        && math::is_equal_approx(m1.columns[1], m2.columns[1], epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::mat<3, 3, T>& m1,
    const detail::mat<3, 3, T>& m2,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(m1.columns[0], m2.columns[0], epsilon)
        && math::is_equal_approx(m1.columns[1], m2.columns[1], epsilon)
        && math::is_equal_approx(m1.columns[2], m2.columns[2], epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::mat<4, 4, T>& m1,
    const detail::mat<4, 4, T>& m2,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(m1.columns[0], m2.columns[0], epsilon)
        && math::is_equal_approx(m1.columns[1], m2.columns[1], epsilon)
        && math::is_equal_approx(m1.columns[2], m2.columns[2], epsilon)
        && math::is_equal_approx(m1.columns[3], m2.columns[3], epsilon);
}

// =============== is_zero_approx ===============

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::mat<2, 2, T>& m,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(m.columns[0], epsilon)
        && math::is_zero_approx(m.columns[1], epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::mat<3, 3, T>& m,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(m.columns[0], epsilon)
        && math::is_zero_approx(m.columns[1], epsilon)
        && math::is_zero_approx(m.columns[2], epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::mat<4, 4, T>& m,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(m.columns[0], epsilon)
        && math::is_zero_approx(m.columns[1], epsilon)
        && math::is_zero_approx(m.columns[2], epsilon)
        && math::is_zero_approx(m.columns[3], epsilon);
}

// =============== round_if_close ===============

template <typename T>
inline constexpr auto round_if_close(
    const detail::mat<2, 2, T>& m,
    const T epsilon = math::epsilon<T>
)
{
    return detail::mat<2, 2, T>(
        math::round_if_close(m.columns[0], epsilon),
        math::round_if_close(m.columns[1], epsilon)
    );
}

template <typename T>
inline constexpr auto round_if_close(
    const detail::mat<3, 3, T>& m,
    const T epsilon = math::epsilon<T>
)
{
    return detail::mat<3, 3, T>(
        math::round_if_close(m.columns[0], epsilon),
        math::round_if_close(m.columns[1], epsilon),
        math::round_if_close(m.columns[2], epsilon)
    );
}

template <typename T>
inline constexpr auto round_if_close(
    const detail::mat<4, 4, T>& m,
    const T epsilon = math::epsilon<T>
)
{
    return detail::mat<4, 4, T>(
        math::round_if_close(m.columns[0], epsilon),
        math::round_if_close(m.columns[1], epsilon),
        math::round_if_close(m.columns[2], epsilon),
        math::round_if_close(m.columns[3], epsilon)
    );
}

}
}