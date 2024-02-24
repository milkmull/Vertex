#pragma once

#include "../detail/common.h"

namespace vx {
namespace math {

// =============== lerp ===============

/**
 * @brief Performs linear interpolation between two values.
 *
 * This function performs linear interpolation between two values x and y based on a interpolation factor t.
 *
 * @tparam T Type of the values.
 * @param x The starting value.
 * @param y The ending value.
 * @param t The interpolation factor, typically in the range [0, 1].
 * @return The interpolated value between x and y.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T lerp(T x, T y, T t)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

// =============== step ===============

/**
 * @brief Computes a step function.
 *
 * This function returns 1 if x is greater than or equal to the edge value, otherwise returns 0.
 *
 * @tparam T Type of the values.
 * @param edge The edge value.
 * @param x The value to compare against the edge.
 * @return 1 if x is greater than or equal to the edge value, otherwise 0.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr T step(T edge, T x)
{
    return static_cast<T>(x >= edge);
}

// =============== smoothstep ===============

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/smoothstep.xhtml

/**
 * @brief Computes a smoothstep function.
 *
 * This function performs smooth interpolation between edge0 and edge1 using a Hermite interpolating polynomial.
 *
 * @tparam T Type of the values.
 * @param edge0 The lower edge.
 * @param edge1 The upper edge.
 * @param x The value to interpolate.
 * @return The smooth interpolated value between edge0 and edge1 based on x.
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