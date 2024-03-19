#pragma once

#include <limits>

#include "../../detail/setup.h"

namespace vx {
namespace math {

template <typename T = float>
VX_FORCE_INLINE constexpr T epsilon() { return static_cast<T>(1e-6); }

template <typename T = float>
VX_FORCE_INLINE constexpr T infinity() { return std::numeric_limits<T>::infinity(); }

template <typename T = float>
VX_FORCE_INLINE constexpr T nan() { return std::numeric_limits<T>::quiet_NaN(); }

template <typename T = float>
VX_FORCE_INLINE constexpr T pi() { return static_cast<T>(3.14159265358979323846264338327950288); }

template <typename T = float>
VX_FORCE_INLINE constexpr T two_pi() { return static_cast<T>(2) * pi<T>(); }

template <typename T = float>
VX_FORCE_INLINE constexpr T half_pi() { return static_cast<T>(0.5) * pi<T>(); }

template <typename T = float>
VX_FORCE_INLINE constexpr T e() { return static_cast<T>(2.71828182845904523536028747135266249); }

template <typename T = float>
VX_FORCE_INLINE constexpr T golden_ratio() { return static_cast<T>(1.61803398874989484820458683436563811); }

}
}