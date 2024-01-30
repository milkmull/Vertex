#pragma once

#include "detail/base_type_defs.h"

namespace vx {
namespace img {

enum class image_format
{
    UNKNOWN = 0,

    R8      = 1,
    RG8     = 2,
    RGB8    = 3,
    RGBA8   = 4,

    R16     = 5,
    RG16    = 6,
    RGB16   = 7,
    RGBA16  = 8,

    R32F    = 9,
    RG32F   = 10,
    RGB32F  = 11,
    RGBA32F = 12
};

struct image_format_data
{
    size_type channels;
    size_type bitdepth;
    size_type pixel_size;
    bool alpha;
    image_format format_8_bit;
};

inline constexpr image_format_data image_format_data_table[] = {
    { 0, 0, 0, false, image_format::UNKNOWN },  // UNKNOWN

    { 1, 8, 1, false, image_format::R8 },       // R8
    { 2, 8, 2, false, image_format::RG8 },      // RG8
    { 3, 8, 3, false, image_format::RGB8 },     // RGB8
    { 4, 8, 4,  true, image_format::RGBA8 },    // RGBA8

    { 1, 16, 2, false, image_format::R8 },      // R16
    { 2, 16, 4, false, image_format::RG8 },     // RG16
    { 3, 16, 6, false, image_format::RGB8 },    // RGB16
    { 4, 16, 8,  true, image_format::RGBA8 },   // RGBA16

    { 1, 32, 4, false, image_format::R8 },      // R32F
    { 2, 32, 8, false, image_format::RG8 },     // RG32F
    { 3, 32, 12, false, image_format::RGB8 },   // RGB32F
    { 4, 32, 16,  true, image_format::RGBA8 }   // RGBA32F
};

inline constexpr size_type get_channel_count(image_format format)
{
    return image_format_data_table[static_cast<int>(format)].channels;
}

// Returns the per-channel bitdepth for a given image format
inline constexpr size_type get_bitdepth(image_format format)
{
    return image_format_data_table[static_cast<int>(format)].bitdepth;
}

// Returns the pixel size in bytes for a given image format
inline constexpr size_type get_pixel_size(image_format format)
{
    return image_format_data_table[static_cast<int>(format)].pixel_size;
}

// Converts an image format to an equivalent 8-bit format
inline constexpr image_format to_8_bit(image_format format)
{
    return image_format_data_table[static_cast<int>(format)].format_8_bit;
}

// Checks if an image format has an alpha channel
inline constexpr bool has_alpha(image_format format)
{
    return image_format_data_table[static_cast<int>(format)].alpha;
}

// Chooses an appropriate image format based on the number of channels
inline constexpr image_format choose_format(size_type channels)
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
}