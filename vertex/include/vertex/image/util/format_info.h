#pragma once

#include "../_priv/base_types.h"
#include "../enum_image_format.h"

namespace vx {
namespace img {

namespace _priv {

struct image_format_data
{
    size_t channels;
    size_t bitdepth;
    size_t pixel_size;
    bool alpha;
    image_format format_8_bit;
};

inline constexpr image_format_data image_format_data_table[] =
{
    { 0, 0, 0, false, image_format::UNKNOWN }, // UNKNOWN

    { 1, 8, 1, false, image_format::R8 },      // R8
    { 2, 8, 2, false, image_format::RG8 },     // RG8
    { 3, 8, 3, false, image_format::RGB8 },    // RGB8
    { 4, 8, 4,  true, image_format::RGBA8 }    // RGBA8
};

} // namespace _priv

namespace util {

///////////////////////////////////////////////////////////////////////////////
/// @brief Get the number of color channels for the given image format.
///
/// @param format The image format.
/// 
/// @return The number of color channels.
///////////////////////////////////////////////////////////////////////////////
inline constexpr size_t get_channel_count(image_format format)
{
    return _priv::image_format_data_table[static_cast<int>(format)].channels;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Get the bit depth for the given image format.
///
/// @param format The image format.
/// 
/// @return The bit depth.
///////////////////////////////////////////////////////////////////////////////
inline constexpr size_t get_bitdepth(image_format format)
{
    return _priv::image_format_data_table[static_cast<int>(format)].bitdepth;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Get the size of each pixel in bytes for the given image format.
///
/// @param format The image format.
/// 
/// @return The size of each pixel in bytes.
///////////////////////////////////////////////////////////////////////////////
inline constexpr size_t get_pixel_size(image_format format)
{
    return _priv::image_format_data_table[static_cast<int>(format)].pixel_size;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Convert the given image format to its corresponding 8-bit format.
///
/// @param format The image format.
/// 
/// @return The corresponding 8-bit image format.
///////////////////////////////////////////////////////////////////////////////
inline constexpr image_format to_8_bit(image_format format)
{
    return _priv::image_format_data_table[static_cast<int>(format)].format_8_bit;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Check if the given image format has an alpha channel.
///
/// @param format The image format.
/// 
/// @return True if the image format has an alpha channel, false otherwise.
///////////////////////////////////////////////////////////////////////////////
inline constexpr bool has_alpha(image_format format)
{
    return _priv::image_format_data_table[static_cast<int>(format)].alpha;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Choose the appropriate image format based on the number of color
/// channels.
///
/// @param channels The number of color channels.
/// 
/// @return The chosen image format.
///////////////////////////////////////////////////////////////////////////////
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

} // namespace util

}
}