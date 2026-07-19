#pragma once

#include "vertex/config/assert.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/hex.hpp"
#include "vertex/std/math/float_bits.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_cast.hpp"
#include "vertex/std/string_utils.hpp"

namespace vx {
namespace str {

//==============================================================================
// result types
//==============================================================================

enum class to_string_error
{
    none,
    buffer_too_small,
    precision_too_large, // only returned for float
};

struct to_string_result
{
    size_t count;
    to_string_error err;
};

//==============================================================================

enum class from_string_error
{
    none,
    invalid_argument,
    out_of_range,
    buffer_too_small // only returned for hex
};

struct from_string_result
{
    size_t count;
    from_string_error err;
};

//==============================================================================
// hex
//==============================================================================

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
to_string_result to_hex_string(const void* data, const size_t size, C* buf, const size_t buf_size, const bool upper = false) noexcept
{
    const size_t needed = 2 * size;
    if (!buf || buf_size < needed)
    {
        return { 0, to_string_error::buffer_too_small };
    }

    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    for (size_t i = 0; i < size; ++i)
    {
        const C c1 = hex::digits[(bytes[i] >> 4) & 0xF]; // High nibble
        const C c2 = hex::digits[(bytes[i] >> 0) & 0xF]; // Low nibble

        *buf++ = upper ? to_upper_ascii_unchecked(c1) : c1;
        *buf++ = upper ? to_upper_ascii_unchecked(c2) : c2;
    }

    return { needed, to_string_error::none };
}

template <typename S, VX_REQUIRES(is_mutable_string_like<S>::value)>
to_string_result to_hex_string(const void* data, const size_t size, S& out) noexcept
{
    using C = typename S::value_type;
    out.resize(2 * size);
    return to_hex_string<C>(data, size, out.data(), out.size());
}

//==============================================================================

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr from_string_result from_hex_string(const C* hex, size_t hex_size, void* buf, size_t buf_size) noexcept
{
    const size_t needed = (hex_size + 1) / 2;
    if (buf_size < needed)
    {
        return { 0, from_string_error::buffer_too_small };
    }

    uint8_t* bytes = static_cast<uint8_t*>(buf);

    size_t i = 0;
    size_t j = 0;

    while (i + 1 < hex_size)
    {
        const int hi = hex::value(hex[i++]);
        const int lo = hex::value(hex[i++]);

        if (hi < 0 || lo < 0)
        {
            return { 0, from_string_error::invalid_argument };
        }

        bytes[j++] = static_cast<uint8_t>((hi << 4) | lo);
    }

    // Handle a trailing half-byte
    if (i < hex_size)
    {
        const int lo = hex::value(hex[i]);

        if (lo < 0)
        {
            return { 0, from_string_error::invalid_argument };
        }

        bytes[j++] = static_cast<uint8_t>(lo);
    }

    return { needed, from_string_error::none };
}

//==============================================================================
// integer helpers
//==============================================================================

enum : int
{
    invalid_digit = 255
};

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int char_to_digit(C c, int base = 10) noexcept
{
    int digit = invalid_digit;

    if (c >= static_cast<C>('0') && c <= static_cast<C>('9'))
    {
        digit = static_cast<int>(c - static_cast<C>('0'));
    }
    else
    {
        c = to_lower_ascii_unchecked(c);
        if (c >= static_cast<C>('a') && c <= static_cast<C>('z'))
        {
            digit = static_cast<int>(c - static_cast<C>('a')) + 10;
        }
    }

    return (digit < base) ? digit : invalid_digit;
}

namespace _string_convert_priv {

inline constexpr size_t digit_count_unsigned(const uint8_t v) noexcept
{
    // clang-format off
    if (v >= 100) return 3;
    if (v >= 10)  return 2;
    return 1;
    // clang-format on
}

inline constexpr size_t digit_count_unsigned(const uint16_t v) noexcept
{
    // clang-format off
    if (v >= 10000) return 5;
    if (v >= 1000)  return 4;
    if (v >= 100)   return 3;
    if (v >= 10)    return 2;
    return 1;
    // clang-format on
}

inline constexpr size_t digit_count_unsigned(const uint32_t v) noexcept
{
    // clang-format off
    if (v >= 100000000UL) return 9;
    if (v >= 10000000UL)  return 8;
    if (v >= 1000000UL)   return 7;
    if (v >= 100000UL)    return 6;
    if (v >= 10000UL)     return 5;
    if (v >= 1000UL)      return 4;
    if (v >= 100UL)       return 3;
    if (v >= 10UL)        return 2;
    return 1;
    // clang-format on
}

inline constexpr size_t digit_count_unsigned(const uint64_t v) noexcept
{
    // Fast path: reuse the uint32_t implementation.
    if (v < 1000000000ULL)
    {
        return digit_count_unsigned(static_cast<uint32_t>(v));
    }

    // clang-format off
    if (v >= 10000000000000000000ULL) return 20;
    if (v >= 1000000000000000000ULL)  return 19;
    if (v >= 100000000000000000ULL)   return 18;
    if (v >= 10000000000000000ULL)    return 17;
    if (v >= 1000000000000000ULL)     return 16;
    if (v >= 100000000000000ULL)      return 15;
    if (v >= 10000000000000ULL)       return 14;
    if (v >= 1000000000000ULL)        return 13;
    if (v >= 100000000000ULL)         return 12;
    if (v >= 10000000000ULL)          return 11;
    return 10;
    // clang-format on
}

static constexpr uint32_t pow10_table[] = {
    10u, 100u, 1000u, 10000u, 100000u, 1000000u, 10000000u, 100000000u
};

inline constexpr uint32_t pow10_u32(size_t n) noexcept
{
    VX_ASSERT(n > 0);
    VX_ASSERT(n <= 9);
    return pow10_table[n - 1];
}

template <typename I>
constexpr size_t digit_count(I value, const I base) noexcept
{
    size_t n = 0;

    do
    {
        value /= base;
        ++n;

    } while (value != 0);

    return n;
}

static constexpr const char int_digits[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z'
};

template <typename C>
constexpr int to_digit(C c) noexcept
{
    if (c >= static_cast<C>('0') && c <= static_cast<C>('9'))
    {
        return c - static_cast<C>('0');
    }

    c = str::to_lower_ascii_unchecked(c);
    if (c >= static_cast<C>('a') && c <= static_cast<C>('f'))
    {
        return c - static_cast<C>('a') + 10;
    }

    return invalid_digit;
}

} // namespace _string_convert_priv

//==============================================================================
// integer
//==============================================================================

struct integer_format_options
{
    int base = 10;
    bool uppercase = false;
    bool force_sign = false;
};

//==============================================================================

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result write_integer(I value, C* buf, const size_t buf_size, const integer_format_options& fmt = {}) noexcept
{
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

    using U = typename std::make_unsigned<I>::type;
    U uvalue = static_cast<U>(value);
    const U ubase = static_cast<U>(fmt.base);
    char sign = fmt.force_sign ? '+' : 0;

    VX_IF_CONSTEXPR (std::is_signed<I>::value)
    {
        if (value < 0)
        {
            // cast through unsigned to avoid UB on min value (-value would overflow)
            uvalue = ~uvalue + 1;
            sign = '-';
        }
    }

    const size_t digit_count = _string_convert_priv::digit_count(uvalue, ubase);
    const size_t needed = digit_count + static_cast<size_t>(sign != 0);
    if (buf_size < needed)
    {
        return { 0, to_string_error::buffer_too_small };
    }

    size_t i = needed;

    do
    {
        const char c = _string_convert_priv::int_digits[uvalue % ubase];
        buf[--i] = fmt.uppercase ? to_upper(c) : c;
        uvalue /= ubase;

    } while (uvalue);

    if (sign)
    {
        buf[0] = static_cast<C>(sign);
    }

    return { needed, to_string_error::none };
}

//==============================================================================

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
constexpr from_string_result parse_integer(const C* s, size_t size, I& value, const integer_format_options& fmt = {}) noexcept
{
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

    using U = typename std::make_unsigned<I>::type;
    bool is_negative = false;
    size_t i = 0;

    if (s[0] == C('+'))
    {
        ++i;
    }
    else if (s[0] == C('-'))
    {
        VX_IF_CONSTEXPR (!std::is_signed<I>::value)
        {
            return { 0, from_string_error::invalid_argument };
        }
        else
        {
            is_negative = true;
            ++i;
        }
    }

    U limit;
    VX_IF_CONSTEXPR (std::is_signed<I>::value)
    {
        limit = is_negative
            ? static_cast<U>(std::numeric_limits<I>::max()) + 1
            : static_cast<U>(std::numeric_limits<I>::max());
    }
    else
    {
        limit = std::numeric_limits<U>::max();
    }

    const size_t start = i;
    bool overflow = false;

    const U ubase = static_cast<U>(fmt.base);
    U uvalue = 0;

    while (i < size)
    {
        const int digit = char_to_digit(s[i], fmt.base);
        if (digit < 0 || digit >= fmt.base)
        {
            break;
        }

        const U udigit = static_cast<U>(digit);
        if (uvalue > (limit - udigit) / ubase)
        {
            overflow = true;
        }
        else
        {
            uvalue = uvalue * ubase + udigit;
        }

        ++i;
    }

    if (i == start)
    {
        return { 0, from_string_error::invalid_argument };
    }
    if (overflow)
    {
        return { i, from_string_error::out_of_range };
    }

    if (std::is_signed<I>::value && is_negative)
    {
        // Convert back to two's complement representation.
        value = static_cast<I>(~uvalue + 1);
    }
    else
    {
        value = static_cast<I>(uvalue);
    }

    return { i, from_string_error::none };
}

//==============================================================================
// float
//==============================================================================

enum class float_format : char
{
    general = 'g',
    fixed = 'f',
    scientific = 'e',
    hex = 'a',
};

namespace _string_convert_priv {

enum : uint32_t
{
    precision_unspecified = std::numeric_limits<uint32_t>::max(),

    float_fixed_default_precision = 6,
    float_scientific_default_precision = 6,

    float_hex_default_precision = 6,
    double_hex_default_precision = 13,

    max_float_precision = 1'000'000
};

} // namespace _string_convert_priv

template <typename C = char>
struct float_to_string_format_options
{
    float_format format = float_format::general;
    uint32_t precision = _string_convert_priv::precision_unspecified;

    VX_STATIC_ASSERT_MSG(type_traits::is_char<C>::value, "C must be a character type");
    C decimal_point = static_cast<C>('.');

    bool uppercase = false;
    bool force_sign = false;
    bool force_exp_sign = false;
    bool round = true;
};

template <typename C = char>
struct float_from_string_format_options
{
    float_format format = float_format::general;

    VX_STATIC_ASSERT_MSG(type_traits::is_char<C>::value, "C must be a character type");
    C decimal_point = static_cast<C>('.');
};

//==============================================================================
// general helpers
//==============================================================================

// https://github.com/microsoft/STL/blob/f3ae96af460b8fcb7d77c46fc1ad7d312900d1e7/stl/inc/xcharconv_ryu.h#L2390

namespace _string_convert_priv {

constexpr size_t digit_count_max3(const int value) noexcept
{
    // clang-format off
    if (value < 10)     return 1;
    if (value < 100)    return 2;
    return 3;
    // clang-format on
}

constexpr size_t digit_count_max4(const int value) noexcept
{
    // clang-format off
    if (value < 10)     return 1;
    if (value < 100)    return 2;
    if (value < 1000)   return 3;
    return 4;
    // clang-format on
}

template <typename F>
constexpr size_t base2_exp_digit_count(const int e2) noexcept
{
    VX_IF_CONSTEXPR (std::is_same<F, float>::value)
    {
        return digit_count_max3(e2);
    }
    VX_IF_CONSTEXPR (std::is_same<F, double>::value)
    {
        return digit_count_max4(e2);
    }
}

inline constexpr uint32_t log10_pow2(int e) noexcept
{
    VX_ASSERT(e >= 0);
    VX_ASSERT(e <= 1650);
    return static_cast<uint32_t>((static_cast<uint64_t>(e) * 0x13441350fbdll) >> 42);
}

inline constexpr int log10_pow5(int e) noexcept
{
    VX_ASSERT(e >= 0);
    VX_ASSERT(e <= 2620);
    return (static_cast<uint32_t>(e) * 732923u) >> 20;
}

inline constexpr size_t ceil_log10_pow2(size_t bits) noexcept
{
    return (bits * 30103 + 99999) / 100000;
}

inline constexpr uint32_t pow10_bit_width(uint32_t e)
{
    // should be exact for all pow 10 through 5000
    return static_cast<uint32_t>((static_cast<uint64_t>(e) * 14267572527ULL) >> 32) + 1;
}

template <typename F>
constexpr uint32_t subnormal_pow10(const typename vx::float_bits<F>::uint_type m_bits) noexcept
{
    // this estimate may be 1 less than the actual power of 10
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;
    constexpr size_t uint_bits = sizeof(uint_type) * CHAR_BIT;

    const int leading = bit::countl_zero(m_bits) - (uint_bits - traits::mantissa_bits);
    const int e2 = 1 - traits::exponent_bias - leading;
    const int e10 = -(log10_pow5(-e2) + e2);
    return e10;
}

template <typename F>
constexpr int normal_pow10(const int e2) noexcept
{
    // this estimate may be 1 greater than the actual power of 10
    const int e10 = -(log10_pow5(-e2) + e2) - (e2 > 3);
    return e10;
}

template <typename F>
constexpr int large_integer_pow10(const int e2) noexcept
{
    // this estimate may be 1 less than the actual power of 10
    const int e10 = log10_pow2(e2);
    return e10;
}

template <typename C>
inline constexpr void fill_n_zeros(C* buf, size_t n) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        for (size_t i = 0; i < n; ++i)
        {
            buf[i] = C('0');
        }
    }
    else
    {
        mem::fill_range(buf, n, C('0'));
    }
}

template <typename F>
struct float_bits
{
    using traits = float_traits<F>;
    using uint_type = typename traits::uint_type;

