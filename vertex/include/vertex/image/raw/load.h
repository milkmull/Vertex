#pragma once

#define VX_IMAGE_LOAD_IMPLIMENTATION
#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

#include <vector>

#include "vertex/config.h"
#include "../image_info.h"

namespace vx {
namespace img {
namespace raw {

///////////////////////////////////////////////////////////////////////////////
/// @brief Get width, height, and format of an image file.
///
/// @param filename The path to the image file.
/// @param info Reference to an image_info object to store the retrieved
/// information.
/// 
/// @return True if the information was obtained without error, otherwise false.
///////////////////////////////////////////////////////////////////////////////
VX_NODISCARD bool get_file_info(const char* filename, image_info& info);

///////////////////////////////////////////////////////////////////////////////
/// @brief Load an image from a file.
/// 
/// 
/// @param filename The path to the image file.
/// @param info Reference to an image_info object to store information about
/// the loaded image.
/// @param data Reference to a vector to store the pixel data of the loaded
/// image.
/// @param flip_vertically_on_load Whether to flip the image vertically during
/// loading (default is false).
/// 
/// @return True if the image was loaded without error, otherwise false.
///////////////////////////////////////////////////////////////////////////////
VX_NODISCARD bool load_image(const char* filename, image_info& info, std::vector<byte_type>& data, bool flip_vertically_on_load = false);

///////////////////////////////////////////////////////////////////////////////
/// @brief Load an image from a file with a specified format.
///
/// @param filename The path to the image file.
/// @param info Reference to an image_info object to store information about
/// the loaded image.
/// @param target_format The target image format to reinterpret the loaded
/// image to.
/// @param data Reference to a vector to store the pixel data of the loaded
/// image.
/// @param flip_vertically_on_load Whether to flip the image vertically
/// during loading (default is false).
/// 
/// @return True if the image was loaded without error, otherwise false.
///////////////////////////////////////////////////////////////////////////////
VX_NODISCARD bool load_image(const char* filename, image_info& info, image_format target_format, std::vector<byte_type>& data, bool flip_vertically_on_load = false);

}
}
}

#endif