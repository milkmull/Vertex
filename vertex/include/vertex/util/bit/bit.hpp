#pragma once

#include <climits> // CHAR_BIT
#include <limits> // numeric_limits

#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"

namespace vx {
namespace bit {

///////////////////////////////////////////////////////////////////////////////
// byteswap
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Byte-swaps a 1-byte (8-bit) unsigned integer.
 *
 * No actual byte swapping is performed since the value is a single byte.
 *
 * @param x The 8-bit unsigned integer to convert.
 * @return The input value, unchanged.
 */
VX_FORCE_INLINE uint8_t byteswap(uint8_t x) noexcept
{
    return x;
}

/**
 * @brief Byte-swaps a 2-byte (16-bit) unsigned integer.
 *
 * Converts between little-endian and big-endian representations.
 * Uses compiler intrinsics if available for optimal performance.
 *
 * @param x The 16-bit unsigned integer to convert.
 * @return The byte-swapped result.
 */
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

/**
 * @brief Byte-swaps a 4-byte (32-bit) unsigned integer.
 *
 * Converts between little-endian and big-endian formats.
 * Utilizes compiler intrinsics when available, falling back to a manual implementation otherwise.
 *
 * @param x The 32-bit unsigned integer to convert.
 * @return The byte-swapped result.
 */
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

/**
 * @brief Byte-swaps an 8-byte (64-bit) unsigned integer.
 *
 * Efficiently reverses the byte order of a 64-bit value.
 * Uses platform intrinsics where supported.
 *
 * @param x The 64-bit unsigned integer to convert.
 * @return The byte-swapped result.
 */
VX_FORCE_INLINE uint64_t byteswap(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_bswap64)

    return __builtin_bswap64(x);

#elif defined(_MSC_VER)

    return _byteswap_uint64(x);

#else

    x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
    x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
    x = (x & 0x00FF00FF00FF00FF) <<  8 | (x & 0xFF00FF00FF00FF00) >>  8;
    return x;

#endif
}

///////////////////////////////////////////////////////////////////////////////
// rotl
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Performs a left bitwise rotation on an unsigned integral value.
 *
 * This is a generic fallback implementation, used when no platform-specific
 * intrinsics are available.
 *
 * @tparam T An unsigned integral type.
 * @param x The value to rotate.
 * @param shift Number of bits to rotate left by.
 * @return The result of left-rotating `x` by `shift` bits.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value&& std::is_unsigned<T>::value)>
VX_FORCE_INLINE T rotl(T x, unsigned int shift) noexcept
{
    constexpr auto mask = (CHAR_BIT * sizeof(T) - 1);
    shift &= mask;
    return static_cast<T>((x << shift) | (x >> ((-shift) & mask)));
}

/**
 * @brief Performs a left bitwise rotation on an 8-bit unsigned integer.
 *
 * Uses `_rotl8` on MSVC; otherwise falls back to the generic implementation.
 *
 * @param x The 8-bit value to rotate.
 * @param shift Number of bits to rotate left by.
 * @return The rotated result.
 */
VX_FORCE_INLINE uint8_t rotl(uint8_t x, unsigned int shift) noexcept
{
#if defined(_MSC_VER)

    return _rotl8(x, static_cast<unsigned char>(shift));

#else

    return rotl<uint8_t>(x, shift);

#endif
}

/**
 * @brief Performs a left bitwise rotation on a 16-bit unsigned integer.
 *
 * Uses `_rotl16` on MSVC; otherwise uses the generic template.
 *
 * @param x The 16-bit value to rotate.
 * @param shift Number of bits to rotate left by.
 * @return The rotated result.
 */
VX_FORCE_INLINE uint16_t rotl(uint16_t x, unsigned int shift) noexcept
{
#if defined(_MSC_VER)

    return _rotl16(x, static_cast<unsigned char>(shift));

#else

    return rotl<uint16_t>(x, shift);

#endif
}

