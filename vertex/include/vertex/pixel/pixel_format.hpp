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
    none = 0,
                  
    packed_8,
    packed_16,
    packed_32,
    uint_array,
    float_array 
};

enum class pixel_channel_order : uint32_t
{
    none = 0,

    r,
    rg,
    rgb,
    bgr,
    rgbx,
    bgrx,
    rgba,
    bgra,
    xrgb,
    xbgr,
    argb,
    abgr 
};

enum class pixel_pack_layout : uint32_t
{
    none = 0,
       
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

    unknown = create_pixel_format(
        pixel_type::none,
        pixel_channel_order::none,
        pixel_pack_layout::none,
        0, 0, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    rgb_332 = create_pixel_format(
         pixel_type::packed_8,
         pixel_channel_order::rgb,
         pixel_pack_layout::_332,
         3, 1, false
     ),
    
    ///////////////////////////////////////////////////////////////////////////////

    rgba_4444 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::rgba, 
        pixel_pack_layout::_4444,
        4, 2, true
    ),

    bgra_4444 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::bgra,
        pixel_pack_layout::_4444,
        4, 2, true
    ),

    xrgb_4444 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::xrgb,
        pixel_pack_layout::_4444,
        4, 2, false
    ),

    xbgr_4444 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::xbgr,
        pixel_pack_layout::_4444,
        4, 2, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    rgb_565 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::rgb,
        pixel_pack_layout::_565,
        3, 2, false
    ),

    bgr_565 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::bgr,
        pixel_pack_layout::_565,
        3, 2, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    rgba_5551 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::rgba,
        pixel_pack_layout::_5551,
        4, 2, true
    ),

    bgra_5551 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::bgra,
        pixel_pack_layout::_5551,
        4, 2, true
    ),

    argb_1555 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::argb,
        pixel_pack_layout::_1555,
        4, 2, true
    ),

    xrgb_1555 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::xrgb,
        pixel_pack_layout::_1555,
        4, 2, false
    ),

    xbgr_1555 = create_pixel_format(
        pixel_type::packed_16,
        pixel_channel_order::xbgr,
        pixel_pack_layout::_1555,
        4, 2, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    r_8 = create_pixel_format(
        pixel_type::uint_array,
        pixel_channel_order::r,
        pixel_pack_layout::none,
        1, 1, false
    ),

    rg_8 = create_pixel_format(
        pixel_type::uint_array,
        pixel_channel_order::rg,
        pixel_pack_layout::none,
        2, 2, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    rgb_8 = create_pixel_format(
        pixel_type::uint_array,
        pixel_channel_order::rgb,
        pixel_pack_layout::none,
        3, 3, false
    ),

    bgr_8 = create_pixel_format(
        pixel_type::uint_array,
        pixel_channel_order::bgr,
        pixel_pack_layout::none,
        3, 3, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    rgba_8888 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::rgba,
        pixel_pack_layout::_8888,
        4, 4, true
    ),

    bgra_8888 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::bgra,
        pixel_pack_layout::_8888,
        4, 4, true
    ),

    argb_8888 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::argb,
        pixel_pack_layout::_8888,
        4, 4, true
    ),

    abgr_8888 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::abgr,
        pixel_pack_layout::_8888,
        4, 4, true
    ),

    rgbx_8888 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::rgbx,
        pixel_pack_layout::_8888,
        4, 4, false
    ),

    bgrx_8888 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::bgrx,
        pixel_pack_layout::_8888,
        4, 4, false
    ),

    xrgb_8888 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::xrgb,
        pixel_pack_layout::_8888,
        4, 4, false
    ),

    xbgr_8888 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::xbgr,
        pixel_pack_layout::_8888,
        4, 4, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    argb_2101010 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::argb,
        pixel_pack_layout::_2101010,
        4, 4, true
    ),

    abgr_2101010 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::abgr,
        pixel_pack_layout::_2101010,
        4, 4, true
    ),

    xrgb_2101010 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::xrgb,
        pixel_pack_layout::_2101010,
        4, 4, false
    ),

    xbgr_2101010 = create_pixel_format(
        pixel_type::packed_32,
        pixel_channel_order::xbgr,
        pixel_pack_layout::_2101010,
        4, 4, false
    ),

    ///////////////////////////////////////////////////////////////////////////////

    rgba_8 = create_pixel_format(
        pixel_type::uint_array,
        pixel_channel_order::rgba,
        pixel_pack_layout::none,
        4, 4, true
    ),

    bgra_8 = create_pixel_format(
        pixel_type::uint_array,
        pixel_channel_order::bgra,
        pixel_pack_layout::none,
        4, 4, true
    ),

    abgr_8 = create_pixel_format(
        pixel_type::uint_array,
        pixel_channel_order::abgr,
        pixel_pack_layout::none,
        4, 4, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    r_16f = create_pixel_format(
        pixel_type::float_array,
        pixel_channel_order::r,
        pixel_pack_layout::none,
        1, 2, false
    ),

    rg_16f = create_pixel_format(
        pixel_type::float_array,
        pixel_channel_order::rg,
        pixel_pack_layout::none,
        2, 4, false
    ),

    rgb_16f = create_pixel_format(
        pixel_type::float_array,
        pixel_channel_order::rgb,
        pixel_pack_layout::none,
        3, 6, false
    ),

    rgba_16f = create_pixel_format(
        pixel_type::float_array,
        pixel_channel_order::rgba,
        pixel_pack_layout::none,
        4, 8, true
    ),

    ///////////////////////////////////////////////////////////////////////////////

    r_32f = create_pixel_format(
        pixel_type::float_array,
        pixel_channel_order::r,
        pixel_pack_layout::none,
        1, 4, false
    ),

    rg_32f = create_pixel_format(
        pixel_type::float_array,
        pixel_channel_order::rg,
        pixel_pack_layout::none,
        2, 8, false
    ),

    rgb_32f = create_pixel_format(
        pixel_type::float_array,
        pixel_channel_order::rgb,
        pixel_pack_layout::none,
        3, 12, false
    ),

    rgba_32f = create_pixel_format(
        pixel_type::float_array,
        pixel_channel_order::rgba,
        pixel_pack_layout::none,
        4, 16, true
    )

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
    return get_pixel_type(format) == pixel_type::float_array;
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
        case 1:     return pixel_format::r_8;
        case 2:     //return pixel_format::rg_8;
        case 3:     return pixel_format::rgb_8;
        case 4:     return pixel_format::rgba_8;
        default:    return pixel_format::unknown;
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
        case pixel_format::unknown:
        {
            break;
        }
        case pixel_format::rgb_332:
        {
            info.r = { 0, 3, 0x07, 0 };
            info.g = { 1, 3, 0x38, 3 };
            info.b = { 2, 2, 0xc0, 6 };
            info.a = { 3, 0, 0x00, 0 };
            break;
        }
        case pixel_format::rgba_4444:
        {
            info.r = { 0, 4, 0x000f,  0 };
            info.g = { 1, 4, 0x00f0,  4 };
            info.b = { 2, 4, 0x0f00,  8 };
            info.a = { 3, 4, 0xf000, 12 };
            break;
        }
        case pixel_format::bgra_4444:
        {
            info.r = { 2, 4, 0x0f00,  8 };
            info.g = { 1, 4, 0x00f0,  4 };
            info.b = { 0, 4, 0x000f,  0 };
            info.a = { 3, 4, 0xf000, 12 };
            break;
        }
        case pixel_format::rgb_565:
        {
            info.r = { 0, 5, 0x001f,  0 };
            info.g = { 1, 6, 0x07e0,  5 };
            info.b = { 2, 5, 0xf800, 11 };
            info.a = { 3, 0, 0x0000,  0 };
            break;
        }
        case pixel_format::bgr_565:
        {
            info.r = { 2, 5, 0xf800, 11 };
            info.g = { 1, 6, 0x07e0,  5 };
            info.b = { 0, 5, 0x001f,  0 };
            info.a = { 3, 0, 0x0000,  0 };
            break;
        }
        case pixel_format::rgba_5551:
        {
            info.r = { 0, 5, 0x001f,  0 };
            info.g = { 1, 5, 0x03e0,  5 };
            info.b = { 2, 5, 0x7c00, 10 };
            info.a = { 3, 1, 0x8000, 15 };
            break;
        }
        case pixel_format::bgra_5551:
        {
            info.r = { 2, 5, 0x7c00, 10 };
            info.g = { 1, 5, 0x03e0,  5 };
            info.b = { 0, 5, 0x001f,  0 };
            info.a = { 3, 1, 0x8000, 15 };
            break;
        }
        case pixel_format::argb_1555:
        {
            info.r = { 1, 5, 0x003e,  1 };
            info.g = { 2, 5, 0x07c0,  6 };
            info.b = { 3, 5, 0xf800, 11 };
            info.a = { 0, 1, 0x0001,  0 };
            break;
        }
        case pixel_format::xrgb_1555:
        {
            info.r = { 1, 5, 0x003e,  1 };
            info.g = { 2, 5, 0x07c0,  6 };
            info.b = { 3, 5, 0xf800, 11 };
            info.a = { 0, 0, 0x0000,  0 };
            break;
        }
        case pixel_format::xbgr_1555:
        {
            info.r = { 3, 5, 0xf800, 11 };
            info.g = { 2, 5, 0x07c0,  6 };
            info.b = { 1, 5, 0x003e,  1 };
            info.a = { 0, 0, 0x0000,  0 };
            break;
        }
        case pixel_format::rgba_8888:
        {
            info.r = { 0, 8, 0x000000ff,  0 };
            info.g = { 1, 8, 0x0000ff00,  8 };
            info.b = { 2, 8, 0x00ff0000, 16 };
            info.a = { 3, 8, 0xff000000, 24 };
            break;
        }
        case pixel_format::bgra_8888:
        {
            info.r = { 2, 8, 0x00ff0000, 16 };
            info.g = { 1, 8, 0x0000ff00,  8 };
            info.b = { 0, 8, 0x000000ff,  0 };
            info.a = { 3, 8, 0xff000000, 24 };
            break;
        }
        case pixel_format::argb_8888:
        {
            info.r = { 1, 8, 0x00ff0000,  8 };
            info.g = { 2, 8, 0x0000ff00, 16 };
            info.b = { 3, 8, 0x000000ff, 24 };
            info.a = { 0, 8, 0xff000000,  0 };
            break;
        }
        case pixel_format::abgr_8888:
        {
            info.r = { 3, 8, 0xff000000, 24 };
            info.g = { 2, 8, 0x00ff0000, 16 };
            info.b = { 1, 8, 0x0000ff00,  8 };
            info.a = { 0, 8, 0x000000ff,  0 };
            break;
        }
        case pixel_format::rgbx_8888:
        {
            info.r = { 0, 8, 0x000000ff,  0 };
            info.g = { 1, 8, 0x0000ff00,  8 };
            info.b = { 2, 8, 0x00ff0000, 16 };
            info.a = { 3, 0, 0x00000000,  0 };
            break;
        }
        case pixel_format::bgrx_8888:
        {
            info.r = { 2, 8, 0x00ff0000, 16 };
            info.g = { 1, 8, 0x0000ff00,  8 };
            info.b = { 0, 8, 0x000000ff,  0 };
            info.a = { 3, 0, 0x00000000,  0 };
            break;
        }
        case pixel_format::xrgb_8888:
        {
            info.r = { 1, 8, 0x00ff0000,  8 };
            info.g = { 2, 8, 0x0000ff00, 16 };
            info.b = { 3, 8, 0x000000ff, 24 };
            info.a = { 0, 0, 0x00000000,  0 };
            break;
        }
        case pixel_format::xbgr_8888:
        {
            info.r = { 3, 8, 0xff000000, 24 };
            info.g = { 2, 8, 0x00ff0000, 16 };
            info.b = { 1, 8, 0x0000ff00,  8 };
            info.a = { 0, 0, 0x00000000,  0 };
            break;
        }
        case pixel_format::argb_2101010:
        {
            info.r = { 1, 10, 0x00000ffc,  2 };
            info.g = { 2, 10, 0x003ff000, 12 };
            info.b = { 3, 10, 0xffc00000, 22 };
            info.a = { 0,  2, 0x00000003,  0 };
            break;
        }
        case pixel_format::abgr_2101010:
        {
            info.r = { 3, 10, 0xffc00000, 22 };
            info.g = { 2, 10, 0x003ff000, 12 };
            info.b = { 1, 10, 0x00000ffc,  2 };
            info.a = { 0,  2, 0x00000003,  0 };
            break;
        }

        case pixel_format::xrgb_2101010:
        {
            info.r = { 1, 10, 0x00000ffc,  2 };
            info.g = { 2, 10, 0x003ff000, 12 };
            info.b = { 3, 10, 0xffc00000, 22 };
            info.a = { 0,  0, 0x00000000,  0 };
            break;
        }
        case pixel_format::xbgr_2101010:
        {
            info.r = { 3, 10, 0xffc00000, 22 };
            info.g = { 2, 10, 0x003ff000, 12 };
            info.b = { 1, 10, 0x00000ffc,  2 };
            info.a = { 0,  0, 0x00000000,  0 };
            break;
        }
        // for array pixel types, the shift represents the array index of the channel
        case pixel_format::r_8:
        {
            info.r = { 0, 8, 0, 0 };
            info.g = { 1, 0, 0, 0 };
            info.b = { 2, 0, 0, 0 };
            info.a = { 3, 0, 0, 0 };
            break;
        }
        case pixel_format::rg_8:
        {
            info.r = { 0, 8, 0, 0 };
            info.g = { 1, 8, 0, 0 };
            info.b = { 2, 0, 0, 0 };
            info.a = { 3, 0, 0, 0 };
            break;
        }
        case pixel_format::rgb_8:
        {
            info.r = { 0, 8, 0, 0 };
            info.g = { 1, 8, 0, 0 };
            info.b = { 2, 8, 0, 0 };
            info.a = { 3, 0, 0, 0 };
            break;
        }
        case pixel_format::bgr_8:
        {
            info.r = { 2, 8, 0, 0 };
            info.g = { 0, 8, 0, 0 };
            info.b = { 0, 8, 0, 0 };
            info.a = { 3, 8, 0, 0 };
            break;
        }
        case pixel_format::rgba_8:
        {
            info.r = { 0, 8, 0, 0 };
            info.g = { 1, 8, 0, 0 };
            info.b = { 2, 8, 0, 0 };
            info.a = { 3, 8, 0, 0 };
            break;
        }
        case pixel_format::bgra_8:
        {
            info.r = { 2, 8, 0, 0 };
            info.g = { 1, 8, 0, 0 };
            info.b = { 0, 8, 0, 0 };
            info.a = { 3, 8, 0, 0 };
            break;
        }
        case pixel_format::abgr_8:
        {
            info.r = { 3, 8, 0, 0 };
            info.g = { 2, 8, 0, 0 };
            info.b = { 1, 8, 0, 0 };
            info.a = { 0, 8, 0, 0 };
            break;
        }
        case pixel_format::r_16f:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1,  0, 0, 0 };
            info.b = { 2,  0, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::rg_16f:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1, 16, 0, 0 };
            info.b = { 2,  0, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::rgb_16f:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1, 16, 0, 0 };
            info.b = { 2, 16, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::rgba_16f:
        {
            info.r = { 0, 16, 0, 0 };
            info.g = { 1, 16, 0, 0 };
            info.b = { 2, 16, 0, 0 };
            info.a = { 3, 16, 0, 0 };
            break;
        }
        case pixel_format::r_32f:
        {
            info.r = { 0, 32, 0, 0 };
            info.g = { 1,  0, 0, 0 };
            info.b = { 2,  0, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::rg_32f:
        {
            info.r = { 0, 32, 0, 0 };
            info.g = { 1, 32, 0, 0 };
            info.b = { 2,  0, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::rgb_32f:
        {
            info.r = { 0, 32, 0, 0 };
            info.g = { 1, 32, 0, 0 };
            info.b = { 2, 32, 0, 0 };
            info.a = { 3,  0, 0, 0 };
            break;
        }
        case pixel_format::rgba_32f:
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
    nearest,
    linear
};

enum class wrap_mode
{
    clamp_to_edge,
    clamp_to_border,
    repeat,
    mirrored_repeat,
    mirror_clamp_to_edge
};

///////////////////////////////////////////////////////////////////////////////

enum class palette_order : uint32_t
{
    lsb = 0,
    msb = 1
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
    index_1_lsb = create_palette_format(
        palette_order::lsb,
        1,
        8
    ),

    index_1_msb = create_palette_format(
        palette_order::msb,
        1,
        8
    ),

    index_2_lsb = create_palette_format(
        palette_order::msb,
        1,
        4
    ),

    index_2_msb = create_palette_format(
        palette_order::lsb,
        1,
        4
    ),

    index_4_lsb = create_palette_format(
        palette_order::lsb,
        1,
        2
    ),

    index_4_msb = create_palette_format(
        palette_order::msb,
        1,
        2
    ),

    index_8 = create_palette_format(
        palette_order::lsb,
        1,
        1
    ),

    index_8_lsb = index_8,
    index_8_msb = index_8
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