#pragma once

#include <type_traits>

namespace vx {
namespace math {
namespace detail {

using int_float_type = double;

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
struct to_float_type
{
    using type = typename std::conditional_t<std::is_integral<T>::value, int_float_type, T>;
};

enum class val_t
{
    integral,
    floating_point
};

template <typename T>
struct value_type_selector
{
    static constexpr val_t value = std::is_integral<T>::value ? val_t::integral : val_t::floating_point;
};

}
}
}