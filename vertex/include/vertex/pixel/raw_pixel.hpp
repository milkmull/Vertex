#pragma once

#include "vertex/pixel/pixel_format.hpp"
#include "vertex/math/core/functions/common.hpp"
#include "vertex/math/color/types/color.hpp"
#include "vertex/math/half.hpp"

namespace vx {
namespace pixel {

///////////////////////////////////////////////////////////////////////////////
// pixel base
///////////////////////////////////////////////////////////////////////////////

template <pixel_format f> struct raw_pixel;

#define _STATIC_CHECK_SIZE(F, FS)         VX_STATIC_ASSERT_MSG(sizeof(raw_pixel<F>) == sizeof(typename raw_pixel<F>::pixel_type), "invalid pixel size for format " FS)
#define _STATIC_CHECK_ALIGNMENT(F, FS)    VX_STATIC_ASSERT_MSG(alignof(raw_pixel<F>) == alignof(typename raw_pixel<F>::pixel_type), "invalid pixel alignment for format " FS)
#define _STATIC_FORMAT_CHECK(F)           _STATIC_CHECK_SIZE(pixel_format::F, #F); _STATIC_CHECK_ALIGNMENT(pixel_format::F, #F)

namespace _priv {

template <typename pixel_type, typename float_type>
inline constexpr pixel_type encode_packed_channel(float_type value, pixel_type mask, uint32_t shift) noexcept
{
    const float_type range = static_cast<float_type>(mask >> shift);
    const float_type scaled = value * range + static_cast<float_type>(0.5);
    const float_type clamped = math::clamp(scaled, static_cast<float_type>(0), range);
    return static_cast<pixel_type>(clamped) << shift;
}

template <typename float_type, typename pixel_type>
inline constexpr float_type decode_packed_channel(pixel_type data, pixel_type mask, uint32_t shift) noexcept
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

#define _PACKED_ENCODE_CHANNEL(x) _priv::encode_packed_channel<pixel_type, float_type>(c.x, static_cast<pixel_type>(info.x.mask), info.x.shift)
#define _PACKED_DECODE_CHANNEL(x) _priv::decode_packed_channel<float_type, pixel_type>(data, static_cast<pixel_type>(info.x.mask), info.x.shift)

#define _ARRAY_ENCODE_CHANNEL(x) _priv::encode_array_channel<channel_type, float_type>(c.x)
#define _ARRAY_DECODE_CHANNEL(x) _priv::decode_array_channel<float_type, channel_type>(data[info.x.index])

///////////////////////////////////////////////////////////////////////////////
// unknown
///////////////////////////////////////////////////////////////////////////////

template <>
struct raw_pixel<pixel_format::unknown>
{
    using channel_type = uint8_t;
    using pixel_type = void;
    using float_type = void;

    static constexpr channel_info info = get_channel_info(pixel_format::unknown);

