#pragma once

#include "util/format.h"

namespace vx {
namespace img {

struct image_info
{
    size_t width, height;
    image_format format;

    inline constexpr bool operator==(const image_info& other) const
    {
        return width == other.width && height == other.height && format == other.format;
    }

    inline constexpr bool operator!=(const image_info& other) const
    {
        return !(*this == other);
    }

    inline constexpr size_t channels() const { return util::get_channel_count(format); }
    inline constexpr size_t bitdepth() const { return util::get_bitdepth(format); }
    inline constexpr bool has_alpha() const { return util::has_alpha(format); }

    inline constexpr size_t pixel_size() const { return util::get_pixel_size(format); }
    inline constexpr size_t size() const { return pixel_size() * width * height; }
    inline constexpr size_t stride() const { return width * pixel_size(); }
};

}
}