#pragma once

#include "fn_trigonometric.h"

namespace vx {
namespace math {

// =============== radians ===============

template <typename T>
inline constexpr auto radians(
    const vec<2, T>& deg
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
    const vec<3, T>& deg
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
    const vec<4, T>& deg
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
    const vec<2, T>& rad
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
    const vec<3, T>& rad
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
    const vec<4, T>& rad
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
inline constexpr vec<2, T> sin(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::sin(x.x),
        math::sin(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> sin(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::sin(x.x),
        math::sin(x.y),
        math::sin(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> sin(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::sin(x.x),
        math::sin(x.y),
        math::sin(x.z),
        math::sin(x.w)
    );
}

// =============== cos ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> cos(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::cos(x.x),
        math::cos(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> cos(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::cos(x.x),
        math::cos(x.y),
        math::cos(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> cos(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::cos(x.x),
        math::cos(x.y),
        math::cos(x.z),
        math::cos(x.w)
    );
}

// =============== tan ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> tan(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::tan(x.x),
        math::tan(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> tan(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::tan(x.x),
        math::tan(x.y),
        math::tan(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> tan(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::tan(x.x),
        math::tan(x.y),
        math::tan(x.z),
        math::tan(x.w)
    );
}

// =============== asin ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> asin(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::asin(x.x),
        math::asin(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> asin(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::asin(x.x),
        math::asin(x.y),
        math::asin(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> asin(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::asin(x.x),
        math::asin(x.y),
        math::asin(x.z),
        math::asin(x.w)
    );
}

// =============== acos ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> acos(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::acos(x.x),
        math::acos(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> acos(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::acos(x.x),
        math::acos(x.y),
        math::acos(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> acos(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::acos(x.x),
        math::acos(x.y),
        math::acos(x.z),
        math::acos(x.w)
    );
}

// =============== atan ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> atan(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::atan(x.x),
        math::atan(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> atan(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::atan(x.x),
        math::atan(x.y),
        math::atan(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> atan(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::atan(x.x),
        math::atan(x.y),
        math::atan(x.z),
        math::atan(x.w)
    );
}

// =============== atan2 ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> atan2(
    const vec<2, T>& y,
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::atan2(y.x, x.x),
        math::atan2(y.y, x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> atan2(
    const vec<3, T>& y,
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::atan2(y.x, x.x),
        math::atan2(y.y, x.y),
        math::atan2(y.z, x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> atan2(
    const vec<4, T>& y,
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::atan2(y.x, x.x),
        math::atan2(y.y, x.y),
        math::atan2(y.z, x.z),
        math::atan2(y.w, x.w)
    );
}

// =============== sinh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> sinh(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::sinh(x.x),
        math::sinh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> sinh(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::sinh(x.x),
        math::sinh(x.y),
        math::sinh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> sinh(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::sinh(x.x),
        math::sinh(x.y),
        math::sinh(x.z),
        math::sinh(x.w)
    );
}

// =============== cosh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> cosh(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::cosh(x.x),
        math::cosh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> cosh(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::cosh(x.x),
        math::cosh(x.y),
        math::cosh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> cosh(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::cosh(x.x),
        math::cosh(x.y),
        math::cosh(x.z),
        math::cosh(x.w)
    );
}

// =============== tanh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> tanh(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::tanh(x.x),
        math::tanh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> tanh(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::tanh(x.x),
        math::tanh(x.y),
        math::tanh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> tanh(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::tanh(x.x),
        math::tanh(x.y),
        math::tanh(x.z),
        math::tanh(x.w)
    );
}

// =============== asinh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> asinh(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::asinh(x.x),
        math::asinh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> asinh(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::asinh(x.x),
        math::asinh(x.y),
        math::asinh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> asinh(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::asinh(x.x),
        math::asinh(x.y),
        math::asinh(x.z),
        math::asinh(x.w)
    );
}

// =============== acosh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> acosh(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::acosh(x.x),
        math::acosh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> acosh(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::acosh(x.x),
        math::acosh(x.y),
        math::acosh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> acosh(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::acosh(x.x),
        math::acosh(x.y),
        math::acosh(x.z),
        math::acosh(x.w)
    );
}

// =============== atanh ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> atanh(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::atanh(x.x),
        math::atanh(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> atanh(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::atanh(x.x),
        math::atanh(x.y),
        math::atanh(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> atanh(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::atanh(x.x),
        math::atanh(x.y),
        math::atanh(x.z),
        math::atanh(x.w)
    );
}

}
}