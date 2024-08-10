#pragma once

#include "surface.h"

#include "vertex/pixel/raw_transform.h"
#include "filter/filter_nearest.h"
#include "filter/filter_bilinear.h"

#include "vertex/math/math/fn/fn_exponential.h"

namespace vx {
namespace pixel {
namespace transform {

///////////////////////////////////////////////////////////////////////////////
// copy
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Make a copy of the surface.
///
/// @param surf The input surface to be coppied.
/// 
/// @return The coppied surface.
///////////////////////////////////////////////////////////////////////////////
inline surface copy(const surface& surf)
{
    surface out = surf;
    copy_raw(out.data(), out.width(), out.height(), out.data(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// flip
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Flip the surface horizontally (along the x-axis).
///
/// @param surf The input surface to be flipped.
/// 
/// @return The flipped surface.
///////////////////////////////////////////////////////////////////////////////
inline surface flip_x(const surface& surf)
{
    surface out = surf;
    flip_x_raw(out.data(), out.width(), out.height(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Flip the surface vertically (along the y-axis).
///
/// @param surf The input surface to be flipped.
/// 
/// @return The flipped surface.
///////////////////////////////////////////////////////////////////////////////
inline surface flip_y(const surface& surf)
{
    surface out = surf;
    flip_y_raw(out.data(), out.width(), out.height(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// rotate
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate the surface 90 degrees clockwise.
///
/// @param surf The input surface to be rotated.
/// 
/// @return The rotated surface.
///////////////////////////////////////////////////////////////////////////////
inline surface rotate_90_cw(const surface& surf)
{
    surface out(surf.height(), surf.width(), surf.format());
    rotate_90_cw_raw(surf.data(), surf.width(), surf.height(), out.data(), surf.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate the surface 90 degrees counter-clockwise.
///
/// @param surf The input surface to be rotated.
/// 
/// @return The rotated surface.
///////////////////////////////////////////////////////////////////////////////
inline surface rotate_90_ccw(const surface& surf)
{
    surface out(surf.height(), surf.width(), surf.format());
    rotate_90_ccw_raw(surf.data(), surf.width(), surf.height(), out.data(), surf.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotate the surface 180 degrees.
///
/// @param surf The input surface to be rotated.
/// 
/// @return The rotated surface.
///////////////////////////////////////////////////////////////////////////////
inline surface rotate_180(const surface& surf)
{
    surface out = surf;
    rotate_180_raw(out.data(), out.width(), out.height(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// crop
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Crop a region of interest from the surface.
///
/// @param surf The input surface from which to crop.
/// @param area The rectangular area to be cropped.
/// 
/// @return The cropped surface.
///////////////////////////////////////////////////////////////////////////////
inline surface crop(const surface& surf, const math::recti& area)
{
    const math::recti cropped = surf.get_rect().crop(area);
    surface out(cropped.size.x, cropped.size.y, surf.format());

    crop_raw(
        surf.data(), surf.width(), surf.height(), out.data(), surf.pixel_size(),
        cropped.position.x, cropped.position.y, cropped.size.x, cropped.size.y
    );

    return out;
}

///////////////////////////////////////////////////////////////////////////////
// resize
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Resize the surface to a specified size using a specified filter.
///
/// @param surf The input surface to be resized.
/// @param size The desired width and height of the resized surface.
/// @param filter The interpolation filter to use for resizing.
/// 
/// @return The resized surface.
///////////////////////////////////////////////////////////////////////////////
inline surface resize(const surface& surf, const math::vec2i& size, filter_mode filter)
{
    surface out(size.x, size.y, surf.format());

    switch (filter)
    {
        case filter_mode::NEAREST:
        {
            filter::filter_nearest(
                surf.data(), surf.width(), surf.height(),
                out.data(), out.width(), out.height(),
                surf.channels()
            );
            break;
        }
        case filter_mode::LINEAR:
        {
            uint32_t masks[4]{};
            uint32_t shifts[4]{};

            if (is_packed_format(surf.format()))
            {
                channel_info info = get_channel_info(surf.format());

                masks[info.r.index] = info.r.mask;
                masks[info.g.index] = info.g.mask;
                masks[info.b.index] = info.b.mask;
                masks[info.a.index] = info.a.mask;

                shifts[info.r.index] = info.r.shift;
                shifts[info.g.index] = info.g.shift;
                shifts[info.b.index] = info.b.shift;
                shifts[info.a.index] = info.a.shift;
            }

            switch (surf.format())
            {
                case pixel_format::RGB_332:
                {
                    filter::filter_bilinear<uint8_t>(
                        surf.data(), surf.width(), surf.height(),
                        out.data(), out.width(), out.height(),
                        surf.channels(), masks, shifts
                    );
                    break;
                }
                case pixel_format::RGBA_4444:
                case pixel_format::BGRA_4444:
                case pixel_format::RGB_565:
                case pixel_format::BGR_565:
                case pixel_format::RGBA_5551:
                case pixel_format::BGRA_5551:
                case pixel_format::ARGB_1555:
                {
                    filter::filter_bilinear<uint16_t>(
                        surf.data(), surf.width(), surf.height(),
                        out.data(), out.width(), out.height(),
                        surf.channels(), masks, shifts
                    );
                    break;
                }
                case pixel_format::RGBA_8888:
                case pixel_format::BGRA_8888:
                case pixel_format::ABGR_8888:
                case pixel_format::ARGB_2101010:
                case pixel_format::ABGR_2101010:
                {
                    filter::filter_bilinear<uint32_t>(
                        surf.data(), surf.width(), surf.height(),
                        out.data(), out.width(), out.height(),
                        surf.channels(), masks, shifts
                    );
                    break;
                }
                case pixel_format::R_8:
                case pixel_format::RG_8:
                case pixel_format::RGB_8:
                case pixel_format::BGR_8:
                case pixel_format::RGBA_8:
                case pixel_format::BGRA_8:
                case pixel_format::ABGR_8:
                {
                    filter::filter_bilinear<uint8_t>(
                        surf.data(), surf.width(), surf.height(),
                        out.data(), out.width(), out.height(),
                        surf.channels()
                    );
                    break;
                }
                case pixel_format::R_16:
                case pixel_format::RG_16:
                case pixel_format::RGB_16:
                case pixel_format::RGBA_16:
                {
                    filter::filter_bilinear<uint16_t>(
                        surf.data(), surf.width(), surf.height(),
                        out.data(), out.width(), out.height(),
                        surf.channels()
                    );
                    break;
                }
                case pixel_format::R_32F:
                case pixel_format::RG_32F:
                case pixel_format::RGB_32F:
                case pixel_format::RGBA_32F:
                {
                    filter::filter_bilinear<float>(
                        surf.data(), surf.width(), surf.height(),
                        out.data(), out.width(), out.height(),
                        surf.channels()
                    );
                    break;
                }
                default:
                {
                    break;
                }
            }

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
/// @brief Resize the surface to a specified size, automatically selecting the
/// interpolation filter based on pixel area change.
///
/// If the resized surface has fewer pixels than the original surface, nearest
/// neighbor interpolation is used.
/// 
/// If the resized surface has more pixels than the original surface, bilinear
/// interpolation is used.
///
/// @param surf The input surface to be resized.
/// @param size The desired width and height of the resized surface.
/// 
/// @return The resized surface.
///////////////////////////////////////////////////////////////////////////////
inline surface resize(const surface& surf, const math::vec2i& size)
{
    const float pixel_area = (
        (static_cast<float>(size.x) / static_cast<float>(surf.width())) *
        (static_cast<float>(size.y) / static_cast<float>(surf.height()))
    );

    const filter_mode filter = (pixel_area < 1.0f) ? filter_mode::NEAREST : filter_mode::LINEAR;
    return resize(surf, size, filter);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Resize the canvas of the surface.
///
/// @param surf The input surface to be resized.
/// @param size The desired width and height of the canvas.
/// @param offset The offset of the surface within the resized canvas.
/// @param fill The color of the canvas (default is black).
/// 
/// @return The resized canvas and surface.
///////////////////////////////////////////////////////////////////////////////
inline surface resize_canvas(const surface& surf, const math::vec2i& size, const math::vec2i& offset, const math::color& fill = math::color(0))
{
    surface out(size.x, size.y, surf.format());

    math::recti src_area = surf.get_rect().move(offset.x, offset.y);

    for (size_t y = 0; y < out.height(); ++y)
    {
        for (size_t x = 0; x < out.width(); ++x)
        {
            if (math::contains(src_area, math::vec2i(x, y)))
            {
                out.set_pixel(x, y, surf.get_pixel(x - offset.x, y - offset.y));
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
/// @brief Resize the surface to the next power of 2.
///
/// @param surf The input surface to be resized.
/// @param square If true, the width and height of the output surface will be
/// the same.
/// @param filter The interpolation filter to use when resizing (default is
/// linear).
/// 
/// @return The resized surface.
///////////////////////////////////////////////////////////////////////////////
inline surface resize_pow2(const surface& surf, bool square = false, filter_mode filter = filter_mode::LINEAR)
{
    size_t w = math::next_pow2(surf.width());
    size_t h = math::next_pow2(surf.height());

    if (square)
    {
        w = h = math::max(w, h);
    }

    return resize(surf, math::vec2i(w, h), filter);
}

}
}
}