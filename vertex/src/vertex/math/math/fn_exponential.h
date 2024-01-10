#pragma once

#include "detail/common.h"

namespace vx {
namespace math {

// =============== pow ===============

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline constexpr T pow(T x, T y)
{
    return std::pow(x, y);
}

// =============== exp ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T exp(T x)
{
    return std::exp(x);
}

// =============== exp2 ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T exp2(T x)
{
    return std::exp2(x);
}

// =============== log ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T log(T x)
{
    return std::log(x);
}

// =============== log2 ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T log2(T x)
{
    return std::log2(x);
}

// =============== sqrt ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T sqrt(T x)
{
    return std::sqrt(x);
}

// =============== inverse_sqrt ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T inverse_sqrt(T x)
{
    return static_cast<T>(1) / std::sqrt(x);
}

}
}
