#pragma once

#include <initializer_list>

#include "detail/common.h"

namespace vx {
namespace math {

// =============== min ===============

/**
 * @brief Get the minimum of two values.
 *
 * This function calculates the minimum of two values and returns the result.
 *
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
 * This function calculates the minimum value from the provided initializer list
 * and returns the result.
 *
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
 * This function calculates the maximum of two values and returns the result.
 *
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
 * This function calculates the maximum value from the provided initializer list
 * and returns the result.
 *
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
 * This function calculates the absolute value of the input and
 * returns the result.
 *
 * @param x The input value.
 * @return The absolute value of the input.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr T abs(T x)
{
    return std::abs(x);
}

// =============== round ===============

/**
 * @brief Round a value to the nearest integer.
 *
 * This function rounds the given value to the nearest integer
 * and returns the result.
 *
 * @param x The input value.
 * @return The nearest integer after rounding the input.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T round(T x)
{
    return std::round(x);
}

// =============== trunc ===============

/**
 * @brief Truncate a floating point value.
 *
 * This function truncates the given value and returns
 * the result.
 *
 * @param x The input value.
 * @return The truncated value towards zero.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T trunc(T x)
{
    return std::trunc(x);
}

// =============== floor ===============

/**
 * @brief Round down a value to the nearest integer.
 *
 * This function rounds down the given value to the nearest
 * integer and returns the result.
 *
 * @param x The input value.
 * @return The largest integer less than or equal to the input value.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T floor(T x)
{
    return std::floor(x);
}

// =============== ceil ===============

/**
 * @brief Round up a value to the nearest integer.
 *
 * This function rounds up the given value to the nearest
 * integer and returns the result.
 *
 * @param x The input value.
 * @return The smallest integer greater than or equal to the input value.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T ceil(T x)
{
    return std::ceil(x);
}

// =============== sign ===============

/**
 * @brief Get the sign of a numeric value.
 *
 * This function determines the sign of the given numeric value and
 * returns +1 if the value is positive, -1 if negative, and 0 if the value is zero.
 * The function is enabled only for arithmetic types.
 *
 * @param x The input numeric value.
 * @return +1 if the value is positive, -1 if negative, and 0 if the value is zero.
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
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
 * This function calculates the remainder of the division
 * operation (x/y) and returns the result.
 *
 * @param x The numerator.
 * @param y The denominator.
 * @return The remainder of (x/y).
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T fmod(T x, T y)
{
    return x - y * std::trunc(x / y);
}

// =============== mod ===============

/**
 * @brief Calculate the modulus of the division operation.
 *
 * This function calculates the modulus of the division
 * operation (x/y) and returns the result.
 *
 * @param x The numerator.
 * @param y The denominator.
 * @return The modulus of (x/y).
 * 
 * @ref https://registry.khronos.org/OpenGL-Refpages/gl4/html/mod.xhtml
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T mod(T x, T y)
{
    return x - y * std::floor(x / y);
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr detail::int_float_type mod(T x, T y)
{
    return static_cast<detail::int_float_type>((x % y + y) % y);
}

// =============== fract ===============

/**
 * @brief Get the fractional part of a value.
 *
 * This function extracts the fractional part of the given
 * value and returns the result.
 *
 * @param x The input value.
 * @return The fractional part of the input value.
 *
 * @ref https://registry.khronos.org/OpenGL-Refpages/gl4/html/fract.xhtml
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T fract(T x)
{
    return x - std::floor(x);
}

// =============== modf ===============

/**
 * @brief Split a value into integer and fractional parts.
 *
 * This function splits the given value into its integer
 * and fractional parts and returns the fractional part. The integer part is stored
 * in the reference parameter 'intpart'.
 *
 * @param x The input value.
 * @param intpart Reference parameter to store the integer part.
 * @return The fractional part of the input value.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T modf(T x, T& intpart)
{
    return std::modf(x, &intpart);
}

// =============== frexp ===============

/**
 * @brief Decompose a value into a normalized fraction and exponent.
 *
 * This function decomposes the given value into a normalized
 * fraction and an exponent, returning the fraction. The exponent is stored in the
 * reference parameter 'exp'.
 *
 * @param x The input floating-point value.
 * @param exp Reference parameter to store the exponent.
 * @return The normalized fraction of the input value.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T frexp(T x, int& exp)
{
    return std::frexp(x, &exp);
}

// =============== snap ===============

/**
 * @brief Snap a value to the nearest multiple of a specified interval.
 *
 * This function snaps the given value to the nearest multiple of a specified
 * interval and returns the result. If the interval is zero, the original value is
 * returned unchanged.
 *
 * @param x The input value to be snapped.
 * @param interval The specified interval for snapping.
 * @return The snapped value to the nearest multiple of the interval.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T snap(T x, T interval)
{
    if (interval == static_cast<T>(0)) return x;
    return static_cast<T>(std::round(x / interval)) * interval;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
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
 * This function clamps the given value within the specified range [min, max]
 * and returns the result. If the value is less than min, it returns min; if the value
 * is greater than max, it returns max; otherwise, it returns the original value.
 *
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

//#pragma once

#include "detail/vectorize.h"
//#include "fn_common.h"

namespace vx {
namespace math {

// =============== min ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto min(
    const detail::vec<L, T, Q>& x,
    T y
)
{
    return detail::vec_scaler_binary<L, T, Q, T>::call<math::min<T>>(x, y);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto min(
    const detail::vec<L, T, Q>& x,
    const detail::vec<L, T, Q>& y
)
{
    return detail::vec_binary<L, T, Q, T>::call<math::min<T>>(x, y);
}

// =============== max ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto max(
    const detail::vec<L, T, Q>& x,
    T y
)
{
    return detail::vec_scaler_binary<L, T, Q, T>::call<math::max<T>>(x, y);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto max(
    const detail::vec<L, T, Q>& x,
    const detail::vec<L, T, Q>& y
)
{
    return detail::vec_binary<L, T, Q, T>::call<math::max<T>>(x, y);
}

// =============== abs ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto abs(const detail::vec<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::abs<T>>(x);
}

// =============== round ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto round(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::round<T>>(x);
}

// =============== trunc ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto trunc(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::trunc<T>>(x);
}

// =============== floor ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto floor(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::floor<T>>(x);
}

// =============== ceil ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto ceil(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::ceil<T>>(x);
}

// =============== sign ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto sign(const detail::vec<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::sign<T>>(x);
}

// =============== fmod ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto fmod(
    const detail::vecf<L, T, Q>& x,
    T y
)
{
    return detail::vec_scaler_binary<L, T, Q, T>::call<math::fmod<T>>(x, y);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto fmod(
    const detail::vecf<L, T, Q>& x,
    const detail::vecf<L, T, Q>& y
)
{
    return detail::vec_binary<L, T, Q, T>::call<math::fmod<T>>(x, y);
}

// =============== mod ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto mod(
    const detail::vec<L, T, Q>& x,
    T y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return detail::vec_scaler_binary<L, T, Q, R>::call<math::mod<T>>(x, y);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto mod(
    const detail::vec<L, T, Q>& x,
    const detail::vec<L, T, Q>& y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return detail::vec_binary<L, T, Q, R>::call<math::mod<T>>(x, y);
}

// =============== fract ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto fract(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::fract<T>>(x);
}

// =============== modf ===============

template <typename T>
inline constexpr auto modf(
    const detail::vecf<2, T>& x,
    detail::vecf<2, T>& intpart
)
{
    return detail::vecf<2, T>(
        math::modf(x.x, intpart.x),
        math::modf(x.y, intpart.y)
    );
}

template <typename T>
inline constexpr auto modf(
    const detail::vecf<3, T>& x,
    detail::vecf<3, T>& intpart
)
{
    return detail::vecf<3, T>(
        math::modf(x.x, intpart.x),
        math::modf(x.y, intpart.y),
        math::modf(x.z, intpart.z)
    );
}

template <typename T>
inline constexpr auto modf(
    const detail::vecf<4, T>& x,
    detail::vecf<4, T>& intpart
)
{
    return detail::vecf<4, T>(
        math::modf(x.x, intpart.x),
        math::modf(x.y, intpart.y),
        math::modf(x.z, intpart.z),
        math::modf(x.w, intpart.w)
    );
}

template <typename T>
inline constexpr auto modf(
    const detail::colf<T>& x,
    detail::colf<T>& intpart
)
{
    return detail::colf<T>(
        math::modf(x.r, intpart.r),
        math::modf(x.g, intpart.g),
        math::modf(x.b, intpart.b),
        math::modf(x.a, intpart.a)
    );
}

// =============== frexp ===============

// here the template param Q is not required, but having it in the template helps avoid
// full specialization of veci type before it has been defined

template <typename T, detail::vec_t Q>
inline constexpr auto frexp(
    const detail::vecf<2, T>& x,
    detail::vec<2, int, Q>& exp
)
{
    return detail::vecf<2, T>(
        math::frexp(x.x, exp.x),
        math::frexp(x.y, exp.y)
    );
}

template <typename T, detail::vec_t Q>
inline constexpr auto frexp(
    const detail::vecf<3, T>& x,
    detail::vec<3, int, Q>& exp
)
{
    return detail::vecf<3, T>(
        math::frexp(x.x, exp.x),
        math::frexp(x.y, exp.y),
        math::frexp(x.z, exp.z)
    );
}

template <typename T, detail::vec_t Q>
inline constexpr auto frexp(
    const detail::vecf<4, T>& x,
    detail::vec<4, int, Q>& exp
)
{
    return detail::vecf<4, T>(
        math::frexp(x.x, exp.x),
        math::frexp(x.y, exp.y),
        math::frexp(x.z, exp.z),
        math::frexp(x.w, exp.w)
    );
}

template <typename T, detail::vec_t Q>
inline constexpr auto frexp(
    const detail::colf<T>& x,
    detail::vec<4, int, Q>& exp
)
{
    return detail::colf<T>(
        math::frexp(x.r, exp.r),
        math::frexp(x.g, exp.g),
        math::frexp(x.b, exp.b),
        math::frexp(x.a, exp.a)
    );
}

// =============== snap ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto snap(
    const detail::vec<L, T, Q>& x,
    T interval
)
{
    return detail::vec_scaler_binary<L, T, Q, T>::call<math::snap<T>>(x, interval);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto snap(
    const detail::vec<L, T, Q>& x,
    const detail::vec<L, T, Q>& interval
)
{
    return detail::vec_binary<L, T, Q, T>::call<math::snap<T>>(x, interval);
}

// =============== clamp ===============

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<2, T>& x,
    T min,
    T max
)
{
    return detail::vecx<2, T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<3, T>& x,
    T min,
    T max
)
{
    return detail::vecx<3, T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max),
        math::clamp(x.z, min, max)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<4, T>& x,
    T min,
    T max
)
{
    return detail::vecx<4, T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max),
        math::clamp(x.z, min, max),
        math::clamp(x.w, min, max)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::colx<T>& x,
    T min,
    T max
)
{
    return detail::colx<T>(
        math::clamp(x.r, min, max),
        math::clamp(x.g, min, max),
        math::clamp(x.b, min, max),
        math::clamp(x.a, min, max)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<2, T>& x,
    const detail::vecx<2, T>& min,
    const detail::vecx<2, T>& max
)
{
    return detail::vecx<2, T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<3, T>& x,
    const detail::vecx<3, T>& min,
    const detail::vecx<3, T>& max
)
{
    return detail::vecx<3, T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y),
        math::clamp(x.z, min.z, max.z)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<4, T>& x,
    const detail::vecx<4, T>& min,
    const detail::vecx<4, T>& max
)
{
    return detail::vecx<4, T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y),
        math::clamp(x.z, min.z, max.z),
        math::clamp(x.w, min.w, max.w)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::colx<T>& v,
    const detail::colx<T>& min,
    const detail::colx<T>& max
)
{
    return detail::colx<T>(
        math::clamp(v.r, min.r, max.r),
        math::clamp(v.g, min.g, max.g),
        math::clamp(v.b, min.b, max.b),
        math::clamp(v.a, min.a, max.a)
    );
}

}
}

