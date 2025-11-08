#pragma once

#include "vertex/pixel/pixel_format.hpp"

namespace vx {
namespace img {

enum class file_format
{
    unknown = 0,
    bmp,
    jpg,
    jpeg = jpg,
    png
};

enum class pixel_format : uint32_t
{
    unknown         = static_cast<uint32_t>(pixel::pixel_format::unknown),

    r_8             = static_cast<uint32_t>(pixel::pixel_format::r_8),
    rg_8            = static_cast<uint32_t>(pixel::pixel_format::rg_8),
    rgb_8           = static_cast<uint32_t>(pixel::pixel_format::rgb_8),
    rgba_8          = static_cast<uint32_t>(pixel::pixel_format::rgba_8)
};

enum : size_t
{
    max_dimensions = 4096,

    min_channels = 1,
    max_channels = 4,

    max_bytes = (max_dimensions * max_dimensions * max_channels)
};

class image;

using byte_type = pixel::byte_type;

} // namespace img
} // namespace vx