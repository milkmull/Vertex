#pragma once

#include <cmath>
#include <algorithm>
#include <type_traits>

#include "../../setup.h"

namespace vx {
namespace math {
namespace detail {

using int_float_type = double;

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
struct to_float_type
{
    using type = typename std::conditional_t<std::is_integral<T>::value, int_float_type, T>;
};

}
}
}