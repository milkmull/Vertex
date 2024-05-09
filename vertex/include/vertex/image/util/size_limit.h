#pragma once

#include "../_priv/base_types.h"
#include "error.h"

namespace vx {
namespace img {
namespace util {

#define VX_MAX_IMAGE_DIMENSIONS (1ull << 12) // 4096

enum : size_t
{
    IMAGE_SIZE_LIMIT_MAX_DIMENSIONS = VX_MAX_IMAGE_DIMENSIONS,

    IMAGE_SIZE_LIMIT_MIN_CHANNELS = 1,
    IMAGE_SIZE_LIMIT_MAX_CHANNELS = 4,

    IMAGE_SIZE_LIMIT_MAX_SIZE = (
        IMAGE_SIZE_LIMIT_MAX_DIMENSIONS *
        IMAGE_SIZE_LIMIT_MAX_DIMENSIONS *
        IMAGE_SIZE_LIMIT_MAX_CHANNELS
    )
};

///////////////////////////////////////////////////////////////////////////////
/// @brief Check if the given image size exceeds predefined limits.
///
/// @param width The width of the image.
/// @param height The height of the image.
/// @param channels The number of channels in the image.
/// 
/// @return The error code indicating if the image size exceeds limits or not.
///////////////////////////////////////////////////////////////////////////////
inline constexpr image_error_code check_image_size_limits(
    size_t width,
    size_t height,
    size_t channels
)
{
    if (width > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS || height > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)
    {
        return image_error_code::MAX_SIZE;
    }
    if (channels < IMAGE_SIZE_LIMIT_MIN_CHANNELS || channels > IMAGE_SIZE_LIMIT_MAX_CHANNELS)
    {
        return image_error_code::UNSUPPORTED_CHANNEL_COUNT;
    }

    return image_error_code::NONE;
}

}
}
}