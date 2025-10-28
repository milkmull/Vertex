#pragma once

#include <cstdint>
#include <array>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace pixel {

using byte_type = uint8_t;

// https://docs.vulkan.org/spec/latest/chapters/formats.html

enum class pixel_type : uint32_t
{
    NONE = 0,
                  
    PACKED_8,
    PACKED_16,
    PACKED_32,
    UINT_ARRAY,
    FLOAT_ARRAY 
};

enum class pixel_channel_order : uint32_t
{
    NONE = 0,

    R,
    RG,
    RGB,
    BGR,
    RGBX,
    BGRX,
    RGBA,
    BGRA,
    XRGB,
    XBGR,
    ARGB,
    ABGR 
};

enum class pixel_pack_layout : uint32_t
{
    NONE = 0,
       
    _332,
    _4444,
    _565,
    _5551,
    _1555,
    _8888,
    _2101010
};

///////////////////////////////////////////////////////////////////////////////
/// @brief Creates a pixel format.
///
/// This function creates a pixel format by combining various parameters into a
/// single uint32_t value.
///
/// @param type The type of pixel.
/// @param channel_order The order of pixel channels.
/// @param pack_layout The layout of pixel packing.
/// @param channel_count The number of channels in the pixel.
/// @param pixel_size The size of the pixel in bytes.
/// @param alpha Indicates if the pixel has an alpha channel.
///
/// @return The created pixel format as a uint32_t value.
///////////////////////////////////////////////////////////////////////////////
inline constexpr uint32_t create_pixel_format(
    pixel_type type,
    pixel_channel_order channel_order,
    pixel_pack_layout pack_layout,
    uint32_t channel_count,
    uint32_t pixel_size,
    bool alpha
) noexcept
{
    return (static_cast<uint32_t>(type)          << 28) | // type         = 4 bits
           (static_cast<uint32_t>(channel_order) << 24) | // order        = 4 bits
           (static_cast<uint32_t>(pack_layout)   << 20) | // layout       = 4 bits
           (channel_count                        << 16) | // channel_cout = 4 bits
           (static_cast<uint32_t>(alpha)         << 11) | // alpha        = 1 bit
           (pixel_size                           <<  0);  // size         = 11 bits
}

enum class pixel_format : uint32_t
{
    ///////////////////////////////////////////////////////////////////////////////

    UNKNOWN = create_pixel_format(
        pixel_type::NONE,
        pixel_channel_order::NONE,
        pixel_pack_layout::NONE,
        0, 0, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    RGB_332 = create_pixel_format(
         pixel_type::PACKED_8,
         pixel_channel_order::RGB,
         pixel_pack_layout::_332,
         3, 1, false
     ),
    
    ///////////////////////////////////////////////////////////////////////////////

    RGBA_4444 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::RGBA, 
        pixel_pack_layout::_4444,
        4, 2, true
    ),

