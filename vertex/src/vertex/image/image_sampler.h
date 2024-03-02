#pragma once

#include "image.h"
#include "enum_image_filter.h"
#include "enum_image_wrap.h"

namespace vx {
namespace img {

struct sampler
{
    // =============== constructors & destructor ===============
    
    sampler(const image& image)
        : m_image(&image) {}

    sampler(const sampler&) = delete;
    sampler(sampler&&) noexcept = delete;
    ~sampler() = default;

    sampler& operator=(const sampler&) = delete;
    sampler& operator=(sampler&&) noexcept = delete;

    // =============== data ===============

    math::vec2 resolution = math::vec2(1.0f);

    image_wrap xwrap = image_wrap::REPEAT;
    image_wrap ywrap = image_wrap::REPEAT;

    math::color border;

    image_filter min_filter = image_filter::NEAREST;
    image_filter mag_filter = image_filter::LINEAR;

    // =============== sampling ===============

    math::color sample_pixel(int x, int y) const
    {
        return sample(
            static_cast<float>(x) / static_cast<float>(m_image->width()),
            static_cast<float>(y) / static_cast<float>(m_image->height())
        );
    }

    math::color sample_pixel(const math::vec2i& p) const
    {
        return sample_pixel(p.x, p.y);
    }

    math::color sample(float u, float v) const
    {
        if (m_image->empty())
        {
            return border;
        }

        u *= (static_cast<float>(m_image->width()) / resolution.x);
        v *= (static_cast<float>(m_image->height()) / resolution.y);

        const float sample_pixel_area = resolution.x * resolution.y;
        image_filter current_filter = (sample_pixel_area < 1.0f) ? min_filter : mag_filter;

        switch (current_filter)
        {
            case image_filter::NEAREST: return sample_nearest(u, v);
            case image_filter::LINEAR:  return sample_bilinear(u, v);

            default:                    break;
        }

        return border;
    }

    math::color sample(const math::vec2& p) const
    {
        return sample(p.x, p.y);
    }

private:

    math::color get_pixel(int x, int y) const
    {
        x = wrap_pixel(x, m_image->width(), xwrap);
        y = wrap_pixel(y, m_image->height(), ywrap);

        return m_image->get_pixel(x, y, border);
    }

    int wrap_pixel(int p, int size, image_wrap wrap) const
    {
        if (0 <= p && p < size)
        {
            return p;
        }

       // https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf (page 260)

        switch (wrap)
        {
            case image_wrap::CLAMP_TO_EDGE:
            {
                return math::clamp(p, 0, size - 1);
            }
            case image_wrap::CLAMP_TO_BORDER:
            {
                return math::clamp(p, -1, size);
            }
            case image_wrap::REPEAT:
            {
                return math::mod(p, size);
            }
            case image_wrap::MIRRORED_REPEAT:
            {
                const int a = math::mod(p, 2 * size) - size;
                return (size - 1) - (a >= 0 ? a : -(1 + a));
            }
            case image_wrap::MIRROR_CLAMP_TO_EDGE:
            {
                const int a = p;
                return math::clamp(a >= 0 ? a : -(1 + a), 0, size - 1);
            }
            default:
            {
                return p;
            }
        }
    }

    // =============== sample methods ===============

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

        // sample
        math::color samp;

        for (size_t c = 0; c < samp.channels(); ++c)
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

private:

    const image* m_image = nullptr;

};

}
}