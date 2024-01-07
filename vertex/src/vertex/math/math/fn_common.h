#pragma once

#include <initializer_list>

#include "detail/common.h"

namespace vx {
namespace math {

// =============== min ===============

template <typename T>
inline constexpr T min(T x, T y);

template <typename T>
inline constexpr T min(std::initializer_list<T> ls);

// =============== max ===============

template <typename T>
inline constexpr T max(T x, T y);

template <typename T>
inline constexpr T max(std::initializer_list<T> ls);

// =============== abs ===============

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline constexpr T abs(T x);

// =============== round ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T round(T x);

// =============== trunc ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T trunc(T x);

// =============== floor ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T floor(T x);

// =============== ceil ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T ceil(T x);

// =============== sign ===============

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline constexpr T sign(T x);

// =============== fmod ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T fmod(T x, T y);

// =============== mod ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T mod(T x, T y);

// =============== fract ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T fract(T x);

// =============== modf ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T modf(T x, T& intpart);

// =============== frexp ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T frexp(T x, T& exp);

// =============== snap ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T snap(T x, T interval);

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline constexpr T snap(T x, T interval);

// =============== clamp ===============

template <typename T>
inline constexpr T clamp(T x, T min, T max);

}
}

#include "_impl/fn_common.inl"
