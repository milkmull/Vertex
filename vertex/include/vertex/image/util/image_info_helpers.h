#pragma once

#include "size_limit.h"
#include "../image_info.h"

namespace vx {
namespace img {
namespace util {

/**
 * @brief Get the error code based on the provided image_info.
 *
 * @param info The image_info object to check.
 * @return The error code indicating any issues with the image_info.
 */
inline constexpr error_code get_image_info_error(const image_info& info)
{
    if (info.format == image_format::UNKNOWN)
    {
        return error_code::UNSUPPORTED_IMAGE_TYPE;
    }
    if (info.width > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS || info.height > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)
    {
        return error_code::MAX_SIZE;
    }
    return error_code::NONE;
}

/**
 * @brief Get the image_info object for an 8-bit representation of the provided image_info.
 *
 * @param info The original image_info object.
 * @return The image_info object representing the 8-bit version of the original image.
 */
inline constexpr image_info get_8_bit_info(const image_info& info)
{
    const image_format format = to_8_bit(info.format);
    const size_t width = (info.width * info.pixel_size()) / get_pixel_size(format);
    const size_t height = info.height;

    return image_info{ width, height, format };
}

/**
 * @brief Reinterpret the image_info object with a new target format.
 *
 * @param info The image_info object to reinterpret.
 * @param target_format The target image format.
 * @return True if reinterpretation was successful, false otherwise.
 */
inline constexpr bool reinterpret_info(image_info& info, image_format target_format)
{
    if (info.format == image_format::UNKNOWN || target_format == image_format::UNKNOWN)
    {
        return false;
    }
    if ((info.width * info.pixel_size()) % get_pixel_size(target_format) != 0)
    {
        return false;
    }

    info.width = (info.width * info.pixel_size()) / get_pixel_size(target_format);
    info.format = target_format;

    return true;
}

}
}
}