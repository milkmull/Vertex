#pragma once

#include "../types/color.hpp"
#include "../../core/functions/interpolation.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// min
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr color_t<T> min(
    const color_t<T>& x,
    const color_t<T>& y
) noexcept
{
    return color_t<T>(
        math::min(x.r, y.r),
        math::min(x.g, y.g),
        math::min(x.b, y.b),
        math::min(x.a, y.a)
    );
}

///////////////////////////////////////////////////////////////////////////////
// max
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr color_t<T> max(
    const color_t<T>& x,
    const color_t<T>& y
) noexcept
{
    return color_t<T>(
        math::max(x.r, y.r),
        math::max(x.g, y.g),
        math::max(x.b, y.b),
        math::max(x.a, y.a)
    );
}

///////////////////////////////////////////////////////////////////////////////
// cmin
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T cmin(const color_t<T>& c) noexcept
{
    return min(c.r, c.g, c.b);
}

///////////////////////////////////////////////////////////////////////////////
// cmax
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T cmax(const color_t<T>& c) noexcept
{
    return max(c.r, c.g, c.b);
}

///////////////////////////////////////////////////////////////////////////////
// clamp
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr color_t<T> clamp(
    const color_t<T>& x,
    T min = color_t<T>::min_channel_value,
    T max = color_t<T>::max_channel_value
) noexcept
{
    return color_t<T>(
        clamp(x.r, min, max),
        clamp(x.g, min, max),
        clamp(x.b, min, max),
        clamp(x.a, min, max)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr color_t<T> clamp(
    const color_t<T>& x,
    const color_t<T>& min,
    const color_t<T>& max
) noexcept
{
    return color_t<T>(
        clamp(x.r, min.r, max.r),
        clamp(x.g, min.g, max.g),
        clamp(x.b, min.b, max.b),
        clamp(x.a, min.a, max.a)
    );
}

///////////////////////////////////////////////////////////////////////////////
// luminance
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE constexpr float luminance(const color& c) noexcept
{
    return (c.r * 0.2126f) + (c.g * 0.7152f) + (c.b * 0.0722f);
}

///////////////////////////////////////////////////////////////////////////////
// modifiers
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE constexpr color lighten(const color& c, f32 amount) noexcept
{
    return color(
        c.r + (1.0f - c.r) * amount,
        c.g + (1.0f - c.g) * amount,
        c.b + (1.0f - c.b) * amount,
        c.a
    );
}

VX_FORCE_INLINE constexpr color darken(const color& c, f32 amount) noexcept
{
    return color(
        c.r * (1.0f - amount),
        c.g * (1.0f - amount),
        c.b * (1.0f - amount),
        c.a
    );
}

///////////////////////////////////////////////////////////////////////////////
// invert
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr color_t<T> invert(const color_t<T>& c) noexcept
{
    return color_t<T>(
        color_t<T>::max_channel_value - c.r,
        color_t<T>::max_channel_value - c.g,
        color_t<T>::max_channel_value - c.b,
        c.a
    );
}

///////////////////////////////////////////////////////////////////////////////
// lerp
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE constexpr color lerp(
    const color& x,
    const color& y,
    f32 t
) noexcept
{
    return x * (1.0f - t) + y * t;
}

///////////////////////////////////////////////////////////////////////////////
// mix
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE constexpr color mix(
    const color& x,
    const color& y,
    f32 t
) noexcept
{
    return lerp(x, y, t);
}

///////////////////////////////////////////////////////////////////////////////
// step
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE constexpr color step(
    f32 edge,
    const color& x
) noexcept
{
    return color(
        step(edge, x.r),
        step(edge, x.g),
        step(edge, x.b),
        step(edge, x.a)
    );
}

///////////////////////////////////////////////////////////////////////////////
// smoothstep
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE constexpr color smoothstep(
    f32 edge0,
    f32 edge1,
    const color& x
) noexcept
{
    return color(
        smoothstep(edge0, edge1, x.r),
        smoothstep(edge0, edge1, x.g),
        smoothstep(edge0, edge1, x.b),
        smoothstep(edge0, edge1, x.a)
    );
}

} // namespace math
} // namespace vx