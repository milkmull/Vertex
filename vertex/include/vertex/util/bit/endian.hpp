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

template<typename T>
VX_FORCE_INLINE constexpr T convert(T x, order from, order to) noexcept
{
    return (from == to) ? x : bit::byteswap(x);
}

template<typename T>
VX_FORCE_INLINE constexpr T big_to_native(T value) noexcept
{
    return convert(value, order::BIG, order::NATIVE);
}

template<typename T>
VX_FORCE_INLINE constexpr T native_to_big(T value) noexcept
{
    return convert(value, order::NATIVE, order::BIG);
}

template<typename T>
VX_FORCE_INLINE constexpr T little_to_native(T value) noexcept
{
    return convert(value, order::LITTLE, order::NATIVE);
}

template<typename T>
VX_FORCE_INLINE constexpr T native_to_little(T value) noexcept
{
    return convert(value, order::NATIVE, order::LITTLE);
}

} // namespace endian

} // namespace vx