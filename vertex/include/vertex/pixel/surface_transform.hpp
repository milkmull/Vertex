#pragma once

#include "vertex/pixel/surface.hpp"

#include "vertex/pixel/raw_transform.hpp"
#include "vertex/pixel/filter/filter_nearest.hpp"
#include "vertex/pixel/filter/filter_bilinear.hpp"

#include "vertex/math/core/functions/exponential.hpp"
#include "vertex/math/geometry/2d/functions/collision.hpp"

namespace vx {
namespace pixel {
namespace transform {

///////////////////////////////////////////////////////////////////////////////
// copy
///////////////////////////////////////////////////////////////////////////////

template <pixel_format F>
inline surface<F> copy(const surface<F>& surf)
{
    surface<F> out = surf;
    copy_raw(out.data(), out.width(), out.height(), out.data(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// flip
///////////////////////////////////////////////////////////////////////////////

template <pixel_format F>
inline surface<F> flip_x(const surface<F>& surf)
{
    surface<F> out = surf;
    flip_x_raw(out.data(), out.width(), out.height(), out.pixel_size());
    return out;
}

template <pixel_format F>
inline surface<F> flip_y(const surface<F>& surf)
{
    surface<F> out = surf;
    flip_y_raw(out.data(), out.width(), out.height(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// rotate
///////////////////////////////////////////////////////////////////////////////

template <pixel_format F>
inline surface<F> rotate_90_cw(const surface<F>& surf)
{
    surface<F> out(surf.height(), surf.width());
    rotate_90_cw_raw(surf.data(), surf.width(), surf.height(), out.data(), surf.pixel_size());
    return out;
}

template <pixel_format F>
inline surface<F> rotate_90_ccw(const surface<F>& surf)
{
    surface<F> out(surf.height(), surf.width());
    rotate_90_ccw_raw(surf.data(), surf.width(), surf.height(), out.data(), surf.pixel_size());
    return out;
}

template <pixel_format F>
inline surface<F> rotate_180(const surface<F>& surf)
{
    surface<F> out = surf;
    rotate_180_raw(out.data(), out.width(), out.height(), out.pixel_size());
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// crop
///////////////////////////////////////////////////////////////////////////////

template <pixel_format F>
inline surface<F> crop(const surface<F>& surf, const math::recti& area)
{
    const math::recti cropped = math::g2::crop(surf.get_rect(), area);
    surface<F> out(cropped.size.x, cropped.size.y);

    crop_raw(
        surf.data(), surf.width(), surf.height(), out.data(), surf.pixel_size(),
        cropped.position.x, cropped.position.y, cropped.size.x, cropped.size.y
    );

    return out;
}

///////////////////////////////////////////////////////////////////////////////
// resize
///////////////////////////////////////////////////////////////////////////////

template <pixel_format F>
inline surface<F> resize(const surface<F>& surf, const math::vec2i& size, filter_mode filter)
{
    surface<F> out(size.x, size.y);

    switch (filter)
    {
        case filter_mode::NEAREST:
        {
            filter_nearest(
                surf.data(), surf.width(), surf.height(),
                out.data(), out.width(), out.height(),
                surf.pixel_size()
            );
            break;
        }
        case filter_mode::LINEAR:
        {
            VX_IF_CONSTEXPR(is_packed_format(F))
            {
                constexpr const auto masks = build_mask_array(F);
                constexpr const auto shifts = build_shift_array(F);

                using pixel_type = typename surface<F>::pixel_type;
                filter_bilinear<pixel_type>(
                    surf.data(), surf.width(), surf.height(),
                    out.data(), out.width(), out.height(),
                    surf.channels(), masks, shifts
                );
            }
            else
            {
                using channel_type = typename surface<F>::channel_type;
                filter_bilinear<channel_type>(
                    surf.data(), surf.width(), surf.height(),
                    out.data(), out.width(), out.height(),
                    surf.channels()
                );
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

template <pixel_format F>
inline surface<F> resize(const surface<F>& surf, const math::vec2i& size)
{
    const float pixel_area = (
        (static_cast<float>(size.x) / static_cast<float>(surf.width())) *
        (static_cast<float>(size.y) / static_cast<float>(surf.height()))
    );

    const filter_mode filter = (pixel_area < 1.0f) ? filter_mode::NEAREST : filter_mode::LINEAR;
    return resize(surf, size, filter);
}

template <pixel_format F>
inline surface<F> resize_canvas(const surface<F>& surf, const math::vec2i& size, const math::vec2i& offset, const math::color& fill = math::color(0))
{
    surface<F> out(size.x, size.y);

    const math::recti src_area = math::g2::move(surf.get_rect(), offset.x, offset.y);

    for (size_t y = 0; y < out.height(); ++y)
    {
        for (size_t x = 0; x < out.width(); ++x)
        {
            if (math::g2::contains(src_area, math::vec2i(x, y)))
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

template <pixel_format F>
inline surface<F> resize_pow2(const surface<F>& surf, bool square = false, filter_mode filter = filter_mode::LINEAR)
{
    size_t w = math::next_pow2(surf.width());
    size_t h = math::next_pow2(surf.height());

    if (square)
    {
        w = h = math::max(w, h);
    }

    return resize(surf, math::vec2i(w, h), filter);
}

} // namespace transform
} // namespace pixel
} // namespace vx