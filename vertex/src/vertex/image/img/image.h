#pragma once

#include <cassert>
#include <utility>

#include "image_info.h"
#include "image_load.h"
#include "image_write.h"

#include "detail/pixel.h"

#include "vertex/math/math/detail/vec2i_type.h"
#include "vertex/math/geometry/detail/recti_type.h"
#include "vertex/math/color/detail/colorf_type.h"

namespace vx {
namespace img {

class image
{
public:

    // =============== constructors ===============

    image()
        : m_info{ 0, 0, image_format::RGB8 } {}

    image(const image_info& info)
        : m_info(info.make_safe()), m_data(m_info.size())
    {
        std::fill(m_data.begin(), m_data.end(), 0);
    }

    image(size_type width, size_type height, image_format format)
        : image(image_info{ width, height, format }) {}

    image(const byte_type* data, const image_info& info)
        : m_info(info.make_safe()), m_data(data, data + m_info.size()) {}

    image(const byte_type* data, size_type width, size_type height, image_format format)
        : image(data, image_info{ width, height, format }) {}

    image(const char* path, error_code& err)
    {
        err = load_image(path, m_info, m_data);

        if (err != error_code::NONE)
        {
            m_info = m_info.make_safe();

            if (m_info.size() != m_data.size())
            {
                m_data.resize(m_info.size());
            }
        }

        assert(m_info.size() == m_data.size());
    }

    image(const image& i)
        : m_info(i.m_info), m_data(i.m_data) {}

    image(image&& i) noexcept
        : m_info(std::move(i.m_info)), m_data(std::move(i.m_data)) {}

    // =============== destructor ===============

    ~image() = default;

    // =============== assignment operators ===============

    image& operator=(const image& i)
    {
        m_info = i.m_info;
        m_data = i.m_data;
        return *this;
    }

    image& operator=(image&& i) noexcept
    {
        m_info = std::move(i.m_info);
        m_data = std::move(i.m_data);
        return *this;
    }

    // =============== info ===============

    inline const image_info& get_info() const { return m_info; }

    inline size_type width() const { return m_info.width; }
    inline size_type height() const { return m_info.height; }
    inline image_format format() const { return m_info.format; }

    inline size_type channels() const { return m_info.channels(); }
    inline size_type bitdepth() const { return m_info.bitdepth(); }
    inline bool has_alpha() const { return m_info.has_alpha(); }

    inline size_type pixel_size() const { return m_info.pixel_size(); }
    inline size_type pitch() const { return m_info.pitch(); }

    inline size_t length() const { return m_data.size(); }
    inline bool empty() const { return m_data.empty(); }

    inline const std::vector<byte_type> data() const { return m_data; }
    inline const byte_type* raw_data() const { return m_data.data(); }

    inline vec2ui size() const { return vec2ui(m_info.width, m_info.height); }
    inline rect2ui get_rect() const { return rect2ui(0, 0, m_info.width, m_info.height); }

    // =============== pixel ===============

    color get_pixel(size_type x, size_type y) const
    {
        const size_type offset = (m_info.width * y + x) * m_info.pixel_size();

        if (offset >= m_data.size())
        {
            return color();
        }

        switch (m_info.format)
        {
            case image_format::R8:		return static_cast<color>(*(detail::pixel_r8*)      (&m_data[offset]));
            case image_format::RG8:		return static_cast<color>(*(detail::pixel_rg8*)     (&m_data[offset]));
            case image_format::RGB8:	return static_cast<color>(*(detail::pixel_rgb8*)    (&m_data[offset]));
            case image_format::RGBA8:	return static_cast<color>(*(detail::pixel_rgba8*)   (&m_data[offset]));
                                                                                                           
            case image_format::R16:		return static_cast<color>(*(detail::pixel_r16*)     (&m_data[offset]));
            case image_format::RG16:	return static_cast<color>(*(detail::pixel_rg16*)    (&m_data[offset]));
            case image_format::RGB16:	return static_cast<color>(*(detail::pixel_rgb16*)   (&m_data[offset]));
            case image_format::RGBA16:	return static_cast<color>(*(detail::pixel_rgba16*)  (&m_data[offset]));
                                                                                                           
            case image_format::R32F:	return static_cast<color>(*(detail::pixel_r32f*)    (&m_data[offset]));
            case image_format::RG32F:	return static_cast<color>(*(detail::pixel_rg32f*)   (&m_data[offset]));
            case image_format::RGB32F:	return static_cast<color>(*(detail::pixel_rgb32f*)  (&m_data[offset]));
            case image_format::RGBA32F:	return static_cast<color>(*(detail::pixel_rgba32f*) (&m_data[offset]));

            default:					break;
        }

        return color();
    }

    void set_pixel(size_type x, size_type y, const color& color)
    {
        const size_type offset = (m_info.width * y + x) * m_info.pixel_size();

        if (offset >= m_data.size())
        {
            return;
        }

        switch (m_info.format)
        {
            case image_format::R8:
            case image_format::RG8:
            case image_format::RGB8:
            case image_format::RGBA8:

                *(detail::pixel_rgba8*)(&m_data[offset]) = detail::pixel_rgba8(color);
                return;

            case image_format::R16:
            case image_format::RG16:
            case image_format::RGB16:
            case image_format::RGBA16:

                *(detail::pixel_rgba16*)(&m_data[offset]) = detail::pixel_rgba16(color);
                return;

            case image_format::R32F:
            case image_format::RG32F:
            case image_format::RGB32F:
            case image_format::RGBA32F:

                *(detail::pixel_rgba32f*)(&m_data[offset]) = color;
                return;

            default:

                return;
        }
    }

    void fill(const color& c)
    {
        for (size_type y = 0; y < m_info.height; ++y)
        {
            for (size_type x = 0; x < m_info.width; ++x)
            {
                set_pixel(x, y, c);
            }
        }
    }

private:

    image_info m_info;
    std::vector<byte_type> m_data;

};

}
}