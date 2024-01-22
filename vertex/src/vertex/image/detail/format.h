#pragma once

#include "base_type_defs.h"

namespace vx {

enum class image_format
{
    UNKNOWN = 0,

    R8, RG8, RGB8, RGBA8,
    R16, RG16, RGB16, RGBA16,
    R32F, RG32F, RGB32F, RGBA32F
};

namespace img {

inline constexpr detail::image_size_type get_channel_count(image_format format)
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
inline constexpr detail::image_size_type get_bitdepth(image_format format)
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
inline constexpr detail::image_size_type get_pixel_size(image_format format)
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
inline constexpr image_format choose_format(detail::image_size_type channels)
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

namespace detail {

// =============== channel type selector ===============

template <image_format f>
struct format_traits_base
{
    static constexpr image_format format = f;
    static constexpr image_format format_8_bit = to_8_bit(f);

    static constexpr image_size_type channel_count = get_channel_count(f);
    static constexpr image_size_type bitdepth = get_bitdepth(f);
    static constexpr image_size_type pixel_size = get_pixel_size(f);

    static constexpr bool alpha = has_alpha(f);

};

template <image_format format>
struct format_traits;

template <>
struct format_traits<image_format::R8> : public format_traits_base<image_format::R8>
{
    using channel_type = uint8_t;
};

template <>
struct format_traits<image_format::RG8> : public format_traits_base<image_format::RG8>
{
    using channel_type = uint8_t;
};

template <>
struct format_traits<image_format::RGB8> : public format_traits_base<image_format::RGB8>
{
    using channel_type = uint8_t;
};

template <>
struct format_traits<image_format::RGBA8> : public format_traits_base<image_format::RGBA8>
{
    using channel_type = uint8_t;
};

template <>
struct format_traits<image_format::R16> : public format_traits_base<image_format::R16>
{
    using channel_type = uint16_t;
};

template <>
struct format_traits<image_format::RG16> : public format_traits_base<image_format::RG16>
{
    using channel_type = uint16_t;
};

template <>
struct format_traits<image_format::RGB16> : public format_traits_base<image_format::RGB16>
{
    using channel_type = uint16_t;
};

template <>
struct format_traits<image_format::RGBA16> : public format_traits_base<image_format::RGBA16>
{
    using channel_type = uint16_t;
};

template <>
struct format_traits<image_format::R32F> : public format_traits_base<image_format::R32F>
{
    using channel_type = float;
};

template <>
struct format_traits<image_format::RG32F> : public format_traits_base<image_format::RG32F>
{
    using channel_type = float;
};

template <>
struct format_traits<image_format::RGB32F> : public format_traits_base<image_format::RGB32F>
{
    using channel_type = float;
};

template <>
struct format_traits<image_format::RGBA32F> : public format_traits_base<image_format::RGBA32F>
{
    using channel_type = float;
};

}
}
}