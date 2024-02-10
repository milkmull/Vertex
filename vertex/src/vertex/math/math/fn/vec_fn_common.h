#pragma once

#include "../detail/vectorize.h"
#include "fn_common.h"

namespace vx {
namespace math {

// =============== min ===============

// scaler binary

template <typename T>
inline constexpr vec2_t<T> min(
    const vec2_t<T>& x,
    T y
)
{
    return vec2_t<T>(
        math::min(x.x, y),
        math::min(x.y, y)
    );
}

template <typename T>
inline constexpr vec3_t<T> min(
    const vec3_t<T>& x,
    T y
)
{
    return vec3_t<T>(
        math::min(x.x, y),
        math::min(x.y, y),
        math::min(x.z, y)
    );
}

template <typename T>
inline constexpr vec4_t<T> min(
    const vec4_t<T>& x,
    T y
)
{
    return vec4_t<T>(
        math::min(x.x, y),
        math::min(x.y, y),
        math::min(x.z, y),
        math::min(x.w, y)
    );
}

// binary

template <typename T>
inline constexpr vec2_t<T> min(
    const vec2_t<T>& x,
    const vec2_t<T>& y
)
{
    return vec2_t<T>(
        math::min(x.x, y.x),
        math::min(x.y, y.y)
    );
}

template <typename T>
inline constexpr vec3_t<T> min(
    const vec3_t<T>& x,
    const vec3_t<T>& y
)
{
    return vec3_t<T>(
        math::min(x.x, y.x),
        math::min(x.y, y.y),
        math::min(x.z, y.z)
    );
}

template <typename T>
inline constexpr vec4_t<T> min(
    const vec4_t<T>& x,
    const vec4_t<T>& y
)
{
    return vec4_t<T>(
        math::min(x.x, y.x),
        math::min(x.y, y.y),
        math::min(x.z, y.z),
        math::min(x.w, y.w)
    );
}

// =============== max ===============

// scaler binary

template <typename T>
inline constexpr vec2_t<T> max(
    const vec2_t<T>& x,
    T y
)
{
    return vec2_t<T>(
        math::max(x.x, y),
        math::max(x.y, y)
    );
}

template <typename T>
inline constexpr vec3_t<T> max(
    const vec3_t<T>& x,
    T y
)
{
    return vec3_t<T>(
        math::max(x.x, y),
        math::max(x.y, y),
        math::max(x.z, y)
    );
}

template <typename T>
inline constexpr vec4_t<T> max(
    const vec4_t<T>& x,
    T y
)
{
    return vec4_t<T>(
        math::max(x.x, y),
        math::max(x.y, y),
        math::max(x.z, y),
        math::max(x.w, y)
    );
}

// binary

template <typename T>
inline constexpr vec2_t<T> max(
    const vec2_t<T>& x,
    const vec2_t<T>& y
)
{
    return vec2_t<T>(
        math::max(x.x, y.x),
        math::max(x.y, y.y)
    );
}

template <typename T>
inline constexpr vec3_t<T> max(
    const vec3_t<T>& x,
    const vec3_t<T>& y
)
{
    return vec3_t<T>(
        math::max(x.x, y.x),
        math::max(x.y, y.y),
        math::max(x.z, y.z)
    );
}

template <typename T>
inline constexpr vec4_t<T> max(
    const vec4_t<T>& x,
    const vec4_t<T>& y
)
{
    return vec4_t<T>(
        math::max(x.x, y.x),
        math::max(x.y, y.y),
        math::max(x.z, y.z),
        math::max(x.w, y.w)
    );
}

// =============== abs ===============

template <typename T>
inline constexpr vec2_t<T> abs(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::abs(x.x),
        math::abs(x.y)
    );
}

template <typename T>
inline constexpr vec3_t<T> abs(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::abs(x.x),
        math::abs(x.y),
        math::abs(x.z)
    );
}

template <typename T>
inline constexpr vec4_t<T> abs(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::abs(x.x),
        math::abs(x.y),
        math::abs(x.z),
        math::abs(x.w)
    );
}

