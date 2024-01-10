#pragma once

#include "detail/base_type_defs.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== is_equal_approx ===============

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::quat<T>& q1,
    const detail::quat<T>& q2,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(q1.w, q2.w, epsilon)
        && math::is_equal_approx(q1.x, q2.x, epsilon)
        && math::is_equal_approx(q1.y, q2.y, epsilon)
        && math::is_equal_approx(q1.z, q2.z, epsilon);
}

// =============== is_zero_approx ===============

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::quat<T>& v,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(q.w, epsilon)
        && math::is_equal_approx(q.x, epsilon)
        && math::is_equal_approx(q.y, epsilon)
        && math::is_equal_approx(q.z, epsilon);
}

// =============== is_finite ===============

template <typename T>
inline constexpr bool is_finite(const detail::quat<T>& q)
{
    return math::is_finite(q.w)
        && math::is_finite(q.x)
        && math::is_finite(q.y)
        && math::is_finite(q.z);
}

// =============== is_infinite ===============

template <typename T>
inline constexpr bool is_infinite(const detail::quat<T>& q)
{
    return math::is_infinite(q.w)
        || math::is_infinite(q.x)
        || math::is_infinite(q.y)
        || math::is_infinite(q.z);
}

// =============== is_nan ===============

template <typename T>
inline constexpr bool is_nan(const detail::quat<T>& q)
{
    return math::is_nan(q.w)
        || math::is_nan(q.x)
        || math::is_nan(q.y)
        || math::is_nan(q.z);
}

}
}