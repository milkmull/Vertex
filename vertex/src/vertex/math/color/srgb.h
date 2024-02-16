#pragma once

#include "type/color_type.h"

namespace vx {
namespace math {
namespace color_util {

// =============== srgb ===============

// https://entropymine.com/imageworsener/srgbformula/

/**
 * @brief Convert the color to sRGB color space.
 *
 * This function performs the conversion of the color to the sRGB color space.
 *
 * @return A new color representing the sRGB color space.
 */
template <typename T = float>
inline constexpr color linear_to_srgb(const color& c)
{
    color srgb(c.clamp());

    srgb.r = (srgb.r < static_cast<T>(0.0031308))
        ? (static_cast<T>(12.92) * srgb.r)
        : (static_cast<T>(1.055)) * math::pow(srgb.r, static_cast<T>(0.4166666667)) - static_cast<T>(0.055);
    srgb.g = (srgb.g < static_cast<T>(0.0031308))
        ? (static_cast<T>(12.92) * srgb.g)
        : (static_cast<T>(1.055)) * math::pow(srgb.g, static_cast<T>(0.4166666667)) - static_cast<T>(0.055);
    srgb.b = (srgb.b < static_cast<T>(0.0031308))
        ? (static_cast<T>(12.92) * srgb.b)
        : (static_cast<T>(1.055)) * math::pow(srgb.b, static_cast<T>(0.4166666667)) - static_cast<T>(0.055);

    return srgb;
}

/**
 * @brief Convert the color to linear color space.
 *
 * This function performs the conversion of the color to the linear color space.
 *
 * @return A new color representing the linear color space.
 */
template <typename T = float>
inline constexpr color srgb_to_linear(const color& c)
{
    color lrgb(c.clamp());

    lrgb.r = (lrgb.r < static_cast<T>(0.04045))
        ? (static_cast<T>(0.0773993808) * lrgb.r)
        : math::pow((lrgb.r + static_cast<T>(0.055)) * static_cast<T>(0.9478672986), static_cast<T>(2.4));
    lrgb.g = (lrgb.g < static_cast<T>(0.04045))
        ? (static_cast<T>(0.0773993808) * lrgb.g)
        : math::pow((lrgb.g + static_cast<T>(0.055)) * static_cast<T>(0.9478672986), static_cast<T>(2.4));
    lrgb.b = (lrgb.b < static_cast<T>(0.04045))
        ? (static_cast<T>(0.0773993808) * lrgb.b)
        : math::pow((lrgb.b + static_cast<T>(0.055)) * static_cast<T>(0.9478672986), static_cast<T>(2.4));

    return lrgb;
}

}
}
}