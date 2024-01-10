#pragma once

#include "constants.h"
#include "fn_exponential.h"
#include "fn_trigonometric.h"

namespace vx {
namespace math {

// https://easings.net/

// =============== linear ===============

template <typename T>
inline constexpr T ease_linear(T x)
{
    return x;
}

// =============== quad ===============

template <typename T>
inline constexpr T ease_in_quad(T x)
{
    return x * x;
}

template <typename T>
inline constexpr T ease_out_quad(T x)
{
    return x * (static_cast<T>(2) - x);
}

template <typename T>
inline constexpr T ease_in_out_quad(T x)
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

// =============== cubic ===============

template <typename T>
inline constexpr T ease_in_cubic(T x)
{
    return x * x * x;
}

template <typename T>
inline constexpr T ease_out_cubic(T x)
{
    const T y = static_cast<T>(1) - x;
    return static_cast<T>(1) - (y * y * y);
}

template <typename T>
inline constexpr T ease_in_out_cubic(T x)
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

// =============== quart ===============

template <typename T>
inline constexpr T ease_in_quart(T x)
{
    return x * x * x * x;
}

template <typename T>
inline constexpr T ease_out_quart(T x)
{
    const T y = static_cast<T>(1) - x;
    return static_cast<T>(1) - (y * y * y * y);
}

template <typename T>
inline constexpr T ease_in_out_quart(T x)
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

// =============== quint ===============

template <typename T>
inline constexpr T ease_in_quint(T x)
{
    return x * x * x * x * x;
}

template <typename T>
inline constexpr T ease_out_quint(T x)
{
    const T y = static_cast<T>(1) - x;
    return static_cast<T>(1) - (y * y * y * y * y);
}

template <typename T>
inline constexpr T ease_in_out_quint(T x)
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

// =============== sine ===============

template <typename T>
inline constexpr T ease_in_sine(T x)
{
    return static_cast<T>(1) - math::cos((x * math::pi<T>) * static_cast<T>(0.5));
}

template <typename T>
inline constexpr T ease_out_sine(T x)
{
    return math::sin((x * math::pi<T>) * static_cast<T>(0.5));
}

template <typename T>
inline constexpr T ease_in_out_sine(T x)
{
    return (static_cast<T>(1) - math::cos(math::pi<T> *x)) * static_cast<T>(0.5);
}

// =============== expo ===============

template <typename T>
inline constexpr T ease_in_expo(T x)
{
    if (x <= static_cast<T>(0))
    {
        return static_cast<T>(0);
    }
    else
    {
        const T y = static_cast<T>(10) * x - static_cast<T>(10);
        return math::pow(static_cast<T>(2), y);
    }
}

template <typename T>
inline constexpr T ease_out_expo(T x)
{
    if (x >= static_cast<T>(1))
    {
        return static_cast<T>(1);
    }
    else
    {
        const T y = static_cast<T>(-10) * x;
        return static_cast<T>(1) - math::pow(static_cast<T>(2), y);
    }
}

template <typename T>
inline constexpr T ease_in_out_expo(T x)
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
        return math::pow(static_cast<T>(2), y) * static_cast<T>(0.5);
    }
    else
    {
        const T y = static_cast<T>(-20) * x + static_cast<T>(10);
        return (static_cast<T>(2) - math::pow(static_cast<T>(2), y)) * static_cast<T>(0.5);
    }
}

// =============== circ ===============

template <typename T>
inline constexpr T ease_in_circ(T x)
{
    return static_cast<T>(1) - math::sqrt(static_cast<T>(1) - (x * x));
}

template <typename T>
inline constexpr T ease_out_circ(T x)
{
    return math::sqrt(x * (static_cast<T>(2) - x));
}

template <typename T>
inline constexpr T ease_in_out_circ(T x)
{
    if (x < static_cast<T>(0.5))
    {
        const T y = static_cast<T>(2) * x;
        return (static_cast<T>(1) - math::sqrt(static_cast<T>(1) - (y * y))) * static_cast<T>(0.5);
    }
    else
    {
        const T y = x * static_cast<T>(2) - static_cast<T>(2);
        return (math::sqrt(static_cast<T>(1) - (y * y)) + static_cast<T>(1)) * static_cast<T>(0.5);
    }
}

// =============== back ===============

template <typename T>
inline constexpr T ease_in_back(T x)
{
    constexpr T c1 = static_cast<T>(1.70158);
    constexpr T c3 = c1 + static_cast<T>(1);

    return c3 * x * x * x - c1 * x * x;
}

template <typename T>
inline constexpr T ease_out_back(T x)
{
    constexpr T c1 = static_cast<T>(1.70158);
    constexpr T c3 = c1 + static_cast<T>(1);

    const T y = x - static_cast<T>(1);
    return static_cast<T>(1) + c3 * y * y * y + c1 * y * y;
}

template <typename T>
inline constexpr T ease_in_out_back(T x)
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

// =============== elastic ===============

template <typename T>
inline constexpr T ease_in_elastic(T x)
{
    constexpr T c4 = math::two_pi<T> / static_cast<T>(3);

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
        return -math::pow(static_cast<T>(2), y) * math::sin((y - static_cast<T>(0.75)) * c4);
    }
}

template <typename T>
inline constexpr T ease_out_elastic(T x)
{
    constexpr T c4 = math::two_pi<T> / static_cast<T>(3);

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
        return math::pow(static_cast<T>(2), y) * math::sin((-y - static_cast<T>(0.75)) * c4) + static_cast<T>(1);
    }
}

template <typename T>
inline constexpr T ease_in_out_elastic(T x)
{
    constexpr T c5 = math::two_pi<T> / static_cast<T>(4.5);

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
        return -(math::pow(static_cast<T>(2), y) * math::sin((y - static_cast<T>(1.125)) * c5)) * static_cast<T>(0.5);
    }
    else
    {
        const T y = static_cast<T>(-20) * x + static_cast<T>(10);
        return (math::pow(static_cast<T>(2), y) * math::sin((-y - static_cast<T>(1.125)) * c5)) * static_cast<T>(0.5) + static_cast<T>(1);
    }
}

// =============== bounce ===============

template <typename T>
inline constexpr T ease_in_bounce(T x)
{
    return static_cast<T>(1) - ease_out_bounce(static_cast<T>(1) - x);
}

template <typename T>
inline constexpr T ease_out_bounce(T x)
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

template <typename T>
inline constexpr T ease_in_out_bounce(T x)
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

}
}
