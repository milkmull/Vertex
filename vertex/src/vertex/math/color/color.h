#pragma once

#include "detail/colorf_type.h"
#include "detail/colori_type.h"

namespace vx {

typedef math::detail::vec<4, float, math::detail::vec_t::col> color;

typedef math::detail::vec<4, uint8_t, math::detail::vec_t::col> color8;
typedef math::detail::vec<4, uint16_t, math::detail::vec_t::col> color16;

}