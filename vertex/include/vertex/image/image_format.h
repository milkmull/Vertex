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

enum image_pixel_format : uint32_t
{
    PIXEL_FORMAT_UNKNOWN = pixel::PIXEL_FORMAT_UNKNOWN,

    PIXEL_FORMAT_R_8     = pixel::PIXEL_FORMAT_R_8,
    PIXEL_FORMAT_RG_8    = pixel::PIXEL_FORMAT_RG_8,
    PIXEL_FORMAT_RGB_8   = pixel::PIXEL_FORMAT_RGB_8,
    PIXEL_FORMAT_RGBA_8  = pixel::PIXEL_FORMAT_RGBA_8
};

inline constexpr bool is_valid_pixel_format(image_pixel_format format)
{
    return format == PIXEL_FORMAT_R_8
        || format == PIXEL_FORMAT_RG_8
        || format == PIXEL_FORMAT_RGB_8
        || format == PIXEL_FORMAT_RGBA_8;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Choose the appropriate image format based on the number of color
/// channels.
///
/// @param channels The number of color channels.
/// 
/// @return The chosen image format.
///////////////////////////////////////////////////////////////////////////////
inline constexpr image_pixel_format pixel_format_from_channels(size_t channels)
{
    switch (channels)
    {
        case 1:		return PIXEL_FORMAT_R_8;
        case 2:		return PIXEL_FORMAT_RG_8;
        case 3:		return PIXEL_FORMAT_RGB_8;
        case 4:		return PIXEL_FORMAT_RGBA_8;

        default:	break;
    }

    return PIXEL_FORMAT_UNKNOWN;
}

}
}