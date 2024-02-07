#pragma once

#include "../detail/common.h"

namespace vx {
namespace math {

// =============== lerp ===============

/**
 * @brief Linear interpolation function.
 *
 * This function performs linear interpolation between two values 'x' and 'y' based on a parameter 't'.
 *
 * @param x The starting value.
 * @param y The ending value.
 * @param t The interpolation parameter (should be in the range [0, 1]).
 * @return The result of linear interpolation between 'x' and 'y'.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T lerp(T x, T y, T t)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

// =============== step ===============

/**
 * @brief Step function.
 *
 * This function returns 0 if the input value 'x' is less than the edge, and 1 otherwise.
 *
 * @param edge The threshold value.
 * @param x The input value.
 * @return 0 if 'x' is less than 'edge', and 1 otherwise.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr T step(T edge, T x)
{
    return static_cast<T>(x >= edge);
}

// =============== smoothstep ===============

/**
 * @brief Smoothstep function.
 *
 * This function performs smooth interpolation between two edges based on the input value 'x'.
 *
 * @param edge0 The lower edge.
 * @param edge1 The upper edge.
 * @param x The input value.
 * @return The result of smoothstep interpolation between 'edge0' and 'edge1'.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T smoothstep(T edge0, T edge1, T x)
{
    const T v = (x - edge0) / (edge1 - edge0);
    const T t = std::clamp(v, static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

}
}