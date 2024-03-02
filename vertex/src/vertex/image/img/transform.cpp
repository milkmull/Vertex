#include "transform.h"
#include "image.h"
#include "enum.h"
#include "raw/transform.h"
#include "raw/filter/filter_nearest.h"
#include "raw/filter/filter_bilinear.h"

namespace vx {
namespace img {

image flip_x(const image& img)
{
    image out = img;
    flip_x(out.data(), out.width(), out.height(), out.channels());
    return out;
}

image flip_y(const image& img)
{
    image out = img;
    flip_y(out.data(), out.width(), out.height(), out.channels());
    return out;
}

image rotate_90_cw(const image& img)
{
    image out(img.height(), img.width(), img.format());
    rotate_90_cw(img.data(), img.width(), img.height(), out.data(), img.channels());
    return out;
}

image rotate_90_ccw(const image& img)
{
    image out(img.height(), img.width(), img.format());
    rotate_90_ccw(img.data(), img.width(), img.height(), out.data(), img.channels());
    return out;
}

image rotate_180(const image& img)
{
    image out = img;
    rotate_180(out.data(), out.width(), out.height(), out.channels());
    return out;
}

image crop(const image& img, const math::recti& area)
{
    const math::recti cropped = img.get_rect().crop(area);
    image out(cropped.size.x, cropped.size.y, img.format());

    const std::array<int32_t, 4> r{ cropped.position.x, cropped.position.y, cropped.size.x, cropped.size.y };
    crop(img.data(), img.width(), img.height(), out.data(), img.channels(), r);

    return out;
}

image resize(const image& img, const math::vec2i& size, image_filter filter)
{
    image out(size.x, size.y, img.format());

    switch (filter)
    {
        case image_filter::NEAREST:
        {
            filter_nearest(
                img.data(), img.width(), img.height(),
                out.data(), out.width(), out.height(),
                img.channels()
            );
            break;
        }
        case image_filter::LINEAR:
        {
            filter_bilinear(
                img.data(), img.width(), img.height(),
                out.data(), out.width(), out.height(),
                img.channels()
            );
            break;
        }
        default:
        {
            break;
        }
    }

    return out;
}

image resize(const image& img, const math::vec2i& size)
{
    const float pixel_area = (
        (static_cast<float>(size.x) / static_cast<float>(img.width())) *
        (static_cast<float>(size.y) / static_cast<float>(img.height()))
    );

    const image_filter filter = (pixel_area < 1.0f) ? image_filter::NEAREST : image_filter::LINEAR;
    return resize(img, size, filter);
}

}
}