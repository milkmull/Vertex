#pragma once

#include <initializer_list>

#include "detail/common.h"

namespace vx {
namespace math {

// =============== min ===============

template <typename T>
inline constexpr T min(T x, T y)
{
    return std::min(x, y);
}

template <typename T>
inline constexpr T min(std::initializer_list<T> ls)
{
    return std::min(ls);
}

// =============== max ===============

template <typename T>
inline constexpr T max(T x, T y)
{
    return std::max(x, y);
}

template <typename T>
inline constexpr T max(std::initializer_list<T> ls)
{
    return std::max(ls);
}

// =============== abs ===============

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline constexpr T abs(T x)
{
    return std::abs(x);
}

// =============== round ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T round(T x)
{
    return std::round(x);
}

// =============== trunc ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T trunc(T x)
{
    return std::trunc(x);
}

// =============== floor ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T floor(T x)
{
    return std::floor(x);
}

// =============== ceil ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T ceil(T x)
{
    return std::ceil(x);
}

// =============== sign ===============

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline constexpr T sign(T x)
{
    if (x > static_cast<T>(0)) return static_cast<T>(+1);
    if (x < static_cast<T>(0)) return static_cast<T>(-1);
    return static_cast<T>(0);
}

// =============== fmod ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T fmod(T x, T y)
{
    return x - y * std::trunc(x / y);
}

// =============== mod ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T mod(T x, T y)
{
    return x - y * std::floor(x / y);
}

// =============== fract ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T fract(T x)
{
    return x - std::floor(x)
}

// =============== modf ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T modf(T x, T& intpart)
{
    return std::modf(x, &intpart);
}

// =============== frexp ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T frexp(T x, T& exp)
{
    return std::frexp(x, &exp);
}

// =============== snap ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T snap(T x, T interval)
{
    if (interval == static_cast<T>(0)) return x;
    return static_cast<T>(std::round(x / interval)) * interval;
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr T snap(T x, T interval)
{
    if (interval == static_cast<T>(0)) return x;
    return (x + interval / static_cast<T>(2)) / interval * interval;
}

// =============== clamp ===============

template <typename T>
inline constexpr T clamp(T x, T min, T max)
{
    return std::clamp(x, min, max);
}

}
}
