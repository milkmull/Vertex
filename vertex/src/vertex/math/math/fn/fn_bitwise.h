#pragma once

#include <initializer_list>

#include "../detail/common.h"

namespace vx {
namespace math {

// =============== bit_count ===============

template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, bool>::type = true>
inline constexpr T bit_count(T x)
{
    T count = 0;
    while (x)
    {
        count += (x & 1);
        x >>= 1;
    }

    return count;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, bool>::type = true>
inline constexpr T bit_count(T x)
{
    using U = typename std::make_unsigned<T>::type;
    return static_cast<T>(bit_count(static_cast<U>(x)));
}

// =============== find_lsb ===============

template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, bool>::type = true>
inline constexpr T find_lsb(T x)
{
    if (x == static_cast<T>(0))
    {
        return static_cast<T>(-1);
    }

    T index = 0;
    while ((x & 1) == 0)
    {
        x >>= 1;
        ++index;
    }

    return index;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, bool>::type = true>
inline constexpr T find_lsb(T x)
{
    using U = typename std::make_unsigned<T>::type;
    return static_cast<T>(find_lsb(static_cast<U>(x)));
}

// =============== find_msb ===============

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/findMSB.xhtml

template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, bool>::type = true>
inline constexpr T find_msb(T x)
{
    if (x == static_cast<T>(0))
    {
        return static_cast<T>(-1);
    }

    T index = 0;
    while (x >>= 1)
    {
        ++index;
    }

    return index;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, bool>::type = true>
inline constexpr T find_msb(T x)
{
    if (x == static_cast<T>(-1))
    {
        return static_cast<T>(-1);
    }

    if (x < static_cast<T>(0))
    {
        x = ~x;
    }

    using U = typename std::make_unsigned<T>::type;
    return static_cast<T>(find_msb(static_cast<U>(x)));
}

// =============== reverse_bits ===============

template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, bool>::type = true>
inline constexpr T reverse_bits(T x)
{
    T rx = 0;

    while (x)
    {
        rx <<= 1;
        rx |= (x & 1);
        x >>= 1;
    }

    return rx;
}

template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, bool>::type = true>
inline constexpr T reverse_bits(T x)
{
    using U = typename std::make_unsigned<T>::type;
    return static_cast<T>(reverse_bits(static_cast<U>(x)));
}

}
}
