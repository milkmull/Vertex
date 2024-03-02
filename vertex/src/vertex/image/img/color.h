#pragma once

#include "vertex/math/color/type/color_type.h"

namespace vx {
namespace img {

class image;

void fill(image& img, const math::color& color);
void invert(image& img);

void replace_color(image& img, const math::color& color1, const math::color& color2, float epsilon = math::epsilon<float>);
void swap_color(image& img, const math::color& color1, const math::color& color2, float epsilon = math::epsilon<float>);
void key(image& img, const math::color& color, float threshold = math::epsilon<float>);

}
}