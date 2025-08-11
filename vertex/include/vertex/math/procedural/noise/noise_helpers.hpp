#pragma once

#include "vertex/math/core/functions/common.hpp"
#include "vertex/math/core/functions/interpolation.hpp"
#include "vertex/math/core/functions/geometric.hpp"

// Based on the work of Stefan Gustavson and Ashima Arts:
// https://github.com/ashima/webgl-noise

namespace vx {
namespace math {
namespace _priv {

////////////////////////////////////////////////////////////////////////////////
// mod289
////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T mod289(T x) noexcept
{
    return x - floor(x * (static_cast<T>(1) / static_cast<T>(289))) * static_cast<T>(289);
}

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> mod289(const vec<L, T>& x) noexcept
{
    return x - floor(x * (static_cast<T>(1) / static_cast<T>(289))) * static_cast<T>(289);
}

////////////////////////////////////////////////////////////////////////////////
// mod7
////////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> mod7(const vec<L, T>& x) noexcept
{
    return x - floor(x * (static_cast<T>(1) / static_cast<T>(7))) * static_cast<T>(7);
}

////////////////////////////////////////////////////////////////////////////////
// permute
////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T permute(T x) noexcept
{
    return mod289(((x * static_cast<T>(34)) + static_cast<T>(1)) * x);
}

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> permute(const vec<L, T>& x) noexcept
{
    return mod289(((x * static_cast<T>(34)) + static_cast<T>(1)) * x);
}

////////////////////////////////////////////////////////////////////////////////
// taylor_inv_sqrt
////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T taylor_inv_sqrt(T r) noexcept
{
    return static_cast<T>(1.79284291400159) - static_cast<T>(0.85373472095314) * r;
}

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> taylor_inv_sqrt(const vec<L, T>& r) noexcept
{
    return static_cast<T>(1.79284291400159) - static_cast<T>(0.85373472095314) * r;
}

////////////////////////////////////////////////////////////////////////////////
// fade
////////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> fade(const vec<L, T>& t) noexcept
{
    return (t * t * t) * (t * (t * static_cast<T>(6) - static_cast<T>(15)) + static_cast<T>(10));
}

////////////////////////////////////////////////////////////////////////////////
// grad4
////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> grad4(T j, const vec<4, T>& ip) noexcept
{
    vec<3, T> pxyz = floor(fract(vec<3, T>(j) * vec<3, T>(ip)) * static_cast<T>(7)) * ip.z - static_cast<T>(1);
    const T pw = static_cast<T>(1.5) - dot(abs(pxyz), vec<3, T>(static_cast<T>(1)));
    const vec<4, T> s = static_cast<T>(1) - step(vec<4, T>(static_cast<T>(0)), vec<4, T>(pxyz, pw));
    pxyz += (vec<3, T>(s) * static_cast<T>(2) - static_cast<T>(1)) * s.w;
    return vec<4, T>(pxyz, pw);
}

} // namespace _priv
} // namespace math
} // namespace vx