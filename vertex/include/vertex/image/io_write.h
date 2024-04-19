#pragma once

#include "image.h"
#include "raw/write.h"

namespace vx {
namespace img {

///////////////////////////////////////////////////////////////////////////////
/// @brief Write the image to a BMP file.
///
/// @param filename The path to the BMP file to write.
/// @param img The image object to write.
/// 
/// @return True if the image was written without error, otherwise false.
///////////////////////////////////////////////////////////////////////////////
inline bool write_bmp(const std::string& filename, const image& img)
{
    return raw::write_bmp(filename.c_str(), img.get_info(), img.data());
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Write the image to a JPEG file.
///
/// @param filename The path to the JPEG file to write.
/// @param img The image object to write.
/// @param quality The quality of the JPEG compression (default is 75).
/// 
/// @return True if the image was written without error, otherwise false.
///////////////////////////////////////////////////////////////////////////////
inline bool write_jpg(const std::string& filename, const image& img, int quality = 75)
{
    return raw::write_jpg(filename.c_str(), img.get_info(), img.data(), quality);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Write the image to a PNG file.
///
/// @param filename The path to the PNG file to write.
/// @param img The image object to write.
/// 
/// @return True if the image was written without error, otherwise false.
//////////////////////////////////////////////////////////////////////////////
inline bool write_png(const std::string& filename, const image& img)
{
    return raw::write_png(filename.c_str(), img.get_info(), img.data());
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Write the image to a TGA file.
///
/// @param filename The path to the TGA file to write.
/// @param img The image object to write.
/// 
/// @return True if the image was written without error, otherwise false.
///////////////////////////////////////////////////////////////////////////////
inline bool write_tga(const std::string& filename, const image& img)
{
    return raw::write_tga(filename.c_str(), img.get_info(), img.data());
}

}
}