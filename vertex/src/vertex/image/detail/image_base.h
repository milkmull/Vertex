#pragma once

#include <string>

#include "vertex/math/math/vec2.h"
#include "vertex/math/color/color.h"
#include "vertex/tools/detail/enum_bit_operators.h"

namespace vx {
namespace img {

enum class image_format
{
    UNKNOWN = 0,

    R8, RG8, RGB8, RGBA8,
    R16, RG16, RGB16, RGBA16,
    R32F, RG32F, RGB32F, RGBA32F
};

enum class image_filter
{
    NEAREST,
    LINEAR,
    CUBIC
};

enum class image_wrap
{
    REPEAT,
    MIRRORED_REPEAT,
    EDGE_CLAMP,
    BORDER_CLAMP,
};

namespace detail {

using image_size_type = size_t;

enum image_metrics : image_size_type
{
    VX_IMAGE_METRICS_MAX_SIZE = 16384,

    VX_IMAGE_METRICS_MIN_BIT_DEPTH = 8,
    VX_IMAGE_METRICS_MAX_BIT_DEPTH = 32,

    VX_IMAGE_METRICS_MIN_CHANNELS = 1,
    VX_IMAGE_METRICS_MAX_CHANNELS = 4
};

enum class image_metic_error_flags : image_size_type
{
    NONE          = 0,

    MAX_WIDTH     = (1 << 0),
    MAX_HEIGHT    = (1 << 1),

    MIN_CHANNELS  = (1 << 2),
    MAX_CHANNELS  = (1 << 3),

    MIN_BIT_DEPTH = (1 << 4),
    MAX_BIT_DEPTH = (1 << 5)
};

// Checks if image metrics (size, channels, bitdepth) are within acceptable limits
inline image_metic_error_flags check_metrics(
    detail::image_size_type width,
    detail::image_size_type height,
    detail::image_size_type channels,
    detail::image_size_type bitdepth
)
{
    image_metic_error_flags errors = image_metic_error_flags::NONE;

    if (width > VX_IMAGE_METRICS_MAX_SIZE)
    {
        errors |= image_metic_error_flags::MAX_WIDTH;
    }
    if (height > VX_IMAGE_METRICS_MAX_SIZE)
    {
        errors |= image_metic_error_flags::MAX_HEIGHT;
    }
    if (channels < VX_IMAGE_METRICS_MIN_CHANNELS)
    {
        errors |= image_metic_error_flags::MIN_CHANNELS;
    }
    if (channels > VX_IMAGE_METRICS_MAX_CHANNELS)
    {
        errors |= image_metic_error_flags::MAX_CHANNELS;
    }
    if (bitdepth < VX_IMAGE_METRICS_MIN_BIT_DEPTH)
    {
        errors |= image_metic_error_flags::MIN_BIT_DEPTH;
    }
    if (bitdepth > VX_IMAGE_METRICS_MAX_BIT_DEPTH)
    {
        errors |= image_metic_error_flags::MAX_BIT_DEPTH;
    }

    return errors;
}

// Returns the number for a given image format
inline constexpr image_size_type get_channel_count(image_format format)
{
    switch (format)
    {
        case image_format::R8:			return 1;
        case image_format::RG8:			return 2;
        case image_format::RGB8:		return 3;
        case image_format::RGBA8:		return 4;

        case image_format::R16:			return 1;
        case image_format::RG16:		return 2;
        case image_format::RGB16:		return 3;
        case image_format::RGBA16:		return 4;

        case image_format::R32F:		return 1;
        case image_format::RG32F:		return 2;
        case image_format::RGB32F:		return 3;
        case image_format::RGBA32F:		return 4;

        default:						break;
    }

    return 0;
}

// Returns the per-channel bitdepth for a given image format
inline constexpr image_size_type get_bitdepth(image_format format)
{
    switch (format)
    {
        case image_format::R8:
        case image_format::RG8:
        case image_format::RGB8:
        case image_format::RGBA8:		return 8;

        case image_format::R16:
        case image_format::RG16:
        case image_format::RGB16:
        case image_format::RGBA16:		return 16;

        case image_format::R32F:
        case image_format::RG32F:
        case image_format::RGB32F:
        case image_format::RGBA32F:		return 32;

        default:						break;
    }

    return 0;
}

// Returns the pixel size in bytes for a given image format
inline constexpr image_size_type get_pixel_size(image_format format)
{
    switch (format)
    {
        case image_format::R8:			return 1;
        case image_format::RG8:			return 2;
        case image_format::RGB8:		return 3;
        case image_format::RGBA8:		return 4;

        case image_format::R16:			return 2;
        case image_format::RG16:		return 4;
        case image_format::RGB16:		return 6;
        case image_format::RGBA16:		return 8;

        case image_format::R32F:		return 4;
        case image_format::RG32F:		return 8;
        case image_format::RGB32F:		return 12;
        case image_format::RGBA32F:		return 16;

        default:						break;
    }

    return 0;
}

// Converts an image format to an equivalent 8-bit format
inline constexpr image_format to_8_bit(image_format format)
{
    switch (format)
    {
        case image_format::R8:
        case image_format::RG8:
        case image_format::RGB8:
        case image_format::RGBA8:		return format;

        case image_format::R16:			return image_format::R8;
        case image_format::RG16:		return image_format::RG8;
        case image_format::RGB16:		return image_format::RGB8;
        case image_format::RGBA16:		return image_format::RGBA8;

        case image_format::R32F:		return image_format::R8;
        case image_format::RG32F:		return image_format::RG8;
        case image_format::RGB32F:		return image_format::RGB8;
        case image_format::RGBA32F:		return image_format::RGBA8;

        default:						break;
    }

    return image_format::UNKNOWN;
}

// Checks if an image format has an alpha channel
inline constexpr bool has_alpha(image_format format)
{
    switch (format)
    {
        case image_format::RGBA8:
        case image_format::RGBA16:
        case image_format::RGBA32F:		return true;

        default:						break;
    }

    return false;
}

// Chooses an appropriate image format based on the number of channels
inline constexpr image_format choose_format(image_size_type channels)
{
    switch (channels)
    {
        case 1:		return image_format::R8;
        case 2:		return image_format::RG8;
        case 3:		return image_format::RGB8;
        case 4:		return image_format::RGBA8;

        default:	break;
    }

    return image_format::UNKNOWN;
}

}

// Struct for image specifications
struct image_specs
{
    detail::image_size_type width, height;
    image_format format;

    inline constexpr detail::image_size_type channels() const { return detail::get_channel_count(format); }
    inline constexpr detail::image_size_type bitdepth() const { return detail::get_bitdepth(format); }
    inline constexpr detail::image_size_type pixelsize() const { return detail::get_pixel_size(format); }
    inline constexpr detail::image_size_type size() const { return pixelsize() * width * height; }

    // Returns a new set of image specifications with the format converted to an
    // equivalent 8-bit format. The width of the image is adjusted such that the
    // total size of the image in bytes remains the same.
    inline constexpr image_specs to_8_bit() const
    {
        const image_format newFormat = detail::to_8_bit(format);
        const detail::image_size_type newWidth = (width * pixelsize()) / detail::get_pixel_size(newFormat);
        const detail::image_size_type newHeight = height;

        return image_specs{ newWidth, newHeight, newFormat };
    }
};

}
}