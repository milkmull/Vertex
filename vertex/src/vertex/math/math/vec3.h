#pragma once

#include "detail/vec3f_type.h"
#include "detail/vec3i_type.h"

#include "vec_fn_common.h"
#include "vec_fn_comparison.h"
#include "vec_fn_geometric.h"

namespace vx {
namespace math {

template <typename T>
using vector3 = detail::vecx<3, T>;

}
}