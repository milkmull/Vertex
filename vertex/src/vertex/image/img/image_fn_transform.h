#pragma once

#include "detail/base_type_defs.h"

namespace vx {

namespace img {

struct image_info;
class image;

image flip_x(const image& img);
image flip_y(const image& img);

image rotate_90_cw(const image& img);
image rotate_90_ccw(const image& img);
image rotate_180(const image& img);

//static image_info crop(const image_info& info, byte_type* data, byte_type*& out, const math::recti& area);
//static image crop(const image& img, const math::recti& area);

}
}