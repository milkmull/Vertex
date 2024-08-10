#pragma once

#include "surface.h"

#include "vertex/math/color/fn/color_fn_common.h"
#include "vertex/math/color/fn/color_fn_comparison.h"
#include "vertex/math/math/fn/vec_fn_comparison.h"

namespace vx {
namespace pixel {
namespace color {

///////////////////////////////////////////////////////////////////////////////
/// @brief Get a rect enclosing the smallest opaque area in the surface.
///
/// @param surf The input surface.
/// @param threshold The minimum alpha value to crop, in the range [0, 1]
/// (default is 0).
/// 
/// @return A rect enclosing the opaque area.
///////////////////////////////////////////////////////////////////////////////
inline math::recti get_alpha_rect(const surface& surf, float threshold = 0.0f)
{
    size_t minx = surf.width();
    size_t maxx = 0;
    size_t miny = surf.height();
    size_t maxy = 0;

    for (size_t y = 0; y < surf.height(); ++y)
    {
        for (size_t x = 0; x < surf.width(); ++x)
        {
            math::color c = surf.get_pixel(x, y);
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
/// @brief Fill the entire surface with the specified color.
///
/// @param surf The surface to be filled.
/// @param color The color to fill the surface with.
///////////////////////////////////////////////////////////////////////////////
inline void fill(surface& surf, const math::color& color)
{
    for (size_t y = 0; y < surf.height(); ++y)
    {
        for (size_t x = 0; x < surf.width(); ++x)
        {
            surf.set_pixel(x, y, color);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Invert the colors of the entire surface.
///
/// @param surf The surface to be inverted.
///////////////////////////////////////////////////////////////////////////////
inline void invert_color(surface& surf)
{
    for (size_t y = 0; y < surf.height(); ++y)
    {
        for (size_t x = 0; x < surf.width(); ++x)
        {
            surf.set_pixel(x, y, math::invert(surf.get_pixel(x, y)));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Replace all occurrences of one color in the surface with another color.
///
/// @param surf The surface to be modified.
/// @param color1 The color to be replaced.
/// @param color2 The color to replace with.
/// @param epsilon The epsilon for color comparison.
///////////////////////////////////////////////////////////////////////////////
inline void replace_color(surface& surf, const math::color& color1, const math::color& color2, float epsilon = math::epsilon<float>())
{
    for (size_t y = 0; y < surf.height(); ++y)
    {
        for (size_t x = 0; x < surf.width(); ++x)
        {
            if (math::is_equal_approx(surf.get_pixel(x, y), color1, epsilon))
            {
                surf.set_pixel(x, y, color2);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Swap all occurrences of two colors in the surface.
///
/// @param surf The surface to be modified.
/// @param color1 The first color to be swapped.
/// @param color2 The second color to be swapped.
/// @param epsilon The epsilon for color comparison.
///////////////////////////////////////////////////////////////////////////////
inline void swap_color(surface& surf, const math::color& color1, const math::color& color2, float epsilon = math::epsilon<float>())
{
    for (size_t y = 0; y < surf.height(); ++y)
    {
        for (size_t x = 0; x < surf.width(); ++x)
        {
            if (math::is_equal_approx(surf.get_pixel(x, y), color1, epsilon))
            {
                surf.set_pixel(x, y, color2);
            }
            else if (math::is_equal_approx(surf.get_pixel(x, y), color2, epsilon))
            {
                surf.set_pixel(x, y, color1);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Set the alpha channel to zero for all pixels in the surface that match
/// the specified color.
///
/// @param surf The surface to be modified.
/// @param color The color to be keyed out.
/// @param epsilon The epsilon for color comparison.
///////////////////////////////////////////////////////////////////////////////
inline void key_color(surface& surf, const math::color& color, float epsilon = math::epsilon<float>())
{
    for (size_t y = 0; y < surf.height(); ++y)
    {
        for (size_t x = 0; x < surf.width(); ++x)
        {
            math::color c = surf.get_pixel(x, y);
            if (math::is_equal_approx(c.rgb(), color.rgb(), epsilon))
            {
                c.a = 0.0f;
                surf.set_pixel(x, y, c);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Multiply the color components of the surface with their alpha channel.
///
/// @param surf The surface to be modified.
///////////////////////////////////////////////////////////////////////////////
inline void premultiply_alpha(surface& surf)
{
    for (size_t y = 0; y < surf.height(); ++y)
    {
        for (size_t x = 0; x < surf.width(); ++x)
        {
            math::color c = surf.get_pixel(x, y);
            c.r *= c.a;
            c.g *= c.a;
            c.b *= c.a;
            surf.set_pixel(x, y, c);
        }
    }
}

}
}
}