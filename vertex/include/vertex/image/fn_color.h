#pragma once

#include "image.h"

#include "vertex/math/color/fn/color_fn_common.h"
#include "vertex/math/color/fn/color_fn_comparison.h"
#include "vertex/math/math/fn/vec_fn_comparison.h"

namespace vx {
namespace img {
namespace color {

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

}
}
}