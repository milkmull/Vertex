#pragma once

#include "vertex/pixel/pixel_format.hpp"
#include "vertex/math/core/functions/common.hpp"
#include "vertex/math/color/types/color.hpp"
#include "vertex/math/core/half.hpp"

namespace vx {
namespace pixel {

///////////////////////////////////////////////////////////////////////////////
// pixel base
///////////////////////////////////////////////////////////////////////////////

template <pixel_format f> struct raw_pixel;

#define _STATIC_CHECK_SIZE(F, FS)         VX_STATIC_ASSERT(sizeof(raw_pixel<F>) == sizeof(typename raw_pixel<F>::pixel_type), "invalid pixel size for format " FS)
#define _STATIC_CHECK_ALIGNMENT(F, FS)    VX_STATIC_ASSERT(alignof(raw_pixel<F>) == alignof(typename raw_pixel<F>::pixel_type), "invalid pixel alignment for format " FS)
#define _STATIC_FORMAT_CHECK(F)           _STATIC_CHECK_SIZE(pixel_format::F, #F); _STATIC_CHECK_ALIGNMENT(pixel_format::F, #F)

namespace _priv {

template <typename pixel_type, typename float_type>
inline constexpr pixel_type encode_packed_channel(float_type value, uint32_t mask, uint32_t shift) noexcept
{
    const float_type range = static_cast<float_type>(mask >> shift);
    const float_type scaled = value * range + static_cast<float_type>(0.5);
    const float_type clamped = math::clamp(scaled, static_cast<float_type>(0), range);
    return static_cast<pixel_type>(clamped) << shift;
}

template <typename float_type, typename pixel_type>
inline constexpr float_type decode_packed_channel(pixel_type data, uint32_t mask, uint32_t shift) noexcept
{
    const pixel_type quantized = (data & mask) >> shift;
    const float_type range = static_cast<float_type>(mask >> shift);
    return static_cast<float_type>(quantized) / range;
}

template <typename channel_type, typename float_type>
inline constexpr channel_type encode_array_channel(float_type value) noexcept
{
    constexpr float_type range = static_cast<float_type>(std::numeric_limits<channel_type>::max());
    const float_type scaled = value * range + static_cast<float_type>(0.5);
    return static_cast<channel_type>(math::clamp(scaled, static_cast<float_type>(0), range));
}

template <typename float_type, typename channel_type>
inline constexpr float_type decode_array_channel(channel_type value) noexcept
{
    constexpr float_type inv_range = static_cast<float_type>(1.0) / static_cast<float_type>(std::numeric_limits<channel_type>::max());
    return static_cast<float_type>(value) * inv_range;
}

} // namespace _priv

#define _PACKED_ENCODE_CHANNEL(x) _priv::encode_packed_channel<pixel_type, float_type>(c.x, info.x.mask, info.x.shift)
#define _PACKED_DECODE_CHANNEL(x) _priv::decode_packed_channel<float_type, pixel_type>(data, info.x.mask, info.x.shift)

#define _ARRAY_ENCODE_CHANNEL(x) _priv::encode_array_channel<channel_type, float_type>(c.x)
#define _ARRAY_DECODE_CHANNEL(x) _priv::decode_array_channel<float_type, channel_type>(data[info.x.index])

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            1.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGB_332);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGBA_4444);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(BGRA_4444);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(uint16_t) raw_pixel<pixel_format::XRGB_4444>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::XRGB_4444;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(XRGB_4444);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::XBGR_4444>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::XBGR_4444;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(XBGR_4444);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            1.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGB_565);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            1.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(BGR_565);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGBA_5551);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(BGRA_5551);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(ARGB_1555);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::XRGB_1555>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::XRGB_1555;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(XRGB_1555);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::XBGR_1555>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::XBGR_1555;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(XBGR_1555);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGBA_8888);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(BGRA_8888);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(ABGR_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::ARGB_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::ARGB_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(ABGR_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::RGBX_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBX_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGBX_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::BGRX_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::BGRX_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(BGRX_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::XRGB_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::XRGB_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(XRGB_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::XBGR_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::XBGR_8888;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(XBGR_8888);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(ARGB_2101010);

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
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b) |
        _PACKED_ENCODE_CHANNEL(a)
    )) {}

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b),
            _PACKED_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(ABGR_2101010);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::XRGB_2101010>
{
    using channel_type = uint16_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::XRGB_2101010;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(XRGB_2101010);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::XBGR_2101010>
{
    using channel_type = uint16_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::XBGR_2101010;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE constexpr raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE constexpr raw_pixel(const math::color& c) noexcept : data(static_cast<pixel_type>(
        _PACKED_ENCODE_CHANNEL(r) |
        _PACKED_ENCODE_CHANNEL(g) |
        _PACKED_ENCODE_CHANNEL(b)
        )) {
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _PACKED_DECODE_CHANNEL(r),
            _PACKED_DECODE_CHANNEL(g),
            _PACKED_DECODE_CHANNEL(b)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(XBGR_2101010);

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
        data[info.r.index] = _ARRAY_ENCODE_CHANNEL(r);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _ARRAY_DECODE_CHANNEL(r),
            0.0f,
            0.0f,
            1.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(R_8);

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
        data[info.r.index] = _ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = _ARRAY_ENCODE_CHANNEL(g);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _ARRAY_DECODE_CHANNEL(r),
            _ARRAY_DECODE_CHANNEL(g),
            0.0f,
            1.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RG_8);

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
        data[info.r.index] = _ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = _ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = _ARRAY_ENCODE_CHANNEL(b);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _ARRAY_DECODE_CHANNEL(r),
            _ARRAY_DECODE_CHANNEL(g),
            _ARRAY_DECODE_CHANNEL(b),
            1.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGB_8);

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
        data[info.r.index] = _ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = _ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = _ARRAY_ENCODE_CHANNEL(b);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _ARRAY_DECODE_CHANNEL(r),
            _ARRAY_DECODE_CHANNEL(g),
            _ARRAY_DECODE_CHANNEL(b),
            1.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(BGR_8);

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
        data[info.r.index] = _ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = _ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = _ARRAY_ENCODE_CHANNEL(b);
        data[info.a.index] = _ARRAY_ENCODE_CHANNEL(a);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _ARRAY_DECODE_CHANNEL(r),
            _ARRAY_DECODE_CHANNEL(g),
            _ARRAY_DECODE_CHANNEL(b),
            _ARRAY_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGBA_8);

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
        data[info.r.index] = _ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = _ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = _ARRAY_ENCODE_CHANNEL(b);
        data[info.a.index] = _ARRAY_ENCODE_CHANNEL(a);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _ARRAY_DECODE_CHANNEL(r),
            _ARRAY_DECODE_CHANNEL(g),
            _ARRAY_DECODE_CHANNEL(b),
            _ARRAY_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(BGRA_8);

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
        data[info.r.index] = _ARRAY_ENCODE_CHANNEL(r);
        data[info.g.index] = _ARRAY_ENCODE_CHANNEL(g);
        data[info.b.index] = _ARRAY_ENCODE_CHANNEL(b);
        data[info.a.index] = _ARRAY_ENCODE_CHANNEL(a);
    }

    VX_FORCE_INLINE constexpr operator math::color() const noexcept
    {
        return math::color(
            _ARRAY_DECODE_CHANNEL(r),
            _ARRAY_DECODE_CHANNEL(g),
            _ARRAY_DECODE_CHANNEL(b),
            _ARRAY_DECODE_CHANNEL(a)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(ABGR_8);

///////////////////////////////////////////////////////////////////////////////
// 16 bit float
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(math::half_t[1])) raw_pixel<pixel_format::R_16F>
{
    using channel_type = math::half_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::R_16F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = math::half_from_float(c.r);
    }

    VX_FORCE_INLINE operator math::color() const noexcept
    {
        return math::color(
            math::half_to_float(data[info.r.index]),
            0.0f,
            0.0f,
            1.0f
        );
    }

    VX_FORCE_INLINE auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(R_16F);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(math::half_t[1])) raw_pixel<pixel_format::RG_16F>
{
    using channel_type = math::half_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RG_16F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = math::half_from_float(c.r);
        data[info.g.index] = math::half_from_float(c.g);
    }

    VX_FORCE_INLINE operator math::color() const noexcept
    {
        return math::color(
            math::half_to_float(data[info.r.index]),
            math::half_to_float(data[info.g.index]),
            0.0f,
            1.0f
        );
    }

    VX_FORCE_INLINE auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RG_16F);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(math::half_t[1])) raw_pixel<pixel_format::RGB_16F>
{
    using channel_type = math::half_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGB_16F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = math::half_from_float(c.r);
        data[info.g.index] = math::half_from_float(c.g);
        data[info.b.index] = math::half_from_float(c.b);
    }

    VX_FORCE_INLINE operator math::color() const noexcept
    {
        return math::color(
            math::half_to_float(data[info.r.index]),
            math::half_to_float(data[info.g.index]),
            math::half_to_float(data[info.b.index]),
            1.0f
        );
    }

    VX_FORCE_INLINE auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGB_16F);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(math::half_t[1])) raw_pixel<pixel_format::RGBA_16F>
{
    using channel_type = math::half_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::RGBA_16F;
    static constexpr channel_info info = get_channel_info(format);
    pixel_type data;

    VX_FORCE_INLINE raw_pixel() noexcept : data{ 0 } {}

    VX_FORCE_INLINE raw_pixel(const math::color& c) noexcept : data{ 0 }
    {
        data[info.r.index] = math::half_from_float(c.r);
        data[info.g.index] = math::half_from_float(c.g);
        data[info.b.index] = math::half_from_float(c.b);
        data[info.a.index] = math::half_from_float(c.a);
    }

    VX_FORCE_INLINE operator math::color() const noexcept
    {
        return math::color(
            math::half_to_float(data[info.r.index]),
            math::half_to_float(data[info.g.index]),
            math::half_to_float(data[info.b.index]),
            math::half_to_float(data[info.a.index])
        );
    }

    VX_FORCE_INLINE auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGBA_16F);

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

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(R_32F);

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

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RG_32F);

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

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGB_32F);

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

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(RGBA_32F);

///////////////////////////////////////////////////////////////////////////////

#undef _STATIC_CHECK_SIZE
#undef _STATIC_CHECK_ALIGNMENT
#undef _STATIC_FORMAT_CHECK

#undef _PACKED_ENCODE_CHANNEL
#undef _PACKED_DECODE_CHANNEL

#undef _ARRAY_ENCODE_CHANNEL
#undef _ARRAY_DECODE_CHANNEL

} // namespace pixel
} // namespace vx