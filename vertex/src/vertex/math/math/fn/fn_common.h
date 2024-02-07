#pragma once

#include <initializer_list>

#include "../detail/common.h"

namespace vx {
namespace math {

// =============== min ===============

/**
 * @brief Get the minimum of two values.
 *
 * This function calculates the minimum of two values and returns the result.
 *
 * @param x The first value.
 * @param y The second value.
 * @return The minimum of the two values.
 */
template <typename T>
inline constexpr T min(T x, T y)
{
    return std::min(x, y);
}

/**
 * @brief Get the minimum value from an initializer list.
 *
 * This function calculates the minimum value from the provided initializer list
 * and returns the result.
 *
 * @param ls The initializer list containing values to find the minimum from.
 * @return The minimum value from the initializer list.
 */
template <typename T>
inline constexpr T min(std::initializer_list<T> ls)
{
    return std::min(ls);
}

// =============== max ===============

/**
 * @brief Get the maximum of two values.
 *
 * This function calculates the maximum of two values and returns the result.
 *
 * @param x The first value.
 * @param y The second value.
 * @return The maximum of the two values.
 */
template <typename T>
inline constexpr T max(T x, T y)
{
    return std::max(x, y);
}

/**
 * @brief Get the maximum value from an initializer list.
 *
 * This function calculates the maximum value from the provided initializer list
 * and returns the result.
 *
 * @param ls The initializer list containing values to find the maximum from.
 * @return The maximum value from the initializer list.
 */
template <typename T>
inline constexpr T max(std::initializer_list<T> ls)
{
    return std::max(ls);
}

// =============== abs ===============

/**
 * @brief Calculate the absolute value of the input.
 *
 * This function calculates the absolute value of the input and
 * returns the result.
 *
 * @param x The input value.
 * @return The absolute value of the input.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr T abs(T x)
{
    return std::abs(x);
}

// =============== round ===============

/**
 * @brief Round a value to the nearest integer.
 *
 * This function rounds the given value to the nearest integer
 * and returns the result.
 *
 * @param x The input value.
 * @return The nearest integer after rounding the input.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T round(T x)
{
    return std::round(x);
}

// =============== trunc ===============

/**
 * @brief Truncate a floating point value.
 *
 * This function truncates the given value and returns
 * the result.
 *
 * @param x The input value.
 * @return The truncated value towards zero.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T trunc(T x)
{
    return std::trunc(x);
}

// =============== floor ===============

/**
 * @brief Round down a value to the nearest integer.
 *
 * This function rounds down the given value to the nearest
 * integer and returns the result.
 *
 * @param x The input value.
 * @return The largest integer less than or equal to the input value.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T floor(T x)
{
    return std::floor(x);
}

// =============== ceil ===============

/**
 * @brief Round up a value to the nearest integer.
 *
 * This function rounds up the given value to the nearest
 * integer and returns the result.
 *
 * @param x The input value.
 * @return The smallest integer greater than or equal to the input value.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T ceil(T x)
{
    return std::ceil(x);
}

// =============== sign ===============

/**
 * @brief Get the sign of a numeric value.
 *
 * This function determines the sign of the given numeric value and
 * returns +1 if the value is positive, -1 if negative, and 0 if the value is zero.
 * The function is enabled only for arithmetic types.
 *
 * @param x The input numeric value.
 * @return +1 if the value is positive, -1 if negative, and 0 if the value is zero.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr T sign(T x)
{
    return static_cast<T>((static_cast<T>(0) < x) - (static_cast<T>(0) > x));
}

// =============== fmod ===============

/**
 * @brief Calculate the remainder of the division operation.
 *
 * This function calculates the remainder of the division
 * operation (x/y) and returns the result.
 *
 * @param x The numerator.
 * @param y The denominator.
 * @return The remainder of (x/y).
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T fmod(T x, T y)
{
    return x - y * std::trunc(x / y);
}

// =============== mod ===============

/**
 * @brief Calculate the modulus of the division operation.
 *
 * This function calculates the modulus of the division
 * operation (x/y) and returns the result.
 *
 * @param x The numerator.
 * @param y The denominator.
 * @return The modulus of (x/y).
 * 
 * @ref https://registry.khronos.org/OpenGL-Refpages/gl4/html/mod.xhtml
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T mod(T x, T y)
{
    return x - y * std::floor(x / y);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr T mod(T x, T y)
{
    return (x % y + y) % y;
}

// =============== fract ===============

/**
 * @brief Get the fractional part of a value.
 *
 * This function extracts the fractional part of the given
 * value and returns the result.
 *
 * @param x The input value.
 * @return The fractional part of the input value.
 *
 * @ref https://registry.khronos.org/OpenGL-Refpages/gl4/html/fract.xhtml
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T fract(T x)
{
    return x - std::floor(x);
}

// =============== modf ===============

/**
 * @brief Split a value into integer and fractional parts.
 *
 * This function splits the given value into its integer
 * and fractional parts and returns the fractional part. The integer part is stored
 * in the reference parameter 'intpart'.
 *
 * @param x The input value.
 * @param intpart Reference parameter to store the integer part.
 * @return The fractional part of the input value.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr auto modf(T x, T& intpart)
{
    return std::modf(x, &intpart);
}

// =============== frexp ===============

/**
 * @brief Decompose a value into a normalized fraction and exponent.
 *
 * This function decomposes the given value into a normalized
 * fraction and an exponent, returning the fraction. The exponent is stored in the
 * reference parameter 'exp'.
 *
 * @param x The input floating-point value.
 * @param exp Reference parameter to store the exponent.
 * @return The normalized fraction of the input value.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr auto frexp(T x, int& exp)
{
    return std::frexp(x, &exp);
}

// =============== snap ===============

/**
 * @brief Snap a value to the nearest multiple of a specified interval.
 *
 * This function snaps the given value to the nearest multiple of a specified
 * interval and returns the result. If the interval is zero, the original value is
 * returned unchanged.
 *
 * @param x The input value to be snapped.
 * @param interval The specified interval for snapping.
 * @return The snapped value to the nearest multiple of the interval.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T snap(T x, T interval)
{
    if (interval == static_cast<T>(0)) return x;
    return static_cast<T>(std::round(x / interval)) * interval;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr T snap(T x, T interval)
{
    if (interval == static_cast<T>(0)) return x;
    const T is_pos = static_cast<T>(x >= static_cast<T>(0));
    return ((x + is_pos * (interval - static_cast<T>(1))) / interval) * interval;
}

// =============== clamp ===============

/**
 * @brief Clamp a value within a specified range.
 *
 * This function clamps the given value within the specified range [min, max]
 * and returns the result. If the value is less than min, it returns min; if the value
 * is greater than max, it returns max; otherwise, it returns the original value.
 *
 * @param x The input value.
 * @param min The minimum value in the range.
 * @param max The maximum value in the range.
 * @return The clamped value within the specified range.
 */
template <typename T>
inline constexpr T clamp(T x, T min, T max)
{
    return std::clamp(x, min, max);
}

}
}
