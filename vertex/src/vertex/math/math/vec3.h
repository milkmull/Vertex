#pragma once

#include "type/vec3f_type.h"
#include "type/vec3i_type.h"

namespace vx {
namespace math {

template <typename T>
using vector3 = detail::vecx<3, T>;

}
}