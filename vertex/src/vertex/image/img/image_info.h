#pragma once

#include "util/format.h"

namespace vx {
namespace img {

struct image_info
{
    size_type width, height;
    image_format format;

    inline bool operator==(const image_info& other) const
    {
        return width == other.width && height == other.height && format == other.format;
    }

    inline bool operator!=(const image_info& other) const
    {
        return !(*this == other);
    }

    inline constexpr size_type channels() const { return util::get_channel_count(format); }
    inline constexpr size_type bitdepth() const { return util::get_bitdepth(format); }
    inline constexpr bool has_alpha() const { return util::has_alpha(format); }

    inline constexpr size_type pixel_size() const { return util::get_pixel_size(format); }
    inline constexpr size_type size() const { return pixel_size() * width * height; }
    inline constexpr size_type stride() const { return width * pixel_size(); }
};

}
}