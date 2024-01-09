#pragma once

#include "detail/vec4i_type.h"
#include "detail/vec4f_type.h"

namespace vx {

typedef math::detail::vec<4, float, math::detail::vec_t::vec> vec4;

typedef math::detail::vec<4, float, math::detail::vec_t::vec> vec4f;
typedef math::detail::vec<4, double, math::detail::vec_t::vec> vec4d;
typedef math::detail::vec<4, int32_t, math::detail::vec_t::vec> vec4i;
typedef math::detail::vec<4, uint32_t, math::detail::vec_t::vec> vec4ui;
typedef math::detail::vec<4, bool, math::detail::vec_t::vec> vec4b;

}
