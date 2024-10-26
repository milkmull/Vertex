#pragma once

#include <cstdint>

#include "vertex/core/compiler.hpp"

namespace vx {
namespace endian {

///////////////////////////////////////////////////////////////////////////////
// Endianness
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

///////////////////////////////////////////////////////////////////////////////
// Byte Swap
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE constexpr uint8_t swap(uint8_t x) noexcept
{
    return x;
}

VX_FORCE_INLINE constexpr uint16_t swap(uint16_t x) noexcept
{
    return (x >> 8) | (x << 8);
}

VX_FORCE_INLINE constexpr uint32_t swap(uint32_t x) noexcept
{
    return ((x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24));
}

VX_FORCE_INLINE constexpr uint64_t swap(uint64_t x) noexcept
{
    x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
    x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
    x = (x & 0x00FF00FF00FF00FF) <<  8 | (x & 0xFF00FF00FF00FF00) >>  8;
    return x;
}

///////////////////////////////////////////////////////////////////////////////
// General Endianness Conversion
///////////////////////////////////////////////////////////////////////////////

template<typename T>
VX_FORCE_INLINE constexpr T convert(T x, order from, order to) noexcept
{
    return (from == to) ? x : swap(x);
}

///////////////////////////////////////////////////////////////////////////////
// Big/Little To/From Native Functions
///////////////////////////////////////////////////////////////////////////////

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

}
}