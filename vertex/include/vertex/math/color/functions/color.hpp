#pragma once

#include "vertex/math/color/types/color.hpp"

namespace vx {
namespace math {

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

} // namespace math
} // namespace vx