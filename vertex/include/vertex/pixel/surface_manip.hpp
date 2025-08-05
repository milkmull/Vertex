#pragma once

#include "vertex/pixel/surface.hpp"

#include "vertex/math/color/functions/color.hpp"
#include "vertex/math/color/functions/common.hpp"
#include "vertex/math/color/functions/comparison.hpp"

namespace vx {
namespace pixel {
namespace manip {

template <pixel_format F>
inline math::recti get_alpha_rect(const surface<F>& surf, float threshold = 0.0f)
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

template <pixel_format F>
inline void fill(surface<F>& surf, const math::color& color)
{
    surf.fill(color);
}

template <pixel_format F>
inline void invert_color(surface<F>& surf)
{
    for (auto& px : surf)
    {
        px = math::invert(px.color());
    }
}

template <pixel_format F>
inline void grayscale(surface<F>& surf)
{
    for (auto& px : surf)
    {
        px = math::grayscale(px.color());
    }
}

template <pixel_format F>
inline void replace_color(
    surface<F>& surf,
    const math::color& color1, const math::color& color2,
    const float epsilon = math::constants<float>::epsilon
)
{
    for (auto& px : surf)
    {
        if (math::equal_approx(px.color(), color1, epsilon))
        {
            px = color2;
        }
    }
}

template <pixel_format F>
inline void swap_color(
    surface<F>& surf,
    const math::color& color1, const math::color& color2,
    const float epsilon = math::constants<float>::epsilon
)
{
    for (auto& px : surf)
    {
        if (math::equal_approx(px.color(), color1, epsilon))
        {
            px = color2;
        }
        else if (math::equal_approx(px.color(), color2, epsilon))
        {
            px = color1;
        }
    }
}

template <pixel_format F>
inline void key_color(surface<F>& surf, const math::color& color, const float epsilon = math::constants<float>::epsilon)
{
    for (auto& px : surf)
    {
        math::color c = px.color();
        if (math::rgb_equal_approx(c, color, epsilon))
        {
            c.a = 0;
            px = c;
        }
    }
}

template <pixel_format F>
inline void premultiply_alpha(surface<F>& surf)
{
    for (auto& px : surf)
    {
        math::color c = px.color();
        
        c.r *= c.a;
        c.g *= c.a;
        c.b *= c.a;

        px = c;
    }
}

} // namespace manip
} // namespace pixel
} // namespace vx