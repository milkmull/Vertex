#pragma once

#include "pixel.h"
#include "vertex/math/color/type/color.h"

namespace vx {
namespace pixel {

///////////////////////////////////////////////////////////////////////////////
// pixel base
///////////////////////////////////////////////////////////////////////////////

template <pixel_format f> struct raw_pixel;

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

    VX_FORCE_INLINE constexpr raw_pixel() {}
    VX_FORCE_INLINE constexpr raw_pixel(const math::color&) {}
    VX_FORCE_INLINE constexpr operator math::color() const { return math::color(); }
};

///////////////////////////////////////////////////////////////////////////////
// 332
///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGB_332>
{
    using channel_type = uint8_t;
    using pixel_type = uint8_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_332;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGB_332>) == get_pixel_size(pixel_format::RGB_332));

///////////////////////////////////////////////////////////////////////////////
// 4444
///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGBA_4444>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_4444;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift) |
        ((pixel_type)(math::clamp(c.a * (info.a.mask >> info.a.shift), 0.0f, (float)(info.a.mask >> info.a.shift))) << info.a.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            (float)((data & info.a.mask) >> info.a.shift) / (info.a.mask >> info.a.shift)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGBA_4444>) == get_pixel_size(pixel_format::RGBA_4444));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::BGRA_4444>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGRA_4444;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift) |
        ((pixel_type)(math::clamp(c.a * (info.a.mask >> info.a.shift), 0.0f, (float)(info.a.mask >> info.a.shift))) << info.a.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            (float)((data & info.a.mask) >> info.a.shift) / (info.a.mask >> info.a.shift)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::BGRA_4444>) == get_pixel_size(pixel_format::BGRA_4444));

///////////////////////////////////////////////////////////////////////////////
// 565
///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGB_565>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_565;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGB_565>) == get_pixel_size(pixel_format::RGB_565));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::BGR_565>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_565;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::BGR_565>) == get_pixel_size(pixel_format::BGR_565));

///////////////////////////////////////////////////////////////////////////////
// 5551 & 1555
///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGBA_5551>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_5551;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift) |
        ((pixel_type)(math::clamp(c.a * (info.a.mask >> info.a.shift), 0.0f, (float)(info.a.mask >> info.a.shift))) << info.a.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            (float)((data & info.a.mask) >> info.a.shift) / (info.a.mask >> info.a.shift)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGBA_5551>) == get_pixel_size(pixel_format::RGBA_5551));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::BGRA_5551>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGRA_5551;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift) |
        ((pixel_type)(math::clamp(c.a * (info.a.mask >> info.a.shift), 0.0f, (float)(info.a.mask >> info.a.shift))) << info.a.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            (float)((data & info.a.mask) >> info.a.shift) / (info.a.mask >> info.a.shift)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::BGRA_5551>) == get_pixel_size(pixel_format::BGRA_5551));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::ARGB_1555>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ARGB_1555;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift) |
        ((pixel_type)(math::clamp(c.a * (info.a.mask >> info.a.shift), 0.0f, (float)(info.a.mask >> info.a.shift))) << info.a.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            (float)((data & info.a.mask) >> info.a.shift) / (info.a.mask >> info.a.shift)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::ARGB_1555>) == get_pixel_size(pixel_format::ARGB_1555));

///////////////////////////////////////////////////////////////////////////////
// packed 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGBA_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift) |
        ((pixel_type)(math::clamp(c.a * (info.a.mask >> info.a.shift), 0.0f, (float)(info.a.mask >> info.a.shift))) << info.a.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            (float)((data & info.a.mask) >> info.a.shift) / (info.a.mask >> info.a.shift)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGBA_8888>) == get_pixel_size(pixel_format::RGBA_8888));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::BGRA_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGRA_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift) |
        ((pixel_type)(math::clamp(c.a * (info.a.mask >> info.a.shift), 0.0f, (float)(info.a.mask >> info.a.shift))) << info.a.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            (float)((data & info.a.mask) >> info.a.shift) / (info.a.mask >> info.a.shift)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::BGRA_8888>) == get_pixel_size(pixel_format::BGRA_8888));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::ABGR_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ABGR_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r * (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift) |
        ((pixel_type)(math::clamp(c.a * (info.a.mask >> info.a.shift), 0.0f, (float)(info.a.mask >> info.a.shift))) << info.a.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            (float)((data & info.a.mask) >> info.a.shift) / (info.a.mask >> info.a.shift)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::ABGR_8888>) == get_pixel_size(pixel_format::ABGR_8888));