// =============== round ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> round(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::round(x.x),
        math::round(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> round(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::round(x.x),
        math::round(x.y),
        math::round(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> round(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::round(x.x),
        math::round(x.y),
        math::round(x.z),
        math::round(x.w)
    );
}

// =============== trunc ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> trunc(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::trunc(x.x),
        math::trunc(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> trunc(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::trunc(x.x),
        math::trunc(x.y),
        math::trunc(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> trunc(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::trunc(x.x),
        math::trunc(x.y),
        math::trunc(x.z),
        math::trunc(x.w)
    );
}

// =============== floor ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> floor(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::floor(x.x),
        math::floor(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> floor(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::floor(x.x),
        math::floor(x.y),
        math::floor(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> floor(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::floor(x.x),
        math::floor(x.y),
        math::floor(x.z),
        math::floor(x.w)
    );
}

// =============== ceil ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> ceil(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::ceil(x.x),
        math::ceil(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> ceil(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::ceil(x.x),
        math::ceil(x.y),
        math::ceil(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> ceil(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::ceil(x.x),
        math::ceil(x.y),
        math::ceil(x.z),
        math::ceil(x.w)
    );
}

// =============== sign ===============

template <typename T>
inline constexpr vec2_t<T> sign(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::sign(x.x),
        math::sign(x.y)
    );
}

template <typename T>
inline constexpr vec3_t<T> sign(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::sign(x.x),
        math::sign(x.y),
        math::sign(x.z)
    );
}

template <typename T>
inline constexpr vec4_t<T> sign(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::sign(x.x),
        math::sign(x.y),
        math::sign(x.z),
        math::sign(x.w)
    );
}

// =============== fmod ===============

// scaler binary

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> fmod(
    const vec2_t<T>& x,
    T y
)
{
    return vec2_t<T>(
        math::fmod(x.x, y),
        math::fmod(x.y, y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> fmod(
    const vec3_t<T>& x,
    T y
)
{
    return vec3_t<T>(
        math::fmod(x.x, y),
        math::fmod(x.y, y),
        math::fmod(x.z, y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> fmod(
    const vec4_t<T>& x,
    T y
)
{
    return vec4_t<T>(
        math::fmod(x.x, y),
        math::fmod(x.y, y),
        math::fmod(x.z, y),
        math::fmod(x.w, y)
    );
}

// binary

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> fmod(
    const vec2_t<T>& x,
    const vec2_t<T>& y
)
{
    return vec2_t<T>(
        math::fmod(x.x, y.x),
        math::fmod(x.y, y.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> fmod(
    const vec3_t<T>& x,
    const vec3_t<T>& y
)
{
    return vec3_t<T>(
        math::fmod(x.x, y.x),
        math::fmod(x.y, y.y),
        math::fmod(x.z, y.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> fmod(
    const vec4_t<T>& x,
    const vec4_t<T>& y
)
{
    return vec4_t<T>(
        math::fmod(x.x, y.x),
        math::fmod(x.y, y.y),
        math::fmod(x.z, y.z),
        math::fmod(x.w, y.w)
    );
}

// =============== mod ===============

// scaler binary

template <typename T>
inline constexpr auto mod(
    const vec2_t<T>& x,
    T y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec2_t<R>(
        math::mod(x.x, y),
        math::mod(x.y, y)
    );
}

template <typename T>
inline constexpr auto mod(
    const vec3_t<T>& x,
    T y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec3_t<R>(
        math::mod(x.x, y),
        math::mod(x.y, y),
        math::mod(x.z, y)
    );
}

template <typename T>
inline constexpr auto mod(
    const vec4_t<T>& x,
    T y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec4_t<R>(
        math::mod(x.x, y),
        math::mod(x.y, y),
        math::mod(x.z, y),
        math::mod(x.w, y)
    );
}

// binary

template <typename T>
inline constexpr auto mod(
    const vec2_t<T>& x,
    const vec2_t<T>& y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec2_t<R>(
        math::mod(x.x, y.x),
        math::mod(x.y, y.y)
    );
}

template <typename T>
inline constexpr auto mod(
    const vec3_t<T>& x,
    const vec3_t<T>& y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec3_t<R>(
        math::mod(x.x, y.x),
        math::mod(x.y, y.y),
        math::mod(x.z, y.z)
    );
}

template <typename T>
inline constexpr auto mod(
    const vec4_t<T>& x,
    const vec4_t<T>& y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec4_t<R>(
        math::mod(x.x, y.x),
        math::mod(x.y, y.y),
        math::mod(x.z, y.z),
        math::mod(x.w, y.w)
    );
}

// =============== fract ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> fract(
    const vec2_t<T>& x
)
{
    return vec2_t<T>(
        math::fract(x.x),
        math::fract(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> fract(
    const vec3_t<T>& x
)
{
    return vec3_t<T>(
        math::fract(x.x),
        math::fract(x.y),
        math::fract(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> fract(
    const vec4_t<T>& x
)
{
    return vec4_t<T>(
        math::fract(x.x),
        math::fract(x.y),
        math::fract(x.z),
        math::fract(x.w)
    );
}

// =============== modf ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> modf(
    const vec2_t<T>& x,
    vec2_t<T>& intpart
)
{
    return vec2_t<T>(
        math::modf(x.x, intpart.x),
        math::modf(x.y, intpart.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> modf(
    const vec3_t<T>& x,
    vec3_t<T>& intpart
)
{
    return vec3_t<T>(
        math::modf(x.x, intpart.x),
        math::modf(x.y, intpart.y),
        math::modf(x.z, intpart.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> modf(
    const vec4_t<T>& x,
    vec4_t<T>& intpart
)
{
    return vec4_t<T>(
        math::modf(x.x, intpart.x),
        math::modf(x.y, intpart.y),
        math::modf(x.z, intpart.z),
        math::modf(x.w, intpart.w)
    );
}

// =============== frexp ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec2_t<T> frexp(
    const vec2_t<T>& x,
    vec2_t<int>& exp
)
{
    return vec2_t<T>(
        math::frexp(x.x, exp.x),
        math::frexp(x.y, exp.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec3_t<T> frexp(
    const vec3_t<T>& x,
    vec3_t<int>& exp
)
{
    return vec3_t<T>(
        math::frexp(x.x, exp.x),
        math::frexp(x.y, exp.y),
        math::frexp(x.z, exp.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec4_t<T> frexp(
    const vec4_t<T>& x,
    vec4_t<int>& exp
)
{
    return vec4_t<T>(
        math::frexp(x.x, exp.x),
        math::frexp(x.y, exp.y),
        math::frexp(x.z, exp.z),
        math::frexp(x.w, exp.w)
    );
}

// =============== snap ===============

// scaler binary

template <typename T>
inline constexpr vec2_t<T> snap(
    const vec2_t<T>& x,
    T y
)
{
    return vec2_t<T>(
        math::snap(x.x, interval),
        math::snap(x.y, interval)
    );
}

template <typename T>
inline constexpr vec3_t<T> snap(
    const vec3_t<T>& x,
    T interval
)
{
    return vec3_t<T>(
        math::snap(x.x, interval),
        math::snap(x.y, interval),
        math::snap(x.z, interval)
    );
}

template <typename T>
inline constexpr vec4_t<T> snap(
    const vec4_t<T>& x,
    T interval
)
{
    return vec4_t<T>(
        math::snap(x.x, interval),
        math::snap(x.y, interval),
        math::snap(x.z, interval),
        math::snap(x.w, interval)
    );
}

// binary

template <typename T>
inline constexpr vec2_t<T> snap(
    const vec2_t<T>& x,
    const vec2_t<T>& interval
)
{
    return vec2_t<T>(
        math::snap(x.x, interval.x),
        math::snap(x.y, interval.y)
    );
}

template <typename T>
inline constexpr vec3_t<T> snap(
    const vec3_t<T>& x,
    const vec3_t<T>& interval
)
{
    return vec3_t<T>(
        math::snap(x.x, interval.x),
        math::snap(x.y, interval.y),
        math::snap(x.z, interval.z)
    );
}

template <typename T>
inline constexpr vec4_t<T> snap(
    const vec4_t<T>& x,
    const vec4_t<T>& interval
)
{
    return vec4_t<T>(
        math::snap(x.x, interval.x),
        math::snap(x.y, interval.y),
        math::snap(x.z, interval.z),
        math::snap(x.w, interval.w)
    );
}

// =============== clamp ===============

// scaler

template <typename T>
inline constexpr vec2_t<T> clamp(
    const vec2_t<T>& x,
    T min,
    T max
)
{
    return vec2_t<T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max)
    );
}

template <typename T>
inline constexpr vec3_t<T> clamp(
    const vec3_t<T>& x,
    T min,
    T max
)
{
    return vec3_t<T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max),
        math::clamp(x.z, min, max)
    );
}

template <typename T>
inline constexpr vec4_t<T> clamp(
    const vec4_t<T>& x,
    T min,
    T max
)
{
    return vec4_t<T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max),
        math::clamp(x.z, min, max),
        math::clamp(x.w, min, max)
    );
}

// vector

template <typename T>
inline constexpr vec2_t<T> clamp(
    const vec2_t<T>& x,
    const vec2_t<T>& min,
    const vec2_t<T>& max
)
{
    return vec2_t<T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y)
    );
}

template <typename T>
inline constexpr vec3_t<T> clamp(
    const vec3_t<T>& x,
    const vec3_t<T>& min,
    const vec3_t<T>& max
)
{
    return vec3_t<T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y),
        math::clamp(x.z, min.z, max.z)
    );
}

template <typename T>
inline constexpr vec4_t<T> clamp(
    const vec4_t<T>& x,
    const vec4_t<T>& min,
    const vec4_t<T>& max
)
{
    return vec4_t<T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y),
        math::clamp(x.z, min.z, max.z),
        math::clamp(x.w, min.w, max.w)
    );
}

}
}
