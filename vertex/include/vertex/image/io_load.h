#pragma once

#include <cassert>

#include "image.h"
#include "raw/load.h"

namespace vx {
namespace img {

/**
 * @brief Load an image from a file.
 *
 * Loads an image from the specified file and returns it as an image object.
 *
 * @param filename The path to the image file to load.
 * @param err Reference to an error_code variable to store any error that occurs during loading.
 * @return An image object representing the loaded image.
 */
inline image load(const char* filename, error_code& err)
{
    image_info info;
    std::vector<byte_type> data;

    err = raw::load_image(filename, info, data);
    assert(info.size() == data.size());

    return image(data.data(), info);
}

/**
 * @brief Load an image from a file with a specified format.
 *
 * Loads an image from the specified file and returns it as an image object with the specified format.
 *
 * @param filename The path to the image file to load.
 * @param format The desired format of the loaded image.
 * @param err Reference to an error_code variable to store any error that occurs during loading.
 * @return An image object representing the loaded image with the specified format.
 */
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