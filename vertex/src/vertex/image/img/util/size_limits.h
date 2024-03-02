#pragma once

#include "../detail/base_types.h"
#include "../error_code.h"

namespace vx {
namespace img {
namespace util {

#define VX_MAX_IMAGE_DIMENSIONS (1ull << 12) // 4096

enum : size_t
{
    VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS = VX_MAX_IMAGE_DIMENSIONS,

    VX_IMAGE_SIZE_LIMIT_MIN_CHANNELS = 1,
    VX_IMAGE_SIZE_LIMIT_MAX_CHANNELS = 4,

    VX_IMAGE_SIZE_LIMIT_MIN_BITDEPTH = 8,
    VX_IMAGE_SIZE_LIMIT_MAX_BITDEPTH = 8,

    VX_IMAGE_SIZE_LIMIT_MAX_SIZE = (
        VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS *
        VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS *
        VX_IMAGE_SIZE_LIMIT_MAX_CHANNELS *
        VX_IMAGE_SIZE_LIMIT_MAX_BITDEPTH /
        8
    )
};

inline constexpr error_code check_image_size_limits(
    size_type width,
    size_type height,
    size_type channels,
    size_type bitdepth
)
{
    if (width > VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS || height > VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)
    {
        return error_code::MAX_SIZE;
    }
    if (channels < VX_IMAGE_SIZE_LIMIT_MIN_CHANNELS || channels > VX_IMAGE_SIZE_LIMIT_MAX_CHANNELS)
    {
        return error_code::UNSUPPORTED_CHANNEL_COUNT;
    }
    if (bitdepth < VX_IMAGE_SIZE_LIMIT_MIN_BITDEPTH || bitdepth > VX_IMAGE_SIZE_LIMIT_MAX_BITDEPTH)
    {
        return error_code::UNSUPPORTED_BITDEPTH;
    }

    return error_code::NONE;
}

}
}
}