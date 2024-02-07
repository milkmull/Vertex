#pragma once

#include "type/quat_type.h"

namespace vx {
namespace math {

template <typename T>
using quaternion = detail::quat<T>;

}
}