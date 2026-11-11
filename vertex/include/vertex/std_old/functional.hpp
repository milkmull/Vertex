#pragma once

#include "vertex/config/language_config.hpp"

namespace vx {

//==============================================================================
// plus
//==============================================================================

template <typename T = void>
struct plus
{
    VX_NO_DISCARD constexpr T operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs + rhs))
    {
        return lhs + rhs;
    }
};

template <>
struct plus<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) + static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) + static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) + static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// minus
//==============================================================================

template <typename T = void>
struct minus
{
    VX_NO_DISCARD constexpr T operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs - rhs))
    {
        return lhs - rhs;
    }
};

template <>
struct minus<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) - static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) - static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) - static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// multiplies
//==============================================================================

template <typename T = void>
struct multiplies
{
    VX_NO_DISCARD constexpr T operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs * rhs))
    {
        return lhs * rhs;
    }
};

template <>
struct multiplies<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) * static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) * static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) * static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// divides
//==============================================================================

template <typename T = void>
struct divides
{
    VX_NO_DISCARD constexpr T operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs / rhs))
    {
        return lhs / rhs;
    }
};

template <>
struct divides<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) / static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) / static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) / static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// modulus
//==============================================================================

template <typename T = void>
struct modulus
{
    VX_NO_DISCARD constexpr T operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs % rhs))
    {
        return lhs % rhs;
    }
};

template <>
struct modulus<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) % static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) % static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) % static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// negate
//==============================================================================

template <typename T = void>
struct negate
{
    VX_NO_DISCARD constexpr T operator()(const T& value) const
        noexcept(noexcept(-value))
    {
        return -value;
    }
};

template <>
struct negate<void>
{
    template <typename T>
    VX_NO_DISCARD constexpr auto operator()(T&& value) const
        noexcept(noexcept(-static_cast<T&&>(value)))
            -> decltype(-static_cast<T&&>(value))
    {
        return -static_cast<T&&>(value);
    }

    using is_transparent = void;
};

//==============================================================================
// equal_to
//==============================================================================

template <typename T = void>
struct equal_to
{
    VX_NO_DISCARD constexpr bool operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs == rhs))
    {
        return lhs == rhs;
    }
};

template <>
struct equal_to<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) == static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) == static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) == static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// not_equal_to
//==============================================================================

template <typename T = void>
struct not_equal_to
{
    VX_NO_DISCARD constexpr bool operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs != rhs))
    {
        return lhs != rhs;
    }
};

template <>
struct not_equal_to<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) != static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) != static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) != static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// greater
//==============================================================================

template <typename T = void>
struct greater
{
    VX_NO_DISCARD constexpr bool operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs > rhs))
    {
        return lhs > rhs;
    }
};

template <>
struct greater<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) > static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) > static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) > static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// greater_equal
//==============================================================================

template <typename T = void>
struct greater_equal
{
    VX_NO_DISCARD constexpr bool operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs >= rhs))
    {
        return lhs >= rhs;
    }
};

template <>
struct greater_equal<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) >= static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) >= static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) >= static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// less
//==============================================================================

template <typename T = void>
struct less
{
    VX_NO_DISCARD constexpr bool operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs < rhs))
    {
        return lhs < rhs;
    }
};

template <>
struct less<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) < static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) < static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) < static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// less_equal
//==============================================================================

template <typename T = void>
struct less_equal
{
    VX_NO_DISCARD constexpr bool operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs <= rhs))
    {
        return lhs <= rhs;
    }
};

template <>
struct less_equal<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) <= static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) <= static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) <= static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// logical_and
//==============================================================================

template <typename T = void>
struct logical_and
{
    VX_NO_DISCARD constexpr bool operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs && rhs))
    {
        return lhs && rhs;
    }
};

template <>
struct logical_and<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) && static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) && static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) && static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// logical_or
//==============================================================================

template <typename T = void>
struct logical_or
{
    VX_NO_DISCARD constexpr bool operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs || rhs))
    {
        return lhs || rhs;
    }
};

template <>
struct logical_or<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) || static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) || static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) || static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// logical_not
//==============================================================================

template <typename T = void>
struct logical_not
{
    VX_NO_DISCARD constexpr bool operator()(const T& value) const
        noexcept(noexcept(!value))
    {
        return !value;
    }
};

template <>
struct logical_not<void>
{
    template <typename T>
    VX_NO_DISCARD constexpr auto operator()(T&& value) const
        noexcept(noexcept(!static_cast<T&&>(value)))
            -> decltype(!static_cast<T&&>(value))
    {
        return !static_cast<T&&>(value);
    }

    using is_transparent = void;
};

//==============================================================================
// bit_and
//==============================================================================

template <typename T = void>
struct bit_and
{
    VX_NO_DISCARD constexpr T operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs & rhs))
    {
        return lhs & rhs;
    }
};

template <>
struct bit_and<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) & static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) & static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) & static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// bit_or
//==============================================================================

template <typename T = void>
struct bit_or
{
    VX_NO_DISCARD constexpr T operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs | rhs))
    {
        return lhs | rhs;
    }
};

template <>
struct bit_or<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) | static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) | static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) | static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// bit_xor
//==============================================================================

template <typename T = void>
struct bit_xor
{
    VX_NO_DISCARD constexpr T operator()(const T& lhs, const T& rhs) const
        noexcept(noexcept(lhs ^ rhs))
    {
        return lhs ^ rhs;
    }
};

template <>
struct bit_xor<void>
{
    template <typename T1, typename T2>
    VX_NO_DISCARD constexpr auto operator()(T1&& lhs, T2&& rhs) const
        noexcept(noexcept(static_cast<T1&&>(lhs) ^ static_cast<T2&&>(rhs)))
            -> decltype(static_cast<T1&&>(lhs) ^ static_cast<T2&&>(rhs))
    {
        return static_cast<T1&&>(lhs) ^ static_cast<T2&&>(rhs);
    }

    using is_transparent = void;
};

//==============================================================================
// bit_not
//==============================================================================

template <typename T = void>
struct bit_not
{
    VX_NO_DISCARD constexpr T operator()(const T& value) const
        noexcept(noexcept(~value))
    {
        return ~value;
    }
};

template <>
struct bit_not<void>
{
    template <typename T>
    VX_NO_DISCARD constexpr auto operator()(T&& value) const
        noexcept(noexcept(~static_cast<T&&>(value)))
            -> decltype(~static_cast<T&&>(value))
    {
        return ~static_cast<T&&>(value);
    }

    using is_transparent = void;
};

} // namespace vx
