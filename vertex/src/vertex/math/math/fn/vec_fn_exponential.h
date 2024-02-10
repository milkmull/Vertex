#pragma once

#include "../detail/vectorize.h"
#include "fn_exponential.h"

namespace vx {
namespace math {

// =============== pow ===============

// scaler binary

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> pow(
    const vec2_t<T>& x,
    T y
)
{
    return vec2_t<T>(
        math::pow(x.x, y),
        math::pow(x.y, y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> pow(
    const vec3_t<T>& x,
    T y
)
{
    return vec3_t<T>(
        math::pow(x.x, y),
        math::pow(x.y, y),
        math::pow(x.z, y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> pow(
    const vec4_t<T>& x,
    T y
)
{
    return vec4_t<T>(
        math::pow(x.x, y),
        math::pow(x.y, y),
        math::pow(x.z, y),
        math::pow(x.w, y)
    );
}

// binary

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> pow(
    const vec2_t<T>& x,
    const vec2_t<T>& y
)
{
    return vec2_t<T>(
        math::pow(x.x, y.x),
        math::pow(x.y, y.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> pow(
    const vec3_t<T>& x,
    const vec3_t<T>& y
)
{
    return vec3_t<T>(
        math::pow(x.x, y.x),
        math::pow(x.y, y.y),
        math::pow(x.z, y.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> pow(
    const vec4_t<T>& x,
    const vec4_t<T>& y
)
{
    return vec4_t<T>(
        math::pow(x.x, y.x),
        math::pow(x.y, y.y),
        math::pow(x.z, y.z),
        math::pow(x.w, y.w)
    );
}

// =============== exp ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> exp(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::exp(x.x),
        math::exp(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> exp(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::exp(x.x),
        math::exp(x.y),
        math::exp(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> exp(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::exp(x.x),
        math::exp(x.y),
        math::exp(x.z),
        math::exp(x.w)
    );
}

// =============== exp2 ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> exp2(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::exp2(x.x),
        math::exp2(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> exp2(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::exp2(x.x),
        math::exp2(x.y),
        math::exp2(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> exp2(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::exp2(x.x),
        math::exp2(x.y),
        math::exp2(x.z),
        math::exp2(x.w)
    );
}

// =============== log ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> log(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::log(x.x),
        math::log(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> log(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::log(x.x),
        math::log(x.y),
        math::log(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> log(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::log(x.x),
        math::log(x.y),
        math::log(x.z),
        math::log(x.w)
    );
}

// =============== log2 ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> log2(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::log2(x.x),
        math::log2(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> log2(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::log2(x.x),
        math::log2(x.y),
        math::log2(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> log2(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::log2(x.x),
        math::log2(x.y),
        math::log2(x.z),
        math::log2(x.w)
    );
}

// =============== sqrt ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> sqrt(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::sqrt(x.x),
        math::sqrt(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> sqrt(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::sqrt(x.x),
        math::sqrt(x.y),
        math::sqrt(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> sqrt(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::sqrt(x.x),
        math::sqrt(x.y),
        math::sqrt(x.z),
        math::sqrt(x.w)
    );
}

// =============== inverse_sqrt ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> inverse_sqrt(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::inverse_sqrt(x.x),
        math::inverse_sqrt(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> inverse_sqrt(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::inverse_sqrt(x.x),
        math::inverse_sqrt(x.y),
        math::inverse_sqrt(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> inverse_sqrt(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::inverse_sqrt(x.x),
        math::inverse_sqrt(x.y),
        math::inverse_sqrt(x.z),
        math::inverse_sqrt(x.w)
    );
}

}
}
