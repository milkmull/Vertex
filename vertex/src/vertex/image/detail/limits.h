#pragma once

#include "base_type_defs.h"
#include "vertex/tools/detail/enum_bit_operators.h"

namespace vx {
namespace img {
namespace detail {

enum image_limits : detail::image_size_type
{
    VX_IMAGE_LIMITS_MAX_SIZE = 16384,

    VX_IMAGE_LIMITS_MIN_BIT_DEPTH = 8,
    VX_IMAGE_LIMITS_MAX_BIT_DEPTH = 32,

    VX_IMAGE_LIMITS_MIN_CHANNELS = 1,
    VX_IMAGE_LIMITS_MAX_CHANNELS = 4
};

enum class image_metic_error : image_size_type
{
    NONE          = 0,

    MAX_WIDTH     = (1ull << 0),
    MAX_HEIGHT    = (1ull << 1),

    MIN_CHANNELS  = (1ull << 2),
    MAX_CHANNELS  = (1ull << 3),

    MIN_BIT_DEPTH = (1ull << 4),
    MAX_BIT_DEPTH = (1ull << 5)
};

// Checks if image metrics (size, channels, bitdepth) are within acceptable limits
inline image_metic_error check_metrics(
    image_size_type width,
    image_size_type height,
    image_size_type channels,
    image_size_type bitdepth
)
{
    image_metic_error errors = image_metic_error::NONE;

    if (width > VX_IMAGE_LIMITS_MAX_SIZE)
    {
        errors |= image_metic_error::MAX_WIDTH;
    }
    if (height > VX_IMAGE_LIMITS_MAX_SIZE)
    {
        errors |= image_metic_error::MAX_HEIGHT;
    }
    if (channels < VX_IMAGE_LIMITS_MIN_CHANNELS)
    {
        errors |= image_metic_error::MIN_CHANNELS;
    }
    if (channels > VX_IMAGE_LIMITS_MAX_CHANNELS)
    {
        errors |= image_metic_error::MAX_CHANNELS;
    }
    if (bitdepth < VX_IMAGE_LIMITS_MIN_BIT_DEPTH)
    {
        errors |= image_metic_error::MIN_BIT_DEPTH;
    }
    if (bitdepth > VX_IMAGE_LIMITS_MAX_BIT_DEPTH)
    {
        errors |= image_metic_error::MAX_BIT_DEPTH;
    }

    return errors;
}

}
}
}