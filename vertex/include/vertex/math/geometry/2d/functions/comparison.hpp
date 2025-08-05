#pragma once

#include "vertex/math/geometry/2d/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

namespace vx {
namespace math {
namespace g2 {

//////////////////////////////////////////////////////////////////////////////
// is_inf
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool is_inf(const line_t<T>& x) noexcept
{
    return is_inf(x.a) || is_inf(x.b);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_inf(const rect_t<T>& x) noexcept
{
    return is_inf(x.position) || is_inf(x.size);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_inf(const circle_t<T>& x) noexcept
{
    return is_inf(x.center) || is_inf(x.radius);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_inf(const triangle_t<T>& x) noexcept
{
    return is_inf(x.a) || is_inf(x.b) || is_inf(x.c);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_inf(const ray_t<T>& x) noexcept
{
    return is_inf(x.origin) || is_inf(x.direction);
}

//////////////////////////////////////////////////////////////////////////////
// is_nan
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool is_nan(const line_t<T>& x) noexcept
{
    return is_nan(x.a) || is_nan(x.b);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_nan(const rect_t<T>& x) noexcept
{
    return is_nan(x.position) || is_nan(x.size);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_nan(const circle_t<T>& x) noexcept
{
    return is_nan(x.center) || is_nan(x.radius);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_nan(const triangle_t<T>& x) noexcept
{
    return is_nan(x.a) || is_nan(x.b) || is_nan(x.c);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_nan(const ray_t<T>& x) noexcept
{
    return is_nan(x.origin) || is_nan(x.direction);
}

//////////////////////////////////////////////////////////////////////////////
// is_zero_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const line_t<T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.a, epsilon)
        && is_zero_approx(x.b, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const rect_t<T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.position, epsilon)
        && is_zero_approx(x.size, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const circle_t<T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.center, epsilon)
        && is_zero_approx(x.radius, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const triangle_t<T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.a, epsilon)
        && is_zero_approx(x.b, epsilon)
        && is_zero_approx(x.c, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const ray_t<T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.origin, epsilon)
        && is_zero_approx(x.direction, epsilon);
}

//////////////////////////////////////////////////////////////////////////////
// equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
    const line_t<T>& x,
    const line_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x.a, y.a, epsilon)
        && equal_approx(x.b, y.b, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
    const rect_t<T>& x,
    const rect_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x.position, y.position, epsilon)
        && equal_approx(x.size, y.size, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
    const circle_t<T>& x,
    const circle_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x.center, y.center, epsilon)
        && equal_approx(x.radius, y.radius, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
    const triangle_t<T>& x,
    const triangle_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x.a, y.a, epsilon)
        && equal_approx(x.b, y.b, epsilon)
        && equal_approx(x.c, y.c, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
    const ray_t<T>& x,
    const ray_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x.origin, y.origin, epsilon)
        && equal_approx(x.direction, y.direction, epsilon);
}

//////////////////////////////////////////////////////////////////////////////
// not_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const line_t<T>& x,
    const line_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(x.a, y.a, epsilon)
        || not_equal_approx(x.b, y.b, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const rect_t<T>& x,
    const rect_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(x.position, y.position, epsilon)
        || not_equal_approx(x.size, y.size, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const circle_t<T>& x,
    const circle_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(x.center, y.center, epsilon)
        || not_equal_approx(x.radius, y.radius, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const triangle_t<T>& x,
    const triangle_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(x.a, y.a, epsilon)
        || not_equal_approx(x.b, y.b, epsilon)
        || not_equal_approx(x.c, y.c, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const ray_t<T>& x,
    const ray_t<T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(x.origin, y.origin, epsilon)
        || not_equal_approx(x.direction, y.direction, epsilon);
}


} // namespace g2
} // namespace math
} // namespace vx