#pragma once

#include <cstdint>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace pixel {

using byte_type = uint8_t;

// https://docs.vulkan.org/spec/latest/chapters/formats.html

enum class pixel_type : uint32_t
{
    NONE          = 0,
                  
    PACKED_8      = 1,
    PACKED_16     = 2,
    PACKED_32     = 3,
    UINT_ARRAY    = 4,
    FLOAT_ARRAY   = 5
};

enum class pixel_channel_order : uint32_t
{
    NONE = 0,

    R    = 1,
    RG   = 2,
    RGB  = 3,
    BGR  = 4,
    RGBA = 5,
    BGRA = 6,
    ARGB = 7,
    ABGR = 8
};

enum class pixel_pack_layout : uint32_t
{
    NONE    = 0,
       
    _332     = 1,
    _4444    = 2,
    _565     = 3,
    _5551    = 4,
    _1555    = 5,
    _8888    = 9,
    _2101010 = 10
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

    UNKNOWN = 0x00000000,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_NONE,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_NONE,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     0, 0, false
    // )

    ///////////////////////////////////////////////////////////////////////////////

    RGB_332 = 0x13130001,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_8,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_332,
    //     3, 1, false
    // )
    
    ///////////////////////////////////////////////////////////////////////////////

    RGBA_4444 = 0x25240802,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_16,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA, 
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_4444,
    //     4, 2, true
    // )

    BGRA_4444 = 0x26240802,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_16,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_BGRA,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_4444,
    //     4, 2, true
    // )

    ///////////////////////////////////////////////////////////////////////////////

    RGB_565 = 0x23330002,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_16,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_565,
    //     3, 2, false
    // )

    BGR_565 = 0x24330002,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_16,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_BGR,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_565,
    //     3, 2, false
    // )

    ///////////////////////////////////////////////////////////////////////////////

    RGBA_5551 = 0x25440802,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_16,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_5551,
    //     4, 2, true
    // )

    BGRA_5551 = 0x26440802,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_16,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_BGRA,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_5551,
    //     4, 2, true
    // )

    ARGB_1555 = 0x27540802,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_16,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_ARGB,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_1555,
    //     4, 2, true
    // )

    ///////////////////////////////////////////////////////////////////////////////

    R_8 = 0x41010001,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_R,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     1, 1, false
    // )

    RG_8 = 0x42020002,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RG,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     2, 2, false
    // )

    ///////////////////////////////////////////////////////////////////////////////

    RGB_8 = 0x43030003,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     3, 3, false
    // )

    BGR_8 = 0x44030003,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_BGR,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     3, 3, false
    // )

    ///////////////////////////////////////////////////////////////////////////////

    RGBA_8888 = 0x35940804,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_32,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_8888,
    //     4, 4, true
    // )

    BGRA_8888 = 0x36940804,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_32,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_BGRA,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_8888,
    //     4, 4, true
    // )

    ABGR_8888 = 0x38940804,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_32,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_ABGR,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_8888,
    //     4, 4, true
    // )

    ///////////////////////////////////////////////////////////////////////////////

    ARGB_2101010 = 0x37A40804,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_32,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_ARGB,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_2101010,
    //     4, 4, true
    // )

    ABGR_2101010 = 0x38A40804,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_PACKED_32,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_ABGR,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_2101010,
    //     4, 4, true
    // )

    ///////////////////////////////////////////////////////////////////////////////

    RGBA_8 = 0x45040804,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     4, 4, true
    // )

    BGRA_8 = 0x46040804,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_BGRA,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     4, 4, true
    // )

    ABGR_8 = 0x48040804,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_ABGR,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     4, 4, true
    // )

    ///////////////////////////////////////////////////////////////////////////////

    R_16 = 0x41010002,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_R,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     1, 2, false
    // )

    RG_16 = 0x42020004,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RG,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     2, 4, false
    // )

    RGB_16 = 0x43030006,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     3, 6, false
    // )

    RGBA_16 = 0x45040808,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     4, 8, true
    // )

    ///////////////////////////////////////////////////////////////////////////////

    R_32 = 0x41010004,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_R,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     1, 4, false
    // )

    RG_32 = 0x42020008,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RG,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     2, 8, false
    // )

    RGB_32 = 0x4303000C,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     3, 12, false
    // )

    RGBA_32 = 0x45040810,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_UINT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     4, 16, true
    // )

    ///////////////////////////////////////////////////////////////////////////////

    R_32F = 0x51010004,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_FLOAT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_R,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     1, 4, false
    // )

    RG_32F = 0x52020008,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_FLOAT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RG,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     2, 8, false
    // )

    RGB_32F = 0x5303000C,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_FLOAT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     3, 12, false
    // )

    RGBA_32F = 0x55040810,
    // create_pixel_format(
    //     pixel_type::PIXEL_TYPE_FLOAT_ARRAY,
    //     pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
    //     pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
    //     4, 16, true
    // )

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

VX_FORCE_INLINE constexpr size_t get_pixel_bit_count(pixel_format format) noexcept
{
    return get_pixel_size(format) * 8;
}

VX_FORCE_INLINE constexpr bool pixel_has_alpha(pixel_format format) noexcept
{
    return (static_cast<uint32_t>(format) & 0x00000800);
}

inline constexpr pixel_format format_from_channels(size_t channels) noexcept
{
    switch (channels)
    {
        case 1:     return pixel_format::R_8;
        case 2:     return pixel_format::RG_8;
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
        case pixel_format::ABGR_8888:
        {
            info.r = { 3, 8, 0xFF000000, 24 };
            info.g = { 2, 8, 0x00FF0000, 16 };
            info.b = { 1, 8, 0x0000FF00,  8 };
            info.a = { 0, 8, 0x000000FF,  0 };
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
        case pixel_format::R_16:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1,  0, 0, 0 };
            info.b = { 2,  0, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::RG_16:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1, 16, 0, 0 };
            info.b = { 2,  0, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::RGB_16:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1, 16, 0, 0 };
            info.b = { 2, 16, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::RGBA_16:
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

} // namespace pixel
} // namespace vx