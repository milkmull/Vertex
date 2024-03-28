#pragma once

#include "../detail/common.h"

namespace vx {
namespace math {

#if defined(min)
#   undef min
#endif

#if defined(max)
#   undef max
#endif

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
VX_FORCE_INLINE constexpr T min(T x, T y)
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
VX_FORCE_INLINE constexpr T min(std::initializer_list<T> ls)
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
VX_FORCE_INLINE constexpr T max(T x, T y)
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
VX_FORCE_INLINE constexpr T max(std::initializer_list<T> ls)
{
    return std::max(ls);
}

// =============== abs ===============

/**
 * @brief Returns the absolute value of a given value.
 *
 * @tparam T The type of the value.
 * @param x The value.
 * @return The absolute value of x.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T abs(T x)
{
    return std::abs(x);
}

// =============== round ===============

/**
 * @brief Rounds the given value to the nearest integer.
 *
 * @tparam T The type of the value.
 * @param x The value to be rounded.
 * @return The rounded value.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T round(T x)
{
    return std::round(x);
}

// =============== trunc ===============

/**
 * @brief Truncates the given value towards zero.
 *
 * @tparam T The type of the value.
 * @param x The value to be truncated.
 * @return The truncated value.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T trunc(T x)
{
    return std::trunc(x);
}

// =============== floor ===============

/**
 * @brief Rounds down the given value to the nearest integer.
 *
 * @tparam T The type of the value.
 * @param x The value to be rounded up.
 * @return The largest integral value not greater than than x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T floor(T x)
{
    return std::floor(x);
}

// =============== ceil ===============

/**
 * @brief Rounds up the given value to the nearest integer.
 *
 * @tparam T The type of the value.
 * @param x The value to be rounded up.
 * @return The smallest integral value not less than x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T ceil(T x)
{
    return std::ceil(x);
}

// =============== sign ===============

/**
 * @brief Returns the sign of the given value.
 *
 * @tparam T The type of the value.
 * @param x The value.
 * @return -1 if x is negative, 0 if x is zero, and 1 if x is positive.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T sign(T x)
{
    return static_cast<T>((static_cast<T>(0) < x) - (static_cast<T>(0) > x));
}

// =============== fmod ===============

/**
 * @brief Returns the floating-point remainder of dividing two values.
 *
 * @tparam T The type of the values.
 * @param x The dividend.
 * @param y The divisor.
 * @return The floating-point remainder of dividing x by y.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T fmod(T x, T y)
{
    return x - y * std::trunc(x / y);
}

// =============== mod ===============

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/mod.xhtml

/**
 * @brief Returns the remainder of dividing two values.
 *
 * @tparam T The type of the values.
 * @param x The dividend.
 * @param y The divisor.
 * @return The remainder of dividing x by y.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T mod(T x, T y)
{
    return x - y * std::floor(x / y);
}

/**
 * @brief Returns the modulo operation result for two integral values.
 *
 * @tparam T The type of the values.
 * @param x The dividend.
 * @param y The divisor.
 * @return The result of the modulo operation (x mod y).
 */
template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T mod(T x, T y)
{
    return (x % y + y) % y;
}

// =============== fract ===============

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/fract.xhtml

/**
 * @brief Returns the fractional part of a floating-point value.
 *
 * @tparam T The type of the value.
 * @param x The value.
 * @return The fractional part of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T fract(T x)
{
    return x - std::floor(x);
}

// =============== modf ===============

/**
 * @brief Splits the given floating-point value into integral and fractional parts.
 *
 * This function splits the given floating-point value into its integral and fractional parts,
 * storing the integral part in intpart and returning the fractional part.
 *
 * @tparam T The type of the value.
 * @param x The value.
 * @param intpart Reference to store the integral part.
 * @return The fractional part of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr auto modf(T x, T& intpart)
{
    return std::modf(x, &intpart);
}

// =============== frexp ===============

/**
 * @brief Breaks down the given floating-point value into a normalized fraction and an integral power of 2.
 *
 * @tparam T The type of the value.
 * @tparam I The type of the exponent.
 * @param x The value.
 * @param exp Reference to store the exponent.
 * @return The normalized fraction of x.
 */
template <typename T, typename I, typename std::enable_if<std::is_floating_point<T>::value && std::is_arithmetic<I>::value, bool>::type = true>
VX_FORCE_INLINE constexpr auto frexp(T x, I& exp)
{
    int exp_int;
    const T f = std::frexp(x, &exp_int);
    exp = static_cast<I>(exp_int);
    return f;
}

// =============== ldexp ===============

/**
 * @brief Returns the result of multiplying the given floating-point value by 2 raised to the power of the given exponent.
 *
 * @tparam T The type of the value.
 * @tparam I The type of the exponent.
 * @param x The value.
 * @param exp The exponent.
 * @return The result of x * (2^exp).
 */
template <typename T, typename I, typename std::enable_if<std::is_floating_point<T>::value && std::is_arithmetic<I>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T ldexp(T x, I exp)
{
    return std::ldexp(x, static_cast<int>(exp));
}

// =============== clamp ===============

/**
 * @brief Clamps the given value within a specified range.
 *
 * @tparam T The type of the value.
 * @param x The value to be clamped.
 * @param min The minimum value of the range.
 * @param max The maximum value of the range.
 * @return The clamped value, which is x if it's within the range, min if x is less than min, or max if x is greater than max.
 */
template <typename T>
VX_FORCE_INLINE constexpr T clamp(T x, T min, T max)
{
    return std::clamp(x, min, max);
}

}
}
