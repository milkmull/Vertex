#pragma once

#include "detail/recti_type.h"
#include "detail/rectf_type.h"

#include "rect_fn_comparison.h"

namespace vx {

typedef math::detail::rect<float> rect2;

typedef math::detail::rect<float> rect2f;
typedef math::detail::rect<int32_t> rect2i;
typedef math::detail::rect<uint32_t> rect2ui;

}