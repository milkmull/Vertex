#pragma once

#include "util/image_format_helpers.h"

namespace vx {
namespace img {

struct image_info
{
    size_t width, height;
    image_pixel_format format;

    inline constexpr bool operator==(const image_info& other) const
    {
        return width == other.width && height == other.height && format == other.format;
    }

    inline constexpr bool operator!=(const image_info& other) const
    {
        return !(*this == other);
    }

    inline constexpr bool is_valid_format() const { return util::is_valid_pixel_format(format); }

    inline constexpr size_t channels() const { return pixel::get_channel_count(static_cast<pixel::pixel_format>(format)); }
    inline constexpr bool has_alpha() const { return pixel::has_alpha(static_cast<pixel::pixel_format>(format)); }
    inline constexpr size_t pixel_size() const { return pixel::get_pixel_size(static_cast<pixel::pixel_format>(format)); }

    inline constexpr size_t size() const { return pixel_size() * width * height; }
    inline constexpr size_t stride() const { return width * pixel_size(); }
};

}
}