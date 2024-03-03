#pragma once

#define VX_IMAGE_WRITE_IMPLIMENTATION
#ifdef VX_IMAGE_WRITE_IMPLIMENTATION

#include <vector>

#include "../../image_info.h"
#include "../../enum_error_code.h"

namespace vx {
namespace img {
namespace raw {

/**
 * @brief Write image data to a BMP file.
 *
 * Writes the provided image data to a BMP file with the specified filename.
 *
 * @param filename The path to the BMP file to write.
 * @param info Information about the image to be written.
 * @param data Pointer to the pixel data of the image.
 * @param flip_vertically_on_write Whether to flip the image vertically during writing.
 * @return An error_code indicating the success or failure of the operation.
 */
error_code write_bmp(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);

/**
 * @brief Write image data to a JPEG file.
 *
 * Writes the provided image data to a JPEG file with the specified filename and quality.
 *
 * @param filename The path to the JPEG file to write.
 * @param info Information about the image to be written.
 * @param data Pointer to the pixel data of the image.
 * @param quality The quality of the JPEG compression.
 * @param flip_vertically_on_write Whether to flip the image vertically during writing.
 * @return An error_code indicating the success or failure of the operation.
 */
error_code write_jpg(const char* filename, const image_info& info, const byte_type* data, int quality = 75, bool flip_vertically_on_write = false);

/**
 * @brief Write image data to a PNG file.
 *
 * Writes the provided image data to a PNG file with the specified filename.
 *
 * @param filename The path to the PNG file to write.
 * @param info Information about the image to be written.
 * @param data Pointer to the pixel data of the image.
 * @param flip_vertically_on_write Whether to flip the image vertically during writing.
 * @return An error_code indicating the success or failure of the operation.
 */
error_code write_png(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);

/**
 * @brief Write image data to a TGA file.
 *
 * Writes the provided image data to a TGA file with the specified filename.
 *
 * @param filename The path to the TGA file to write.
 * @param info Information about the image to be written.
 * @param data Pointer to the pixel data of the image.
 * @param flip_vertically_on_write Whether to flip the image vertically during writing.
 * @return An error_code indicating the success or failure of the operation.
 */
error_code write_tga(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);

}
}
}

#endif