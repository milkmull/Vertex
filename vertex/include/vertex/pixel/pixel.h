#pragma once

#include <unordered_map>

#include "vertex/config.h"

namespace vx {
namespace pixel {

// https://docs.vulkan.org/spec/latest/chapters/formats.html

enum class pixel_type : uint32_t
{
    PIXEL_TYPE_NONE          = 0,
                             
    PIXEL_TYPE_PACKED_8      = 1,
    PIXEL_TYPE_PACKED_16     = 2,
    PIXEL_TYPE_PACKED_32     = 3,
    PIXEL_TYPE_UINT_ARRAY    = 4,
    PIXEL_TYPE_FLOAT_ARRAY   = 5
};

enum class pixel_channel_order : uint32_t
{
    PIXEL_CHANNEL_ORDER_NONE = 0,

    PIXEL_CHANNEL_ORDER_R    = 1,
    PIXEL_CHANNEL_ORDER_RG   = 2,
    PIXEL_CHANNEL_ORDER_RGB  = 3,
    PIXEL_CHANNEL_ORDER_BGR  = 4,
    PIXEL_CHANNEL_ORDER_RGBA = 5,
    PIXEL_CHANNEL_ORDER_BGRA = 6,
    PIXEL_CHANNEL_ORDER_ARGB = 7,
    PIXEL_CHANNEL_ORDER_ABGR = 8
};

enum class pixel_pack_layout : uint32_t
{
    PIXEL_PACK_LAYOUT_NONE    = 0,
                        
    PIXEL_PACK_LAYOUT_332     = 1,
    PIXEL_PACK_LAYOUT_4444    = 2,
    PIXEL_PACK_LAYOUT_565     = 3,
    PIXEL_PACK_LAYOUT_5551    = 4,
    PIXEL_PACK_LAYOUT_1555    = 5,
    PIXEL_PACK_LAYOUT_8888    = 9,
    PIXEL_PACK_LAYOUT_2101010 = 10
};

inline constexpr uint32_t create_pixel_format(
    pixel_type type,
    pixel_channel_order channel_order,
    pixel_pack_layout pack_layout,
    uint32_t channel_count,
    uint32_t pixel_size,
    bool alpha
)
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

    PIXEL_FORMAT_UNKNOWN = create_pixel_format(
        pixel_type::PIXEL_TYPE_NONE,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_NONE,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        0, 0, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_RGB_332 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_8,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_332,
        3, 1, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_RGBA_4444 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_16,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA, 
        pixel_pack_layout::PIXEL_PACK_LAYOUT_4444,
        4, 2, true
    ),

    PIXEL_FORMAT_BGRA_4444 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_16,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_BGRA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_4444,
        4, 2, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_RGB_565 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_16,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_565,
        3, 2, false
    ),

