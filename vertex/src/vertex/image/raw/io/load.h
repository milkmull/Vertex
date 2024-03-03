#pragma once

#define VX_IMAGE_LOAD_IMPLIMENTATION
#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

#include <vector>

#include "../../image_info.h"
#include "../../enum_error_code.h"

namespace vx {
namespace img {
namespace raw {

/**
 * @brief Get information about an image file.
 *
 * Retrieves information about the specified image file, such as its width, height, and format.
 *
 * @param filename The path to the image file.
 * @param info Reference to an image_info object to store the retrieved information.
 * @return An error_code indicating the success or failure of the operation.
 */
error_code get_file_info(const char* filename, image_info& info);

/**
 * @brief Load an image from a file.
 *
 * Loads an image from the specified file into the provided image_info and data vectors.
 *
 * @param filename The path to the image file.
 * @param info Reference to an image_info object to store information about the loaded image.
 * @param data Reference to a vector to store the pixel data of the loaded image.
 * @param flip_vertically_on_load Whether to flip the image vertically during loading (default is false).
 * @return An error_code indicating the success or failure of the operation.
 */
error_code load_image(const char* filename, image_info& info, std::vector<byte_type>& data, bool flip_vertically_on_load = false);

/**
 * @brief Load an image from a file with a specified format.
 *
 * Loads an image from the specified file into the provided image_info and data vectors with the specified format.
 *
 * @param filename The path to the image file.
 * @param info Reference to an image_info object to store information about the loaded image.
 * @param target_format The target image format to reinterpret the loaded image to.
 * @param data Reference to a vector to store the pixel data of the loaded image.
 * @param flip_vertically_on_load Whether to flip the image vertically during loading (default is false).
 * @return An error_code indicating the success or failure of the operation.
 */
error_code load_image(const char* filename, image_info& info, image_format target_format, std::vector<byte_type>& data, bool flip_vertically_on_load = false);

}
}
}

#endif