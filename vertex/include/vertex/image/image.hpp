#pragma once

#include "vertex/image/defs.hpp"
#include "vertex/pixel/surface.hpp"

namespace vx {
namespace img {

class image
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // Constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_DISABLE_MSVC_WARNING_PUSH()
    VX_DISABLE_MSVC_WARNING(26495) // variable uninitialized warning (for the union)
    image() {}
    VX_DISABLE_MSVC_WARNING_POP()

    image(size_t width, size_t height, pixel_format fmt)
        : m_format(fmt)
    {
        construct(width, height, fmt);
    }

    image(const uint8_t* data, size_t width, size_t height, pixel_format fmt)
        : m_format(fmt)
    {
        construct(data, width, height, fmt);
    }

    image(const image& other)
        : m_format(other.m_format)
    {
        copy_construct(other);
    }

    image& operator=(const image& other)
    {
        if (this != &other)
        {
            destroy();
            m_format = other.m_format;
            copy_construct(other);
        }
        return *this;
    }

    image(image&& other) noexcept
        : m_format(other.m_format)
    {
        move_construct(std::move(other));
        other.m_format = pixel_format::UNKNOWN;
    }

    image& operator=(image&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            m_format = other.m_format;
            move_construct(std::move(other));
            other.m_format = pixel_format::UNKNOWN;
        }
        return *this;
    }

    ~image()
    {
        destroy();
    }

private:

    void destroy() noexcept
    {
        switch (m_format)
        {
            case pixel_format::R_8:     m_r.~surface(); break;
            case pixel_format::RGB_8:   m_rgb.~surface(); break;
            case pixel_format::RGBA_8:  m_rgba.~surface(); break;
            default: break;
        }
    }

    void construct(size_t width, size_t height, pixel_format fmt)
    {
        switch (fmt)
        {
            case pixel_format::R_8:     new (&m_r) pixel::surface<pixel::pixel_format::R_8>(width, height); break;
            case pixel_format::RGB_8:   new (&m_rgb) pixel::surface<pixel::pixel_format::RGB_8>(width, height); break;
            case pixel_format::RGBA_8:  new (&m_rgba) pixel::surface<pixel::pixel_format::RGBA_8>(width, height); break;
            default: break;
        }
    }

    void construct(const byte_type* data, size_t width, size_t height, pixel_format fmt)
    {
        switch (fmt)
        {
            case pixel_format::R_8:     new (&m_r) pixel::surface<pixel::pixel_format::R_8>(data, width, height); break;
            case pixel_format::RGB_8:   new (&m_rgb) pixel::surface<pixel::pixel_format::RGB_8>(data, width, height); break;
            case pixel_format::RGBA_8:  new (&m_rgba) pixel::surface<pixel::pixel_format::RGBA_8>(data, width, height); break;
            default: break;
        }
    }

    void copy_construct(const image& other)
    {
        switch (other.m_format)
        {
            case pixel_format::R_8:     new (&m_r) pixel::surface<pixel::pixel_format::R_8>(other.m_r); break;
            case pixel_format::RGB_8:   new (&m_rgb) pixel::surface<pixel::pixel_format::RGB_8>(other.m_rgb); break;
            case pixel_format::RGBA_8:  new (&m_rgba) pixel::surface<pixel::pixel_format::RGBA_8>(other.m_rgba); break;
            default: break;
        }
    }

    void move_construct(image&& other) noexcept
    {
        switch (other.m_format)
        {
            case pixel_format::R_8:     new (&m_r) pixel::surface<pixel::pixel_format::R_8>(std::move(other.m_r)); break;
            case pixel_format::RGB_8:   new (&m_rgb) pixel::surface<pixel::pixel_format::RGB_8>(std::move(other.m_rgb)); break;
            case pixel_format::RGBA_8:  new (&m_rgba) pixel::surface<pixel::pixel_format::RGBA_8>(std::move(other.m_rgba)); break;
            default: break;
        }
    }

public:

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    bool operator==(const image& other) const
    {
        if (m_format != other.m_format)
        {
            return false;
        }

        switch (m_format)
        {
            case pixel_format::R_8:     return (m_r == other.m_r);
            case pixel_format::RGB_8:   return (m_rgb == other.m_rgb);
            case pixel_format::RGBA_8:  return (m_rgba == other.m_rgba);
            default:                    return false;
        }
    }

    bool operator!=(const image& other) const
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // info
    ///////////////////////////////////////////////////////////////////////////////

    pixel_format format() const { return m_format; }
    bool unknown_format() const { return m_format == pixel_format::UNKNOWN; }

    size_t channels() const
    {
        switch (m_format)
        {
            case pixel_format::R_8:     return m_r.channels();
            case pixel_format::RGB_8:   return m_rgb.channels();
            case pixel_format::RGBA_8:  return m_rgba.channels();
            default:                    return 0;
        }
    }

