#pragma once

#include "../../../math/fn/fn_common.h"

// Based on the work of Stefan Gustavson and Ashima Arts:
// https://github.com/ashima/webgl-noise

namespace vx {
namespace math {
namespace detail {

template <typename T>
inline constexpr T mod289(T x)
{
    return x - math::floor(x * (static_cast<T>(1) / static_cast<T>(289))) * static_cast<T>(289);
}

template <typename T>
inline constexpr T permute(T x)
{
    return mod289(((x * static_cast<T>(34)) + static_cast<T>(1)) * x);
}

template <size_type L, typename T>
inline constexpr vec<L, T> permute(const vec<L, T>& x)
{
    return mod289(((x * static_cast<T>(34)) + static_cast<T>(1)) * x);
}

template <typename T>
inline constexpr T taylor_inv_sqrt(T r)
{
    return static_cast<T>(1.79284291400159) - static_cast<T>(0.85373472095314) * r;
}

template <size_type L, typename T>
inline constexpr vec<L, T> taylor_inv_sqrt(const vec<L, T>& r)
{
    return static_cast<T>(1.79284291400159) - static_cast<T>(0.85373472095314) * r;
}

template <size_type L, typename T>
inline constexpr vec<L, T> fade(const vec<L, T>& t)
{
    return (t * t * t) * (t * (t * static_cast<T>(6) - static_cast<T>(15)) + static_cast<T>(10));
}

}
}
}