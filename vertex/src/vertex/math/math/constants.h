#pragma once

#include "detail/common.h"

namespace vx {
namespace math {

template <typename T>
inline constexpr T epsilon = static_cast<T>(1e-6);

template <typename T>
inline constexpr T zero = static_cast<T>(0);

template <typename T>
inline constexpr T one = static_cast<T>(1);

template <typename T = float>
inline constexpr T infinity = std::numeric_limits<T>::infinity();

template <typename T = float>
inline constexpr T nan = std::numeric_limits<T>::quiet_NaN();

template <typename T = float>
inline constexpr T pi = static_cast<T>(3.14159265358979323846264338327950288);

template <typename T = float>
inline constexpr T two_pi = static_cast<T>(2) * pi<T>;

template <typename T = float>
inline constexpr T half_pi = static_cast<T>(0.5) * pi<T>;

template <typename T = float>
inline constexpr T e = static_cast<T>(2.71828182845904523536);

template <typename T = float>
inline constexpr T golden_ratio = static_cast<T>(1.61803398874989484820458683436563811);

}
}