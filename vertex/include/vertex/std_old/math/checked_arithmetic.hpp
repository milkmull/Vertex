#pragma once

#include <limits>

#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"

namespace vx {
namespace math {

namespace _checked_arithmetic_priv {

//=========================================================================
// unsigned fallback
//=========================================================================

template <typename T, VX_REQUIRES(std::is_unsigned<T>::value)>
constexpr bool fallback_add(T a, T b, T& out) noexcept
{
    if (b > std::numeric_limits<T>::max() - a)
    {
        return false;
    }

    out = static_cast<T>(a + b);
    return true;
}

template <typename T, VX_REQUIRES(std::is_unsigned<T>::value)>
constexpr bool fallback_sub(T a, T b, T& out) noexcept
{
    if (b > a)
    {
        return false;
    }
    out = static_cast<T>(a - b);
    return true;
}

template <typename T, VX_REQUIRES(std::is_unsigned<T>::value)>
constexpr bool fallback_mul(T a, T b, T& out) noexcept
{
    if (a != 0 && b > std::numeric_limits<T>::max() / a)
    {
        return false;
    }
    out = static_cast<T>(a * b);
    return true;
}

//=========================================================================
// signed fallback
//=========================================================================

template <typename T, VX_REQUIRES(std::is_signed<T>::value)>
constexpr bool fallback_add(T a, T b, T& out) noexcept
{
    if ((b > 0 && a > std::numeric_limits<T>::max() - b) ||
        (b < 0 && a < std::numeric_limits<T>::min() - b))
    {
        return false;
    }
    out = static_cast<T>(a + b);
    return true;
}

template <typename T, VX_REQUIRES(std::is_signed<T>::value)>
constexpr bool fallback_sub(T a, T b, T& out) noexcept
{
    if ((b < 0 && a > std::numeric_limits<T>::max() + b) ||
        (b > 0 && a < std::numeric_limits<T>::min() + b))
    {
        return false;
    }
    out = static_cast<T>(a - b);
    return true;
}

template <typename T, VX_REQUIRES(std::is_signed<T>::value)>
constexpr bool fallback_mul(T a, T b, T& out) noexcept
{
    if (a == 0 || b == 0)
    {
        out = 0;
        return true;
    }

    // Guard the one case where negating b below would itself overflow.
    if (a == -1 && b == std::numeric_limits<T>::min())
    {
        return false;
    }
    if (b == -1 && a == std::numeric_limits<T>::min())
    {
        return false;
    }

    const T abs_a = a < 0 ? static_cast<T>(-a) : a;
    const T abs_b = b < 0 ? static_cast<T>(-b) : b;

    if (abs_a > std::numeric_limits<T>::max() / abs_b)
    {
        return false;
    }

    out = static_cast<T>(a * b);
    return true;
}

} // namespace _checked_arithmetic_priv

//=========================================================================
// public api
//=========================================================================

// out = a + b. Returns false (out left unmodified) on overflow.
template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
constexpr VX_NO_DISCARD bool checked_add(T a, T b, T& out) noexcept
{
#if VX_HAS_BUILTIN(__builtin_add_overflow)

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        return !__builtin_add_overflow(a, b, &out);
    }

#endif

    return _checked_arithmetic_priv::fallback_add<T>(a, b, out);
}

// out = a - b. Returns false (out left unmodified) on overflow/underflow.
template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
constexpr VX_NO_DISCARD bool checked_sub(T a, T b, T& out) noexcept
{
#if VX_HAS_BUILTIN(__builtin_sub_overflow)

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        return !__builtin_sub_overflow(a, b, &out);
    }

#endif

    return _checked_arithmetic_priv::fallback_sub<T>(a, b, out);
}

// out = a * b. Returns false (out left unmodified) on overflow.
template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
constexpr VX_NO_DISCARD bool checked_mul(T a, T b, T& out) noexcept
{
#if VX_HAS_BUILTIN(__builtin_mul_overflow)

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        return !__builtin_mul_overflow(a, b, &out);
    }

#endif

    return _checked_arithmetic_priv::fallback_mul<T>(a, b, out);
}

// out = a / b. Returns false on divide-by-zero, or on the single signed
// overflow case (T::min() / -1).
template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
constexpr VX_NO_DISCARD bool checked_div(T a, T b, T& out) noexcept
{
    if (b == 0)
    {
        return false;
    }

    if (std::is_signed<T>::value && a == std::numeric_limits<T>::min() && b == static_cast<T>(-1))
    {
        return false;
    }

    out = static_cast<T>(a / b);
    return true;
}

// out = a % b. Same failure cases as checked_div.
template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
constexpr VX_NO_DISCARD bool checked_mod(T a, T b, T& out) noexcept
{
    if (b == 0)
    {
        return false;
    }

    if (std::is_signed<T>::value && a == std::numeric_limits<T>::min() && b == static_cast<T>(-1))
    {
        return false;
    }

    out = static_cast<T>(a % b);
    return true;
}

// out = -a. Only meaningful for signed types; fails on T::min().
template <typename T, VX_REQUIRES(std::is_signed<T>::value)>
constexpr VX_NO_DISCARD bool checked_negate(T a, T& out) noexcept
{
    if (a == std::numeric_limits<T>::min())
    {
        return false;
    }

    out = static_cast<T>(-a);
    return true;
}

} // namespace math
} // namespace vx