/**
 * @brief Performs a left bitwise rotation on a 32-bit unsigned integer.
 *
 * Uses compiler intrinsics if available (`__builtin_rotateleft32` or `_rotl`),
 * otherwise falls back to the generic implementation.
 *
 * @param x The 32-bit value to rotate.
 * @param shift Number of bits to rotate left by.
 * @return The rotated result.
 */
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

/**
 * @brief Performs a left bitwise rotation on a 64-bit unsigned integer.
 *
 * Uses compiler intrinsics (`__builtin_rotateleft64` or `_rotl64`) if available,
 * otherwise falls back to the generic version.
 *
 * @param x The 64-bit value to rotate.
 * @param shift Number of bits to rotate left by.
 * @return The rotated result.
 */
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

/**
 * @brief Performs a right bitwise rotation on an unsigned integral value.
 *
 * Generic fallback implementation used when intrinsics are not available.
 *
 * @tparam T An unsigned integral type.
 * @param x The value to rotate.
 * @param shift Number of bits to rotate right by.
 * @return The result of right-rotating `x` by `shift` bits.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value && std::is_unsigned<T>::value)>
VX_FORCE_INLINE T rotr(T x, unsigned int shift) noexcept
{
    constexpr auto mask = std::numeric_limits<T>::digits - 1;
    shift &= mask;
    return static_cast<T>((x >> shift) | (x << ((-shift) & mask)));
}

/**
 * @brief Performs a right bitwise rotation on an 8-bit unsigned integer.
 *
 * Uses `_rotr8` on MSVC; otherwise falls back to the generic implementation.
 *
 * @param x The 8-bit value to rotate.
 * @param shift Number of bits to rotate right by.
 * @return The rotated result.
 */
VX_FORCE_INLINE uint8_t rotr(uint8_t x, unsigned int shift) noexcept
{
#if defined(_MSC_VER)

    return _rotr8(x, static_cast<unsigned char>(shift));

#else

    return rotr<uint8_t>(x, shift);

#endif
}

/**
 * @brief Performs a right bitwise rotation on a 16-bit unsigned integer.
 *
 * Uses `_rotr16` on MSVC; otherwise uses the generic template.
 *
 * @param x The 16-bit value to rotate.
 * @param shift Number of bits to rotate right by.
 * @return The rotated result.
 */
VX_FORCE_INLINE uint16_t rotr(uint16_t x, unsigned int shift) noexcept
{
#if defined(_MSC_VER)

    return _rotr16(x, static_cast<unsigned char>(shift));

#else

    return rotr<uint16_t>(x, shift);

#endif
}

/**
 * @brief Performs a right bitwise rotation on a 32-bit unsigned integer.
 *
 * Uses `__builtin_rotateright32` or `_rotr` if available, or falls back to the generic version.
 *
 * @param x The 32-bit value to rotate.
 * @param shift Number of bits to rotate right by.
 * @return The rotated result.
 */
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

/**
 * @brief Performs a right bitwise rotation on a 64-bit unsigned integer.
 *
 * Uses `__builtin_rotateright64` or `_rotr64` when available.
 *
 * @param x The 64-bit value to rotate.
 * @param shift Number of bits to rotate right by.
 * @return The rotated result.
 */
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

/**
 * @brief Counts the number of leading zero bits in an unsigned integer.
 *
 * This generic fallback implementation performs a manual bit scan
 * from the most significant bit (MSB) toward the least significant bit (LSB).
 *
 * @tparam T An unsigned integral type.
 * @param x The value to count leading zeros in.
 * @return The number of consecutive zero bits starting from the MSB.
 * Returns `std::numeric_limits<T>::digits` if `x == 0`.
 *
 * @note This version is selected if no platform-specific intrinsic is available.
 * @note Requires T to be an unsigned integral type.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value && std::is_unsigned<T>::value)>
VX_FORCE_INLINE int countl_zero(T x) noexcept
{
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

/**
 * @brief Counts the number of leading zero bits in a 32-bit unsigned integer.
 *
 * Uses platform intrinsics when available:
 * - GCC/Clang: `__builtin_clz`
 * - MSVC: `_BitScanReverse`
 * Falls back to generic implementation otherwise.
 *
 * @param x 32-bit unsigned integer.
 * @return The number of leading zeros. Returns 32 if `x == 0`.
 */
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

