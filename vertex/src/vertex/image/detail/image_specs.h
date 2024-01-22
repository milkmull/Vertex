#pragma once

#include "format.h"

namespace vx {
namespace img {

// Struct for image specifications
struct image_specs
{
    detail::image_size_type width, height;
    image_format format;

    inline constexpr detail::image_size_type channels() const { return get_channel_count(format); }
    inline constexpr detail::image_size_type bitdepth() const { return get_bitdepth(format); }
    inline constexpr detail::image_size_type pixelsize() const { return get_pixel_size(format); }
    inline constexpr detail::image_size_type size() const { return pixelsize() * width * height; }

    // Returns a new set of image specifications with the format converted to an
    // equivalent 8-bit format. The width of the image is adjusted such that the
    // total size of the image in bytes remains the same.
    inline constexpr image_specs to_8_bit() const
    {
        const image_format new_format = img::to_8_bit(format);
        const detail::image_size_type new_width = (width * pixelsize()) / get_pixel_size(new_format);
        const detail::image_size_type new_height = height;

        return image_specs{ new_width, new_height, new_format };
    }
};

}
}