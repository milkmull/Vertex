#pragma once

#include <limits>

#include "vertex/math/types.hpp"

namespace vx {
namespace math {

template <typename T>
struct constants
{
    static constexpr T epsilon = T{ 1e-6 };
    static constexpr T infinity = std::numeric_limits<T>::infinity();
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    static constexpr T pi = T{ 3.14159265358979323846 };
    static constexpr T two_pi = T{ 2 } * pi;
    static constexpr T half_pi = T{ 0.5 } * pi;
    static constexpr T e = T{ 2.71828182845904523536 };
    static constexpr T golden_ratio = T{ 1.61803398874989484820 };
};

using const_b = constants<b>;
using const_i = constants<i>;
using const_u = constants<u>;
using const_f = constants<f>;
using const_d = constants<d>;

} // namespace math
} // namespace vx