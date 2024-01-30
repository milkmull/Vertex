#pragma once

#include <cassert>
#include <utility>

#include "detail/iterator.h"
#include "image_load.h"
#include "image_write.h"

#include "vertex/math/math/detail/vec2i_type.h"
#include "vertex/math/geometry/detail/recti_type.h"
#include "vertex/math/math/vec_fn_comparison.h"

namespace vx {
namespace img {

class image
{
public:

    // =============== constructors ===============

    image()
        : m_width(0)
        , m_height(0)
        , m_format(image_format::RGBA8) {}

    image(size_type width, size_type height, image_format format)
        : m_width (std::min(width,  static_cast<size_type>(VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)))
        , m_height(std::min(height, static_cast<size_type>(VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)))
        , m_format((format != image_format::UNKNOWN) ? format : image_format::RGBA8)
        , m_data(m_width * m_height * pixel_size(), 0) {}

    image(const byte_type* data, size_type width, size_type height, image_format format)
        : m_width (std::min(width,  static_cast<size_type>(VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)))
        , m_height(std::min(height, static_cast<size_type>(VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)))
        , m_format((format != image_format::UNKNOWN) ? format : image_format::RGBA8)
        , m_data(data, data + (m_width * m_height * pixel_size())) {}

    image(const char* path, error_code& err)
    {
        image_info info;
        err = load_image(path, info, m_data);

        m_width = info.width;
        m_height = info.height;
        m_format = info.format;

        assert(info.size() == m_data.size());
    }

    image(const char* path, image_format format, error_code& err)
    {
        image_info info;
        err = load_image(path, info, format, m_data);

        m_width = info.width;
        m_height = info.height;
        m_format = info.format;

        assert(info.size() == m_data.size());
    }

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

    // =============== destructor ===============

    ~image() = default;

    // =============== assignment operators ===============

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

    // =============== info ===============

    inline size_type width() const { return m_width; }
    inline size_type height() const { return m_height; }
    inline image_format format() const { return m_format; }
    inline bool is_valid_format() const { return m_format != image_format::UNKNOWN; }

    inline image_info get_info() const { return image_info{ m_width, m_height, m_format }; }

    inline size_type channels() const { return get_channel_count(m_format); }
    inline size_type bitdepth() const { return get_bitdepth(m_format); }
    inline bool has_alpha() const { return img::has_alpha(m_format); }

    inline size_type pixel_size() const { return get_pixel_size(m_format); }
    inline size_type stride() const { return m_width * pixel_size(); }

    inline size_t length() const { return m_data.size(); }
    inline bool empty() const { return m_data.empty(); }

    inline const std::vector<byte_type> data() const { return m_data; }
    inline const byte_type* raw_data() const { return m_data.data(); }

    inline vec2ui size() const { return vec2ui(m_width, m_height); }
    inline rect2ui get_rect() const { return rect2ui(0, 0, m_width, m_height); }

    // =============== comparison ===============

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

    // =============== pixel ===============

    color get_pixel(size_type x, size_type y) const
    {
        const size_type offset = (m_width * y + x) * pixel_size();

        if (offset >= m_data.size())
        {
            return color();
        }

        switch (m_format)
        {
            case image_format::R8:		return static_cast<color>(*(pixel_r8*)      (&m_data[offset]));
            case image_format::RG8:		return static_cast<color>(*(pixel_rg8*)     (&m_data[offset]));
            case image_format::RGB8:	return static_cast<color>(*(pixel_rgb8*)    (&m_data[offset]));
            case image_format::RGBA8:	return static_cast<color>(*(pixel_rgba8*)   (&m_data[offset]));
                                                                                                   
            case image_format::R16:		return static_cast<color>(*(pixel_r16*)     (&m_data[offset]));
            case image_format::RG16:	return static_cast<color>(*(pixel_rg16*)    (&m_data[offset]));
            case image_format::RGB16:	return static_cast<color>(*(pixel_rgb16*)   (&m_data[offset]));
            case image_format::RGBA16:	return static_cast<color>(*(pixel_rgba16*)  (&m_data[offset]));
                                                                                                   
            case image_format::R32F:	return static_cast<color>(*(pixel_r32f*)    (&m_data[offset]));
            case image_format::RG32F:	return static_cast<color>(*(pixel_rg32f*)   (&m_data[offset]));
            case image_format::RGB32F:	return static_cast<color>(*(pixel_rgb32f*)  (&m_data[offset]));
            case image_format::RGBA32F:	return static_cast<color>(*(pixel_rgba32f*) (&m_data[offset]));

            default:					break;
        }

        return color();
    }