    constexpr float_bits(const F value) noexcept
    {
        const auto bits = bit::bit_cast<uint_type>(value);
        m_bits = (bits & traits::mantissa_field_mask);
        e_bits = (bits & traits::exponent_field_mask) >> traits::mantissa_bits;
        sign_bit = (bits & traits::sign_bit_mask) != 0;
    }

    uint_type m_bits;
    uint_type e_bits;
    bool sign_bit;

    constexpr bool is_subnormal() const noexcept
    {
        return (e_bits == 0 && m_bits != 0);
    }
};

enum class float_write_status
{
    failed,
    finished,
    continue_
};

template <typename limb_type_, uint32_t limb_count_, typename wide_type_>
struct big_int
{
    using limb_type = limb_type_;
    using wide_type = wide_type_;
    static constexpr uint32_t limb_count = limb_count_;
    static constexpr size_t limb_bits = sizeof(limb_type) * CHAR_BIT;
    static constexpr size_t bit_count = limb_count * limb_bits;

    limb_type bits[limb_count_];

    constexpr big_int() noexcept
        : bits{}
    {
    }

    constexpr big_int(limb_type x) noexcept
        : bits{ x }
    {
    }

    constexpr big_int(const wide_type x) noexcept
        : bits{ static_cast<limb_type>(x), static_cast<limb_type>(x >> limb_bits) }
    {
    }

    constexpr big_int(limb_type m, int shift) noexcept
        : bits{}
    {
        const uint32_t base_limb = shift / limb_bits;
        const uint32_t bit_offset = shift % limb_bits;

        // 1. Store into the first limb
        bits[base_limb] = static_cast<limb_type>(m << bit_offset);

        // 2. Overflow into the second limb
        if (bit_offset)
        {
            bits[base_limb + 1] = static_cast<limb_type>(m >> (limb_bits - bit_offset));
        }
    }

    constexpr big_int(wide_type m, int shift) noexcept
        : bits{}
    {
        const uint32_t base_limb = shift / limb_bits;
        const uint32_t bit_offset = shift % limb_bits;

        // 1. Store into the first limb
        bits[base_limb] = static_cast<limb_type>(m << bit_offset);
        // 2. Overflow into the second limb
        bits[base_limb + 1] = static_cast<limb_type>(m >> (limb_bits - bit_offset));

        // Total bits available in the first two limbs starting from our current bit_offset
        constexpr uint32_t two_limbs_bits = limb_bits * 2;

        // 3. Overflow into the third limb (Optimized away at compile-time for 32-bit floats)
        if (bit_offset)
        {
            bits[base_limb + 2] = static_cast<limb_type>(m >> (two_limbs_bits - bit_offset));
        }
    }

    static constexpr big_int pow10(size_t n) noexcept
    {
        const size_t rem = n % 9;
        big_int out{ _string_convert_priv::pow10_u32(rem) };

        size_t count = n / 9;
        while (count--)
        {
            out.mul(1000000000u);
        }

        return out;
    }

    constexpr limb_type mul(limb_type x) noexcept
    {
        limb_type carry = 0;
        for (size_t j = 0; j < limb_count; ++j)
        {
            const wide_type product = (static_cast<wide_type>(bits[j]) * static_cast<wide_type>(x)) + static_cast<wide_type>(carry);
            bits[j] = static_cast<limb_type>(product);
            carry = static_cast<limb_type>(product >> limb_bits);
        }

        return carry;
    }

    constexpr limb_type extract_digit(size_t top_limb, limb_type lower_bits) const noexcept
    {
        const limb_type lo = bits[top_limb] >> lower_bits;
        const limb_type hi = bits[top_limb + 1] << (limb_bits - lower_bits);
        return lo | hi;
    }

    constexpr limb_type mul_extract(limb_type x, size_t top_limb, limb_type lower_bits) noexcept
    {
        mul(x);
        return extract_digit(top_limb, lower_bits);
    }

    constexpr limb_type mul_extract_clear(limb_type x, size_t top_limb, limb_type lower_bits, limb_type upper_mask) noexcept
    {
        const limb_type digit = mul_extract(x, top_limb, lower_bits);
        bits[top_limb] &= upper_mask;
        bits[top_limb + 1] = 0;
        return digit;
    }

    //=========================================================================

    constexpr void mul_pow10(size_t count) noexcept
    {
        while (count >= 9)
        {
            mul(1000000000u);
            count -= 9;
        }

        if (count)
        {
            mul(pow10_u32(count));
        }
    }

    constexpr bool mul_pow10_safe(size_t count) noexcept
    {
        while (count >= 9)
        {
            if (mul(1000000000u) != 0)
            {
                return false;
            }
            count -= 9;
        }

        return count ? (mul(pow10_u32(count)) == 0) : true;
    }

    //=========================================================================

    constexpr limb_type div_extract(limb_type x, size_t start_index) noexcept
    {
        constexpr uint32_t half = limb_bits / 2;
        constexpr limb_type half_mask = (limb_type{ 1 } << half) - 1;

        limb_type remainder = 0;

        for (size_t i = start_index + 1; i-- > 0;)
        {
            const limb_type limb = bits[i];
            const limb_type hi = limb >> half;
            const limb_type lo = limb & half_mask;

            const wide_type d1 = (static_cast<wide_type>(remainder) << half) | hi;
            const limb_type q1 = static_cast<limb_type>(d1 / x);
            remainder = static_cast<limb_type>(d1 % x);

            const wide_type d2 = (static_cast<wide_type>(remainder) << half) | lo;
            const limb_type q2 = static_cast<limb_type>(d2 / x);
            remainder = static_cast<limb_type>(d2 % x);

            bits[i] = (q1 << half) | q2;
        }

        return remainder;
    }

    // don't track top limb to allow loop unrolling
    constexpr limb_type div_extract(limb_type x) noexcept
    {
        constexpr uint32_t half = limb_bits / 2;
        constexpr limb_type half_mask = (limb_type{ 1 } << half) - 1;

        limb_type remainder = 0;

        for (size_t i = limb_count; i-- > 0;)
        {
            const limb_type limb = bits[i];
            const limb_type hi = limb >> half;
            const limb_type lo = limb & half_mask;

            const wide_type d1 = (static_cast<wide_type>(remainder) << half) | hi;
            const limb_type q1 = static_cast<limb_type>(d1 / x);
            remainder = static_cast<limb_type>(d1 % x);

            const wide_type d2 = (static_cast<wide_type>(remainder) << half) | lo;
            const limb_type q2 = static_cast<limb_type>(d2 / x);
            remainder = static_cast<limb_type>(d2 % x);

            bits[i] = (q1 << half) | q2;
        }

        return remainder;
    }

    //=========================================================================

    constexpr limb_type insert_digit(limb_type x) noexcept
    {
        limb_type carry = x;

        for (size_t j = 0; j < limb_count; ++j)
        {
            const wide_type product = (static_cast<wide_type>(bits[j]) * static_cast<wide_type>(10)) + static_cast<wide_type>(carry);
            bits[j] = static_cast<limb_type>(product);
            carry = static_cast<limb_type>(product >> limb_bits);
        }

        return carry;
    }

    constexpr void shl(uint32_t shift) noexcept
    {
        const uint32_t limbshift = shift / limb_bits;
        const uint32_t bitshift = shift % limb_bits;

        if (bitshift == 0)
        {
            for (size_t i = limb_count; i-- > limbshift;)
            {
                bits[i] = bits[i - limbshift];
            }
        }
        else
        {
            for (size_t i = limb_count; i-- > limbshift + 1;)
            {
                bits[i] = (bits[i - limbshift] << bitshift) | (bits[i - limbshift - 1] >> (limb_bits - bitshift));
            }
            bits[limbshift] = bits[0] << bitshift;
        }

        for (size_t i = 0; i < limbshift; ++i)
        {
            bits[i] = 0;
        }
    }

    constexpr uint32_t bit_width() const noexcept
    {
        for (size_t i = limb_count; i-- > 0;)
        {
            if (bits[i] != 0)
            {
                return static_cast<uint32_t>(i * limb_bits + bit::bit_width(bits[i]));
            }
        }

        return 0;
    }

    template <typename U>
    constexpr auto extract_low_bits() const noexcept
    {
        constexpr size_t limbs_needed = (sizeof(U) + sizeof(limb_type) - 1) / sizeof(limb_type);

        U result = 0;
        for (size_t i = limbs_needed; i-- > 0;)
        {
            result <<= limb_bits;
            result |= static_cast<U>(bits[i]);
        }

        return result;
    }
};

//==============================================================================

template <typename F>
struct float_writing_traits
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;
    using limb_type = uint32_t;
    using wide_type = uint64_t;

    static constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;
    static constexpr uint32_t max_shift = traits::exponent_bias + traits::full_mantissa_bits;
    static constexpr uint32_t limb_count = ((max_shift + (limb_bits - 1)) / limb_bits) + 1;

    using big_int_type = big_int<limb_type, limb_count, wide_type>;
};

template <typename F>
struct float_frac_digit_extractor
{
    using base = float_writing_traits<F>;
    using uint_type = typename base::uint_type;
    using limb_type = typename base::limb_type;
    using big_int_type = typename base::big_int_type;

    uint32_t top_limb;
    uint32_t lower_bits;
    uint32_t upper_mask;
    big_int_type bits;

    constexpr float_frac_digit_extractor(const uint_type m2, const int shift) noexcept
        : top_limb(shift / base::limb_bits)
        , lower_bits(shift % base::limb_bits)
        , upper_mask((limb_type{ 1 } << lower_bits) - 1)
        , bits{ m2 }
    {}

    constexpr void shave_digits(size_t count) noexcept
    {
        bits.mul_pow10(count);
    }

    constexpr limb_type extract_digit() noexcept
    {
        return bits.mul_extract_clear(10, top_limb, lower_bits, upper_mask);
    }

    constexpr limb_type extract_last_digit() noexcept
    {
        return bits.mul_extract(10, top_limb, lower_bits);
    }
};

//==============================================================================

template <typename F, bool is_small>
struct float_int_digit_extractor;

template <typename F>
struct float_int_digit_extractor<F, false>
{
    using base = float_writing_traits<F>;
    using traits = typename base::traits;
    using uint_type = typename base::uint_type;
    using limb_type = typename base::limb_type;
    using big_int_type = typename base::big_int_type;

    big_int_type bits;
    size_t top_limb;

    constexpr float_int_digit_extractor(const uint_type m2, const int shift) noexcept
        : top_limb((static_cast<size_t>(shift) + traits::digits - 1) / base::limb_bits)
        , bits(m2, shift)
    {}

private:

    constexpr void shrink() noexcept
    {
        while (top_limb && bits.bits[top_limb] == 0)
        {
            --top_limb;
        }
    }

    constexpr void div_shrink(limb_type x) noexcept
    {
        bits.div_extract(x, top_limb);
        shrink();
    }

    constexpr limb_type div_extract_shrink(limb_type x) noexcept
    {
        const limb_type r = bits.div_extract(x, top_limb);
        shrink();
        return r;
    }

public:

    constexpr void shave_digits(size_t count) noexcept
    {
        while (count >= 9)
        {
            div_shrink(1000000000u);
            count -= 9;
        }

        if (count)
        {
            div_shrink(pow10_u32(count));
        }
    }

    constexpr limb_type extract_digit() noexcept
    {
        return div_extract_shrink(10);
    }

    constexpr limb_type extract_chunk() noexcept
    {
        return div_extract_shrink(1000000000u);
    }

