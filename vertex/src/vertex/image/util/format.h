#pragma once

#include "../detail/base_types.h"
#include "../enum_image_format.h"

namespace vx {
namespace img {

namespace detail {

struct image_format_data
{
    size_t channels;
    size_t bitdepth;
    size_t pixel_size;
    bool alpha;
    image_format format_8_bit;
};

inline constexpr image_format_data image_format_data_table[] = {
    { 0, 0, 0, false, image_format::UNKNOWN },  // UNKNOWN

    { 1, 8, 1, false, image_format::R8 },       // R8
    { 2, 8, 2, false, image_format::RG8 },      // RG8
    { 3, 8, 3, false, image_format::RGB8 },     // RGB8
    { 4, 8, 4,  true, image_format::RGBA8 }     // RGBA8
};

} // detail

namespace util {

inline constexpr size_t get_channel_count(image_format format)
{
    return detail::image_format_data_table[static_cast<int>(format)].channels;
}

// Returns the per-channel bitdepth for a given image format
inline constexpr size_t get_bitdepth(image_format format)
{
    return detail::image_format_data_table[static_cast<int>(format)].bitdepth;
}

// Returns the pixel size in bytes for a given image format
inline constexpr size_t get_pixel_size(image_format format)
{
    return detail::image_format_data_table[static_cast<int>(format)].pixel_size;
}

// Converts an image format to an equivalent 8-bit format
inline constexpr image_format to_8_bit(image_format format)
{
    return detail::image_format_data_table[static_cast<int>(format)].format_8_bit;
}

// Checks if an image format has an alpha channel
inline constexpr bool has_alpha(image_format format)
{
    return detail::image_format_data_table[static_cast<int>(format)].alpha;
}

// Chooses an appropriate image format based on the number of channels
inline constexpr image_format choose_format(size_t channels)
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

} // util

}
}