#pragma once

#include "vertex/pixel/pixel_format.hpp"

namespace vx {
namespace img {

enum class file_format
{
    UNKNOWN = 0,
    BMP,
    JPG,
    JPEG = JPG,
    PNG
};

enum class pixel_format : uint32_t
{
    UNKNOWN         = static_cast<uint32_t>(pixel::pixel_format::UNKNOWN),

    R_8             = static_cast<uint32_t>(pixel::pixel_format::R_8),
    RG_8            = static_cast<uint32_t>(pixel::pixel_format::RG_8),
    RGB_8           = static_cast<uint32_t>(pixel::pixel_format::RGB_8),
    RGBA_8          = static_cast<uint32_t>(pixel::pixel_format::RGBA_8)
};

enum : size_t
{
    IMAGE_SIZE_LIMIT_MAX_DIMENSIONS = 4096,

    IMAGE_SIZE_LIMIT_MIN_CHANNELS = 1,
    IMAGE_SIZE_LIMIT_MAX_CHANNELS = 4,

    IMAGE_SIZE_LIMIT_MAX_BYTES = (
        IMAGE_SIZE_LIMIT_MAX_DIMENSIONS *
        IMAGE_SIZE_LIMIT_MAX_DIMENSIONS *
        IMAGE_SIZE_LIMIT_MAX_CHANNELS
    )
};

class image;

using byte_type = pixel::byte_type;

} // namespace img
} // namespace vx