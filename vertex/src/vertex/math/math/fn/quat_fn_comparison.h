#pragma once

#include "../detail/base_types.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== is_inf ===============

template <typename T>
inline constexpr bool is_inf(const quat_t<T>& x)
{
    return math::is_inf(x.w)
        || math::is_inf(x.x)
        || math::is_inf(x.y)
        || math::is_inf(x.z);
}

// =============== is_nan ===============

template <typename T>
inline constexpr bool is_nan(const quat_t<T>& x)
{
    return math::is_nan(x.w)
        || math::is_nan(x.x)
        || math::is_nan(x.y)
        || math::is_nan(x.z);
}

// =============== is_zero_approx ===============

template <typename T>
inline constexpr bool is_zero_approx(
    const quat_t<T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(x.w, epsilon)
        && math::is_equal_approx(x.x, epsilon)
        && math::is_equal_approx(x.y, epsilon)
        && math::is_equal_approx(x.z, epsilon);
}

// =============== is_equal_approx ===============

template <typename T>
inline constexpr bool is_equal_approx(
    const quat_t<T>& a,
    const quat_t<T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.w, b.w, epsilon)
        && math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon)
        && math::is_equal_approx(a.z, b.z, epsilon);
}

// =============== is_not_equal_approx ===============

template <typename T>
inline constexpr bool is_not_equal_approx(
    const quat_t<T>& a,
    const quat_t<T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_not_equal_approx(a.w, b.w, epsilon)
        && math::is_not_equal_approx(a.x, b.x, epsilon)
        && math::is_not_equal_approx(a.y, b.y, epsilon)
        && math::is_not_equal_approx(a.z, b.z, epsilon);
}

}
}