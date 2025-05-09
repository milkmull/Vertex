#pragma once

#include "vertex/util/bit/bit.hpp"

namespace vx {
namespace endian {

///////////////////////////////////////////////////////////////////////////////
// endianness
///////////////////////////////////////////////////////////////////////////////

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#   define VX_ENDIAN_NATIVE_ORDER_INITIALIZER LITTLE

#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

#   define VX_ENDIAN_NATIVE_ORDER_INITIALIZER BIG

#elif defined(__BYTE_ORDER__) && defined(__ORDER_PDP_ENDIAN__) && __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__

#   error Vertex does not support platforms with PDP endianness

#elif defined(__LITTLE_ENDIAN__)

#   define VX_ENDIAN_NATIVE_ORDER_INITIALIZER LITTLE

#elif defined(__BIG_ENDIAN__)

#   define VX_ENDIAN_NATIVE_ORDER_INITIALIZER BIG

#elif defined(_MSC_VER) || defined(__i386__) || defined(__x86_64__)

#   define VX_ENDIAN_NATIVE_ORDER_INITIALIZER LITTLE

#else

#   error Vertex could not determine the endianness of this platform.

#endif

enum class order
{
    BIG,
    LITTLE,
    NATIVE = VX_ENDIAN_NATIVE_ORDER_INITIALIZER
};

#undef VX_ENDIAN_NATIVE_ORDER_INITIALIZER

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
template<typename T>
VX_FORCE_INLINE constexpr T convert(T x, order from, order to) noexcept
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
template<typename T>
VX_FORCE_INLINE constexpr T big_to_native(T value) noexcept
{
    return convert(value, order::BIG, order::NATIVE);
}

/**
 * @brief Converts a native-endian value to big-endian.
 *
 * @tparam T The type of the value.
 * @param value The value in native byte order.
 * @return The value in big-endian format.
 */
template<typename T>
VX_FORCE_INLINE constexpr T native_to_big(T value) noexcept
{
    return convert(value, order::NATIVE, order::BIG);
}

/**
 * @brief Converts a little-endian value to native endianness.
 *
 * @tparam T The type of the value.
 * @param value The value in little-endian format.
 * @return The value in native byte order.
 */
template<typename T>
VX_FORCE_INLINE constexpr T little_to_native(T value) noexcept
{
    return convert(value, order::LITTLE, order::NATIVE);
}

/**
 * @brief Converts a native-endian value to little-endian.
 *
 * @tparam T The type of the value.
 * @param value The value in native byte order.
 * @return The value in little-endian format.
 */
template<typename T>
VX_FORCE_INLINE constexpr T native_to_little(T value) noexcept
{
    return convert(value, order::NATIVE, order::LITTLE);
}

} // namespace endian

} // namespace vx