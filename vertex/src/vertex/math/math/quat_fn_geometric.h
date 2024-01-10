#pragma once

#include "fn_trigonometric.h"
#include "fn_exponential.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== dot ===============

// the dot product of 2 quaternions gives cos(t / 2)

template <typename T>
inline constexpr T dot(
    const detail::quat<T>& q1,
    const detail::quat<T>& q2
)
{
    return (q1.w * q2.w) + (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z);
}

template <typename T>
inline constexpr T normalized_dot(
    const detail::quat<T>& q1,
    const detail::quat<T>& q2
)
{
    return dot(normalize(q1), normalize(q2));
}

// =============== scale ===============

template <typename T, typename U>
inline constexpr detail::quat<T> scale(
    const detail::quat<T>& q,
    U scaler
)
{
    return q * scaler;
}

// =============== length ===============

template <typename T>
inline constexpr T length_squared(const detail::quat<T>& q)
{
    return dot(q, q);
}

template <typename T>
inline constexpr T length(const detail::quat<T>& q)
{
    return sqrt(length_squared(q));
}

// =============== normalize ===============

template <typename T>
inline constexpr auto normalize(const detail::quat<T>& q)
{
    const T magsq = length_squared(q);
    if (magsq < math::epsilon<T>) return detail::quat<T>();
    return q * math::inverse_sqrt(magsq);
}

// =============== angle ===============

template <typename T>
static inline constexpr T angle(
    const detail::quat<T>& from,
    const detail::quat<T>& to
)
{
    // We use the half angle identity:
    // cos(t / 2) = sqrt[(1 + cos(t)) / 2]
    // cos(t) = cos2(t / 2) * 2 - 1
    const T d = normalized_dot(from, to);
    return math::acos_clamped(d * d * static_cast<T>(2) - static_cast<T>(1));
}

template <typename T>
static inline constexpr T signed_angle(
    const detail::quat<T>& from,
    const detail::quat<T>& to
)
{
    const T a = angle(from, to);
    const T c = (from.w * to.w) - (from.x * to.x) - (from.y * to.y) - (from.z * to.z);
    return (c < static_cast<T>(0)) ? -a : a;
}

}
}
