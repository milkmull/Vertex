#pragma once

#include "vertex/pixel/pixel.h"

namespace vx {
namespace img {

enum class image_file_format
{
    UNKNOWN = 0,

    JPEG,
    JPG = JPEG,
    PNG,
    BMP,
    TGA
};

enum class image_pixel_format : uint32_t
{
    PIXEL_FORMAT_UNKNOWN = pixel::pixel_format::PIXEL_FORMAT_UNKNOWN,

    PIXEL_FORMAT_R_8     = pixel::pixel_format::PIXEL_FORMAT_R_8,
    PIXEL_FORMAT_RG_8    = pixel::pixel_format::PIXEL_FORMAT_RG_8,
    PIXEL_FORMAT_RGB_8   = pixel::pixel_format::PIXEL_FORMAT_RGB_8,
    PIXEL_FORMAT_RGBA_8  = pixel::pixel_format::PIXEL_FORMAT_RGBA_8
};

}
}