    constexpr limb_type extract_small_chunk(size_t digits) noexcept
    {
        return div_extract_shrink(pow10_u32(digits));
    }

    constexpr bool has_last_digit() noexcept
    {
        return bits.bits[0] != 0;
    }

    constexpr limb_type extract_last_digit() noexcept
    {
        VX_ASSERT(top_limb == 0);
        VX_ASSERT(bits.bits[0] <= 9);
        return bits.bits[0];
    }
};

template <typename F>
struct float_int_digit_extractor<F, true>
{
    using base = float_writing_traits<F>;
    using uint_type = typename base::uint_type;
    using limb_type = typename base::limb_type;
    using big_int_type = typename base::big_int_type;

    big_int_type bits;

    constexpr float_int_digit_extractor(const uint_type m2, const int shift) noexcept
        : bits(m2, shift)
    {}

public:

    constexpr void shave_digits(size_t count) noexcept
    {
        while (count >= 9)
        {
            bits.div_extract(1000000000u);
            count -= 9;
        }

        if (count)
        {
            bits.div_extract(pow10_u32(count));
        }
    }

    constexpr limb_type extract_digit() noexcept
    {
        return bits.div_extract(10);
    }

    constexpr limb_type extract_chunk() noexcept
    {
        return bits.div_extract(1000000000u);
    }

    constexpr limb_type extract_small_chunk(size_t digits) noexcept
    {
        return bits.div_extract(pow10_u32(digits));
    }

    constexpr bool has_last_digit() noexcept
    {
        return bits.bits[0] != 0;
    }

    constexpr limb_type extract_last_digit() noexcept
    {
        VX_ASSERT(bits.bits[0] <= 9);
        return bits.bits[0];
    }
};

template <typename F>
using float_int_digit_extractor_selector = float_int_digit_extractor<F, (sizeof(F) <= sizeof(float))>;

//==============================================================================

template <typename F>
struct chunk_buffer
{
    using base = float_writing_traits<F>;
    using traits = typename base::traits;
    using uint_type = typename base::uint_type;
    using limb_type = typename base::limb_type;
    using int_writer_context = float_int_digit_extractor_selector<F>;

    static constexpr uint32_t max_shift = base::max_shift;
    static constexpr uint32_t total_bits = max_shift + traits::digits;
    static constexpr size_t max_chunks = ceil_log10_pow2(total_bits) / 9;

    int_writer_context& ext;
    limb_type chunks[max_chunks];
    size_t chunk_count;

    constexpr chunk_buffer(int_writer_context& ctx_) noexcept
        : ext(ctx_)
        , chunks{}
        , chunk_count(0)
    {}

    constexpr void extract_chunk() noexcept
    {
        chunks[chunk_count++] = ext.extract_chunk();
    }

    constexpr auto get(size_t index) const noexcept
    {
        VX_ASSERT(index < chunk_count);
        return chunks[index];
    }

    constexpr size_t count() const noexcept
    {
        return chunk_count;
    }

    constexpr auto get_last_chunk(size_t digits) const noexcept
    {
        return ext.extract_small_chunk(digits);
    }

    constexpr auto has_last_digit() const noexcept
    {
        return ext.has_last_digit();
    }

    constexpr auto get_last_digit() const noexcept
    {
        return ext.extract_last_digit();
    }
};

//==============================================================================

template <typename F, typename C>
constexpr float_write_status write_float_start(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt, size_t& n) noexcept
{
    using traits = typename float_bits<F>::traits;

    if (!buf)
    {
        return float_write_status::finished;
    }

    const char sign = fb.sign_bit ? '-' : (fmt.force_sign ? '+' : 0);
    if (sign)
    {
        if (buf_size < 2)
        {
            return float_write_status::failed;
        }

        buf[n++] = static_cast<C>(sign);
    }

    if (fb.e_bits == traits::inf_nan_exponent)
    {
        const C n_val = static_cast<C>(fmt.uppercase ? 'N' : 'n');

        // inf
        if (fb.m_bits == 0)
        {
            if (buf_size - n < 3)
            {
                return float_write_status::failed;
            }

            buf[n++] = static_cast<C>(fmt.uppercase ? 'I' : 'i');
            buf[n++] = n_val;
            buf[n++] = static_cast<C>(fmt.uppercase ? 'F' : 'f');
        }
        // nan
        else
        {
            const bool indefinite = fb.m_bits == traits::quiet_nan_bit_mask;

            const size_t needed = indefinite ? 8 : 3;
            if (buf_size - n < needed)
            {
                return float_write_status::failed;
            }

            buf[n++] = n_val;
            buf[n++] = static_cast<C>(fmt.uppercase ? 'A' : 'a');
            buf[n++] = n_val;

            if (indefinite)
            {
                buf[n++] = static_cast<C>('(');
                buf[n++] = static_cast<C>(fmt.uppercase ? 'I' : 'i');
                buf[n++] = n_val;
                buf[n++] = static_cast<C>(fmt.uppercase ? 'D' : 'd');
                buf[n++] = static_cast<C>(')');
            }
        }

        return float_write_status::finished;
    }

    return float_write_status::continue_;
}

//==============================================================================
// fixed format
//==============================================================================

template <typename C>
constexpr bool round_fixed(C* ptr, size_t int_digit_count, size_t precision) noexcept
{
    const size_t decimal_index = int_digit_count;

    // Fractional digits
    for (size_t i = decimal_index + precision; i-- > decimal_index;)
    {
        if (ptr[i] != static_cast<C>('9'))
        {
            ++ptr[i];
            return false;
        }

        ptr[i] = static_cast<C>('0');
    }

    // Integer digits
    for (size_t i = decimal_index; i-- > 0;)
    {
        if (ptr[i] != static_cast<C>('9'))
        {
            ++ptr[i];
            return false;
        }

        ptr[i] = static_cast<C>('0');
    }

    // Overflow (e.g. 999.99 -> 1000.00)
    return true;
}

template <typename C>
constexpr size_t write_fixed_zero(C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt, const size_t precision) noexcept
{
    const size_t precision_char_count = static_cast<size_t>(precision > 0) + precision;
    const size_t needed = 1 + precision_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    *buf++ = static_cast<C>('0');
    if (precision > 0)
    {
        *buf++ = fmt.decimal_point;
        fill_n_zeros(buf, precision);
    }

    return needed;
}

template <typename F, typename C>
constexpr size_t write_fixed_normal(typename float_bits<F>::uint_type m2, int e10, int shift, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt, const size_t precision) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    const size_t precision_char_count = static_cast<size_t>(precision > 0) + precision;
    const size_t needed = 1 + precision_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    buf[0] = static_cast<C>('0');
    if (precision == 0 && !fmt.round)
    {
        return 1;
    }

    buf[1] = fmt.decimal_point;

    using extractor = float_frac_digit_extractor<F>;
    extractor ext(m2, shift);

    // default for most common case
    size_t leading_zero_count = 0;

    C* ptr = buf + 2;

    // first find the first non 0 digit
    if (e10 > 1)
    {
        // The estimated decimal exponent can either be exact, or 1 too large
        // We subtract 2 here to move the estimate to either exact or 1 too small
        leading_zero_count = static_cast<size_t>(e10) - 2;
        if (precision < leading_zero_count)
        {
            fill_n_zeros(ptr, precision);
            return needed;
        }

        fill_n_zeros(ptr, leading_zero_count);
        ptr += leading_zero_count;
        ext.shave_digits(leading_zero_count);
    }

    const size_t remaining = precision - leading_zero_count;
    for (size_t i = 0; i < remaining; ++i)
    {
        const auto digit = ext.extract_digit();
        *ptr++ = static_cast<C>(hex::digits[digit]);
    }

    if (fmt.round)
    {
        const auto round_digit = ext.extract_last_digit();
        if (round_digit >= 5)
        {
            round_fixed<C>(buf, 1, precision_char_count);
        }
    }

    return needed;
}

template <typename F, typename C>
constexpr size_t write_fixed_large(typename float_bits<F>::uint_type m2, int e10, int shift, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt, const size_t precision) noexcept
{
    size_t int_digit_count = static_cast<size_t>(e10) + 1;
    const size_t precision_char_count = static_cast<size_t>(precision > 0) + precision;
    const size_t needed = int_digit_count + precision_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    using extractor = float_int_digit_extractor<F, false>;
    extractor ext(m2, shift);

    C* ptr = buf + int_digit_count;

    VX_IF_CONSTEXPR (sizeof(F) <= 4)
    {
        for (size_t i = 0; i < int_digit_count; ++i)
        {
            const auto digit = ext.extract_digit();
            *(--ptr) = static_cast<C>(hex::digits[digit]);
        }
    }
    else
    {
        using chunk_tracker = chunk_buffer<F>;
        chunk_tracker chunks(ext);

        size_t digits = int_digit_count;

        while (digits >= 9)
        {
            chunks.extract_chunk();
            digits -= 9;
        }

        for (size_t i = 0; i < chunks.count(); ++i)
        {
            uint32_t chunk = chunks.get(i);
            for (size_t i = 0; i < 9; ++i)
            {
                const auto digit = chunk % 10;
                *(--ptr) = static_cast<C>(hex::digits[digit]);
                chunk /= 10;
            }
        }

        if (digits)
        {
            auto chunk = chunks.get_last_chunk(digits);
            for (size_t i = 0; i < digits; ++i)
            {
                const auto digit = chunk % 10;
                *(--ptr) = static_cast<C>(hex::digits[digit]);
                chunk /= 10;
            }
        }
    }

    // There may be 1 digit left over in rare cases
    if (ext.has_last_digit())
    {
        if (buf_size < (needed + 1))
        {
            return 0;
        }

        const auto digit = ext.extract_last_digit();
        mem::move_range(buf + 1, buf, int_digit_count);
        buf[0] = static_cast<C>(hex::digits[digit]);
        ++int_digit_count;
    }

    if (precision > 0)
    {
        buf[int_digit_count] = fmt.decimal_point;
        fill_n_zeros(buf + int_digit_count + 1, precision);
    }

    return int_digit_count + precision_char_count;
}

template <typename F, typename C>
constexpr size_t write_fixed_mixed(typename float_bits<F>::uint_type m2, int shift, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt, const size_t precision) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;
    using limb_type = uint32_t;

    const int frac_bit_count = -shift;
    uint_type int_bits = m2 >> frac_bit_count;

    // should be exact
    const size_t int_digit_count = digit_count_unsigned(int_bits);

    const size_t precision_char_count = static_cast<size_t>(precision > 0) + precision;
    size_t needed = int_digit_count + precision_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    C* ptr = buf + int_digit_count;

    // int digits
    {
        while (int_bits)
        {
            const auto digit = int_bits % 10;
            *--ptr = static_cast<C>(hex::digits[digit]);
            int_bits /= 10;
        }
    }

    if (precision == 0)
    {
        return needed;
    }

    // float digits
    {
        ptr += int_digit_count;
        *ptr++ = fmt.decimal_point;

        const uint_type raw_frac_bits = m2 & ((uint_type{ 1 } << frac_bit_count) - 1);
        using extractor = float_frac_digit_extractor<F>;
        extractor ext(raw_frac_bits, -shift);

        for (size_t i = 0; i < precision; ++i)
        {
            const limb_type digit = ext.extract_digit();
            *ptr++ = static_cast<C>(hex::digits[digit]);
        }

        // round
        if (fmt.round)
        {
            const auto round_digit = ext.extract_last_digit();
            if (round_digit >= 5)
            {
                if (round_fixed<C>(buf, int_digit_count, precision))
                {
                    ++needed;
                    if (buf_size < needed)
                    {
                        return 0;
                    }

                    mem::move(buf + 1, buf, needed - 1);
                    buf[0] = static_cast<C>('1');
                }
            }
        }
    }

    return needed;
}

template <typename F, typename C>
constexpr size_t write_float_fixed_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    const size_t precision = fmt.precision == precision_unspecified ? float_fixed_default_precision : fmt.precision;

    if (fb.e_bits == 0)
    {
        if (fb.m_bits == 0)
        {
            // zero
            return write_fixed_zero<C>(buf, buf_size, fmt, precision);
        }
        else
        {
            // subnormal
            const int e10 = subnormal_pow10<F>(fb.m_bits);
            const int shift = static_cast<int>(traits::exponent_bias + traits::full_mantissa_bits);
            return _string_convert_priv::write_fixed_normal<F, C>(fb.m_bits, e10, shift, buf, buf_size, fmt, precision);
        }
    }

    const uint_type m2 = fb.m_bits | (uint_type{ 1 } << traits::mantissa_bits);
    const int e2 = static_cast<int>(fb.e_bits) - traits::exponent_bias;
    const int shift = e2 - traits::mantissa_bits;

    if (e2 <= 0)
    {
        // normal
        const int e10 = _string_convert_priv::normal_pow10<F>(e2);
        return _string_convert_priv::write_fixed_normal<F, C>(m2, e10, -shift, buf, buf_size, fmt, precision);
    }

    if (shift >= 0)
    {
        // large integer
        const int e10 = _string_convert_priv::large_integer_pow10<F>(e2);
        return _string_convert_priv::write_fixed_large<F, C>(m2, e10, shift, buf, buf_size, fmt, precision);
    }
    else
    {
        // int and float part
        return _string_convert_priv::write_fixed_mixed<F, C>(m2, shift, buf, buf_size, fmt, precision);
    }
}

