#pragma once

#include "vertex/pixel/pixel_format.hpp"
#include "vertex/math/core/functions/common.hpp"
#include "vertex/math/color/types/color.hpp"

namespace vx {
namespace pixel {

///////////////////////////////////////////////////////////////////////////////
// pixel base
///////////////////////////////////////////////////////////////////////////////

template <pixel_format f> struct raw_pixel;

#define __STATIC_CHECK_SIZE(F, FS)         VX_STATIC_ASSERT(sizeof(raw_pixel<F>) == sizeof(typename raw_pixel<F>::pixel_type), "invalid pixel size for format " FS)
#define __STATIC_CHECK_ALIGNMENT(F, FS)    VX_STATIC_ASSERT(alignof(raw_pixel<F>) == alignof(typename raw_pixel<F>::pixel_type), "invalid pixel alignment for format " FS)
#define __STATIC_FORMAT_CHECK(F)           __STATIC_CHECK_SIZE(pixel_format::F, #F); __STATIC_CHECK_ALIGNMENT(pixel_format::F, #F)

#define __PACKED_ENCODE_CHANNEL(x) (static_cast<pixel_type>(math::clamp(static_cast<float_type>(c.x * (info.x.mask >> info.x.shift)), static_cast<float_type>(0), static_cast<float_type>(info.x.mask >> info.x.shift))) << info.x.shift)
#define __PACKED_DECODE_CHANNEL(x) (static_cast<float>((data & info.x.mask) >> info.x.shift) / (info.x.mask >> info.x.shift))

#define __ARRAY_ENCODE_CHANNEL(x) (static_cast<channel_type>(math::clamp(static_cast<float_type>(c.x * static_cast<channel_type>(~0)), static_cast<float_type>(0.0f), static_cast<float_type>(static_cast<channel_type>(~0)))))
#define __ARRAY_DECODE_CHANNEL(x) (static_cast<float>(data[info.x.index]) / static_cast<channel_type>(~0))

///////////////////////////////////////////////////////////////////////////////
// unknown
///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::UNKNOWN>
{
    using channel_type = uint8_t;
    using pixel_type = void;
    using float_type = void;

    static constexpr channel_info info = get_channel_info(pixel_format::UNKNOWN);

    VX_FORCE_INLINE constexpr raw_pixel() noexcept {}
    VX_FORCE_INLINE constexpr raw_pixel(const math::color&) noexcept {}
    VX_FORCE_INLINE constexpr operator math::color() const noexcept { return math::color(); }
};

///////////////////////////////////////////////////////////////////////////////
// 332
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t)) raw_pixel<pixel_format::RGB_332>
{
    using channel_type = uint8_t;
    using pixel_type = uint8_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_332;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(RGB_332);

///////////////////////////////////////////////////////////////////////////////
// 4444
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(uint16_t) raw_pixel<pixel_format::RGBA_4444>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_4444;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b) |
        __PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            __PACKED_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(RGBA_4444);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::BGRA_4444>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGRA_4444;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b) |
        __PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            __PACKED_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(BGRA_4444);

///////////////////////////////////////////////////////////////////////////////
// 565
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::RGB_565>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_565;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(RGB_565);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::BGR_565>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_565;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(BGR_565);

///////////////////////////////////////////////////////////////////////////////
// 5551 & 1555
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::RGBA_5551>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_5551;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b) |
        __PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            __PACKED_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(RGBA_5551);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::BGRA_5551>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGRA_5551;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b) |
        __PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            __PACKED_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(BGRA_5551);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::ARGB_1555>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ARGB_1555;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b) |
        __PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            __PACKED_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(ARGB_1555);

///////////////////////////////////////////////////////////////////////////////
// packed 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::RGBA_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b) |
        __PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            __PACKED_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(RGBA_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::BGRA_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGRA_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b) |
        __PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            __PACKED_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(BGRA_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::ABGR_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ABGR_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b) |
        __PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            __PACKED_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(ABGR_8888);

