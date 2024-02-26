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
        : m_image(image), m_area(image.get_rect()) {}

    image_sampler(image& image, const math::rectu& area)
        : m_image(image), m_area(m_image.get_rect().crop(area)) {}

    image_sampler(const image_sampler&) = default;
    image_sampler(image_sampler&&) noexcept = default;
    ~image_sampler() = default;

    image_sampler& operator=(const image_sampler&) = default;
    image_sampler& operator=(image_sampler&&) noexcept = default;

    // =============== specs ===============

    image_wrap get_xwrap() const { return m_xwrap; }
    image_wrap get_ywrap() const { return m_ywrap; }

    void set_xwrap(image_wrap wrap) { m_xwrap = wrap; }
    void set_ywrap(image_wrap wrap) { m_ywrap = wrap; }

    const math::color& get_border() const { return m_border; }
    void set_border(const math::color& border) { m_border = border; }

    const math::vec2& get_resolution() const { return m_resolution; }
    void set_resolution(const math::vec2& resolution) { m_resolution = resolution; }

    // =============== sampling ===============

    math::color sample(float u, float v) const
    {
        if (m_area.area() == 0)
        {
            return m_border;
        }

        const float x = u * (static_cast<float>(m_area.size.x) / m_resolution.x);
        const float y = v * (static_cast<float>(m_area.size.y) / m_resolution.y);

        const float sample_pixel_size = m_resolution.x * m_resolution.y;
        image_filter current_filter = (sample_pixel_size < 1.0f) ? m_min_filter : m_mag_filter;

        switch (current_filter)
        {
            case image_filter::NEAREST: return sample_nearest(x, y);
            case image_filter::LINEAR:  return sample_bilinear(x, y);

            default:                    break;
        }

        return m_border;
    }

    math::color sample_pixel(int x, int y) const
    {
        return sample(
            static_cast<float>(x) / static_cast<float>(m_area.size.x),
            static_cast<float>(y) / static_cast<float>(m_area.size.y)
        );
    }

private:

    math::color get_pixel(int x, int y) const
    {
        x = wrap_pixel(x, m_area.size.x, m_xwrap);
        y = wrap_pixel(y, m_area.size.y, m_ywrap);

        if (x < 0 || x >= static_cast<int>(m_area.size.x) || y < 0 || y >= static_cast<int>(m_area.size.y))
        {
            return m_border;
        }

        x += m_area.position.x;
        y += m_area.position.y;

        return m_image.get_pixel(x, y);
    }

    int wrap_pixel(int p, int size, image_wrap wrap) const
    {
        if (0 <= p && p < size)
        {
            return p;
        }

        switch (wrap)
        {
            case image_wrap::REPEAT:
                return p % size;

            case image_wrap::MIRRORED_REPEAT:
                return (p / size) % 2 ? size - 1 - (p % size) : (p % size);

            case image_wrap::EDGE_CLAMP:
                return math::clamp(p, 0, size - 1);

            default:
                return p;
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
        math::color out;

        for (size_t c = 0; c < out.size(); ++c)
        {
            out[c] = (
                pixels[0][c] * weights[0] +
                pixels[1][c] * weights[1] +
                pixels[2][c] * weights[2] +
                pixels[3][c] * weights[3]
            );
        }

        return out;
    }

private:

    image& m_image;
    math::rectu m_area;

    image_wrap m_xwrap = image_wrap::REPEAT;
    image_wrap m_ywrap = image_wrap::REPEAT;

    math::color m_border;

    math::vec2 m_resolution = math::vec2(1.0f);

    image_filter m_min_filter = image_filter::NEAREST;
    image_filter m_mag_filter = image_filter::LINEAR;
};

}
}