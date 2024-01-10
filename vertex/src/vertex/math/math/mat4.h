#pragma once

#include "detail/mat4x4_type.h"

#include "mat_fn_comparison.h"
#include "mat_fn_projection.h"

namespace vx {

typedef math::detail::mat<4, 4, float> mat4;

typedef math::detail::mat<4, 4, float> mat4f;
typedef math::detail::mat<4, 4, double> mat4d;

}