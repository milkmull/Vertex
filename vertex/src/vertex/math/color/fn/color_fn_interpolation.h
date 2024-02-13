#pragma once

#include "../detail/base_types.h"
#include "../../math/fn/fn_interpolation.h"

namespace vx {
namespace math {

// =============== lerp ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr colorf_t<T> lerp(
    const colorf_t<T>& x,
    const colorf_t<T>& y,
    T t
)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool> = true>
inline constexpr colori_t<T> lerp(
    const colori_t<T>& x,
    const colori_t<T>& y,
    typename colori_t<T>::float_value_type t
)
{
    using FT = decltype(t);
    return lerp(colorf_t<FT>(x), colorf_t<FT>(y), t);
}

}
}