#pragma once

#include "../detail/common.h"
#include "../constants.h"

namespace vx {
namespace math {

// =============== is_zero_approx ===============

/**
 * @brief Check if a value is approximately zero.
 *
 * This function checks if the given value is approximately zero
 * within a specified epsilon range.
 *
 * @param x The input value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the value is approximately zero, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_zero_approx(T x, const T epsilon = math::epsilon<T>)
{
    return ((x < static_cast<T>(0)) ? -x : x) < epsilon;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr bool is_zero_approx(T x, const T epsilon = math::epsilon<T>)
{
    return x == static_cast<T>(0);
}

// =============== is_equal_approx ===============

/**
 * @brief Check if two values are approximately equal.
 *
 * This function checks if two values are approximately equal
 * within a specified epsilon range.
 *
 * @param a The first value.
 * @param b The second value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the values are approximately equal, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return is_zero_approx(a - b, epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr bool is_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a == b;
}

// =============== is_not_equal_approx ===============

/**
 * @brief Check if two values are not approximately equal.
 *
 * This function checks if two values are not approximately equal
 * within a specified epsilon range.
 *
 * @param a The first value.
 * @param b The second value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the values are not approximately equal, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_not_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return !is_zero_approx(a - b, epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr bool is_not_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a != b;
}

// =============== is_greater_approx ===============

/**
 * @brief Check if a value is greater than another approximately.
 *
 * This function checks if the first value is greater than
 * the second approximately within a specified epsilon range.
 *
 * @param a The first value.
 * @param b The second value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the first value is greater than the second approximately, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_greater_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a > (b + epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr bool is_greater_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a > b;
}

// =============== is_greater_or_equal_approx ===============

/**
 * @brief Check if a value is greater than or equal to another approximately.
 *
 * This function checks if the first value is greater than or equal
 * to the second approximately within a specified epsilon range.
 *
 * @param a The first value.
 * @param b The second value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the first value is greater than or equal to the second approximately, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_greater_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a > (b - epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr bool is_greater_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a >= b;
}

// =============== is_less_approx ===============

/**
 * @brief Check if a value is less than another approximately.
 *
 * This function checks if the first value is less than the
 * second approximately within a specified epsilon range.
 *
 * @param a The first value.
 * @param b The second value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the first value is less than the second approximately, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_less_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a < (b - epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr bool is_less_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a < b;
}

// =============== is_less_or_equal_approx ===============

/**
 * @brief Check if a value is less than or equal to another approximately.
 *
 * This function checks if the first value is less than or equal
 * to the second approximately within a specified epsilon range.
 *
 * @param a The first value.
 * @param b The second value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the first value is less than or equal to the second approximately, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_less_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a < (b + epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr bool is_less_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a <= b;
}

// =============== is_finite ===============

/**
 * @brief Check if a value is finite.
 *
 * This function checks if the given value is finite.
 *
 * @param x The input value.
 * @return True if the value is finite, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_finite(T x)
{
    return std::isfinite(x);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr bool is_finite(T x)
{
    return true;
}

// =============== is_infinite ===============

/**
 * @brief Check if a value is infinite.
 *
 * This function checks if the given value is infinite.
 *
 * @param x The input value.
 * @return True if the value is infinite, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_infinite(T x)
{
    return std::isinf(x);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr bool is_infinite(T x)
{
    return false;
}

// =============== is_nan ===============

/**
 * @brief Check if a value is NaN (Not a Number).
 *
 * This function checks if the given value is NaN.
 *
 * @param x The input value.
 * @return True if the value is NaN, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_nan(T x)
{
    return std::isnan(x);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr bool is_nan(T x)
{
    return false;
}

// =============== round_epsilon ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T round_epsilon(T x, const T epsilon = math::epsilon<T>)
{
    const T xr = std::round(x);
    return is_equal_approx(x, xr, epsilon) ? xr : x;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr T round_epsilon(T x, const T epsilon = math::epsilon<T>)
{
    return x;
}

}
}