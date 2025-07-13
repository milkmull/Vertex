#pragma once

#include "vertex/math/color/functions/common.hpp"
#include "vertex/math/core/functions/interpolation.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// lerp
///////////////////////////////////////////////////////////////////////////////

VXM_FORCE_TEMPLATE()
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

VXM_FORCE_TEMPLATE()
VX_FORCE_INLINE constexpr color mix(
    const color& x,
    const color& y,
    f32 t
) noexcept
{
    return lerp(x, y, t);
}

//////////////////////////////////////////////////////////////////////////////
// lerp_clamped
//////////////////////////////////////////////////////////////////////////////

VXM_FORCE_TEMPLATE()
VX_FORCE_INLINE constexpr color lerp_clamped(
    const color& x,
    const color& y,
    f32 t
) noexcept
{
    return clamp(lerp(x, y, t), x, y);
}

///////////////////////////////////////////////////////////////////////////////
// step
///////////////////////////////////////////////////////////////////////////////

VXM_FORCE_TEMPLATE()
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

VXM_FORCE_TEMPLATE()
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