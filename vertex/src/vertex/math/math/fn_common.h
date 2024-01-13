#pragma once

#include <initializer_list>

#include "detail/common.h"

namespace vx {
namespace math {

// =============== min ===============

/**
 * @brief Get the minimum of two values.
 *
 * This template function calculates the minimum of two values and returns the result.
 *
 * @tparam T The data type of the values.
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
 * This template function calculates the minimum value from the provided initializer list
 * and returns the result.
 *
 * @tparam T The data type of the values in the initializer list.
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
 * This template function calculates the maximum of two values and returns the result.
 *
 * @tparam T The data type of the values.
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
 * This template function calculates the maximum value from the provided initializer list
 * and returns the result.
 *
 * @tparam T The data type of the values in the initializer list.
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
 * This template function calculates the absolute value of the input and
 * returns the result.
 *
 * @tparam T The numeric data type for which the absolute value is calculated.
 * @param x The input value.
 * @return The absolute value of the input.
 */
template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline constexpr T abs(T x)
{
    return std::abs(x);
}

// =============== round ===============

/**
 * @brief Round a value to the nearest integer.
 *
 * This template function rounds the given value to the nearest integer
 * and returns the result.
 *
 * @tparam T The data type for which the rounding is performed.
 * @param x The input value.
 * @return The nearest integer after rounding the input.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T round(T x)
{
    return std::round(x);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type round(T x)
{
    return static_cast<detail::int_float_type>(x);
}

// =============== trunc ===============

/**
 * @brief Truncate a value.
 *
 * This template function truncates the given value and returns
 * the result.
 *
 * @tparam T The data type for which truncation is performed.
 * @param x The input value.
 * @return The truncated value towards zero.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T trunc(T x)
{
    return std::trunc(x);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type trunc(T x)
{
    return static_cast<detail::int_float_type>(x);
}

// =============== floor ===============

/**
 * @brief Round down a value to the nearest integer.
 *
 * This template function rounds down the given value to the nearest
 * integer and returns the result.
 *
 * @tparam T The data type for which the floor operation is performed.
 * @param x The input value.
 * @return The largest integer less than or equal to the input value.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T floor(T x)
{
    return std::floor(x);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type floor(T x)
{
    return static_cast<detail::int_float_type>(x);
}

// =============== ceil ===============

/**
 * @brief Round up a value to the nearest integer.
 *
 * This template function rounds up the given value to the nearest
 * integer and returns the result.
 *
 * @tparam T The data type for which the ceil operation is performed.
 * @param x The input value.
 * @return The smallest integer greater than or equal to the input value.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T ceil(T x)
{
    return std::ceil(x);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type ceil(T x)
{
    return static_cast<detail::int_float_type>(x);
}

// =============== sign ===============

/**
 * @brief Get the sign of a numeric value.
 *
 * This template function determines the sign of the given numeric value and
 * returns +1 if the value is positive, -1 if negative, and 0 if the value is zero.
 * The function is enabled only for arithmetic types.
 *
 * @tparam T The numeric data type for which the sign is determined.
 * @param x The input numeric value.
 * @return +1 if the value is positive, -1 if negative, and 0 if the value is zero.
 */
template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline constexpr T sign(T x)
{
    if (x > static_cast<T>(0)) return static_cast<T>(+1);
    if (x < static_cast<T>(0)) return static_cast<T>(-1);
    return static_cast<T>(0);
}

// =============== fmod ===============

/**
 * @brief Calculate the remainder of the division operation.
 *
 * This template function calculates the remainder of the division
 * operation (x/y) and returns the result.
 *
 * @tparam T The data type for which the fmod operation is performed.
 * @param x The numerator.
 * @param y The denominator.
 * @return The remainder of (x/y).
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T fmod(T x, T y)
{
    return x - y * std::trunc(x / y);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type fmod(T x, T y)
{
    return static_cast<detail::int_float_type>(x % y);
}

// =============== mod ===============

/**
 * @brief Calculate the modulus of the division operation.
 *
 * This template function calculates the modulus of the division
 * operation (x/y) and returns the result.
 *
 * @tparam T The data type for which the mod operation is performed.
 * @param x The numerator.
 * @param y The denominator.
 * @return The modulus of (x/y).
 * 
 * @ref https://registry.khronos.org/OpenGL-Refpages/gl4/html/mod.xhtml
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T mod(T x, T y)
{
    return x - y * std::floor(x / y);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type mod(T x, T y)
{
    return static_cast<detail::int_float_type>((x % y + y) % y);
}

// =============== fract ===============

/**
 * @brief Get the fractional part of a value.
 *
 * This template function extracts the fractional part of the given
 * value and returns the result.
 *
 * @tparam T The data type for which the fractional part is extracted.
 * @param x The input value.
 * @return The fractional part of the input value.
 *
 * @ref https://registry.khronos.org/OpenGL-Refpages/gl4/html/fract.xhtml
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T fract(T x)
{
    return x - std::floor(x);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type fract(T x)
{
    return static_cast<detail::int_float_type>(0);
}

// =============== modf ===============

/**
 * @brief Split a value into integer and fractional parts.
 *
 * This template function splits the given value into its integer
 * and fractional parts and returns the fractional part. The integer part is stored
 * in the reference parameter 'intpart'.
 *
 * @tparam T The data type for which the split is performed.
 * @param x The input value.
 * @param intpart Reference parameter to store the integer part.
 * @return The fractional part of the input value.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T modf(T x, T& intpart)
{
    return std::modf(x, &intpart);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type modf(T x, detail::int_float_type& intpart)
{
    intpart = static_cast<detail::int_float_type>(x);
    return static_cast<detail::int_float_type>(0) * intpart;
}

// =============== frexp ===============

/**
 * @brief Decompose a value into a normalized fraction and exponent.
 *
 * This template function decomposes the given value into a normalized
 * fraction and an exponent, returning the fraction. The exponent is stored in the
 * reference parameter 'exp'.
 *
 * @tparam T The data type for which the decomposition is performed.
 * @param x The input floating-point value.
 * @param exp Reference parameter to store the exponent.
 * @return The normalized fraction of the input value.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T frexp(T x, int& exp)
{
    return std::frexp(x, &exp);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr detail::int_float_type frexp(T x, int& exp)
{
    if (x == static_cast<detail::int_float_type>(0))
    {
        exp = 0;
        return static_cast<detail::int_float_type>(0);
    }

    exp = static_cast<int>(std::log2(std::abs(x))) + 1;
    return x / static_cast<detail::int_float_type>(std::pow(2, exp));
}

// =============== snap ===============

/**
 * @brief Snap a value to the nearest multiple of a specified interval.
 *
 * This template function snaps the given value to the nearest multiple of a specified
 * interval and returns the result. If the interval is zero, the original value is
 * returned unchanged.
 *
 * @tparam T The data type for which the snapping is performed.
 * @param x The input value to be snapped.
 * @param interval The specified interval for snapping.
 * @return The snapped value to the nearest multiple of the interval.
 */
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
    const T is_pos = static_cast<T>(x >= static_cast<T>(0));
    return ((x + is_pos * (interval - static_cast<T>(1))) / interval) * interval;
}

// =============== clamp ===============

/**
 * @brief Clamp a value within a specified range.
 *
 * This template function clamps the given value within the specified range [min, max]
 * and returns the result. If the value is less than min, it returns min; if the value
 * is greater than max, it returns max; otherwise, it returns the original value.
 *
 * @tparam T The data type of the value to be clamped.
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
