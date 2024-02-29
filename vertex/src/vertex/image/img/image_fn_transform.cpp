#include "image_fn_transform.h"
#include "raw/transform.h"
#include "image.h"

namespace vx {
namespace img {

image flip_x(const image& img)
{
    image out = img;
    flip_x(out.raw_data(), out.width(), out.height(), out.channels());
    return out;
}

image flip_y(const image& img)
{
    image out = img;
    flip_y(out.raw_data(), out.width(), out.height(), out.channels());
    return out;
}

image rotate_90_cw(const image& img)
{
    image out(img.height(), img.width(), img.format());
    rotate_90_cw(img.raw_data(), img.width(), img.height(), out.raw_data(), img.channels());
    return out;
}

image rotate_90_ccw(const image& img)
{
    image out(img.height(), img.width(), img.format());
    rotate_90_ccw(img.raw_data(), img.width(), img.height(), out.raw_data(), img.channels());
    return out;
}

image rotate_180(const image& img)
{
    image out = img;
    rotate_180(out.raw_data(), out.width(), out.height(), out.channels());
    return out;
}

}
}