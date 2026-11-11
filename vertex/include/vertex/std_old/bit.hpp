#pragma once

#include <climits> // CHAR_BIT
#include <intrin.h>
#include <limits> // numeric_limits

#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/memory.hpp"

namespace vx {
namespace bit {

namespace _bit_priv {

template <typename T>
struct is_bit_unsigned : std::integral_constant<bool, std::is_integral<T>::value && std::is_unsigned<T>::value && !std::is_same<T, bool>::value>
{};

} // namespace _bit_priv

//=============================================================================
// byteswap
//=============================================================================

namespace _bit_priv {

constexpr uint16_t byteswap_16_generic(uint16_t x) noexcept
{
    return static_cast<uint16_t>((x << 8) | (x >> 8));
}

constexpr uint32_t byteswap_32_generic(uint32_t x) noexcept
{
    return ((x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24));
}

constexpr uint64_t byteswap_64_generic(uint64_t x) noexcept
{
    x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
    x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
    x = (x & 0x00FF00FF00FF00FF) << 8 | (x & 0xFF00FF00FF00FF00) >> 8;
    return x;
}

constexpr uint8_t byteswap_8_impl(uint8_t x) noexcept
{
    return x;
}

constexpr uint16_t byteswap_16_impl(uint16_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_bswap16)

    return __builtin_bswap16(x);

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return byteswap_16_generic(x);
    }
    return _byteswap_ushort(x);

#else

    return byteswap_16_generic(x);

#endif
}

constexpr uint32_t byteswap_32_impl(uint32_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_bswap32)

    return __builtin_bswap32(x);

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return byteswap_32_generic(x);
    }
    return static_cast<uint32_t>(_byteswap_ulong(x));

#else

    return byteswap_32_generic(x);

#endif
}

constexpr uint64_t byteswap_64_impl(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_bswap64)

    return __builtin_bswap64(x);

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return byteswap_64_generic(x);
    }
    return _byteswap_uint64(x);

#else

    return byteswap_64_generic(x);

#endif
}

template <typename T>
constexpr T byteswap_impl(T x) noexcept
{
    VX_STATIC_ASSERT_MSG(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
        "Unsupported type size for byteswap");

    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return static_cast<T>(byteswap_8_impl(static_cast<uint8_t>(x)));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        return static_cast<T>(byteswap_16_impl(static_cast<uint16_t>(x)));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return static_cast<T>(byteswap_32_impl(static_cast<uint32_t>(x)));
    }
    else // VX_IF_CONSTEXPR (sizeof(T) == 8)
    {
        return static_cast<T>(byteswap_64_impl(static_cast<uint64_t>(x)));
    }
}

} // namespace _bit_priv

