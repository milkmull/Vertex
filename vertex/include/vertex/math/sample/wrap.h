#pragma once

#include "../math/fn/fn_common.h"

namespace vx {
namespace math {
namespace wrap {

// https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf (page 260)

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T mirror(T a)
{
    return (a >= static_cast<T>(0)) ? a : -(static_cast<T>(1) + a);
}

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T clamp_to_edge(T p, T size)
{
    return math::clamp(p, static_cast<T>(0), size - static_cast<T>(1));
}

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T clamp_to_border(T p, T size)
{
    return math::clamp(p, static_cast<T>(-1), size);
}

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T repeat(T p, T size)
{
    return math::mod(p, size);
}

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T mirrored_repeat(T p, T size)
{
    return (size - static_cast<T>(1)) - mirror(math::mod(p, static_cast<T>(2) * size) - size);
}

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T mirror_clamp_to_edge(T p, T size)
{
    return math::clamp(mirror(p), static_cast<T>(0), size - static_cast<T>(1));
}

}
}
}