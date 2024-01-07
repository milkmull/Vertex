#pragma once

#include "detail/common.h"

namespace vx {
namespace math {

// =============== lerp ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T lerp(T x, T y, T t);

// =============== step ===============

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
inline constexpr T step(T edge, T x);

// =============== smoothstep ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
inline constexpr T smoothstep(T edge0, T edge1, T x);

}
}

#include "_impl/fn_interpolation.inl"