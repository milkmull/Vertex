#pragma once

#include "detail/base_type_defs.h"
#include "vertex/tools/detail/enum_bit_operators.h"

namespace vx {
namespace img {

enum image_size_limit : size_type
{
    VX_IMAGE_SIZE_LIMIT_NONE = 0,

    VX_IMAGE_SIZE_LIMIT_MAX_SIZE = (1ull << 24),

    VX_IMAGE_SIZE_LIMIT_MIN_CHANNELS = 1,
    VX_IMAGE_SIZE_LIMIT_MAX_CHANNELS = 4,

    VX_IMAGE_SIZE_LIMIT_MIN_BITDEPTH = 8,
    VX_IMAGE_SIZE_LIMIT_MAX_BITDEPTH = 32,
};

inline constexpr image_size_limit check_image_size_limits(
    size_type width,
    size_type height,
    size_type channels,
    size_type bitdepth
)
{
    if (width * height > VX_IMAGE_SIZE_LIMIT_MAX_SIZE)
    {
        return VX_IMAGE_SIZE_LIMIT_MAX_SIZE;
    }
    if (channels < VX_IMAGE_SIZE_LIMIT_MIN_CHANNELS)
    {
        return VX_IMAGE_SIZE_LIMIT_MIN_CHANNELS;
    }
    if (channels > VX_IMAGE_SIZE_LIMIT_MAX_CHANNELS)
    {
        return VX_IMAGE_SIZE_LIMIT_MAX_CHANNELS;
    }
    if (bitdepth < VX_IMAGE_SIZE_LIMIT_MIN_BITDEPTH)
    {
        return VX_IMAGE_SIZE_LIMIT_MIN_BITDEPTH;
    }
    if (bitdepth > VX_IMAGE_SIZE_LIMIT_MAX_BITDEPTH)
    {
        return VX_IMAGE_SIZE_LIMIT_MAX_BITDEPTH;
    }

    return VX_IMAGE_SIZE_LIMIT_NONE;
}

inline constexpr const char* get_image_size_limit_error_message(image_size_limit limit)
{
    switch (limit)
    {
        case VX_IMAGE_SIZE_LIMIT_MAX_SIZE:
            return "Image size too large";

        case VX_IMAGE_SIZE_LIMIT_MIN_CHANNELS:
        case VX_IMAGE_SIZE_LIMIT_MAX_CHANNELS:
            return "Image channel count not supported";

        case VX_IMAGE_SIZE_LIMIT_MIN_BITDEPTH:
        case VX_IMAGE_SIZE_LIMIT_MAX_BITDEPTH:
            return "Image bitdepth not supported";

        default:
            break;
    }

    return "";
}

}
}