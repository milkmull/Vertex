#pragma once

namespace vx {
namespace math {

// =============== lerp ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T lerp(T x, T y, T t)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

// =============== step ===============

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool>>
inline constexpr T step(T edge, T x)
{
    return (x < edge) ? static_cast<T>(0) : static_cast<T>(1);
}

// =============== smoothstep ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T smoothstep(T edge0, T edge1, T x)
{
    const T v = (x - edge0) / (edge1 - edge0);
    const T t = std::clamp(v, static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

}
}