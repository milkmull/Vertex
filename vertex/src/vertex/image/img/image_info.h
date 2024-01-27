#pragma once

#include "detail/base_type_defs.h"
#include "image_size_limit.h"
#include "image_format.h"

namespace vx {
namespace img {

struct image_info
{
    size_type width, height;
    image_format format;

    inline constexpr size_type channels() const { return get_channel_count(format); }
    inline constexpr size_type bitdepth() const { return get_bitdepth(format); }
    inline constexpr size_type pixel_size() const { return get_pixel_size(format); }
    inline constexpr size_type size() const { return pixel_size() * width * height; }

    // Returns a new set of image specifications with the format converted to an
    // equivalent 8-bit format. The width of the image is adjusted such that the
    // total size of the image in bytes remains the same.
    inline constexpr image_info to_8_bit() const
    {
        const image_format new_format = img::to_8_bit(format);
        const size_type new_width = (width * pixel_size()) / get_pixel_size(new_format);
        const size_type new_height = height;

        return image_info{ new_width, new_height, new_format };
    }

    inline constexpr image_info make_safe() const
    {
        const image_format new_format = (format != image_format::UNKNOWN) ? format : image_format::RGB8;
        const size_type new_width = std::min(width, static_cast<size_type>(VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS));
        const size_type new_height = std::min(height, static_cast<size_type>(VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS));
        
        return image_info{ new_width, new_height, new_format };
    }

    inline constexpr error_code get_error() const
    {
        if (format == image_format::UNKNOWN)
        {
            return error_code::UNSUPPORTED_IMAGE_TYPE;
        }
        return check_image_size_limits(width, height, channels(), bitdepth());
    }
};

}
}