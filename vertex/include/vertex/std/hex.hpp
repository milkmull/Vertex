#pragma once

#include "vertex/config/type_traits.hpp"

namespace vx {
namespace hex {

inline constexpr char digits[] = "0123456789abcdef";

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
VX_NO_DISCARD constexpr C digit(const size_t value) noexcept
{
    return static_cast<C>(digits[value & 0x0F]);
}

template <size_t N, typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
VX_NO_DISCARD constexpr C digit_c() noexcept
{
    VX_STATIC_ASSERT_MSG(N < 16, "hex digit index out of range");
    return static_cast<C>(digits[N]);
}

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
VX_NO_DISCARD constexpr int value(const C c) noexcept
{
    if (c >= static_cast<C>('0') && c <= static_cast<C>('9'))
    {
        return c - static_cast<C>('0');
    }
    if (c >= static_cast<C>('A') && c <= static_cast<C>('F'))
    {
        return c - static_cast<C>('A') + 10;
    }
    if (c >= static_cast<C>('a') && c <= static_cast<C>('f'))
    {
        return c - static_cast<C>('a') + 10;
    }

    return -1;
}

VX_NO_DISCARD constexpr bool is_hex(char c) noexcept
{
    return value(c) >= 0;
}

} // namespace hex
} // namespace vx
