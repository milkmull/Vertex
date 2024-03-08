#pragma once

#include "../../../math/fn/fn_common.h"

// Based on the work of Stefan Gustavson and Ashima Arts:
// https://github.com/ashima/webgl-noise

namespace vx {
namespace math {
namespace detail {

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T mod289(T x)
{
    return x - floor(x * (static_cast<T>(1) / static_cast<T>(289))) * static_cast<T>(289);
}

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> mod289(const vec<L, T>& x)
{
    return x - floor(x * (static_cast<T>(1) / static_cast<T>(289))) * static_cast<T>(289);
}

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> mod7(const vec<L, T>& x)
{
    return x - floor(x * (static_cast<T>(1) / static_cast<T>(7))) * static_cast<T>(7);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T permute(T x)
{
    return mod289(((x * static_cast<T>(34)) + static_cast<T>(1)) * x);
}

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> permute(const vec<L, T>& x)
{
    return mod289(((x * static_cast<T>(34)) + static_cast<T>(1)) * x);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T taylor_inv_sqrt(T r)
{
    return static_cast<T>(1.79284291400159) - static_cast<T>(0.85373472095314) * r;
}

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> taylor_inv_sqrt(const vec<L, T>& r)
{
    return static_cast<T>(1.79284291400159) - static_cast<T>(0.85373472095314) * r;
}

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> fade(const vec<L, T>& t)
{
    return (t * t * t) * (t * (t * static_cast<T>(6) - static_cast<T>(15)) + static_cast<T>(10));
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<4, T> grad4(T j, const vec<4, T>& ip)
{
    vec<3, T> pxyz = floor(fract(vec<3, T>(j) * vec<3, T>(ip)) * static_cast<T>(7)) * ip.z - static_cast<T>(1);
    T pw = static_cast<T>(1.5) - dot(abs(pxyz), vec<3, T>(static_cast<T>(1)));
    vec<4, T> s = static_cast<T>(1) - step(vec<4, T>(static_cast<T>(0)), vec<4, T>(pxyz, pw));
    pxyz += (vec<3, T>(s) * static_cast<T>(2) - static_cast<T>(1)) * s.w;
    return vec<4, T>(pxyz, pw);
}

}
}
}