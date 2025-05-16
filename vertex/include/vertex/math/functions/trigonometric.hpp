#pragma once

#include "vertex/math/fn/common.hpp"

namespace vx {
namespace math {

//////////////////////////////////////////////////////////////////////////////
// radians
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T radians(T deg) noexcept
{
    constexpr T r = constants<T>::two_pi / static_cast<T>(360);
    return deg * r;
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr auto radians(T deg) noexcept
{
    return radians(static_cast<double>(deg));
}

//////////////////////////////////////////////////////////////////////////////
// degrees
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T degrees(T rad) noexcept
{
    constexpr T r = static_cast<T>(360) / constants<T>::two_pi;
    return rad * r;
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr auto degrees(T rad) noexcept
{
    return degrees(static_cast<double>(rad));
}

//////////////////////////////////////////////////////////////////////////////
// sin
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T sin(T x) noexcept
{
    return std::sin(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> sin(const vec<2, T>& x) noexcept
{
    return vec<2, T>(sin(x.x), sin(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> sin(const vec<3, T>& x) noexcept
{
    return vec<3, T>(sin(x.x), sin(x.y), sin(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> sin(const vec<4, T>& x) noexcept
{
    return vec<4, T>(sin(x.x), sin(x.y), sin(x.z), sin(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// cos
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T cos(T x) noexcept
{
    return std::cos(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> cos(const vec<2, T>& x) noexcept
{
    return vec<2, T>(cos(x.x), cos(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> cos(const vec<3, T>& x) noexcept
{
    return vec<3, T>(cos(x.x), cos(x.y), cos(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> cos(const vec<4, T>& x) noexcept
{
    return vec<4, T>(cos(x.x), cos(x.y), cos(x.z), cos(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// tan
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T tan(T x) noexcept
{
    return std::tan(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> tan(const vec<2, T>& x) noexcept
{
    return vec<2, T>(tan(x.x), tan(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> tan(const vec<3, T>& x) noexcept
{
    return vec<3, T>(tan(x.x), tan(x.y), tan(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> tan(const vec<4, T>& x) noexcept
{
    return vec<4, T>(tan(x.x), tan(x.y), tan(x.z), tan(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// asin
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T asin(T x) noexcept
{
    return std::asin(x);
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T asin_safe(T x) noexcept
{
    return std::asin(clamp(x, static_cast<T>(-1), static_cast<T>(1)));
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> asin(const vec<2, T>& x) noexcept
{
    return vec<2, T>(asin(x.x), asin(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> asin(const vec<3, T>& x) noexcept
{
    return vec<3, T>(asin(x.x), asin(x.y), asin(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> asin(const vec<4, T>& x) noexcept
{
    return vec<4, T>(asin(x.x), asin(x.y), asin(x.z), asin(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// acos
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T acos(T x) noexcept
{
    return std::acos(x);
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T acos_safe(T x) noexcept
{
    return std::acos(clamp(x, static_cast<T>(-1), static_cast<T>(1)));
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> acos(const vec<2, T>& x) noexcept
{
    return vec<2, T>(acos(x.x), acos(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> acos(const vec<3, T>& x) noexcept
{
    return vec<3, T>(acos(x.x), acos(x.y), acos(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> acos(const vec<4, T>& x) noexcept
{
    return vec<4, T>(acos(x.x), acos(x.y), acos(x.z), acos(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// atan
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T atan(T x) noexcept
{
    return std::atan(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> atan(const vec<2, T>& x) noexcept
{
    return vec<2, T>(atan(x.x), atan(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> atan(const vec<3, T>& x) noexcept
{
    return vec<3, T>(atan(x.x), atan(x.y), atan(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> atan(const vec<4, T>& x) noexcept
{
    return vec<4, T>(atan(x.x), atan(x.y), atan(x.z), atan(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// atan2
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T atan2(T y, T x) noexcept
{
    return std::atan2(y, x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> atan2(
    const vec<2, T>& y,
    const vec<2, T>& x
) noexcept
{
    return vec<2, T>(
        atan2(y.x, x.x),
        atan2(y.y, x.y)
    );
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> atan2(
    const vec<3, T>& y,
    const vec<3, T>& x
) noexcept
{
    return vec<3, T>(
        atan2(y.x, x.x),
        atan2(y.y, x.y),
        atan2(y.z, x.z)
    );
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> atan2(
    const vec<4, T>& y,
    const vec<4, T>& x
) noexcept
{
    return vec<4, T>(
        atan2(y.x, x.x),
        atan2(y.y, x.y),
        atan2(y.z, x.z),
        atan2(y.w, x.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// sinh
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T sinh(T x) noexcept
{
    return std::sinh(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> sinh(const vec<2, T>& x) noexcept
{
    return vec<2, T>(sinh(x.x), sinh(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> sinh(const vec<3, T>& x) noexcept
{
    return vec<3, T>(sinh(x.x), sinh(x.y), sinh(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> sinh(const vec<4, T>& x) noexcept
{
    return vec<4, T>(sinh(x.x), sinh(x.y), sinh(x.z), sinh(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// cosh
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T cosh(T x) noexcept
{
    return std::cosh(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> cosh(const vec<2, T>& x) noexcept
{
    return vec<2, T>(cosh(x.x), cosh(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> cosh(const vec<3, T>& x) noexcept
{
    return vec<3, T>(cosh(x.x), cosh(x.y), cosh(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> cosh(const vec<4, T>& x) noexcept
{
    return vec<4, T>(cosh(x.x), cosh(x.y), cosh(x.z), cosh(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// tanh
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T tanh(T x) noexcept
{
    return std::tanh(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> tanh(const vec<2, T>& x) noexcept
{
    return vec<2, T>(tanh(x.x), tanh(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> tanh(const vec<3, T>& x) noexcept
{
    return vec<3, T>(tanh(x.x), tanh(x.y), tanh(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> tanh(const vec<4, T>& x) noexcept
{
    return vec<4, T>(tanh(x.x), tanh(x.y), tanh(x.z), tanh(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// asinh
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T asinh(T x) noexcept
{
    return std::asinh(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> asinh(const vec<2, T>& x) noexcept
{
    return vec<2, T>(asinh(x.x), asinh(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> asinh(const vec<3, T>& x) noexcept
{
    return vec<3, T>(asinh(x.x), asinh(x.y), asinh(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> asinh(const vec<4, T>& x) noexcept
{
    return vec<4, T>(asinh(x.x), asinh(x.y), asinh(x.z), asinh(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// acosh
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T acosh(T x) noexcept
{
    return std::acosh(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> acosh(const vec<2, T>& x) noexcept
{
    return vec<2, T>(acosh(x.x), acosh(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> acosh(const vec<3, T>& x) noexcept
{
    return vec<3, T>(acosh(x.x), acosh(x.y), acosh(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> acosh(const vec<4, T>& x) noexcept
{
    return vec<4, T>(acosh(x.x), acosh(x.y), acosh(x.z), acosh(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// atanh
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T atanh(T x) noexcept
{
    return std::atanh(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> atanh(const vec<2, T>& x) noexcept
{
    return vec<2, T>(atanh(x.x), atanh(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> atanh(const vec<3, T>& x) noexcept
{
    return vec<3, T>(atanh(x.x), atanh(x.y), atanh(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> atanh(const vec<4, T>& x) noexcept
{
    return vec<4, T>(atanh(x.x), atanh(x.y), atanh(x.z), atanh(x.w));
}

} // namespace math
} // namespace vx