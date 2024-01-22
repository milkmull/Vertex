#pragma once

#include "format.h"
#include "vertex/math/color/color.h"

namespace vx {
namespace img {
namespace detail {

// =============== pixel ===============

template <image_format format> struct pixel;

// =============== 8 bit ===============

template <>
struct pixel<image_format::R8> : public format_traits<image_format::R8>
{
    channel_type r;

    inline constexpr pixel(channel_type r)
        : r(r) {}

    inline constexpr pixel(const color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 255.0f, 0.0f, 255.0f))) {}

    inline constexpr operator color() const
    {
        return color(
            r / 255.0f,
            0.0f,
            0.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RG8> : public format_traits<image_format::RG8>
{
    channel_type r, g;

    inline constexpr pixel(
        channel_type r,
        channel_type g
    )
        : r(r), g(g) {}

    inline constexpr pixel(const color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 255.0f, 0.0f, 255.0f)))
        , g(static_cast<channel_type>(math::clamp(c.g * 255.0f, 0.0f, 255.0f))) {}

    inline constexpr operator color() const
    {
        return color(
            r / 255.0f,
            g / 255.0f,
            0.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RGB8> : public format_traits<image_format::RGB8>
{
    channel_type r, g, b;

    inline constexpr pixel(
        channel_type r,
        channel_type g,
        channel_type b
    )
        : r(r), g(g), b(b) {}

    inline constexpr pixel(const color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 255.0f, 0.0f, 255.0f)))
        , g(static_cast<channel_type>(math::clamp(c.g * 255.0f, 0.0f, 255.0f)))
        , b(static_cast<channel_type>(math::clamp(c.b * 255.0f, 0.0f, 255.0f))) {}

    inline constexpr operator color() const
    {
        return color(
            r / 255.0f,
            g / 255.0f,
            b / 255.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RGBA8> : public format_traits<image_format::RGBA8>
{
    channel_type r, g, b, a;

    inline constexpr pixel(
        channel_type r,
        channel_type g,
        channel_type b,
        channel_type a
    )
        : r(r), g(g), b(b), a(a) {}

    inline constexpr pixel(const color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 255.0f, 0.0f, 255.0f)))
        , g(static_cast<channel_type>(math::clamp(c.g * 255.0f, 0.0f, 255.0f)))
        , b(static_cast<channel_type>(math::clamp(c.b * 255.0f, 0.0f, 255.0f)))
        , a(static_cast<channel_type>(math::clamp(c.a * 255.0f, 0.0f, 255.0f))) {}

    inline constexpr operator color() const
    {
        return color(
            r / 255.0f,
            g / 255.0f,
            b / 255.0f,
            a / 255.0f
        );
    }
};

// =============== 16 bit ===============

template <>
struct pixel<image_format::R16> : public format_traits<image_format::R16>
{
    channel_type r;

    inline constexpr pixel(channel_type r)
        : r(r) {}

    inline constexpr pixel(const color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 65535.0f, 0.0f, 65535.0f))) {}

    inline constexpr operator color() const
    {
        return color(
            r / 65535.0f,
            0.0f,
            0.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RG16> : public format_traits<image_format::RG16>
{
    channel_type r, g;

    inline constexpr pixel(
        channel_type r,
        channel_type g
    )
        : r(r), g(g) {}

    inline constexpr pixel(const color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 65535.0f, 0.0f, 65535.0f)))
        , g(static_cast<channel_type>(math::clamp(c.g * 65535.0f, 0.0f, 65535.0f))) {}

    inline constexpr operator color() const
    {
        return color(
            r / 65535.0f,
            g / 65535.0f,
            0.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RGB16> : public format_traits<image_format::RGB16>
{
    channel_type r, g, b;

    inline constexpr pixel(
        channel_type r,
        channel_type g,
        channel_type b
    )
        : r(r), g(g), b(b) {}

    inline constexpr pixel(const color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 65535.0f, 0.0f, 65535.0f)))
        , g(static_cast<channel_type>(math::clamp(c.g * 65535.0f, 0.0f, 65535.0f)))
        , b(static_cast<channel_type>(math::clamp(c.b * 65535.0f, 0.0f, 65535.0f))) {}

    inline constexpr operator color() const
    {
        return color(
            r / 65535.0f,
            g / 65535.0f,
            b / 65535.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RGBA16> : public format_traits<image_format::RGBA16>
{
    channel_type r, g, b, a;

    inline constexpr pixel(
        channel_type r,
        channel_type g,
        channel_type b,
        channel_type a
    )
        : r(r), g(g), b(b), a(a) {}

    inline constexpr pixel(const color& c)
        : r(static_cast<channel_type>(math::clamp(c.r * 65535.0f, 0.0f, 65535.0f)))
        , g(static_cast<channel_type>(math::clamp(c.g * 65535.0f, 0.0f, 65535.0f)))
        , b(static_cast<channel_type>(math::clamp(c.b * 65535.0f, 0.0f, 65535.0f)))
        , a(static_cast<channel_type>(math::clamp(c.a * 65535.0f, 0.0f, 65535.0f))) {}

    inline constexpr operator color() const
    {
        return color(
            r / 65535.0f,
            g / 65535.0f,
            b / 65535.0f,
            a / 65535.0f
        );
    }
};

// =============== 32 bit float ===============

template <>
struct pixel<image_format::R32F> : public format_traits<image_format::R32F>
{
    channel_type r;

    inline constexpr pixel(channel_type r)
        : r(r) {}

    inline constexpr pixel(const color& c)
        : r(c.r) {}

    inline constexpr operator color() const
    {
        return color(
            r,
            0.0f,
            0.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RG32F> : public format_traits<image_format::RG32F>
{
    channel_type r, g;

    inline constexpr pixel(
        channel_type r,
        channel_type g
    )
        : r(r), g(g) {}

    inline constexpr pixel(const color& c)
        : r(c.r)
        , g(c.g) {}

    inline constexpr operator color() const
    {
        return color(
            r,
            g,
            0.0f,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RGB32F> : public format_traits<image_format::RGB32F>
{
    channel_type r, g, b;

    inline constexpr pixel(
        channel_type r,
        channel_type g,
        channel_type b
    )
        : r(r), g(g), b(b) {}

    inline constexpr pixel(const color& c)
        : r(c.r)
        , g(c.g)
        , b(c.b) {}

    inline constexpr operator color() const
    {
        return color(
            r,
            g,
            b,
            0.0f
        );
    }
};

template <>
struct pixel<image_format::RGBA32F> : public format_traits<image_format::RGBA32F>
{
    channel_type r, g, b, a;

    inline constexpr pixel(
        channel_type r,
        channel_type g,
        channel_type b,
        channel_type a
    )
        : r(r), g(g), b(b), a(a) {}

    inline constexpr pixel(const color& c)
        : r(c.r)
        , g(c.g)
        , b(c.b)
        , a(c.a) {}

    inline constexpr operator color() const
    {
        return color(
            r,
            g,
            b,
            a
        );
    }
};

}
}
}