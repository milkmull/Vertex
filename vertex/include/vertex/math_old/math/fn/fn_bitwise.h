#pragma once

#include "../util/type_traits.h"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// bit_count
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Counts the number of set bits (1s) in the binary representation 
/// of an integer.
///
/// @tparam T The type of the integer.
/// @param x The integer for which to count the set bits.
/// 
/// @return The number of set bits in the binary representation of the integer.
///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
// find_lsb
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Finds the index of the least significant bit (LSB) that is set to 1 
/// in the binary representation of an integer.
///
/// If no bit is set (the input is zero), it returns -1.
///
/// @tparam T The type of the integer.
/// @param x The integer for which to find the index of the least significant bit.
/// 
/// @return The index of the least significant bit (LSB) that is set to 1, or 
/// -1 if no bit is set.
///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
// find_msb
///////////////////////////////////////////////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/findMSB.xhtml

///////////////////////////////////////////////////////////////////////////////
/// @brief Finds the index of the most significant bit (MSB) that is set to 1 
/// in the binary representation of an integer.
///
/// For positive integers, the result will be the bit number of the most
/// significant bit that is set to 1. For negative integers, the result will be
/// the bit number of the most significant bit set to 0. For a value of zero or 
/// negative 1, -1 will be returned.
///
/// @tparam T The type of the integer.
/// @param x The integer for which to find the index of the most significant bit.
/// 
/// @return The index of the most significant bit (MSB) that is set to 1, or -1 
/// if no bit is set.
///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
// reverse_bits
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Reverses the order of bits in the binary representation of an integer.
///
/// @tparam T The type of the integer.
/// @param x The integer for which to reverse the bits.
/// @return The integer with its bits reversed.
///////////////////////////////////////////////////////////////////////////////
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
