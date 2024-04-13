#pragma once

#include "../_priv/common.h"
#include "../constants.h"

namespace vx {
namespace math {

// =============== common ===============

/**
 * @brief Checks if two values are equal.
 *
 * @tparam T The type of the values.
 * @param x The first value.
 * @param y The second value.
 * @return True if the values are equal, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool equal(T x, T y)
{
    return x == y;
}

/**
 * @brief Checks if two values are not equal.
 *
 * @tparam T The type of the values.
 * @param x The first value.
 * @param y The second value.
 * @return True if the values are not equal, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool not_equal(T x, T y)
{
    return x != y;
}

/**
 * @brief Checks if the first value is greater than the second value.
 *
 * @tparam T The type of the values.
 * @param x The first value.
 * @param y The second value.
 * @return True if the first value is greater than the second value, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool greater_than(T x, T y)
{
    return x > y;
}

/**
 * @brief Checks if the first value is greater than or equal to the second value.
 *
 * @tparam T The type of the values.
 * @param x The first value.
 * @param y The second value.
 * @return True if the first value is greater than or equal to the second value, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool greater_than_equal(T x, T y)
{
    return x >= y;
}

/**
 * @brief Checks if the first value is less than the second value.
 *
 * @tparam T The type of the values.
 * @param x The first value.
 * @param y The second value.
 * @return True if the first value is less than the second value, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool less_than(T x, T y)
{
    return x < y;
}

/**
 * @brief Checks if the first value is less than or equal to the second value.
 *
 * @tparam T The type of the values.
 * @param x The first value.
 * @param y The second value.
 * @return True if the first value is less than or equal to the second value, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool less_than_equal(T x, T y)
{
    return x <= y;
}

// =============== is_inf ===============

/**
 * @brief Checks if a value is infinite.
 *
 * @tparam T Type of the value.
 * @param x The value to check.
 * @return True if x is infinite, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_inf(T x)
{
    return std::isinf(x);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_inf(T x)
{
    return false;
}

// =============== is_nan ===============

/**
 * @brief Checks if a value is NaN (Not a number).
 *
 * @tparam T Type of the value.
 * @param x The value to check.
 * @return True if x is NaN, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_nan(T x)
{
    return std::isnan(x);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_nan(T x)
{
    return false;
}

// =============== is_zero_approx ===============

/**
 * @brief Checks if a value is approximately zero.
 *
 * This function checks whether the absolute value of the input value is less than a specified epsilon.
 *
 * @tparam T Type of the value.
 * @param x The value to check.
 * @param epsilon The tolerance threshold for considering a value as zero.
 * @return True if the absolute value of x is less than epsilon, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_zero_approx(T x, const T epsilon = math::epsilon<T>())
{
    return ((x < static_cast<T>(0)) ? -x : x) < epsilon;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_zero_approx(T x, const T epsilon = math::epsilon<T>())
{
    return x == static_cast<T>(0);
}

// =============== is_equal_approx ===============

/**
 * @brief Checks if two values are approximately equal.
 *
 * This function checks whether the absolute difference between two values is less than a specified epsilon.
 *
 * @tparam T Type of the values.
 * @param a The first value to compare.
 * @param b The second value to compare.
 * @param epsilon The tolerance threshold for considering the values as equal.
 * @return True if the absolute difference between a and b is less than epsilon, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_equal_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return is_zero_approx(a - b, epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_equal_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return a == b;
}

// =============== is_not_equal_approx ===============

/**
 * @brief Checks if two values are approximately not equal.
 *
 * This function checks whether the absolute difference between two values is greater than or equal to a specified epsilon.
 *
 * @tparam T Type of the values.
 * @param a The first value to compare.
 * @param b The second value to compare.
 * @param epsilon The tolerance threshold for considering the values as equal.
 * @return True if the absolute difference between a and b is greater than or equal to epsilon, false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_not_equal_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return !is_zero_approx(a - b, epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_not_equal_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return a != b;
}

// =============== is_greater_than_approx ===============

/**
 * @brief Checks if a value is approximately greater than another.
 *
 * This function checks whether the first value is greater than the second value plus a specified epsilon.
 *
 * @tparam T Type of the values.
 * @param a The first value to compare.
 * @param b The second value to compare.
 * @param epsilon The tolerance threshold for considering the values as approximately equal.
 * @return True if a is greater than (b + epsilon), false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_greater_than_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return a > (b + epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_greater_than_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return a > b;
}

// =============== is_greater_than_equal_approx ===============

/**
 * @brief Checks if a value is approximately greater than or equal to another.
 *
 * This function checks whether the first value is greater than or equal to the second value minus a specified epsilon.
 *
 * @tparam T Type of the values.
 * @param a The first value to compare.
 * @param b The second value to compare.
 * @param epsilon The tolerance threshold for considering the values as approximately equal.
 * @return True if a is greater than or equal to (b - epsilon), false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_greater_than_equal_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return a > (b - epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_greater_than_equal_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return a >= b;
}

// =============== is_less_than_approx ===============

/**
 * @brief Checks if a value is approximately less than another.
 *
 * This function checks whether the first value is less than the second value minus a specified epsilon.
 *
 * @tparam T Type of the values.
 * @param a The first value to compare.
 * @param b The second value to compare.
 * @param epsilon The tolerance threshold for considering the values as approximately equal.
 * @return True if a is less than (b - epsilon), false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_less_than_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return a < (b - epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_less_than_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return a < b;
}

// =============== is_less_than_equal_approx ===============

/**
 * @brief Checks if a value is approximately less than or equal to another.
 *
 * This function checks whether the first value is less than or equal to the second value plus a specified epsilon.
 *
 * @tparam T Type of the values.
 * @param a The first value to compare.
 * @param b The second value to compare.
 * @param epsilon The tolerance threshold for considering the values as approximately equal.
 * @return True if a is less than or equal to (b + epsilon), false otherwise.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_less_than_equal_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return a < (b + epsilon);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_less_than_equal_approx(T a, T b, const T epsilon = math::epsilon<T>())
{
    return a <= b;
}

}
}