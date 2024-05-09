#pragma once

#include "size_limit.h"
#include "../image_info.h"

namespace vx {
namespace img {
namespace util {

///////////////////////////////////////////////////////////////////////////////
/// @brief Get the error code based on the provided image_info.
///
/// @param info The image_info object to check.
/// 
/// @return The error code indicating any issues with the image_info.
///////////////////////////////////////////////////////////////////////////////
inline constexpr image_error_code get_image_info_error(const image_info& info)
{
    if (!is_valid_pixel_format(info.format))
    {
        return image_error_code::UNSUPPORTED_IMAGE_FORMAT;
    }
    if (info.width > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS || info.height > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)
    {
        return image_error_code::MAX_SIZE;
    }
    return image_error_code::NONE;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Reinterpret the image_info object with a new target format.
///
/// @param info The image_info object to reinterpret.
/// @param target_format The target image format.
/// @param resize_vertically If true, the height will be stretched
/// to match the new format, otherwise the width will be stretched.
/// 
/// @return True if reinterpretation was successful, false otherwise.
///////////////////////////////////////////////////////////////////////////////
inline constexpr image_error_code reinterpret_info(image_info& info, image_pixel_format target_format, bool resize_vertically = false)
{
    if (!is_valid_pixel_format(info.format) || !is_valid_pixel_format(target_format))
    {
        return image_error_code::UNSUPPORTED_IMAGE_FORMAT;
    }

    const size_t target_pixel_size = pixel::get_pixel_size(static_cast<pixel::pixel_format>(target_format));

    if (resize_vertically)
    {
        if ((info.height * info.pixel_size()) % target_pixel_size != 0)
        {
            return image_error_code::REINTERPRETATION_ERROR;
        }

        info.height = (info.height * info.pixel_size()) / target_pixel_size;
    }
    else
    {
        if ((info.width * info.pixel_size()) % target_pixel_size != 0)
        {
            return image_error_code::REINTERPRETATION_ERROR;
        }

        info.width = (info.width * info.pixel_size()) / target_pixel_size;
    }

    info.format = target_format;

    return image_error_code::NONE;
}

}
}
}