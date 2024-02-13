#pragma once

#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== is_zero_approx ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_zero_approx(
    const vec2_t<T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_zero_approx(
    const vec3_t<T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon)
        && math::is_zero_approx(x.z, epsilon);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_zero_approx(
    const vec4_t<T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon)
        && math::is_zero_approx(x.z, epsilon)
        && math::is_zero_approx(x.w, epsilon);
}

// =============== is_equal_approx ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_equal_approx(
    const vec2_t<T>& a,
    const vec2_t<T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_equal_approx(
    const vec3_t<T>& a,
    const vec3_t<T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon)
        && math::is_equal_approx(a.z, b.z, epsilon);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_equal_approx(
    const vec4_t<T>& a,
    const vec4_t<T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon)
        && math::is_equal_approx(a.z, b.z, epsilon)
        && math::is_equal_approx(a.w, b.w, epsilon);
}

// =============== is_finite ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_finite(const vec2_t<T>& x)
{
    return math::is_finite(x.x)
        && math::is_finite(x.y);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_finite(const vec3_t<T>& x)
{
    return math::is_finite(x.x)
        && math::is_finite(x.y)
        && math::is_finite(x.z);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_finite(const vec4_t<T>& x)
{
    return math::is_finite(x.x)
        && math::is_finite(x.y)
        && math::is_finite(x.z)
        && math::is_finite(x.w);
}

// =============== is_infinite ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_infinite(const vec2_t<T>& x)
{
    return math::is_infinite(x.x)
        || math::is_infinite(x.y);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_infinite(const vec3_t<T>& x)
{
    return math::is_infinite(x.x)
        || math::is_infinite(x.y)
        || math::is_infinite(x.z);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_infinite(const vec4_t<T>& x)
{
    return math::is_infinite(x.x)
        || math::is_infinite(x.y)
        || math::is_infinite(x.z)
        || math::is_infinite(x.w);
}

// =============== is_nan ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_nan(const vec2_t<T>& x)
{
    return math::is_nan(x.x)
        || math::is_nan(x.y);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_nan(const vec3_t<T>& x)
{
    return math::is_nan(x.x)
        || math::is_nan(x.y)
        || math::is_nan(x.z);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_nan(const vec4_t<T>& x)
{
    return math::is_nan(x.x)
        || math::is_nan(x.y)
        || math::is_nan(x.z)
        || math::is_nan(x.w);
}

}
}