#pragma once

#include <cmath>

#include "vertex/config/language_config.hpp"
#include "vertex/math/type_traits.hpp"
#include "vertex/math/constants.hpp"

namespace vx {
namespace math {

//////////////////////////////////////////////////////////////////////////////
// common
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////
// min
///////////////////////////////////////

#if defined(min)
#   undef min
#endif

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

///////////////////////////////////////
// max
///////////////////////////////////////

#if defined(max)
#   undef max
#endif

template <typename T>
VX_FORCE_INLINE constexpr T max(T x, T y) noexcept
{
    return std::max(x, y);
}

template <typename T, typename... Ts>
VX_FORCE_INLINE constexpr T max(T x, T y, Ts... rest) noexcept
{
    return max((y < x) ? y : x, rest...);
}

///////////////////////////////////////
// abs
///////////////////////////////////////

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T abs(T x) noexcept
{
    return std::abs(x);
}

///////////////////////////////////////
// round
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T round(T x) noexcept
{
    return std::round(x);
}

///////////////////////////////////////
// trunc
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T trunc(T x) noexcept
{
    return std::trunc(x);
}

///////////////////////////////////////
// floor
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T floor(T x) noexcept
{
    return std::floor(x);
}

///////////////////////////////////////
// ceil
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T ceil(T x) noexcept
{
    return std::ceil(x);
}

///////////////////////////////////////
// sign
///////////////////////////////////////

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T sign(T x) noexcept
{
    return static_cast<T>((static_cast<T>(0) < x) - (static_cast<T>(0) > x));
}

///////////////////////////////////////
// fmod
///////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T fmod(T x, T y) noexcept
{
    return x - y * std::trunc(x / y);
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