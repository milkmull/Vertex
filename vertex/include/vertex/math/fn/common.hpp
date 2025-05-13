#pragma once

#include <cmath>

#include "vertex/config/language_config.hpp"
#include "vertex/math/constants.hpp"
#include "vertex/math/types.hpp"

namespace vx {
namespace math {

//////////////////////////////////////////////////////////////////////////////
// min
//////////////////////////////////////////////////////////////////////////////

#if defined(min)
#   undef min
#endif

// scalar

template <typename T>
VX_FORCE_INLINE constexpr T min(T x, T y) noexcept
{
    return std::min(x, y);
}

template <typename T, typename... Ts>
VX_FORCE_INLINE constexpr T min(T x, T y, Ts... rest) noexcept
{
    return min((y < x) ? y : x, rest...);
}

// vec scalar

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> min(const vec<2, T>& x, T y) noexcept
{
    return vec<2, T>(min(x.x, y), min(x.y, y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> min(const vec<3, T>& x, T y) noexcept
{
    return vec<3, T>(min(x.x, y), min(x.y, y), min(x.z, y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> min(const vec<4, T>& x, T y) noexcept
{
    return vec<4, T>(min(x.x, y), min(x.y, y), min(x.z, y), min(x.w, y));
}

// vec binary

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> min(
    const vec<2, T>& x,
    const vec<2, T>& y
) noexcept
{
    return vec<2, T>(min(x.x, y.x), min(x.y, y.y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> min(
    const vec<3, T>& x,
    const vec<3, T>& y
) noexcept
{
    return vec<3, T>(min(x.x, y.x), min(x.y, y.y), min(x.z, y.z));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> min(
    const vec<4, T>& x,
    const vec<4, T>& y
) noexcept
{
    return vec<4, T>(min(x.x, y.x), min(x.y, y.y), min(x.z, y.z), min(x.w, y.w));
}

//////////////////////////////////////////////////////////////////////////////
// max
//////////////////////////////////////////////////////////////////////////////

#if defined(max)
#   undef max
#endif

// scalar

template <typename T>
VX_FORCE_INLINE constexpr T max(T x, T y) noexcept
{
    return std::max(x, y);
}

template <typename T, typename... Ts>
VX_FORCE_INLINE constexpr T max(T x, T y, Ts... rest) noexcept
{
    return max((x < y) ? y : x, rest...);
}

// vec scalar

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> max(const vec<2, T>& x, T y) noexcept
{
    return vec<2, T>(max(x.x, y), max(x.y, y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> max(const vec<3, T>& x, T y) noexcept
{
    return vec<3, T>(max(x.x, y), max(x.y, y), max(x.z, y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> max(const vec<4, T>& x, T y) noexcept
{
    return vec<4, T>(max(x.x, y), max(x.y, y), max(x.z, y), max(x.w, y));
}

// vec binary

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> max(
    const vec<2, T>& x,
    const vec<2, T>& y
) noexcept
{
    return vec<2, T>(max(x.x, y.x), max(x.y, y.y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> max(
    const vec<3, T>& x,
    const vec<3, T>& y
) noexcept
{
    return vec<3, T>(max(x.x, y.x), max(x.y, y.y), max(x.z, y.z));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> max(
    const vec<4, T>& x,
    const vec<4, T>& y
) noexcept
{
    return vec<4, T>(max(x.x, y.x), max(x.y, y.y), max(x.z, y.z), max(x.w, y.w));
}

//////////////////////////////////////////////////////////////////////////////
// cmin
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T cmin(const vec<2, T>& x) noexcept
{
    return min(x.x, x.y);
}

template <typename T>
VX_FORCE_INLINE constexpr T cmin(const vec<3, T>& x) noexcept
{
    return min(x.x, x.y, x.z);
}

template <typename T>
VX_FORCE_INLINE constexpr T cmin(const vec<4, T>& x) noexcept
{
    return min(x.x, x.y, x.z, x.w);
}

//////////////////////////////////////////////////////////////////////////////
// cmax
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T cmax(const vec<2, T>& x) noexcept
{
    return max(x.x, x.y);
}

template <typename T>
VX_FORCE_INLINE constexpr T cmax(const vec<3, T>& x) noexcept
{
    return max(x.x, x.y, x.z);
}

template <typename T>
VX_FORCE_INLINE constexpr T cmax(const vec<4, T>& x) noexcept
{
    return max(x.x, x.y, x.z, x.w);
}

//////////////////////////////////////////////////////////////////////////////
// csum
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T csum(const vec<2, T>& x) noexcept
{
    return x.x + x.y;
}

template <typename T>
VX_FORCE_INLINE constexpr T csum(const vec<3, T>& x) noexcept
{
    return x.x + x.y + x.z;
}

template <typename T>
VX_FORCE_INLINE constexpr T csum(const vec<4, T>& x) noexcept
{
    return x.x + x.y + x.z + x.w;
}

//////////////////////////////////////////////////////////////////////////////
// abs
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T abs(T x) noexcept
{
    return std::abs(x);
}

// vec

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr vec<2, T> abs(const vec<2, T>& x) noexcept
{
    return vec<2, T>(abs(x.x), abs(x.y));
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr vec<3, T> abs(const vec<3, T>& x) noexcept
{
    return vec<3, T>(abs(x.x), abs(x.y), abs(x.z));
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr vec<4, T> abs(const vec<4, T>& x) noexcept
{
    return vec<4, T>(abs(x.x), abs(x.y), abs(x.z), abs(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// round
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T round(T x) noexcept
{
    return std::round(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> round(const vec<2, T>& x) noexcept
{
    return vec<2, T>(round(x.x), round(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> round(const vec<3, T>& x) noexcept
{
    return vec<3, T>(round(x.x), round(x.y), round(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> round(const vec<4, T>& x) noexcept
{
    return vec<4, T>(round(x.x), round(x.y), round(x.z), round(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// trunc
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T trunc(T x) noexcept
{
    return std::trunc(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> trunc(const vec<2, T>& x) noexcept
{
    return vec<2, T>(trunc(x.x), trunc(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> trunc(const vec<3, T>& x) noexcept
{
    return vec<3, T>(trunc(x.x), trunc(x.y), trunc(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> trunc(const vec<4, T>& x) noexcept
{
    return vec<4, T>(trunc(x.x), trunc(x.y), trunc(x.z), trunc(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// floor
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T floor(T x) noexcept
{
    return std::floor(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> floor(const vec<2, T>& x) noexcept
{
    return vec<2, T>(floor(x.x), floor(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> floor(const vec<3, T>& x) noexcept
{
    return vec<3, T>(floor(x.x), floor(x.y), floor(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> floor(const vec<4, T>& x) noexcept
{
    return vec<4, T>(floor(x.x), floor(x.y), floor(x.z), floor(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// ceil
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T ceil(T x) noexcept
{
    return std::ceil(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> ceil(const vec<2, T>& x) noexcept
{
    return vec<2, T>(ceil(x.x), ceil(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> ceil(const vec<3, T>& x) noexcept
{
    return vec<3, T>(ceil(x.x), ceil(x.y), ceil(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> ceil(const vec<4, T>& x) noexcept
{
    return vec<4, T>(ceil(x.x), ceil(x.y), ceil(x.z), ceil(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// sign
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T sign(T x) noexcept
{
    return static_cast<T>((static_cast<T>(0) < x) - (static_cast<T>(0) > x));
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> sign(const vec<2, T>& x) noexcept
{
    return vec<2, T>(sign(x.x), sign(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> sign(const vec<3, T>& x) noexcept
{
    return vec<3, T>(sign(x.x), sign(x.y), sign(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> sign(const vec<4, T>& x) noexcept
{
    return vec<4, T>(sign(x.x), sign(x.y), sign(x.z), sign(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// fmod
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T fmod(T x, T y) noexcept
{
    return x - y * std::trunc(x / y);
}

// vec scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> fmod(const vec<2, T>& x, T y) noexcept
{
    return vec<2, T>(fmod(x.x, y), fmod(x.y, y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> fmod(const vec<3, T>& x, T y) noexcept
{
    return vec<3, T>(fmod(x.x, y), fmod(x.y, y), fmod(x.z, y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> fmod(const vec<4, T>& x, T y) noexcept
{
    return vec<4, T>(fmod(x.x, y), fmod(x.y, y), fmod(x.z, y), fmod(x.w, y));
}

// vec binary

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> fmod(
    const vec<2, T>& x,
    const vec<2, T>& y
) noexcept
{
    return vec<2, T>(fmod(x.x, y.x), fmod(x.y, y.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> fmod(
    const vec<3, T>& x,
    const vec<3, T>& y
) noexcept
{
    return vec<3, T>(fmod(x.x, y.x), fmod(x.y, y.y), fmod(x.z, y.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> fmod(
    const vec<4, T>& x,
    const vec<4, T>& y
) noexcept
{
    return vec<4, T>(fmod(x.x, y.x), fmod(x.y, y.y), fmod(x.z, y.z), fmod(x.w, y.w));
}

//////////////////////////////////////////////////////////////////////////////
// mod
//////////////////////////////////////////////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/mod.xhtml

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T mod(T x, T y) noexcept
{
    return x - y * std::floor(x / y);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr T mod(T x, T y) noexcept
{
    return (x % y + y) % y;
}

// vec scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto mod(const vec<2, T>& x, T y) noexcept
{
    using R = decltype(mod(std::declval<T>(), std::declval<T>()));
    return vec<2, R>(mod(x.x, y), mod(x.y, y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto mod(const vec<3, T>& x, T y) noexcept
{
    using R = decltype(mod(std::declval<T>(), std::declval<T>()));
    return vec<3, R>(mod(x.x, y), mod(x.y, y), mod(x.z, y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto mod(const vec<4, T>& x, T y) noexcept
{
    using R = decltype(mod(std::declval<T>(), std::declval<T>()));
    return vec<4, R>(mod(x.x, y), mod(x.y, y), mod(x.z, y), mod(x.w, y));
}

// vec binary

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto mod(
    const vec<2, T>& x,
    const vec<2, T>& y
) noexcept
{
    using R = decltype(mod(std::declval<T>(), std::declval<T>()));
    return vec<2, R>(mod(x.x, y.x), mod(x.y, y.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto mod(
    const vec<3, T>& x,
    const vec<3, T>& y
) noexcept
{
    using R = decltype(mod(std::declval<T>(), std::declval<T>()));
    return vec<3, R>(mod(x.x, y.x), mod(x.y, y.y), mod(x.z, y.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto mod(
    const vec<4, T>& x,
    const vec<4, T>& y
) noexcept
{
    using R = decltype(mod(std::declval<T>(), std::declval<T>()));
    return vec<4, R>(mod(x.x, y.x), mod(x.y, y.y), mod(x.z, y.z), mod(x.w, y.w));
}

//////////////////////////////////////////////////////////////////////////////
// fract
//////////////////////////////////////////////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/fract.xhtml

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T fract(T x) noexcept
{
    return x - std::floor(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> fract(const vec<2, T>& x) noexcept
{
    return vec<2, T>(fract(x.x), fract(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> fract(const vec<3, T>& x) noexcept
{
    return vec<3, T>(fract(x.x), fract(x.y), fract(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> fract(const vec<4, T>& x) noexcept
{
    return vec<4, T>(fract(x.x), fract(x.y), fract(x.z), fract(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// modf
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto modf(T x, T& intpart) noexcept
{
    return std::modf(x, &intpart);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> modf(
    const vec<2, T>& x,
    vec<2, T>& intpart
) noexcept
{
    return vec<2, T>(modf(x.x, intpart.x), modf(x.y, intpart.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> modf(
    const vec<3, T>& x,
    vec<3, T>& intpart
) noexcept
{
    return vec<3, T>(modf(x.x, intpart.x), modf(x.y, intpart.y), modf(x.z, intpart.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> modf(
    const vec<4, T>& x,
    vec<4, T>& intpart
) noexcept
{
    return vec<4, T>(modf(x.x, intpart.x), modf(x.y, intpart.y), modf(x.z, intpart.z), modf(x.w, intpart.w));
}

//////////////////////////////////////////////////////////////////////////////
// fma
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T fma(T x, T y, T z) noexcept
{
    return std::fma(x, y, z);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> fma(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const vec<2, T>& z
) noexcept
{
    return vec<2, T>(
        fma(x.x, y.x, z.x),
        fma(x.y, y.y, z.y)
    );
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> fma(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const vec<3, T>& z
) noexcept
{
    return vec<3, T>(
        fma(x.x, y.x, z.x),
        fma(x.y, y.y, z.y),
        fma(x.z, y.z, z.z)
    );
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> fma(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const vec<4, T>& z
) noexcept
{
    return vec<4, T>(
        fma(x.x, y.x, z.x),
        fma(x.y, y.y, z.y),
        fma(x.z, y.z, z.z),
        fma(x.w, y.w, z.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// frexp
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T frexp(T x, i32& exp) noexcept
{
    return std::frexp(x, &exp);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> frexp(
    const vec<2, T>& x,
    vec<2, i32>& exp
) noexcept
{
    return vec<2, T>(frexp(x.x, exp.x), frexp(x.y, exp.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> frexp(
    const vec<3, T>& x,
    vec<3, i32>& exp
) noexcept
{
    return vec<3, T>(frexp(x.x, exp.x), frexp(x.y, exp.y), frexp(x.z, exp.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> frexp(
    const vec<4, T>& x,
    vec<4, i32>& exp
) noexcept
{
    return vec<4, T>(frexp(x.x, exp.x), frexp(x.y, exp.y), frexp(x.z, exp.z), frexp(x.w, exp.w));
}

//////////////////////////////////////////////////////////////////////////////
// ldexp
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T ldexp(T x, i32 exp) noexcept
{
    return std::ldexp(x, exp);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> ldexp(
    const vec<2, T>& x,
    const vec<2, i32>& exp
) noexcept
{
    return vec<2, T>(ldexp(x.x, exp.x), ldexp(x.y, exp.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> ldexp(
    const vec<3, T>& x,
    const vec<3, i32>& exp
) noexcept
{
    return vec<3, T>(ldexp(x.x, exp.x), ldexp(x.y, exp.y), ldexp(x.z, exp.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> ldexp(
    const vec<4, T>& x,
    const vec<4, i32>& exp
) noexcept
{
    return vec<4, T>(ldexp(x.x, exp.x), ldexp(x.y, exp.y), ldexp(x.z, exp.z), ldexp(x.w, exp.w));
}

//////////////////////////////////////////////////////////////////////////////
// clamp
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T>
VX_FORCE_INLINE constexpr T clamp(T x, T min, T max) noexcept
{
    const T t = x < min ? min : x;
    return t > max ? max : t;
}

// vec scalar

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> clamp(
    const vec<2, T>& x,
    T min,
    T max
) noexcept
{
    return vec<2, T>(
        clamp(x.x, min, max),
        clamp(x.y, min, max)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> clamp(
    const vec<3, T>& x,
    T min,
    T max
) noexcept
{
    return vec<3, T>(
        clamp(x.x, min, max),
        clamp(x.y, min, max),
        clamp(x.z, min, max)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> clamp(
    const vec<4, T>& x,
    T min,
    T max
) noexcept
{
    return vec<4, T>(
        clamp(x.x, min, max),
        clamp(x.y, min, max),
        clamp(x.z, min, max),
        clamp(x.w, min, max)
    );
}

// vec trinary

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> clamp(
    const vec<2, T>& x,
    const vec<2, T>& min,
    const vec<2, T>& max
) noexcept
{
    return vec<2, T>(
        clamp(x.x, min.x, max.x),
        clamp(x.y, min.y, max.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> clamp(
    const vec<3, T>& x,
    const vec<3, T>& min,
    const vec<3, T>& max
) noexcept
{
    return vec<3, T>(
        clamp(x.x, min.x, max.x),
        clamp(x.y, min.y, max.y),
        clamp(x.z, min.z, max.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> clamp(
    const vec<4, T>& x,
    const vec<4, T>& min,
    const vec<4, T>& max
) noexcept
{
    return vec<4, T>(
        clamp(x.x, min.x, max.x),
        clamp(x.y, min.y, max.y),
        clamp(x.z, min.z, max.z),
        clamp(x.w, min.w, max.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// gdc
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr T gcd(T a, T b) noexcept
{
    while (b != static_cast<T>(0))
    {
        T temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}

// vec scalar

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr vec<2, T> gcd(const vec<2, T>& a, T b) noexcept
{
    return vec<2, T>(gcd(a.x, b), gcd(a.y, b));
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr vec<3, T> gcd(const vec<3, T>& a, T b) noexcept
{
    return vec<3, T>(gcd(a.x, b), gcd(a.y, b), gcd(a.z, b));
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr vec<4, T> gcd(const vec<4, T>& a, T b) noexcept
{
    return vec<4, T>(gcd(a.x, b), gcd(a.y, b), gcd(a.z, b), gcd(a.w, b));
}

// vec binary

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr vec<2, T> gcd(
    const vec<2, T>& a,
    const vec<2, T>& b
) noexcept
{
    return vec<2, T>(gcd(a.x, b.x), gcd(a.y, b.y));
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr vec<3, T> gcd(
    const vec<3, T>& a,
    const vec<3, T>& b
) noexcept
{
    return vec<3, T>(gcd(a.x, b.x), gcd(a.y, b.y), gcd(a.z, b.z));
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr vec<4, T> gcd(
    const vec<4, T>& a,
    const vec<4, T>& b
) noexcept
{
    return vec<4, T>(gcd(a.x, b.x), gcd(a.y, b.y), gcd(a.z, b.z), gcd(a.w, b.w));
}

} // namespace math
} // namespace vx