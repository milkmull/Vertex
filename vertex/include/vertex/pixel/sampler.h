#pragma once

#include "surface.h"
#include "vertex/math/sample/wrap.h"

namespace vx {
namespace pixel {

class sampler
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    math::vec2 resolution = math::vec2(1.0f);

    wrap_mode xwrap = wrap_mode::REPEAT;
    wrap_mode ywrap = wrap_mode::REPEAT;

    math::color border;

    filter_mode min_filter = filter_mode::NEAREST;
    filter_mode mag_filter = filter_mode::LINEAR;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // sampling
    ///////////////////////////////////////////////////////////////////////////////

    math::color sample_pixel(const surface& surf, int32_t x, int32_t y) const
    {
        return sample(
            surf,
            static_cast<float>(x) / static_cast<float>(surf.width()),
            static_cast<float>(y) / static_cast<float>(surf.height())
        );
    }

    math::color sample(const surface& surf, float u, float v) const
    {
        if (surf.empty() || surf.unknown_format())
        {
            return border;
        }

        const math::vec2i size = surf.size();

        u *= (static_cast<float>(size.x) / resolution.x);
        v *= (static_cast<float>(size.y) / resolution.y);

        const float sample_pixel_area = resolution.x * resolution.y;
        filter_mode current_filter = (sample_pixel_area < 1.0f) ? min_filter : mag_filter;

        switch (current_filter)
        {
            case filter_mode::NEAREST:  return sample_nearest(surf, size, u, v);
            case filter_mode::LINEAR:   return sample_bilinear(surf, size, u, v);
            default:                    return border;
        }
    }

private:

    math::color get_pixel(const surface& surf, const math::vec2i& size, int32_t x, int32_t y) const
    {
        x = wrap_pixel(x, static_cast<int32_t>(size.x), xwrap);
        y = wrap_pixel(y, static_cast<int32_t>(size.y), ywrap);

        return surf.get_pixel(x, y, border);
    }

    int32_t wrap_pixel(int32_t p, int32_t size, wrap_mode wrap) const
    {
        if (0 <= p && p < size)
        {
            return p;
        }

        switch (wrap)
        {
            case wrap_mode::CLAMP_TO_EDGE:         return math::wrap::clamp_to_edge(p, size);
            case wrap_mode::CLAMP_TO_BORDER:       return math::wrap::clamp_to_border(p, size);
            case wrap_mode::REPEAT:                return math::wrap::repeat(p, size);
            case wrap_mode::MIRRORED_REPEAT:       return math::wrap::mirrored_repeat(p, size);
            case wrap_mode::MIRROR_CLAMP_TO_EDGE:  return math::wrap::mirror_clamp_to_edge(p, size);
            default:                               return p;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // sample methods
    ///////////////////////////////////////////////////////////////////////////////

    math::color sample_nearest(const surface& surf, const math::vec2i& size, float u, float v) const
    {
        return get_pixel(surf, size, static_cast<int32_t>(u), static_cast<int32_t>(v));
    }

    math::color sample_bilinear(const surface& surf, const math::vec2i& size, float u, float v) const
    {
        // x
        const float srcxfrac = u - 0.5f;
        const int32_t srcx = static_cast<int32_t>(math::floor(srcxfrac));

        const float fx2 = srcxfrac - srcx;
        const float fx1 = 1.0f - fx2;

        // y
        const float srcyfrac = v - 0.5f;
        const int32_t srcy = static_cast<int32_t>(math::floor(srcyfrac));

        const float fy2 = srcyfrac - srcy;
        const float fy1 = 1.0f - fy2;

        // interpolate
        const math::color pixels[4] =
        {
            get_pixel(surf, size, srcx, srcy),
            get_pixel(surf, size, srcx + 1, srcy),
            get_pixel(surf, size, srcx, srcy + 1),
            get_pixel(surf, size, srcx + 1, srcy + 1)
        };

        const float weights[4] =
        {
            fx1 * fy1,
            fx2 * fy1,
            fx1 * fy2,
            fx2 * fy2
        };

        math::color samp;

        for (size_t c = 0; c < samp.size(); ++c)
        {
            samp[c] = (
                pixels[0][c] * weights[0] +
                pixels[1][c] * weights[1] +
                pixels[2][c] * weights[2] +
                pixels[3][c] * weights[3]
            );
        }

        return samp;
    }

};

}
}