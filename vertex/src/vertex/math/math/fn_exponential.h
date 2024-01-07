#pragma once

#include "detail/common.h"

namespace vx {
namespace math {

// =============== pow ===============

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline constexpr T pow(T x, T y);

// =============== exp ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T exp(T x);

// =============== exp2 ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T exp2(T x);

// =============== log ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T log(T x);

// =============== log2 ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T log2(T x);

// =============== sqrt ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T sqrt(T x);

// =============== inverse_sqrt ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T inverse_sqrt(T x);

}
}

#include "_impl/fn_exponential.inl"
