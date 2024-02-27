#pragma once

#include "image.h"
#include "enum.h"

#include <iostream>

namespace vx {
namespace img {

class image_sampler
{
public:

    // =============== constructors & destructor ===============
    
    image_sampler(image& image)
        : m_image(image) {}

    image_sampler(const image_sampler&) = default;
    image_sampler(image_sampler&&) noexcept = default;
    ~image_sampler() = default;

    image_sampler& operator=(const image_sampler&) = default;
    image_sampler& operator=(image_sampler&&) noexcept = default;

    // =============== specs ===============

    const math::vec2& get_resolution() const { return m_resolution; }
    void set_resolution(const math::vec2& resolution) { m_resolution = resolution; }

    image_wrap get_xwrap() const { return m_xwrap; }
    image_wrap get_ywrap() const { return m_ywrap; }

    void set_xwrap(image_wrap wrap) { m_xwrap = wrap; }
    void set_ywrap(image_wrap wrap) { m_ywrap = wrap; }

    const math::color& get_border() const { return m_border; }
    void set_border(const math::color& border) { m_border = border; }

    image_filter get_min_filter() const { return m_min_filter; }
    image_filter get_mag_filter() const { return m_mag_filter; }

    void set_min_filter(image_filter filter) { m_min_filter = filter; }
    void set_mag_filter(image_filter filter) { m_mag_filter = filter; }

    // =============== sampling ===============

    math::color sample_pixel(int x, int y) const
    {
        return sample(
            static_cast<float>(x) / static_cast<float>(m_image.width()),
            static_cast<float>(y) / static_cast<float>(m_image.height())
        );
    }

    math::color sample(float u, float v) const
    {
        if (m_image.empty())
        {
            return m_border;
        }

        u *= (static_cast<float>(m_image.width()) / m_resolution.x);
        v *= (static_cast<float>(m_image.height()) / m_resolution.y);

        const float sample_pixel_size = m_resolution.x * m_resolution.y;
        image_filter current_filter = (sample_pixel_size < 1.0f) ? m_min_filter : m_mag_filter;

        switch (current_filter)
        {
            case image_filter::NEAREST: return sample_nearest(u, v);
            case image_filter::LINEAR:  return sample_bilinear(u, v);

            default:                    break;
        }

        return m_border;
    }

private:

    math::color get_pixel(int x, int y) const
    {
        x = wrap_pixel(x, m_image.width(), m_xwrap);
        y = wrap_pixel(y, m_image.height(), m_ywrap);

        if (!math::contains(m_image.get_rect(), math::vec2i(x, y)))
        {
            return m_border;
        }

        return m_image.get_pixel(x, y);
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
        const int srcx = static_cast<int>(srcxfrac);

        const float fx2 = srcxfrac - srcx;
        const float fx = 1.0f - fx2;

        // y
        const float srcyfrac = v - 0.5f;
        int srcy = static_cast<int>(srcyfrac);

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

    image& m_image;

    math::vec2 m_resolution = math::vec2(1.0f);

    image_wrap m_xwrap = image_wrap::REPEAT;
    image_wrap m_ywrap = image_wrap::REPEAT;

    math::color m_border;

    image_filter m_min_filter = image_filter::NEAREST;
    image_filter m_mag_filter = image_filter::LINEAR;
};

}
}