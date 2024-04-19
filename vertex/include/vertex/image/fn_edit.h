#pragma once

#include "image.h"
#include "raw/transform.h"

#include "enum_image_filter.h"
#include "vertex/math/sample/filter/filter_nearest.h"
#include "vertex/math/sample/filter/filter_bilinear.h"

#include "vertex/math/math/fn/fn_exponential.h"
#include "vertex/math/geometry/fn/rect_fn_collision.h"

#include "vertex/math/color/fn/color_fn_common.h"
#include "vertex/math/color/fn/color_fn_comparison.h"

namespace vx {
namespace img {

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
    raw::copy(out.data(), out.width(), out.height(), out.data(), out.channels());
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
    raw::flip_x(out.data(), out.width(), out.height(), out.channels());
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
    raw::flip_y(out.data(), out.width(), out.height(), out.channels());
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
    raw::rotate_90_cw(img.data(), img.width(), img.height(), out.data(), img.channels());
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
    raw::rotate_90_ccw(img.data(), img.width(), img.height(), out.data(), img.channels());
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
    raw::rotate_180(out.data(), out.width(), out.height(), out.channels());
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

    const std::array<int32_t, 4> r{ cropped.position.x, cropped.position.y, cropped.size.x, cropped.size.y };
    raw::crop(img.data(), img.width(), img.height(), out.data(), img.channels(), r);

    return out;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Get a rect enclosing the smallest opaque area in the image.
///
/// @param img The input image.
/// @param threshold The minimum alpha value to crop, in the range [0, 1]
/// (default is 0).
/// 
/// @return A rect enclosing the opaque area.
///////////////////////////////////////////////////////////////////////////////
inline math::recti get_alpha_rect(const image& img, float threshold = 0.0f)
{
    size_t minx = img.width();
    size_t maxx = 0;
    size_t miny = img.height();
    size_t maxy = 0;

    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            math::color c = img.get_pixel(x, y);
            if (c.a > threshold)
            {
                if (x < minx) minx = x;
                if (x > maxx) maxx = x;
                if (y < miny) miny = y;
                if (y > maxy) maxy = y;
            }

        }
    }

