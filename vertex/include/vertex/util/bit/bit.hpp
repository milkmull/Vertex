#pragma once

#include <type_traits>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace bit {

namespace __detail {

template <typename T>
struct is_unsigned_integral : std::bool_constant<std::is_integral<T>::value && std::is_unsigned<T>::value> {};

} // namespace __detail

///////////////////////////////////////////////////////////////////////////////
// byteswap
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE uint8_t byteswap(uint8_t x) noexcept
{
    return x;
}

VX_FORCE_INLINE uint16_t byteswap(uint16_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_bswap16)

    return __builtin_bswap16(x);

#elif defined(_MSC_VER)

    return _byteswap_ushort(x);

#else

    return (x << 8) | (x >> 8);

#endif
}

VX_FORCE_INLINE uint32_t byteswap(uint32_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_bswap32)

    return __builtin_bswap32(x);

#elif defined(_MSC_VER)

    return _byteswap_ulong(x);

#else

    return ((x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24));

#endif
}

VX_FORCE_INLINE uint64_t byteswap(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_bswap64)

    return __builtin_bswap64(x);

#elif defined(_MSC_VER)

    return _byteswap_uint64(x);

#else

    x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
    x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
    x = (x & 0x00FF00FF00FF00FF) << 8 | (x & 0xFF00FF00FF00FF00) >> 8;
    return x;

#endif
}

///////////////////////////////////////////////////////////////////////////////
// rotl
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE T rotl(T x, unsigned int shift) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");

    constexpr auto mask = (CHAR_BIT * sizeof(T) - 1);
    shift &= mask;
    return (x << shift) | (x >> ((-shift) & mask));
}

VX_FORCE_INLINE uint8_t rotl(uint8_t x, unsigned int shift) noexcept
{
#if defined(_MSC_VER)

    return _rotl8(x, static_cast<unsigned char>(shift));

#else

    return rotl<uint8_t>(x, shift);

#endif
}

VX_FORCE_INLINE uint16_t rotl(uint16_t x, unsigned int shift) noexcept
{
#if defined(_MSC_VER)

    return _rotl16(x, static_cast<unsigned char>(shift));

#else

    return rotl<uint16_t>(x, shift);

#endif
}

VX_FORCE_INLINE uint32_t rotl(uint32_t x, unsigned int shift) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateleft32)

    return __builtin_rotateleft32(x, shift);

#elif defined(_MSC_VER)

    return _rotl(x, static_cast<int>(shift));

#else

    return rotl<uint32_t>(x, shift);

#endif
}

VX_FORCE_INLINE uint64_t rotl(uint64_t x, unsigned int shift) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateleft64)

    return __builtin_rotateleft64(x, shift);

#elif defined(_MSC_VER)

    return _rotl64(x, static_cast<int>(shift));

#else

    return rotl<uint64_t>(x, shift);

#endif
}

///////////////////////////////////////////////////////////////////////////////
// rotr
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE T rotr(T x, unsigned int shift) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");

    constexpr auto mask = std::numeric_limits<T>::digits - 1;
    shift &= mask;
    return (x >> shift) | (x << ((-shift) & mask));
}

VX_FORCE_INLINE uint8_t rotr(uint8_t x, unsigned int shift) noexcept
{
#if defined(_MSC_VER)

    return _rotr8(x, static_cast<unsigned char>(shift));

#else

    return rotr<uint8_t>(x, shift);

#endif
}

VX_FORCE_INLINE uint16_t rotr(uint16_t x, unsigned int shift) noexcept
{
#if defined(_MSC_VER)

    return _rotr16(x, static_cast<unsigned char>(shift));

#else

    return rotr<uint16_t>(x, shift);

#endif
}

VX_FORCE_INLINE uint32_t rotr(uint32_t x, unsigned int shift) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateright32)

    return __builtin_rotateright32(x, shift);

#elif defined(_MSC_VER)

    return _rotr(x, static_cast<int>(shift));

#else

    return rotr<uint32_t>(x, shift);

#endif
}

VX_FORCE_INLINE uint64_t rotr(uint64_t x, unsigned int shift) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateright64)

    return __builtin_rotateright64(x, shift);

#elif defined(_MSC_VER)

    return _rotr64(x, static_cast<int>(shift));

#else

    return rotr<uint64_t>(x, shift);

