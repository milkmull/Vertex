#pragma once

#include "../type/color.h"
#include "../../math/fn/fn_common.h"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// min
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr color_t<T> min(
    const color_t<T>& x,
    const color_t<T>& y
)
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
)
{
    return color_t<T>(
        math::max(x.r, y.r),
        math::max(x.g, y.g),
        math::max(x.b, y.b),
        math::max(x.a, y.a)
    );
}

///////////////////////////////////////////////////////////////////////////////
// clamp
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief Clamps the color components within the range [0, 1].
///
/// @return The color with its components clamped to the range [0, 1].
////////////////////////////////////////////////////////////////////////////////
template <typename T>
VX_FORCE_INLINE constexpr color_t<T> clamp(
    const color_t<T>& x,
    const color_t<T>& min = color_t<T>(color_t<T>::MIN_CHANNEL_VALUE),
    const color_t<T>& max = color_t<T>(color_t<T>::MAX_CHANNEL_VALUE)
)
{
    return color_t<T>(
        math::clamp(x.r, min.r, max.r),
        math::clamp(x.g, min.g, max.g),
        math::clamp(x.b, min.b, max.b),
        math::clamp(x.a, min.a, max.a)
    );
}

///////////////////////////////////////////////////////////////////////////////
// luminance
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief Computes the luminance of a color.
///
/// Luminance = (Red * 0.2126) + (Green * 0.7152) + (Blue * 0.0722).
///
/// @param c The color for which to compute the luminance.
/// 
/// @return The luminance value of the color.
////////////////////////////////////////////////////////////////////////////////
VX_FORCE_INLINE constexpr float luminance(const color& c)
{
    return (c.r * 0.2126f) + (c.g * 0.7152f) + (c.b * 0.0722f);
}

///////////////////////////////////////////////////////////////////////////////
// modifiers
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief Lightens a color by a specified amount.
///
/// @param c The color to lighten.
/// @param amount The amount by which to lighten the color.
/// 
/// @return The lightened color.
////////////////////////////////////////////////////////////////////////////////
VX_FORCE_INLINE constexpr color lighten(const color& c, float amount)
{
    return color(
        c.r + (1.0f - c.r) * amount,
        c.g + (1.0f - c.g) * amount,
        c.b + (1.0f - c.b) * amount,
        c.a
    );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Darkens a color by a specified amount.
///
/// @param c The color to darken.
/// @param amount The amount by which to darken the color.
/// 
/// @return The darkened color.
////////////////////////////////////////////////////////////////////////////////
VX_FORCE_INLINE constexpr color darken(const color& c, float amount)
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

////////////////////////////////////////////////////////////////////////////////
/// @brief Inverts a color.
///
/// @tparam T The type of the color components.
/// @param c The color to invert.
/// 
/// @return The inverted color.
////////////////////////////////////////////////////////////////////////////////
template <typename T>
VX_FORCE_INLINE constexpr color_t<T> invert(const color_t<T>& c)
{
    return color_t<T>(
        color_t<T>::MAX_CHANNEL_VALUE - c.r,
        color_t<T>::MAX_CHANNEL_VALUE - c.g,
        color_t<T>::MAX_CHANNEL_VALUE - c.b,
        c.a
    );
}

}
}
