#pragma once

#include <limits>

#include "vertex/std/math/config.hpp"

namespace vx {

template <typename T>
using numeric_limits = std::numeric_limits<T>;

// addition: would (a + b) overflow?
template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
constexpr bool add_overflow(T a, T b) noexcept
{
    VX_IF_CONSTEXPR (std::is_signed<T>::value)
    {
        if (b > T(0) && a > numeric_limits<T>::max() - b)
        {
            return true;
        }
        if (b < T(0) && a < numeric_limits<T>::min() - b)
        {
            return true;
        }

        return false;
    }
    else
    {
        return a > numeric_limits<T>::max() - b;
    }
}

// subtraction: would (a - b) overflow?
template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
constexpr bool sub_overflow(T a, T b) noexcept
{
    VX_IF_CONSTEXPR (std::is_signed<T>::value)
    {
        if (b < T(0) && a > numeric_limits<T>::max() + b)
        {
            return true;
        }
        if (b > T(0) && a < numeric_limits<T>::min() + b)
        {
            return true;
        }

        return false;
    }
    else
    {
        return a < b;
    }
}

// multiplication: would (a * b) overflow?
template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
constexpr bool mul_overflow(T a, T b) noexcept
{
    if (a == T(0) || b == T(0))
    {
        return false;
    }

    VX_IF_CONSTEXPR (std::is_signed<T>::value)
    {
        if (a == T(-1))
        {
            return b == numeric_limits<T>::min();
        }
        if (b == T(-1))
        {
            return a == numeric_limits<T>::min();
        }
        if ((a > T(0)) == (b > T(0)))
        {
            return a > numeric_limits<T>::max() / b;
        }
        else
        {
            return a < numeric_limits<T>::min() / b;
        }
    }
    else
    {
        return a > numeric_limits<T>::max() / b;
    }
}

// would casting value of type From to type To lose information?
template <typename To, typename From, VX_REQUIRES(std::is_integral<To>::value&& std::is_integral<From>::value)>
constexpr bool cast_overflow(From value) noexcept
{
    VX_IF_CONSTEXPR (std::is_signed<From>::value && std::is_unsigned<To>::value)
    {
        if (value < From(0))
        {
            return true;
        }
    }

    VX_IF_CONSTEXPR (sizeof(To) >= sizeof(From) && std::is_signed<To>::value == std::is_signed<From>::value)
    {
        return false; // always safe
    }

    using C = typename std::common_type<To, From>::type;
    return static_cast<C>(value) < static_cast<C>(numeric_limits<To>::min()) || static_cast<C>(value) > static_cast<C>(numeric_limits<To>::max());
}

// signed -> unsigned: shifts [-min, max] -> [0, max - min]
// e.g. int8_t  [-128, 127] -> uint8_t [0, 255]
template <typename T, VX_REQUIRES(std::is_integral<T>::value&& std::is_signed<T>::value)>
constexpr auto to_unsigned_range(T value) noexcept
{
    using U = typename std::make_unsigned<T>::type;
    return static_cast<U>(value) - static_cast<U>(numeric_limits<T>::min());
}

// unsigned -> signed: shifts [0, max - min] -> [-min, max]
// e.g. uint8_t [0, 255] -> int8_t [-128, 127]
template <typename T, VX_REQUIRES(std::is_integral<T>::value&& std::is_unsigned<T>::value)>
constexpr auto to_signed_range(T value) noexcept
{
    using S = typename std::make_signed<T>::type;
    return static_cast<S>(value + static_cast<T>(numeric_limits<S>::min()));
}

} // namespace vx
