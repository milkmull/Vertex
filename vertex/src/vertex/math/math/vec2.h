#pragma once

#include "detail/vec2f_type.h"
#include "detail/vec2i_type.h"

namespace vx {

typedef math::detail::vec<2, float, math::detail::vec_t::vec> vec2;

typedef math::detail::vec<2, float, math::detail::vec_t::vec> vec2f;
typedef math::detail::vec<2, double, math::detail::vec_t::vec> vec2d;
typedef math::detail::vec<2, int32_t, math::detail::vec_t::vec> vec2i;
typedef math::detail::vec<2, uint32_t, math::detail::vec_t::vec> vec2ui;
typedef math::detail::vec<2, bool, math::detail::vec_t::vec> vec2b;

}