    VX_FORCE_INLINE constexpr raw_pixel() noexcept {}
    VX_FORCE_INLINE constexpr raw_pixel(const math::color&) noexcept {}
    VX_FORCE_INLINE constexpr operator math::color() const noexcept { return math::color(); }
};

///////////////////////////////////////////////////////////////////////////////
// 332
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t)) raw_pixel<pixel_format::rgb_332>
{
    using channel_type = uint8_t;
    using pixel_type = uint8_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgb_332;
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

_STATIC_FORMAT_CHECK(rgb_332);

///////////////////////////////////////////////////////////////////////////////
// 4444
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(uint16_t) raw_pixel<pixel_format::rgba_4444>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgba_4444;
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

_STATIC_FORMAT_CHECK(rgba_4444);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::bgra_4444>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::bgra_4444;
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

_STATIC_FORMAT_CHECK(bgra_4444);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(uint16_t) raw_pixel<pixel_format::xrgb_4444>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::xrgb_4444;
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

_STATIC_FORMAT_CHECK(xrgb_4444);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::xbgr_4444>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::xbgr_4444;
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

_STATIC_FORMAT_CHECK(xbgr_4444);

///////////////////////////////////////////////////////////////////////////////
// 565
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::rgb_565>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgb_565;
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

_STATIC_FORMAT_CHECK(rgb_565);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::bgr_565>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgb_565;
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

_STATIC_FORMAT_CHECK(bgr_565);

///////////////////////////////////////////////////////////////////////////////
// 5551 & 1555
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::rgba_5551>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgba_5551;
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

_STATIC_FORMAT_CHECK(rgba_5551);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::bgra_5551>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::bgra_5551;
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

_STATIC_FORMAT_CHECK(bgra_5551);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::argb_1555>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::argb_1555;
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

_STATIC_FORMAT_CHECK(argb_1555);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::xrgb_1555>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::xrgb_1555;
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

_STATIC_FORMAT_CHECK(xrgb_1555);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint16_t)) raw_pixel<pixel_format::xbgr_1555>
{
    using channel_type = uint8_t;
    using pixel_type = uint16_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::xbgr_1555;
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

_STATIC_FORMAT_CHECK(xbgr_1555);

///////////////////////////////////////////////////////////////////////////////
// packed 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::rgba_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgba_8888;
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

_STATIC_FORMAT_CHECK(rgba_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::bgra_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::bgra_8888;
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

_STATIC_FORMAT_CHECK(bgra_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::abgr_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::abgr_8888;
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

_STATIC_FORMAT_CHECK(abgr_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::argb_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::argb_8888;
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

_STATIC_FORMAT_CHECK(abgr_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::rgbx_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgbx_8888;
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

_STATIC_FORMAT_CHECK(rgbx_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::bgrx_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::bgrx_8888;
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

_STATIC_FORMAT_CHECK(bgrx_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::xrgb_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::xrgb_8888;
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

_STATIC_FORMAT_CHECK(xrgb_8888);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::xbgr_8888>
{
    using channel_type = uint8_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::xbgr_8888;
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

_STATIC_FORMAT_CHECK(xbgr_8888);

///////////////////////////////////////////////////////////////////////////////
// 2101010
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::argb_2101010>
{
    using channel_type = uint16_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::argb_2101010;
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

_STATIC_FORMAT_CHECK(argb_2101010);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::abgr_2101010>
{
    using channel_type = uint16_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::abgr_2101010;
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

_STATIC_FORMAT_CHECK(abgr_2101010);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::xrgb_2101010>
{
    using channel_type = uint16_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::xrgb_2101010;
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

_STATIC_FORMAT_CHECK(xrgb_2101010);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint32_t)) raw_pixel<pixel_format::xbgr_2101010>
{
    using channel_type = uint16_t;
    using pixel_type = uint32_t;
    using float_type = float;

    static constexpr pixel_format format = pixel_format::xbgr_2101010;
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

_STATIC_FORMAT_CHECK(xbgr_2101010);

///////////////////////////////////////////////////////////////////////////////
// 8 bit
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[1])) raw_pixel<pixel_format::r_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::r_8;
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
            0.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(r_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[2])) raw_pixel<pixel_format::rg_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[2];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rg_8;
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
            0.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(rg_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[3])) raw_pixel<pixel_format::rgb_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgb_8;
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
            _ARRAY_DECODE_CHANNEL(b)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(rgb_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[3])) raw_pixel<pixel_format::bgr_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::bgr_8;
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
            _ARRAY_DECODE_CHANNEL(b)
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(bgr_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[4])) raw_pixel<pixel_format::rgba_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgba_8;
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

_STATIC_FORMAT_CHECK(rgba_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[4])) raw_pixel<pixel_format::bgra_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::bgra_8;
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

_STATIC_FORMAT_CHECK(bgra_8);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(uint8_t[4])) raw_pixel<pixel_format::abgr_8>
{
    using channel_type = uint8_t;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::abgr_8;
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

_STATIC_FORMAT_CHECK(abgr_8);

///////////////////////////////////////////////////////////////////////////////
// 16 bit float
///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(math::half_t[1])) raw_pixel<pixel_format::r_16f>
{
    using channel_type = math::half_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::r_16f;
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
            0.0f
        );
    }

    VX_FORCE_INLINE auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(r_16f);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(math::half_t[1])) raw_pixel<pixel_format::rg_16f>
{
    using channel_type = math::half_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rg_16f;
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
            0.0f
        );
    }

    VX_FORCE_INLINE auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(rg_16f);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(math::half_t[1])) raw_pixel<pixel_format::rgb_16f>
{
    using channel_type = math::half_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgb_16f;
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
            math::half_to_float(data[info.b.index])
        );
    }

    VX_FORCE_INLINE auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(rgb_16f);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(math::half_t[1])) raw_pixel<pixel_format::rgba_16f>
{
    using channel_type = math::half_t;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgba_16f;
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

_STATIC_FORMAT_CHECK(rgba_16f);

///////////////////////////////////////////////////////////////////////////////
// 32 bit float
///////////////////////////////////////////////////////////////////////////////

VX_STATIC_ASSERT_MSG(sizeof(float) == 4, "invalid float size");

template <>
struct alignas(alignof(float[1])) raw_pixel<pixel_format::r_32f>
{
    using channel_type = float;
    using pixel_type = channel_type[1];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::r_32f;
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
            0.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(r_32f);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(float[2])) raw_pixel<pixel_format::rg_32f>
{
    using channel_type = float;
    using pixel_type = channel_type[2];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rg_32f;
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
            0.0f
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(rg_32f);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(float[3])) raw_pixel<pixel_format::rgb_32f>
{
    using channel_type = float;
    using pixel_type = channel_type[3];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgb_32f;
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
            data[info.b.index]
        );
    }

    VX_FORCE_INLINE constexpr auto color() const noexcept { return operator math::color(); }
};

_STATIC_FORMAT_CHECK(rgb_32f);

///////////////////////////////////////////////////////////////////////////////

template <>
struct alignas(alignof(float[4])) raw_pixel<pixel_format::rgba_32f>
{
    using channel_type = float;
    using pixel_type = channel_type[4];
    using float_type = float;

    static constexpr pixel_format format = pixel_format::rgba_32f;
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

_STATIC_FORMAT_CHECK(rgba_32f);

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
