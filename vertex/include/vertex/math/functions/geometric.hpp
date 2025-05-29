#pragma once

#include "vertex/math/functions/common.hpp"
#include "vertex/math/functions/comparison.hpp"
#include "vertex/math/functions/exponential.hpp"
#include "vertex/math/functions/trigonometric.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// dot
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T dot(
    const vec<2, T>& v,
    const vec<2, T>& u
) noexcept
{
    return (v.x * u.x) + (v.y * u.y);
}

template <typename T>
VX_FORCE_INLINE constexpr T dot(
    const vec<3, T>& v,
    const vec<3, T>& u
) noexcept
{
    return (v.x * u.x) + (v.y * u.y) + (v.z * u.z);
}

template <typename T>
VX_FORCE_INLINE constexpr T dot(
    const vec<4, T>& v,
    const vec<4, T>& u
) noexcept
{
    return (v.x * u.x) + (v.y * u.y) + (v.z * u.z) + (v.w * u.w);
}

///////////////////////////////////////////////////////////////////////////////
// orthogonal
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ(is_signed<T>::value)>
VX_FORCE_INLINE constexpr vec<2, T> orthogonal(const vec<2, T>& v) noexcept
{
    return vec<2, T>(-v.y, v.x);
}

///////////////////////////////////////////////////////////////////////////////
// cross
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T cross(
    const vec<2, T>& v,
    const vec<2, T>& u
) noexcept
{
    return (v.x * u.y) - (v.y * u.x);
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> cross(
    const vec<3, T>& v,
    const vec<3, T>& u
) noexcept
{
    return vec<3, T>(
        (v.y * u.z) - (v.z * u.y),
        (v.z * u.x) - (v.x * u.z),
        (v.x * u.y) - (v.y * u.x)
    );
}

///////////////////////////////////////////////////////////////////////////////
// length_squared
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T length_squared(const vec<L, T>& v) noexcept
{
    return dot(v, v);
}

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T magnitude_squared(const vec<L, T>& v) noexcept
{
    return length_squared(v);
}

///////////////////////////////////////////////////////////////////////////////
// length
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T length(const vec<L, T>& v) noexcept
{
    return sqrt(length_squared(v));
}

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T magnitude(const vec<L, T>& v) noexcept
{
    return length(v);
}

///////////////////////////////////////////////////////////////////////////////
// distance_squared
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T distance_squared(
    const vec<L, T>& p0,
    const vec<L, T>& p1
) noexcept
{
    return length_squared(p1 - p0);
}

///////////////////////////////////////////////////////////////////////////////
// distance
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T distance(
    const vec<L, T>& p0,
    const vec<L, T>& p1
) noexcept
{
    return length(p1 - p0);
}

///////////////////////////////////////////////////////////////////////////////
// normalize
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> normalize(const vec<L, T>& v) noexcept
{
    const T magsq = length_squared(v);
    return (magsq <= constants<T>::epsilon) ? vec<L, T>(0) : (v * inverse_sqrt(magsq));
}

///////////////////////////////////////////////////////////////////////////////
// normalize_fast
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> normalize_fast(const vec<L, T>& v) noexcept
{
    return v * inverse_sqrt(length_squared(v));
}

///////////////////////////////////////////////////////////////////////////////
// is_normalized
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_normalized(
    const vec<L, T>& v,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(length_squared(v), static_cast<T>(1), epsilon);
}

///////////////////////////////////////////////////////////////////////////////
// normalized_dot
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto normalized_dot(
    const vec<L, T>& v,
    const vec<L, T>& u
) noexcept
{
    return dot(normalize(v), normalize(u));
}

///////////////////////////////////////////////////////////////////////////////
// set_length
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> set_length(const vec<L, T>& v, T len) noexcept
{
    const T mag = length(v);
    if (mag <= constants<T>::epsilon)
    {
        return vec<L, T>(0);
    }

    const T scale = len / mag;
    return v * scale;
}

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> set_magnitude(const vec<L, T>& v, T len) noexcept
{
    return set_length(v, len);
}

///////////////////////////////////////////////////////////////////////////////
// clamp_length
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> clamp_length(
    const vec<L, T>& v,
    T min,
    T max
) noexcept
{
    const T mag = length(v);
    if (mag <= constants<T>::epsilon)
    {
        return vec<L, T>(0);
    }

    const T scale = clamp(mag, min, max) / mag;
    return v * scale;
}

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> clamp_magnitude(
    const vec<L, T>& v,
    T min,
    T max
) noexcept
{
    return clamp_length(v, min, max);
}

///////////////////////////////////////////////////////////////////////////////
// aspect
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T aspect(T x, T y) noexcept
{
    return static_cast<T>(is_zero_approx(y)) ? static_cast<T>(0) : (x / y);
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T aspect(const vec<2, T>& v) noexcept
{
    return aspect(v.x, v.y);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr auto aspect(const vec<2, T>& v) noexcept
{
    using F = VXM_FLOAT_TYPE(T);
    return aspect(static_cast<F>(v.x), static_cast<F>(v.y));
}

///////////////////////////////////////////////////////////////////////////////
// area
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T area(const vec<2, T>& v) noexcept
{
    return v.x * v.y;
}

///////////////////////////////////////////////////////////////////////////////
// volume
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T volume(const vec<3, T>& v) noexcept
{
    return v.x * v.y * v.z;
}

///////////////////////////////////////////////////////////////////////////////
// angle
///////////////////////////////////////////////////////////////////////////////

// vec2

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto angle(const vec<2, T>& v) noexcept
{
    return atan2(v.y, v.x);
}

// vec3

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T angle(
    const vec<L, T>& from,
    const vec<L, T>& to
) noexcept
{
    return acos(normalized_dot(from, to));
}

///////////////////////////////////////////////////////////////////////////////
// signed_angle
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T signed_angle(
    const vec<2, T>& from,
    const vec<2, T>& to
) noexcept
{
    const T a = angle(from, to);
    return (cross(from, to) < static_cast<T>(0)) ? -a : a;
}

template <typename T>
VX_FORCE_INLINE constexpr T signed_angle(
    const vec<3, T>& from,
    const vec<3, T>& to,
    const vec<3, T>& nref = vec<3, T>::up()
) noexcept {
    const T a = angle(from, to);
    const vec<3, T> c = cross(from, to);
    const T sign = normalized_dot(c, nref);
    return (sign < static_cast<T>(0)) ? -a : a;
}

///////////////////////////////////////////////////////////////////////////////
// direction
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> direction(T angle) noexcept
{
    return vec<2, T>(
        cos(angle),
        sin(angle)
    );
}

///////////////////////////////////////////////////////////////////////////////
// rotate
///////////////////////////////////////////////////////////////////////////////

// axis (2d)

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> rotate(const vec<2, T>& v, T angle) noexcept
{
    const T cosa = cos(angle);
    const T sina = sin(angle);

    return vec<2, T>(
        (v.x * cosa) - (v.y * sina),
        (v.x * sina) + (v.y * cosa)
    );
}

// axis angle (3d)

// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> rotate(
    const vec<3, T>& v,
    const vec<3, T>& axis,
    T angle
) noexcept
{
    const vec<3, T> naxis = normalize(axis);

    const T cosa = cos(angle);
    const T sina = sin(angle);

    return v * cosa + cross(naxis, v) * sina + naxis * dot(naxis, v) * (static_cast<T>(1) - cosa);
}

///////////////////////////////////////////////////////////////////////////////
// project
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> project(
    const vec<L, T>& i,
    const vec<L, T>& n
) noexcept
{
    return n * (dot(i, n) / length_squared(n));
}

///////////////////////////////////////////////////////////////////////////////
// reflect
///////////////////////////////////////////////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> reflect(
    const vec<L, T>& i,
    const vec<L, T>& n
) noexcept
{
    return i - static_cast<T>(2) * normalized_dot(n, i) * n;
}

///////////////////////////////////////////////////////////////////////////////
// bounce
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> bounce(
    const vec<L, T>& i,
    const vec<L, T>& n
) noexcept
{
    return -reflect(i, n);
}

///////////////////////////////////////////////////////////////////////////////
// refract
///////////////////////////////////////////////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/refract.xhtml

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> refract(
    const vec<L, T>& i,
    const vec<L, T>& n,
    T eta
) noexcept
{
    const T d = normalized_dot(n, i);
    const T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - d * d);
    return (k <= constants<T>::epsilon)
        ? vec<L, T>(0)
        : (eta * i - (eta * d + sqrt(k)) * n);
}

///////////////////////////////////////////////////////////////////////////////
// face_forward
///////////////////////////////////////////////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/faceforward.xhtml

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> face_forward(
    const vec<L, T>& n,
    const vec<L, T>& i,
    const vec<L, T>& nref
) noexcept
{
    return (normalized_dot(nref, i) < static_cast<T>(0)) ? n : -n;
}

} // namespace math
} // namespace vx