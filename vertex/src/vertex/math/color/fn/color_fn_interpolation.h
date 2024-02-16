#pragma once

#include "../detail/base_types.h"
#include "../../math/fn/fn_interpolation.h"

namespace vx {
namespace math {

// =============== lerp ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr color_t<T> lerp(
    const color_t<T>& x,
    const color_t<T>& y,
    T t
)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool> = true>
inline constexpr color_t<T> lerp(
    const color_t<T>& x,
    const color_t<T>& y,
    typename color_t<T>::float_value_type t
)
{
    using float_type = typename color_t<T>::float_type;
    return lerp(float_type(x), float_type(y), t);
}

}
}