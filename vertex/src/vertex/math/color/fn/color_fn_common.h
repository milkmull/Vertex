#pragma once

#include "../type/color.h"
#include "../../math/fn/fn_common.h"

namespace vx {
namespace math {

// =============== min ===============

template <typename T>
inline constexpr color_t<T> min(
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

// =============== max ===============

template <typename T>
inline constexpr color_t<T> max(
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

// =============== clamp ===============

/**
 * @brief Clamps the color components within the range [0, 1].
 *
 * This function clamps the color components of the color to the range [0, 1].
 * This function is only enabled for floating-point color types.
 *
 * @return The color with its components clamped to the range [0, 1].
 */
template <typename T>
inline constexpr color_t<T> clamp(
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

// =============== color attributes ===============

/**
 * @brief Calculates the luminance of the color.
 *
 * This function calculates the luminance of the color using the formula:
 * luminance = (0.2126 * r) + (0.7152 * g) + (0.0722 * b)
 *
 * @return The luminance of the color.
 */
inline constexpr float luminance(const color& c)
{
    return (c.r * 0.2126f) + (c.g * 0.7152f) + (c.b * 0.0722f);
}

// =============== color modifiers ===============

/**
 * @brief Creates a lighter version of the color.
 *
 * Returns a new color that is lightened by the specified amount.
 *
 * @param amount The amount by which to lighten the color. Should be in the range [0, 1].
 * @return The new color lightened by the specified amount.
 */
inline constexpr color lighten(const color& c, float amount)
{
    return color(
        c.r + (1.0f - c.r) * amount,
        c.g + (1.0f - c.g) * amount,
        c.b + (1.0f - c.b) * amount,
        c.a
    );
}

/**
 * @brief Creates a darker version of the color.
 *
 * Returns a new color that is darkened by the specified amount.
 *
 * @param amount The amount by which to darken the color. Should be in the range [0, 1].
 * @return The new color darkened by the specified amount.
 */
inline constexpr color darken(const color& c, float amount)
{
    return color(
        c.r * (1.0f - amount),
        c.g * (1.0f - amount),
        c.b * (1.0f - amount),
        c.a
    );
}

// =============== invert ===============

template <typename T>
inline constexpr color_t<T> invert(const color_t<T>& c)
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