///////////////////////////////////////////////////////////////////////////////
// 2101010
///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::ARGB_2101010>
{
    using channel_type = uint16_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ARGB_2101010;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r* (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift) |
        ((pixel_type)(math::clamp(c.a * (info.a.mask >> info.a.shift), 0.0f, (float)(info.a.mask >> info.a.shift))) << info.a.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            (float)((data & info.a.mask) >> info.a.shift) / (info.a.mask >> info.a.shift)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::ARGB_2101010>) == get_pixel_size(pixel_format::ARGB_2101010));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::ABGR_2101010>
{
    using channel_type = uint16_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ABGR_2101010;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data(
        ((pixel_type)(math::clamp(c.r* (info.r.mask >> info.r.shift), 0.0f, (float)(info.r.mask >> info.r.shift))) << info.r.shift) |
        ((pixel_type)(math::clamp(c.g * (info.g.mask >> info.g.shift), 0.0f, (float)(info.g.mask >> info.g.shift))) << info.g.shift) |
        ((pixel_type)(math::clamp(c.b * (info.b.mask >> info.b.shift), 0.0f, (float)(info.b.mask >> info.b.shift))) << info.b.shift) |
        ((pixel_type)(math::clamp(c.a * (info.a.mask >> info.a.shift), 0.0f, (float)(info.a.mask >> info.a.shift))) << info.a.shift)
    ) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)((data & info.r.mask) >> info.r.shift) / (info.r.mask >> info.r.shift),
            (float)((data & info.g.mask) >> info.g.shift) / (info.g.mask >> info.g.shift),
            (float)((data & info.b.mask) >> info.b.shift) / (info.b.mask >> info.b.shift),
            (float)((data & info.a.mask) >> info.a.shift) / (info.a.mask >> info.a.shift)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::ABGR_2101010>) == get_pixel_size(pixel_format::ABGR_2101010));

