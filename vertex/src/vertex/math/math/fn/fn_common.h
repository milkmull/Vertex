#pragma once

#include <initializer_list>

#include "../detail/common.h"

namespace vx {
namespace math {

// =============== min ===============

/**
 * @brief Returns the minimum of two values.
 *
 * @tparam T Type of the values.
 * @param x First value.
 * @param y Second value.
 * @return The smaller of the two input values.
 */
template <typename T>
inline constexpr T min(T x, T y)
{
    return std::min(x, y);
}

/**
 * @brief Returns the minimum value from a list of values.
 *
 * @tparam T Type of the values in the initializer list.
 * @param ls The initializer list containing values to compare.
 * @return The smallest value in the initializer list.
 */
template <typename T>
inline constexpr T min(std::initializer_list<T> ls)
{
    return std::min(ls);
}

// =============== max ===============

/**
 * @brief Returns the maximum of two values.
 *
 * @tparam T Type of the values.
 * @param x First value.
 * @param y Second value.
 * @return The greater of the two input values.
 */
template <typename T>
inline constexpr T max(T x, T y)
{
    return std::max(x, y);
}

/**
 * @brief Returns the maximum value from a list of values.
 *
 * @tparam T Type of the values in the initializer list.
 * @param ls The initializer list containing values to compare.
 * @return The largest value in the initializer list.
 */
template <typename T>
inline constexpr T max(std::initializer_list<T> ls)
{
    return std::max(ls);
}

// =============== abs ===============

/**
 * @brief Returns the absolute value of a numeric type.
 *
 * @tparam T Type of the numeric value.
 * @param x The numeric value.
 * @return The absolute value of the input numeric value.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr T abs(T x)
{
    return std::abs(x);
}

// =============== round ===============

/**
 * @brief Rounds a number to the nearest integer.
 *
 * @tparam T Type of the number.
 * @param x The number to round.
 * @return The nearest integer value to the input number.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T round(T x)
{
    return std::round(x);
}

// =============== trunc ===============

/**
 * @brief Truncates a number towards zero.
 *
 * @tparam T Type of the number.
 * @param x The number to truncate.
 * @return The truncated value of the input number.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T trunc(T x)
{
    return std::trunc(x);
}

// =============== floor ===============

/**
 * @brief Rounds a number down to the nearest integer.
 *
 * @tparam T Type of the number.
 * @param x The number to round down.
 * @return The largest integral value not greater than the input number.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T floor(T x)
{
    return std::floor(x);
}

// =============== ceil ===============

/**
 * @brief Rounds a number up to the nearest integer.
 *
 * @tparam T Type of the number.
 * @param x The number to round up.
 * @return The smallest integral value not less than the input number.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T ceil(T x)
{
    return std::ceil(x);
}

// =============== sign ===============

/**
 * @brief Returns the sign of a numeric value.
 *
 * @tparam T Type of the numeric value.
 * @param x The numeric value.
 * @return -1 if the input is negative, 1 if positive, and 0 if zero.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr T sign(T x)
{
    return static_cast<T>((static_cast<T>(0) < x) - (static_cast<T>(0) > x));
}

// =============== fmod ===============

/**
 * @brief Computes the remainder of division of two numbers.
 *
 * @tparam T Type of the numbers.
 * @param x The dividend.
 * @param y The divisor.
 * @return The remainder of dividing x by y.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T fmod(T x, T y)
{
    return x - y * std::trunc(x / y);
}

// =============== mod ===============

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/mod.xhtml

/**
 * @brief Computes the remainder of division of two numbers.
 *
 * @tparam T Type of the numbers.
 * @param x The dividend.
 * @param y The divisor.
 * @return The remainder of dividing x by y.
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

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/fract.xhtml

/**
 * @brief Computes the fractional part of a number.
 *
 * @tparam T Type of the number.
 * @param x The number.
 * @return The fractional part of the input number.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T fract(T x)
{
    return x - std::floor(x);
}

// =============== modf ===============

/**
 * @brief Decomposes a number into integral and fractional parts.
 *
 * This function decomposes the input number into its integral and fractional parts.
 * The integral part is stored in the provided reference variable, and the fractional part is returned.
 *
 * @tparam T Type of the number.
 * @param x The number to decompose.
 * @param intpart Reference variable to store the integral part of x.
 * @return The fractional part of the input number.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr auto modf(T x, T& intpart)
{
    return std::modf(x, &intpart);
}

// =============== frexp ===============

/**
 * @brief Decomposes a number into its normalized fraction and exponent.
 * 
 * This function decomposes the input number into its normalized fraction and exponent.
 * The normalized fraction is returned, and the exponent is stored in the provided reference variable.
 * 
 * @tparam T Type of the number.
 * @param x The number to decompose.
 * @param exp Reference variable to store the exponent of x.
 * @return The normalized fraction of the input number.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr auto frexp(T x, int& exp)
{
    return std::frexp(x, &exp);
}

// =============== clamp ===============

/**
 * @brief Clamps a value within a specified range.
 *
 * @tparam T Type of the value.
 * @param x The value to clamp.
 * @param min The minimum value in the range.
 * @param max The maximum value in the range.
 * @return The clamped value.
 */
template <typename T>
inline constexpr T clamp(T x, T min, T max)
{
    return std::clamp(x, min, max);
}

}
}
