#pragma once

#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== is_zero_approx ===============

template <typename T>
inline constexpr bool is_zero_approx(
    const vec<2, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const vec<3, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon)
        && math::is_zero_approx(x.z, epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const vec<4, T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon)
        && math::is_zero_approx(x.z, epsilon)
        && math::is_zero_approx(x.w, epsilon);
}

// =============== is_equal_approx ===============

template <typename T>
inline constexpr bool is_equal_approx(
    const vec<2, T>& a,
    const vec<2, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const vec<3, T>& a,
    const vec<3, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon)
        && math::is_equal_approx(a.z, b.z, epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const vec<4, T>& a,
    const vec<4, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon)
        && math::is_equal_approx(a.z, b.z, epsilon)
        && math::is_equal_approx(a.w, b.w, epsilon);
}

// =============== is_not_equal_approx ===============

template <typename T>
inline constexpr bool is_not_equal_approx(
    const vec<2, T>& a,
    const vec<2, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_not_equal_approx(a.x, b.x, epsilon)
        && math::is_not_equal_approx(a.y, b.y, epsilon);
}

template <typename T>
inline constexpr bool is_not_equal_approx(
    const vec<3, T>& a,
    const vec<3, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_not_equal_approx(a.x, b.x, epsilon)
        && math::is_not_equal_approx(a.y, b.y, epsilon)
        && math::is_not_equal_approx(a.z, b.z, epsilon);
}

template <typename T>
inline constexpr bool is_not_equal_approx(
    const vec<4, T>& a,
    const vec<4, T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_not_equal_approx(a.x, b.x, epsilon)
        && math::is_not_equal_approx(a.y, b.y, epsilon)
        && math::is_not_equal_approx(a.z, b.z, epsilon)
        && math::is_not_equal_approx(a.w, b.w, epsilon);
}

// =============== is_finite ===============

template <typename T>
inline constexpr bool is_finite(const vec<2, T>& x)
{
    return math::is_finite(x.x)
        && math::is_finite(x.y);
}

template <typename T>
inline constexpr bool is_finite(const vec<3, T>& x)
{
    return math::is_finite(x.x)
        && math::is_finite(x.y)
        && math::is_finite(x.z);
}

template <typename T>
inline constexpr bool is_finite(const vec<4, T>& x)
{
    return math::is_finite(x.x)
        && math::is_finite(x.y)
        && math::is_finite(x.z)
        && math::is_finite(x.w);
}

// =============== is_infinite ===============

template <typename T>
inline constexpr bool is_infinite(const vec<2, T>& x)
{
    return math::is_infinite(x.x)
        || math::is_infinite(x.y);
}

template <typename T>
inline constexpr bool is_infinite(const vec<3, T>& x)
{
    return math::is_infinite(x.x)
        || math::is_infinite(x.y)
        || math::is_infinite(x.z);
}

template <typename T>
inline constexpr bool is_infinite(const vec<4, T>& x)
{
    return math::is_infinite(x.x)
        || math::is_infinite(x.y)
        || math::is_infinite(x.z)
        || math::is_infinite(x.w);
}

// =============== is_nan ===============

template <typename T>
inline constexpr bool is_nan(const vec<2, T>& x)
{
    return math::is_nan(x.x)
        || math::is_nan(x.y);
}

template <typename T>
inline constexpr bool is_nan(const vec<3, T>& x)
{
    return math::is_nan(x.x)
        || math::is_nan(x.y)
        || math::is_nan(x.z);
}

template <typename T>
inline constexpr bool is_nan(const vec<4, T>& x)
{
    return math::is_nan(x.x)
        || math::is_nan(x.y)
        || math::is_nan(x.z)
        || math::is_nan(x.w);
}

// =============== is_nan ===============

template <typename T>
inline constexpr vec<2, T> round_epsilon(const vec<2, T>& x, const T epsilon = math::epsilon<T>)
{
    return vec<2, T>(
        math::round_epsilon(x.x, epsilon),
        math::round_epsilon(x.y, epsilon)
    );
}

template <typename T>
inline constexpr vec<3, T> round_epsilon(const vec<3, T>& x, const T epsilon = math::epsilon<T>)
{
    return vec<3, T>(
        math::round_epsilon(x.x, epsilon),
        math::round_epsilon(x.y, epsilon),
        math::round_epsilon(x.z, epsilon)
    );
}

template <typename T>
inline constexpr vec<4, T> round_epsilon(const vec<4, T>& x, const T epsilon = math::epsilon<T>)
{
    return vec<4, T>(
        math::round_epsilon(x.x, epsilon),
        math::round_epsilon(x.y, epsilon),
        math::round_epsilon(x.z, epsilon),
        math::round_epsilon(x.w, epsilon)
    );
}

}
}