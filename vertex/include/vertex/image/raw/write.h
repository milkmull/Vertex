#pragma once

#define VX_IMAGE_WRITE_IMPLIMENTATION
#ifdef VX_IMAGE_WRITE_IMPLIMENTATION

#include <vector>

#include "vertex/config.h"
#include "../image_info.h"

namespace vx {
namespace img {
namespace raw {

///////////////////////////////////////////////////////////////////////////////
/// @brief Write image data to a BMP file.
///
/// @param filename The path to the BMP file to write.
/// @param info Information about the image to be written.
/// @param data Pointer to the pixel data of the image.
/// @param flip_vertically_on_write Whether to flip the image vertically
/// during writing (default is false).
/// 
/// @return True if the image was written without error, otherwise false.
///////////////////////////////////////////////////////////////////////////////
VX_NODISCARD bool write_bmp(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);

///////////////////////////////////////////////////////////////////////////////
/// @brief Write image data to a JPEG file.
///
/// @param filename The path to the JPEG file to write.
/// @param info Information about the image to be written.
/// @param data Pointer to the pixel data of the image.
/// @param quality The quality of the JPEG compression (default is 75).
/// @param flip_vertically_on_write Whether to flip the image vertically
/// during writing (default is false).
/// 
/// @return True if the image was written without error, otherwise false.
///////////////////////////////////////////////////////////////////////////////
VX_NODISCARD bool write_jpg(const char* filename, const image_info& info, const byte_type* data, int quality = 75, bool flip_vertically_on_write = false);

///////////////////////////////////////////////////////////////////////////////
/// @brief Write image data to a PNG file.
///
/// @param filename The path to the PNG file to write.
/// @param info Information about the image to be written.
/// @param data Pointer to the pixel data of the image.
/// @param flip_vertically_on_write Whether to flip the image vertically
/// during writing (default is false).
/// 
/// @return True if the image was written without error, otherwise false.
///////////////////////////////////////////////////////////////////////////////
VX_NODISCARD bool write_png(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);

///////////////////////////////////////////////////////////////////////////////
/// @brief Write image data to a TGA file.
///
/// @param filename The path to the TGA file to write.
/// @param info Information about the image to be written.
/// @param data Pointer to the pixel data of the image.
/// @param flip_vertically_on_write Whether to flip the image vertically
/// during writing (default is false).
/// 
/// @return True if the image was written without error, otherwise false.
///////////////////////////////////////////////////////////////////////////////
VX_NODISCARD bool write_tga(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);

}
}
}

#endif