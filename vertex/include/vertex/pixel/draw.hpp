#pragma once

#include "vertex/pixel/surface.hpp"

namespace vx {
namespace pixel {
namespace draw {

///////////////////////////////////////////////////////////////////////////////
// draw pixel
///////////////////////////////////////////////////////////////////////////////

template <pixel_format F>
inline void draw_pixel(surface<F>& surf, size_t x, size_t y, const math::color& c)
{
    surf.set_pixel(x, y, c);
}

///////////////////////////////////////////////////////////////////////////////
// rect
///////////////////////////////////////////////////////////////////////////////

template <pixel_format F>
inline void draw_rect(surface<F>& surf, const math::recti& r, const math::color& c)
{
    const math::recti surf_rect = surf.get_rect();
    const math::recti cropped = math::g2::crop(surf_rect, r);

    if (cropped.empty())
    {
        return;
    }

    for (size_t y = cropped.position.y; y != cropped.bottom(); ++y)
    {
        for (size_t x = cropped.position.x; x != cropped.right(); ++x)
        {
            surf.set_pixel(x, y, c);
        }
    }
}

} // namespace draw
} // namespace pixel
} // namespace vx