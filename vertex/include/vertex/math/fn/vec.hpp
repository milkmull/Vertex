#pragma once

#include "vertex/math/fn/scalar.hpp"

namespace vx {
namespace math {

//////////////////////////////////////////////////////////////////////////////

#define __VEC2_UNARY(v, T, fn) vec<2, T>(fn(v.x), fn(v.y))
#define __VEC3_UNARY(v, T, fn) vec<3, T>(fn(v.x), fn(v.y), fn(v.z))
#define __VEC4_UNARY(v, T, fn) vec<4, T>(fn(v.x), fn(v.y), fn(v.z), fn(v.w))

#define __VEC2_SCALAR_BINARY(v, s, T, fn) vec<2, T>(fn(v.x, s), fn(v.y, s))
#define __VEC3_SCALAR_BINARY(v, s, T, fn) vec<3, T>(fn(v.x, s), fn(v.y, s), fn(v.z, s))
#define __VEC4_SCALAR_BINARY(v, s, T, fn) vec<4, T>(fn(v.x, s), fn(v.y, s), fn(v.z, s), fn(v.w, s))

#define __VEC2_BINARY(v1, v2, T, fn) vec<2, T>(fn(v1.x, v2.x), fn(v1.y, v2.y))
#define __VEC3_BINARY(v1, v2, T, fn) vec<3, T>(fn(v1.x, v2.x), fn(v1.y, v2.y), fn(v1.z, v2.z))
#define __VEC4_BINARY(v1, v2, T, fn) vec<4, T>(fn(v1.x, v2.x), fn(v1.y, v2.y), fn(v1.z, v2.z), fn(v1.w, v2.w))

//////////////////////////////////////////////////////////////////////////////
// common
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////
// min
///////////////////////////////////////

// scalar binary

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

// binary

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

///////////////////////////////////////
// max
///////////////////////////////////////

// scalar binary

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

// binary

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

///////////////////////////////////////
// cmin
///////////////////////////////////////

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

///////////////////////////////////////
// cmax
///////////////////////////////////////

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

///////////////////////////////////////
// csum
///////////////////////////////////////

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

///////////////////////////////////////
// abs
///////////////////////////////////////

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

///////////////////////////////////////
// trunc
///////////////////////////////////////

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

///////////////////////////////////////
// trunc
///////////////////////////////////////

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

///////////////////////////////////////
// floor
///////////////////////////////////////

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

///////////////////////////////////////
// ceil
///////////////////////////////////////

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

///////////////////////////////////////
// sign
///////////////////////////////////////

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

///////////////////////////////////////
// fmod
///////////////////////////////////////

// scalar binary

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

// binary

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

///////////////////////////////////////
// mod
///////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/mod.xhtml

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

///////////////////////////////////////
// fract
///////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/fract.xhtml

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T fract(T x) noexcept
{
    return x - std::floor(x);
}

///////////////////////////////////////
// modf
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto modf(T x, T& intpart) noexcept
{
    return std::modf(x, &intpart);
}

///////////////////////////////////////
// fma
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto fma(T x, T y, T z) noexcept
{
    return std::fma(x, y, z);
}

///////////////////////////////////////
// frexp
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto frexp(T x, int& exp) noexcept
{
    const T f = std::frexp(x, &exp);
    return f;
}

///////////////////////////////////////
// ldexp
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T ldexp(T x, int exp) noexcept
{
    return std::ldexp(x, exp);
}

///////////////////////////////////////
// clamp
///////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T clamp(T x, T min, T max) noexcept
{
    const T t = x < min ? min : x;
    return t > max ? max : t;
}

///////////////////////////////////////
// gdc
///////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////
// comparison
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////
// basic
///////////////////////////////////////

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool equal(T x, T y) noexcept
{
    return x == y;
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool not_equal(T x, T y) noexcept
{
    return x != y;
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool greater_than(T x, T y) noexcept
{
    return x > y;
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool greater_than_equal(T x, T y) noexcept
{
    return x >= y;
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool less_than(T x, T y) noexcept
{
    return x < y;
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool less_than_equal(T x, T y) noexcept
{
    return x <= y;
}
///////////////////////////////////////
// is_inf
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_inf(T x) noexcept
{
    return std::isinf(x);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_inf(T x) noexcept
{
    return false;
}

///////////////////////////////////////
// is_nan
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_nan(T x) noexcept
{
    return std::isnan(x);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_nan(T x) noexcept
{
    return false;
}

///////////////////////////////////////
// is_zero_approx
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_zero_approx(T x, const T epsilon = constants<T>::epsilon) noexcept
{
    return ((x < static_cast<T>(0)) ? -x : x) < epsilon;
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_zero_approx(T x, const T epsilon = constants<T>::epsilon) noexcept
{
    return x == static_cast<T>(0);
}

///////////////////////////////////////
// is_equal_approx
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return is_zero_approx(a - b, epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a == b;
}

///////////////////////////////////////
// is_not_equal_approx
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_not_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return !is_zero_approx(a - b, epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_not_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a != b;
}

///////////////////////////////////////
// is_greater_than_approx
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_greater_than_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a > (b + epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_greater_than_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a > b;
}

///////////////////////////////////////
// is_greater_than_equal_approx
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_greater_than_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a > (b - epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_greater_than_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a >= b;
}

///////////////////////////////////////
// is_less_than_approx
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_less_than_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a < (b - epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_less_than_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a < b;
}

///////////////////////////////////////
// is_less_than_equal_approx
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_less_than_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a < (b + epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_less_than_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a <= b;
}

//////////////////////////////////////////////////////////////////////////////
// exponential
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////
// pow
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T pow(T x, T y) noexcept
{
    return std::pow(x, y);
}

///////////////////////////////////////
// exp
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T exp(T x) noexcept
{
    return std::exp(x);
}

///////////////////////////////////////
// exp2
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T exp2(T x) noexcept
{
    return std::exp2(x);
}

///////////////////////////////////////
// log
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T log(T x) noexcept
{
    return std::log(x);
}

///////////////////////////////////////
// log2
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T log2(T x) noexcept
{
    return std::log2(x);
}

///////////////////////////////////////
// sqrt
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T sqrt(T x) noexcept
{
    return std::sqrt(x);
}

///////////////////////////////////////
// inverse
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T inverse(T x) noexcept
{
    return static_cast<T>(1) / x;
}

///////////////////////////////////////
// inverse_sqrt
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T inverse_sqrt(T x) noexcept
{
    return static_cast<T>(1) / std::sqrt(x);
}

///////////////////////////////////////
// next_pow2
///////////////////////////////////////

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr T next_pow2(T x) noexcept
{
    --x;

    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    if (sizeof(T) == 8)
    {
        x |= x >> 32;
    }

    return ++x;
}

//////////////////////////////////////////////////////////////////////////////
// interpolation
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////
// lerp
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T lerp(T x, T y, T t) noexcept
{
    return x * (static_cast<T>(1) - t) + y * t;
}

///////////////////////////////////////
// mix
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T mix(T x, T y, T t) noexcept
{
    return lerp(x, y, t);
}

///////////////////////////////////////
// step
///////////////////////////////////////

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T step(T edge, T x) noexcept
{
    return static_cast<T>(x >= edge);
}

///////////////////////////////////////
// smoothstep
///////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/smoothstep.xhtml

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T smoothstep(T edge0, T edge1, T x) noexcept
{
    const T v = (x - edge0) / (edge1 - edge0);
    T t = v < static_cast<T>(0) ? static_cast<T>(0) : v;
    t = v > static_cast<T>(1) ? static_cast<T>(1) : v;
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

//////////////////////////////////////////////////////////////////////////////
// trigonometric
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////
// radians
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T radians(T deg) noexcept
{
    constexpr T r = constants<T>::two_pi / static_cast<T>(360);
    return deg * r;
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr auto radians(T deg) noexcept
{
    return radians(static_cast<d>(deg));
}

///////////////////////////////////////
// degrees
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T degrees(T rad) noexcept
{
    constexpr T r = static_cast<T>(360) / constants<T>::two_pi;
    return rad * r;
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr auto degrees(T rad) noexcept
{
    return degrees(static_cast<d>(rad));
}

///////////////////////////////////////
// sin
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T sin(T x) noexcept
{
    return std::sin(x);
}

///////////////////////////////////////
// cos
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T cos(T x) noexcept
{
    return std::cos(x);
}

///////////////////////////////////////
// tan
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T tan(T x) noexcept
{
    return std::tan(x);
}

///////////////////////////////////////
// asin
///////////////////////////////////////

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

///////////////////////////////////////
// acos
///////////////////////////////////////

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

///////////////////////////////////////
// atan
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T atan(T x) noexcept
{
    return std::atan(x);
}

///////////////////////////////////////
// atan2
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T atan2(T y, T x) noexcept
{
    return std::atan2(y, x);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr auto atan2(T y, T x) noexcept
{
    return std::atan2(static_cast<double>(y), static_cast<double>(x));
}

///////////////////////////////////////
// sinh
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T sinh(T x) noexcept
{
    return std::sinh(x);
}

///////////////////////////////////////
// cosh
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T cosh(T x) noexcept
{
    return std::cosh(x);
}

///////////////////////////////////////
// tanh
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T tanh(T x) noexcept
{
    return std::tanh(x);
}

///////////////////////////////////////
// asinh
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T asinh(T x) noexcept
{
    return std::asinh(x);
}

///////////////////////////////////////
// acosh
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T acosh(T x) noexcept
{
    return std::acosh(x);
}

///////////////////////////////////////
// atanh
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T atanh(T x) noexcept
{
    return std::atanh(x);
}

} // namespace math
} // namespace vx