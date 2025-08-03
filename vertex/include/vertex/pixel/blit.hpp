#pragma once

#include "vertex/pixel/surface.hpp"
#include "vertex/math/geometry/2d/functions/collision.hpp"

namespace vx {
namespace pixel {

template <pixel_format SRC_FMT, pixel_format DST_FMT>
inline bool blit(
    const surface<SRC_FMT>& src, const math::recti& src_area,
    surface<DST_FMT>& dst, const math::vec2i& dst_position
)
{
    if (src.unknown_format() || dst.unknown_format())
    {
        return false;
    }

    // Crop the area within the bounds of the src surface.
    math::recti area = math::geometry2d::crop(src.get_rect(), src_area);
    if (area.empty())
    {
        return true;
    }

    // Calculate the shift needed to transform a point
    // in the dst area to a point in the src area.
    const math::vec2i shift = dst_position - src_area.position;

    // Translate the src coordinate system to the dst coordinate system by
    // shifting the src area to allign with the dst position. The dst area is
    // then cropped within the dst rect. This gives us the minimum area that
    // we have to copy from src to dst.
    area = math::geometry2d::crop(dst.get_rect(), math::geometry2d::move(area, shift.x, shift.y));
    if (area.empty())
    {
        return true;
    }

    for (size_t y = area.position.y; y < static_cast<size_t>(area.bottom()); ++y)
    {
        for (size_t x = area.position.x; x < static_cast<size_t>(area.right()); ++x)
        {
            dst.set_pixel(x, y, src.get_pixel(x - shift.x, y - shift.y));
        }
    }

    return true;
}

template <pixel_format SRC_FMT, pixel_format DST_FMT>
inline bool blit(const surface<SRC_FMT>& src, surface<DST_FMT>& dst, const math::vec2i& dst_position)
{
    return blit(src, src.get_rect(), dst, dst_position);
}

template <pixel_format SRC_FMT, pixel_format DST_FMT, typename blend_func>
inline bool blit(
    const surface<SRC_FMT>& src, const math::recti& src_area,
    surface<DST_FMT>& dst, const math::vec2i& dst_position,
    const blend_func& blend
)
{
    if (src.unknown_format() || dst.unknown_format())
    {
        return false;
    }

    // Crop the area within the bounds of the src surface.
    math::recti area = math::geometry2d::crop(src.get_rect(), src_area);
    if (area.empty())
    {
        return true;
    }

    // Calculate the shift needed to transform a point
    // in the dst area to a point in the src area.
    const math::vec2i shift = dst_position - src_area.position;

    // Translate the src coordinate system to the dst coordinate system by
    // shifting the src area to allign with the dst position. The dst area is
    // then cropped within the dst rect. This gives us the minimum area that
    // we have to copy from src to dst.
    area = math::geometry2d::crop(dst.get_rect(), math::geometry2d::move(area, shift.x, shift.y));
    if (area.empty())
    {
        return true;
    }

    for (size_t y = area.position.y; y < static_cast<size_t>(area.bottom()); ++y)
    {
        for (size_t x = area.position.x; x < static_cast<size_t>(area.right()); ++x)
        {
            dst.set_pixel(
                x, y,
                blend(src.get_pixel(x - shift.x, y - shift.y), dst.get_pixel(x, y))
            );
        }
    }

    return true;
}

} // namespace pixel
} // namespace vx