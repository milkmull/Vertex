#pragma once

#include "detail/vec2f_type.h"
#include "detail/vec2i_type.h"

#include "vec_fn_common.h"
#include "vec_fn_comparison.h"
#include "vec_fn_geometric.h"

namespace vx {
namespace math {

template <typename T>
using vector2 = detail::vecx<2, T>;

}
}