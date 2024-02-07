#pragma once

#include "../detail/vectorize.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== is_zero_approx ===============

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::vecf<2, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::vecf<3, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon)
        && math::is_zero_approx(x.z, epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::vecf<4, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon)
        && math::is_zero_approx(x.z, epsilon)
        && math::is_zero_approx(x.w, epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::colf<T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.r, epsilon)
        && math::is_zero_approx(x.g, epsilon)
        && math::is_zero_approx(x.b, epsilon)
        && math::is_zero_approx(x.a, epsilon);
}

// =============== is_equal_approx ===============

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::vecf<2, T>& a,
    const detail::vecf<2, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::vecf<3, T>& a,
    const detail::vecf<3, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon)
        && math::is_equal_approx(a.z, b.z, epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::vecf<4, T>& a,
    const detail::vecf<4, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon)
        && math::is_equal_approx(a.z, b.z, epsilon)
        && math::is_equal_approx(a.w, b.w, epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::colf<T>& a,
    const detail::colf<T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.r, b.r, epsilon)
        && math::is_equal_approx(a.g, b.g, epsilon)
        && math::is_equal_approx(a.b, b.b, epsilon)
        && math::is_equal_approx(a.a, b.a, epsilon);
}

// =============== is_finite ===============

template <typename T>
inline constexpr bool is_finite(const detail::vecf<2, T>& x)
{
    return math::is_finite(x.x)
        && math::is_finite(x.y);
}

template <typename T>
inline constexpr bool is_finite(const detail::vecf<3, T>& x)
{
    return math::is_finite(x.x)
        && math::is_finite(x.y)
        && math::is_finite(x.z);
}

template <typename T>
inline constexpr bool is_finite(const detail::vecf<4, T>& x)
{
    return math::is_finite(x.x)
        && math::is_finite(x.y)
        && math::is_finite(x.z)
        && math::is_finite(x.w);
}

template <typename T>
inline constexpr bool is_finite(const detail::colf<T>& x)
{
    return math::is_finite(x.r)
        && math::is_finite(x.g)
        && math::is_finite(x.b)
        && math::is_finite(x.a);
}

// =============== is_infinite ===============

template <typename T>
inline constexpr bool is_infinite(const detail::vecf<2, T>& x)
{
    return math::is_infinite(x.x)
        || math::is_infinite(x.y);
}

template <typename T>
inline constexpr bool is_infinite(const detail::vecf<3, T>& x)
{
    return math::is_infinite(x.x)
        || math::is_infinite(x.y)
        || math::is_infinite(x.z);
}

template <typename T>
inline constexpr bool is_infinite(const detail::vecf<4, T>& x)
{
    return math::is_infinite(x.x)
        || math::is_infinite(x.y)
        || math::is_infinite(x.z)
        || math::is_infinite(x.w);
}

template <typename T>
inline constexpr bool is_infinite(const detail::colf<T>& x)
{
    return math::is_infinite(x.r)
        || math::is_infinite(x.g)
        || math::is_infinite(x.b)
        || math::is_infinite(x.a);
}

// =============== is_nan ===============

template <typename T>
inline constexpr bool is_nan(const detail::vecf<2, T>& x)
{
    return math::is_nan(x.x)
        || math::is_nan(x.y);
}

template <typename T>
inline constexpr bool is_nan(const detail::vecf<3, T>& x)
{
    return math::is_nan(x.x)
        || math::is_nan(x.y)
        || math::is_nan(x.z);
}

template <typename T>
inline constexpr bool is_nan(const detail::vecf<4, T>& x)
{
    return math::is_nan(x.x)
        || math::is_nan(x.y)
        || math::is_nan(x.z)
        || math::is_nan(x.w);
}

template <typename T>
inline constexpr bool is_nan(const detail::colf<T>& x)
{
    return math::is_nan(x.r)
        || math::is_nan(x.g)
        || math::is_nan(x.b)
        || math::is_nan(x.a);
}

// =============== round_if_close ===============

template <typename T>
inline constexpr auto round_if_close(
    const detail::vecf<2, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return detail::vecf<2, T>(
        math::round_if_close(x.x, epsilon),
        math::round_if_close(x.y, epsilon)
    );
}

template <typename T>
inline constexpr auto round_if_close(
    const detail::vecf<3, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return detail::vecf<3, T>(
        math::round_if_close(x.x, epsilon),
        math::round_if_close(x.y, epsilon),
        math::round_if_close(x.z, epsilon)
    );
}

template <typename T>
inline constexpr auto round_if_close(
    const detail::vecf<4, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return detail::vecf<4, T>(
        math::round_if_close(x.x, epsilon),
        math::round_if_close(x.y, epsilon),
        math::round_if_close(x.z, epsilon),
        math::round_if_close(x.w, epsilon)
    );
}

template <typename T>
inline constexpr auto round_if_close(
    const detail::colf<T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return detail::colf<T>(
        math::round_if_close(x.r, epsilon),
        math::round_if_close(x.g, epsilon),
        math::round_if_close(x.b, epsilon),
        math::round_if_close(x.a, epsilon)
    );
}

}
}