    BGRA_4444 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::BGRA,
        pixel_pack_layout::_4444,
        4, 2, true
    ),

    XRGB_4444 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::XRGB,
        pixel_pack_layout::_4444,
        4, 2, false
    ),

    XBGR_4444 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::XBGR,
        pixel_pack_layout::_4444,
        4, 2, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    RGB_565 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::RGB,
        pixel_pack_layout::_565,
        3, 2, false
    ),

    BGR_565 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::BGR,
        pixel_pack_layout::_565,
        3, 2, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    RGBA_5551 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::RGBA,
        pixel_pack_layout::_5551,
        4, 2, true
    ),

    BGRA_5551 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::BGRA,
        pixel_pack_layout::_5551,
        4, 2, true
    ),

    ARGB_1555 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::ARGB,
        pixel_pack_layout::_1555,
        4, 2, true
    ),

    XRGB_1555 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::XRGB,
        pixel_pack_layout::_1555,
        4, 2, false
    ),

    XBGR_1555 = create_pixel_format(
        pixel_type::PACKED_16,
        pixel_channel_order::XBGR,
        pixel_pack_layout::_1555,
        4, 2, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    R_8 = create_pixel_format(
        pixel_type::UINT_ARRAY,
        pixel_channel_order::R,
        pixel_pack_layout::NONE,
        1, 1, false
    ),

    RG_8 = create_pixel_format(
        pixel_type::UINT_ARRAY,
        pixel_channel_order::RG,
        pixel_pack_layout::NONE,
        2, 2, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    RGB_8 = create_pixel_format(
        pixel_type::UINT_ARRAY,
        pixel_channel_order::RGB,
        pixel_pack_layout::NONE,
        3, 3, false
    ),

    BGR_8 = create_pixel_format(
        pixel_type::UINT_ARRAY,
        pixel_channel_order::BGR,
        pixel_pack_layout::NONE,
        3, 3, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    RGBA_8888 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::RGBA,
        pixel_pack_layout::_8888,
        4, 4, true
    ),

    BGRA_8888 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::BGRA,
        pixel_pack_layout::_8888,
        4, 4, true
    ),

    ARGB_8888 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::ARGB,
        pixel_pack_layout::_8888,
        4, 4, true
    ),

    ABGR_8888 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::ABGR,
        pixel_pack_layout::_8888,
        4, 4, true
    ),

    RGBX_8888 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::RGBX,
        pixel_pack_layout::_8888,
        4, 4, false
    ),

    BGRX_8888 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::BGRX,
        pixel_pack_layout::_8888,
        4, 4, false
    ),

    XRGB_8888 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::XRGB,
        pixel_pack_layout::_8888,
        4, 4, false
    ),

    XBGR_8888 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::XBGR,
        pixel_pack_layout::_8888,
        4, 4, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    ARGB_2101010 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::ARGB,
        pixel_pack_layout::_2101010,
        4, 4, true
    ),

    ABGR_2101010 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::ABGR,
        pixel_pack_layout::_2101010,
        4, 4, true
    ),

    XRGB_2101010 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::XRGB,
        pixel_pack_layout::_2101010,
        4, 4, false
    ),

    XBGR_2101010 = create_pixel_format(
        pixel_type::PACKED_32,
        pixel_channel_order::XBGR,
        pixel_pack_layout::_2101010,
        4, 4, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    RGBA_8 = create_pixel_format(
        pixel_type::UINT_ARRAY,
        pixel_channel_order::RGBA,
        pixel_pack_layout::NONE,
        4, 4, true
    ),

    BGRA_8 = create_pixel_format(
        pixel_type::UINT_ARRAY,
        pixel_channel_order::BGRA,
        pixel_pack_layout::NONE,
        4, 4, true
    ),

    ABGR_8 = create_pixel_format(
        pixel_type::UINT_ARRAY,
        pixel_channel_order::ABGR,
        pixel_pack_layout::NONE,
        4, 4, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    R_16F = create_pixel_format(
        pixel_type::FLOAT_ARRAY,
        pixel_channel_order::R,
        pixel_pack_layout::NONE,
        1, 2, false
    ),

    RG_16F = create_pixel_format(
        pixel_type::FLOAT_ARRAY,
        pixel_channel_order::RG,
        pixel_pack_layout::NONE,
        2, 4, false
    ),

    RGB_16F = create_pixel_format(
        pixel_type::FLOAT_ARRAY,
        pixel_channel_order::RGB,
        pixel_pack_layout::NONE,
        3, 6, false
    ),

    RGBA_16F = create_pixel_format(
        pixel_type::FLOAT_ARRAY,
        pixel_channel_order::RGBA,
        pixel_pack_layout::NONE,
        4, 8, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    R_32F = create_pixel_format(
        pixel_type::FLOAT_ARRAY,
        pixel_channel_order::R,
        pixel_pack_layout::NONE,
        1, 4, false
    ),

    RG_32F = create_pixel_format(
        pixel_type::FLOAT_ARRAY,
        pixel_channel_order::RG,
        pixel_pack_layout::NONE,
        2, 8, false
    ),

    RGB_32F = create_pixel_format(
        pixel_type::FLOAT_ARRAY,
        pixel_channel_order::RGB,
        pixel_pack_layout::NONE,
        3, 12, false
    ),

    RGBA_32F = create_pixel_format(
        pixel_type::FLOAT_ARRAY,
        pixel_channel_order::RGBA,
        pixel_pack_layout::NONE,
        4, 16, true
    ),

    ///////////////////////////////////////////////////////////////////////////////
};

VX_FORCE_INLINE constexpr pixel_type get_pixel_type(pixel_format format) noexcept
{
    return static_cast<pixel_type>((static_cast<uint32_t>(format) >> 28) & 0x0F);
}

VX_FORCE_INLINE constexpr bool is_packed_format(pixel_format format) noexcept
{
    return (static_cast<uint32_t>(format) >> 20) & 0x0F;
}

VX_FORCE_INLINE constexpr bool is_normalized_format(pixel_format format) noexcept
{
    return get_pixel_type(format) == pixel_type::FLOAT_ARRAY;
}

VX_FORCE_INLINE constexpr size_t get_pixel_channel_count(pixel_format format) noexcept
{
    return (static_cast<uint32_t>(format) >> 16) & 0x0F;
}

VX_FORCE_INLINE constexpr size_t get_pixel_size(pixel_format format) noexcept
{
    return (static_cast<uint32_t>(format) & 0x000007FF);
}

VX_FORCE_INLINE constexpr size_t get_bits_per_pixel(pixel_format format) noexcept
{
    return get_pixel_size(format) * 8;
}

VX_FORCE_INLINE constexpr bool pixel_has_alpha(pixel_format format) noexcept
{
    return (static_cast<uint32_t>(format) & 0x00000800);
}

inline constexpr pixel_format channel_count_to_8_bit_format(size_t channels) noexcept
{
    switch (channels)
    {
        case 1:     return pixel_format::R_8;
        case 2:     //return pixel_format::RG_8;
        case 3:     return pixel_format::RGB_8;
        case 4:     return pixel_format::RGBA_8;
        default:    return pixel_format::UNKNOWN;
    }
}

struct channel_info
{
    struct channel_data
    {
        uint32_t index;
        uint32_t bits;
        uint32_t mask;
        uint32_t shift;
    };

    channel_data r, g, b, a;
};

// try not to call this at run time
inline constexpr channel_info get_channel_info(pixel_format format) noexcept
{
    channel_info info{};

    switch (format)
    {
        case pixel_format::UNKNOWN:
        {
            break;
        }
        case pixel_format::RGB_332:
        {
            info.r = { 0, 3, 0x07, 0 };
            info.g = { 1, 3, 0x38, 3 };
            info.b = { 2, 2, 0xC0, 6 };
            info.a = { 3, 0, 0x00, 0 };
            break;
        }
        case pixel_format::RGBA_4444:
        {
            info.r = { 0, 4, 0x000F,  0 };
            info.g = { 1, 4, 0x00F0,  4 };
            info.b = { 2, 4, 0x0F00,  8 };
            info.a = { 3, 4, 0xF000, 12 };
            break;
        }
        case pixel_format::BGRA_4444:
        {
            info.r = { 2, 4, 0x0F00,  8 };
            info.g = { 1, 4, 0x00F0,  4 };
            info.b = { 0, 4, 0x000F,  0 };
            info.a = { 3, 4, 0xF000, 12 };
            break;
        }
        case pixel_format::RGB_565:
        {
            info.r = { 0, 5, 0x001F,  0 };
            info.g = { 1, 6, 0x07E0,  5 };
            info.b = { 2, 5, 0xF800, 11 };
            info.a = { 3, 0, 0x0000,  0 };
            break;
        }
        case pixel_format::BGR_565:
        {
            info.r = { 2, 5, 0xF800, 11 };
            info.g = { 1, 6, 0x07E0,  5 };
            info.b = { 0, 5, 0x001F,  0 };
            info.a = { 3, 0, 0x0000,  0 };
            break;
        }
        case pixel_format::RGBA_5551:
        {
            info.r = { 0, 5, 0x001F,  0 };
            info.g = { 1, 5, 0x03E0,  5 };
            info.b = { 2, 5, 0x7C00, 10 };
            info.a = { 3, 1, 0x8000, 15 };
            break;
        }
        case pixel_format::BGRA_5551:
        {
            info.r = { 2, 5, 0x7C00, 10 };
            info.g = { 1, 5, 0x03E0,  5 };
            info.b = { 0, 5, 0x001F,  0 };
            info.a = { 3, 1, 0x8000, 15 };
            break;
        }
        case pixel_format::ARGB_1555:
        {
            info.r = { 1, 5, 0x003E,  1 };
            info.g = { 2, 5, 0x07C0,  6 };
            info.b = { 3, 5, 0XF800, 11 };
            info.a = { 0, 1, 0x0001,  0 };
            break;
        }
        case pixel_format::XRGB_1555:
        {
            info.r = { 1, 5, 0x003E,  1 };
            info.g = { 2, 5, 0x07C0,  6 };
            info.b = { 3, 5, 0XF800, 11 };
            info.a = { 0, 0, 0x0000,  0 };
            break;
        }
        case pixel_format::XBGR_1555:
        {
            info.r = { 3, 5, 0XF800, 11 };
            info.g = { 2, 5, 0x07C0,  6 };
            info.b = { 1, 5, 0x003E,  1 };
            info.a = { 0, 0, 0x0000,  0 };
            break;
        }
        case pixel_format::RGBA_8888:
        {
            info.r = { 0, 8, 0x000000FF,  0 };
            info.g = { 1, 8, 0x0000FF00,  8 };
            info.b = { 2, 8, 0x00FF0000, 16 };
            info.a = { 3, 8, 0xFF000000, 24 };
            break;
        }
        case pixel_format::BGRA_8888:
        {
            info.r = { 2, 8, 0x00FF0000, 16 };
            info.g = { 1, 8, 0x0000FF00,  8 };
            info.b = { 0, 8, 0x000000FF,  0 };
            info.a = { 3, 8, 0xFF000000, 24 };
            break;
        }
        case pixel_format::ARGB_8888:
        {
            info.r = { 1, 8, 0x00FF0000,  8 };
            info.g = { 2, 8, 0x0000FF00, 16 };
            info.b = { 3, 8, 0x000000FF, 24 };
            info.a = { 0, 8, 0xFF000000,  0 };
            break;
        }
        case pixel_format::ABGR_8888:
        {
            info.r = { 3, 8, 0xFF000000, 24 };
            info.g = { 2, 8, 0x00FF0000, 16 };
            info.b = { 1, 8, 0x0000FF00,  8 };
            info.a = { 0, 8, 0x000000FF,  0 };
            break;
        }
        case pixel_format::RGBX_8888:
        {
            info.r = { 0, 8, 0x000000FF,  0 };
            info.g = { 1, 8, 0x0000FF00,  8 };
            info.b = { 2, 8, 0x00FF0000, 16 };
            info.a = { 3, 0, 0x00000000,  0 };
            break;
        }
        case pixel_format::BGRX_8888:
        {
            info.r = { 2, 8, 0x00FF0000, 16 };
            info.g = { 1, 8, 0x0000FF00,  8 };
            info.b = { 0, 8, 0x000000FF,  0 };
            info.a = { 3, 0, 0x00000000,  0 };
            break;
        }
        case pixel_format::XRGB_8888:
        {
            info.r = { 1, 8, 0x00FF0000,  8 };
            info.g = { 2, 8, 0x0000FF00, 16 };
            info.b = { 3, 8, 0x000000FF, 24 };
            info.a = { 0, 0, 0x00000000,  0 };
            break;
        }
        case pixel_format::XBGR_8888:
        {
            info.r = { 3, 8, 0xFF000000, 24 };
            info.g = { 2, 8, 0x00FF0000, 16 };
            info.b = { 1, 8, 0x0000FF00,  8 };
            info.a = { 0, 0, 0x00000000,  0 };
            break;
        }
        case pixel_format::ARGB_2101010:
        {
            info.r = { 1, 10, 0x00000FFC,  2 };
            info.g = { 2, 10, 0x003FF000, 12 };
            info.b = { 3, 10, 0xFFC00000, 22 };
            info.a = { 0,  2, 0x00000003,  0 };
            break;
        }
        case pixel_format::ABGR_2101010:
        {
            info.r = { 3, 10, 0xFFC00000, 22 };
            info.g = { 2, 10, 0x003FF000, 12 };
            info.b = { 1, 10, 0x00000FFC,  2 };
            info.a = { 0,  2, 0x00000003,  0 };
            break;
        }

        case pixel_format::XRGB_2101010:
        {
            info.r = { 1, 10, 0x00000FFC,  2 };
            info.g = { 2, 10, 0x003FF000, 12 };
            info.b = { 3, 10, 0xFFC00000, 22 };
            info.a = { 0,  0, 0x00000000,  0 };
            break;
        }
        case pixel_format::XBGR_2101010:
        {
            info.r = { 3, 10, 0xFFC00000, 22 };
            info.g = { 2, 10, 0x003FF000, 12 };
            info.b = { 1, 10, 0x00000FFC,  2 };
            info.a = { 0,  0, 0x00000000,  0 };
            break;
        }
        // For array pixel types, the shift represents the array index of the channel
        case pixel_format::R_8:
        {
            info.r = { 0, 8, 0, 0 };
            info.g = { 1, 0, 0, 0 };
            info.b = { 2, 0, 0, 0 };
            info.a = { 3, 0, 0, 0 };
            break;
        }
        case pixel_format::RG_8:
        {
            info.r = { 0, 8, 0, 0 };
            info.g = { 1, 8, 0, 0 };
            info.b = { 2, 0, 0, 0 };
            info.a = { 3, 0, 0, 0 };
            break;
        }
        case pixel_format::RGB_8:
        {
            info.r = { 0, 8, 0, 0 };
            info.g = { 1, 8, 0, 0 };
            info.b = { 2, 8, 0, 0 };
            info.a = { 3, 0, 0, 0 };
            break;
        }
        case pixel_format::BGR_8:
        {
            info.r = { 2, 8, 0, 0 };
            info.g = { 0, 8, 0, 0 };
            info.b = { 0, 8, 0, 0 };
            info.a = { 3, 8, 0, 0 };
            break;
        }
        case pixel_format::RGBA_8:
        {
            info.r = { 0, 8, 0, 0 };
            info.g = { 1, 8, 0, 0 };
            info.b = { 2, 8, 0, 0 };
            info.a = { 3, 8, 0, 0 };
            break;
        }
        case pixel_format::BGRA_8:
        {
            info.r = { 2, 8, 0, 0 };
            info.g = { 1, 8, 0, 0 };
            info.b = { 0, 8, 0, 0 };
            info.a = { 3, 8, 0, 0 };
            break;
        }
        case pixel_format::ABGR_8:
        {
            info.r = { 3, 8, 0, 0 };
            info.g = { 2, 8, 0, 0 };
            info.b = { 1, 8, 0, 0 };
            info.a = { 0, 8, 0, 0 };
            break;
        }
        case pixel_format::R_16F:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1,  0, 0, 0 };
            info.b = { 2,  0, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::RG_16F:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1, 16, 0, 0 };
            info.b = { 2,  0, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::RGB_16F:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1, 16, 0, 0 };
            info.b = { 2, 16, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::RGBA_16F:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1, 16, 0, 0 };
            info.b = { 2, 16, 0, 0 };
            info.a = { 3, 16, 0, 0 };
            break;
        }
        case pixel_format::R_32F:
        {
            info.r = { 0, 32, 0, 0 };
            info.g = { 1,  0, 0, 0 };
            info.b = { 2,  0, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::RG_32F:
        {
            info.r = { 0, 32, 0, 0 };
            info.g = { 1, 32, 0, 0 };
            info.b = { 2,  0, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::RGB_32F:
        {
            info.r = { 0, 32, 0, 0 };
            info.g = { 1, 32, 0, 0 };
            info.b = { 2, 32, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::RGBA_32F:
        {
            info.r = { 0, 32, 0, 0 };
            info.g = { 1, 32, 0, 0 };
            info.b = { 2, 32, 0, 0 };
            info.a = { 3, 32, 0, 0 };
            break;
        }
    }

    return info;
}

inline constexpr std::array<uint32_t, 4> build_mask_array(pixel_format format) noexcept
{
    const channel_info info = get_channel_info(format);

    std::array<uint32_t, 4> masks{};
    masks[info.r.index] = info.r.mask;
    masks[info.g.index] = info.g.mask;
    masks[info.b.index] = info.b.mask;
    masks[info.a.index] = info.a.mask;

    return masks;
}

inline constexpr std::array<uint32_t, 4> build_shift_array(pixel_format format) noexcept
{
    const channel_info info = get_channel_info(format);

    std::array<uint32_t, 4> shifts{};
    shifts[info.r.index] = info.r.shift;
    shifts[info.g.index] = info.g.shift;
    shifts[info.b.index] = info.b.shift;
    shifts[info.a.index] = info.a.shift;

    return shifts;
}

enum class filter_mode
{
    NEAREST,
    LINEAR
};

enum class wrap_mode
{
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
    REPEAT,
    MIRRORED_REPEAT,
    MIRROR_CLAMP_TO_EDGE
};

///////////////////////////////////////////////////////////////////////////////

enum class palette_order : uint32_t
{
    LSB = 0,
    MSB = 1
};

VX_FORCE_INLINE constexpr uint32_t create_palette_format(
    palette_order order,
    uint32_t block_size,
    uint32_t sub_index_count
) noexcept
{
    return (static_cast<uint32_t>(order) << 31) | // order           = 1 bit
           (block_size                   << 16) | // size            = 8 bits
           (sub_index_count              <<  8);  // sub index count = 8 bits
}

enum class palette_format : uint32_t
{
    INDEX_1LSB = create_palette_format(
        palette_order::LSB,
        1,
        8
    ),

    INDEX_1MSB = create_palette_format(
        palette_order::MSB,
        1,
        8
    ),

    INDEX_2LSB = create_palette_format(
        palette_order::MSB,
        1,
        4
    ),

    INDEX_2MSB = create_palette_format(
        palette_order::LSB,
        1,
        4
    ),

    INDEX_4LSB = create_palette_format(
        palette_order::LSB,
        1,
        2
    ),

    INDEX_4MSB = create_palette_format(
        palette_order::MSB,
        1,
        2
    ),

    INDEX_8 = create_palette_format(
        palette_order::LSB,
        1,
        1
    ),

    INDEX_8LSB = INDEX_8,
    INDEX_8MSB = INDEX_8
};

VX_FORCE_INLINE constexpr palette_order get_palette_order(palette_format format) noexcept
{
    return static_cast<palette_order>((static_cast<uint32_t>(format) >> 31) & 0x01);
}

VX_FORCE_INLINE constexpr size_t get_block_size(palette_format format) noexcept
{
    return (static_cast<uint32_t>(format) >> 16) & 0xFF;
}

VX_FORCE_INLINE constexpr size_t get_bits_per_block(palette_format format) noexcept
{
    return get_block_size(format) * 8;
}

VX_FORCE_INLINE constexpr size_t get_sub_index_count(palette_format format) noexcept
{
    return (static_cast<uint32_t>(format) >> 8) & 0xFF;
}

VX_FORCE_INLINE constexpr size_t get_bits_per_sub_index(palette_format format) noexcept
{
    return get_bits_per_block(format) / get_sub_index_count(format);
}

VX_FORCE_INLINE constexpr size_t get_max_palette_size(palette_format format) noexcept
{
    return get_sub_index_count(format) * (1ull << get_bits_per_sub_index(format));
}

} // namespace pixel
} // namespace vx