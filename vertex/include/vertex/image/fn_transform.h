#pragma once

#include "image.h"

#include "vertex/pixel/transform.h"
#include "vertex/pixel/enum_filter_mode.h"
#include "vertex/pixel/filter/filter_nearest.h"
#include "vertex/pixel/filter/filter_bilinear.h"

#include "vertex/math/math/fn/fn_exponential.h"

namespace vx {
namespace img {
namespace transform {

///////////////////////////////////////////////////////////////////////////////
// copy
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Make a copy of the image.
///
/// @param img The input image to be coppied.
/// 
/// @return The coppied image.
///////////////////////////////////////////////////////////////////////////////
inline image copy(const image& img)
{
    image out = img;
    pixel::transform::copy(out.data(), out.width(), out.height(), out.data(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// flip
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Flip the image horizontally (along the x-axis).
///
/// @param img The input image to be flipped.
/// 
/// @return The flipped image.
///////////////////////////////////////////////////////////////////////////////
inline image flip_x(const image& img)
{
    image out = img;
    pixel::transform::flip_x(out.data(), out.width(), out.height(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Flip the image vertically (along the y-axis).
///
/// @param img The input image to be flipped.
/// 
/// @return The flipped image.
///////////////////////////////////////////////////////////////////////////////
inline image flip_y(const image& img)
{
    image out = img;
    pixel::transform::flip_y(out.data(), out.width(), out.height(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// rotate
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate the image 90 degrees clockwise.
///
/// @param img The input image to be rotated.
/// 
/// @return The rotated image.
///////////////////////////////////////////////////////////////////////////////
inline image rotate_90_cw(const image& img)
{
    image out(img.height(), img.width(), img.format());
    pixel::transform::rotate_90_cw(img.data(), img.width(), img.height(), out.data(), img.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate the image 90 degrees counter-clockwise.
///
/// @param img The input image to be rotated.
/// 
/// @return The rotated image.
///////////////////////////////////////////////////////////////////////////////
inline image rotate_90_ccw(const image& img)
{
    image out(img.height(), img.width(), img.format());
    pixel::transform::rotate_90_ccw(img.data(), img.width(), img.height(), out.data(), img.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate the image 180 degrees.
///
/// @param img The input image to be rotated.
/// 
/// @return The rotated image.
///////////////////////////////////////////////////////////////////////////////
inline image rotate_180(const image& img)
{
    image out = img;
    pixel::transform::rotate_180(out.data(), out.width(), out.height(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// crop
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Crop a region of interest from the image.
///
/// @param img The input image from which to crop.
/// @param area The rectangular area to be cropped.
/// 
/// @return The cropped image.
///////////////////////////////////////////////////////////////////////////////
inline image crop(const image& img, const math::recti& area)
{
    const math::recti cropped = img.get_rect().crop(area);
    image out(cropped.size.x, cropped.size.y, img.format());

    pixel::transform::crop(
        img.data(), img.width(), img.height(), out.data(), img.pixel_size(),
        cropped.position.x, cropped.position.y, cropped.size.x, cropped.size.y
    );

    return out;
}

///////////////////////////////////////////////////////////////////////////////
// resize
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Resize the image to a specified size using a specified filter.
///
/// @param img The input image to be resized.
/// @param size The desired width and height of the resized image.
/// @param filter The interpolation filter to use for resizing.
/// 
/// @return The resized image.
///////////////////////////////////////////////////////////////////////////////
inline image resize(const image& img, const math::vec2i& size, pixel::filter_mode filter)
{
    image out(size.x, size.y, img.format());

    switch (filter)
    {
        case pixel::filter_mode::NEAREST:
        {
            pixel::filter::filter_nearest(
                img.data(), img.width(), img.height(),
                out.data(), out.width(), out.height(),
                img.channels()
            );
            break;
        }
        case pixel::filter_mode::LINEAR:
        {
            pixel::filter::filter_bilinear<image::channel_type>(
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

///////////////////////////////////////////////////////////////////////////////
/// @brief Resize the image to a specified size, automatically selecting the
/// interpolation filter based on pixel area change.
///
/// If the resized image has fewer pixels than the original image, nearest
/// neighbor interpolation is used.
/// 
/// If the resized image has more pixels than the original image, bilinear
/// interpolation is used.
///
/// @param img The input image to be resized.
/// @param size The desired width and height of the resized image.
/// 
/// @return The resized image.
///////////////////////////////////////////////////////////////////////////////
inline image resize(const image& img, const math::vec2i& size)
{
    const float pixel_area = (
        (static_cast<float>(size.x) / static_cast<float>(img.width())) *
        (static_cast<float>(size.y) / static_cast<float>(img.height()))
        );

    const pixel::filter_mode filter = (pixel_area < 1.0f) ? pixel::filter_mode::NEAREST : pixel::filter_mode::LINEAR;
    return resize(img, size, filter);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Resize the canvas of the image.
///
/// @param img The input image to be resized.
/// @param size The desired width and height of the canvas.
/// @param offset The offset of the image within the resized canvas.
/// @param fill The color of the canvas (default is black).
/// 
/// @return The resized canvas and image.
///////////////////////////////////////////////////////////////////////////////
inline image resize_canvas(const image& img, const math::vec2i& size, const math::vec2i& offset, const math::color& fill = math::color(0))
{
    image out(size.x, size.y, img.format());

    math::recti src_area = img.get_rect().move(offset.x, offset.y);

    for (size_t y = 0; y < out.height(); ++y)
    {
        for (size_t x = 0; x < out.width(); ++x)
        {
            if (math::contains(src_area, math::vec2i(x, y)))
            {
                out.set_pixel(x, y, img.get_pixel(x - offset.x, y - offset.y));
            }
            else
            {
                out.set_pixel(x, y, fill);
            }
        }
    }

    return out;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Resize the image to the next power of 2.
///
/// @param img The input image to be resized.
/// @param square If true, the width and height of the output image will be
/// the same.
/// @param filter The interpolation filter to use when resizing (default is
/// linear).
/// 
/// @return The resized image.
///////////////////////////////////////////////////////////////////////////////
inline image resize_pow2(const image& img, bool square = false, pixel::filter_mode filter = pixel::filter_mode::LINEAR)
{
    size_t w = math::next_pow2(img.width());
    size_t h = math::next_pow2(img.height());

    if (square)
    {
        w = h = math::max(w, h);
    }

    return resize(img, math::vec2i(w, h), filter);
}

}
}
}