    return math::recti(
        minx,
        miny,
        (maxx + 1) - minx,
        (maxy + 1) - miny
    );
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Remove alpha borders from an image.
///
/// @param img The input image from which to crop.
/// @param threshold The minimum alpha value to crop, in the range [0, 1]
/// (default is 0).
/// 
/// @return The cropped image.
///////////////////////////////////////////////////////////////////////////////
inline image crop_alpha(const image& img, float threshold = 0.0f)
{
    return crop(img, get_alpha_rect(img, threshold));
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
inline image resize(const image& img, const math::vec2i& size, image_filter filter)
{
    image out(size.x, size.y, img.format());

    switch (filter)
    {
        case image_filter::NEAREST:
        {
            math::filter_nearest(
                img.data(), img.width(), img.height(),
                out.data(), out.width(), out.height(),
                img.channels()
            );
            break;
        }
        case image_filter::LINEAR:
        {
            math::filter_bilinear(
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

    const image_filter filter = (pixel_area < 1.0f) ? image_filter::NEAREST : image_filter::LINEAR;
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
inline image resize_pow2(const image& img, bool square = false, image_filter filter = image_filter::LINEAR)
{
    size_t w = math::next_pow2(img.width());
    size_t h = math::next_pow2(img.height());

    if (square)
    {
        w = h = math::max(w, h);
    }

    return resize(img, math::vec2i(w, h), filter);
}

///////////////////////////////////////////////////////////////////////////////
// color adjust
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Fill the entire image with the specified color.
///
/// @param img The image to be filled.
/// @param color The color to fill the image with.
///////////////////////////////////////////////////////////////////////////////
inline void fill(image& img, const math::color& color)
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            img.set_pixel(x, y, color);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Invert the colors of the entire image.
///
/// @param img The image to be inverted.
///////////////////////////////////////////////////////////////////////////////
inline void invert_color(image& img)
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            img.set_pixel(x, y, math::invert(img.get_pixel(x, y)));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Replace all occurrences of one color in the image with another color.
///
/// @param img The image to be modified.
/// @param color1 The color to be replaced.
/// @param color2 The color to replace with.
/// @param epsilon The epsilon for color comparison.
///////////////////////////////////////////////////////////////////////////////
inline void replace_color(image& img, const math::color& color1, const math::color& color2, float epsilon = math::epsilon<float>())
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            if (math::is_equal_approx(img.get_pixel(x, y), color1, epsilon))
            {
                img.set_pixel(x, y, color2);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Swap all occurrences of two colors in the image.
///
/// @param img The image to be modified.
/// @param color1 The first color to be swapped.
/// @param color2 The second color to be swapped.
/// @param epsilon The epsilon for color comparison.
///////////////////////////////////////////////////////////////////////////////
inline void swap_color(image& img, const math::color& color1, const math::color& color2, float epsilon = math::epsilon<float>())
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            if (math::is_equal_approx(img.get_pixel(x, y), color1, epsilon))
            {
                img.set_pixel(x, y, color2);
            }
            else if (math::is_equal_approx(img.get_pixel(x, y), color2, epsilon))
            {
                img.set_pixel(x, y, color1);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Set the alpha channel to zero for all pixels in the image that match
/// the specified color.
///
/// @param img The image to be modified.
/// @param color The color to be keyed out.
/// @param epsilon The epsilon for color comparison.
///////////////////////////////////////////////////////////////////////////////
inline void key_color(image& img, const math::color& color, float epsilon = math::epsilon<float>())
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            math::color c = img.get_pixel(x, y);
            if (math::is_equal_approx(c.rgb(), color.rgb(), epsilon))
            {
                c.a = 0.0f;
                img.set_pixel(x, y, c);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Multiply the color components of the image with their alpha channel.
///
/// @param img The image to be modified.
///////////////////////////////////////////////////////////////////////////////
inline void premultiply_alpha(image& img)
{
    for (size_t y = 0; y < img.height(); ++y)
    {
        for (size_t x = 0; x < img.width(); ++x)
        {
            math::color c = img.get_pixel(x, y);
            c.r *= c.a;
            c.g *= c.a;
            c.b *= c.a;
            img.set_pixel(x, y, c);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// blit
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Blit a portion of the source image onto the destination image at
/// the specified position.
///
/// @param src The source image.
/// @param src_area The rectangular area within the source image to be copied.
/// @param dst The destination image.
/// @param dst_position The position within the destination image where the
/// source image will be copied.
///////////////////////////////////////////////////////////////////////////////
inline void blit(
    const image& src, const math::recti& src_area,
    image& dst, const math::vec2i& dst_position
)
{
    // Crop the area within the bounds of the src image.

    math::recti area = src.get_rect().crop(src_area);
    if (area.empty())
    {
        return;
    }

    // Calculate the shift needed to transform a point in the dst area
    // to a point in the src area.

    const math::vec2i shift = dst_position - src_area.position;

    // Translate the src coordinate system to the dst coordinate system by
    // shifting the src area to allign with the dst position. The dst area is
    // then cropped within the dst rect. This gives us the minimum area that
    // we have to copy from src to dst.

    area = dst.get_rect().crop(area.move(shift.x, shift.y));
    if (area.empty())
    {
        return;
    }

    for (size_t y = area.position.y; y < static_cast<size_t>(area.bottom()); ++y)
    {
        for (size_t x = area.position.x; x < static_cast<size_t>(area.right()); ++x)
        {
            dst.set_pixel(x, y, src.get_pixel(x - shift.x, y - shift.y));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Blit the entire source image onto the destination image at the
/// specified position.
///
/// @param src The source image.
/// @param dst The destination image.
/// @param dst_position The position within the destination image where the
/// source image will be copied.
///////////////////////////////////////////////////////////////////////////////
inline void blit(const image& src, image& dst, const math::vec2i& dst_position)
{
    blit(src, src.get_rect(), dst, dst_position);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Blit a portion of the source image onto the destination image at
/// the specified position with blending.
///
/// @tparam blend_func The blending function type.
/// @param src The source image.
/// @param src_area The rectangular area within the source image to be copied.
/// @param dst The destination image.
/// @param dst_position The position within the destination image where the
/// source image will be copied.
/// @param blend The blending function to be applied when copying pixels.
///////////////////////////////////////////////////////////////////////////////
template <typename blend_func>
inline void blit(
    const image& src, const math::recti& src_area,
    image& dst, const math::vec2i& dst_position,
    const blend_func& blend
)
{
    // Crop the area within the bounds of the src image.

    math::recti area = src.get_rect().crop(src_area);
    if (area.empty())
    {
        return;
    }

    // Calculate the shift needed to transform a point in the dst area
    // to a point in the src area.

    const math::vec2i shift = dst_position - src_area.position;

    // Translate the src coordinate system to the dst coordinate system by
    // shifting the src area to allign with the dst position. The dst area is
    // then cropped within the dst rect. This gives us the minimum area that
    // we have to copy from src to dst.

    area = dst.get_rect().crop(area.move(shift));
    if (area.empty())
    {
        return;
    }

    for (size_t y = area.position.y; y < static_cast<size_t>(area.bottom()); ++y)
    {
        for (size_t x = area.position.x; x < static_cast<size_t>(area.right()); ++x)
        {
            dst.set_pixel(
                x, y,
                blend.blend(
                    src.get_pixel(x - shift.x, y - shift.y),
                    dst.get_pixel(x, y)
                )
            );
        }
    }
}

}
}