    size_t width() const
    {
        switch (m_format)
        {
            case pixel_format::R_8:     return m_r.width();
            case pixel_format::RGB_8:   return m_rgb.width();
            case pixel_format::RGBA_8:  return m_rgba.width();
            default:                    return 0;
        }
    }

    size_t height() const
    {
        switch (m_format)
        {
            case pixel_format::R_8:     return m_r.height();
            case pixel_format::RGB_8:   return m_rgb.height();
            case pixel_format::RGBA_8:  return m_rgba.height();
            default:                    return 0;
        }
    }

    math::vec2i size() const
    {
        switch (m_format)
        {
            case pixel_format::R_8:     return m_r.size();
            case pixel_format::RGB_8:   return m_rgb.size();
            case pixel_format::RGBA_8:  return m_rgba.size();
            default:                    return math::vec2i();
        }
    }

    math::recti get_rect() const
    {
        switch (m_format)
        {
            case pixel_format::R_8:     return m_r.get_rect();
            case pixel_format::RGB_8:   return m_rgb.get_rect();
            case pixel_format::RGBA_8:  return m_rgba.get_rect();
            default:                    return math::recti();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // raw data
    ///////////////////////////////////////////////////////////////////////////////

    const byte_type* data() const
    {
        switch (m_format)
        {
            case pixel_format::R_8:     return m_r.data();
            case pixel_format::RGB_8:   return m_rgb.data();
            case pixel_format::RGBA_8:  return m_rgba.data();
            default:                    return nullptr;
        }
    }

    size_t data_size() const
    {
        switch (m_format)
        {
            case pixel_format::R_8:     return m_r.data_size();
            case pixel_format::RGB_8:   return m_rgb.data_size();
            case pixel_format::RGBA_8:  return m_rgba.data_size();
            default:                    return 0;
        }
    }

    bool empty() const
    {
        switch (m_format)
        {
            case pixel_format::R_8:     return m_r.empty();
            case pixel_format::RGB_8:   return m_rgb.empty();
            case pixel_format::RGBA_8:  return m_rgba.empty();
            default:                    return true;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // pixel access
    ///////////////////////////////////////////////////////////////////////////////

    math::color get_pixel(size_t x, size_t y, const math::color& default_color = math::color()) const
    {
        switch (m_format)
        {
            case pixel_format::R_8:     return m_r.get_pixel(x, y, default_color);
            case pixel_format::RGB_8:   return m_rgb.get_pixel(x, y, default_color);
            case pixel_format::RGBA_8:  return m_rgba.get_pixel(x, y, default_color);
            default:                    return default_color;
        }
    }

    math::color get_pixel(const math::vec2i& p, const math::color& default_color = math::color()) const
    {
        return get_pixel(
            static_cast<size_t>(p.x),
            static_cast<size_t>(p.y),
            default_color
        );
    }

    void set_pixel(size_t x, size_t y, const math::color& color)
    {
        switch (m_format)
        {
            case pixel_format::R_8:     m_r.set_pixel(x, y, color); return;
            case pixel_format::RGB_8:   m_rgb.set_pixel(x, y, color); return;
            case pixel_format::RGBA_8:  m_rgba.set_pixel(x, y, color); return;
            default:                    return;
        }
    }

    void set_pixel(const math::vec2i& p, const math::color& color) noexcept
    {
        set_pixel(
            static_cast<size_t>(p.x),
            static_cast<size_t>(p.y),
            color
        );
    }

    void fill(const math::color& color)
    {
        switch (m_format)
        {
            case pixel_format::R_8:     m_r.fill(color); return;
            case pixel_format::RGB_8:   m_rgb.fill(color); return;
            case pixel_format::RGBA_8:  m_rgba.fill(color); return;
            default:                    return;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // conversion
    ///////////////////////////////////////////////////////////////////////////////

    image convert(pixel_format fmt) const
    {
        image out;

        if (fmt == m_format)
        {
            out = *this;
        }
        else
        {
            const size_t w = width();
            const size_t h = height();

            out = image(w, h, fmt);

            for (size_t y = 0; y < h; ++y)
            {
                for (size_t x = 0; x < w; ++x)
                {
                    out.set_pixel(x, y, get_pixel(x, y));
                }
            }
        }

        return out;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // to_surface
    ///////////////////////////////////////////////////////////////////////////////

    template <pixel::pixel_format F>
    pixel::surface<F> to_surface() const
    {
        switch (m_format)
        {
            case pixel_format::R_8:     return m_r.convert<F>();
            case pixel_format::RGB_8:   return m_rgb.convert<F>();
            case pixel_format::RGBA_8:  return m_rgba.convert<F>();
            default:                    return {};
        }
    }

private:

    pixel_format m_format = pixel_format::UNKNOWN;

    union
    {
        pixel::surface<pixel::pixel_format::R_8> m_r;
        pixel::surface<pixel::pixel_format::RGB_8> m_rgb;
        pixel::surface<pixel::pixel_format::RGBA_8> m_rgba;
    };
};

} // namespace img
} // namespace vx