//==============================================================================
// scientific helpers
//==============================================================================

template <typename C>
constexpr void write_positive_exponent_block(int e10, C* buf, size_t exp_char_count, bool upper, bool force_sign) noexcept
{
    buf[0] = static_cast<C>(upper ? 'E' : 'e');

    if (force_sign)
    {
        buf[1] = static_cast<C>('+');
    }

    size_t i = exp_char_count;

    do
    {
        const int digit = e10 % 10;
        buf[--i] = static_cast<C>(hex::digits[digit]);
        e10 /= 10;

    } while (e10 > 0);
}

template <typename C>
constexpr void write_negative_exponent_block(int e10, C* buf, size_t exp_char_count, bool upper) noexcept
{
    buf[0] = static_cast<C>(upper ? 'E' : 'e');
    buf[1] = static_cast<C>('-');

    size_t i = exp_char_count;

    do
    {
        const int digit = e10 % 10;
        buf[--i] = static_cast<C>(hex::digits[digit]);
        e10 /= 10;

    } while (e10 > 0);
}

template <typename C>
constexpr bool scientific_carry_round(C* ptr, size_t leading_char_count) noexcept
{
    // precision == 0
    if (leading_char_count == 1)
    {
        if (ptr[0] != static_cast<C>('9'))
        {
            ++ptr[0];
            return false;
        }

        ptr[0] = static_cast<C>('1');
        return true;
    }

    // should have at lease digit + decimal + digit
    VX_ASSERT(leading_char_count >= 3);

    // fractional digits only
    for (size_t i = leading_char_count - 1; i > 1; --i)
    {
        if (ptr[i] != static_cast<C>('9'))
        {
            ++ptr[i];
            return false;
        }

        ptr[i] = static_cast<C>('0');
    }

    // leading digit
    if (ptr[0] != static_cast<C>('9'))
    {
        ++ptr[0];
        return false;
    }

    // If we reach here, a number like "9.99" is now "0.00"
    // Move ONLY the fractional zeros right by 1 (skipping the decimal point)
    // The very last character gets safely dropped off the end.
    mem::move_range(ptr + 3, ptr + 2, leading_char_count - 3);

    // Fix the prefix directly
    ptr[0] = static_cast<C>('1');
    ptr[2] = static_cast<C>('0');

    return true;
}

template <typename C>
constexpr size_t write_scientific_zero(C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt, const size_t precision) noexcept
{
    const size_t precision_char_count = static_cast<size_t>(precision > 0) + precision;
    const size_t leading_char_count = 1 + precision_char_count;
    const size_t exp_char_count = 1 + static_cast<size_t>(fmt.force_exp_sign) + 1;
    const size_t needed = leading_char_count + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    buf[0] = static_cast<C>('0');

    if (precision > 0)
    {
        buf[1] = fmt.decimal_point;
        fill_n_zeros(buf + 2, precision);
    }

    C* exp_ptr = buf + leading_char_count;

    exp_ptr[0] = static_cast<C>(fmt.uppercase ? 'E' : 'e');
    if (fmt.force_exp_sign)
    {
        exp_ptr[1] = static_cast<C>('+');
    }
    exp_ptr[exp_char_count - 1] = static_cast<C>('0');

    return needed;
}

template <typename F, typename C>
constexpr size_t write_scientific_normal(typename float_bits<F>::uint_type m2, int e10, int shift, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt, const size_t precision) noexcept
{
    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
    size_t exp_char_count = 1 + 1 + digit_count_max3(e10);
    size_t needed = leading_char_count + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    using extractor = float_frac_digit_extractor<F>;
    extractor ext(m2, shift);

    bool exp_changed = false;

    // first find the first non 0 digit
    if (e10 <= 1)
    {
        // Our estimate should be accurate when e10 is 1, and this is the most common case.
        const auto digit = ext.extract_digit();
        buf[0] = static_cast<C>(hex::digits[digit]);
    }
    else
    {
        // The estimated decimal exponent can be at most 1 too large. Rather than
        // removing all estimated leading zeros, we leave one digit in place so we
        // can verify the estimate.
        //
        // After shaving off (leading_zero_count - 1) digits, we inspect the next digit:
        //
        //   digit != 0  -> the estimate was 1 too large, so decrement e10.
        //   digit == 0  -> the estimate was correct; consume one more digit to
        //                  reach the first non-zero digit.
        //
        // Example:
        //   Estimated 4 leading zeros, actual 5.
        //   Remove 3 zeros and inspect the 4th digit. If it is non-zero, the
        //   estimate was too large by 1. Otherwise, consume one more digit to
        //   reach the leading non-zero digit.
        const size_t leading_zero_count = static_cast<size_t>(e10) - 1;
        const size_t shave = leading_zero_count - 1;
        ext.shave_digits(shave);

        auto digit = ext.extract_digit();
        if (digit != 0)
        {
            // we over estimated e10 by 1
            --e10;
            exp_changed = true;
        }
        else
        {
            digit = ext.extract_digit();
        }

        buf[0] = static_cast<C>(hex::digits[digit]);
    }

    if (precision > 0)
    {
        buf[1] = fmt.decimal_point;

        for (size_t i = 0; i < precision; ++i)
        {
            const auto digit = ext.extract_digit();
            buf[2 + i] = static_cast<C>(hex::digits[digit]);
        }
    }

    if (fmt.round)
    {
        const auto round_digit = ext.extract_last_digit();
        if (round_digit >= 5)
        {
            if (scientific_carry_round(buf, leading_char_count))
            {
                ++e10;
                // if we changed the exponent before, we now need to change it back
                exp_changed = !exp_changed;
            }
        }
    }

    if (exp_changed)
    {
        exp_char_count = 1 + 1 + digit_count_max3(e10);
        needed = leading_char_count + exp_char_count;
        if (buf_size < needed)
        {
            return 0;
        }
    }

    write_negative_exponent_block(e10, buf + leading_char_count, exp_char_count, fmt.uppercase);
    return needed;
}

template <typename F, typename C>
constexpr size_t write_scientific_large(typename float_bits<F>::uint_type m2, int e10, int shift, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt, const size_t precision) noexcept
{
    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
    size_t exp_char_count = 1 + static_cast<size_t>(fmt.force_exp_sign) + digit_count_max3(e10);
    size_t needed = leading_char_count + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    using extractor = float_int_digit_extractor_selector<F>;
    extractor ext(m2, shift);

    bool exp_changed = false;

    using limb_type = typename extractor::limb_type;
    limb_type round_digit = 0;
    C* back_ptr = buf + leading_char_count;

    const size_t int_digit_count = static_cast<size_t>(e10) + 1;
    const size_t digits_needed = static_cast<size_t>(precision) + 1;
    size_t zero_fill = 0;

    // first we shave off the digits we don't care about
    if (digits_needed < int_digit_count)
    {
        const size_t trailing_digit_count = int_digit_count - digits_needed;
        ext.shave_digits(trailing_digit_count - 1);
        round_digit = ext.extract_digit();
    }
    else if (digits_needed > int_digit_count)
    {
        // pad with 0s if there are not enough digits
        zero_fill = digits_needed - int_digit_count;
        fill_n_zeros(back_ptr - zero_fill, zero_fill);
        back_ptr -= zero_fill;
    }

    const size_t remaining = digits_needed - zero_fill;
    for (size_t i = 1; i < remaining; ++i)
    {
        const auto digit = ext.extract_digit();
        *(--back_ptr) = static_cast<C>(hex::digits[digit]);
    }

    // decimal + final digit
    {
        if (precision > 0)
        {
            *(--back_ptr) = fmt.decimal_point;
        }
        const auto digit = ext.extract_digit();
        *(--back_ptr) = static_cast<C>(hex::digits[digit]);
    }

    // There may be 1 digit left over in rare cases
    if (ext.has_last_digit())
    {
        const auto digit = ext.extract_last_digit();
        round_digit = static_cast<decltype(round_digit)>(buf[leading_char_count - 1] - '0');

        if (precision > 0)
        {
            // swap the first digit with the decimal and shift back
            mem::swap(*back_ptr, *(back_ptr + 1));
            mem::move_range(back_ptr + 1, back_ptr, digits_needed); // use digits needed because it includes the decimal
        }

        *back_ptr = static_cast<C>(hex::digits[digit]);

        // exponent estimate was too low by 1; adjust
        ++e10;
        exp_changed = true;
    }

    if (fmt.round && round_digit >= 5)
    {
        if (scientific_carry_round(buf, leading_char_count))
        {
            ++e10;
            exp_changed = true;
        }
    }

    if (exp_changed)
    {
        exp_char_count = 1 + 1 + digit_count_max3(e10);
        needed = leading_char_count + exp_char_count;
        if (buf_size < needed)
        {
            return 0;
        }
    }

    write_positive_exponent_block(e10, buf + leading_char_count, exp_char_count, fmt.uppercase, fmt.force_exp_sign);
    return needed;
}

template <typename F, typename C>
constexpr size_t write_scientific_mixed(typename float_bits<F>::uint_type m2, int shift, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt, const size_t precision) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    const int frac_bit_count = -shift;
    uint_type int_bits = m2 >> frac_bit_count;

    // should be exact
    const size_t int_digit_count = digit_count_unsigned(int_bits);
    int e10 = static_cast<int>(int_digit_count - 1);

    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
    const size_t old_exp_digits = digit_count_max3(e10);
    size_t exp_char_count = 1 + static_cast<size_t>(fmt.force_exp_sign) + old_exp_digits;
    size_t needed = leading_char_count + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    const size_t digits_needed = static_cast<size_t>(precision) + 1;
    const size_t int_digits_needed = (int_digit_count >= digits_needed) ? digits_needed : int_digit_count;
    const size_t frac_digits_needed = digits_needed - int_digits_needed;

    using extractor = float_frac_digit_extractor<F>;
    using limb_type = typename extractor::limb_type;
    limb_type round_digit = 0;

    // int digits
    {
        // first we shave off the digits we don't care about
        if (int_digits_needed < int_digit_count)
        {
            size_t shave = int_digit_count - int_digits_needed;

            // Batch divide by 10^9 (or 10^18 if using wide_type) to wipe out chunks of digits instantly
            while (shave > 9)
            {
                int_bits /= 1000000000u;
                shave -= 9;
            }

            if (shave > 1)
            {
                int_bits /= pow10_u32(shave);
            }

            round_digit = int_bits % 10;
            int_bits /= 10;
        }

        C* ptr = buf + int_digits_needed + static_cast<size_t>(precision > 0);

        // populate integer part
        size_t remaining = int_digits_needed;
        while (--remaining)
        {
            const limb_type digit = int_bits % 10;
            int_bits /= 10;
            *(--ptr) = static_cast<C>(hex::digits[digit]);
        }

        // decimal + final digit
        if (precision > 0)
        {
            *(--ptr) = fmt.decimal_point;
        }
        const limb_type digit = int_bits % 10;
        *(--ptr) = static_cast<C>(hex::digits[digit]);
    }

    // frac part + rounding
    if (frac_digits_needed || (int_digits_needed == int_digit_count && fmt.round))
    {
        const uint_type raw_frac_bits = m2 & ((uint_type{ 1 } << frac_bit_count) - 1);
        C* ptr = buf + int_digits_needed + 1;

        extractor ext(raw_frac_bits, -shift);

        for (size_t i = 0; i < frac_digits_needed; ++i)
        {
            const auto digit = ext.extract_digit();
            *ptr++ = static_cast<C>(hex::digits[digit]);
        }

        if (fmt.round)
        {
            round_digit = ext.extract_last_digit();
        }
    }

    bool exp_changed = false;

    if (fmt.round && round_digit >= 5)
    {
        if (scientific_carry_round(buf, leading_char_count))
        {
            ++e10;
            exp_changed = true;
        }
    }

    if (exp_changed)
    {
        exp_char_count = 1 + 1 + digit_count_max3(e10);
        needed = leading_char_count + exp_char_count;
        if (buf_size < needed)
        {
            return 0;
        }
    }

    write_positive_exponent_block(e10, buf + leading_char_count, exp_char_count, fmt.uppercase, fmt.force_exp_sign);
    return needed;
}

//==============================================================================
// scientific format
//==============================================================================

