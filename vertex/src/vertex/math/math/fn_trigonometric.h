#pragma once

#include "detail/common.h"

namespace vx {
namespace math {

// =============== radians ===============

/**
 * @brief Convert degrees to radians.
 *
 * This function converts degrees to radians.
 *
 * @param deg The angle in degrees.
 * @return The equivalent angle in radians.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T radians(T deg)
{
    constexpr T r = math::two_pi<T> / static_cast<T>(360);
    return deg * r;
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type radians(T deg)
{
    return radians(static_cast<detail::int_float_type>(deg));
}

// =============== degrees ===============

/**
 * @brief Convert radians to degrees.
 *
 * This function converts radians to degrees.
 *
 * @param rad The angle in radians.
 * @return The equivalent angle in degrees.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T degrees(T rad)
{
    constexpr T r = static_cast<T>(360) / math::two_pi<T>;
    return rad * r;
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type degrees(T rad)
{
    return degrees(static_cast<detail::int_float_type>(rad));
}

// =============== sin ===============

/**
 * @brief Compute the sine of an angle.
 *
 * This function computes the sine of the given angle.
 *
 * @param x The angle in radians.
 * @return The sine of the angle.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T sin(T x)
{
    return std::sin(x);
}

// =============== cos ===============

/**
 * @brief Compute the cosine of an angle.
 *
 * This function computes the cosine of the given angle.
 *
 * @param x The angle in radians.
 * @return The cosine of the angle.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T cos(T x)
{
    return std::cos(x);
}

// =============== tan ===============

/**
 * @brief Compute the tangent of an angle.
 *
 * This function computes the tangent of the given angle.
 *
 * @param x The angle in radians.
 * @return The tangent of the angle.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T tan(T x)
{
    return std::tan(x);
}

// =============== asin ===============

/**
 * @brief Compute the arcsine of a value.
 *
 * This function computes the arcsine of the given value.
 *
 * @note The input value should be in the range [-1, 1].
 * @note The output is in the range [-pi/2, pi/2].
 *
 * @param x The input value, should be in the range [-1, 1].
 * @return The arcsine of the input value, in radians.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T asin(T x)
{
    return std::asin(x);
}

/**
 * @brief Calculates the arcsine of the given value within the clamped range [-1, 1].
 *
 * This function computes the arcsine of the input value 'x' and ensures that the input is clamped
 * to the valid range [-1, 1]. The result is in the range [-pi/2, pi/2].
 *
 * @param x The input value to compute the arcsine for, clamped to the range [-1, 1].
 * @return The arcsine of the clamped input value in the range [-pi/2, pi/2].
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T asin_clamped(T x)
{
    return math::asin(std::clamp(x, static_cast<T>(-1), static_cast<T>(1)));
}

// =============== acos ===============

/**
 * @brief Compute the arccosine of a value.
 *
 * This function computes the arccosine of the given value.
 *
 * @param x The input value, within the range [-1, 1].
 * @return The arccosine of the input value, in radians.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T acos(T x)
{
    return std::acos(x);
}

/**
 * @brief Calculates the arccosine of the given value within the clamped range [-1, 1].
 *
 * This function computes the arccosine of the input value 'x' and ensures that the input is clamped
 * to the valid range [-1, 1]. The result is in the range [0, pi].
 *
 * @param x The input value to compute the arccosine for, clamped to the range [-1, 1].
 * @return The arccosine of the clamped input value in the range [0, pi].
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T acos_clamped(T x)
{
    return math::acos(std::clamp(x, static_cast<T>(-1), static_cast<T>(1)));
}

// =============== atan ===============

/**
 * @brief Calculates the arctangent of the given value.
 *
 * This function computes the arctangent of the input value 'x', returning the angle
 * in radians whose tangent is 'x'. The result is in the range [-pi/2, pi/2].
 *
 * @param x The input value to compute the arctangent for.
 * @return The arctangent of the input value in the range [-pi/2, pi/2].
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T atan(T x)
{
    return std::atan(x);
}

// =============== atan2 ===============

/**
 * @brief Calculates the arctangent of the quotient of its arguments.
 *
 * This function computes the four-quadrant arctangent of the ratio 'x/y', returning
 * the angle in radians whose tangent is the quotient 'x/y'. The result is in the range [-pi, pi].
 *
 * @param x Numerator of the ratio for which to compute the arctangent.
 * @param y Denominator of the ratio for which to compute the arctangent.
 * @return The arctangent of 'x/y' in the range [-pi, pi].
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T atan2(T x, T y)
{
    return std::atan2(x, y);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type atan2(T x, T y)
{
    return std::atan2(
        static_cast<detail::int_float_type>(x),
        static_cast<detail::int_float_type>(y)
    );
}

// =============== sinh ===============

/**
 * @brief Calculates the hyperbolic sine of the specified value.
 *
 * This function computes the hyperbolic sine of the input 'x'.
 *
 * @param x The value for which to calculate the hyperbolic sine.
 * @return The hyperbolic sine of 'x'.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T sinh(T x)
{
    return std::sinh(x);
}

// =============== cosh ===============

/**
 * @brief Calculates the hyperbolic cosine of the specified value.
 *
 * This function computes the hyperbolic cosine of the input 'x'.
 *
 * @param x The value for which to calculate the hyperbolic cosine.
 * @return The hyperbolic cosine of 'x'.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T cosh(T x)
{
    return std::cosh(x);
}

// =============== tanh ===============

/**
 * @brief Calculates the hyperbolic tangent of the specified value.
 *
 * This function computes the hyperbolic tangent of the input 'x'.
 *
 * @param x The value for which to calculate the hyperbolic tangent.
 * @return The hyperbolic tangent of 'x'.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T tanh(T x)
{
    return std::tanh(x);
}

// =============== asinh ===============

/**
 * @brief Calculates the inverse hyperbolic sine of the specified value.
 *
 * This function computes the inverse hyperbolic sine of the input 'x'.
 *
 * @param x The value for which to calculate the inverse hyperbolic sine.
 * @return The inverse hyperbolic sine of 'x'.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T asinh(T x)
{
    return std::asinh(x);
}

// =============== acosh ===============

/**
 * @brief Calculates the inverse hyperbolic cosine of the specified value.
 *
 * This function computes the inverse hyperbolic cosine of the input 'x'.
 * The input 'x' must be greater than or equal to 1.
 *
 * @param x The value for which to calculate the inverse hyperbolic cosine.
 * @return The inverse hyperbolic cosine of 'x'.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T acosh(T x)
{
    return std::acosh(x);
}

// =============== atanh ===============

/**
 * @brief Calculates the inverse hyperbolic tangent of the specified value.
 *
 * This function computes the inverse hyperbolic tangent of the input 'x'.
 * The absolute value of 'x' must be less than 1.
 *
 * @param x The value for which to calculate the inverse hyperbolic tangent.
 * @return The inverse hyperbolic tangent of 'x'.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T atanh(T x)
{
    return std::atanh(x);
}

}
}