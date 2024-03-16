#pragma once

#include "../detail/common.h"
#include "../constants.h"

namespace vx {
namespace math {

// =============== radians ===============

/**
 * @brief Converts degrees to radians.
 *
 * This function converts an angle from degrees to radians.
 *
 * @tparam T Type of the angle values.
 * @param deg The angle in degrees.
 * @return The angle in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T radians(T deg)
{
    constexpr T r = math::two_pi<T>() / static_cast<T>(360);
    return deg * r;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr auto radians(T deg)
{
    return radians(static_cast<double>(deg));
}

// =============== degrees ===============

/**
 * @brief Converts radians to degrees.
 *
 * This function converts an angle from radians to degrees.
 *
 * @tparam T Type of the angle values.
 * @param rad The angle in radians.
 * @return The angle in degrees.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T degrees(T rad)
{
    constexpr T r = static_cast<T>(360) / math::two_pi<T>();
    return rad * r;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr auto degrees(T rad)
{
    return degrees(static_cast<double>(rad));
}

// =============== sin ===============

/**
 * @brief Computes the sine of an angle.
 *
 * This function computes the sine of the input angle.
 *
 * @tparam T Type of the angle.
 * @param x The angle in radians.
 * @return The sine of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T sin(T x)
{
    return std::sin(x);
}

// =============== cos ===============

/**
 * @brief Computes the cosine of an angle.
 *
 * This function computes the cosine of the input angle.
 *
 * @tparam T Type of the angle.
 * @param x The angle in radians.
 * @return The cosine of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T cos(T x)
{
    return std::cos(x);
}

// =============== tan ===============

/**
 * @brief Computes the tangent of an angle.
 *
 * This function computes the tangent of the input angle.
 *
 * @tparam T Type of the angle.
 * @param x The angle in radians.
 * @return The tangent of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T tan(T x)
{
    return std::tan(x);
}

// =============== asin ===============

/**
 * @brief Computes the arcsine of a value.
 *
 * This function computes the arcsine (inverse sine) of the input value.
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The arcsine of x in radians, in the range [-pi/2, pi/2].
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T asin(T x)
{
    return std::asin(x);
}

/**
 * @brief Computes the clamped arcsine of a value.
 *
 * This function computes the arcsine (inverse sine) of the input value after clamping it to the range [-1, 1].
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The arcsine of x in radians, clamped to the range [-pi/2, pi/2].
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T asin_clamped(T x)
{
    return std::asin(std::clamp(x, static_cast<T>(-1), static_cast<T>(1)));
}

// =============== acos ===============

/**
 * @brief Computes the arccosine of a value.
 *
 * This function computes the arccosine (inverse cosine) of the input value.
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The arccosine of x in radians, in the range [0, pi].
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T acos(T x)
{
    return std::acos(x);
}

/**
 * @brief Computes the clamped arccosine of a value.
 *
 * This function computes the arccosine (inverse cosine) of the input value after clamping it to the range [-1, 1].
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The arccosine of x in radians, clamped to the range [0, pi].
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T acos_clamped(T x)
{
    return std::acos(std::clamp(x, static_cast<T>(-1), static_cast<T>(1)));
}

// =============== atan ===============

/**
 * @brief Computes the arctangent of a value.
 *
 * This function computes the arctangent (inverse tangent) of the input value.
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The arctangent of x in radians, in the range [-pi/2, pi/2].
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T atan(T x)
{
    return std::atan(x);
}

// =============== atan2 ===============

/**
 * @brief Computes the arctangent of the quotient of its arguments.
 *
 * This function computes the arctangent (inverse tangent) of the quotient y/x,
 * where y is the first argument and x is the second argument.
 *
 * @tparam T Type of the arguments.
 * @param y The numerator.
 * @param x The denominator.
 * @return The arctangent of y/x in radians, in the range [-pi, pi].
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T atan2(T y, T x)
{
    return std::atan2(y, x);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr auto atan2(T y, T x)
{
    return std::atan2(static_cast<double>(y), static_cast<double>(x));
}

// =============== sinh ===============

/**
 * @brief Computes the hyperbolic sine of a value.
 *
 * This function computes the hyperbolic sine of the input value.
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The hyperbolic sine of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T sinh(T x)
{
    return std::sinh(x);
}

// =============== cosh ===============

/**
 * @brief Computes the hyperbolic cosine of a value.
 *
 * This function computes the hyperbolic cosine of the input value.
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The hyperbolic cosine of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T cosh(T x)
{
    return std::cosh(x);
}

// =============== tanh ===============

/**
 * @brief Computes the hyperbolic tangent of a value.
 *
 * This function computes the hyperbolic tangent of the input value.
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The hyperbolic tangent of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T tanh(T x)
{
    return std::tanh(x);
}

// =============== asinh ===============

/**
 * @brief Computes the inverse hyperbolic sine of a value.
 *
 * This function computes the inverse hyperbolic sine of the input value.
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The inverse hyperbolic sine of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T asinh(T x)
{
    return std::asinh(x);
}

// =============== acosh ===============

/**
 * @brief Computes the inverse hyperbolic cosine of a value.
 *
 * This function computes the inverse hyperbolic cosine of the input value.
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The inverse hyperbolic cosine of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T acosh(T x)
{
    return std::acosh(x);
}

// =============== atanh ===============

/**
 * @brief Computes the inverse hyperbolic tangent of a value.
 *
 * This function computes the inverse hyperbolic tangent of the input value.
 *
 * @tparam T Type of the value.
 * @param x The value.
 * @return The inverse hyperbolic tangent of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T atanh(T x)
{
    return std::atanh(x);
}

}
}