///////////////////////////////////////////////////////////////////////////////
// 2101010
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::ARGB_2101010>
{
    using channel_type = uint16_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ARGB_2101010;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b) |
        __PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            __PACKED_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(ARGB_2101010);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::ABGR_2101010>
{
    using channel_type = uint16_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ABGR_2101010;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        __PACKED_ENCODE_CHANNEL(r) |
        __PACKED_ENCODE_CHANNEL(g) |
        __PACKED_ENCODE_CHANNEL(b) |
        __PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __PACKED_DECODE_CHANNEL(r),
            __PACKED_DECODE_CHANNEL(g),
            __PACKED_DECODE_CHANNEL(b),
            __PACKED_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(ABGR_2101010);

///////////////////////////////////////////////////////////////////////////////
// 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[1])) raw_pixel<pixel_format::R_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::R_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            0.0f,
            0.0f,
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(R_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[2])) raw_pixel<pixel_format::RG_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[2];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RG_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            __ARRAY_DECODE_CHANNEL(g),
            0.0f,
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(RG_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[3])) raw_pixel<pixel_format::RGB_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = __ARRAY_ENCODE_CHANNEL(b);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            __ARRAY_DECODE_CHANNEL(g),
            __ARRAY_DECODE_CHANNEL(b),
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(RGB_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[3])) raw_pixel<pixel_format::BGR_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGR_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = __ARRAY_ENCODE_CHANNEL(b);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            __ARRAY_DECODE_CHANNEL(g),
            __ARRAY_DECODE_CHANNEL(b),
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(BGR_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[4])) raw_pixel<pixel_format::RGBA_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = __ARRAY_ENCODE_CHANNEL(b);
        data[info.a.index] = __ARRAY_ENCODE_CHANNEL(a);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            __ARRAY_DECODE_CHANNEL(g),
            __ARRAY_DECODE_CHANNEL(b),
            __ARRAY_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(RGBA_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[4])) raw_pixel<pixel_format::BGRA_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGRA_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = __ARRAY_ENCODE_CHANNEL(b);
        data[info.a.index] = __ARRAY_ENCODE_CHANNEL(a);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            __ARRAY_DECODE_CHANNEL(g),
            __ARRAY_DECODE_CHANNEL(b),
            __ARRAY_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(BGRA_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[4])) raw_pixel<pixel_format::ABGR_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ABGR_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = __ARRAY_ENCODE_CHANNEL(b);
        data[info.a.index] = __ARRAY_ENCODE_CHANNEL(a);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            __ARRAY_DECODE_CHANNEL(g),
            __ARRAY_DECODE_CHANNEL(b),
            __ARRAY_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(ABGR_8);

///////////////////////////////////////////////////////////////////////////////
// 16 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t[1])) raw_pixel<pixel_format::R_16>
{
    using channel_type = uint16_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::R_16;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            0.0f,
            0.0f,
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(R_16);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t[2])) raw_pixel<pixel_format::RG_16>
{
    using channel_type = uint16_t;
    using pixel_type = channel_type[2];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RG_16;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            __ARRAY_DECODE_CHANNEL(g),
            0.0f,
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(RG_16);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t[3])) raw_pixel<pixel_format::RGB_16>
{
    using channel_type = uint16_t;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_16;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = __ARRAY_ENCODE_CHANNEL(b);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            __ARRAY_DECODE_CHANNEL(g),
            __ARRAY_DECODE_CHANNEL(b),
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(RGB_16);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t[4])) raw_pixel<pixel_format::RGBA_16>
{
    using channel_type = uint16_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_16;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = __ARRAY_ENCODE_CHANNEL(b);
        data[info.a.index] = __ARRAY_ENCODE_CHANNEL(a);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            __ARRAY_DECODE_CHANNEL(r),
            __ARRAY_DECODE_CHANNEL(g),
            __ARRAY_DECODE_CHANNEL(b),
            __ARRAY_DECODE_CHANNEL(a)
        );
    }
};

__STATIC_FORMAT_CHECK(RGBA_16);

///////////////////////////////////////////////////////////////////////////////
// 32 bit
///////////////////////////////////////////////////////////////////////////////

//template <>
//struct alignas(alignof(uint32_t[1])) raw_pixel<pixel_format::R_32>
//{
//    using channel_type = uint32_t;
//    using pixel_type = channel_type[1];
//    using float_type = double;
//
//    static constexpr pixel_format format = pixel_format::R_32;
//    static constexpr channel_info info = get_channel_info(format);
//    pixel_type data;
//
//    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}
//
//    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
//    {
//        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
//    }
//
//    VX_FORCE_INLINE constexpr operator math::color() const noexcept
//    {
//        return math::color(
//            __ARRAY_DECODE_CHANNEL(r),
//            0.0f,
//            0.0f,
//            1.0f
//        );
//    }
//};
//
//__STATIC_FORMAT_CHECK(R_32);

///////////////////////////////////////////////////////////////////////////////

//template <>
//struct alignas(alignof(uint32_t[2])) raw_pixel<pixel_format::RG_32>
//{
//    using channel_type = uint32_t;
//    using pixel_type = channel_type[2];
//    using float_type = double;
//
//    static constexpr pixel_format format = pixel_format::RG_32;
//    static constexpr channel_info info = get_channel_info(format);
//    pixel_type data;
//
//    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}
//
//    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
//    {
//        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
//        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
//    }
//
//    VX_FORCE_INLINE constexpr operator math::color() const noexcept
//    {
//        return math::color(
//            __ARRAY_DECODE_CHANNEL(r),
//            __ARRAY_DECODE_CHANNEL(g),
//            0.0f,
//            1.0f
//        );
//    }
//};
//
//__STATIC_FORMAT_CHECK(RG_32);

