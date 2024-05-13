#pragma once

#include "../image_format.h"

namespace vx {
namespace img {
namespace util {

inline constexpr bool is_valid_pixel_format(image_pixel_format format)
{
    return format == image_pixel_format::PIXEL_FORMAT_R_8
        || format == image_pixel_format::PIXEL_FORMAT_RG_8
        || format == image_pixel_format::PIXEL_FORMAT_RGB_8
        || format == image_pixel_format::PIXEL_FORMAT_RGBA_8;
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
        case 1:		return image_pixel_format::PIXEL_FORMAT_R_8;
        case 2:		return image_pixel_format::PIXEL_FORMAT_RG_8;
        case 3:		return image_pixel_format::PIXEL_FORMAT_RGB_8;
        case 4:		return image_pixel_format::PIXEL_FORMAT_RGBA_8;

        default:	break;
    }

    return image_pixel_format::PIXEL_FORMAT_UNKNOWN;
}

}
}
}