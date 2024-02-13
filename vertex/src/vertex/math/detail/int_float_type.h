#pragma once

#include <type_traits>

namespace vx {
namespace math {
namespace detail {

using int_float_type = double;

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
struct to_float_type : std::conditional<std::is_integral<T>::value, double, T> {};

}
}
}