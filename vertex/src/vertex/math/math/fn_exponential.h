#pragma once

#include "detail/common.h"

namespace vx {
namespace math {

// =============== pow ===============

/**
 * @brief Power function.
 *
 * This template function calculates the value of 'x' raised to the power of 'y'.
 *
 * @tparam T The data type for which the power is calculated.
 * @param x The base value.
 * @param y The exponent value.
 * @return The result of raising 'x' to the power of 'y'.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T pow(T x, T y)
{
    return std::pow(x, y);
}

// =============== exp ===============

/**
 * @brief Exponential function.
 *
 * This template function calculates the exponential of the input value 'x'.
 *
 * @tparam T The data type for which the exponential is calculated.
 * @param x The input value.
 * @return The result of the exponential function e^x.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T exp(T x)
{
    return std::exp(x);
}

// =============== exp2 ===============

/**
 * @brief Base-2 exponential function.
 *
 * This template function calculates 2 raised to the power of the input value 'x'.
 *
 * @tparam T The data type for which the base-2 exponential is calculated.
 * @param x The input value.
 * @return The result of 2^x.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T exp2(T x)
{
    return std::exp2(x);
}

// =============== log ===============

/**
 * @brief Natural logarithm function.
 *
 * This template function calculates the natural logarithm of the input value 'x'.
 *
 * @tparam T The data type for which the natural logarithm is calculated.
 * @param x The input value.
 * @return The natural logarithm of 'x'.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T log(T x)
{
    return std::log(x);
}

// =============== log2 ===============

/**
 * @brief Base-2 logarithm function.
 *
 * This template function calculates the base-2 logarithm of the input value 'x'.
 *
 * @tparam T The data type for which the base-2 logarithm is calculated.
 * @param x The input value.
 * @return The base-2 logarithm of 'x'.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T log2(T x)
{
    return std::log2(x);
}

// =============== sqrt ===============

/**
 * @brief Square root function.
 *
 * This template function calculates the square root of the input value 'x'.
 *
 * @tparam T The data type for which the square root is calculated.
 * @param x The input value.
 * @return The square root of 'x'.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T sqrt(T x)
{
    return std::sqrt(x);
}

// =============== inverse_sqrt ===============

/**
 * @brief Inverse square root function.
 *
 * This template function calculates the inverse square root of the input value 'x'.
 *
 * @tparam T The data type for which the inverse square root is calculated.
 * @param x The input value.
 * @return The result of 1/sqrt(x).
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T inverse_sqrt(T x)
{
    return static_cast<T>(1) / std::sqrt(x);
}

}
}
