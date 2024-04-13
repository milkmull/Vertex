#pragma once

#include "image.h"
#include "raw/write.h"

namespace vx {
namespace img {

/**
 * @brief Write the image to a BMP file.
 *
 * Writes the image data to a BMP file with the specified filename.
 *
 * @param filename The path to the BMP file to write.
 * @param img The image object to write.
 * @return An error_code indicating the success or failure of the operation.
 */
inline error_code write_bmp(const char* filename, const image& img)
{
    return raw::write_bmp(filename, img.get_info(), img.data());
}

/**
 * @brief Write the image to a JPEG file.
 *
 * Writes the image data to a JPEG file with the specified filename and quality.
 *
 * @param filename The path to the JPEG file to write.
 * @param img The image object to write.
 * @param quality The quality of the JPEG compression (default is 75).
 * @return An error_code indicating the success or failure of the operation.
 */
inline error_code write_jpg(const char* filename, const image& img, int quality = 75)
{
    return raw::write_jpg(filename, img.get_info(), img.data(), quality);
}

/**
 * @brief Write the image to a PNG file.
 *
 * Writes the image data to a PNG file with the specified filename.
 *
 * @param filename The path to the PNG file to write.
 * @param img The image object to write.
 * @return An error_code indicating the success or failure of the operation.
 */
inline error_code write_png(const char* filename, const image& img)
{
    return raw::write_png(filename, img.get_info(), img.data());
}

/**
 * @brief Write the image to a TGA file.
 *
 * Writes the image data to a TGA file with the specified filename.
 *
 * @param filename The path to the TGA file to write.
 * @param img The image object to write.
 * @return An error_code indicating the success or failure of the operation.
 */
inline error_code write_tga(const char* filename, const image& img)
{
    return raw::write_tga(filename, img.get_info(), img.data());
}

}
}