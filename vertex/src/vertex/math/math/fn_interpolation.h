#pragma once

#include "detail/common.h"

namespace vx {
namespace math {

// =============== lerp ===============

/**
 * @brief Linear interpolation function.
 *
 * This template function performs linear interpolation between two values `x` and `y` based on a parameter `t`.
 *
 * @tparam T The data type for which the linear interpolation is performed (must be floating-point).
 * @param x The starting value.
 * @param y The ending value.
 * @param t The interpolation parameter (should be in the range [0, 1]).
 * @return The result of linear interpolation between `x` and `y`.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T lerp(T x, T y, T t)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

// =============== step ===============

/**
 * @brief Step function.
 *
 * This template function returns 0 if the input value `x` is less than the edge, and 1 otherwise.
 *
 * @tparam T The data type for which the step function is applied.
 * @param edge The threshold value.
 * @param x The input value.
 * @return 0 if `x` is less than `edge`, and 1 otherwise.
 */
template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline constexpr T step(T edge, T x)
{
    return (x < edge) ? static_cast<T>(0) : static_cast<T>(1);
}

// =============== smoothstep ===============

/**
 * @brief Smoothstep function.
 *
 * This template function performs smooth interpolation between two edges based on the input value `x`.
 *
 * @tparam T The data type for which the smoothstep function is applied (must be floating-point).
 * @param edge0 The lower edge.
 * @param edge1 The upper edge.
 * @param x The input value.
 * @return The result of smoothstep interpolation between `edge0` and `edge1`.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T smoothstep(T edge0, T edge1, T x)
{
    const T v = (x - edge0) / (edge1 - edge0);
    const T t = std::clamp(v, static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

}
}