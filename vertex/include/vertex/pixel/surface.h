#pragma once

#include "iterator.h"
#include "vertex/math/geometry/type/recti.h"

namespace vx {
namespace pixel {

class surface
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    surface()
        : m_width(0)
        , m_height(0)
        , m_format(pixel_format::RGBA_8) {}

    // basic constructors

    surface(size_t width, size_t height, pixel_format format)
        : m_width(width)
        , m_height(height)
        , m_format(format)
        , m_data(m_width * m_height * pixel_size(), 0) {}

    // data constructors

    surface(const byte_type* data, size_t width, size_t height, pixel_format format)
        : m_width(width)
        , m_height(height)
        , m_format(format)
        , m_data(data, data + (m_width * m_height * pixel_size())) {}

    // copy & move constructors

    surface(const surface& other)
        : m_width(other.m_width)
        , m_height(other.m_height)
        , m_format(other.m_format)
        , m_data(other.m_data) {}

    surface(surface&& other) noexcept
        : m_width(other.m_width)
        , m_height(other.m_height)
        , m_format(other.m_format)
        , m_data(std::move(other.m_data)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////////

    ~surface() = default;

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operator
    ///////////////////////////////////////////////////////////////////////////////

    surface& operator=(const surface& other)
    {
        m_width = other.m_width;
        m_height = other.m_height;
        m_format = other.m_format;
        m_data = other.m_data;
        return *this;
    }

    surface& operator=(surface&& other) noexcept
    {
        m_width = other.m_width;
        m_height = other.m_height;
        m_format = other.m_format;
        m_data = std::move(other.m_data);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // info
    ///////////////////////////////////////////////////////////////////////////////

    inline constexpr size_t width() const { return m_width; }
    inline constexpr size_t height() const { return m_height; }
    inline constexpr pixel_format format() const { return m_format; }
    inline constexpr bool unknown_format() const { return m_format == pixel_format::UNKNOWN; }

    inline constexpr size_t channels() const { return get_pixel_channel_count(m_format); }
    inline constexpr bool has_alpha() const { return pixel_has_alpha(m_format); }
    inline constexpr size_t pixel_size() const { return get_pixel_size(m_format); }
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
            && m_format == other.m_format
            && m_data == other.m_data;
    }

    inline bool operator!=(const surface& other) const
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // pixel access
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Get the color of the pixel at the specified coordinates.
    ///
    /// @param x The x-coordinate of the pixel.
    /// @param y The y-coordinate of the pixel.
    /// @param default_color The default color to return if the pixel is out of bounds.
    /// 
    /// @return The color of the pixel at the specified coordinates.
    ///////////////////////////////////////////////////////////////////////////////
    math::color get_pixel(size_t x, size_t y, const math::color& default_color = math::color()) const
    {
        if (x >= m_width || y >= m_height)
        {
            return default_color;
        }
    
        const size_t offset = (m_width * y + x) * pixel_size();
        return pixel_to_color(&m_data[offset], m_format);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Get the color of the pixel at the specified coordinates.
    ///
    /// @param p The coordinates of the pixel.
    /// @param default_color The default color to return if the pixel is out of bounds.
    /// 
    /// @return The color of the pixel at the specified coordinates.
    ///////////////////////////////////////////////////////////////////////////////
    math::color get_pixel(const math::vec2i& p, const math::color& default_color = math::color()) const
    {
        return get_pixel(static_cast<size_t>(p.x), static_cast<size_t>(p.y), default_color);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Set the color of the pixel at the specified coordinates.
    ///
    /// @param x The x-coordinate of the pixel.
    /// @param y The y-coordinate of the pixel.
    /// 
    /// @param color The color to set for the pixel.
    ///////////////////////////////////////////////////////////////////////////////
    void set_pixel(size_t x, size_t y, const math::color& color)
    {
        if (x >= m_width || y >= m_height)
        {
            return;
        }
    
        const size_t offset = (m_width * y + x) * pixel_size();
        color_to_pixel(color, &m_data[offset], m_format);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Set the color of the pixel at the specified coordinates.
    ///
    /// @param p The coordinates of the pixel.
    /// @param color The color to set for the pixel.
    ///////////////////////////////////////////////////////////////////////////////
    void set_pixel(const math::vec2i& p, const math::color& color)
    {
        set_pixel(static_cast<size_t>(p.x), static_cast<size_t>(p.y), color);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Fill the entire surface with the specified color.
    ///
    /// @param color The color to fill the surface with.
    ///////////////////////////////////////////////////////////////////////////////
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

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Convert the surface to the specified format.
    ///
    /// @param format The target surface format to convert to.
    ///////////////////////////////////////////////////////////////////////////////
    surface convert(pixel_format format) const
    {
        if (format == m_format)
        {
            return *this;
        }
    
        surface converted(m_width, m_height, format);
    
        for (size_t y = 0; y < m_height; ++y)
        {
            for (size_t x = 0; x < m_width; ++x)
            {
                converted.set_pixel(x, y, get_pixel(x, y));
            }
        }
    
        return converted;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Reinterpret the surface data to match a new target width, height,
    /// and pixel format without modifying the underlying pixel data.
    ///
    /// @param target_width The new width of the surface after reinterpretation.
    /// @param target_height The new height of the surface after reinterpretation.
    /// @param target_format The new pixel format to reinterpret the surface as.
    ///
    /// @return bool Returns true if the reinterpretation is successful, meaning
    /// the data size matches the required size for the new dimensions and format.
    /// Returns false if the target format is UNKNOWN or if the new size calculated
    /// from the target dimensions and format does not match the existing data
    /// size.
    ///////////////////////////////////////////////////////////////////////////////
    VX_NODISCARD bool reinterpret(size_t target_width, size_t target_height, pixel_format target_format)
    {
        if (target_format == pixel_format::UNKNOWN)
        {
            return false;
        }

        size_t target_size = target_width * target_height * get_pixel_size(target_format);
        if (target_size != m_data.size())
        {
            return false;
        }
    
        m_width = target_width;
        m_height = target_height;
        m_format = target_format;
    
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // iterators
    ///////////////////////////////////////////////////////////////////////////////

    using iterator = pixel::iterator;
    using const_iterator = pixel::const_iterator;

    inline iterator begin()
    {
        return iterator(m_data.data(), 0, 0, m_width, m_height, m_format);
    }

    inline iterator end()
    {
        return iterator(m_data.data() + m_data.size(), 0, m_height, m_width, m_height, m_format);
    }

    inline const_iterator begin() const
    {
        return const_iterator(m_data.data(), 0, 0, m_width, m_height, m_format);
    }

    inline const_iterator end() const
    {
        return const_iterator(m_data.data() + m_data.size(), 0, m_height, m_width, m_height, m_format);
    }

    inline const_iterator cbegin() const
    {
        return begin();
    }

    inline const_iterator cend() const
    {
        return end();
    }

private:

    size_t m_width;
    size_t m_height;
    pixel_format m_format;
    std::vector<byte_type> m_data;

};

}
}