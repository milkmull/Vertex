#pragma once

#include "detail/base_type_defs.h"
#include "fn_trigonometric.h"
#include "fn_exponential.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== to_float ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr typename auto to_float(const detail::veci<L, T, Q>& v)
{
    return typename detail::veci<L, T, Q>::float_type(*this);
}

// =============== dot ===============

template <typename T>
inline constexpr T dot(
    const detail::vecx<2, T>& v,
    const detail::vecx<2, T>& u
)
{
    return (v.x * u.x) + (v.y * u.y);
}

template <typename T>
inline constexpr T dot(
    const detail::vecx<3, T>& v,
    const detail::vecx<3, T>& u
)
{
    return (v.x * u.x) + (v.y * u.y) + (v.z * u.z);
}

template <typename T>
inline constexpr T dot(
    const detail::vecx<4, T>& v,
    const detail::vecx<4, T>& u
)
{
    return (v.x * u.x) + (v.y * u.y) + (v.z * u.z) + (v.w * u.w);
}

template <typename T>
inline constexpr T dot(
    const detail::col<T>& v,
    const detail::col<T>& u
)
{
    return (v.r * u.r) + (v.g * u.g) + (v.b * u.b) + (v.a * u.a);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr T normalized_dot(
    const detail::vecf<L, T, Q>& v,
    const detail::vecf<L, T, Q>& u
)
{
    return dot(normalize(v), normalize(u));
}

// =============== cross ===============

template <typename T>
inline constexpr T cross(
    const detail::vecx<2, T>& v,
    const detail::vecx<2, T>& u
)
{
    return (v.x * u.y) - (v.y * u.x);
}

template <typename T>
inline constexpr detail::vecx<3, T> cross(
    const detail::vecx<3, T>& v,
    const detail::vecx<3, T>& u
)
{
    return detail::vecx<3, T>(
        (v.y * u.z) - (v.z * u.y),
        (v.z * u.x) - (v.x * u.z),
        (v.x * u.y) - (v.y * u.x)
    );
}

// =============== scale ===============

template <detail::length_type L, typename T, detail::vec_t Q, typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
inline constexpr detail::vec<L, T, Q> scale(
    const detail::vec<L, T, Q>& v,
    U scaler
)
{
    return v * scaler;
}

template <detail::length_type L, typename T, detail::vec_t Q, typename U>
inline constexpr detail::vec<L, T, Q> scale(
    const detail::vec<L, T, Q>& v,
    const detail::vec<L, U, Q>& scaler
)
{
    return v * scaler;
}

// =============== length ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto length_squared(const detail::vecf<L, T, Q>& v)
{
    return dot(v, v);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto length_squared(const detail::veci<L, T, Q>& v)
{
    using FT = typename detail::veci<L, T, Q>::float_value_type;
    return static_cast<FT>(dot(v, v));
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto length(const detail::vec<L, T, Q>& v)
{
    return sqrt(length_squared(v));
}

// =============== distance ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto distance_squared(
    const detail::vec<L, T, Q>& p0,
    const detail::vec<L, T, Q>& p1
)
{
    return length_squared(p1 - p0);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto distance(
    const detail::vec<L, T, Q>& p0,
    const detail::vec<L, T, Q>& p1
)
{
    return length(p1 - p0);
}

// =============== normalize ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> normalize(const detail::vecf<L, T, Q>& v)
{
    const T magsq = length_squared(v);
    if (magsq < math::epsilon<T>) return detail::vecf<L, T, Q>();
    return v * inverse_sqrt(magsq);
}

// =============== angle ===============

template <typename T>
static inline constexpr T angle(
    const detail::vecf<2, T>& from,
    const detail::vecf<2, T>& to
)
{
    return math::acos(normalized_dot(from, to));
}

template <typename T>
static inline constexpr T signed_angle(
    const detail::vecf<2, T>& from,
    const detail::vecf<2, T>& to
)
{
    const T a = angle(from, to);
    return (cross(from, to) < static_cast<T>(0)) ? -a : a;
}

// =============== project ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> project(
    const detail::vecf<L, T, Q>& i,
    const detail::vecf<L, T, Q>& n
)
{
    return n * (dot(i, n) / length_squared(n));
}

// =============== reflect ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> reflect(
    const detail::vecf<L, T, Q>& i,
    const detail::vecf<L, T, Q>& n
)
{
    return i - static_cast<T>(2) * normalized_dot(n, i) * n;
}

// =============== bounce ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> bounce(
    const detail::vecf<L, T, Q>& i,
    const detail::vecf<L, T, Q>& n
)
{
    return -reflect(i, n);
}

// =============== refract ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> refract(
    const detail::vecf<L, T, Q>& i,
    const detail::vecf<L, T, Q>& n,
    T eta
)
{
    const T d = normalized_dot(n, i);
    const T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - d * d);

    if (k < math::epsilon<T>) return detail::vec<L, T, Q>(0);
    return eta * i - (eta * d + sqrt(k)) * n;
}

// =============== face_forward ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> face_forward(
    const detail::vecf<L, T, Q>& n,
    const detail::vecf<L, T, Q>& i,
    const detail::vecf<L, T, Q>& nref
)
{
    return (normalized_dot(nref, i) < static_cast<T>(0)) ? n : -n;
}

// =============== move_toward ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> move_toward(
    const detail::vecf<L, T, Q>& from,
    const detail::vecf<L, T, Q>& to,
    T delta
)
{
    const detail::vecf<L, T, Q> vd(to - from);
    const T d = length(vd);
    return (d <= delta || d < math::epsilon<T>) ? to : (from + (vd / d * delta));
}

}
}
