#pragma once

#include "type/vec4f_type.h"
#include "type/vec4i_type.h"

namespace vx {
namespace math {

template <typename T>
using vector4 = detail::vecx<4, T>;

}
}
