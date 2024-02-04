#pragma once

#include "image_size_limit.h"
#include "image_format.h"

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

namespace util {

inline constexpr error_code get_image_info_error(const image_info& info)
{
    if (info.format == image_format::UNKNOWN)
    {
        return error_code::UNSUPPORTED_IMAGE_TYPE;
    }
    if (info.width > VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS || info.height > VX_IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)
    {
        return error_code::MAX_SIZE;
    }
    return error_code::NONE;
}

inline constexpr image_info get_8_bit_info(const image_info& info)
{
    const image_format format = to_8_bit(info.format);
    const size_type width = (info.width * info.pixel_size()) / get_pixel_size(format);
    const size_type height = info.height;

    return image_info{ width, height, format };
}

inline constexpr bool reinterpret_info(image_info& info, image_format target_format)
{
    if (info.format == image_format::UNKNOWN || target_format == image_format::UNKNOWN)
    {
        return false;
    }
    if ((info.width * info.pixel_size()) % get_pixel_size(target_format) != 0)
    {
        return false;
    }

    info.width = (info.width * info.pixel_size()) / get_pixel_size(target_format);
    info.format = target_format;

    return true;
}

}
}
}