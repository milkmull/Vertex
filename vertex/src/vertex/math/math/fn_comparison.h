#pragma once

#include "constants.h"

namespace vx {
namespace math {

// =============== is_zero_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_zero_approx(T x, const T epsilon = math::epsilon<T>);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_zero_approx(T x, const T epsilon = math::epsilon<T>);

// =============== is_equal_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_equal_approx(T a, T b, const T epsilon = math::epsilon<T>);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_equal_approx(T a, T b, const T epsilon = math::epsilon<T>);

// =============== is_not_equal_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_not_equal_approx(T a, T b, const T epsilon = math::epsilon<T>);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_not_equal_approx(T a, T b, const T epsilon = math::epsilon<T>);

// =============== is_greater_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_greater_approx(T a, T b, const T epsilon = math::epsilon<T>);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_greater_approx(T a, T b, const T epsilon = math::epsilon<T>);

// =============== is_greater_or_equal_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_greater_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_greater_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>);

// =============== is_less_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_less_approx(T a, T b, const T epsilon = math::epsilon<T>);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_less_approx(T a, T b, const T epsilon = math::epsilon<T>);

// =============== is_less_or_equal_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_less_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_less_or_equal_approx(T a, T b, const T epsilon = math::epsilon<T>);

// =============== is_finite ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_finite(T x);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_finite(T x);

// =============== is_infinite ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_infinite(T x);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_infinite(T x);

// =============== is_nan ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr bool is_nan(T x);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr bool is_nan(T x);

// =============== round_if_close ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T round_if_close(T x, const T epsilon = math::epsilon<T>);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr T round_if_close(T x, const T epsilon = math::epsilon<T>);

// =============== make_pretty ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T make_pretty(T x, const T epsilon = math::epsilon<T>);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr T make_pretty(T x, const T epsilon = math::epsilon<T>);

}
}

#include "_impl/fn_comparison.inl"