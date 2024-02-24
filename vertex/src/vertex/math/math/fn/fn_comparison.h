#pragma once

#include "../detail/common.h"
#include "../constants.h"

namespace vx {
namespace math {

// =============== is_zero_approx ===============

/**
 * @brief Checks if a number is approximately zero.
 *
 * This function checks whether the absolute value of the input number is less than a specified epsilon.
 *
 * @tparam T Type of the number.
 * @param x The number to check.
 * @param epsilon The tolerance threshold for considering a number as zero.
 * @return True if the absolute value of x is less than epsilon, false otherwise.
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
 * @brief Checks if two numbers are approximately equal.
 *
 * This function checks whether the absolute difference between two numbers is less than a specified epsilon.
 *
 * @tparam T Type of the numbers.
 * @param a The first number to compare.
 * @param b The second number to compare.
 * @param epsilon The tolerance threshold for considering the numbers as equal.
 * @return True if the absolute difference between a and b is less than epsilon, false otherwise.
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
 * @brief Checks if two numbers are approximately not equal.
 *
 * This function checks whether the absolute difference between two numbers is greater than or equal to a specified epsilon.
 *
 * @tparam T Type of the numbers.
 * @param a The first number to compare.
 * @param b The second number to compare.
 * @param epsilon The tolerance threshold for considering the numbers as equal.
 * @return True if the absolute difference between a and b is greater than or equal to epsilon, false otherwise.
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
 * @brief Checks if a number is approximately greater than another.
 *
 * This function checks whether the first number is greater than the second number plus a specified epsilon.
 *
 * @tparam T Type of the numbers.
 * @param a The first number to compare.
 * @param b The second number to compare.
 * @param epsilon The tolerance threshold for considering the numbers as approximately equal.
 * @return True if a is greater than (b + epsilon), false otherwise.
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
 * @brief Checks if a number is approximately greater than or equal to another.
 *
 * This function checks whether the first number is greater than or equal to the second number minus a specified epsilon.
 *
 * @tparam T Type of the numbers.
 * @param a The first number to compare.
 * @param b The second number to compare.
 * @param epsilon The tolerance threshold for considering the numbers as approximately equal.
 * @return True if a is greater than or equal to (b - epsilon), false otherwise.
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
 * @brief Checks if a number is approximately less than another.
 *
 * This function checks whether the first number is less than the second number minus a specified epsilon.
 *
 * @tparam T Type of the numbers.
 * @param a The first number to compare.
 * @param b The second number to compare.
 * @param epsilon The tolerance threshold for considering the numbers as approximately equal.
 * @return True if a is less than (b - epsilon), false otherwise.
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
 * @brief Checks if a number is approximately less than or equal to another.
 *
 * This function checks whether the first number is less than or equal to the second number plus a specified epsilon.
 *
 * @tparam T Type of the numbers.
 * @param a The first number to compare.
 * @param b The second number to compare.
 * @param epsilon The tolerance threshold for considering the numbers as approximately equal.
 * @return True if a is less than or equal to (b + epsilon), false otherwise.
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
 * @brief Checks if a number is finite.
 *
 * This function checks whether the input number is finite, i.e., it is neither infinite nor NaN (Not a Number).
 *
 * @tparam T Type of the number.
 * @param x The number to check.
 * @return True if x is a finite number, false otherwise.
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
 * @brief Checks if a number is infinite.
 *
 * This function checks whether the input number is infinite.
 *
 * @tparam T Type of the number.
 * @param x The number to check.
 * @return True if x is infinite, false otherwise.
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
 * @brief Checks if a number is NaN (Not a Number).
 *
 * This function checks whether the input number is NaN (Not a Number).
 *
 * @tparam T Type of the number.
 * @param x The number to check.
 * @return True if x is NaN, false otherwise.
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

/**
 * @brief Rounds a number to the nearest integer if it is close enough.
 *
 * This function rounds the input number to the nearest integer if it is close
 * enough to the rounded value within a specified epsilon. If the difference
 * between the input number and its rounded value is within the epsilon, it
 * returns the rounded value, otherwise it returns the input number unchanged.
 *
 * @tparam T Type of the number.
 * @param x The number to round.
 * @param epsilon The tolerance threshold for considering the number as approximately equal to its rounded value.
 * @return The rounded value if close enough, otherwise the input number unchanged.
 */
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