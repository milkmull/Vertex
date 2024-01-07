#pragma once

#include "detail/common.h"

namespace vx {
namespace math {

// =============== radians ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T radians(T deg) noexcept;

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr auto radians(T deg);

// =============== degrees ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T degrees(T rad) noexcept;

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr auto degrees(T rad);

// =============== sin ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T sin(T x);

// =============== cos ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T cos(T x);

// =============== tan ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T tan(T x);

// =============== asin ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T asin(T x);

// =============== asin clamped ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T asin_clamped(T x);

// =============== acos ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T acos(T x);

// =============== acos clamped ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T acos_clamped(T x);

// =============== atan ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T atan(T x);

// =============== atan2 ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T atan2(T x, T y);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr auto atan2(T x, T y);

// =============== sinh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T sinh(T x);

// =============== cosh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T cosh(T x);

// =============== tanh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T tanh(T x);

// =============== asinh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T asinh(T x);

// =============== acosh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T acosh(T x);

// =============== atanh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T atanh(T x);

}
}

#include "_impl/fn_trigonometric.inl"