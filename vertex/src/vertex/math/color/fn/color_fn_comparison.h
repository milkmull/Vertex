#pragma once

#include "../type/color.h"
#include "../../math/fn/fn_comparison.h"

namespace vx {
namespace math {

// =============== is_inf ===============

template <typename T>
inline constexpr bool is_inf(const color_t<T>& x)
{
    return math::is_inf(x.r)
        || math::is_inf(x.g)
        || math::is_inf(x.b)
        || math::is_inf(x.a);
}

// =============== is_nan ===============

template <typename T>
inline constexpr bool is_nan(const color_t<T>& x)
{
    return math::is_nan(x.r)
        || math::is_nan(x.g)
        || math::is_nan(x.b)
        || math::is_nan(x.a);
}

// =============== is_zero_approx ===============

template <typename T>
inline constexpr bool is_zero_approx(
    const color_t<T>& x,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_zero_approx(x.r, epsilon)
        && math::is_zero_approx(x.g, epsilon)
        && math::is_zero_approx(x.b, epsilon)
        && math::is_zero_approx(x.a, epsilon);
}

// =============== is_equal_approx ===============

template <typename T>
inline constexpr bool is_equal_approx(
    const color_t<T>& a,
    const color_t<T>& b,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_equal_approx(a.r, b.r, epsilon)
        && math::is_equal_approx(a.g, b.g, epsilon)
        && math::is_equal_approx(a.b, b.b, epsilon)
        && math::is_equal_approx(a.a, b.a, epsilon);
}

// =============== is_not_equal_approx ===============

template <typename T>
inline constexpr bool is_not_equal_approx(
    const color_t<T>& a,
    const color_t<T>& b,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_not_equal_approx(a.r, b.r, epsilon)
        && math::is_not_equal_approx(a.g, b.g, epsilon)
        && math::is_not_equal_approx(a.b, b.b, epsilon)
        && math::is_not_equal_approx(a.a, b.a, epsilon);
}

}
}