    PIXEL_FORMAT_BGR_565 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_16,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_BGR,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_565,
        3, 2, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_RGBA_5551 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_16,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_5551,
        4, 2, true
    ),

    PIXEL_FORMAT_BGRA_5551 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_16,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_BGRA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_5551,
        4, 2, true
    ),

    PIXEL_FORMAT_ARGB_1555 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_16,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_ARGB,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_1555,
        4, 2, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_R_8 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_R,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        1, 1, false
    ),

    PIXEL_FORMAT_RG_8 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RG,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        2, 2, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_RGB_8 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        3, 3, false
    ),

    PIXEL_FORMAT_BGR_8 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_BGR,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        3, 3, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_RGBA_8888 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_32,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_8888,
        4, 4, true
    ),

    PIXEL_FORMAT_BGRA_8888 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_32,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_BGRA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_8888,
        4, 4, true
    ),

    PIXEL_FORMAT_ABGR_8888 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_32,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_ABGR,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_8888,
        4, 4, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_RGBA_8 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        4, 4, true
    ),

    PIXEL_FORMAT_BGRA_8 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_BGRA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        4, 4, true
    ),

    PIXEL_FORMAT_ABGR_8 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_ABGR,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        4, 4, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_ARGB_2101010 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_32,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_ARGB,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_2101010,
        4, 4, true
    ),

    PIXEL_FORMAT_ABGR_2101010 = create_pixel_format(
        pixel_type::PIXEL_TYPE_PACKED_32,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_ABGR,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_2101010,
        4, 4, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_R_16 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_R,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        1, 2, false
    ),

    PIXEL_FORMAT_RG_16 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RG,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        2, 4, false
    ),

    PIXEL_FORMAT_RGB_16 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        3, 6, false
    ),

    PIXEL_FORMAT_RGBA_16 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        4, 8, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_R_32 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_R,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        1, 4, false
    ),

    PIXEL_FORMAT_RG_32 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RG,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        2, 8, false
    ),

    PIXEL_FORMAT_RGB_32 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        3, 12, false
    ),

    PIXEL_FORMAT_RGBA_32 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        4, 16, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_R_32F = create_pixel_format(
        pixel_type::PIXEL_TYPE_FLOAT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_R,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        1, 4, false
    ),

    PIXEL_FORMAT_RG_32F = create_pixel_format(
        pixel_type::PIXEL_TYPE_FLOAT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RG,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        2, 8, false
    ),

    PIXEL_FORMAT_RGB_32F = create_pixel_format(
        pixel_type::PIXEL_TYPE_FLOAT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        3, 12, false
    ),

    PIXEL_FORMAT_RGBA_32F = create_pixel_format(
        pixel_type::PIXEL_TYPE_FLOAT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        4, 16, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    PIXEL_FORMAT_R_64 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_R,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        1, 8, false
    ),

    PIXEL_FORMAT_RG_64 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RG,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        2, 16, false
    ),

    PIXEL_FORMAT_RGB_64 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGB,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        3, 24, false
    ),

    PIXEL_FORMAT_RGBA_64 = create_pixel_format(
        pixel_type::PIXEL_TYPE_UINT_ARRAY,
        pixel_channel_order::PIXEL_CHANNEL_ORDER_RGBA,
        pixel_pack_layout::PIXEL_PACK_LAYOUT_NONE,
        4, 32, true
    )

    ///////////////////////////////////////////////////////////////////////////////
};

inline constexpr bool is_packed_format(pixel_format format)
{
    return (static_cast<uint32_t>(format) >> 20) & 0x0F;
}

inline constexpr size_t get_channel_count(pixel_format format)
{
    return (static_cast<uint32_t>(format) >> 16) & 0x0F;
}

inline constexpr size_t get_pixel_size(pixel_format format)
{
    return (static_cast<uint32_t>(format) & 0x000007FF);
}

inline constexpr size_t get_pixel_bit_count(pixel_format format)
{
    return get_pixel_size(format) * 8;
}

inline constexpr bool has_alpha(pixel_format format)
{
    return (static_cast<uint32_t>(format) & 0x00000800);
}

struct pixel_format_info
{
    pixel_format format;

    size_t channel_count;
    size_t pixel_size;
    size_t pixel_bit_count;
    bool alpha;

    struct channel_data
    {
        uint32_t r;
        uint32_t g;
        uint32_t b;
        uint32_t a;
    };

    struct channel_info
    {
        channel_data bits;
        channel_data mask;
        channel_data shift;
    };

    channel_info channels;
};

