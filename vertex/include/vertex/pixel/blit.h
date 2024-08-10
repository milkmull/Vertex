#pragma once

#include "surface.h"

namespace vx {
namespace pixel {

///////////////////////////////////////////////////////////////////////////////
/// @brief Copies a specified rectangular area from the source surface to the
/// destination surface at a given position.
///
/// @param src The source surface to copy pixels from.
/// @param src_area The rectangular area within the source surface to copy.
/// @param dst The destination surface to copy pixels to.
/// @param dst_position The position on the destination surface where the
/// top-left corner of the src_area should be copied.
/// 
/// @return true if the operation succeeds, false if either surface has 
/// an unknown format.
///////////////////////////////////////////////////////////////////////////////
inline bool blit(
    const surface& src, const math::recti& src_area,
    surface& dst, const math::vec2i& dst_position
)
{
    if (src.unknown_format() || dst.unknown_format())
    {
        return false;
    }

    // Crop the area within the bounds of the src surface.
    math::recti area = src.get_rect().crop(src_area);
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
    area = dst.get_rect().crop(area.move(shift.x, shift.y));
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

inline bool blit(const surface& src, surface& dst, const math::vec2i& dst_position)
{
    return blit(src, src.get_rect(), dst, dst_position);
}


///////////////////////////////////////////////////////////////////////////////
/// @brief Copies a specified rectangular area from the source surface to the 
/// destination surface using a custom blending function.
///
/// @tparam blend_func A functor or lambda that takes two colors (source and 
/// destination) and returns a blended color.
/// @param src The source surface to copy pixels from.
/// @param src_area The rectangular area within the source surface to copy.
/// @param dst The destination surface to copy pixels to.
/// @param dst_position The position on the destination surface where the top-left 
/// corner of the src_area should be copied.
/// @param blend A blending function or functor that defines how to combine 
/// source and destination pixels.
/// 
/// @return true if the operation succeeds, false if either surface has 
/// an unknown format.
///////////////////////////////////////////////////////////////////////////////
template <typename blend_func>
inline bool blit(
    const surface& src, const math::recti& src_area,
    surface& dst, const math::vec2i& dst_position,
    const blend_func& blend
)
{
    if (src.unknown_format() || dst.unknown_format())
    {
        return false;
    }

    // Crop the area within the bounds of the src surface.
    math::recti area = src.get_rect().crop(src_area);
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
    area = dst.get_rect().crop(area.move(shift.x, shift.y));
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

}
}