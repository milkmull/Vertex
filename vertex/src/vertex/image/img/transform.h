#pragma once

#include "detail/base_types.h"
#include "vertex/math/geometry/type/rect_type.h"

namespace vx {
namespace img {

struct image_info;
class image;
enum class image_filter;

image flip_x(const image& img);
image flip_y(const image& img);

image rotate_90_cw(const image& img);
image rotate_90_ccw(const image& img);
image rotate_180(const image& img);

image crop(const image& img, const math::recti& area);

image resize(const image& img, const math::vec2i& size, image_filter filter);
image resize(const image& img, const math::vec2i& size);

}
}