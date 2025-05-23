#pragma once

#include <limits>

namespace vx {
namespace math {

template <typename T>
struct constants
{
    static constexpr T zero = static_cast<T>(0);
    static constexpr T one = static_cast<T>(1);

    static constexpr T epsilon = static_cast<T>(1e-6);
    static constexpr T infinity = std::numeric_limits<T>::infinity();
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    static constexpr T pi = static_cast<T>(3.141592653589793);
    static constexpr T quarter_pi = static_cast<T>(0.785398163397448);
    static constexpr T half_pi = static_cast<T>(1.570796326794896);
    static constexpr T three_pi_over_four = static_cast<T>(2.356194490192345);
    static constexpr T three_pi_over_two = static_cast<T>(4.712388980384690);
    static constexpr T two_pi = static_cast<T>(6.283185307179586);

    static constexpr T e = static_cast<T>(2.718281828459045);
    static constexpr T ln_two = static_cast<T>(0.693147180559945);
    static constexpr T ln_ten = static_cast<T>(2.302585092994046);

    static constexpr T inv_root_two = static_cast<T>(0.707106781186547);
    static constexpr T root_two = static_cast<T>(1.414213562373095);
    static constexpr T root_tree = static_cast<T>(1.732050807568877);
    static constexpr T root_five = static_cast<T>(2.23606797749979);

    static constexpr T golden_ratio = static_cast<T>(1.618033988749895);
};

} // namespace math
} // namespace vx