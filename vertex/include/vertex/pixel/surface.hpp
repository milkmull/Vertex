#pragma once

#include <vector>

#include "./raw_pixel.hpp"
#include "vertex/math/rect.hpp"

namespace vx {
namespace pixel {

template <pixel_format F>
class surface
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // format details
    ///////////////////////////////////////////////////////////////////////////////

    static constexpr pixel_format format = F;

    static VX_FORCE_INLINE constexpr size_t channels() const noexcept { return get_pixel_channel_count(format); }
    static VX_FORCE_INLINE constexpr bool has_alpha() const noexcept { return pixel_has_alpha(format); }
    static VX_FORCE_INLINE constexpr size_t pixel_size() const noexcept { return get_pixel_size(format); }

    using raw_pixel_type = typename raw_pixel<format>;
    using pixel_type = typename raw_pixel_type::pixel_type;
    using channel_type = typename raw_pixel_type::channel_type;

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    surface()
        : m_width(0)
        , m_height(0) {}

    // basic constructors

    surface(size_t width, size_t height)
        : m_width(width)
        , m_height(height)
        , m_data(m_width * m_height * pixel_size(), 0) {}

    // data constructors

    surface(const byte_type* data, size_t width, size_t height)
        : m_width(width)
        , m_height(height)
        , m_data(data, data + (m_width * m_height * pixel_size())) {}

    // copy & move constructors

    surface(const surface& other)
        : m_width(other.m_width)
        , m_height(other.m_height)
        , m_data(other.m_data) {}

    surface(surface&& other) noexcept
        : m_width(other.m_width)
        , m_height(other.m_height)
        , m_data(std::move(other.m_data)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operator
    ///////////////////////////////////////////////////////////////////////////////

    surface& operator=(const surface& other)
    {
        m_width = other.m_width;
        m_height = other.m_height;
        m_data = other.m_data;
        return *this;
    }

    surface& operator=(surface&& other) noexcept
    {
        m_width = other.m_width;
        m_height = other.m_height;
        m_data = std::move(other.m_data);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // info
    ///////////////////////////////////////////////////////////////////////////////

    inline constexpr size_t width() const { return m_width; }
    inline constexpr size_t height() const { return m_height; }
    inline constexpr size_t stride() const { return m_width * pixel_size(); }

    inline byte_type* data() { return m_data.data(); }
    inline const byte_type* data() const { return m_data.data(); }
    inline size_t data_size() const { return m_data.size(); }
    inline bool empty() const { return m_data.empty(); }

    inline constexpr math::vec2i size() const { return math::vec2i(m_width, m_height); }
    inline constexpr math::recti get_rect() const { return math::recti(0, 0, m_width, m_height); }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    inline bool operator==(const surface& other) const
    {
        return m_width == other.m_width
            && m_height == other.m_height
            && m_data == other.m_data;
    }

    inline bool operator!=(const surface& other) const
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // pixel access
    ///////////////////////////////////////////////////////////////////////////////

    math::color get_pixel(size_t x, size_t y, const math::color& default_color = math::color()) const
    {
        if (x >= m_width || y >= m_height)
        {
            return default_color;
        }
    
        const size_t offset = (m_width * y + x) * pixel_size();
        return *(raw_pixel_type*)(&m_data[offset], m_format);
    }

    math::color get_pixel(const math::vec2i& p, const math::color& default_color = math::color()) const
    {
        return get_pixel(static_cast<size_t>(p.x), static_cast<size_t>(p.y), default_color);
    }

    void set_pixel(size_t x, size_t y, const math::color& color)
    {
        if (x >= m_width || y >= m_height)
        {
            return;
        }
    
        const size_t offset = (m_width * y + x) * pixel_size();
        (*(raw_pixel_type*)(&m_data[offset], m_format)) = color;
    }

    void set_pixel(const math::vec2i& p, const math::color& color)
    {
        set_pixel(static_cast<size_t>(p.x), static_cast<size_t>(p.y), color);
    }

    void fill(const math::color& color)
    {
        for (size_t y = 0; y < m_height; ++y)
        {
            for (size_t x = 0; x < m_width; ++x)
            {
                set_pixel(x, y, color);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // conversion
    ///////////////////////////////////////////////////////////////////////////////

    template <pixel_format F2>
    surface<F2> convert() const
    {
        surface<F2> converted(m_width, m_height);
    
        for (size_t y = 0; y < m_height; ++y)
        {
            for (size_t x = 0; x < m_width; ++x)
            {
                converted.set_pixel(x, y, get_pixel(x, y));
            }
        }
    
        return converted;
    }

    template <pixel_format F2>
    static surface<F2> reinterpret(surface<F2>&& src, size_t new_width, size_t new_height)
    {
        if (F2 == pixel_format::UNKNOWN)
        {
            throw std::invalid_argument("Cannot reinterpret to UNKNOWN format");
        }

        size_t expected_size = new_width * new_height * get_pixel_size(F2);
        if (expected_size != src.m_data.size())
        {
            throw std::runtime_error("Size mismatch in reinterpretation");
        }

        surface<F2> dst;
        dst.m_width = new_width;
        dst.m_height = new_height;
        dst.m_data = std::move(src.m_data);

        return dst;
    }

private:

    size_t m_width;
    size_t m_height;
    std::vector<byte_type> m_data;

};

} // namespace pixel
} // namespace vx