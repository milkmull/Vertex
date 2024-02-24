#pragma once

#include "../detail/common.h"

namespace vx {
namespace math {

// =============== pow ===============

/**
 * @brief Computes the value of x raised to the power of y.
 *
 * This function computes the value of x raised to the power of y.
 *
 * @tparam T Type of the numbers.
 * @param x The base.
 * @param y The exponent.
 * @return The value of x raised to the power of y.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T pow(T x, T y)
{
    return std::pow(x, y);
}

// =============== exp ===============

/**
 * @brief Computes the exponential (e^x) of a number.
 *
 * This function computes the exponential (e^x) of the input number.
 *
 * @tparam T Type of the number.
 * @param x The exponent.
 * @return The value of e raised to the power of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T exp(T x)
{
    return std::exp(x);
}

// =============== exp2 ===============

/**
 * @brief Computes 2 raised to the power of a number.
 *
 * This function computes 2 raised to the power of the input number.
 *
 * @tparam T Type of the number.
 * @param x The exponent.
 * @return The value of 2 raised to the power of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T exp2(T x)
{
    return std::exp2(x);
}

// =============== log ===============

/**
 * @brief Computes the natural logarithm of a number.
 *
 * This function computes the natural logarithm (base e) of the input number.
 *
 * @tparam T Type of the number.
 * @param x The number.
 * @return The natural logarithm of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T log(T x)
{
    return std::log(x);
}

// =============== log2 ===============

/**
 * @brief Computes the base-2 logarithm of a number.
 *
 * This function computes the base-2 logarithm of the input number.
 *
 * @tparam T Type of the number.
 * @param x The number.
 * @return The base-2 logarithm of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T log2(T x)
{
    return std::log2(x);
}

// =============== sqrt ===============

/**
 * @brief Computes the square root of a number.
 *
 * This function computes the square root of the input number.
 *
 * @tparam T Type of the number.
 * @param x The number.
 * @return The square root of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T sqrt(T x)
{
    return std::sqrt(x);
}

// =============== inverse_sqrt ===============

/**
 * @brief Computes the inverse square root of a number.
 *
 * This function computes the inverse square root of the input number.
 *
 * @tparam T Type of the number.
 * @param x The number.
 * @return The inverse square root of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T inverse_sqrt(T x)
{
    return static_cast<T>(1) / std::sqrt(x);
}

}
}
