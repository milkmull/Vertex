#pragma once

#include "fn_trigonometric.h"

namespace vx {
namespace math {

// =============== radians ===============

template <typename T>
inline constexpr auto radians(
    const vec2_t<T>& deg
)
{
    using R = decltype(math::radians(std::declval<T>()));
    return vec2_t<R>(
        math::radians(deg.x),
        math::radians(deg.y)
    );
}

template <typename T>
inline constexpr auto radians(
    const vec3_t<T>& deg
)
{
    using R = decltype(math::radians(std::declval<T>()));
    return vec3_t<R>(
        math::radians(deg.x),
        math::radians(deg.y),
        math::radians(deg.z)
    );
}

template <typename T>
inline constexpr auto radians(
    const vec4_t<T>& deg
)
{
    using R = decltype(math::radians(std::declval<T>()));
    return vec4_t<R>(
        math::radians(deg.x),
        math::radians(deg.y),
        math::radians(deg.z),
        math::radians(deg.w)
    );
}

// =============== degrees ===============

template <typename T>
inline constexpr auto degrees(
    const vec2_t<T>& rad
)
{
    using R = decltype(math::degrees(std::declval<T>()));
    return vec2_t<R>(
        math::degrees(rad.x),
        math::degrees(rad.y)
    );
}

template <typename T>
inline constexpr auto degrees(
    const vec3_t<T>& rad
)
{
    using R = decltype(math::degrees(std::declval<T>()));
    return vec3_t<R>(
        math::degrees(rad.x),
        math::degrees(rad.y),
        math::degrees(rad.z)
    );
}

template <typename T>
inline constexpr auto degrees(
    const vec4_t<T>& rad
)
{
    using R = decltype(math::degrees(std::declval<T>()));
    return vec4_t<R>(
        math::degrees(rad.x),
        math::degrees(rad.y),
        math::degrees(rad.z),
        math::degrees(rad.w)
    );
}

// =============== sin ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> sin(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::sin(x.x),
        math::sin(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> sin(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::sin(x.x),
        math::sin(x.y),
        math::sin(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> sin(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::sin(x.x),
        math::sin(x.y),
        math::sin(x.z),
        math::sin(x.w)
    );
}

// =============== cos ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> cos(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::cos(x.x),
        math::cos(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> cos(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::cos(x.x),
        math::cos(x.y),
        math::cos(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> cos(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::cos(x.x),
        math::cos(x.y),
        math::cos(x.z),
        math::cos(x.w)
    );
}

// =============== tan ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> tan(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::tan(x.x),
        math::tan(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> tan(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::tan(x.x),
        math::tan(x.y),
        math::tan(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> tan(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::tan(x.x),
        math::tan(x.y),
        math::tan(x.z),
        math::tan(x.w)
    );
}

// =============== asin ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> asin(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::asin(x.x),
        math::asin(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> asin(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::asin(x.x),
        math::asin(x.y),
        math::asin(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> asin(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::asin(x.x),
        math::asin(x.y),
        math::asin(x.z),
        math::asin(x.w)
    );
}

// =============== acos ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> acos(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::acos(x.x),
        math::acos(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> acos(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::acos(x.x),
        math::acos(x.y),
        math::acos(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> acos(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::acos(x.x),
        math::acos(x.y),
        math::acos(x.z),
        math::acos(x.w)
    );
}

// =============== atan ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> atan(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::atan(x.x),
        math::atan(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> atan(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::atan(x.x),
        math::atan(x.y),
        math::atan(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> atan(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::atan(x.x),
        math::atan(x.y),
        math::atan(x.z),
        math::atan(x.w)
    );
}

// =============== atan2 ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> atan2(
    const vec2_t<T>& y,
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::atan2(y.x, x.x),
        math::atan2(y.y, x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> atan2(
    const vec3_t<T>& y,
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::atan2(y.x, x.x),
        math::atan2(y.y, x.y),
        math::atan2(y.z, x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> atan2(
    const vec4_t<T>& y,
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::atan2(y.x, x.x),
        math::atan2(y.y, x.y),
        math::atan2(y.z, x.z),
        math::atan2(y.w, x.w)
    );
}

// =============== sinh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> sinh(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::sinh(x.x),
        math::sinh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> sinh(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::sinh(x.x),
        math::sinh(x.y),
        math::sinh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> sinh(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::sinh(x.x),
        math::sinh(x.y),
        math::sinh(x.z),
        math::sinh(x.w)
    );
}

// =============== cosh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> cosh(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::cosh(x.x),
        math::cosh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> cosh(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::cosh(x.x),
        math::cosh(x.y),
        math::cosh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> cosh(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::cosh(x.x),
        math::cosh(x.y),
        math::cosh(x.z),
        math::cosh(x.w)
    );
}

// =============== tanh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> tanh(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::tanh(x.x),
        math::tanh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> tanh(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::tanh(x.x),
        math::tanh(x.y),
        math::tanh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> tanh(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::tanh(x.x),
        math::tanh(x.y),
        math::tanh(x.z),
        math::tanh(x.w)
    );
}

// =============== asinh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> asinh(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::asinh(x.x),
        math::asinh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> asinh(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::asinh(x.x),
        math::asinh(x.y),
        math::asinh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> asinh(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::asinh(x.x),
        math::asinh(x.y),
        math::asinh(x.z),
        math::asinh(x.w)
    );
}

// =============== acosh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> acosh(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::acosh(x.x),
        math::acosh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> acosh(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::acosh(x.x),
        math::acosh(x.y),
        math::acosh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> acosh(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::acosh(x.x),
        math::acosh(x.y),
        math::acosh(x.z),
        math::acosh(x.w)
    );
}

// =============== atanh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> atanh(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::atanh(x.x),
        math::atanh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> atanh(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::atanh(x.x),
        math::atanh(x.y),
        math::atanh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> atanh(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::atanh(x.x),
        math::atanh(x.y),
        math::atanh(x.z),
        math::atanh(x.w)
    );
}

}
}