#pragma once

#include "../types.hpp"
#include "../functions/common.hpp"
#include "../../core/functions/exponential.hpp"

namespace vx {
namespace math {

// https://entropymine.com/imageworsener/srgbformula/

VX_FORCE_INLINE constexpr color linear_to_srgb(const color& c) noexcept
{
    color srgb = clamp(c);

    srgb.r = (srgb.r < 0.0031308f)
        ? (12.92f * srgb.r)
        : (1.055f) * pow(srgb.r, 0.4166666667f) - 0.055f;
    srgb.g = (srgb.g < 0.0031308f)
        ? (12.92f * srgb.g)
        : (1.055f) * pow(srgb.g, 0.4166666667f) - 0.055f;
    srgb.b = (srgb.b < 0.0031308f)
        ? (12.92f * srgb.b)
        : (1.055f) * pow(srgb.b, 0.4166666667f) - 0.055f;

    return srgb;
}

VX_FORCE_INLINE constexpr color srgb_to_linear(const color& c) noexcept
{
    color lrgb = clamp(c);

    lrgb.r = (lrgb.r < 0.04045f)
        ? (0.0773993808f * lrgb.r)
        : pow((lrgb.r + 0.055f) * 0.9478672986f, 2.4f);
    lrgb.g = (lrgb.g < 0.04045f)
        ? (0.0773993808f * lrgb.g)
        : pow((lrgb.g + 0.055f) * 0.9478672986f, 2.4f);
    lrgb.b = (lrgb.b < 0.04045f)
        ? (0.0773993808f * lrgb.b)
        : pow((lrgb.b + 0.055f) * 0.9478672986f, 2.4f);

    return lrgb;
}

} // namespace math
} // namespace vx