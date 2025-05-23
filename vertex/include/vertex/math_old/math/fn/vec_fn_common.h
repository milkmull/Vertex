#pragma once

#include "fn_common.h"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// min
///////////////////////////////////////////////////////////////////////////////

// scalar binary

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> min(
    const vec<2, T>& x,
    T y
)
{
    return vec<2, T>(
        math::min(x.x, y),
        math::min(x.y, y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> min(
    const vec<3, T>& x,
    T y
)
{
    return vec<3, T>(
        math::min(x.x, y),
        math::min(x.y, y),
        math::min(x.z, y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> min(
    const vec<4, T>& x,
    T y
)
{
    return vec<4, T>(
        math::min(x.x, y),
        math::min(x.y, y),
        math::min(x.z, y),
        math::min(x.w, y)
    );
}

// binary

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> min(
    const vec<2, T>& x,
    const vec<2, T>& y
)
{
    return vec<2, T>(
        math::min(x.x, y.x),
        math::min(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> min(
    const vec<3, T>& x,
    const vec<3, T>& y
)
{
    return vec<3, T>(
        math::min(x.x, y.x),
        math::min(x.y, y.y),
        math::min(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> min(
    const vec<4, T>& x,
    const vec<4, T>& y
)
{
    return vec<4, T>(
        math::min(x.x, y.x),
        math::min(x.y, y.y),
        math::min(x.z, y.z),
        math::min(x.w, y.w)
    );
}

// component

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the component-wise minimum of a vector.
///
/// @tparam T The type of the vector components.
/// @param x The vector.
/// 
/// @return The component-wise minimum of the vector.
///////////////////////////////////////////////////////////////////////////////
template <typename T>
VX_FORCE_INLINE constexpr T cmin(
    const vec<2, T>& x
)
{
    return math::min(x.x, x.y);
}

template <typename T>
VX_FORCE_INLINE constexpr T cmin(
    const vec<3, T>& x
)
{
    return math::min({ x.x, x.y, x.z });
}

template <typename T>
VX_FORCE_INLINE constexpr T cmin(
    const vec<4, T>& x
)
{
    return math::min({ x.x, x.y, x.z, x.w });
}

///////////////////////////////////////////////////////////////////////////////
// max
///////////////////////////////////////////////////////////////////////////////

// scalar binary

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> max(
    const vec<2, T>& x,
    T y
)
{
    return vec<2, T>(
        math::max(x.x, y),
        math::max(x.y, y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> max(
    const vec<3, T>& x,
    T y
)
{
    return vec<3, T>(
        math::max(x.x, y),
        math::max(x.y, y),
        math::max(x.z, y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> max(
    const vec<4, T>& x,
    T y
)
{
    return vec<4, T>(
        math::max(x.x, y),
        math::max(x.y, y),
        math::max(x.z, y),
        math::max(x.w, y)
    );
}

// binary

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> max(
    const vec<2, T>& x,
    const vec<2, T>& y
)
{
    return vec<2, T>(
        math::max(x.x, y.x),
        math::max(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> max(
    const vec<3, T>& x,
    const vec<3, T>& y
)
{
    return vec<3, T>(
        math::max(x.x, y.x),
        math::max(x.y, y.y),
        math::max(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> max(
    const vec<4, T>& x,
    const vec<4, T>& y
)
{
    return vec<4, T>(
        math::max(x.x, y.x),
        math::max(x.y, y.y),
        math::max(x.z, y.z),
        math::max(x.w, y.w)
    );
}

// component

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the component-wise maximum of a vector.
///
/// @tparam T The type of the vector components.
/// @param x The vector.
/// 
/// @return The component-wise maximum of the vector.
///////////////////////////////////////////////////////////////////////////////
template <typename T>
VX_FORCE_INLINE constexpr T cmax(
    const vec<2, T>& x
)
{
    return math::max(x.x, x.y);
}

template <typename T>
VX_FORCE_INLINE constexpr T cmax(
    const vec<3, T>& x
)
{
    return math::max({ x.x, x.y, x.z });
}

template <typename T>
VX_FORCE_INLINE constexpr T cmax(
    const vec<4, T>& x
)
{
    return math::max({ x.x, x.y, x.z, x.w });
}

///////////////////////////////////////////////////////////////////////////////
// csum
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the component-wise sum of a vector.
///
/// @tparam T The type of the vector components.
/// @param x The vector.
/// 
/// @return The component-wise sum of the vector.
///////////////////////////////////////////////////////////////////////////////
template <typename T>
VX_FORCE_INLINE constexpr T csum(
    const vec<2, T>& x
)
{
    return x.x + x.y;
}

template <typename T>
VX_FORCE_INLINE constexpr T csum(
    const vec<3, T>& x
)
{
    return x.x + x.y + x.z;
}

template <typename T>
VX_FORCE_INLINE constexpr T csum(
    const vec<4, T>& x
)
{
    return x.x + x.y + x.z + x.w;
}

///////////////////////////////////////////////////////////////////////////////
// abs
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> abs(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::abs(x.x),
        math::abs(x.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> abs(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::abs(x.x),
        math::abs(x.y),
        math::abs(x.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> abs(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::abs(x.x),
        math::abs(x.y),
        math::abs(x.z),
        math::abs(x.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// round
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> round(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::round(x.x),
        math::round(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> round(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::round(x.x),
        math::round(x.y),
        math::round(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> round(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::round(x.x),
        math::round(x.y),
        math::round(x.z),
        math::round(x.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// trunc
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> trunc(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::trunc(x.x),
        math::trunc(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> trunc(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::trunc(x.x),
        math::trunc(x.y),
        math::trunc(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> trunc(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::trunc(x.x),
        math::trunc(x.y),
        math::trunc(x.z),
        math::trunc(x.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// floor
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> floor(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::floor(x.x),
        math::floor(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> floor(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::floor(x.x),
        math::floor(x.y),
        math::floor(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> floor(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::floor(x.x),
        math::floor(x.y),
        math::floor(x.z),
        math::floor(x.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// ceil
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> ceil(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::ceil(x.x),
        math::ceil(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> ceil(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::ceil(x.x),
        math::ceil(x.y),
        math::ceil(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> ceil(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::ceil(x.x),
        math::ceil(x.y),
        math::ceil(x.z),
        math::ceil(x.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// sign
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> sign(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::sign(x.x),
        math::sign(x.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> sign(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::sign(x.x),
        math::sign(x.y),
        math::sign(x.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> sign(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::sign(x.x),
        math::sign(x.y),
        math::sign(x.z),
        math::sign(x.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// fmod
///////////////////////////////////////////////////////////////////////////////

// scalar binary

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> fmod(
    const vec<2, T>& x,
    T y
)
{
    return vec<2, T>(
        math::fmod(x.x, y),
        math::fmod(x.y, y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> fmod(
    const vec<3, T>& x,
    T y
)
{
    return vec<3, T>(
        math::fmod(x.x, y),
        math::fmod(x.y, y),
        math::fmod(x.z, y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> fmod(
    const vec<4, T>& x,
    T y
)
{
    return vec<4, T>(
        math::fmod(x.x, y),
        math::fmod(x.y, y),
        math::fmod(x.z, y),
        math::fmod(x.w, y)
    );
}

// binary

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> fmod(
    const vec<2, T>& x,
    const vec<2, T>& y
)
{
    return vec<2, T>(
        math::fmod(x.x, y.x),
        math::fmod(x.y, y.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> fmod(
    const vec<3, T>& x,
    const vec<3, T>& y
)
{
    return vec<3, T>(
        math::fmod(x.x, y.x),
        math::fmod(x.y, y.y),
        math::fmod(x.z, y.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> fmod(
    const vec<4, T>& x,
    const vec<4, T>& y
)
{
    return vec<4, T>(
        math::fmod(x.x, y.x),
        math::fmod(x.y, y.y),
        math::fmod(x.z, y.z),
        math::fmod(x.w, y.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// mod
///////////////////////////////////////////////////////////////////////////////

// scalar binary

template <typename T>
VX_FORCE_INLINE constexpr auto mod(
    const vec<2, T>& x,
    T y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec<2, R>(
        math::mod(x.x, y),
        math::mod(x.y, y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr auto mod(
    const vec<3, T>& x,
    T y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec<3, R>(
        math::mod(x.x, y),
        math::mod(x.y, y),
        math::mod(x.z, y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr auto mod(
    const vec<4, T>& x,
    T y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec<4, R>(
        math::mod(x.x, y),
        math::mod(x.y, y),
        math::mod(x.z, y),
        math::mod(x.w, y)
    );
}

// binary

template <typename T>
VX_FORCE_INLINE constexpr auto mod(
    const vec<2, T>& x,
    const vec<2, T>& y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec<2, R>(
        math::mod(x.x, y.x),
        math::mod(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr auto mod(
    const vec<3, T>& x,
    const vec<3, T>& y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec<3, R>(
        math::mod(x.x, y.x),
        math::mod(x.y, y.y),
        math::mod(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr auto mod(
    const vec<4, T>& x,
    const vec<4, T>& y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return vec<4, R>(
        math::mod(x.x, y.x),
        math::mod(x.y, y.y),
        math::mod(x.z, y.z),
        math::mod(x.w, y.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// fract
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> fract(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::fract(x.x),
        math::fract(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> fract(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::fract(x.x),
        math::fract(x.y),
        math::fract(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> fract(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::fract(x.x),
        math::fract(x.y),
        math::fract(x.z),
        math::fract(x.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// modf
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> modf(
    const vec<2, T>& x,
    vec<2, T>& intpart
)
{
    return vec<2, T>(
        math::modf(x.x, intpart.x),
        math::modf(x.y, intpart.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> modf(
    const vec<3, T>& x,
    vec<3, T>& intpart
)
{
    return vec<3, T>(
        math::modf(x.x, intpart.x),
        math::modf(x.y, intpart.y),
        math::modf(x.z, intpart.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> modf(
    const vec<4, T>& x,
    vec<4, T>& intpart
)
{
    return vec<4, T>(
        math::modf(x.x, intpart.x),
        math::modf(x.y, intpart.y),
        math::modf(x.z, intpart.z),
        math::modf(x.w, intpart.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// frexp
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename I, typename std::enable_if<std::is_floating_point<T>::value && std::is_arithmetic<I>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> frexp(
    const vec<2, T>& x,
    vec<2, I>& exp
)
{
    return vec<2, T>(
        math::frexp(x.x, exp.x),
        math::frexp(x.y, exp.y)
    );
}

template <typename T, typename I, typename std::enable_if<std::is_floating_point<T>::value && std::is_arithmetic<I>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> frexp(
    const vec<3, T>& x,
    vec<3, I>& exp
)
{
    return vec<3, T>(
        math::frexp(x.x, exp.x),
        math::frexp(x.y, exp.y),
        math::frexp(x.z, exp.z)
    );
}

template <typename T, typename I, typename std::enable_if<std::is_floating_point<T>::value && std::is_arithmetic<I>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> frexp(
    const vec<4, T>& x,
    vec<4, I>& exp
)
{
    return vec<4, T>(
        math::frexp(x.x, exp.x),
        math::frexp(x.y, exp.y),
        math::frexp(x.z, exp.z),
        math::frexp(x.w, exp.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// ldexp
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename I, typename std::enable_if<std::is_floating_point<T>::value && std::is_arithmetic<I>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> ldexp(
    const vec<2, T>& x,
    const vec<2, I>& exp
)
{
    return vec<2, T>(
        math::ldexp(x.x, exp.x),
        math::ldexp(x.y, exp.y)
    );
}

template <typename T, typename I, typename std::enable_if<std::is_floating_point<T>::value && std::is_arithmetic<I>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> ldexp(
    const vec<3, T>& x,
    const vec<3, I>& exp
)
{
    return vec<3, T>(
        math::ldexp(x.x, exp.x),
        math::ldexp(x.y, exp.y),
        math::ldexp(x.z, exp.z)
    );
}

template <typename T, typename I, typename std::enable_if<std::is_floating_point<T>::value && std::is_arithmetic<I>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> ldexp(
    const vec<4, T>& x,
    const vec<4, I>& exp
)
{
    return vec<4, T>(
        math::ldexp(x.x, exp.x),
        math::ldexp(x.y, exp.y),
        math::ldexp(x.z, exp.z),
        math::ldexp(x.w, exp.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// clamp
///////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> clamp(
    const vec<2, T>& x,
    T min,
    T max
)
{
    return vec<2, T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> clamp(
    const vec<3, T>& x,
    T min,
    T max
)
{
    return vec<3, T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max),
        math::clamp(x.z, min, max)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> clamp(
    const vec<4, T>& x,
    T min,
    T max
)
{
    return vec<4, T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max),
        math::clamp(x.z, min, max),
        math::clamp(x.w, min, max)
    );
}

// vector

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> clamp(
    const vec<2, T>& x,
    const vec<2, T>& min,
    const vec<2, T>& max
)
{
    return vec<2, T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> clamp(
    const vec<3, T>& x,
    const vec<3, T>& min,
    const vec<3, T>& max
)
{
    return vec<3, T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y),
        math::clamp(x.z, min.z, max.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> clamp(
    const vec<4, T>& x,
    const vec<4, T>& min,
    const vec<4, T>& max
)
{
    return vec<4, T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y),
        math::clamp(x.z, min.z, max.z),
        math::clamp(x.w, min.w, max.w)
    );
}

}
}