///////////////////////////////////////////////////////////////////////////////

//template <>
//struct alignas(alignof(uint32_t[3])) raw_pixel<pixel_format::RGB_32>
//{
//    using channel_type = uint32_t;
//    using pixel_type = channel_type[3];
//    using float_type = double;
//
//    static constexpr pixel_format format = pixel_format::RGB_32;
//    static constexpr channel_info info = get_channel_info(format);
//    pixel_type data;
//
//    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}
//
//    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
//    {
//        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
//        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
//        data[info.b.index] = __ARRAY_ENCODE_CHANNEL(b);
//    }
//
//    VX_FORCE_INLINE constexpr operator math::color() const noexcept
//    {
//        return math::color(
//            __ARRAY_DECODE_CHANNEL(r),
//            __ARRAY_DECODE_CHANNEL(g),
//            __ARRAY_DECODE_CHANNEL(b),
//            1.0f
//        );
//    }
//};
//
//__STATIC_FORMAT_CHECK(RGB_32);

///////////////////////////////////////////////////////////////////////////////

//template <>
//struct alignas(alignof(uint32_t[4])) raw_pixel<pixel_format::RGBA_32>
//{
//    using channel_type = uint32_t;
//    using pixel_type = channel_type[4];
//    using float_type = double;
//
//    static constexpr pixel_format format = pixel_format::RGBA_32;
//    static constexpr channel_info info = get_channel_info(format);
//    pixel_type data;
//
//    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}
//
//    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
//    {
//        data[info.r.index] = __ARRAY_ENCODE_CHANNEL(r);
//        data[info.g.index] = __ARRAY_ENCODE_CHANNEL(g);
//        data[info.b.index] = __ARRAY_ENCODE_CHANNEL(b);
//        data[info.a.index] = __ARRAY_ENCODE_CHANNEL(a);
//    }
//
//    VX_FORCE_INLINE constexpr operator math::color() const noexcept
//    {
//        return math::color(
//            __ARRAY_DECODE_CHANNEL(r),
//            __ARRAY_DECODE_CHANNEL(g),
//            __ARRAY_DECODE_CHANNEL(b),
//            __ARRAY_DECODE_CHANNEL(a)
//        );
//    }
//};
//
//__STATIC_FORMAT_CHECK(RGBA_32);

///////////////////////////////////////////////////////////////////////////////
// 32 bit float
///////////////////////////////////////////////////////////////////////////////

VX_STATIC_ASSERT(sizeof(float) == 4, "invalid float size");

template <>
struct alignas(alignof(float[1])) raw_pixel<pixel_format::R_32F>
{
    using channel_type = float;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::R_32F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = c.r;
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            data[info.r.index],
            0.0f,
            0.0f,
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(R_32F);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(float[2])) raw_pixel<pixel_format::RG_32F>
{
    using channel_type = float;
    using pixel_type = channel_type[2];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RG_32F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = c.r;
        data[info.g.index] = c.g;
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            data[info.r.index],
            data[info.g.index],
            0.0f,
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(RG_32F);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(float[3])) raw_pixel<pixel_format::RGB_32F>
{
    using channel_type = float;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_32F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = c.r;
        data[info.g.index] = c.g;
        data[info.b.index] = c.b;
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            data[info.r.index],
            data[info.g.index],
            data[info.b.index],
            1.0f
        );
    }
};

__STATIC_FORMAT_CHECK(RGB_32F);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(float[4])) raw_pixel<pixel_format::RGBA_32F>
{
    using channel_type = float;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_32F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = c.r;
        data[info.g.index] = c.g;
        data[info.b.index] = c.b;
        data[info.a.index] = c.a;
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            data[info.r.index],
            data[info.g.index],
            data[info.b.index],
            data[info.a.index]
        );
    }
};

__STATIC_FORMAT_CHECK(RGBA_32F);

///////////////////////////////////////////////////////////////////////////////

#undef __STATIC_CHECK_SIZE
#undef __STATIC_CHECK_ALIGNMENT
#undef __STATIC_FORMAT_CHECK

#undef __PACKED_ENCODE_CHANNEL
#undef __PACKED_DECODE_CHANNEL

#undef __ARRAY_ENCODE_CHANNEL
#undef __ARRAY_DECODE_CHANNEL

} // namespace pixel
} // namespace vx