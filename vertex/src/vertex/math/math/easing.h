#pragma once

#include "math.h"

namespace vx {
namespace math {
namespace ease {

// https://easings.net/

// =============== linear ===============

/**
 * @brief Linear easing function.
 *
 * This function represents a linear easing function, where the output
 * is equal to the input. It provides a simple linear transition.
 *
 * @param x The input value.
 * @return The linearly eased output value.
 */
template <typename T>
inline constexpr T ease_linear(T x)
{
    return x;
}

// =============== quad ===============

/**
 * @brief Quadratic easing function (ease-in).
 *
 * This function represents a quadratic easing function (ease-in),
 * where the output is proportional to the square of the input. It provides
 * a smooth acceleration at the beginning of the transition.
 *
 * @param x The input value.
 * @return The quadratically eased output value.
 */
template <typename T>
inline constexpr T ease_in_quad(T x)
{
    return x * x;
}

/**
 * @brief Quadratic easing function (ease-out).
 *
 * This function represents a quadratic easing function (ease-out),
 * where the output is obtained by subtracting the square of the inverted input
 * from 2. It provides a smooth deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The quadratically eased output value.
 */
template <typename T>
inline constexpr T ease_out_quad(T x)
{
    return x * (static_cast<T>(2) - x);
}

/**
 * @brief Quadratic easing function (ease-in-out).
 *
 * This function represents a quadratic easing function (ease-in-out),
 * where the output is a combination of the ease-in and ease-out behavior. It
 * provides a smooth acceleration at the beginning and a smooth deceleration at
 * the end of the transition.
 *
 * @param x The input value.
 * @return The quadratically eased output value.
 */
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

/**
 * @brief Cubic easing function (ease-in).
 *
 * This function represents a cubic easing function (ease-in), where the
 * output is proportional to the cube of the input. It provides a smooth acceleration
 * at the beginning of the transition.
 *
 * @param x The input value.
 * @return The cubicly eased output value.
 */
template <typename T>
inline constexpr T ease_in_cubic(T x)
{
    return x * x * x;
}

/**
 * @brief Cubic easing function (ease-out).
 *
 * This function represents a cubic easing function (ease-out), where the
 * output is obtained by subtracting the cube of the inverted input from 1. It
 * provides a smooth deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The cubicly eased output value.
 */
template <typename T>
inline constexpr T ease_out_cubic(T x)
{
    const T y = static_cast<T>(1) - x;
    return static_cast<T>(1) - (y * y * y);
}

/**
 * @brief Cubic easing function (ease-in-out).
 *
 * This function represents a cubic easing function (ease-in-out), where the
 * output is a combination of the ease-in and ease-out behavior. It provides a smooth
 * acceleration at the beginning and a smooth deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The cubicly eased output value.
 */
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

/**
 * @brief Quartic easing function (ease-in).
 *
 * This function represents a quartic easing function (ease-in), where the
 * output is proportional to the fourth power of the input. It provides a smooth
 * acceleration at the beginning of the transition.
 *
 * @param x The input value.
 * @return The quartically eased output value.
 */
template <typename T>
inline constexpr T ease_in_quart(T x)
{
    return x * x * x * x;
}

/**
 * @brief Quartic easing function (ease-out).
 *
 * This function represents a quartic easing function (ease-out), where the
 * output is obtained by subtracting the fourth power of the inverted input from 1.
 * It provides a smooth deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The quartically eased output value.
 */
template <typename T>
inline constexpr T ease_out_quart(T x)
{
    const T y = static_cast<T>(1) - x;
    return static_cast<T>(1) - (y * y * y * y);
}

/**
 * @brief Quartic easing function (ease-in-out).
 *
 * This function represents a quartic easing function (ease-in-out), where
 * the output is a combination of the ease-in and ease-out behavior. It provides a
 * smooth acceleration at the beginning and a smooth deceleration at the end of the
 * transition.
 *
 * @param x The input value.
 * @return The quartically eased output value.
 */
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

/**
 * @brief Quintic easing function (ease-in).
 *
 * This function represents a quintic easing function (ease-in), where
 * the output is proportional to the fifth power of the input. It provides a
 * smooth acceleration at the beginning of the transition.
 *
 * @param x The input value.
 * @return The quintically eased output value.
 */
template <typename T>
inline constexpr T ease_in_quint(T x)
{
    return x * x * x * x * x;
}

/**
 * @brief Quintic easing function (ease-out).
 *
 * This function represents a quintic easing function (ease-out), where
 * the output is obtained by subtracting the fifth power of the inverted input from 1.
 * It provides a smooth deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The quintically eased output value.
 */
template <typename T>
inline constexpr T ease_out_quint(T x)
{
    const T y = static_cast<T>(1) - x;
    return static_cast<T>(1) - (y * y * y * y * y);
}

/**
 * @brief Quintic easing function (ease-in-out).
 *
 * This function represents a quintic easing function (ease-in-out), where
 * the output is a combination of the ease-in and ease-out behavior. It provides a
 * smooth acceleration at the beginning and a smooth deceleration at the end of the
 * transition.
 *
 * @param x The input value.
 * @return The quintically eased output value.
 */
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

/**
 * @brief Sine easing function (ease-in).
 *
 * This function represents a sine easing function (ease-in), where
 * the output is obtained by subtracting the cosine of the input multiplied by
 * pi/2 from 1. It provides a smooth acceleration at the beginning of the transition.
 *
 * @param x The input value.
 * @return The sine-eased output value.
 */
template <typename T>
inline constexpr T ease_in_sine(T x)
{
    return static_cast<T>(1) - math::cos((x * math::pi<T>) * static_cast<T>(0.5));
}

/**
 * @brief Sine easing function (ease-out).
 *
 * This function represents a sine easing function (ease-out), where
 * the output is obtained by taking the sine of the input multiplied by pi/2.
 * It provides a smooth deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The sine-eased output value.
 */
template <typename T>
inline constexpr T ease_out_sine(T x)
{
    return math::sin((x * math::pi<T>) * static_cast<T>(0.5));
}

/**
 * @brief Sine easing function (ease-in-out).
 *
 * This function represents a sine easing function (ease-in-out), where
 * the output is a combination of the ease-in and ease-out behavior. It provides a
 * smooth acceleration at the beginning and a smooth deceleration at the end of the
 * transition.
 *
 * @param x The input value.
 * @return The sine-eased output value.
 */
template <typename T>
inline constexpr T ease_in_out_sine(T x)
{
    return (static_cast<T>(1) - math::cos(math::pi<T> *x)) * static_cast<T>(0.5);
}

// =============== expo ===============

/**
 * @brief Exponential easing function (ease-in).
 *
 * This function represents an exponential easing function (ease-in), where
 * the output is obtained by raising 2 to the power of (10 * x - 10) for positive x,
 * and 0 for non-positive x. It provides a smooth acceleration at the beginning of
 * the transition.
 *
 * @param x The input value.
 * @return The exponentially eased output value.
 */
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

/**
 * @brief Exponential easing function (ease-out).
 *
 * This function represents an exponential easing function (ease-out), where
 * the output is obtained by subtracting 2 to the power of (-10 * x) from 1 for x in
 * the range [0, 1], and 1 for x greater than or equal to 1. It provides a smooth
 * deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The exponentially eased output value.
 */
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

/**
 * @brief Exponential easing function (ease-in-out).
 *
 * This function represents an exponential easing function (ease-in-out),
 * where the output is a combination of the ease-in and ease-out behavior. It provides
 * a smooth acceleration at the beginning and a smooth deceleration at the end of the
 * transition.
 *
 * @param x The input value.
 * @return The exponentially eased output value.
 */
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

/**
 * @brief Circular easing function (ease-in).
 *
 * This function represents a circular easing function (ease-in), where
 * the output is obtained by subtracting the square root of (1 - x^2) from 1.
 * It provides a smooth acceleration at the beginning of the transition.
 *
 * @param x The input value.
 * @return The circularly eased output value.
 */
template <typename T>
inline constexpr T ease_in_circ(T x)
{
    return static_cast<T>(1) - math::sqrt(static_cast<T>(1) - (x * x));
}

/**
 * @brief Circular easing function (ease-out).
 *
 * This function represents a circular easing function (ease-out), where
 * the output is obtained by taking the square root of (x * (2 - x)). It provides
 * a smooth deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The circularly eased output value.
 */
template <typename T>
inline constexpr T ease_out_circ(T x)
{
    return math::sqrt(x * (static_cast<T>(2) - x));
}

/**
 * @brief Circular easing function (ease-in-out).
 *
 * This function represents a circular easing function (ease-in-out), where
 * the output is a combination of the ease-in and ease-out behavior. It provides a
 * smooth acceleration at the beginning and a smooth deceleration at the end of the
 * transition.
 *
 * @param x The input value.
 * @return The circularly eased output value.
 */
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

/**
 * @brief Back easing function (ease-in).
 *
 * This function represents a back easing function (ease-in), where
 * the output is obtained by applying a cubic function with specific constants.
 * It provides a smooth acceleration at the beginning of the transition.
 *
 * @param x The input value.
 * @return The back-eased output value.
 */
template <typename T>
inline constexpr T ease_in_back(T x)
{
    constexpr T c1 = static_cast<T>(1.70158);
    constexpr T c3 = c1 + static_cast<T>(1);

    return c3 * x * x * x - c1 * x * x;
}

/**
 * @brief Back easing function (ease-out).
 *
 * This function represents a back easing function (ease-out), where
 * the output is obtained by applying a cubic function with specific constants.
 * It provides a smooth deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The back-eased output value.
 */
template <typename T>
inline constexpr T ease_out_back(T x)
{
    constexpr T c1 = static_cast<T>(1.70158);
    constexpr T c3 = c1 + static_cast<T>(1);

    const T y = x - static_cast<T>(1);
    return static_cast<T>(1) + c3 * y * y * y + c1 * y * y;
}

/**
 * @brief Back easing function (ease-in-out).
 *
 * This function represents a back easing function (ease-in-out), where
 * the output is a combination of the ease-in and ease-out behavior. It provides a
 * smooth acceleration at the beginning and a smooth deceleration at the end of the
 * transition.
 *
 * @param x The input value.
 * @return The back-eased output value.
 */
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

/**
 * @brief Elastic easing function (ease-in).
 *
 * This function represents an elastic easing function (ease-in), where
 * the output is obtained by applying an elastic function with specific constants.
 * It provides a smooth acceleration at the beginning of the transition.
 *
 * @param x The input value.
 * @return The elastically eased output value.
 */
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

/**
 * @brief Elastic easing function (ease-out).
 *
 * This function represents an elastic easing function (ease-out), where
 * the output is obtained by applying an elastic function with specific constants.
 * It provides a smooth deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The elastically eased output value.
 */
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

/**
 * @brief Elastic easing function (ease-in-out).
 *
 * This function represents an elastic easing function (ease-in-out), where
 * the output is a combination of the ease-in and ease-out behavior. It provides a
 * smooth acceleration at the beginning and a smooth deceleration at the end of the
 * transition.
 *
 * @param x The input value.
 * @return The elastically eased output value.
 */
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

/**
 * @brief Bounce easing function (ease-in).
 *
 * This function represents a bounce easing function (ease-in), where
 * the output is obtained by subtracting the ease-out-bounce value from 1. It
 * provides a smooth acceleration at the beginning of the transition.
 *
 * @param x The input value.
 * @return The bounced-eased output value.
 */
template <typename T>
inline constexpr T ease_in_bounce(T x)
{
    return static_cast<T>(1) - ease_out_bounce(static_cast<T>(1) - x);
}

/**
 * @brief Bounce easing function (ease-out).
 *
 * This function represents a bounce easing function (ease-out), where
 * the output is obtained by applying a piecewise quadratic function with specific
 * constants. It provides a smooth deceleration at the end of the transition.
 *
 * @param x The input value.
 * @return The bounced-eased output value.
 */
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

/**
 * @brief Bounce easing function (ease-in-out).
 *
 * This function represents a bounce easing function (ease-in-out), where
 * the output is a combination of the ease-in and ease-out behavior. It provides a
 * smooth acceleration at the beginning and a smooth deceleration at the end of the
 * transition.
 *
 * @param x The input value.
 * @return The bounced-eased output value.
 */
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
}
