#pragma once

#include "pixel.h"

namespace vx {
namespace pixel {

///////////////////////////////////////////////////////////////////////////////
// type info
///////////////////////////////////////////////////////////////////////////////

template <pixel_format f> struct pixel_type_info;

///////////////////////////////////////////////////////////////////////////////
// unknown
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_UNKNOWN>
{
    using pixel_type = void;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////
// 332
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGB_332>
{
    using pixel_type = uint8_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////
// 4444
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGBA_4444>
{
    using pixel_type = uint16_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_BGRA_4444>
{
    using pixel_type = uint16_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////
// 565
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGB_565>
{
    using pixel_type = uint16_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_BGR_565>
{
    using pixel_type = uint16_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////
// 5551 & 1555
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGBA_5551>
{
    using pixel_type = uint16_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_BGRA_5551>
{
    using pixel_type = uint16_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_ARGB_1555>
{
    using pixel_type = uint16_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////
// packed 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGBA_8888>
{
    using pixel_type = uint32_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_BGRA_8888>
{
    using pixel_type = uint32_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_ABGR_8888>
{
    using pixel_type = uint32_t;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////
// 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_R_8>
{
    using pixel_type = void;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RG_8>
{
    using pixel_type = void;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGB_8>
{
    using pixel_type = void;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGBA_8>
{
    using pixel_type = void;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_BGR_8>
{
    using pixel_type = void;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_BGRA_8>
{
    using pixel_type = void;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_ABGR_8>
{
    using pixel_type = void;
    using channel_type = uint8_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////
// 2101010
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_ARGB_2101010>
{
    using pixel_type = uint32_t;
    using channel_type = uint16_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_ABGR_2101010>
{
    using pixel_type = uint32_t;
    using channel_type = uint16_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////
// 16 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_R_16>
{
    using pixel_type = void;
    using channel_type = uint16_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RG_16>
{
    using pixel_type = void;
    using channel_type = uint16_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGB_16>
{
    using pixel_type = void;
    using channel_type = uint16_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGBA_16>
{
    using pixel_type = void;
    using channel_type = uint16_t;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////
// 32 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_R_32>
{
    using pixel_type = void;
    using channel_type = uint32_t;
    using float_type = double;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RG_32>
{
    using pixel_type = void;
    using channel_type = uint32_t;
    using float_type = double;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGB_32>
{
    using pixel_type = void;
    using channel_type = uint32_t;
    using float_type = double;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGBA_32>
{
    using pixel_type = void;
    using channel_type = uint32_t;
    using float_type = double;
};

///////////////////////////////////////////////////////////////////////////////
// 32 bit float
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_R_32F>
{
    using pixel_type = void;
    using channel_type = float;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RG_32F>
{
    using pixel_type = void;
    using channel_type = float;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGB_32F>
{
    using pixel_type = void;
    using channel_type = float;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGBA_32F>
{
    using pixel_type = void;
    using channel_type = float;
    using float_type = float;
};

///////////////////////////////////////////////////////////////////////////////
// 64 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_R_64>
{
    using pixel_type = void;
    using channel_type = uint64_t;
    using float_type = double;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RG_64>
{
    using pixel_type = void;
    using channel_type = uint64_t;
    using float_type = double;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGB_64>
{
    using pixel_type = void;
    using channel_type = uint64_t;
    using float_type = double;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct pixel_type_info<pixel_format::PIXEL_FORMAT_RGBA_64>
{
    using pixel_type = void;
    using channel_type = uint64_t;
    using float_type = double;
};

///////////////////////////////////////////////////////////////////////////////

}
}