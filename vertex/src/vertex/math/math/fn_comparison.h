#pragma once

#include "constants.h"

namespace vx {
namespace math {

// =============== is_zero_approx ===============

/**
 * @brief Check if a floating-point value is approximately zero.
 *
 * This template function checks if the given floating-point value is approximately zero
 * within a specified epsilon range. The function is enabled only for floating-point types.
 *
 * @tparam T The floating-point data type for which the check is performed.
 * @param x The input floating-point value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the value is approximately zero, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_zero_approx(T x, const T epsilon = math::epsilon<T>)
{
    return ((x < static_cast<T>(0)) ? -x : x) < epsilon;
}

/**
 * @brief Check if an integral value is approximately zero.
 *
 * This template function checks if the given integral value is zero. Since integral
 * values do not have a floating-point epsilon, the epsilon parameter is ignored.
 * The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the check is performed.
 * @param x The input integral value.
 * @param epsilon The epsilon value (ignored for integral types).
 * @return True if the value is zero, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_zero_approx(T x, const T epsilon = math::epsilon<T>)
{
    return x == static_cast<T>(0);
}

// =============== is_equal_approx ===============

/**
 * @brief Check if two floating-point values are approximately equal.
 *
 * This template function checks if two floating-point values are approximately equal
 * within a specified epsilon range. The function is enabled only for floating-point types.
 *
 * @tparam T The floating-point data type for which the check is performed.
 * @param a The first floating-point value.
 * @param b The second floating-point value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the values are approximately equal, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return is_zero_approx(a - b, epsilon);
}

/**
 * @brief Check if two integral values are equal.
 *
 * This template function checks if two integral values are equal. Since integral
 * values do not have a floating-point epsilon, the epsilon parameter is ignored.
 * The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the check is performed.
 * @param a The first integral value.
 * @param b The second integral value.
 * @param epsilon The epsilon value (ignored for integral types).
 * @return True if the values are equal, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a == b;
}

// =============== is_not_equal_approx ===============

/**
 * @brief Check if two floating-point values are not approximately equal.
 *
 * This template function checks if two floating-point values are not approximately equal
 * within a specified epsilon range. The function is enabled only for floating-point types.
 *
 * @tparam T The floating-point data type for which the check is performed.
 * @param a The first floating-point value.
 * @param b The second floating-point value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the values are not approximately equal, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_not_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return !is_zero_approx(a - b, epsilon);
}

/**
 * @brief Check if two integral values are not equal.
 *
 * This template function checks if two integral values are not equal. Since integral
 * values do not have a floating-point epsilon, the epsilon parameter is ignored.
 * The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the check is performed.
 * @param a The first integral value.
 * @param b The second integral value.
 * @param epsilon The epsilon value (ignored for integral types).
 * @return True if the values are not equal, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_not_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a != b;
}

// =============== is_greater_approx ===============

/**
 * @brief Check if a floating-point value is greater than another approximately.
 *
 * This template function checks if the first floating-point value is greater than
 * the second approximately within a specified epsilon range. The function is enabled
 * only for floating-point types.
 *
 * @tparam T The floating-point data type for which the check is performed.
 * @param a The first floating-point value.
 * @param b The second floating-point value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the first value is greater than the second approximately, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_greater_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a > (b + epsilon);
}

/**
 * @brief Check if an integral value is greater than another.
 *
 * This template function checks if the first integral value is greater than the second.
 * Since integral values do not have a floating-point epsilon, the epsilon parameter is ignored.
 * The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the check is performed.
 * @param a The first integral value.
 * @param b The second integral value.
 * @param epsilon The epsilon value (ignored for integral types).
 * @return True if the first value is greater than the second, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_greater_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a > b;
}

// =============== is_greater_or_equal_approx ===============

/**
 * @brief Check if a floating-point value is greater than or equal to another approximately.
 *
 * This template function checks if the first floating-point value is greater than or equal
 * to the second approximately within a specified epsilon range. The function is enabled
 * only for floating-point types.
 *
 * @tparam T The floating-point data type for which the check is performed.
 * @param a The first floating-point value.
 * @param b The second floating-point value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the first value is greater than or equal to the second approximately, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_greater_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a > (b - epsilon);
}

/**
 * @brief Check if an integral value is greater than or equal to another.
 *
 * This template function checks if the first integral value is greater than or equal
 * to the second. Since integral values do not have a floating-point epsilon, the
 * epsilon parameter is ignored. The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the check is performed.
 * @param a The first integral value.
 * @param b The second integral value.
 * @param epsilon The epsilon value (ignored for integral types).
 * @return True if the first value is greater than or equal to the second, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_greater_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a >= b;
}

// =============== is_less_approx ===============

/**
 * @brief Check if a floating-point value is less than another approximately.
 *
 * This template function checks if the first floating-point value is less than the
 * second approximately within a specified epsilon range. The function is enabled
 * only for floating-point types.
 *
 * @tparam T The floating-point data type for which the check is performed.
 * @param a The first floating-point value.
 * @param b The second floating-point value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the first value is less than the second approximately, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_less_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a < (b - epsilon);
}

/**
 * @brief Check if an integral value is less than another.
 *
 * This template function checks if the first integral value is less than the second.
 * Since integral values do not have a floating-point epsilon, the epsilon parameter is ignored.
 * The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the check is performed.
 * @param a The first integral value.
 * @param b The second integral value.
 * @param epsilon The epsilon value (ignored for integral types).
 * @return True if the first value is less than the second, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_less_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a < b;
}

// =============== is_less_or_equal_approx ===============

/**
 * @brief Check if a floating-point value is less than or equal to another approximately.
 *
 * This template function checks if the first floating-point value is less than or equal
 * to the second approximately within a specified epsilon range. The function is enabled
 * only for floating-point types.
 *
 * @tparam T The floating-point data type for which the check is performed.
 * @param a The first floating-point value.
 * @param b The second floating-point value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return True if the first value is less than or equal to the second approximately, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_less_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a < (b + epsilon);
}

/**
 * @brief Check if an integral value is less than or equal to another.
 *
 * This template function checks if the first integral value is less than or equal
 * to the second. Since integral values do not have a floating-point epsilon, the
 * epsilon parameter is ignored. The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the check is performed.
 * @param a The first integral value.
 * @param b The second integral value.
 * @param epsilon The epsilon value (ignored for integral types).
 * @return True if the first value is less than or equal to the second, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_less_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>)
{
    return a <= b;
}

// =============== is_finite ===============

/**
 * @brief Check if a floating-point value is finite.
 *
 * This template function checks if the given floating-point value is finite.
 * The function is enabled only for floating-point types.
 *
 * @tparam T The floating-point data type for which the check is performed.
 * @param x The input floating-point value.
 * @return True if the value is finite, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_finite(T x)
{
    return std::isfinite(x);
}

/**
 * @brief Check if an integral value is finite.
 *
 * This template function always returns true for integral values since they
 * are inherently finite. The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the check is performed.
 * @param x The input integral value.
 * @return Always returns true for integral types.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_finite(T x)
{
    return true;
}

// =============== is_infinite ===============

/**
 * @brief Check if a floating-point value is infinite.
 *
 * This template function checks if the given floating-point value is infinite.
 * The function is enabled only for floating-point types.
 *
 * @tparam T The floating-point data type for which the check is performed.
 * @param x The input floating-point value.
 * @return True if the value is infinite, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_infinite(T x)
{
    return std::isinf(x);
}

/**
 * @brief Check if an integral value is infinite.
 *
 * This template function always returns false for integral values since they
 * are inherently not infinite. The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the check is performed.
 * @param x The input integral value.
 * @return Always returns false for integral types.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_infinite(T x)
{
    return false;
}

// =============== is_nan ===============

/**
 * @brief Check if a floating-point value is NaN (Not a Number).
 *
 * This template function checks if the given floating-point value is NaN.
 * The function is enabled only for floating-point types.
 *
 * @tparam T The floating-point data type for which the check is performed.
 * @param x The input floating-point value.
 * @return True if the value is NaN, false otherwise.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_nan(T x)
{
    return std::isnan(x);
}

/**
 * @brief Check if an integral value is NaN (Not a Number).
 *
 * This template function always returns false for integral values since they
 * cannot represent NaN. The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the check is performed.
 * @param x The input integral value.
 * @return Always returns false for integral types.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_nan(T x)
{
    return false;
}

// =============== round_if_close ===============

/**
 * @brief Round a floating-point value if it is close to an integer.
 *
 * This template function rounds the given floating-point value to the nearest integer
 * if it is close enough (within a specified epsilon range). If the value is not close
 * to an integer, it returns the original value. The function is enabled only for
 * floating-point types.
 *
 * @tparam T The floating-point data type for which the rounding is performed.
 * @param x The input floating-point value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return The rounded value if close to an integer, otherwise the original value.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T round_if_close(T x, const T epsilon = math::epsilon<T>)
{
    const T xr = std::round(x);
    return is_equal_approx(x, xr, epsilon) ? xr : x;
}

/**
 * @brief Round an integral value.
 *
 * This template function always returns the original value for integral types since
 * they are already considered rounded to the nearest integer. The function is enabled
 * only for integral types.
 *
 * @tparam T The integral data type for which the rounding is performed.
 * @param x The input integral value.
 * @param epsilon The epsilon value (ignored for integral types).
 * @return The original integral value.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr T round_if_close(T x, const T epsilon = math::epsilon<T>)
{
    return x;
}

// =============== make_pretty ===============

/**
 * @brief Make a floating-point value "pretty" by rounding if close to an integer.
 *
 * This template function makes the given floating-point value "pretty" by rounding
 * it to the nearest integer if it is close enough (within a specified epsilon range).
 * If the rounded value is zero, it returns the absolute value of the input value.
 * The function is enabled only for floating-point types.
 *
 * @tparam T The floating-point data type for which the operation is performed.
 * @param x The input floating-point value.
 * @param epsilon The epsilon value for the approximation (default is math::epsilon<T>).
 * @return The "pretty" floating-point value.
 */
template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T make_pretty(T x, const T epsilon = math::epsilon<T>)
{
    const T r = round_if_close(x, epsilon);
    return (r == static_cast<T>(0)) ? std::abs(r) : r;
}

/**
 * @brief Make an integral value "pretty."
 *
 * This template function always returns the original value for integral types since
 * they are considered "pretty" by default. The function is enabled only for integral types.
 *
 * @tparam T The integral data type for which the operation is performed.
 * @param x The input integral value.
 * @param epsilon The epsilon value (ignored for integral types).
 * @return The original integral value.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr T make_pretty(T x, const T epsilon = math::epsilon<T>)
{
    return x;
}

}
}