#pragma once

#include "detail/base_type_defs.h"

namespace vx {
namespace img {

struct image_info;
class image;

namespace transform {

image_info flip_x(const image_info& info, byte_type* data);
void flip_x(image& img);

image_info flip_y(const image_info& info, byte_type* data);
void flip_y(image& img);

image_info rotate_90_cw(const image_info& info, byte_type* data);
void rotate_90_cw(image& img);

image_info rotate_90_ccw(const image_info& info, byte_type* data);
void rotate_90_ccw(image& img);

image_info rotate_180(const image_info& info, byte_type* data);
void rotate_180(image& img);

}
}
}