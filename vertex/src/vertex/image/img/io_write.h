#pragma once

#include "image.h"
#include "raw/io/write.h"

namespace vx {
namespace img {

inline error_code write_bmp(const char* filename, const image& img)
{
    return raw::write_bmp(filename, img.get_info(), img.data());
}

inline error_code write_jpg(const char* filename, const image& img, int quality = 75)
{
    return raw::write_jpg(filename, img.get_info(), img.data(), quality);
}

inline error_code write_png(const char* filename, const image& img)
{
    return raw::write_png(filename, img.get_info(), img.data());
}

inline error_code write_tga(const char* filename, const image& img)
{
    return raw::write_tga(filename, img.get_info(), img.data());
}

}
}