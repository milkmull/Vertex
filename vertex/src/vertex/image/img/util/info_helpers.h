#pragma once

#include "size_limits.h"
#include "../image_info.h"

namespace vx {
namespace img {
namespace util {

inline constexpr error_code get_image_info_error(const image_info& info)
{
    if (info.format == image_format::UNKNOWN)
    {
        return error_code::UNSUPPORTED_IMAGE_TYPE;
    }
    if (info.width > VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS || info.height > VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)
    {
        return error_code::MAX_SIZE;
    }
    return error_code::NONE;
}

inline constexpr image_info get_8_bit_info(const image_info& info)
{
    const image_format format = to_8_bit(info.format);
    const size_t width = (info.width * info.pixel_size()) / get_pixel_size(format);
    const size_t height = info.height;

    return image_info{ width, height, format };
}

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