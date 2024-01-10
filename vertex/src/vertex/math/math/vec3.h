#pragma once

#include "detail/vec3i_type.h"
#include "detail/vec3f_type.h"

#include "vec_fn_common.h"
#include "vec_fn_comparison.h"
#include "vec_fn_exponential.h"
#include "vec_fn_geometric.h"
#include "vec_fn_interpolation.h"
#include "vec_fn_trigonometric.h"

namespace vx {

typedef math::detail::vec<3, float, math::detail::vec_t::vec> vec3;

typedef math::detail::vec<3, float, math::detail::vec_t::vec> vec3f;
typedef math::detail::vec<3, double, math::detail::vec_t::vec> vec3d;
typedef math::detail::vec<3, int32_t, math::detail::vec_t::vec> vec3i;
typedef math::detail::vec<3, uint32_t, math::detail::vec_t::vec> vec3ui;
typedef math::detail::vec<3, bool, math::detail::vec_t::vec> vec3b;

}
