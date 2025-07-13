#pragma once

#include "vertex/math/core/functions/common.hpp"

namespace vx {
namespace math {
namespace wrap {

// https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf (page 260)

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T mirror(T a) noexcept
{
    return (a >= static_cast<T>(0)) ? a : -(static_cast<T>(1) + a);
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T clamp_to_edge(T p, T size) noexcept
{
    return clamp(p, static_cast<T>(0), size - static_cast<T>(1));
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T clamp_to_border(T p, T size) noexcept
{
    return clamp(p, static_cast<T>(-1), size);
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T repeat(T p, T size) noexcept
{
    return mod(p, size);
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T mirrored_repeat(T p, T size) noexcept
{
    return (size - static_cast<T>(1)) - mirror(mod(p, static_cast<T>(2) * size) - size);
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T mirror_clamp_to_edge(T p, T size) noexcept
{
    return clamp(mirror(p), static_cast<T>(0), size - static_cast<T>(1));
}

} // namespace wrap
} // namespace math
} // namespace vx