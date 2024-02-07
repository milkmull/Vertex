#pragma once

#include "../detail/base_types.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== is_equal_approx ===============

template <typename T>
inline constexpr bool is_equal_approx(
    const detail::quat<T>& a,
    const detail::quat<T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.w, b.w, epsilon)
        && math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon)
        && math::is_equal_approx(a.z, b.z, epsilon);
}

// =============== is_zero_approx ===============

template <typename T>
inline constexpr bool is_zero_approx(
    const detail::quat<T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(x.w, epsilon)
        && math::is_equal_approx(x.x, epsilon)
        && math::is_equal_approx(x.y, epsilon)
        && math::is_equal_approx(x.z, epsilon);
}

// =============== is_finite ===============

template <typename T>
inline constexpr bool is_finite(const detail::quat<T>& x)
{
    return math::is_finite(x.w)
        && math::is_finite(x.x)
        && math::is_finite(x.y)
        && math::is_finite(x.z);
}

// =============== is_infinite ===============

template <typename T>
inline constexpr bool is_infinite(const detail::quat<T>& x)
{
    return math::is_infinite(x.w)
        || math::is_infinite(x.x)
        || math::is_infinite(x.y)
        || math::is_infinite(x.z);
}

// =============== is_nan ===============

template <typename T>
inline constexpr bool is_nan(const detail::quat<T>& x)
{
    return math::is_nan(x.w)
        || math::is_nan(x.x)
        || math::is_nan(x.y)
        || math::is_nan(x.z);
}

}
}