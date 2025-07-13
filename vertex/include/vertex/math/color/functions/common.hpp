#pragma once

#include "vertex/math/color/types/color.hpp"
#include "vertex/math/core/functions/common.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// min
///////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T>
VX_FORCE_INLINE constexpr color_t<T> min(
    const color_t<T>& x,
    T y
) noexcept
{
    return color_t<T>(
        min(x.r, y),
        min(x.g, y),
        min(x.b, y),
        min(x.a, y)
    );
}

// binary

template <typename T>
VX_FORCE_INLINE constexpr color_t<T> min(
    const color_t<T>& x,
    const color_t<T>& y
) noexcept
{
    return color_t<T>(
        min(x.r, y.r),
        min(x.g, y.g),
        min(x.b, y.b),
        min(x.a, y.a)
    );
}

///////////////////////////////////////////////////////////////////////////////
// max
///////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T>
VX_FORCE_INLINE constexpr color_t<T> max(
    const color_t<T>& x,
    T y
) noexcept
{
    return color_t<T>(
        max(x.r, y),
        max(x.g, y),
        max(x.b, y),
        max(x.a, y)
    );
}

// binary

template <typename T>
VX_FORCE_INLINE constexpr color_t<T> max(
    const color_t<T>& x,
    const color_t<T>& y
) noexcept
{
    return color_t<T>(
        max(x.r, y.r),
        max(x.g, y.g),
        max(x.b, y.b),
        max(x.a, y.a)
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

// scalar

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

// binary

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

} // namespace math
} // namespace vx