/**
 * @brief Counts the number of leading zero bits in a 64-bit unsigned integer.
 *
 * Uses platform intrinsics when available:
 * - GCC/Clang: `__builtin_clzll`
 * - MSVC: `_BitScanReverse64`
 * Falls back to generic implementation otherwise.
 *
 * @param x 64-bit unsigned integer.
 * @return The number of leading zeros. Returns 64 if `x == 0`.
 */
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

/**
 * @brief Counts the number of consecutive 1-bits starting from the most significant bit (MSB).
 *
 * Computes the number of leading one bits by inverting the bits of `x` and applying `countl_zero`.
 *
 * @tparam T An unsigned integral type.
 * @param x The value to analyze.
 * @return The number of consecutive one bits starting from the MSB.
 * Returns `std::numeric_limits<T>::digits` if `x == ~T(0)` (i.e., all bits are 1).
 *
 * @note Requires T to be an unsigned integral type.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value && std::is_unsigned<T>::value)>
VX_FORCE_INLINE int countl_one(T x) noexcept
{
    return countl_zero(static_cast<T>(~x));
}

///////////////////////////////////////////////////////////////////////////////
// countr_zero
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Counts the number of consecutive 0-bits starting from the least significant bit (LSB).
 *
 * Computes the number of trailing zeros in the binary representation of `x`.
 *
 * @tparam T An unsigned integral type.
 * @param x The value to analyze.
 * @return The number of trailing zero bits.
 *         Returns `std::numeric_limits<T>::digits` if `x == 0`.
 *
 * @note Requires T to be an unsigned integral type.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value && std::is_unsigned<T>::value)>
VX_FORCE_INLINE int countr_zero(T x) noexcept
{
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

/**
 * @brief Counts the number of consecutive 0-bits starting from the least significant bit (LSB) in a 32-bit unsigned integer.
 *
 * This overload uses platform-specific intrinsics for optimal performance:
 * - On compilers with `__builtin_ctz`, this intrinsic is used.
 * - On MSVC, `_BitScanForward` is used.
 * - If neither is available, it falls back to a portable implementation.
 *
 * @param x The 32-bit unsigned integer value.
 * @return The number of trailing zeros in the binary representation of `x`.
 *         Returns 32 if `x == 0`.
 */
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

/**
 * @brief Counts the number of consecutive 0-bits starting from the least significant bit (LSB) in a 64-bit unsigned integer.
 *
 * This overload uses platform-specific intrinsics for optimal performance:
 * - On compilers with `__builtin_ctzll`, this intrinsic is used.
 * - On MSVC, `_BitScanForward64` is used.
 * - If neither is available, it falls back to a portable implementation.
 *
 * @param x The 64-bit unsigned integer value.
 * @return The number of trailing zeros in the binary representation of `x`.
 *         Returns 64 if `x == 0`.
 */
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

