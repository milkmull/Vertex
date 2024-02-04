#pragma once

#include "detail/vec4f_type.h"
#include "detail/vec4i_type.h"

#include "vec_fn_common.h"
#include "vec_fn_comparison.h"
#include "math/vec_fn_geometric.h"

namespace vx {
namespace math {

template <typename T>
using vector4 = detail::vecx<4, T>;

}
}
