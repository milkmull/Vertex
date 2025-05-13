#pragma once

#include <limits>

namespace vx {
namespace math {

template <typename T>
struct constants
{
    static constexpr T epsilon = static_cast<T>(1e-6);
    static constexpr T infinity = std::numeric_limits<T>::infinity();
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    static constexpr T pi = static_cast<T>(3.14159265358979323846);
    static constexpr T two_pi = static_cast<T>(2) * pi;
    static constexpr T half_pi = static_cast<T>(0.5) * pi;
    static constexpr T e = static_cast<T>(2.71828182845904523536);
    static constexpr T golden_ratio = static_cast<T>(1.61803398874989484820);
};

} // namespace math
} // namespace vx