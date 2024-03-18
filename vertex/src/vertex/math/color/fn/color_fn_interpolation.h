#pragma once

#include "../type/color.h"
#include "../../math/fn/fn_interpolation.h"

namespace vx {
namespace math {

// =============== lerp ===============

VX_FORCE_INLINE constexpr color lerp(
    const color& x,
    const color& y,
    float t
)
{
    return x * (1.0f - t) + y * t;
}

// =============== mix ===============

VX_FORCE_INLINE constexpr color mix(
    const color& x,
    const color& y,
    float t
)
{
    return lerp(x, y, t);
}

// =============== step ===============

VX_FORCE_INLINE constexpr color step(
    float edge,
    const color& x
)
{
    return color(
        step(edge, x.r),
        step(edge, x.g),
        step(edge, x.b),
        step(edge, x.a)
    );
}

// =============== smoothstep ===============

VX_FORCE_INLINE constexpr color smoothstep(
    float edge0,
    float edge1,
    const color& x
)
{
    return color(
        smoothstep(edge0, edge1, x.r),
        smoothstep(edge0, edge1, x.g),
        smoothstep(edge0, edge1, x.b),
        smoothstep(edge0, edge1, x.a)
    );
}

}
}