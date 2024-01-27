#pragma once

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
        : m_info(info), m_data(m_info.safe_size())
    {
        validate();
        std::fill(m_data.begin(), m_data.end(), 0);
    }

    image(size_type width, size_type height, image_format format)
        : image(image_info{ width, height, format }) {}

    image(const byte_type* data, const image_info& info)
        : m_info(info), m_data(data, data + m_info.safe_size())
    {
        validate();
    }

    image(const byte_type* data, size_type width, size_type height, image_format format)
        : image(data, image_info{ width, height, format }) {}

    image(const char* path, image_format format, error_code& err)
    {
        err = load_image(path, m_info, m_data);
        
        if (result && m_info.format != image_format::UNKNOWN)
        {
            const uint32_t new_width = m_info.width * m_info.pixel_size() / get_pixel_size(format);
            const uint32_t new_height = m_info.height;
        
            reinterpret({ newWidth, newHeight, format });
        }
        else
        {
            reset();
        }
    }

private:

    void validate()
    {
        if (m_info.validate() != VX_IMAGE_SIZE_LIMIT_NONE)
        {
            reset();
        }
    }

    void reset()
    {
        m_info.width = m_info.height = 0;
        m_info.format = image_format::UNKNOWN;
        m_data.clear();
    }

private:

    image_info m_info;
    std::vector<byte_type> m_data;

};

}
}