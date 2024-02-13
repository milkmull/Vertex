#pragma once

#include <type_traits>

namespace vx {
namespace math {
namespace type_traits {

template <typename T>
struct to_float_type
{
    static_assert(std::is_arithmetic<T>::value, "T must be arithmetic type");
    using type = typename std::conditional<std::is_integral<T>::value, double, T>::type;
};

}
}
}