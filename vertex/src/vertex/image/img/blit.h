#pragma once

#include "image.h"
#include "vertex/math/color/blend.h"

namespace vx {
namespace img {

inline constexpr void blit(
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

    area = dst.get_rect().crop(area.move(shift));
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

inline constexpr void blit(const image& src, image& dst, const math::vec2i& dst_position)
{
    blit(src, src.get_rect(), dst, dst_position);
}

template <typename blend_func>
inline constexpr void blit(
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