///////////////////////////////////////////////////////////////////////////////
// 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::R_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::R_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            0.0f,
            0.0f,
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::R_8>) == get_pixel_size(pixel_format::R_8));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RG_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[2];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RG_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.g.index] = (channel_type)(math::clamp(c.g * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            (float)(data[info.g.index]) / (channel_type)(~0),
            0.0f,
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RG_8>) == get_pixel_size(pixel_format::RG_8));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGB_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.g.index] = (channel_type)(math::clamp(c.g * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.b.index] = (channel_type)(math::clamp(c.b * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            (float)(data[info.g.index]) / (channel_type)(~0),
            (float)(data[info.b.index]) / (channel_type)(~0),
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGB_8>) == get_pixel_size(pixel_format::RGB_8));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::BGR_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGR_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.g.index] = (channel_type)(math::clamp(c.g * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.b.index] = (channel_type)(math::clamp(c.b * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            (float)(data[info.g.index]) / (channel_type)(~0),
            (float)(data[info.b.index]) / (channel_type)(~0),
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::BGR_8>) == get_pixel_size(pixel_format::BGR_8));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGBA_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.g.index] = (channel_type)(math::clamp(c.g * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.b.index] = (channel_type)(math::clamp(c.b * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.a.index] = (channel_type)(math::clamp(c.a * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            (float)(data[info.g.index]) / (channel_type)(~0),
            (float)(data[info.b.index]) / (channel_type)(~0),
            (float)(data[info.a.index]) / (channel_type)(~0)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGBA_8>) == get_pixel_size(pixel_format::RGBA_8));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::BGRA_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGRA_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.g.index] = (channel_type)(math::clamp(c.g * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.b.index] = (channel_type)(math::clamp(c.b * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.a.index] = (channel_type)(math::clamp(c.a * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            (float)(data[info.g.index]) / (channel_type)(~0),
            (float)(data[info.b.index]) / (channel_type)(~0),
            (float)(data[info.a.index]) / (channel_type)(~0)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::BGRA_8>) == get_pixel_size(pixel_format::BGRA_8));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::ABGR_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ABGR_8;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.g.index] = (channel_type)(math::clamp(c.g * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.b.index] = (channel_type)(math::clamp(c.b * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.a.index] = (channel_type)(math::clamp(c.a * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            (float)(data[info.g.index]) / (channel_type)(~0),
            (float)(data[info.b.index]) / (channel_type)(~0),
            (float)(data[info.a.index]) / (channel_type)(~0)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::ABGR_8>) == get_pixel_size(pixel_format::ABGR_8));

///////////////////////////////////////////////////////////////////////////////
// 16 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::R_16>
{
    using channel_type = uint16_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::R_16;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            0.0f,
            0.0f,
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::R_16>) == get_pixel_size(pixel_format::R_16));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RG_16>
{
    using channel_type = uint16_t;
    using pixel_type = channel_type[2];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RG_16;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.g.index] = (channel_type)(math::clamp(c.g * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            (float)(data[info.g.index]) / (channel_type)(~0),
            0.0f,
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RG_16>) == get_pixel_size(pixel_format::RG_16));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGB_16>
{
    using channel_type = uint16_t;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_16;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.g.index] = (channel_type)(math::clamp(c.g * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.b.index] = (channel_type)(math::clamp(c.b * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            (float)(data[info.g.index]) / (channel_type)(~0),
            (float)(data[info.b.index]) / (channel_type)(~0),
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGB_16>) == get_pixel_size(pixel_format::RGB_16));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGBA_16>
{
    using channel_type = uint16_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_16;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = (channel_type)(math::clamp(c.r * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.g.index] = (channel_type)(math::clamp(c.g * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.b.index] = (channel_type)(math::clamp(c.b * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
        data[info.a.index] = (channel_type)(math::clamp(c.a * (channel_type)(~0), 0.0f, (float)((channel_type)(~0))));
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            (float)(data[info.r.index]) / (channel_type)(~0),
            (float)(data[info.g.index]) / (channel_type)(~0),
            (float)(data[info.b.index]) / (channel_type)(~0),
            (float)(data[info.a.index]) / (channel_type)(~0)
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGBA_16>) == get_pixel_size(pixel_format::RGBA_16));

///////////////////////////////////////////////////////////////////////////////
// 32 bit
///////////////////////////////////////////////////////////////////////////////

//template <>
//struct raw_pixel<pixel_format::R_32>
//{
//    using channel_type = uint32_t;
//    using pixel_type = channel_type[1];
//    using float_type = double;
//
//    static constexpr pixel_format format = pixel_format::R_32;
//    static constexpr channel_info info = get_channel_info(format);
//    pixel_type data;
//
//    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}
//
//    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
//    {
//        data[info.r.index] = (channel_type)(math::clamp((float_type)(c.r * (channel_type)(~0)), (float_type)(0.0f), (float_type)((channel_type)(~0))));
//    }
//
//    VX_FORCE_INLINE constexpr operator math::color() const
//    {
//        return math::color(
//            (float_type)(data[info.r.index]) / (channel_type)(~0),
//            0.0f,
//            0.0f,
//            1.0f
//        );
//    }
//};
//
//static_assert(sizeof(raw_pixel<pixel_format::R_32>) == get_pixel_size(pixel_format::R_32));

///////////////////////////////////////////////////////////////////////////////

//template <>
//struct raw_pixel<pixel_format::RG_32>
//{
//    using channel_type = uint32_t;
//    using pixel_type = channel_type[2];
//    using float_type = double;
//
//    static constexpr pixel_format format = pixel_format::RG_32;
//    static constexpr channel_info info = get_channel_info(format);
//    pixel_type data;
//
//    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}
//
//    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
//    {
//        data[info.r.index] = (channel_type)(math::clamp((float_type)(c.r * (channel_type)(~0)), (float_type)(0.0f), (float_type)((channel_type)(~0))));
//        data[info.g.index] = (channel_type)(math::clamp((float_type)(c.g * (channel_type)(~0)), (float_type)(0.0f), (float_type)((channel_type)(~0))));
//    }
//
//    VX_FORCE_INLINE constexpr operator math::color() const
//    {
//        return math::color(
//            (float_type)(data[info.r.index]) / (channel_type)(~0),
//            (float_type)(data[info.g.index]) / (channel_type)(~0),
//            0.0f,
//            1.0f
//        );
//    }
//};
//
//static_assert(sizeof(raw_pixel<pixel_format::RG_32>) == get_pixel_size(pixel_format::RG_32));

///////////////////////////////////////////////////////////////////////////////

//template <>
//struct raw_pixel<pixel_format::RGB_32>
//{
//    using channel_type = uint32_t;
//    using pixel_type = channel_type[3];
//    using float_type = double;
//
//    static constexpr pixel_format format = pixel_format::RGB_32;
//    static constexpr channel_info info = get_channel_info(format);
//    pixel_type data;
//
//    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}
//
//    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
//    {
//        data[info.r.index] = (channel_type)(math::clamp((float_type)(c.r * (channel_type)(~0)), (float_type)(0.0f), (float_type)((channel_type)(~0))));
//        data[info.g.index] = (channel_type)(math::clamp((float_type)(c.g * (channel_type)(~0)), (float_type)(0.0f), (float_type)((channel_type)(~0))));
//        data[info.b.index] = (channel_type)(math::clamp((float_type)(c.b * (channel_type)(~0)), (float_type)(0.0f), (float_type)((channel_type)(~0))));
//    }
//
//    VX_FORCE_INLINE constexpr operator math::color() const
//    {
//        return math::color(
//            (float_type)(data[info.r.index]) / (channel_type)(~0),
//            (float_type)(data[info.g.index]) / (channel_type)(~0),
//            (float_type)(data[info.b.index]) / (channel_type)(~0),
//            1.0f
//        );
//    }
//};
//
//static_assert(sizeof(raw_pixel<pixel_format::RGB_32>) == get_pixel_size(pixel_format::RGB_32));

///////////////////////////////////////////////////////////////////////////////

//template <>
//struct raw_pixel<pixel_format::RGBA_32>
//{
//    using channel_type = uint32_t;
//    using pixel_type = channel_type[4];
//    using float_type = double;
//
//    static constexpr pixel_format format = pixel_format::RGBA_32;
//    static constexpr channel_info info = get_channel_info(format);
//    pixel_type data;
//
//    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}
//
//    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
//    {
//        data[info.r.index] = (channel_type)(math::clamp((float_type)(c.r * (channel_type)(~0)), (float_type)(0.0f), (float_type)((channel_type)(~0))));
//        data[info.g.index] = (channel_type)(math::clamp((float_type)(c.g * (channel_type)(~0)), (float_type)(0.0f), (float_type)((channel_type)(~0))));
//        data[info.b.index] = (channel_type)(math::clamp((float_type)(c.b * (channel_type)(~0)), (float_type)(0.0f), (float_type)((channel_type)(~0))));
//        data[info.a.index] = (channel_type)(math::clamp((float_type)(c.a * (channel_type)(~0)), (float_type)(0.0f), (float_type)((channel_type)(~0))));
//    }
//
//    VX_FORCE_INLINE constexpr operator math::color() const
//    {
//        return math::color(
//            (float_type)(data[info.r.index]) / (channel_type)(~0),
//            (float_type)(data[info.g.index]) / (channel_type)(~0),
//            (float_type)(data[info.b.index]) / (channel_type)(~0),
//            (float_type)(data[info.a.index]) / (channel_type)(~0)
//        );
//    }
//};
//
//static_assert(sizeof(raw_pixel<pixel_format::RGBA_32>) == get_pixel_size(pixel_format::RGBA_32));

///////////////////////////////////////////////////////////////////////////////
// 32 bit float
///////////////////////////////////////////////////////////////////////////////

static_assert(sizeof(float) == 4);

template <>
struct raw_pixel<pixel_format::R_32F>
{
    using channel_type = float;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::R_32F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = c.r;
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            data[info.r.index],
            0.0f,
            0.0f,
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::R_32F>) == get_pixel_size(pixel_format::R_32F));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RG_32F>
{
    using channel_type = float;
    using pixel_type = channel_type[2];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RG_32F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = c.r;
        data[info.g.index] = c.g;
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            data[info.r.index],
            data[info.g.index],
            0.0f,
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RG_32F>) == get_pixel_size(pixel_format::RG_32F));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGB_32F>
{
    using channel_type = float;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_32F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = c.r;
        data[info.g.index] = c.g;
        data[info.b.index] = c.b;
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            data[info.r.index],
            data[info.g.index],
            data[info.b.index],
            1.0f
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGB_32F>) == get_pixel_size(pixel_format::RGB_32F));

///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::RGBA_32F>
{
    using channel_type = float;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_32F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) : data{ 0 }
    {
        data[info.r.index] = c.r;
        data[info.g.index] = c.g;
        data[info.b.index] = c.b;
        data[info.a.index] = c.a;
    }

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            data[info.r.index],
            data[info.g.index],
            data[info.b.index],
            data[info.a.index]
        );
    }
};

static_assert(sizeof(raw_pixel<pixel_format::RGBA_32F>) == get_pixel_size(pixel_format::RGBA_32F));

///////////////////////////////////////////////////////////////////////////////

}
}