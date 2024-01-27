#pragma once

#include <cassert>
#include <utility>

#include "image_info.h"
#include "image_load.h"
#include "image_write.h"

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
    inline size_type pixel_size() const { return m_info.pixel_size(); }

    inline const std::vector<byte_type> data() const { return m_data; }

private:

    image_info m_info;
    std::vector<byte_type> m_data;

};

}
}