#endif
}

///////////////////////////////////////////////////////////////////////////////
// countl_zero
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE int countl_zero(T x) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");
    constexpr int bits = static_cast<int>(std::numeric_limits<T>::digits);

    if (x == 0)
    {
        return bits;
    }

    constexpr T mask = static_cast<T>(1) << (bits - 1);

    int n = 0;
    while ((x & mask) == 0)
    {
        ++n;
        x <<= 1;
    }

    return n;
}

VX_FORCE_INLINE int countl_zero(uint32_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_clz)

    return __builtin_clz(x);

#elif defined(_MSC_VER)

    unsigned long index{};
    if (_BitScanReverse(&index, x))
    {
        return 31 - index;
    }

    return 32;

#else

    return countl_zero<uint32_t>(x);

#endif
}

VX_FORCE_INLINE int countl_zero(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_clzll)

    return __builtin_clzll(x);

#elif defined(_MSC_VER)

    unsigned long index{};
    if (_BitScanReverse64(&index, x))
    {
        return 63 - index;
    }

    return 64;

#else

    return countl_zero<uint64_t>(x);

#endif
}

///////////////////////////////////////////////////////////////////////////////
// countl_one
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE int countl_one(T x) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");

    return countl_zero(static_cast<T>(~x));
}

///////////////////////////////////////////////////////////////////////////////
// countr_zero
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE int countr_zero(T x) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");
    constexpr int bits = static_cast<int>(std::numeric_limits<T>::digits);

    if (x == 0)
    {
        return bits;
    }

    int n = 0;
    while ((x & 0x01) == 0)
    {
        ++n;
        x >>= 1;
    }

    return n;
}

VX_FORCE_INLINE int countr_zero(uint32_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_ctz)

    return __builtin_ctz(x);

#elif defined(_MSC_VER)

    unsigned long index{};
    if (_BitScanForward(&index, x))
    {
        return index;
    }

    return 32;

#else

    return countr_zero<uint32_t>(x);

#endif
}

VX_FORCE_INLINE int countr_zero(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_ctzll)

    return __builtin_ctzll(x);

#elif defined(_MSC_VER)

    unsigned long index{};
    if (_BitScanForward64(&index, x))
    {
        return index;
    }

    return 64;

#else

    return countr_zero<uint64_t>(x);

#endif
}

///////////////////////////////////////////////////////////////////////////////
// countr_one
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE int countr_one(T x) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");

    return countr_zero(static_cast<T>(~x));
}

///////////////////////////////////////////////////////////////////////////////
// popcount
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE int popcount(T x) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");

    int n = 0;

    while (x)
    {
        n += x & 0x01;
        x >>= 1;
    }

    return n;
}

VX_FORCE_INLINE int popcount(uint32_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_popcount)

    return __builtin_popcount(x);

#elif defined(_MSC_VER)

    return __popcnt(x);

#else

    return popcount<uint32_t>(x);

#endif
}

VX_FORCE_INLINE int popcount(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_popcountll)

    return __builtin_popcountll(x);

#elif defined(_MSC_VER)

    return static_cast<int>(__popcnt64(x));

#else

    return popcount<uint64_t>(x);

#endif
}

///////////////////////////////////////////////////////////////////////////////
// has_single_bit
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE bool has_single_bit(T x) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");

    return (popcount(x) == 1);
}

///////////////////////////////////////////////////////////////////////////////
// bit_width
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE int bit_width(T x) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");

    return static_cast<int>(std::numeric_limits<T>::digits - countl_zero(x));
}

///////////////////////////////////////////////////////////////////////////////
// bit_ceil
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE T bit_ceil(T x) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");

    if (x <= static_cast<T>(1))
    {
        return static_cast<T>(1);
    }

    if ((x & (x - 1)) == static_cast<T>(0))
    {
        return x;
    }

    return static_cast<T>(1) << bit_width(static_cast<T>(x - 1));
}

///////////////////////////////////////////////////////////////////////////////
// bit_floor
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE T bit_floor(T x) noexcept
{
    static_assert(__detail::is_unsigned_integral<T>::value, "T must be unsigned integral type");

    if (x == static_cast<T>(0))
    {
        return static_cast<T>(0);
    }

    return static_cast<T>(1) << bit_width(static_cast<T>(x >> 1));
}

} // namespace bit
} // namespace vx