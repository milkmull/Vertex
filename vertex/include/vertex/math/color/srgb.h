#pragma once

#include "../math/fn/fn_exponential.h"
#include "fn/color_fn_common.h"

namespace vx {
namespace math {

// https://entropymine.com/imageworsener/srgbformula/

////////////////////////////////////////////////////////////////////////////////
/// @brief Converts a linear color to sRGB color space.
///
/// @param c The linear color to be converted.
/// 
/// @return The sRGB color corresponding to the input linear color.
////////////////////////////////////////////////////////////////////////////////
VX_FORCE_INLINE constexpr color linear_to_srgb(const color& c)
{
    using T = float;

    color srgb = math::clamp(c);

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

////////////////////////////////////////////////////////////////////////////////
/// @brief Converts an sRGB color to linear color space.
///
/// @param c The sRGB color to be converted.
/// 
/// @return The linear color corresponding to the input sRGB color.
////////////////////////////////////////////////////////////////////////////////
VX_FORCE_INLINE constexpr color srgb_to_linear(const color& c)
{
    using T = float;

    color lrgb = math::clamp(c);

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