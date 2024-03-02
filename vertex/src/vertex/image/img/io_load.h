#pragma once

#include <cassert>

#include "image.h"
#include "raw/io_load.h"

namespace vx {
namespace img {

inline image load(const char* filename, error_code& err)
{
    image_info info;
    std::vector<byte_type> data;

    err = raw::load_image(filename, info, data);
    assert(info.size() == data.size());

    return image(data.data(), info);
}

inline image load(const char* filename, image_format format, error_code& err)
{
    image_info info;
    std::vector<byte_type> data;

    err = raw::load_image(filename, info, format, data);
    assert(info.size() == data.size());

    return image(data.data(), info);
}

}
}