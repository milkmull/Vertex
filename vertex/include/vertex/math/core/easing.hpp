#pragma once

#include "constants.hpp"
#include "functions/exponential.hpp"
#include "functions/trigonometric.hpp"

namespace vx {
namespace math {
namespace ease {

// https://easings.net/

///////////////////////////////////////////////////////////////////////////////
// linear
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_linear(const T& x) noexcept
{
    return x;
}

///////////////////////////////////////////////////////////////////////////////
// quadratic
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_quad(const T& x) noexcept
{
    return x * x;
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_out_quad(const T& x) noexcept
{
    return x * (static_cast<T>(2) - x);
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_out_quad(const T& x) noexcept
{
    if (x < static_cast<T>(0.5))
    {
        return static_cast<T>(2) * x * x;
    }
    else
    {
        const T y = x * static_cast<T>(2) - static_cast<T>(2);
        return static_cast<T>(1) - (y * y) * static_cast<T>(0.5);
    }
}

///////////////////////////////////////////////////////////////////////////////
// cubic
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_cubic(const T& x) noexcept
{
    return x * x * x;
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_out_cubic(const T& x) noexcept
{
    const T y = static_cast<T>(1) - x;
    return static_cast<T>(1) - (y * y * y);
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_out_cubic(const T& x) noexcept
{
    if (x < static_cast<T>(0.5))
    {
        return static_cast<T>(4) * x * x * x;
    }
    else
    {
        const T y = static_cast<T>(2) - static_cast<T>(2) * x;
        return static_cast<T>(1) - (y * y * y) * static_cast<T>(0.5);
    }
}

///////////////////////////////////////////////////////////////////////////////
// quartic
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_quart(const T& x) noexcept
{
    return x * x * x * x;
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_out_quart(const T& x) noexcept
{
    const T y = static_cast<T>(1) - x;
    return static_cast<T>(1) - (y * y * y * y);
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_out_quart(const T& x) noexcept
{
    if (x < static_cast<T>(0.5))
    {
        return static_cast<T>(8) * x * x * x * x;
    }
    else
    {
        const T y = static_cast<T>(2) - static_cast<T>(2) * x;
        return static_cast<T>(1) - (y * y * y * y) * static_cast<T>(0.5);
    }
}

///////////////////////////////////////////////////////////////////////////////
// quintic
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_quint(const T& x) noexcept
{
    return x * x * x * x * x;
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_out_quint(const T& x) noexcept
{
    const T y = static_cast<T>(1) - x;
    return static_cast<T>(1) - (y * y * y * y * y);
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_out_quint(const T& x) noexcept
{
    if (x < static_cast<T>(0.5))
    {
        return static_cast<T>(16) * x * x * x * x * x;
    }
    else
    {
        const T y = static_cast<T>(2) - static_cast<T>(2) * x;
        return static_cast<T>(1) - (y * y * y * y * y) * static_cast<T>(0.5);
    }
}

///////////////////////////////////////////////////////////////////////////////
// sine
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_sine(const T& x) noexcept
{
    return static_cast<T>(1) - cos((x * constants<T>::pi) * static_cast<T>(0.5));
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_out_sine(const T& x) noexcept
{
    return sin((x * constants<T>::pi) * static_cast<T>(0.5));
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_out_sine(const T& x) noexcept
{
    return (static_cast<T>(1) - cos(constants<T>::pi * x)) * static_cast<T>(0.5);
}

///////////////////////////////////////////////////////////////////////////////
// exponential
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_expo(const T& x) noexcept
{
    if (x <= static_cast<T>(0))
    {
        return static_cast<T>(0);
    }
    else
    {
        const T y = static_cast<T>(10) * x - static_cast<T>(10);
        return pow(static_cast<T>(2), y);
    }
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_out_expo(const T& x) noexcept
{
    if (x >= static_cast<T>(1))
    {
        return static_cast<T>(1);
    }
    else
    {
        const T y = static_cast<T>(-10) * x;
        return static_cast<T>(1) - pow(static_cast<T>(2), y);
    }
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_out_expo(const T& x) noexcept
{
    if (x <= static_cast<T>(0))
    {
        return static_cast<T>(0);
    }
    if (x >= static_cast<T>(1))
    {
        return static_cast<T>(1);
    }
    if (x < static_cast<T>(0.5))
    {
        const T y = static_cast<T>(20) * x - static_cast<T>(10);
        return pow(static_cast<T>(2), y) * static_cast<T>(0.5);
    }
    else
    {
        const T y = static_cast<T>(-20) * x + static_cast<T>(10);
        return (static_cast<T>(2) - pow(static_cast<T>(2), y)) * static_cast<T>(0.5);
    }
}

///////////////////////////////////////////////////////////////////////////////
// circular
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_circ(const T& x) noexcept
{
    return static_cast<T>(1) - sqrt(static_cast<T>(1) - (x * x));
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_out_circ(const T& x) noexcept
{
    return sqrt(x * (static_cast<T>(2) - x));
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_out_circ(const T& x) noexcept
{
    if (x < static_cast<T>(0.5))
    {
        const T y = static_cast<T>(2) * x;
        return (static_cast<T>(1) - sqrt(static_cast<T>(1) - (y * y))) * static_cast<T>(0.5);
    }
    else
    {
        const T y = x * static_cast<T>(2) - static_cast<T>(2);
        return (sqrt(static_cast<T>(1) - (y * y)) + static_cast<T>(1)) * static_cast<T>(0.5);
    }
}

///////////////////////////////////////////////////////////////////////////////
// back
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_back(const T& x) noexcept
{
    constexpr T c1 = static_cast<T>(1.70158);
    constexpr T c3 = c1 + static_cast<T>(1);

    return c3 * x * x * x - c1 * x * x;
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_out_back(const T& x) noexcept
{
    constexpr T c1 = static_cast<T>(1.70158);
    constexpr T c3 = c1 + static_cast<T>(1);

    const T y = x - static_cast<T>(1);
    return static_cast<T>(1) + c3 * y * y * y + c1 * y * y;
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_out_back(const T& x) noexcept
{
    constexpr T c1 = static_cast<T>(1.70158);
    constexpr T c2 = c1 * static_cast<T>(1.525);

    if (x < static_cast<T>(0.5))
    {
        const T y = static_cast<T>(2) * x;
        return ((y * y) * ((c2 + static_cast<T>(1)) * static_cast<T>(2) * x - c2)) * static_cast<T>(0.5);
    }
    else
    {
        const T y = static_cast<T>(2) * x - static_cast<T>(2);
        return ((y * y) * ((c2 + static_cast<T>(1)) * y + c2) + static_cast<T>(2)) * static_cast<T>(0.5);
    }
}

///////////////////////////////////////////////////////////////////////////////
// elastic
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_elastic(const T& x) noexcept
{
    constexpr T c4 = constants<T>::two_pi / static_cast<T>(3);

    if (x == static_cast<T>(0))
    {
        return static_cast<T>(0);
    }
    if (x == static_cast<T>(1))
    {
        return static_cast<T>(1);
    }
    else
    {
        const T y = static_cast<T>(10) * x - static_cast<T>(10);
        return -pow(static_cast<T>(2), y) * sin((y - static_cast<T>(0.75)) * c4);
    }
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_out_elastic(const T& x) noexcept
{
    constexpr T c4 = constants<T>::two_pi / static_cast<T>(3);

    if (x == static_cast<T>(0))
    {
        return static_cast<T>(0);
    }
    if (x == static_cast<T>(1))
    {
        return static_cast<T>(1);
    }
    else
    {
        const T y = static_cast<T>(-10) * x;
        return pow(static_cast<T>(2), y) * sin((-y - static_cast<T>(0.75)) * c4) + static_cast<T>(1);
    }
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_out_elastic(const T& x) noexcept
{
    constexpr T c5 = constants<T>::two_pi / static_cast<T>(4.5);

    if (x == static_cast<T>(0))
    {
        return static_cast<T>(0);
    }
    if (x == static_cast<T>(1))
    {
        return static_cast<T>(1);
    }
    if (x < static_cast<T>(0.5))
    {
        const T y = static_cast<T>(20) * x - static_cast<T>(10);
        return -(pow(static_cast<T>(2), y) * sin((y - static_cast<T>(1.125)) * c5)) * static_cast<T>(0.5);
    }
    else
    {
        const T y = static_cast<T>(-20) * x + static_cast<T>(10);
        return (pow(static_cast<T>(2), y) * sin((-y - static_cast<T>(1.125)) * c5)) * static_cast<T>(0.5) + static_cast<T>(1);
    }
}

///////////////////////////////////////////////////////////////////////////////
// bounce
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_bounce(const T& x) noexcept
{
    return static_cast<T>(1) - ease_out_bounce(static_cast<T>(1) - x);
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_out_bounce(const T& x) noexcept
{
    constexpr T n1 = static_cast<T>(7.5625);
    constexpr T d1 = static_cast<T>(2.75);
    constexpr T invd1 = static_cast<T>(1) / d1;

    if (x < invd1)
    {
        return n1 * x * x;
    }
    if (x < static_cast<T>(2) * invd1)
    {
        const T y = x - static_cast<T>(1.5) * invd1;
        return n1 * y * y + static_cast<T>(0.75);
    }
    if (x < static_cast<T>(2.5) * invd1)
    {
        const T y = x - static_cast<T>(2.25) * invd1;
        return n1 * y * y + static_cast<T>(0.9375);
    }
    else
    {
        const T y = x - static_cast<T>(2.625) * invd1;
        return n1 * y * y + static_cast<T>(0.984375);
    }
}

template <typename T, VXM_REQ_FLOAT(typename scalar_type<T>::type)>
VX_FORCE_INLINE constexpr T ease_in_out_bounce(const T& x) noexcept
{
    if (x < static_cast<T>(0.5))
    {
        return (static_cast<T>(1) - ease_out_bounce(static_cast<T>(1) - static_cast<T>(2) * x)) * static_cast<T>(0.5);
    }
    else
    {
        return (static_cast<T>(1) + ease_out_bounce(static_cast<T>(2) * x - static_cast<T>(1))) * static_cast<T>(0.5);
    }
}

} // namespace ease
} // namespace math
} // namespace vx