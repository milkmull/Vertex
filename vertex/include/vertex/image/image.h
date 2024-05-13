#pragma once

#include <vector>

#include "util/image_format_helpers.h"
#include "util/image_info_helpers.h"
#include "util/size_limit.h"
#include "vertex/pixel/pixel_color.h"

#include "vertex/math/geometry/type/recti.h"

namespace vx {
namespace img {

class image
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    image()
        : m_width(0)
        , m_height(0)
        , m_format(image_pixel_format::PIXEL_FORMAT_RGBA_8) {}

    // basic constructors

    image(size_t width, size_t height, image_pixel_format format)
        : m_width (math::min(width,  static_cast<size_t>(util::IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)))
        , m_height(math::min(height, static_cast<size_t>(util::IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)))
        , m_format(util::is_valid_pixel_format(format) ? format : image_pixel_format::PIXEL_FORMAT_R_8)
        , m_data(m_width * m_height * pixel_size(), 0) {}

    image(const image_info& info)
        : image(info.width, info.height, info.format) {}

    // data constructors

    image(const byte_type* data, size_t width, size_t height, image_pixel_format format)
        : m_width (math::min(width,  static_cast<size_t>(util::IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)))
        , m_height(math::min(height, static_cast<size_t>(util::IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)))
        , m_format(util::is_valid_pixel_format(format) ? format : image_pixel_format::PIXEL_FORMAT_R_8)
        , m_data(data, data + (m_width * m_height * pixel_size())) {}

    image(const byte_type* data, const image_info& info)
        : image(data, info.width, info.height, info.format) {}

    // copy & move constructors

    image(const image& other)
        : m_width(other.m_width)
        , m_height(other.m_height)
        , m_format(other.m_format)
        , m_data(other.m_data) {}

    image(image&& other) noexcept
        : m_width(other.m_width)
        , m_height(other.m_height)
        , m_format(other.m_format)
        , m_data(std::move(other.m_data)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////////

    ~image() = default;

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operator
    ///////////////////////////////////////////////////////////////////////////////

    image& operator=(const image& other)
    {
        m_width = other.m_width;
        m_height = other.m_height;
        m_format = other.m_format;
        m_data = other.m_data;
        return *this;
    }

    image& operator=(image&& other) noexcept
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

    using channel_type = uint8_t;

    inline constexpr size_t width() const { return m_width; }
    inline constexpr size_t height() const { return m_height; }
    inline constexpr image_pixel_format format() const { return m_format; }
    inline constexpr bool is_valid_format() const { return util::is_valid_pixel_format(m_format); }

    inline constexpr image_info get_info() const { return image_info{ m_width, m_height, m_format }; }

    inline constexpr size_t channels() const { return pixel::get_channel_count(static_cast<pixel::pixel_format>(m_format)); }
    inline constexpr bool has_alpha() const { return pixel::has_alpha(static_cast<pixel::pixel_format>(m_format)); }
    inline constexpr size_t pixel_size() const { return pixel::get_pixel_size(static_cast<pixel::pixel_format>(m_format)); }

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

    inline bool operator==(const image& other) const
    {
        return m_width == other.m_width
            && m_height == other.m_height
            && m_format == other.m_format
            && m_data == other.m_data;
    }

    inline bool operator!=(const image& other) const
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

        switch (m_format)
        {
            case image_pixel_format::PIXEL_FORMAT_R_8:	    return static_cast<math::color>(*(pixel::pixel_r_8*)      (&m_data[offset]));
            case image_pixel_format::PIXEL_FORMAT_RG_8:	    return static_cast<math::color>(*(pixel::pixel_rg_8*)     (&m_data[offset]));
            case image_pixel_format::PIXEL_FORMAT_RGB_8:	return static_cast<math::color>(*(pixel::pixel_rgb_8*)    (&m_data[offset]));
            case image_pixel_format::PIXEL_FORMAT_RGBA_8:	return static_cast<math::color>(*(pixel::pixel_rgba_8*)   (&m_data[offset]));

            default:					                    break;
        }

        return default_color;
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

        switch (m_format)
        {
            case image_pixel_format::PIXEL_FORMAT_R_8:	    return (void)(*(pixel::pixel_r_8*)      (&m_data[offset]) = color);
            case image_pixel_format::PIXEL_FORMAT_RG_8:	    return (void)(*(pixel::pixel_rg_8*)     (&m_data[offset]) = color);
            case image_pixel_format::PIXEL_FORMAT_RGB_8:	return (void)(*(pixel::pixel_rgb_8*)    (&m_data[offset]) = color);
            case image_pixel_format::PIXEL_FORMAT_RGBA_8:	return (void)(*(pixel::pixel_rgba_8*)   (&m_data[offset]) = color);

            default:                                        break;
        }

        return;
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
    /// @brief Fill the entire image with the specified color.
    ///
    /// @param color The color to fill the image with.
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
    /// @brief Convert the image to the specified format.
    ///
    /// @param format The target image format to convert to.
    ///////////////////////////////////////////////////////////////////////////////
    void convert(image_pixel_format format)
    {
        if (format == m_format)
        {
            return;
        }

        image converted(m_width, m_height, format);

        for (size_t y = 0; y < m_height; ++y)
        {
            for (size_t x = 0; x < m_width; ++x)
            {
                converted.set_pixel(x, y, get_pixel(x, y));
            }
        }

        *this = converted;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Reinterpret the image with a new format.
    ///
    /// Reinterpreting the image does not alter the underlying pixel data but
    /// rather changes how the data is interpreted.
    /// 
    /// @param format The target image format to reinterpret to.
    /// 
    /// @return True if reinterpretation was successful, false otherwise.
    ///////////////////////////////////////////////////////////////////////////////
    VX_NODISCARD bool reinterpret(image_pixel_format format)
    {
        image_info info = get_info();

        if (util::reinterpret_info(info, format) != util::image_error_code::NONE)
        {
            return false;
        }

        if (util::get_image_info_error(info) != util::image_error_code::NONE)
        {
            return false;
        }

        m_width = info.width;
        m_height = info.height;
        m_format = format;

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Reinterpret the image with the provided image_info.
    /// 
    /// Reinterpreting the image does not alter the underlying pixel data but
    /// rather changes how the data is interpreted.
    ///
    /// @param info The target image_info to reinterpret to.
    /// 
    /// @return True if reinterpretation was successful, false otherwise.
    ///////////////////////////////////////////////////////////////////////////////
    VX_NODISCARD bool reinterpret(const image_info& info)
    {
        if (util::get_image_info_error(info) != util::image_error_code::NONE)
        {
            return false;
        }

        if (info.size() != m_data.size())
        {
            return false;
        }

        m_width = info.width;
        m_height = info.height;
        m_format = info.format;

        return true;
    }

private:

    size_t m_width;
    size_t m_height;
    image_pixel_format m_format;
    std::vector<byte_type> m_data;

};

}
}