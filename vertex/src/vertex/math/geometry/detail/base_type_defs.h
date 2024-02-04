#pragma once

#include "../../math/detail/base_type_defs.h"

namespace vx {
namespace math {
namespace detail {

template <typename T, val_t V = value_type_selector<T>::value> struct rect;

template <typename T>
using rectx = rect<T>;

template <typename T>
using recti = rect<T, val_t::integral>;

template <typename T>
using rectf = rect<T, val_t::floating_point>;

}
}
}