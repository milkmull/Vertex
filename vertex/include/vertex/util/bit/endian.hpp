#pragma once

#include "vertex/util/bit/bit.hpp"

namespace vx {
namespace endian {

///////////////////////////////////////////////////////////////////////////////
// endianness
///////////////////////////////////////////////////////////////////////////////

#define VX_ORDER_BIG_ENDIAN    0
#define VX_ORDER_LITTLE_ENDIAN 1

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN

#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_BIG_ENDIAN

#elif defined(__BYTE_ORDER__) && defined(__ORDER_PDP_ENDIAN__) && __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__

    #error Vertex does not support platforms with PDP endianness

#elif defined(__LITTLE_ENDIAN__)

    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN

#elif defined(__BIG_ENDIAN__)

    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_BIG_ENDIAN

#elif defined(_MSC_VER) || defined(__i386__) || defined(__x86_64__)

    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN

#else

    #error Vertex could not determine the endianness of this platform.

#endif

enum class order
{
    big = VX_ORDER_BIG_ENDIAN,
    little = VX_ORDER_LITTLE_ENDIAN,
    native = VX_ORDER_NATIVE_ENDIAN
};

/**
 * @brief Converts a value from one byte order to another.
 *
 * If the source and target byte orders are the same, the value is returned unchanged.
 * Otherwise, the value is byte-swapped using `bit::byteswap`.
 *
 * @tparam T A trivially copyable, integral or floating-point type.
 * @param x The value to convert.
 * @param from The byte order to convert from.
 * @param to The byte order to convert to.
 * @return The converted value.
 */
template <typename T>
constexpr T convert(T x, order from, order to) noexcept
{
    return (from == to) ? x : bit::byteswap(x);
}

/**
 * @brief Converts a big-endian value to native endianness.
 *
 * @tparam T The type of the value.
 * @param value The value in big-endian format.
 * @return The value in native byte order.
 */
template <typename T>
constexpr T big_to_native(T value) noexcept
{
    return convert(value, order::big, order::native);
}

/**
 * @brief Converts a native-endian value to big-endian.
 *
 * @tparam T The type of the value.
 * @param value The value in native byte order.
 * @return The value in big-endian format.
 */
template <typename T>
constexpr T native_to_big(T value) noexcept
{
    return convert(value, order::native, order::big);
}

/**
 * @brief Converts a little-endian value to native endianness.
 *
 * @tparam T The type of the value.
 * @param value The value in little-endian format.
 * @return The value in native byte order.
 */
template <typename T>
constexpr T little_to_native(T value) noexcept
{
    return convert(value, order::little, order::native);
}

/**
 * @brief Converts a native-endian value to little-endian.
 *
 * @tparam T The type of the value.
 * @param value The value in native byte order.
 * @return The value in little-endian format.
 */
template <typename T>
constexpr T native_to_little(T value) noexcept
{
    return convert(value, order::native, order::little);
}

} // namespace endian

} // namespace vx
