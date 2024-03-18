#pragma once

#include "../detail/common.h"

namespace vx {
namespace math {

// =============== pow ===============

/**
 * @brief Returns the value of x raised to the power of y.
 *
 * @tparam T The type of the values.
 * @param x The base value.
 * @param y The exponent value.
 * @return The result of x raised to the power of y.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T pow(T x, T y)
{
    return std::pow(x, y);
}

// =============== exp ===============

/**
 * @brief Returns the exponential value of the given value.
 *
 * @tparam T The type of the value.
 * @param x The value.
 * @return The exponential value of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T exp(T x)
{
    return std::exp(x);
}

// =============== exp2 ===============

/**
 * @brief Returns 2 raised to the power of the given value.
 *
 * @tparam T The type of the value.
 * @param x The exponent value.
 * @return 2 raised to the power of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T exp2(T x)
{
    return std::exp2(x);
}

// =============== log ===============

/**
 * @brief Returns the natural logarithm of the given value.
 *
 * @tparam T The type of the value.
 * @param x The value.
 * @return The natural logarithm of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T log(T x)
{
    return std::log(x);
}

// =============== log2 ===============

/**
 * @brief Returns the base-2 logarithm of the given value.
 *
 * @tparam T The type of the value.
 * @param x The value.
 * @return The base-2 logarithm of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T log2(T x)
{
    return std::log2(x);
}

// =============== sqrt ===============

/**
 * @brief Returns the square root of the given value.
 *
 * @tparam T The type of the value.
 * @param x The value.
 * @return The square root of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T sqrt(T x)
{
    return std::sqrt(x);
}

// =============== inverse_sqrt ===============

/**
 * @brief Returns the inverse square root of the given value.
 *
 * @tparam T The type of the value.
 * @param x The value.
 * @return The inverse square root of x.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T inverse_sqrt(T x)
{
    return static_cast<T>(1) / std::sqrt(x);
}

}
}