inline constexpr pixel_format_info::channel_info get_pixel_format_channel_info(pixel_format format)
{
    pixel_format_info::channel_info info{};

    // mask

    switch (format)
    {
        case pixel_format::PIXEL_FORMAT_UNKNOWN:
        {
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGB_332:
        {
            info.mask = { 0x07, 0x38, 0xC0, 0x00 };
            info.bits = { 3, 3, 2, 0 };
            info.shift = { 0, 3, 6, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGBA_4444:
        {
            info.mask = { 0x000F, 0x00F0, 0x0F00, 0xF000 };
            info.bits = { 4, 4, 4, 4 };
            info.shift = { 0, 4, 8, 16 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_BGRA_4444:
        {
            info.mask = { 0x0F00, 0x00F0, 0x000F, 0xF000 };
            info.bits = { 4, 4, 4, 4 };
            info.shift = { 8, 4, 0, 16 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGB_565:
        {
            info.mask = { 0x001F, 0x07E0, 0xF800, 0x0000 };
            info.bits = { 5, 6, 5, 0 };
            info.shift = { 0, 5, 11, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_BGR_565:
        {
            info.mask = { 0xF800, 0x07E0, 0x001F, 0x0000 };
            info.bits = { 5, 6, 5, 0 };
            info.shift = { 11, 5, 0, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGBA_5551:
        {
            info.mask = { 0x001F, 0x03E0, 0x7C00, 0x8000 };
            info.bits = { 5, 5, 5, 1 };
            info.shift = { 0, 5, 10, 15 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_BGRA_5551:
        {
            info.mask = { 0x7C00, 0x03E0, 0x001F, 0x8000 };
            info.bits = { 5, 5, 5, 1 };
            info.shift = { 10, 5, 0, 15};
            break;
        }
        case pixel_format::PIXEL_FORMAT_ARGB_1555:
        {
            info.mask = { 0x003E, 0x07C0, 0XF800, 0x0001 };
            info.bits = { 5, 5, 5, 1 };
            info.shift = { 1, 6, 11, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGBA_8888:
        {
            info.mask = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
            info.bits = { 8, 8, 8, 8 };
            info.shift = { 0, 8, 16, 24 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_BGRA_8888:
        {
            info.mask = { 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 };
            info.bits = { 8, 8, 8, 8 };
            info.shift = { 16, 8, 0, 24 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_ABGR_8888:
        {
            info.mask = { 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF };
            info.bits = { 8, 8, 8, 8 };
            info.shift = { 24, 16, 8, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_ARGB_2101010:
        {
            info.mask = { 0x00000FFC, 0x003FF000, 0xFFC00000, 0x00000003 };
            info.bits = { 10, 10, 10, 2 };
            info.shift = { 2, 12, 22, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_ABGR_2101010:
        {
            info.mask = { 0xFFC00000, 0x003FF000, 0x00000FFC, 0x00000003 };
            info.bits = { 10, 10, 10, 2 };
            info.shift = { 22, 12, 2, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_R_8:
        {
            info.mask = { 0, 0, 0, 0 };
            info.bits = { 8, 0, 0, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RG_8:
        {
            info.mask = { 0, 1, 0, 0 };
            info.bits = { 8, 8, 0, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGB_8:
        {
            info.mask = { 0, 1, 2, 0 };
            info.bits = { 8, 8, 8, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_BGR_8:
        {
            info.mask = { 2, 1, 0, 0 };
            info.bits = { 8, 8, 8, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGBA_8:
        {
            info.mask = { 0, 1, 2, 3 };
            info.bits = { 8, 8, 8, 8 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_BGRA_8:
        {
            info.mask = { 2, 1, 0, 3 };
            info.bits = { 8, 8, 8, 8 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_ABGR_8:
        {
            info.mask = { 3, 2, 1, 0 };
            info.bits = { 8, 8, 8, 8 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_R_16:
        {
            info.mask = { 0, 0, 0, 0 };
            info.bits = { 16, 0, 0, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RG_16:
        {
            info.mask = { 0, 1, 0, 0 };
            info.bits = { 16, 16, 0, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGB_16:
        {
            info.mask = { 0, 1, 2, 0 };
            info.bits = { 16, 16, 16, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGBA_16:
        {
            info.mask = { 0, 1, 2, 3 };
            info.bits = { 16, 16, 16, 16 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_R_32:
        case pixel_format::PIXEL_FORMAT_R_32F:
        {
            info.mask = { 0, 0, 0, 0 };
            info.bits = { 32, 0, 0, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RG_32:
        case pixel_format::PIXEL_FORMAT_RG_32F:
        {
            info.mask = { 0, 1, 0, 0 };
            info.bits = { 32, 32, 0, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGB_32:
        case pixel_format::PIXEL_FORMAT_RGB_32F:
        {
            info.mask = { 0, 1, 2, 0 };
            info.bits = { 32, 32, 32, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGBA_32:
        case pixel_format::PIXEL_FORMAT_RGBA_32F:
        {
            info.mask = { 0, 1, 2, 3 };
            info.bits = { 32, 32, 32, 32 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_R_64:
        {
            info.mask = { 0, 0, 0, 0 };
            info.bits = { 64, 0, 0, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RG_64:
        {
            info.mask = { 0, 1, 0, 0 };
            info.bits = { 64, 64, 0, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGB_64:
        {
            info.mask = { 0, 1, 2, 0 };
            info.bits = { 64, 64, 64, 0 };
            break;
        }
        case pixel_format::PIXEL_FORMAT_RGBA_64:
        {
            info.mask = { 0, 1, 2, 3 };
            info.bits = { 64, 64, 64, 64 };
            break;
        }
    }

    return info;
}

inline constexpr pixel_format_info get_pixel_format_info(pixel_format format)
{
    return pixel_format_info{
        format,
        get_channel_count(format),
        get_pixel_size(format),
        get_pixel_bit_count(format),
        has_alpha(format),
        get_pixel_format_channel_info(format)
    };
}

}
}