    void set_pixel(size_type x, size_type y, const color& color)
    {
        const size_type offset = (m_width * y + x) * pixel_size();

        if (offset >= m_data.size())
        {
            return;
        }

        switch (m_format)
        {
            case image_format::R8:      return (void)(*(pixel_r8*)      (&m_data[offset]) = color);
            case image_format::RG8:     return (void)(*(pixel_rg8*)     (&m_data[offset]) = color);
            case image_format::RGB8:    return (void)(*(pixel_rgb8*)    (&m_data[offset]) = color);
            case image_format::RGBA8:   return (void)(*(pixel_rgba8*)   (&m_data[offset]) = color);

            case image_format::R16:     return (void)(*(pixel_r16*)     (&m_data[offset]) = color);
            case image_format::RG16:    return (void)(*(pixel_rg16*)    (&m_data[offset]) = color);
            case image_format::RGB16:   return (void)(*(pixel_rgb16*)   (&m_data[offset]) = color);
            case image_format::RGBA16:  return (void)(*(pixel_rgba16*)  (&m_data[offset]) = color);

            case image_format::R32F:    return (void)(*(pixel_r32f*)    (&m_data[offset]) = color);
            case image_format::RG32F:   return (void)(*(pixel_rg32f*)   (&m_data[offset]) = color);
            case image_format::RGB32F:  return (void)(*(pixel_rgb32f*)  (&m_data[offset]) = color);
            case image_format::RGBA32F: return (void)(*(pixel_rgba32f*) (&m_data[offset]) = color);

            default:                    break;
        }

        return;
    }

    void fill(const color& fill_color)
    {
        for (size_type y = 0; y < m_height; ++y)
        {
            for (size_type x = 0; x < m_width; ++x)
            {
                set_pixel(x, y, fill_color);
            }
        }
    }

    void key(const color& key_color, float threshold = 0.0f)
    {
        for (size_type y = 0; y < m_height; ++y)
        {
            for (size_type x = 0; x < m_width; ++x)
            {
                color c(get_pixel(x, y));
                if (math::is_equal_approx(c, key_color, threshold))
                {
                    c.a = 0.0f;
                    set_pixel(x, y, c);
                }
            }
        }
    }

    // =============== conversion ===============

    void convert(image_format format)
    {
        if (format == m_format)
        {
            return;
        }

        image converted(m_width, m_height, format);

        for (size_type y = 0; y < m_height; ++y)
        {
            for (size_type x = 0; x < m_width; ++x)
            {
                converted.set_pixel(x, y, get_pixel(x, y));
            }
        }

        *this = converted;
    }

    bool reinterpret(image_format format)
    {
        image_info info = get_info();

        if (reinterpret_info(info, format) && get_image_info_error(info) == error_code::NONE)
        {
            m_width = info.width;
            m_height = info.height;
            m_format = format;

            return true;
        }

        return false;
    }

    bool reinterpret(size_type width, size_type height, image_format format)
    {
        image_info info{ width, height, format };

        if (get_image_info_error(info) == error_code::NONE && info.size() == static_cast<size_type>(m_data.size()))
        {
            m_width = width;
            m_height = height;
            m_format = format;

            return true;
        }

        return false;
    }

    // =============== iterator ===============

    template <typename T>
    auto begin() noexcept
    {
        return detail::iterator<T>((T*)m_data.data(), 0, 0, m_width, m_height);
    }

    template <typename T>
    auto begin() const noexcept
    {
        return cbegin<T>();
    }

    template <typename T>
    auto end() noexcept
    {
        return detail::iterator<T>((T*)(m_data.data() + m_data.size()), 0, m_height, m_width, m_height);
    }

    template <typename T>
    auto end() const noexcept
    {
        return cend<T>();
    }

    template <typename T>
    auto cbegin() const noexcept
    {
        return detail::iterator<T>((const T*)m_data.data(), 0, 0, m_width, m_height);
    }

    template <typename T>
    auto cend() const noexcept
    {
        return detail::iterator<T>((const T*)(m_data.data() + m_data.size()), 0, m_height, m_width, m_height);
    }

private:

    size_type m_width;
    size_type m_height;
    image_format m_format;
    std::vector<byte_type> m_data;

};

}
}