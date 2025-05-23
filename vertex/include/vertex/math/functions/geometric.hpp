#pragma once

#include "vertex/math/functions/common.hpp"
#include "vertex/math/functions/exponential.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// dot
///////////////////////////////////////////////////////////////////////////////

// vec

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

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr T dot(
    const quat_t<T>& q1,
    const quat_t<T>& q2
) noexcept
{
    return (q1.w * q2.w) + (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z);
}

///////////////////////////////////////////////////////////////////////////////
// normalized_dot
///////////////////////////////////////////////////////////////////////////////

// vec

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto normalized_dot(
    const vec<L, T>& v,
    const vec<L, T>& u
) noexcept
{
    return dot(normalize(v), normalize(u));
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr T normalized_dot(
    const quat_t<T>& q1,
    const quat_t<T>& q2
) noexcept
{
    return dot(normalize(q1), normalize(q2));
}

///////////////////////////////////////////////////////////////////////////////
// orthogonal
///////////////////////////////////////////////////////////////////////////////

template <typename T>
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

// vec

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

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr T length_squared(const quat_t<T>& q) noexcept
{
    return dot(q, q);
}

template <typename T>
VX_FORCE_INLINE constexpr T magnitude_squared(const quat_t<T>& q) noexcept
{
    return length_squared(q);
}

///////////////////////////////////////////////////////////////////////////////
// length
///////////////////////////////////////////////////////////////////////////////

// vec

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

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr T length(const quat_t<T>& q) noexcept
{
    return math::sqrt(length_squared(q));
}

template <typename T>
VX_FORCE_INLINE constexpr T magnitude(const quat_t<T>& q) noexcept
{
    return length(q);
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

// vec

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> normalize(const vec<L, T>& v) noexcept
{
    const T magsq = length_squared(v);
    return (magsq <= constants<T>::epsilon) ? vec<L, T>(0) : (v * inverse_sqrt(magsq));
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> normalize(const quat_t<T>& q) noexcept
{
    const T magsq = length_squared(q);
    return (magsq <= constants<T>::epsilon) ? quat_t<T>(0) : (q * inverse_sqrt(magsq));
}

///////////////////////////////////////////////////////////////////////////////
// normalize_fast
///////////////////////////////////////////////////////////////////////////////

// vec

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> normalize_fast(const vec<L, T>& v) noexcept
{
    return v * inverse_sqrt(length_squared(v));
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> normalize_fast(const quat_t<T>& q) noexcept
{
    return q * inverse_sqrt(length_squared(q));
}

///////////////////////////////////////////////////////////////////////////////
// is_normalized
///////////////////////////////////////////////////////////////////////////////

// vec

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_normalized(
    const vec<L, T>& v,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return (length_squared(v) - static_cast<T>(1)) <= epsilon;
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr bool is_normalized(
    const quat_t<T>& q,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return (length_squared(q) - static_cast<T>(1)) <= epsilon;
}

///////////////////////////////////////////////////////////////////////////////
// clamp_magnitude
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> clamp_magnitude(
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

    const T scale = math::clamp(min, max, mag) / mag;
    return v * scale;
}

///////////////////////////////////////////////////////////////////////////////
// aspect
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T aspect(T x, T y) noexcept
{
    return static_cast<T>(math::abs(y) >= constants<T>::epsilon) * (x / y);
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
// angle
///////////////////////////////////////////////////////////////////////////////

// vec2

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr auto angle(const vec<2, T>& v) noexcept
{
    return math::atan2(v.y, v.x);
}

// vec3

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T angle(
    const vec<L, T>& from,
    const vec<L, T>& to
) noexcept
{
    return math::acos(normalized_dot(from, to));
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr T angle(
    const quat_t<T>& from,
    const quat_t<T>& to
) noexcept
{
    // We use the half angle identity:
    // cos(t / 2) = sqrt[(1 + cos(t)) / 2]
    // cos(t) = cos2(t / 2) * 2 - 1
    const T d = normalized_dot(from, to);
    return math::acos_safe(d * d * static_cast<T>(2) - static_cast<T>(1));
}

///////////////////////////////////////////////////////////////////////////////
// signed_angle
///////////////////////////////////////////////////////////////////////////////

// vec

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T signed_angle(
    const vec<L, T>& from,
    const vec<L, T>& to
) noexcept
{
    const T a = angle(from, to);
    return (cross(from, to) < static_cast<T>(0)) ? -a : a;
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr T signed_angle(
    const quat_t<T>& from,
    const quat_t<T>& to
) noexcept
{
    const T a = angle(from, to);
    const T c = (from.w * to.w) - (from.x * to.x) - (from.y * to.y) - (from.z * to.z);
    return (c < static_cast<T>(0)) ? -a : a;
}

///////////////////////////////////////////////////////////////////////////////
// direction
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> direction(T angle) noexcept
{
    return vec<2, T>(math::cos(angle), math::sin(angle));
}

///////////////////////////////////////////////////////////////////////////////
// conjugate
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> conjugate(const quat_t<T>& q) noexcept
{
    return quat_t<T>(q.w, -q.x, -q.y, -q.z);
}

///////////////////////////////////////////////////////////////////////////////
// inverse
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> inverse(const quat_t<T>& q) noexcept
{
    return conjugate(q) / magnitude_squared(q);
}

///////////////////////////////////////////////////////////////////////////////
// rotate
///////////////////////////////////////////////////////////////////////////////

// axis (2d)

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> rotate(const vec<2, T>& v, T angle) noexcept
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

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

    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return v * cosa + cross(naxis, v) * sina + naxis * dot(naxis, v) * (static_cast<T>(1) - cosa);
}

// quaternion (3d)

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> rotate(
    const vec<3, T>& v,
    const quat_t<T>& rotation
) noexcept
{
    return rotation * v;
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
        : (eta * i - (eta * d + math::sqrt(k)) * n);
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

///////////////////////////////////////////////////////////////////////////////
// move_toward
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> move_toward(
    const vec<L, T>& from,
    const vec<L, T>& to,
    T delta
) noexcept
{
    const vec<L, T> vd = to - from;
    const T d = length(vd);
    return (d <= delta) ? to : (from + (vd / d * delta));
}

} // namespace math
} // namespace vx