#pragma once

#include "detail/quat_type.h"

#include "quat_fn_common.h"
#include "quat_fn_comparison.h"
#include "quat_fn_geometric.h"

namespace vx {
namespace math {

template <typename T>
using quaternion = detail::quat<T>;

}
}