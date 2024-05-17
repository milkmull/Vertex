#pragma once

#include "enum_filter_mode.h"
#include "enum_wrap_mode.h"
#include "pixel_color.h"

#include "vertex/math/sample/wrap.h"
#include "vertex/math/math/type/vec2.h"
#include "vertex/math/math/type/vec2i.h"

namespace vx {
namespace pixel {

template <pixel_format f>
class sampler
{
public:

    using pixel_type = pixel<f>;

    inline sampler(const uint8_t* data, size_t width, size_t height)
        : m_data(data), m_width(width), m_height(height) {}

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

private:

    const uint8_t* m_data;
    size_t m_width;
    size_t m_height;

public:

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

    math::color sample_pixel(int x, int y) const
    {
        return sample(
            static_cast<float>(x) / static_cast<float>(m_width),
            static_cast<float>(y) / static_cast<float>(m_height)
        );
    }

    math::color sample_pixel(const math::vec2i& p) const
    {
        return sample_pixel(p.x, p.y);
    }

    math::color sample(float u, float v) const
    {
        if (m_width == 0 || m_height == 0 || f == pixel_format::PIXEL_FORMAT_UNKNOWN)
        {
            return border;
        }

        u *= (static_cast<float>(m_width) / resolution.x);
        v *= (static_cast<float>(m_height) / resolution.y);

        const float sample_pixel_area = resolution.x * resolution.y;
        filter_mode current_filter = (sample_pixel_area < 1.0f) ? min_filter : mag_filter;

        switch (current_filter)
        {
            case filter_mode::NEAREST: return sample_nearest(u, v);
            case filter_mode::LINEAR:  return sample_bilinear(u, v);

            default:                    break;
        }

        return border;
    }

    math::color sample(const math::vec2& uv) const
    {
        return sample(uv.x, uv.y);
    }

private:

    math::color get_pixel(int x, int y) const
    {
        x = wrap_pixel(x, static_cast<int>(m_width), xwrap);
        y = wrap_pixel(y, static_cast<int>(m_height), ywrap);

        if (x >= m_width || y >= m_height)
        {
            return border;
        }

        const size_t offset = (m_width * y + x) * pixel_type::info.pixel_size;
        return *(pixel_type*)(&m_data[offset]);
    }

    int wrap_pixel(int p, int size, wrap_mode wrap) const
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

    math::color sample_nearest(float u, float v) const
    {
        return get_pixel(static_cast<int>(u), static_cast<int>(v));
    }

    math::color sample_bilinear(float u, float v) const
    {
        // x
        const float srcxfrac = u - 0.5f;
        const int srcx = static_cast<int>(math::floor(srcxfrac));

        const float fx2 = srcxfrac - srcx;
        const float fx = 1.0f - fx2;

        // y
        const float srcyfrac = v - 0.5f;
        const int srcy = static_cast<int>(math::floor(srcyfrac));

        const float fy2 = srcyfrac - srcy;
        const float fy = 1.0f - fy2;

        // interpolate
        const math::color pixels[4] =
        {
            get_pixel(srcx, srcy),
            get_pixel(srcx + 1, srcy),
            get_pixel(srcx, srcy + 1),
            get_pixel(srcx + 1, srcy + 1)
        };

        const float weights[4] =
        {
            fx  * fy,
            fx2 * fy,
            fx  * fy2,
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