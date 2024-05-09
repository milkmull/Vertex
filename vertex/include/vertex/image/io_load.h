#pragma once

#include <cassert>

#include "image.h"
#include "raw/load.h"

namespace vx {
namespace img {

///////////////////////////////////////////////////////////////////////////////
/// @brief Load an image from a file.
/// 
/// @param filename The path to the image file to load.
/// @param status Will be set to true if the file is loaded successfully and
/// false otherwise.
/// 
/// @return The loaded image.
///////////////////////////////////////////////////////////////////////////////
inline image load(const std::string& filename, bool& status)
{
    image_info info;
    std::vector<byte_type> data;

    status = raw::load_image(filename.c_str(), info, data);
    assert(info.size() == data.size());

    return image(data.data(), info);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Load an image from a file with a specified format.
///
/// @param filename The path to the image file to load.
/// @param format The desired format of the loaded image.
/// @param status Will be set to true if the file is loaded successfully and
/// false otherwise.
/// 
/// @return The loaded image.
///////////////////////////////////////////////////////////////////////////////
inline image load(const std::string& filename, image_pixel_format format, bool& status)
{
    image_info info;
    std::vector<byte_type> data;

    status = raw::load_image(filename.c_str(), info, format, data);
    assert(info.size() == data.size());

    return image(data.data(), info);
}

}
}