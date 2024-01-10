#pragma once

#include "detail/quat_type.h"

#include "quat_fn_common.h"
#include "quat_fn_comparison.h"
#include "quat_fn_exponential.h"
#include "quat_fn_geometric.h"
#include "quat_fn_interpolation.h"

namespace vx {

typedef math::detail::quat<float> quaternion;

typedef math::detail::quat<float> quaternionf;
typedef math::detail::quat<double> quaterniond;

}