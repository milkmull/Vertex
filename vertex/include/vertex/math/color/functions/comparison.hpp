#pragma once

#include "../types.hpp"
#include "../../core/functions/comparison.hpp"

namespace vx {
namespace math {

//////////////////////////////////////////////////////////////////////////////
// is_inf
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool is_inf(const color_t<T>& x) noexcept
{
    return is_inf(x.r) || is_inf(x.g) || is_inf(x.b) || is_inf(x.a);
}

//////////////////////////////////////////////////////////////////////////////
// is_nan
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool is_nan(const color_t<T>& x) noexcept
{
    return is_nan(x.r) || is_nan(x.g) || is_nan(x.b) || is_nan(x.a);
}

//////////////////////////////////////////////////////////////////////////////
// is_zero_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const color_t<T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.r, epsilon)
        && is_zero_approx(x.g, epsilon)
        && is_zero_approx(x.b, epsilon)
        && is_zero_approx(x.a, epsilon);
}

//////////////////////////////////////////////////////////////////////////////
// equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
    const color_t<T>& x,
    const color_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x.r, y.r, epsilon)
        && equal_approx(x.g, y.g, epsilon)
        && equal_approx(x.b, y.b, epsilon)
        && equal_approx(x.a, y.a, epsilon);
}

//////////////////////////////////////////////////////////////////////////////
// not_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const color_t<T>& x,
    const color_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(x.r, y.r, epsilon)
        || not_equal_approx(x.g, y.g, epsilon)
        || not_equal_approx(x.b, y.b, epsilon)
        || not_equal_approx(x.a, y.a, epsilon);
}

} // namespace math
} // namespace vx