/**
 * @brief Reverses the byte order of an integer (like C++23 std::byteswap).
 *
 * Accepts any integral type except bool, signed or unsigned, of size 1, 2, 4 or 8.
 * Uses compiler/platform intrinsics at run time and a portable constexpr
 * implementation during constant evaluation.
 *
 * @tparam T An integral type (not bool).
 * @param x The value to byte-swap.
 * @return The value with its bytes in reverse order.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr T byteswap(T x) noexcept
{
    using U = typename std::make_unsigned<T>::type;
    return static_cast<T>(_bit_priv::byteswap_impl(static_cast<U>(x)));
}

//=============================================================================
// rotl
//=============================================================================

namespace _bit_priv {

// `r` must already be reduced to [0, digits - 1].
template <typename T>
constexpr T rotl_generic(T x, unsigned int r) noexcept
{
    constexpr unsigned int mask = static_cast<unsigned int>(std::numeric_limits<T>::digits) - 1u;
    return static_cast<T>((x << r) | (x >> ((0u - r) & mask)));
}

constexpr uint8_t rotl_8_impl(uint8_t x, unsigned int r) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateleft8)

    return __builtin_rotateleft8(x, static_cast<uint8_t>(r));

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return rotl_generic<uint8_t>(x, r);
    }
    return _rotl8(x, static_cast<unsigned char>(r));

#else

    return rotl_generic<uint8_t>(x, r);

#endif
}

constexpr uint16_t rotl_16_impl(uint16_t x, unsigned int r) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateleft16)

    return __builtin_rotateleft16(x, static_cast<uint16_t>(r));

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return rotl_generic<uint16_t>(x, r);
    }
    return _rotl16(x, static_cast<unsigned char>(r));

#else

    return rotl_generic<uint16_t>(x, r);

#endif
}

constexpr uint32_t rotl_32_impl(uint32_t x, unsigned int r) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateleft32)

    return __builtin_rotateleft32(x, r);

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return rotl_generic<uint32_t>(x, r);
    }
    return static_cast<uint32_t>(_rotl(x, static_cast<int>(r)));

#else

    return rotl_generic<uint32_t>(x, r);

#endif
}

constexpr uint64_t rotl_64_impl(uint64_t x, unsigned int r) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateleft64)

    return __builtin_rotateleft64(x, r);

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return rotl_generic<uint64_t>(x, r);
    }
    return _rotl64(x, static_cast<int>(r));

#else

    return rotl_generic<uint64_t>(x, r);

#endif
}

template <typename T>
constexpr T rotl_impl(T x, unsigned int r) noexcept
{
    VX_STATIC_ASSERT_MSG(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
        "Unsupported type size for rotl");

    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return static_cast<T>(rotl_8_impl(static_cast<uint8_t>(x), r));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        return static_cast<T>(rotl_16_impl(static_cast<uint16_t>(x), r));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return static_cast<T>(rotl_32_impl(static_cast<uint32_t>(x), r));
    }
    else
    {
        return static_cast<T>(rotl_64_impl(static_cast<uint64_t>(x), r));
    }
}

} // namespace _bit_priv

/**
 * @brief Rotates the bits of an unsigned integer to the left (like C++20 std::rotl).
 *
 * The shift is taken modulo the bit width, so any `int` is valid.
 * A negative shift rotates to the right.
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The value to rotate.
 * @param shift Number of bits to rotate left by.
 * @return The rotated value.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr T rotl(T x, int shift) noexcept
{
    constexpr unsigned int mask = static_cast<unsigned int>(std::numeric_limits<T>::digits) - 1u;
    return _bit_priv::rotl_impl<T>(x, static_cast<unsigned int>(shift) & mask);
}

//=============================================================================
// rotl_assume_valid_shift
//=============================================================================

/**
 * @brief rotl without the shift reduction.
 * @pre `0 <= shift < std::numeric_limits<T>::digits`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr T rotl_assume_valid_shift(T x, unsigned int shift) noexcept
{
    VX_ASSERT(shift < static_cast<unsigned int>(std::numeric_limits<T>::digits));
    return _bit_priv::rotl_impl<T>(x, shift);
}

//=============================================================================
// rotr
//=============================================================================

namespace _bit_priv {

template <typename T>
constexpr T rotr_generic(T x, unsigned int r) noexcept
{
    constexpr unsigned int mask = static_cast<unsigned int>(std::numeric_limits<T>::digits) - 1u;
    return static_cast<T>((x >> r) | (x << ((0u - r) & mask)));
}

constexpr uint8_t rotr_8_impl(uint8_t x, unsigned int r) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateright8)

    return __builtin_rotateright8(x, static_cast<uint8_t>(r));

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return rotr_generic<uint8_t>(x, r);
    }
    return _rotr8(x, static_cast<unsigned char>(r));

#else

    return rotr_generic<uint8_t>(x, r);

#endif
}

constexpr uint16_t rotr_16_impl(uint16_t x, unsigned int r) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateright16)

    return __builtin_rotateright16(x, static_cast<uint16_t>(r));

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return rotr_generic<uint16_t>(x, r);
    }
    return _rotr16(x, static_cast<unsigned char>(r));

#else

    return rotr_generic<uint16_t>(x, r);

#endif
}

constexpr uint32_t rotr_32_impl(uint32_t x, unsigned int r) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateright32)

    return __builtin_rotateright32(x, r);

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return rotr_generic<uint32_t>(x, r);
    }
    return static_cast<uint32_t>(_rotr(x, static_cast<int>(r)));

#else

    return rotr_generic<uint32_t>(x, r);

#endif
}

constexpr uint64_t rotr_64_impl(uint64_t x, unsigned int r) noexcept
{
#if VX_HAS_BUILTIN(__builtin_rotateright64)

    return __builtin_rotateright64(x, r);

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return rotr_generic<uint64_t>(x, r);
    }
    return _rotr64(x, static_cast<int>(r));

#else

    return rotr_generic<uint64_t>(x, r);

#endif
}

template <typename T>
constexpr T rotr_impl(T x, unsigned int r) noexcept
{
    VX_STATIC_ASSERT_MSG(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
        "Unsupported type size for rotr");

    VX_IF_CONSTEXPR (sizeof(T) == 1)
    {
        return static_cast<T>(rotr_8_impl(static_cast<uint8_t>(x), r));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 2)
    {
        return static_cast<T>(rotr_16_impl(static_cast<uint16_t>(x), r));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return static_cast<T>(rotr_32_impl(static_cast<uint32_t>(x), r));
    }
    else
    {
        return static_cast<T>(rotr_64_impl(static_cast<uint64_t>(x), r));
    }
}

} // namespace _bit_priv

/**
 * @brief Rotates the bits of an unsigned integer to the right (like C++20 std::rotr).
 *
 * The shift is taken modulo the bit width, so any `int` is valid.
 * A negative shift rotates to the left.
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The value to rotate.
 * @param shift Number of bits to rotate right by.
 * @return The rotated value.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr T rotr(T x, int shift) noexcept
{
    constexpr unsigned int mask = static_cast<unsigned int>(std::numeric_limits<T>::digits) - 1u;
    return _bit_priv::rotr_impl<T>(x, static_cast<unsigned int>(shift) & mask);
}

//=============================================================================
// rotr_assume_valid_shift
//=============================================================================

/**
 * @brief rotr without the shift reduction.
 * @pre `0 <= shift < std::numeric_limits<T>::digits`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr T rotr_assume_valid_shift(T x, unsigned int shift) noexcept
{
    VX_ASSERT(shift < static_cast<unsigned int>(std::numeric_limits<T>::digits));
    return _bit_priv::rotr_impl<T>(x, shift);
}

//=============================================================================
// countl_zero
//=============================================================================

namespace _bit_priv {

// Log-step scan: 5 iterations for 32-bit, 6 for 64-bit.
template <typename T>
constexpr int countl_zero_generic(T x) noexcept
{
    constexpr int digits = std::numeric_limits<T>::digits;

    if (x == 0)
    {
        return digits;
    }

    int n = 0;
    for (int s = digits / 2; s != 0; s /= 2)
    {
        if ((x >> (digits - s)) == 0)
        {
            n += s;
            x = static_cast<T>(x << s);
        }
    }

    return n;
}

constexpr int countl_zero_32_impl(uint32_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_clz)

    // __builtin_clz(0) is undefined. Compilers fold the branch away
    // when the target has LZCNT / ARM clz.
    return x ? __builtin_clz(static_cast<unsigned int>(x)) : 32;

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return countl_zero_generic<uint32_t>(x);
    }

    unsigned long index{};
    return _BitScanReverse(&index, x) ? (31 - static_cast<int>(index)) : 32;

#else

    return countl_zero_generic<uint32_t>(x);

#endif
}

constexpr int countl_zero_64_impl(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_clzll)

    return x ? __builtin_clzll(static_cast<unsigned long long>(x)) : 64;

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return countl_zero_generic<uint64_t>(x);
    }

    #if defined(_M_X64) || defined(_M_ARM64) || defined(_M_ARM64EC)

    unsigned long index{};
    return _BitScanReverse64(&index, x) ? (63 - static_cast<int>(index)) : 64;

    #else // 32-bit MSVC: no 64-bit scan intrinsic

    const uint32_t hi = static_cast<uint32_t>(x >> 32);
    return (hi != 0) ? countl_zero_32_impl(hi) : 32 + countl_zero_32_impl(static_cast<uint32_t>(x));

    #endif

#else

    return countl_zero_generic<uint64_t>(x);

#endif
}

template <typename T>
constexpr int countl_zero_impl(T x) noexcept
{
    VX_STATIC_ASSERT_MSG(sizeof(T) <= 8, "Unsupported type size for countl_zero");

    constexpr int digits = std::numeric_limits<T>::digits;

    VX_IF_CONSTEXPR (sizeof(T) <= 4)
    {
        // Zero-extension adds (32 - digits) phantom leading zeros; remove them.
        // x == 0 gives 32 - (32 - digits) == digits.
        return countl_zero_32_impl(static_cast<uint32_t>(x)) - (32 - digits);
    }
    else
    {
        return countl_zero_64_impl(static_cast<uint64_t>(x));
    }
}

} // namespace _bit_priv

/**
 * @brief Counts consecutive 0 bits starting from the most significant bit (like C++20 std::countl_zero).
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The value to analyze.
 * @return The number of leading zero bits; `std::numeric_limits<T>::digits` if `x == 0`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr int countl_zero(T x) noexcept
{
    return _bit_priv::countl_zero_impl<T>(x);
}

//=============================================================================
// countl_zero_assume_nonzero
//=============================================================================

namespace _bit_priv {

constexpr int countl_zero_nz_32_impl(uint32_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_clz)

    return __builtin_clz(static_cast<unsigned int>(x));

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return countl_zero_generic<uint32_t>(x);
    }

    unsigned long index{};
    _BitScanReverse(&index, x); // return value ignored: x != 0
    return 31 - static_cast<int>(index);

#else

    return countl_zero_generic<uint32_t>(x);

#endif
}

constexpr int countl_zero_nz_64_impl(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_clzll)

    return __builtin_clzll(static_cast<unsigned long long>(x));

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return countl_zero_generic<uint64_t>(x);
    }

    #if defined(_M_X64) || defined(_M_ARM64) || defined(_M_ARM64EC)

    unsigned long index{};
    _BitScanReverse64(&index, x);
    return 63 - static_cast<int>(index);

    #else // 32-bit MSVC: the hi != 0 test is inherent to the split

    const uint32_t hi = static_cast<uint32_t>(x >> 32);
    return (hi != 0) ? countl_zero_nz_32_impl(hi) : 32 + countl_zero_nz_32_impl(static_cast<uint32_t>(x));

    #endif

#else

    return countl_zero_generic<uint64_t>(x);

#endif
}

template <typename T>
constexpr int countl_zero_nz_impl(T x) noexcept
{
    VX_STATIC_ASSERT_MSG(sizeof(T) <= 8, "Unsupported type size for countl_zero_assume_nonzero");

    constexpr int digits = std::numeric_limits<T>::digits;

    VX_IF_CONSTEXPR (sizeof(T) <= 4)
    {
        return countl_zero_nz_32_impl(static_cast<uint32_t>(x)) - (32 - digits);
    }
    else
    {
        return countl_zero_nz_64_impl(static_cast<uint64_t>(x));
    }
}

} // namespace _bit_priv

/**
 * @brief countl_zero without the zero check.
 * @pre `x != 0`. Violation is UB (compile error in constant evaluation, assert in debug).
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr int countl_zero_assume_nonzero(T x) noexcept
{
    VX_ASSERT(x != 0);
    return _bit_priv::countl_zero_nz_impl<T>(x);
}

//=============================================================================
// countl_one
//=============================================================================

/**
 * @brief Counts consecutive 1 bits starting from the most significant bit (like C++20 std::countl_one).
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The value to analyze.
 * @return The number of leading one bits; `std::numeric_limits<T>::digits` if all bits are set.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr int countl_one(T x) noexcept
{
    return _bit_priv::countl_zero_impl<T>(static_cast<T>(~x));
}

//=============================================================================
// countr_zero
//=============================================================================

namespace _bit_priv {

template <typename T>
constexpr int countr_zero_generic(T x) noexcept
{
    constexpr int digits = std::numeric_limits<T>::digits;

    if (x == 0)
    {
        return digits;
    }

    int n = 0;
    for (int s = digits / 2; s != 0; s /= 2)
    {
        if ((x & ((static_cast<T>(1) << s) - 1)) == 0)
        {
            n += s;
            x = static_cast<T>(x >> s);
        }
    }
    return n;
}

constexpr int countr_zero_32_impl(uint32_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_ctz)

    return x ? __builtin_ctz(static_cast<unsigned int>(x)) : 32;

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return countr_zero_generic<uint32_t>(x);
    }

    unsigned long index{};
    return _BitScanForward(&index, x) ? static_cast<int>(index) : 32;

#else

    return countr_zero_generic<uint32_t>(x);

#endif
}

constexpr int countr_zero_64_impl(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_ctzll)

    return x ? __builtin_ctzll(static_cast<unsigned long long>(x)) : 64;

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return countr_zero_generic<uint64_t>(x);
    }

    #if defined(_M_X64) || defined(_M_ARM64) || defined(_M_ARM64EC)

    unsigned long index{};
    return _BitScanForward64(&index, x) ? static_cast<int>(index) : 64;

    #else // 32-bit MSVC

    const uint32_t lo = static_cast<uint32_t>(x);
    return (lo != 0) ? countr_zero_32_impl(lo) : 32 + countr_zero_32_impl(static_cast<uint32_t>(x >> 32));

    #endif

#else

    return countr_zero_generic<uint64_t>(x);

#endif
}

template <typename T>
constexpr int countr_zero_impl(T x) noexcept
{
    VX_STATIC_ASSERT_MSG(sizeof(T) <= 8, "Unsupported type size for countr_zero");

    constexpr int digits = std::numeric_limits<T>::digits;

    VX_IF_CONSTEXPR (sizeof(T) < 4)
    {
        // Sentinel bit just above the value: x == 0 yields `digits`,
        // and a zero-check inside the impl folds away.
        return countr_zero_32_impl(static_cast<uint32_t>(x) | (static_cast<uint32_t>(1) << digits));
    }
    else VX_IF_CONSTEXPR (sizeof(T) == 4)
    {
        return countr_zero_32_impl(static_cast<uint32_t>(x));
    }
    else
    {
        return countr_zero_64_impl(static_cast<uint64_t>(x));
    }
}

} // namespace _bit_priv

/**
 * @brief Counts consecutive 0 bits starting from the least significant bit (like C++20 std::countr_zero).
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The value to analyze.
 * @return The number of trailing zero bits; `std::numeric_limits<T>::digits` if `x == 0`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr int countr_zero(T x) noexcept
{
    return _bit_priv::countr_zero_impl<T>(x);
}

//=============================================================================
// countr_zero_assume_nonzero
//=============================================================================

namespace _bit_priv {

constexpr int countr_zero_nz_32_impl(uint32_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_ctz)

    return __builtin_ctz(static_cast<unsigned int>(x));

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return countr_zero_generic<uint32_t>(x);
    }

    unsigned long index{};
    _BitScanForward(&index, x);
    return static_cast<int>(index);

#else

    return countr_zero_generic<uint32_t>(x);

#endif
}

constexpr int countr_zero_nz_64_impl(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_ctzll)

    return __builtin_ctzll(static_cast<unsigned long long>(x));

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return countr_zero_generic<uint64_t>(x);
    }

    #if defined(_M_X64) || defined(_M_ARM64) || defined(_M_ARM64EC)

    unsigned long index{};
    _BitScanForward64(&index, x);
    return static_cast<int>(index);

    #else

    const uint32_t lo = static_cast<uint32_t>(x);
    return (lo != 0) ? countr_zero_nz_32_impl(lo) : 32 + countr_zero_nz_32_impl(static_cast<uint32_t>(x >> 32));

    #endif

#else

    return countr_zero_generic<uint64_t>(x);

#endif
}

template <typename T>
constexpr int countr_zero_nz_impl(T x) noexcept
{
    VX_STATIC_ASSERT_MSG(sizeof(T) <= 8, "Unsupported type size for countr_zero_assume_nonzero");

    VX_IF_CONSTEXPR (sizeof(T) <= 4)
    {
        // No sentinel bit needed: zero is excluded, so widening is exact.
        return countr_zero_nz_32_impl(static_cast<uint32_t>(x));
    }
    else
    {
        return countr_zero_nz_64_impl(static_cast<uint64_t>(x));
    }
}

} // namespace _bit_priv

/**
 * @brief countr_zero without the zero check.
 * @pre `x != 0`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr int countr_zero_assume_nonzero(T x) noexcept
{
    VX_ASSERT(x != 0);
    return _bit_priv::countr_zero_nz_impl<T>(x);
}

//=============================================================================
// countr_one
//=============================================================================

/**
 * @brief Counts consecutive 1 bits starting from the least significant bit (like C++20 std::countr_one).
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The value to analyze.
 * @return The number of trailing one bits; `std::numeric_limits<T>::digits` if all bits are set.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr int countr_one(T x) noexcept
{
    return _bit_priv::countr_zero_impl<T>(static_cast<T>(~x));
}

//=============================================================================
// popcount
//=============================================================================

namespace _bit_priv {

// SWAR: no loops, no hardware requirement, fine in constexpr.
constexpr int popcount_32_generic(uint32_t x) noexcept
{
    x = x - ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x + (x >> 4)) & 0x0F0F0F0Fu;
    return static_cast<int>((x * 0x01010101u) >> 24);
}

constexpr int popcount_64_generic(uint64_t x) noexcept
{
    x = x - ((x >> 1) & 0x5555555555555555ull);
    x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0Full;
    return static_cast<int>((x * 0x0101010101010101ull) >> 56);
}

constexpr int popcount_32_impl(uint32_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_popcount)

    return __builtin_popcount(static_cast<unsigned int>(x));

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED && (defined(_M_X64) || defined(_M_IX86))

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return popcount_32_generic(x);
    }
    return static_cast<int>(__popcnt(x));

#else

    return popcount_32_generic(x);

#endif
}

constexpr int popcount_64_impl(uint64_t x) noexcept
{
#if VX_HAS_BUILTIN(__builtin_popcountll)

    return __builtin_popcountll(static_cast<unsigned long long>(x));

#elif defined(_MSC_VER) && VX_HAS_IS_CONSTANT_EVALUATED && (defined(_M_X64) || defined(_M_IX86))

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return popcount_64_generic(x);
    }

    #if defined(_M_X64)

    return static_cast<int>(__popcnt64(x));

    #else // 32-bit x86: no __popcnt64

    return static_cast<int>(__popcnt(static_cast<uint32_t>(x)) + __popcnt(static_cast<uint32_t>(x >> 32)));

    #endif

#else

    return popcount_64_generic(x);

#endif
}

template <typename T>
constexpr int popcount_impl(T x) noexcept
{
    VX_STATIC_ASSERT_MSG(sizeof(T) <= 8, "Unsupported type size for popcount");

    VX_IF_CONSTEXPR (sizeof(T) <= 4)
    {
        return popcount_32_impl(static_cast<uint32_t>(x)); // zero-extension adds no set bits
    }
    else
    {
        return popcount_64_impl(static_cast<uint64_t>(x));
    }
}

} // namespace _bit_priv

/**
 * @brief Counts the number of set bits (like C++20 std::popcount).
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The value to analyze.
 * @return The number of 1 bits in `x`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr int popcount(T x) noexcept
{
    return _bit_priv::popcount_impl<T>(x);
}

//=============================================================================
// log2_floor
//=============================================================================

/**
 * @brief Index of the highest set bit, i.e. floor(log2(x)).
 *
 * `log2_floor(1) == 0`, `log2_floor(8) == 3`, `log2_floor(0xFF) == 7`.
 * Equals `bit_width(x) - 1`. On x86 this compiles to a bare `bsr`.
 *
 * @pre `x != 0`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr int log2_floor(T x) noexcept
{
    VX_ASSERT(x != 0);
    return std::numeric_limits<T>::digits - 1 - _bit_priv::countl_zero_nz_impl<T>(x);
}

//=============================================================================
// has_single_bit
//=============================================================================

/**
 * @brief Checks whether `x` is an integral power of two (like C++20 std::has_single_bit).
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The value to check.
 * @return True if exactly one bit is set.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr bool has_single_bit(T x) noexcept
{
    return x != 0 && (x & static_cast<T>(x - 1)) == 0;
}

//=============================================================================
// has_single_bit_assume_nonzero
//=============================================================================

/**
 * @brief has_single_bit without the zero test.
 * @pre `x != 0`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr bool has_single_bit_assume_nonzero(T x) noexcept
{
    VX_ASSERT(x != 0);
    return (x & static_cast<T>(x - 1)) == 0;
}

//=============================================================================
// bit_width
//=============================================================================

/**
 * @brief Minimum number of bits needed to represent `x` (like C++20 std::bit_width).
 *
 * `bit_width(0) == 0`, `bit_width(1) == 1`, `bit_width(8) == 4`.
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The value to measure.
 * @return `digits - countl_zero(x)`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr int bit_width(T x) noexcept
{
    return std::numeric_limits<T>::digits - countl_zero(x);
}

//=============================================================================
// bit_ceil
//=============================================================================

/**
 * @brief Smallest power of two >= `x` (like C++20 std::bit_ceil).
 *
 * `bit_ceil(0) == bit_ceil(1) == 1`.
 *
 * @note If the result is not representable in `T` (i.e. `x` exceeds the largest
 *       power of two in `T`), this returns 0 instead of invoking undefined behavior.
 *       The standard version makes this a precondition violation.
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The input value.
 * @return The smallest power of two >= `x`, or 0 on overflow.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr T bit_ceil(T x) noexcept
{
    if (x <= static_cast<T>(1))
    {
        return static_cast<T>(1);
    }

    const int n = bit_width(static_cast<T>(x - 1));
    if (n >= std::numeric_limits<T>::digits)
    {
        return static_cast<T>(0); // 1 << digits is not representable
    }

    return static_cast<T>(static_cast<T>(1) << n);
}

//=============================================================================
// bit_ceil_assume_valid_shift
//=============================================================================

/**
 * @brief bit_ceil without the `x <= 1` and overflow checks.
 *
 * Uses the identity `bit_ceil(x) == 2 << log2_floor(x - 1)` for x >= 2.
 *
 * @pre `x >= 2` and `x <= (1 << (digits - 1))` (the result is representable).
 *      For inputs that may be 0 or 1, use the checked `bit_ceil`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr T bit_ceil_assume_valid_shift(T x) noexcept
{
    VX_ASSERT(x >= 2);
    VX_ASSERT(x <= static_cast<T>(static_cast<T>(1) << (std::numeric_limits<T>::digits - 1)));
    return static_cast<T>(static_cast<T>(2) << log2_floor(static_cast<T>(x - 1)));
}

//=============================================================================
// bit_floor
//=============================================================================

/**
 * @brief Largest power of two <= `x` (like C++20 std::bit_floor).
 *
 * `bit_floor(0) == 0`.
 *
 * @tparam T An unsigned integral type (not bool).
 * @param x The input value.
 * @return The largest power of two <= `x`, or 0 if `x == 0`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr T bit_floor(T x) noexcept
{
    if (x == static_cast<T>(0))
    {
        return static_cast<T>(0);
    }

    return static_cast<T>(static_cast<T>(1) << (bit_width(x) - 1));
}

//=============================================================================
// bit_floor_assume_nonzero
//=============================================================================

/**
 * @brief bit_floor without the zero test.
 * @pre `x != 0`.
 */
template <typename T, VX_REQUIRES(_bit_priv::is_bit_unsigned<T>::value)>
constexpr T bit_floor_assume_nonzero(T x) noexcept
{
    VX_ASSERT(x != 0);
    return static_cast<T>(static_cast<T>(1) << log2_floor(x));
}

//=============================================================================
// bit_cast
//=============================================================================

template <typename To, typename From, VX_REQUIRES(sizeof(To) == sizeof(From) && std::is_trivially_copyable<From>::value && std::is_trivially_copyable<To>::value)>
To bit_cast(const From& src) noexcept
{
    VX_STATIC_ASSERT_MSG(std::is_trivially_constructible<To>::value,
        "This implementation additionally requires "
        "destination type to be trivially constructible");

#if VX_HAS_BUILTIN(__builtin_bit_cast)

    return __builtin_bit_cast(To, src);

#else

    To dst;
    mem::copy(&dst, &src, sizeof(To));
    return dst;

#endif
}

} // namespace bit
} // namespace vx
