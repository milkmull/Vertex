#pragma once

#include "util/format_traits.h"
#include "vertex/math/color/type/color.h"

namespace vx {
namespace img {

// =============== pixel ===============

template <image_format format> struct pixel;

template<typename T>
struct is_pixel : std::false_type {};

template<image_format format>
struct is_pixel<pixel<format>> : std::true_type {};

// =============== 8 bit ===============

template <>
struct pixel<image_format::R8> : public util::format_traits<image_format::R8>
{
    channel_type r;

    VX_FORCE_INLINE constexpr pixel(channel_type r)
        : r(r) {}

    VX_FORCE_INLINE constexpr pixel(const math::color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 255.0f, 0.0f, 255.0f))) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            r / 255.0f,
            0.0f,
            0.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RG8> : public util::format_traits<image_format::RG8>
{
    channel_type r, g;

    VX_FORCE_INLINE constexpr pixel(
        channel_type r,
        channel_type g
    )
        : r(r), g(g) {}

    VX_FORCE_INLINE constexpr pixel(const math::color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 255.0f, 0.0f, 255.0f)))
        , g(static_cast<channel_type>(math::clamp(c.g * 255.0f, 0.0f, 255.0f))) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            r / 255.0f,
            g / 255.0f,
            0.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RGB8> : public util::format_traits<image_format::RGB8>
{
    channel_type r, g, b;

    VX_FORCE_INLINE constexpr pixel(
        channel_type r,
        channel_type g,
        channel_type b
    )
        : r(r), g(g), b(b) {}

    VX_FORCE_INLINE constexpr pixel(const math::color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 255.0f, 0.0f, 255.0f)))
        , g(static_cast<channel_type>(math::clamp(c.g * 255.0f, 0.0f, 255.0f)))
        , b(static_cast<channel_type>(math::clamp(c.b * 255.0f, 0.0f, 255.0f))) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            r / 255.0f,
            g / 255.0f,
            b / 255.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RGBA8> : public util::format_traits<image_format::RGBA8>
{
    channel_type r, g, b, a;

    VX_FORCE_INLINE constexpr pixel(
        channel_type r,
        channel_type g,
        channel_type b,
        channel_type a
    )
        : r(r), g(g), b(b), a(a) {}

    VX_FORCE_INLINE constexpr pixel(const math::color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 255.0f, 0.0f, 255.0f)))
        , g(static_cast<channel_type>(math::clamp(c.g * 255.0f, 0.0f, 255.0f)))
        , b(static_cast<channel_type>(math::clamp(c.b * 255.0f, 0.0f, 255.0f)))
        , a(static_cast<channel_type>(math::clamp(c.a * 255.0f, 0.0f, 255.0f))) {}

    VX_FORCE_INLINE constexpr operator math::color() const
    {
        return math::color(
            r / 255.0f,
            g / 255.0f,
            b / 255.0f,
            a / 255.0f
        );
    }
};

// =============== types ===============

using pixel_r8 = pixel<image_format::R8>;
using pixel_rg8 = pixel<image_format::RG8>;
using pixel_rgb8 = pixel<image_format::RGB8>;
using pixel_rgba8 = pixel<image_format::RGBA8>;

}
}