/**
 * @brief Counts the number of consecutive 1-bits starting from the least significant bit (LSB).
 *
 * This is implemented by inverting the input and passing it to `countr_zero`.
 * For example, `0b00011111` has 5 trailing ones.
 *
 * @tparam T An unsigned integral type.
 * @param x The value to analyze.
 * @return The number of trailing 1s in the binary representation of `x`.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value && std::is_unsigned<T>::value)>
VX_FORCE_INLINE int countr_one(T x) noexcept
{
    return countr_zero(static_cast<T>(~x));
}

///////////////////////////////////////////////////////////////////////////////
// popcount
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Counts the number of 1-bits (population count) in the binary representation of a value.
 *
 * This generic version uses a simple loop and bitmask to count bits.
 * Platform-specific intrinsics are used in specialized overloads for optimal performance.
 *
 * @tparam T An unsigned integral type.
 * @param x The value to analyze.
 * @return The number of 1s in the binary representation of `x`.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value && std::is_unsigned<T>::value)>
VX_FORCE_INLINE int popcount(T x) noexcept
{
    int n = 0;

    while (x)
    {
        n += x & 0x01;
        x >>= 1;
    }

    return n;
}

/**
 * @brief Counts the number of 1-bits in a 32-bit unsigned integer.
 *
 * Uses compiler intrinsics where available:
 * - GCC/Clang: `__builtin_popcount`
 * - MSVC: `__popcnt`
 * Falls back to the generic `popcount<uint32_t>` otherwise.
 *
 * @param x The 32-bit unsigned integer.
 * @return The number of set bits in `x`.
 */
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

/**
 * @brief Counts the number of 1-bits in a 64-bit unsigned integer.
 *
 * Uses compiler intrinsics where available:
 * - GCC/Clang: `__builtin_popcountll`
 * - MSVC: `__popcnt64`
 * Falls back to the generic `popcount<uint64_t>` otherwise.
 *
 * @param x The 64-bit unsigned integer.
 * @return The number of set bits in `x`.
 */
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

/**
 * @brief Checks whether the value has exactly one bit set (i.e., is a power of two).
 *
 * Equivalent to: `(x != 0) && ((x & (x - 1)) == 0)`
 * This implementation uses `popcount(x) == 1` for clarity and consistency.
 *
 * @tparam T An unsigned integral type.
 * @param x The value to check.
 * @return True if `x` has a single bit set; otherwise false.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value && std::is_unsigned<T>::value)>
VX_FORCE_INLINE bool has_single_bit(T x) noexcept
{
    return (popcount(x) == 1);
}

///////////////////////////////////////////////////////////////////////////////
// bit_width
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Computes the number of bits needed to represent a non-zero value in binary.
 *
 * For example:
 * - `bit_width(1)` returns 1
 * - `bit_width(8)` returns 4
 * - `bit_width(0)` returns 0
 *
 * Internally, this is defined as: `digits - countl_zero(x)`.
 *
 * @tparam T An unsigned integral type.
 * @param x The value whose bit width is to be computed.
 * @return The minimum number of bits required to represent `x`.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value && std::is_unsigned<T>::value)>
VX_FORCE_INLINE int bit_width(T x) noexcept
{
    return static_cast<int>(std::numeric_limits<T>::digits - countl_zero(x));
}

///////////////////////////////////////////////////////////////////////////////
// bit_ceil
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Returns the smallest power of two that is greater than or equal to the given value.
 *
 * If `x <= 1`, the result is 1.
 * If `x` is already a power of two, it is returned as-is.
 * Otherwise, it returns `1 << bit_width(x - 1)`.
 *
 * @tparam T An unsigned integral type.
 * @param x The input value.
 * @return The smallest power of two >= `x`.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value && std::is_unsigned<T>::value)>
VX_FORCE_INLINE T bit_ceil(T x) noexcept
{
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

/**
 * @brief Returns the largest power of two less than or equal to the given value.
 *
 * If `x == 0`, the result is 0.
 * Otherwise, it returns `1 << bit_width(x >> 1)`.
 *
 * @tparam T An unsigned integral type.
 * @param x The input value.
 * @return The largest power of two <= `x`.
 */
template <typename T, VX_REQUIRES(std::is_integral<T>::value && std::is_unsigned<T>::value)>
VX_FORCE_INLINE T bit_floor(T x) noexcept
{
    if (x == static_cast<T>(0))
    {
        return static_cast<T>(0);
    }

    return static_cast<T>(1) << bit_width(static_cast<T>(x >> 1));
}

} // namespace bit
} // namespace vx