template <typename F, typename C>
constexpr size_t write_float_scientific_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    const size_t precision = fmt.precision == precision_unspecified ? float_scientific_default_precision : fmt.precision;

    if (fb.e_bits == 0)
    {
        if (fb.m_bits == 0)
        {
            // zero
            return write_scientific_zero<C>(buf, buf_size, fmt, precision);
        }
        else
        {
            // subnormal
            const int e10 = subnormal_pow10<F>(fb.m_bits);
            const int shift = static_cast<int>(traits::exponent_bias + traits::full_mantissa_bits);
            return _string_convert_priv::write_scientific_normal<F, C>(fb.m_bits, e10, shift, buf, buf_size, fmt, precision);
        }
    }

    const uint_type m2 = fb.m_bits | (uint_type{ 1 } << traits::mantissa_bits);
    const int e2 = static_cast<int>(fb.e_bits) - traits::exponent_bias;
    const int shift = e2 - traits::mantissa_bits;

    if (e2 < 0)
    {
        // normal
        const int e10 = _string_convert_priv::normal_pow10<F>(e2);
        return _string_convert_priv::write_scientific_normal<F, C>(m2, e10, -shift, buf, buf_size, fmt, precision);
    }

    if (shift >= 0)
    {
        // large integer
        const int e10 = _string_convert_priv::large_integer_pow10<F>(e2);
        return _string_convert_priv::write_scientific_large<F, C>(m2, e10, shift, buf, buf_size, fmt, precision);
    }
    else
    {
        // int and float part
        return _string_convert_priv::write_scientific_mixed<F, C>(m2, shift, buf, buf_size, fmt, precision);
    }
}

//==============================================================================
// hex format
//==============================================================================

template <typename F, typename C>
constexpr size_t write_float_hex_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    size_t precision = fmt.precision;
    if (precision == precision_unspecified)
    {
        VX_IF_CONSTEXPR (std::is_same<F, float>::value)
        {
            precision = float_hex_default_precision;
        }
        else VX_IF_CONSTEXPR (std::is_same<F, double>::value)
        {
            precision = double_hex_default_precision;
        }
    }

    const bool subnormal = fb.is_subnormal();
    size_t n = 0;

    // First calulate the exponent
    const int exp = subnormal
        ? 1 - static_cast<int>(traits::exponent_bias)
        : fb.e_bits
        ? static_cast<int>(fb.e_bits - traits::exponent_bias)
        : 0;

    const char exp_sign = (exp < 0) ? '-' : (fmt.force_exp_sign ? '+' : 0);

    int abs_exp = (exp < 0) ? -exp : exp;
    const size_t exp_digit_count = _string_convert_priv::base2_exp_digit_count<F>(abs_exp);

    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
    const size_t exp_char_count = 1 + static_cast<size_t>(exp_sign != 0) + exp_digit_count;
    const size_t needed = leading_char_count + exp_char_count;
    if (buf_size - n < needed)
    {
        return 0;
    }

    const bool leading_one = (fb.e_bits != 0);
    buf[n++] = static_cast<C>(hex::digits[leading_one]);

    if (precision > 0)
    {
        buf[n++] = fmt.decimal_point;

        constexpr uint32_t mbits = traits::mantissa_bits;
        constexpr uint32_t hex_digits = (mbits + 3) / 4;
        constexpr uint32_t shift = hex_digits * 4 - mbits;
        constexpr uint32_t topshift = (hex_digits - 1) * 4;

        size_t digits_remaining = precision;
        uint_type frac = fb.m_bits << shift;

        while (frac && digits_remaining)
        {
            const uint32_t digit = static_cast<uint32_t>((frac >> topshift) & 0xF);
            const char c = hex::digits[digit];
            buf[n++] = static_cast<C>(fmt.uppercase ? to_upper(c) : c);

            frac <<= 4;
            --digits_remaining;
        }

        if (digits_remaining)
        {
            _string_convert_priv::fill_n_zeros(buf + n, digits_remaining);
            n += digits_remaining;
        }
    }

    buf[n++] = static_cast<C>(fmt.uppercase ? 'P' : 'p');
    if (exp_sign != 0)
    {
        buf[n++] = static_cast<C>(exp_sign);
    }

    size_t pos = n + exp_digit_count;

    do
    {
        const int digit = abs_exp % 10;
        buf[--pos] = static_cast<C>(hex::digits[digit]);
        abs_exp /= 10;

    } while (abs_exp);

    n += exp_digit_count;

    return n;
}

template <typename F>
constexpr float_format resolve_general_format(const float_bits<F>& fb) noexcept
{
    if (fb.is_subnormal())
    {
        return float_format::scientific;
    }

    using traits = typename _string_convert_priv::float_bits<F>::traits;
    const int e2 = static_cast<int>(fb.e_bits - traits::exponent_bias);

    int e10;
    if (e2 < 0)
    {
        e10 = _string_convert_priv::normal_pow10<F>(e2);
    }
    else
    {
        e10 = _string_convert_priv::large_integer_pow10<F>(e2);
    }

    return (e10 < -4 || e10 >= float_scientific_default_precision) ? float_format::scientific : float_format::fixed;
}

} // namespace _string_convert_priv

