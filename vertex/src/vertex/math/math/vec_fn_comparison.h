#pragma once

#include "detail/vectorize.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== is_equal_approx ===============

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::vecf<2, T>& v1,
    const detail::vecf<2, T>& v2,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(v1.x, v2.x, epsilon)
        && math::is_equal_approx(v1.y, v2.y, epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::vecf<3, T>& v1,
    const detail::vecf<3, T>& v2,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(v1.x, v2.x, epsilon)
        && math::is_equal_approx(v1.y, v2.y, epsilon)
        && math::is_equal_approx(v1.z, v2.z, epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::vecf<4, T>& v1,
    const detail::vecf<4, T>& v2,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(v1.x, v2.x, epsilon)
        && math::is_equal_approx(v1.y, v2.y, epsilon)
        && math::is_equal_approx(v1.z, v2.z, epsilon)
        && math::is_equal_approx(v1.w, v2.w, epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::col<T>& v1,
    const detail::col<T>& v2,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(v1.r, v2.r, epsilon)
        && math::is_equal_approx(v1.g, v2.g, epsilon)
        && math::is_equal_approx(v1.b, v2.b, epsilon)
        && math::is_equal_approx(v1.a, v2.a, epsilon);
}

// =============== is_zero_approx ===============

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::vecf<2, T>& v,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(v.x, epsilon)
        && math::is_zero_approx(v.y, epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::vecf<3, T>& v,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(v.x, epsilon)
        && math::is_zero_approx(v.y, epsilon)
        && math::is_zero_approx(v.z, epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::vecf<4, T>& v,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(v.x, , epsilon)
        && math::is_zero_approx(v.y, , epsilon)
        && math::is_zero_approx(v.z, , epsilon)
        && math::is_zero_approx(v.w, , epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::col<T>& v,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(v.r, epsilon)
        && math::is_zero_approx(v.g, epsilon)
        && math::is_zero_approx(v.b, epsilon)
        && math::is_zero_approx(v.a, epsilon);
}

// =============== is_finite ===============

template <typename T>
inline constexpr bool is_finite(const detail::vecf<2, T>& v)
{
    return math::is_finite(v.x)
        && math::is_finite(v.y);
}

template <typename T>
inline constexpr bool is_finite(const detail::vecf<3, T>& v)
{
    return math::is_finite(v.x)
        && math::is_finite(v.y)
        && math::is_finite(v.z);
}

template <typename T>
inline constexpr bool is_finite(const detail::vecf<4, T>& v)
{
    return math::is_finite(v.x)
        && math::is_finite(v.y)
        && math::is_finite(v.z)
        && math::is_finite(v.w);
}

template <typename T>
inline constexpr bool is_finite(const detail::vec<4, T, detail::vec_t::col, detail::val_t::floating_point>& v)
{
    return math::is_finite(v.r)
        && math::is_finite(v.g)
        && math::is_finite(v.b)
        && math::is_finite(v.a);
}

// =============== is_infinite ===============

template <typename T>
inline constexpr bool is_infinite(const detail::vecf<2, T>& v)
{
    return math::is_infinite(v.x)
        || math::is_infinite(v.y);
}

template <typename T>
inline constexpr bool is_infinite(const detail::vecf<3, T>& v)
{
    return math::is_infinite(v.x)
        || math::is_infinite(v.y)
        || math::is_infinite(v.z);
}

template <typename T>
inline constexpr bool is_infinite(const detail::vecf<4, T>& v)
{
    return math::is_infinite(v.x)
        || math::is_infinite(v.y)
        || math::is_infinite(v.z)
        || math::is_infinite(v.w);
}

template <typename T>
inline constexpr bool is_infinite(const detail::col<T>& v)
{
    return math::is_infinite(v.r)
        || math::is_infinite(v.g)
        || math::is_infinite(v.b)
        || math::is_infinite(v.a);
}

// =============== is_nan ===============

template <typename T>
inline constexpr bool is_nan(const detail::vecf<2, T>& v)
{
    return math::is_nan(v.x)
        || math::is_nan(v.y);
}

template <typename T>
inline constexpr bool is_nan(const detail::vecf<3, T>& v)
{
    return math::is_nan(v.x)
        || math::is_nan(v.y)
        || math::is_nan(v.z);
}

template <typename T>
inline constexpr bool is_nan(const detail::vecf<4, T>& v)
{
    return math::is_nan(v.x)
        || math::is_nan(v.y)
        || math::is_nan(v.z)
        || math::is_nan(v.w);
}

template <typename T>
inline constexpr bool is_nan(const detail::col<T>& v)
{
    return math::is_nan(v.r)
        || math::is_nan(v.g)
        || math::is_nan(v.b)
        || math::is_nan(v.a);
}

// =============== round_if_close ===============

template <typename T>
inline constexpr auto round_if_close(
    const detail::vecf<2, T>& v,
    const T epsilon = math::epsilon<T>
)
{
    return detail::vecf<2, T>(
        math::round_if_close(v.x, epsilon),
        math::round_if_close(v.y, epsilon)
    );
}

template <typename T>
inline constexpr auto round_if_close(
    const detail::vecf<3, T>& v,
    const T epsilon = math::epsilon<T>
)
{
    return detail::vecf<3, T>(
        math::round_if_close(v.x, epsilon),
        math::round_if_close(v.y, epsilon),
        math::round_if_close(v.z, epsilon)
    );
}

template <typename T>
inline constexpr auto round_if_close(
    const detail::vecf<4, T>& v,
    const T epsilon = math::epsilon<T>
)
{
    return detail::vecf<4, T>(
        math::round_if_close(v.x, epsilon),
        math::round_if_close(v.y, epsilon),
        math::round_if_close(v.z, epsilon),
        math::round_if_close(v.w, epsilon)
    );
}

template <typename T>
inline constexpr auto round_if_close(
    const detail::col<T>& v,
    const T epsilon = math::epsilon<T>
)
{
    return detail::col<T>(
        math::round_if_close(v.r, epsilon),
        math::round_if_close(v.g, epsilon),
        math::round_if_close(v.b, epsilon),
        math::round_if_close(v.a, epsilon)
    );
}

}
}