//==============================================================================
// general
//==============================================================================

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result write_float_fixed(const F value, C* buf, size_t buf_size, const float_to_string_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    const _string_convert_priv::float_bits<F> bits{ value };
    size_t n = 0;

    using float_write_status = _string_convert_priv::float_write_status;
    const float_write_status status = _string_convert_priv::write_float_start<F, C>(bits, buf, buf_size, fmt, n);
    if (status == float_write_status::failed)
    {
        return { 0, to_string_error::buffer_too_small };
    }
    if (status == float_write_status::finished)
    {
        return { n, to_string_error::none };
    }

    buf += n;
    buf_size -= n;

    const size_t written = _string_convert_priv::write_float_fixed_impl<F, C>(bits, buf, buf_size, fmt);
    if (written == 0)
    {
        return { 0, to_string_error::buffer_too_small };
    }

    return { n + written, to_string_error::none };
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result write_float_scientific(const F value, C* buf, size_t buf_size, const float_to_string_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    const _string_convert_priv::float_bits<F> bits{ value };
    size_t n = 0;

    using float_write_status = _string_convert_priv::float_write_status;
    const float_write_status status = _string_convert_priv::write_float_start<F, C>(bits, buf, buf_size, fmt, n);
    if (status == float_write_status::failed)
    {
        return { 0, to_string_error::buffer_too_small };
    }
    if (status == float_write_status::finished)
    {
        return { n, to_string_error::none };
    }

    buf += n;
    buf_size -= n;

    const size_t written = _string_convert_priv::write_float_scientific_impl<F, C>(bits, buf, buf_size, fmt);
    if (written == 0)
    {
        return { 0, to_string_error::buffer_too_small };
    }

    return { n + written, to_string_error::none };
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result write_float_hex(const F value, C* buf, size_t buf_size, const float_to_string_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    const _string_convert_priv::float_bits<F> bits{ value };
    size_t n = 0;

    using float_write_status = _string_convert_priv::float_write_status;
    const float_write_status status = _string_convert_priv::write_float_start<F, C>(bits, buf, buf_size, fmt, n);
    if (status == float_write_status::failed)
    {
        return { 0, to_string_error::buffer_too_small };
    }
    if (status == float_write_status::finished)
    {
        return { n, to_string_error::none };
    }

    buf += n;
    buf_size -= n;

    const size_t written = _string_convert_priv::write_float_hex_impl<F, C>(bits, buf, buf_size, fmt);
    if (written == 0)
    {
        return { 0, to_string_error::buffer_too_small };
    }

    return { n + written, to_string_error::none };
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result write_float(const F value, C* buf, size_t buf_size, const float_to_string_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    if (fmt.precision != _string_convert_priv::precision_unspecified && fmt.precision > _string_convert_priv::max_float_precision)
    {
        return { 0, to_string_error::precision_too_large };
    }

    const _string_convert_priv::float_bits<F> bits{ value };
    size_t header = 0;
    size_t body = 0;

    using float_write_status = _string_convert_priv::float_write_status;
    const float_write_status status = _string_convert_priv::write_float_start<F, C>(bits, buf, buf_size, fmt, header);
    if (status == float_write_status::failed)
    {
        return { 0, to_string_error::buffer_too_small };
    }
    if (status == float_write_status::finished)
    {
        return { header, to_string_error::none };
    }

    float_format format = fmt.format;
    if (format == float_format::general)
    {
        format = _string_convert_priv::resolve_general_format(bits);
    }

    buf += header;
    buf_size -= header;

    switch (format)
    {
        default:
        case float_format::fixed:
        {
            body = _string_convert_priv::write_float_fixed_impl(bits, buf, buf_size, fmt);
            break;
        }
        case float_format::scientific:
        {
            body = _string_convert_priv::write_float_scientific_impl(bits, buf, buf_size, fmt);
            break;
        }
        case float_format::hex:
        {
            body = _string_convert_priv::write_float_hex_impl(bits, buf, buf_size, fmt);
            break;
        }
    }

    if (body == 0)
    {
        return { 0, to_string_error::buffer_too_small };
    }

    return { header + body, to_string_error::none };
}

//==============================================================================
// fixed float parsing
//==============================================================================

namespace _string_convert_priv {

struct float_reading_traits_base
{
    // Upper bound on the number of significant decimal digits that can ever
    // affect the correctly-rounded binary result for any supported float
    // format. Digits beyond this point are too small to change which way
    // the final mantissa rounds, so the parser never needs to look at more
    // than this many digits of the input.
    static constexpr size_t max_decimal_digits = 768;

    // Number of *bits* needed to exactly hold a 768-digit decimal integer
    // in binary. A decimal integer with D digits is < 10^D, so it takes
    // ceil(log2(10^D)) = ceil(D * log2(10)) bits to represent exactly.
    //   768 * log2(10) = 768 * 3.321928094887... = 2551.24...
    //   ceil(2551.24...) = 2552
    // This is the size (in bits) of the big integer that results from
    // treating the up-to-768 significant decimal digits as a plain integer
    // mantissa (before scaling by the decimal exponent).
    static constexpr size_t max_decimal_precision_bits = 2552; // ceil(log2(10^768))
};

template <typename F>
struct float_reading_traits : float_reading_traits_base
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;
    using limb_type = uint32_t;
    using wide_type = uint64_t;
    static constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    // traits::mantissa_bits is the number of *stored* fraction bits (e.g. 52
    // for double). The true precision of the significand is one bit wider
    // than that because of the implicit leading 1 bit that normal floats
    // don't store: full_significand_bits = mantissa_bits + 1.
    //
    // To correctly round an arbitrary-precision value down to that
    // full_significand_bits-wide significand (round-to-nearest-even), you
    // need one additional guard bit beyond the target width to know which
    // way to round:
    //   required_precision_bits = full_significand_bits + 1
    //                            = (mantissa_bits + 1) + 1
    //                            = mantissa_bits + 2
    // If fewer bits than this are available (value is exactly representable
    // with room to spare), that's fine — this is just the amount reserved
    // for the worst case.
    static constexpr size_t required_precision_bits = traits::full_mantissa_bits + 1;

    // Smallest exponent of a *normal* float. IEEE-754-style biased exponent
    // field's minimum nonzero value is 1, so the real exponent is 1 - bias.
    static constexpr int min_normal_exponent = 1 - traits::exponent_bias;

    // Smallest exponent representable at all (smallest subnormal). Subnormals
    // trade mantissa bits for exponent range one at a time: as the value
    // shrinks below min_normal_exponent, the implicit leading bit disappears
    // and the significand effectively shifts right, one bit per exponent
    // step, until all mantissa_bits of precision are exhausted. So the
    // smallest representable (nonzero) value sits mantissa_bits exponent
    // steps below min_normal_exponent.
    static constexpr int min_subnormal_exponent = min_normal_exponent - static_cast<int>(traits::mantissa_bits);

    // Total number of bits the working big integer must hold to correctly
    // parse *any* decimal string into *any* representable value of F,
    // including the smallest subnormal.
    //
    // During parsing, the up-to-768-digit decimal significand (which needs
    // max_decimal_precision_bits bits to hold exactly) has to be aligned
    // against the target binary exponent. The worst case for how far that
    // alignment shift can go is when the result is the smallest subnormal:
    // the value's bits must be shifted down across the entire span from
    // "no shift" to min_subnormal_exponent, i.e. up to
    // abs(min_subnormal_exponent) bits of shift/precision.
    //
    // So the total working precision needed is:
    //   (bits absorbed by shifting all the way down to the smallest
    //    subnormal exponent)      -> abs(min_subnormal_exponent)
    // + (bits needed to hold the full decimal significand exactly)
    //                              -> max_decimal_precision_bits
    // + (extra guard bits needed to correctly round the final mantissa)
    //                              -> required_precision_bits
    static constexpr size_t max_bits = static_cast<size_t>(-min_subnormal_exponent) + max_decimal_precision_bits + required_precision_bits;
    static constexpr size_t limb_count = (max_bits + limb_bits - 1) / limb_bits;
    using big_int_type = big_int<limb_type, limb_count, wide_type>;
};

template <typename F>
struct float_divider
{
    using base = float_reading_traits<F>;
    using uint_type = typename base::uint_type;
    using limb_type = typename base::limb_type;
    using big_int_type = typename base::big_int_type;

    big_int_type& bits;

    constexpr float_divider(big_int_type& bits_) noexcept
        : bits(bits_)
    {}

private:

    constexpr void shrink(size_t& top_limb) noexcept
    {
        while (top_limb && bits.bits[top_limb] == 0)
        {
            --top_limb;
        }
    }

    constexpr limb_type div_extract_shrink(limb_type x, size_t& top_limb) noexcept
    {
        const limb_type remainder = bits.div_extract(x, top_limb);
        shrink(top_limb);
        return remainder;
    }

public:

    constexpr bool div_pow10(size_t count) noexcept
    {
        limb_type remainder = 0;
        bool zero_remainder = true;

        size_t top_limb = big_int_type::limb_count - 1;
        shrink(top_limb);

        while (count >= 9)
        {
            remainder = div_extract_shrink(1000000000u, top_limb);
            zero_remainder = zero_remainder && (remainder == 0);
            count -= 9;
        }

        if (count)
        {
            remainder = div_extract_shrink(pow10_u32(count), top_limb);
            zero_remainder = zero_remainder && (remainder == 0);
        }

        return zero_remainder;
    }
};

template <typename F>
constexpr uint32_t max_decimal_mantissa_digits() noexcept
{
    using traits = typename float_bits<F>::traits;

    constexpr uint32_t P = traits::digits;
    constexpr int32_t Emin = traits::min_exponent - 1;

    constexpr int64_t N = static_cast<int64_t>(P) - Emin; // P - Emin
    constexpr int64_t K = N - P - 1;                      // == -Emin - 1

    return static_cast<uint32_t>(N) - log10_pow2(static_cast<int>(K));
}

// Computes _Value / 2^shift, then rounds the result according to round_to_nearest.
// By the time we call this function, we will already have discarded most digits.
// The caller must pass true for _Has_zero_tail if all discarded bits were zeroes.
template <typename M>
constexpr M shr_round(const M value, const uint32_t shift, const bool has_zero_tail) noexcept
{
    constexpr size_t bit_count = sizeof(M) * CHAR_BIT;

    if (shift >= bit_count)
    {
        if (shift == bit_count)
        {
            constexpr M shave_bits_mask = (1ULL << (bit_count - 1)) - 1;
            constexpr M round_bits_mask = (1ULL << (bit_count - 1));

            const bool round_bit = (value & round_bits_mask) != 0;
            const bool tail_bits = !has_zero_tail || (value & shave_bits_mask) != 0;

            // We round up the answer to 1 if the answer is greater than 0.5. Otherwise, we round down the answer to 0
            // if either [1] the answer is less than 0.5 or [2] the answer is exactly 0.5.
            return static_cast<M>(round_bit && tail_bits);
        }
        else
        {
            // If we'd need to shift 65 or more bits, the answer is less than 0.5 and is always rounded to zero:
            return 0;
        }
    }

    const M lsb_bit = value;
    const M round_bit = value << 1;
    const M has_tail_bits = round_bit - static_cast<M>(has_zero_tail);
    const M should_round = ((round_bit & (has_tail_bits | lsb_bit)) >> shift) & M{ 1 };
    return (value >> shift) + should_round;
}

template <typename C>
constexpr bool starts_with_case_insensitive(
    const C* s1,
    const size_t size1,
    const C* s2,
    size_t size2) noexcept
{
    if (size2 > size1)
    {
        return false;
    }

    for (; size2; --size2, ++s1, ++s2)
    {
        if (str::to_lower_ascii_unchecked(*s1) != *s2)
        {
            return false;
        }
    }

    return true;
}

template <typename F>
constexpr void assemble_float_zero(const bool is_negative, F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    uint_type sign_component = is_negative;
    sign_component <<= traits::sign_bit_shift;
    value = bit::bit_cast<F>(sign_component);
}

template <typename F>
constexpr void assemble_float_infinity(const bool is_negative, F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    uint_type sign_component = is_negative;
    sign_component <<= traits::sign_bit_shift;

    constexpr uint_type exponent_component = traits::exponent_field_mask;
    value = bit::bit_cast<F>(sign_component | exponent_component);
}

template <typename F>
constexpr void assemble_float_nan(const bool is_negative, const bool is_quiet, F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    uint_type sign_component = is_negative;
    sign_component <<= traits::sign_bit_shift;

    // Quiet NaN: MSB of the mantissa set, all other payload bits clear.
    // Signaling NaN: MSB of the mantissa clear, low payload bit set (must be nonzero payload).
    const uint_type mantissa_component = is_quiet ? traits::quiet_nan_bit_mask : uint_type{ 1 };

    constexpr uint_type exponent_component = traits::exponent_field_mask;
    value = bit::bit_cast<F>(sign_component | exponent_component | mantissa_component);
}

template <typename F>
constexpr void assemble_float(const bool is_negative, const typename float_bits<F>::uint_type mantissa, const int exponent, F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    uint_type sign_component = static_cast<uint_type>(is_negative);
    sign_component <<= traits::sign_bit_shift;

    uint_type exponent_component = static_cast<uint_type>(exponent + (traits::exponent_bias - 1));
    exponent_component <<= traits::exponent_field_shift;

    value = bit::bit_cast<F>(sign_component | (exponent_component + mantissa));
}

template <typename F>
constexpr from_string_error assemble_float_shifted(
    typename float_bits<F>::uint_type mantissa,
    int initial_exponent,
    bool is_negative,
    bool has_zero_tail,
    F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    constexpr int min_binary_exponent = traits::min_exponent - 1;
    constexpr int max_binary_exponent = traits::max_exponent - 1;

    // position of the highest set bit (0-based from LSB)
    const uint32_t mantissa_bit_count = bit::bit_width(mantissa);
    const int shift = static_cast<int>(traits::digits) - static_cast<int>(mantissa_bit_count);
    int exponent = initial_exponent - shift;

    if (exponent > max_binary_exponent)
    {
        // Overflow to infinity
        assemble_float_infinity<F>(is_negative, value);
        return from_string_error::out_of_range;
    }

    from_string_error err = from_string_error::none;

    if (exponent < min_binary_exponent)
    {
        // maybe representable as a subnormal, or underflow to zero
        exponent = min_binary_exponent;
        const int subnormal_shift = initial_exponent - min_binary_exponent;

        if (subnormal_shift < 0)
        {
            mantissa = shr_round(mantissa, static_cast<uint32_t>(-subnormal_shift), has_zero_tail);
            if (mantissa == 0)
            {
                // underflow to 0 (still valid)
                err = from_string_error::out_of_range;
            }
        }
        else
        {
            mantissa <<= subnormal_shift;
        }
    }
    else
    {
        if (shift < 0)
        {
            mantissa = shr_round(mantissa, static_cast<uint32_t>(-shift), has_zero_tail);
            if (mantissa > traits::mantissa_with_implicit_bit_mask && exponent == max_binary_exponent)
            {
                // overflow to infinity (still valid)
                err = from_string_error::out_of_range;
            }
        }
        else
        {
            mantissa <<= shift;
        }
    }

    assemble_float<F>(is_negative, mantissa, exponent, value);
    return err;
}

template <typename F, typename I>
constexpr from_string_error assemble_float_integer(
    const I& integer_value,
    uint32_t precision_bits,
    bool is_negative,
    bool has_nonzero_fractional_part,
    F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    constexpr size_t mantissa_bits = sizeof(uint_type) * CHAR_BIT;
    constexpr size_t limb_bits = I::limb_bits;

    // Extract the top bits to fill the uint type
    uint_type mantissa;
    int exponent = traits::mantissa_bits;
    bool has_zero_tail = !has_nonzero_fractional_part;

    if (precision_bits <= mantissa_bits)
    {
        VX_IF_CONSTEXPR (mantissa_bits <= limb_bits)
        {
            mantissa = static_cast<uint_type>(integer_value.bits[0]);
        }
        else
        {
            const auto lo = integer_value.bits[0];
            const auto hi = integer_value.bits[1];
            mantissa = static_cast<uint_type>(lo) + (static_cast<uint_type>(hi) << limb_bits);
        }
    }
    else
    {
        const uint32_t top_element_bits = precision_bits % limb_bits;
        const uint32_t top_element_index = precision_bits / limb_bits;

        const uint32_t top_mask = (1u << top_element_bits) - 1;
        const uint32_t top_shift = static_cast<uint32_t>(mantissa_bits) - top_element_bits; // Left

        const uint32_t bottom_bits = static_cast<uint32_t>(limb_bits) - top_element_bits;
        const uint32_t bottom_mask = ~top_mask;
        const uint32_t bottom_shift = static_cast<uint32_t>(limb_bits) - bottom_bits; // Right

        constexpr size_t bottom_index_offset = mantissa_bits / limb_bits;
        const uint32_t bottom_element_index = top_element_index - bottom_index_offset;

        VX_IF_CONSTEXPR (mantissa_bits <= limb_bits)
        {
            // occupies either 1 full limb in the middle, or 2 partial limbs
            mantissa =
                (static_cast<uint_type>(integer_value.bits[top_element_index] & top_mask) << top_shift) +
                (static_cast<uint_type>(integer_value.bits[bottom_element_index] & bottom_mask) >> bottom_shift);
        }
        else
        {
            // occupies either 2 full limbs, or 1 full middle limb and 2 partial limbs
            const uint32_t middle_element_index = top_element_index - 1;
            const uint32_t middle_shift = top_shift - static_cast<uint32_t>(limb_bits); // Left

            mantissa =
                (static_cast<uint_type>(integer_value.bits[top_element_index] & top_mask) << top_shift) +
                (static_cast<uint_type>(integer_value.bits[middle_element_index]) << middle_shift) +
                (static_cast<uint_type>(integer_value.bits[bottom_element_index] & bottom_mask) >> bottom_shift);
        }

        exponent += static_cast<int>(bottom_element_index * limb_bits + top_element_bits);

        has_zero_tail = has_zero_tail && (integer_value.bits[bottom_element_index] & top_mask) == 0;
        for (uint32_t i = 0; has_zero_tail && i != bottom_element_index; ++i)
        {
            has_zero_tail = integer_value.bits[i] == 0;
        }
    }

    return assemble_float_shifted(mantissa, exponent, is_negative, has_zero_tail, value);
}

template <typename C>
struct float_digit_stream
{
    const C* int_first;
    uint32_t int_digit_count;

    const C* frac_first;
    uint32_t frac_digit_count;

    size_t n;

    constexpr uint32_t total_digits() const noexcept
    {
        return int_digit_count + frac_digit_count;
    }

    constexpr C get_char(size_t i) const noexcept
    {
        return i < int_digit_count ? int_first[i] : frac_first[i - int_digit_count];
    }

    constexpr auto get_digit(size_t i) const noexcept
    {
        const auto c = get_char(i);
        return hex::value(c);
    }

    constexpr auto next_digit() noexcept
    {
        return get_digit(n++);
    }
};

template <typename F, typename C>
constexpr from_string_error string_to_float_decimal(float_digit_stream<C>& stream, F& value, bool is_negative, int exponent, bool has_zero_tail) noexcept
{
    using read_traits = float_reading_traits<F>;
    using traits = typename read_traits::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = typename read_traits::limb_type;
    using big_int_type = typename read_traits::big_int_type;
    constexpr uint32_t required_precision_bits = static_cast<uint32_t>(read_traits::required_precision_bits);

    // The input is of the form 0.mantissa * 10^exponent, where 'mantissa' are the decimal digits of the mantissa
    // and 'exponent' is the decimal exponent. We decompose the mantissa into two parts: an integer part and a
    // fractional part. If the exponent is positive, then the integer part consists of the first 'exponent' digits,
    // or all present digits if there are fewer digits. If the exponent is zero or negative, then the integer part
    // is empty. In either case, the remaining digits form the fractional part of the mantissa.
    const uint32_t total_digits = stream.total_digits();
    const uint32_t positive_exponent = static_cast<uint32_t>(std::max(0, exponent));
    const uint32_t int_digit_count = std::min(positive_exponent, total_digits);
    const uint32_t int_digits_missing = positive_exponent - int_digit_count;
    const uint32_t frac_digit_count = total_digits - int_digit_count;

    // Type-specific coarse exponent bounds, computed at compile time.
    // Wider than the type's exact representable range by a small margin,
    // since exp_adjustment reflects only the leading digit's position —
    // the precise overflow/underflow check happens later once the
    // mantissa is assembled.

    big_int_type int_value{};
    for (uint32_t i = 0; i < int_digit_count; ++i)
    {
        const auto digit = static_cast<limb_type>(stream.next_digit());
        int_value.insert_digit(digit);
    }

    if (int_digits_missing > 0)
    {
        if (!int_value.mul_pow10_safe(int_digits_missing))
        {
            assemble_float_infinity(is_negative, value);
            return from_string_error::out_of_range;
        }
    }

    // At this point `mantissa` holds the full integer part's value. If either
    // [1] this already has more than the required bits of precision, or
    // [2] there is no fractional part at all (nothing was trimmed away that
    //     could still matter),
    // then the fractional digits can't change the result — assemble now.
    const uint32_t int_precision_bits = int_value.bit_width();
    {
        const bool has_zero_fractional_part = frac_digit_count == 0 && has_zero_tail;
        if (int_precision_bits >= required_precision_bits || has_zero_fractional_part)
        {
            return assemble_float_integer(int_value, int_precision_bits, is_negative, !has_zero_fractional_part, value);
        }
    }

    // Otherwise, we did not get enough bits of precision from the integer part, and the mantissa has a fractional
    // part. We parse the fractional part of the mantissa to obtain more bits of precision. To do this, we convert
    // the fractional part into an actual fraction N/M, where the numerator N is computed from the digits of the
    // fractional part, and the denominator M is computed as the power of 10 such that N/M is equal to the value
    // of the fractional part of the mantissa.
    big_int_type frac_numerator{};
    for (uint32_t i = 0; i < frac_digit_count; ++i)
    {
        const auto digit = static_cast<limb_type>(stream.next_digit());
        frac_numerator.insert_digit(digit);
    }

    const uint32_t frac_denominator_exponent = (exponent < 0)
        ? frac_digit_count + static_cast<uint32_t>(-exponent)
        : frac_digit_count;

    const uint32_t frac_denominator_bit_count = pow10_bit_width(frac_denominator_exponent);
    if (frac_denominator_bit_count > big_int_type::bit_count)
    {
        // If there were any digits in the integer part, it is impossible to underflow (because the exponent
        // cannot possibly be small enough), so if we underflow here it is a true underflow and we return zero.
        assemble_float_zero(is_negative, value);
        return from_string_error::out_of_range; // Underflow example: "1e-2000"
    }

    // Because we are using only the fractional part of the mantissa here, the numerator is guaranteed to be smaller
    // than the denominator. We normalize the fraction such that the most significant bit of the numerator is in the
    // same position as the most significant bit in the denominator. This ensures that when we later shift the
    // numerator N bits to the left, we will produce N bits of precision.
    const uint32_t frac_numerator_bit_count = frac_numerator.bit_width();

    const uint32_t fractional_shift = (frac_denominator_bit_count > frac_numerator_bit_count)
        ? frac_denominator_bit_count - frac_numerator_bit_count
        : 0;

    const uint32_t required_frac_precision_bits = required_precision_bits - int_precision_bits;

    uint32_t remaining_precision_bits_needed = required_frac_precision_bits;
    if (int_precision_bits > 0)
    {
        // If the fractional part of the mantissa provides no bits of precision and cannot affect rounding,
        // we can just take whatever bits we got from the integer part of the mantissa. This is the case for numbers
        // like 5.0000000000000000000001, where the significant digits of the fractional part start so far to the
        // right that they do not affect the floating-point representation.

        // If the fractional shift is exactly equal to the number of bits of precision that we require,
        // then no fractional bits will be part of the result, but the result may affect rounding.
        // This is e.g. the case for large, odd integers with a fractional part greater than or equal to .5.
        // Thus, we need to do the division to correctly round the result.
        if (fractional_shift > remaining_precision_bits_needed)
        {
            return assemble_float_integer(int_value, int_precision_bits, is_negative, frac_digit_count != 0 || !has_zero_tail, value);
        }

        remaining_precision_bits_needed -= fractional_shift;
    }

    frac_numerator.shl(fractional_shift + remaining_precision_bits_needed);

    float_divider<F> div{ frac_numerator };
    const bool zero_remainder = div.div_pow10(frac_denominator_exponent);
    has_zero_tail = has_zero_tail && zero_remainder;

    // We may have produced more bits of precision than were required. Check, and remove any "extra" bits:
    uint_type frac_mantissa = frac_numerator.extract_low_bits<uint_type>();
    const uint32_t frac_mantissa_bit_count = bit::bit_width(frac_mantissa);

    // The fractional exponent is the power of two by which we must multiply the fractional part to move it into the
    // range [1.0, 2.0). If the post-division quotient needed more bits than we asked for, the normalized fraction
    // was in [1.0, 2.0) already (fractional_shift); otherwise it was in [0.5, 1.0) (fractional_shift + 1).
    // This is equivalent to the discarded frac_numerator < frac_denominator comparison, read off the quotient instead.
    const uint32_t fractional_exponent = (frac_mantissa_bit_count > remaining_precision_bits_needed)
        ? fractional_shift
        : fractional_shift + 1;

    if (frac_mantissa_bit_count > required_frac_precision_bits)
    {
        const uint32_t shift = frac_mantissa_bit_count - required_frac_precision_bits;
        has_zero_tail = has_zero_tail && (frac_mantissa & ((1ULL << shift) - 1)) == 0;
        frac_mantissa >>= shift;
    }

    // Compose the mantissa from the integer and fractional parts:
    const uint_type int_mantissa = int_value.extract_low_bits<uint_type>();
    const uint_type complete_mantissa = (int_mantissa << required_frac_precision_bits) + frac_mantissa;

    // Compute the final exponent:
    // * If the mantissa had an integer part, then the exponent is one less than the number of bits we obtained
    // from the integer part. (It's one less because we are converting to the form 1.11111,
    // with one 1 to the left of the decimal point.)
    // * If the mantissa had no integer part, then the exponent is the fractional exponent that we computed.
    // Then, in both cases, we subtract an additional one from the exponent,
    // to account for the fact that we've generated an extra bit of precision, for use in rounding.
    const int final_exponent = (int_precision_bits > 0)
        ? static_cast<int>(int_precision_bits - 2)
        : -static_cast<int>(fractional_exponent) - 1;

    return assemble_float_shifted(static_cast<uint_type>(complete_mantissa), final_exponent, is_negative, has_zero_tail, value);
}

template <typename F, typename C>
constexpr from_string_error string_to_float_hex(float_digit_stream<C>& stream, F& value, bool is_negative, int initial_exponent, bool has_zero_tail) noexcept
{
    using read_traits = float_reading_traits<F>;
    using traits = typename read_traits::traits;
    using uint_type = typename traits::uint_type;

    uint_type mantissa = 0;
    int exponent = initial_exponent + traits::mantissa_bits;

    // Accumulate bits into the mantissa buffer
    size_t total_digits = stream.total_digits() + 1;
    while (--total_digits && mantissa <= traits::mantissa_with_implicit_bit_mask)
    {
        mantissa *= 16;
        mantissa += stream.next_digit();
        exponent -= 4; // The exponent is in binary; log2(16) == 4
    }

    while (has_zero_tail && total_digits)
    {
        has_zero_tail = stream.next_digit() == 0;
    }

    return assemble_float_shifted(mantissa, exponent, is_negative, has_zero_tail, value);
}

template <typename F, typename C>
constexpr from_string_result parse_infinity(const C* const str, size_t str_size, F& value, const bool is_negative, size_t i) noexcept
{
    constexpr C nfinity[] = { 'n', 'f', 'i', 'n', 'i', 't', 'y' };

    // pre: _Next points at 'i' (case-insensitively)
    if (!starts_with_case_insensitive(str + i + 1, str_size - i - 1, nfinity, 2))
    {
        // definitely invalid
        return { 0, from_string_error::invalid_argument };
    }

    // definitely inf
    i += 3;

    if (starts_with_case_insensitive(str + i, str_size - i, nfinity + 2, 5))
    {
        // definitely infinity
        i += 5;
    }

    assemble_float_infinity(is_negative, value);
    return { i, from_string_error::none };
}

template <typename F, typename C>
constexpr from_string_result parse_nan(const C* const str, size_t str_size, F& value, bool is_negative, size_t i) noexcept
{
    constexpr C snan[] = { 's', 'n', 'a', 'n' };

    // pre: str + i points at 'n' (case-insensitively)
    if (!starts_with_case_insensitive(str + i + 1, str_size - i - 1, snan + 2, 2))
    {
        return { 0, from_string_error::invalid_argument };
    }

    // definitely nan
    i += 3;

    bool is_quiet = true;

    if (i != str_size && str[i] == static_cast<C>('('))
    {
        // possibly nan(n-char-sequence[opt])
        constexpr C ind[] = { 'i', 'n', 'd' };

        const size_t seq_begin = i + 1;
        size_t j = seq_begin;

        for (; j != str_size; ++j)
        {
            if (str[j] == static_cast<C>(')'))
            {
                // definitely nan(n-char-sequence[opt])
                const size_t seq_len = j - seq_begin;
                i = j + 1;

                if (seq_len == 3 && starts_with_case_insensitive(str + seq_begin, seq_len, ind, 3))
                {
                    // definitely nan(ind)
                    // The UCRT considers indeterminate NaN to be negative quiet NaN with no payload bits set.
                    // It parses "nan(ind)" and "-nan(ind)" identically.
                    is_negative = true;
                }
                else if (seq_len == 4 && starts_with_case_insensitive(str + seq_begin, seq_len, snan, 4))
                {
                    // definitely nan(snan)
                    is_quiet = false;
                }

                break;
            }

            const C ch = str[j];
            const bool is_nchar = (ch == static_cast<C>('_')) ||
                (ch >= static_cast<C>('0') && ch <= static_cast<C>('9')) ||
                (ch >= static_cast<C>('A') && ch <= static_cast<C>('Z')) ||
                (ch >= static_cast<C>('a') && ch <= static_cast<C>('z'));

            if (!is_nchar)
            {
                // definitely nan, not nan(n-char-sequence[opt])
                break;
            }
            // else: possibly nan(n-char-sequence[opt]), keep going
        }
    }

    // Intentional behavior difference between the UCRT and this implementation:
    // strtod()/strtof() parse plain "nan" as a quiet NaN with all payload bits set.
    // numeric_limits::quiet_NaN() returns a quiet NaN with no payload bits set.
    // This implementation chooses to be consistent with numeric_limits.
    assemble_float_nan(is_negative, is_quiet, value);
    return { i, from_string_error::none };
}

template <typename F, typename C>
constexpr from_string_result parse_float_impl(const C* str, size_t str_size, F& value, const float_from_string_format_options<C>& fmt, bool is_negative, size_t i) noexcept
{
    using traits = typename float_bits<F>::traits;
    constexpr size_t max_mantissa_digits = float_reading_traits_base::max_decimal_digits;

    const bool is_hex = fmt.format == float_format::hex;
    const int base{ is_hex ? 16 : 10 };

    const size_t digit_parse_start = i;
    size_t digit_count = 0;
    bool has_zero_tail = true;

    // int part
    //===========================================

    while (i < str_size && str[i] == C('0'))
    {
        ++i;
    }

    const size_t int_digit_start = i;
    while (i < str_size)
    {
        const auto digit = to_digit(str[i]);
        if (digit >= base)
        {
            break;
        }

        if (digit_count < max_mantissa_digits)
        {
            ++digit_count;
        }
        else
        {
            has_zero_tail = has_zero_tail && (str[i] == C('0'));
        }

        ++i;
    }

    size_t int_digit_count = i - int_digit_start;

    // fractional part
    //===========================================

    size_t frac_digit_start = i;
    size_t frac_digit_count = 0;
    size_t frac_leading_zero_count = 0;
    bool has_decimal_point = false;

    if (i < str_size && str[i] == fmt.decimal_point)
    {
        ++i;
        has_decimal_point = true;
        ++frac_digit_start;

        // Only trim fractional leading zeros if the integer part had
        // no significant digits (e.g. "0.000123" or ".000123").
        if (int_digit_count == 0)
        {
            while (i < str_size && str[i] == C('0'))
            {
                ++i;
            }
        }

        frac_leading_zero_count = i - frac_digit_start;
        frac_digit_start = i;

        while (i < str_size)
        {
            const auto digit = to_digit(str[i]);
            if (digit >= base)
            {
                break;
            }

            if (digit_count < max_mantissa_digits)
            {
                ++digit_count;
            }
            else
            {
                has_zero_tail = has_zero_tail && (str[i] == C('0'));
            }

            ++i;
        }

        frac_digit_count = digit_count - int_digit_count;
    }

    //===========================================

    // Total characters consumed so far, minus the decimal point itself
    // (if present), tells us whether any digit was actually seen —
    // independent of whether trimming later reduced the significant
    // digit counts to zero (e.g. "0", "0.0", "00.000").
    const size_t chars_consumed = i - digit_parse_start;
    const bool saw_any_digit = chars_consumed > static_cast<size_t>(has_decimal_point);
    if (!saw_any_digit)
    {
        return { 0, from_string_error::invalid_argument };
    }

    // exponent
    //===========================================

    // The exponent adjustment holds the number of digits in the mantissa buffer that appeared before the radix point.
    // It can be negative, and leading zeroes in the integer part are ignored. Examples:
    // For "03333.111", it is 4.
    // For "00000.111", it is 0.
    // For "00000.001", it is -2.
    int exponent_adjustment = (int_digit_count > 0)
        ? static_cast<int>(int_digit_count)
        : -static_cast<int>(frac_leading_zero_count);

    const C exponent_prefix = static_cast<C>(is_hex ? 'p' : 'e');
    int exponent = 0;
    bool exponent_is_negative = false;
    bool exponent_overflow = false;

    const size_t exponent_scan_start = i;

    if (fmt.format != float_format::fixed && i < str_size && str::to_lower_ascii_unchecked(str[i]) == exponent_prefix)
    {
        // Lookahead index: only committed to `i` once each digit is actually
        // confirmed, so "1.5e" or "1.5e+" with nothing after leaves the
        // prefix/sign unconsumed (matches from_chars semantics).
        size_t j = i + 1;

        if (j < str_size && (str[j] == C('-') || str[j] == C('+')))
        {
            exponent_is_negative = (str[j] == C('-'));
            ++j;
        }

        while (j < str_size)
        {
            const auto digit = (str[j] - C('0'));
            if (digit >= 10)
            {
                break;
            }

            if (exponent < INT_MAX / 10 || (exponent == INT_MAX / 10 && digit <= INT_MAX % 10))
            {
                exponent = exponent * 10 + digit;
            }
            else
            {
                exponent_overflow = true;
            }

            ++j;
        }

        i = j;

        if (exponent_is_negative)
        {
            exponent = -exponent;
        }
    }

    // scientific format requires an exponent to be present
    if (fmt.format == float_format::scientific && i == exponent_scan_start)
    {
        return { 0, from_string_error::invalid_argument };
    }

    //===========================================

    // Trim trailing zeros
    {
        // Trim trailing zeros from the fractional span first...
        while (frac_digit_count > 0 && str[frac_digit_start + frac_digit_count - 1] == C('0'))
        {
            --frac_digit_count;
        }

        // ...then, only if the fractional part vanished entirely (absent, or
        // turned out to be all zeros), cascade the trim into the tail of the
        // integer span too -- "100" and "100.00" both need to end up with a
        // single significant digit. "100.5" must NOT hit this branch, since
        // frac_digit_count is 1 there (the "00" in "100" stays untouched --
        // those are internal digits, not trailing ones).
        if (frac_digit_count == 0)
        {
            while (int_digit_count > 0 && str[int_digit_start + int_digit_count - 1] == C('0'))
            {
                --int_digit_count;
            }
        }

        // Zero fast-path, deferred until after exponent scanning so the reported
        // position is correct for inputs like "0e999" or "0.0e-5".
        const size_t total_digit_count = int_digit_count + frac_digit_count;
        if (total_digit_count == 0)
        {
            _string_convert_priv::assemble_float_zero(is_negative, value);
            return { i, from_string_error::none };
        }
    }

    //===========================================

    if (exponent_overflow)
    {
        if (exponent_is_negative)
        {
            _string_convert_priv::assemble_float_zero(is_negative, value);
        }
        else
        {
            _string_convert_priv::assemble_float_infinity(is_negative, value);
        }

        return { i, from_string_error::out_of_range };
    }

    // Adjust exponent and exponent_adjustment when they have different signedness to avoid overflow.
    if (exponent > 0 && exponent_adjustment < 0)
    {
        if (is_hex)
        {
            const int further_adjustment = std::max(-((exponent - 1) / 4 + 1), exponent_adjustment);
            exponent += further_adjustment * 4;
            exponent_adjustment -= further_adjustment;
        }
        else
        {
            const int further_adjustment = std::max(-exponent, exponent_adjustment);
            exponent += further_adjustment;
            exponent_adjustment -= further_adjustment;
        }
    }
    else if (exponent < 0 && exponent_adjustment > 0)
    {
        if (is_hex)
        {
            const int further_adjustment = std::min((-exponent - 1) / 4 + 1, exponent_adjustment);
            exponent += further_adjustment * 4;
            exponent_adjustment -= further_adjustment;
        }
        else
        {
            const int further_adjustment = std::min(-exponent, exponent_adjustment);
            exponent += further_adjustment;
            exponent_adjustment -= further_adjustment;
        }
    }

    //===========================================

    constexpr int maximum_temporary_decimal_exponent = 5200;
    constexpr int minimum_temporary_decimal_exponent = -5200;

    // In hex floating constants the exponent suffix ('p'/'P') is a base-2
    // exponent, while `position_exponent` is in units of hex digits
    // (base 16). Scale by log2(16) == 4 so the two are combinable.
    const int exponent_adjustment_multiplier = is_hex ? 4 : 1;

    // And then exponent and exponent_adjustment are either both non-negative or both non-positive.
    // So we can detect out-of-range cases directly.
    {
        if (exponent > maximum_temporary_decimal_exponent ||
            exponent_adjustment > maximum_temporary_decimal_exponent / exponent_adjustment_multiplier)
        {
            _string_convert_priv::assemble_float_infinity(is_negative, value);
            return { i, from_string_error::out_of_range }; // Overflow example: "1e+9999"
        }

        if (exponent < minimum_temporary_decimal_exponent ||
            exponent_adjustment < minimum_temporary_decimal_exponent / exponent_adjustment_multiplier)
        {
            _string_convert_priv::assemble_float_zero(is_negative, value);
            return { i, from_string_error::out_of_range }; // Underflow example: "1e-9999"
        }
    }

    exponent += exponent_adjustment * exponent_adjustment_multiplier;
    {
        if (exponent > maximum_temporary_decimal_exponent)
        {
            _string_convert_priv::assemble_float_infinity(is_negative, value);
            return { i, from_string_error::out_of_range };
        }

        if (exponent < minimum_temporary_decimal_exponent)
        {
            _string_convert_priv::assemble_float_zero(is_negative, value);
            return { i, from_string_error::out_of_range };
        }
    }

    //===========================================

    float_digit_stream<C> stream{
        str + int_digit_start,
        static_cast<uint32_t>(int_digit_count),
        str + frac_digit_start,
        static_cast<uint32_t>(frac_digit_count),
        0
    };

    from_string_error err;
    if (is_hex)
    {
        err = string_to_float_hex(stream, value, is_negative, exponent, has_zero_tail);
    }
    else
    {
        err = string_to_float_decimal(stream, value, is_negative, exponent, has_zero_tail);
    }

    return { i, err };
}

} // namespace _string_convert_priv

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr from_string_result parse_float(const C* str, size_t str_size, F& value, const float_from_string_format_options<C>& fmt = {}) noexcept
{
    if (str_size == 0)
    {
        return { 0, from_string_error::invalid_argument };
    }

    size_t i = 0;
    bool is_negative = false;

    if (str[0] == C('-') || str[0] == C('+'))
    {
        if (str_size == 1)
        {
            return { 0, from_string_error::invalid_argument };
        }

        ++i;
        is_negative = str[0] == C('-');
    }

    // Distinguish ordinary numbers versus inf/nan with a single test.
    // ordinary numbers start with ['.'] ['0', '9'] ['A', 'F'] ['a', 'f']
    // inf/nan start with ['I'] ['N'] ['i'] ['n']
    // All other starting characters are invalid.
    // Setting the 0x20 bit folds these ranges in a useful manner.
    // ordinary (and some invalid) starting characters are folded to ['.'] ['0', '9'] ['a', 'f']
    // inf/nan starting characters are folded to ['i'] ['n']
    // These are ordered: ['.'] ['0', '9'] ['a', 'f'] < ['i'] ['n']
    // Note that invalid starting characters end up on both sides of this test.
    const C folded_start = str::to_lower_ascii_unchecked(str[i]);

    if (folded_start <= 'f')
    {
        // possibly an ordinary number
        return _string_convert_priv::parse_float_impl(str, str_size, value, fmt, is_negative, i);
    }
    if (folded_start == 'i')
    {
        // possibly inf
        return _string_convert_priv::parse_infinity(str, str_size, value, is_negative, i);
    }
    if (folded_start == 'n')
    {
        // possibly nan
        return _string_convert_priv::parse_nan(str, str_size, value, is_negative, i);
    }

    // definitely invalid
    return { 0, from_string_error::invalid_argument };
}

//==============================================================================
// to string
//==============================================================================

template <typename I, typename C, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result to_string(I value, C* buf, size_t buf_size, const integer_format_options& fmt = {}) noexcept
{
    return write_integer(value, buf, buf_size, fmt);
}

template <typename I, typename S, VX_REQUIRES(std::is_integral<I>::value&& is_mutable_string_like<S>::value)>
to_string_result to_string(I value, S& out, const integer_format_options& fmt = {}) noexcept
{
    using C = typename S::value_type;

    using U = typename std::make_unsigned<I>::type;
    U uvalue = static_cast<U>(value);
    const U ubase = static_cast<U>(fmt.base);
    char sign = fmt.force_sign ? '+' : 0;

    VX_IF_CONSTEXPR (std::is_signed<I>::value)
    {
        if (value < 0)
        {
            uvalue = ~uvalue + 1;
            sign = '-';
        }
    }

    size_t count = _string_convert_priv::digit_count(uvalue, ubase) + static_cast<size_t>(sign);
    out.resize(count, 0);

    do
    {
        const char c = _string_convert_priv::int_digits[uvalue % ubase];
        out[--count] = fmt.uppercase ? to_upper(c) : c;
        uvalue /= ubase;

    } while (uvalue);

    if (sign)
    {
        out[0] = static_cast<C>(sign);
    }

    return to_string_result{ out.size(), to_string_error::none };
}

//==============================================================================

template <typename F, typename C, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result to_string(F value, C* buf, size_t buf_size, const float_to_string_format_options<C>& fmt = {}) noexcept
{
    return write_float(value, buf, buf_size, fmt);
}

template <typename F, typename S, VX_REQUIRES(std::is_floating_point<F>::value&& is_mutable_string_like<S>::value)>
to_string_result to_string(F value, S& out, const float_to_string_format_options<typename S::value_type>& fmt = {}) noexcept
{
    using C = typename S::value_type;

    size_t buf_size = 24;
    out.resize(buf_size);

    to_string_result res = to_string<F, C>(value, out.data(), buf_size, fmt);
    if (res.err == to_string_error::precision_too_large)
    {
        return res;
    }

    while (res.err == to_string_error::buffer_too_small)
    {
        buf_size *= 2;
        out.resize(buf_size);
        res = to_string<F, C>(value, out.data(), buf_size, fmt);
    }

    out.shrink_to_fit();
    return res;
}

//==============================================================================
// from string
//==============================================================================

template <typename I, typename C, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
from_string_result from_string(const C* s, size_t size, I& out, const integer_format_options& fmt = {}) noexcept
{
    return parse_integer(s, size, out, fmt);
}

template <typename I, typename C, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
from_string_result from_string(const C* s, I& out, const integer_format_options& fmt = {}) noexcept
{
    const size_t size = str::length(s);
    return parse_integer(s, size, out, fmt);
}

template <typename I, typename S, VX_REQUIRES(std::is_integral<I>::value&& is_string_like<S>::value)>
from_string_result from_string(const S& s, I& out, const integer_format_options& fmt = {}) noexcept
{
    return parse_integer(s.data(), s.size(), out, fmt);
}

//==============================================================================

template <typename F, typename C, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
from_string_result from_string(const C* s, size_t size, F& out, const float_from_string_format_options<C>& fmt = {}) noexcept
{
    return parse_float(s, size, out, fmt);
}

template <typename F, typename C, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
from_string_result from_string(const C* s, F& out, const float_from_string_format_options<C>& fmt = {}) noexcept
{
    const size_t size = str::length(s);
    return parse_float(s, size, out, fmt);
}

template <typename F, typename S, VX_REQUIRES(std::is_floating_point<F>::value&& is_string_like<S>::value)>
from_string_result from_string(const S& s, F& out, const float_from_string_format_options<typename S::value_type>& fmt = {}) noexcept
{
    return parse_float(s.data(), s.size(), out, fmt);
}

} // namespace str
} // namespace vx
