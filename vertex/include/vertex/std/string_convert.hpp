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
// data to hex
//==============================================================================

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
size_t to_hex_string(const void* data, const size_t size, C* buf, const size_t buf_size, const bool upper = false) noexcept
{
    const size_t needed = 2 * size;
    if (!buf || buf_size < needed)
    {
        return 0;
    }

    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    for (size_t i = 0; i < size; ++i)
    {
        const C c1 = hex::digits[(bytes[i] >> 4) & 0xF]; // High nibble
        const C c2 = hex::digits[(bytes[i] >> 0) & 0xF]; // Low nibble

        *buf++ = upper ? to_upper(c1) : c1;
        *buf++ = upper ? to_upper(c2) : c2;
    }

    return needed;
}

template <typename S, VX_REQUIRES(is_mutable_string_like<S>::value)>
size_t to_hex_string(const void* data, const size_t size, S& out) noexcept
{
    using C = typename S::value_type;
    out.resize(2 * size);
    return to_hex_string<C>(data, size, out.data(), out.size());
}

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
str::basic_string<C> to_hex_string(const void* data, const size_t size) noexcept
{
    basic_string<C> res(2 * size, C());
    to_hex_string<C>(data, size, res.data(), res.size());
    return res;
}

//==============================================================================
// integer format specifiers
//==============================================================================

struct integer_format_options
{
    uint32_t base = 10;
    bool uppercase = false;
    bool force_sign = false;
};

//==============================================================================
// integer printing
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int char_to_digit(const C c, int base = 10) noexcept
{
    int digit = -1;

    if (c >= static_cast<C>('0') && c <= static_cast<C>('9'))
    {
        digit = static_cast<int>(c - static_cast<C>('0'));
    }
    else if (c >= static_cast<C>('A') && c <= static_cast<C>('Z'))
    {
        digit = static_cast<int>(c - static_cast<C>('A')) + 10;
    }
    else if (c >= static_cast<C>('a') && c <= static_cast<C>('z'))
    {
        digit = static_cast<int>(c - static_cast<C>('a')) + 10;
    }

    return (digit < base) ? digit : -1;
}

//==============================================================================
// to string
//==============================================================================

namespace _string_convert_priv {

inline constexpr size_t digit_count_unsigned(uint8_t v) noexcept
{
    // clang-format off
    if (v >= 100) return 3;
    if (v >= 10)  return 2;
    return 1;
    // clang-format on
}

inline constexpr size_t digit_count_unsigned(uint16_t v) noexcept
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

inline constexpr size_t digit_count_unsigned(uint64_t v) noexcept
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
constexpr size_t digit_count(I value, int base) noexcept
{
    size_t n = 0;

    do
    {
        value /= static_cast<I>(base);
        ++n;

    } while (value != static_cast<I>(0));

    return n;
}

} // namespace _string_convert_priv

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
size_t write_integer(I value, C* buf, const size_t buf_size)
{
    if (!buf)
    {
        return 0;
    }

    using U = typename std::make_unsigned<I>::type;
    U uvalue = static_cast<U>(value);
    bool negative = false;

    VX_IF_CONSTEXPR (std::is_signed<I>::value)
    {
        negative = (value < 0);
        if (negative)
        {
            uvalue = ~uvalue + 1;
        }
    }

    const size_t digit_count = _string_convert_priv::digit_count_unsigned(uvalue);
    const size_t needed = digit_count + static_cast<size_t>(negative);
    if (buf_size < needed)
    {
        return 0;
    }

    size_t i = needed;

    do
    {
        const auto digit = uvalue % 10;
        buf[--i] = static_cast<C>('0' + digit);
        uvalue /= 10;

    } while (uvalue);

    if (negative)
    {
        buf[0] = static_cast<C>('-');
    }

    return needed;
}

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
size_t write_integer(I value, C* buf, const size_t buf_size, const integer_format_options& fmt)
{
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

    constexpr const char digits[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y', 'z'
    };

    if (!buf)
    {
        return 0;
    }

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
        return 0;
    }

    size_t i = needed;

    do
    {
        const char c = digits[uvalue % ubase];
        buf[--i] = fmt.uppercase ? to_upper(c) : c;
        uvalue /= ubase;

    } while (uvalue);

    if (sign)
    {
        buf[0] = static_cast<C>(sign);
    }

    return needed;
}

//==============================================================================
// float format specifiers
//==============================================================================

enum class float_format : char
{
    fixed = 'f',
    scientific = 'e',
    general = 'g',
    hex = 'a',
};

namespace _string_convert_priv {

enum : uint32_t
{
    precision_unspecified = std::numeric_limits<uint32_t>::max(),

    float_fixed_default = 6,
    float_scientific_default = 6,
    float_general_default = 6,

    float_hex_default = 6,
    double_hex_default = 13,

    max_float_precision = 1'000'000
};

template <typename F>
constexpr uint32_t resolve_precision(uint32_t format, uint32_t precision) noexcept
{
    if (precision == precision_unspecified)
    {
        switch (format)
        {
            case float_format::fixed: return float_fixed_default;
            case float_format::scientific: return float_scientific_default;
            case float_format::general: return float_general_default;
            case float_format::hex:
            {
                VX_IF_CONSTEXPR (std::is_same<F, float>::value)
                {
                    return float_hex_default;
                }
                VX_IF_CONSTEXPR (std::is_same<F, double>::value)
                {
                    return double_hex_default;
                }
            }
        }
    }

    return (precision > max_float_precision)
        ? max_float_precision
        : precision;
}

} // namespace _string_convert_priv

template <typename C = char>
struct float_format_options
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

//==============================================================================
// general helpers
//==============================================================================

// https://github.com/microsoft/STL/blob/f3ae96af460b8fcb7d77c46fc1ad7d312900d1e7/stl/inc/xcharconv_ryu.h#L2390

namespace _string_convert_priv {

template <typename I>
constexpr size_t digit_count_max3(I value) noexcept
{
    if (value < static_cast<I>(10))
    {
        return 1;
    }
    if (value < static_cast<I>(100))
    {
        return 2;
    }

    return 3;
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

template <typename limb_type_, uint32_t limb_count_, typename wide_type_>
struct big_int
{
    using limb_type = limb_type_;
    using wide_type = wide_type_;
    static constexpr uint32_t limb_count = limb_count_;
    static constexpr size_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    limb_type bits[limb_count_];

    inline constexpr big_int() noexcept
        : bits{}
    {}

    inline constexpr big_int(const limb_type x) noexcept
        : bits{ x }
    {
    }

    inline constexpr big_int(const wide_type x) noexcept
        : bits{ static_cast<limb_type>(x), static_cast<limb_type>(x >> limb_bits) }
    {
    }

    inline constexpr big_int(limb_type m, int shift) noexcept
        : bits{}
    {
        const uint32_t base_limb = shift / limb_bits;
        const uint32_t bit_offset = shift % limb_bits;

        // 1. Store into the first limb
        bits[base_limb] |= static_cast<limb_type>(m << bit_offset);

        // 2. Overflow into the second limb
        if (base_limb + 1 < limb_count)
        {
            bits[base_limb + 1] |= static_cast<limb_type>(m >> (limb_bits - bit_offset));
        }
    }

    inline constexpr big_int(wide_type m, int shift) noexcept
        : bits{}
    {
        const uint32_t base_limb = shift / limb_bits;
        const uint32_t bit_offset = shift % limb_bits;

        // 1. Store into the first limb
        bits[base_limb] |= static_cast<limb_type>(m << bit_offset);

        // 2. Overflow into the second limb
        if (base_limb + 1 < limb_count)
        {
            bits[base_limb + 1] |= static_cast<limb_type>(m >> (limb_bits - bit_offset));
        }

        // Total bits available in the first two limbs starting from our current bit_offset
        constexpr uint32_t two_limbs_bits = limb_bits * 2;
        constexpr uint32_t wide_bits = sizeof(wide_type) * CHAR_BIT;

        // 3. Overflow into the third limb (Optimized away at compile-time for 32-bit floats)
        if ((bit_offset + wide_bits > two_limbs_bits) && (base_limb + 2 < limb_count))
        {
            bits[base_limb + 2] |= static_cast<limb_type>(m >> (two_limbs_bits - bit_offset));
        }
    }

    inline constexpr limb_type mul(limb_type x) noexcept
    {
        limb_type carry = 0;

        for (size_t j = 0; j < limb_count; ++j)
        {
            // Fold multiplication and carry into a single wide operation
            const wide_type product = (static_cast<wide_type>(bits[j]) * static_cast<wide_type>(x)) + static_cast<wide_type>(carry);

            bits[j] = static_cast<limb_type>(product);
            carry = static_cast<limb_type>(product >> limb_bits);
        }

        return carry;
    }

    inline constexpr limb_type extract_digit(uint32_t limb, uint32_t partial_bits) noexcept
    {
        const limb_type lo = bits[limb] >> partial_bits;
        const limb_type hi = bits[limb + 1] << (limb_bits - partial_bits);
        return lo | hi;
    }

    inline constexpr limb_type mul_extract(limb_type x, uint32_t limb, uint32_t partial_bits) noexcept
    {
        mul(x);
        return extract_digit(limb, partial_bits);
    }

    inline constexpr void mul_clear(limb_type x, uint32_t limb, limb_type digit_mask) noexcept
    {
        mul(x);
        bits[limb] &= digit_mask;
        bits[limb + 1] = 0;
    }

    inline constexpr limb_type mul_extract_clear(limb_type x, uint32_t limb, uint32_t partial_bits, limb_type digit_mask) noexcept
    {
        const limb_type digit = mul_extract(x, limb, partial_bits);
        bits[limb] &= digit_mask;
        bits[limb + 1] = 0;
        return digit;
    }

    inline constexpr void mul_shave_digits(size_t count, uint32_t limb, limb_type digit_mask) noexcept
    {
        while (count >= 9)
        {
            mul_clear(1000000000u, limb, digit_mask);
            count -= 9;
        }

        if (count)
        {
            mul_clear(pow10_u32(count), limb, digit_mask);
        }
    }

    inline constexpr limb_type extract_digit_lower(uint32_t limb, uint32_t partial_bits) noexcept
    {
        return bits[limb] >> partial_bits;
    }

    inline constexpr limb_type mul_extract_lower(limb_type x, uint32_t limb, uint32_t partial_bits) noexcept
    {
        mul(x);
        return extract_digit_lower(limb, partial_bits);
    }

    inline constexpr void mul_clear_lower(limb_type x, uint32_t limb, limb_type digit_mask) noexcept
    {
        mul(x);
        bits[limb] &= digit_mask;
    }

    inline constexpr limb_type mul_extract_clear_lower(limb_type x, uint32_t limb, uint32_t partial_bits, limb_type digit_mask) noexcept
    {
        const limb_type digit = mul_extract_lower(x, limb, partial_bits);
        bits[limb] &= digit_mask;
        return digit;
    }

    inline constexpr void mul_shave_digits_lower(size_t count, uint32_t limb, limb_type digit_mask) noexcept
    {
        while (count >= 9)
        {
            mul_clear_lower(1000000000u, limb, digit_mask);
            count -= 9;
        }

        if (count)
        {
            mul_clear_lower(pow10_u32(count), limb, digit_mask);
        }
    }

    inline constexpr limb_type div_extract(limb_type x, size_t start_index) noexcept
    {
        constexpr uint32_t half = limb_bits / 2;
        constexpr limb_type half_mask = (limb_type{ 1 } << half) - limb_type{ 1 };

        limb_type remainder = 0;

        for (size_t i = start_index + 1; i-- > 0;)
        {
            const limb_type cur_hi = bits[i] >> half;
            const limb_type cur_lo = bits[i] & half_mask;

            const wide_type d1 = (static_cast<wide_type>(remainder) << half) | cur_hi;
            const limb_type q1 = static_cast<limb_type>(d1 / x);
            const limb_type r1 = static_cast<limb_type>(d1 % x);

            const wide_type d2 = (static_cast<wide_type>(r1) << half) | cur_lo;
            const limb_type q2 = static_cast<limb_type>(d2 / x);
            remainder = static_cast<limb_type>(d2 % x);

            bits[i] = (q1 << half) | q2;
        }

        return remainder;
    }

    inline constexpr void div_shrink(limb_type x, size_t& max_index) noexcept
    {
        div_extract(x, max_index);

        while (bits[max_index] == 0)
        {
            --max_index;
        }
    }

    inline constexpr limb_type div_shrink_extract(limb_type x, size_t& max_index) noexcept
    {
        const limb_type remainder = div_extract(x, max_index);

        while (bits[max_index] == 0)
        {
            --max_index;
        }

        return remainder;
    }

    inline constexpr void div_shave_digits(size_t count, size_t& max_index) noexcept
    {
        while (count >= 9)
        {
            div_shrink(1000000000u, max_index);
            count -= 9;
        }

        if (count)
        {
            div_shrink(pow10_u32(count), max_index);
        }
    }
};

//==============================================================================
// scientific helpers
//==============================================================================

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

template <typename F>
constexpr uint32_t subnormal_pow10(const typename vx::float_bits<F>::uint_type m_bits) noexcept
{
    // this estimate may be 1 less than the actual power of 10
    using traits = typename vx::float_bits<F>::traits;
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
    const int e10 = -(vx::str::_string_convert_priv::log10_pow5(-e2) + e2) - (e2 > 3);
    return e10;
}

template <typename F>
constexpr int large_integer_pow10(const int e2) noexcept
{
    // this estimate may be 1 less than the actual power of 10
    const int e10 = vx::str::_string_convert_priv::log10_pow2(e2);
    return e10;
}

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
        buf[--i] = static_cast<C>('0' + digit);
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
        buf[--i] = static_cast<C>('0' + digit);
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
constexpr size_t write_scientific_zero(C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    const size_t precision_char_count = static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    const size_t leading_char_count = 1 + precision_char_count;
    const size_t exp_char_count = 1 + static_cast<size_t>(fmt.force_exp_sign) + 1;
    const size_t needed = leading_char_count + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    buf[0] = static_cast<C>('0');

    if (fmt.precision > 0)
    {
        buf[1] = fmt.decimal_point;
        fill_n_zeros(buf + 2, fmt.precision);
    }

    C* const exp_ptr = buf + leading_char_count;

    exp_ptr[0] = static_cast<C>(fmt.uppercase ? 'E' : 'e');
    if (fmt.force_exp_sign)
    {
        exp_ptr[1] = static_cast<C>('+');
    }
    exp_ptr[exp_char_count - 1] = static_cast<C>('0');

    return needed;
}

template <typename F, typename C>
constexpr size_t write_scientific_subnormal(typename float_bits<F>::uint_type m_bits, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    int e10 = subnormal_pow10<F>(m_bits);

    const size_t leading_char_count = 1 + static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    size_t exp_char_count = 1 + 1 + digit_count_max3(e10);

    size_t needed = leading_char_count + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    bool exp_changed = false;

    // For a subnormal, the value is: mantissa * 2^(1 - bias - digits)
    // The LSB is at bit position (bias + digits - 2) below the binary point,
    // so we need (bias + digits - 2) fractional bits.
    constexpr uint32_t max_shift = traits::exponent_bias + traits::digits - 2;
    constexpr uint32_t limb_count = (max_shift + (limb_bits - 1)) / limb_bits;

    constexpr uint32_t keep_limbs = max_shift / limb_bits;
    constexpr uint32_t partial_bits = max_shift % limb_bits;
    constexpr limb_type digit_mask = (limb_type{ 1 } << partial_bits) - 1;

    // our large integer type
    using big_int_type = big_int<limb_type, limb_count, wide_type>;
    big_int_type frac_bits{ m_bits };

    // first find the first non 0 digit
    {
        const size_t leading_zero_count = static_cast<size_t>(e10) - 1;
        frac_bits.mul_shave_digits_lower(leading_zero_count, keep_limbs, digit_mask);
        limb_type digit = frac_bits.mul_extract_clear_lower(10, keep_limbs, partial_bits, digit_mask);

        // we may be short by 1 digit, meaning 1 unaccounted for leading 0
        if (digit == 0)
        {
            digit = frac_bits.mul_extract_clear_lower(10, keep_limbs, partial_bits, digit_mask);

            // out e10 estimate was off by 1
            ++e10;
            exp_changed = true;
        }

        VX_ASSERT(digit != 0);
        buf[0] = static_cast<C>('0' + digit);
    }

    if (fmt.precision > 0)
    {
        buf[1] = fmt.decimal_point;

        C* ptr = buf + 2;
        for (size_t i = 0; i < fmt.precision; ++i)
        {
            const limb_type digit = frac_bits.mul_extract_clear_lower(10, keep_limbs, partial_bits, digit_mask);
            ptr[i] = static_cast<C>('0' + digit);
        }
    }

    if (fmt.round)
    {
        const limb_type round_digit = frac_bits.mul_extract_lower(10, keep_limbs, partial_bits);

        if (round_digit >= 5)
        {
            const bool overflow = scientific_carry_round(buf, leading_char_count);
            if (overflow)
            {
                ++e10;
                exp_changed = true;
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
constexpr size_t write_scientific_normal(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const size_t leading_char_count = 1 + static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    size_t exp_char_count = 1 + 1 + _string_convert_priv::digit_count_max3(e10);
    size_t needed = leading_char_count + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    // we are either at, or 1 more than the actual exponent
    bool exp_changed = false;

    constexpr uint32_t max_shift = traits::exponent_bias + traits::mantissa_bits - 1;
    constexpr uint32_t limb_count = ((max_shift + (limb_bits - 1)) / limb_bits) + 1;

    const uint_type m2 = m_bits | (uint_type{ 1 } << traits::mantissa_bits);

    const uint32_t keep_limbs = shift / limb_bits;
    const uint32_t partial_bits = shift % limb_bits;
    const limb_type digit_mask = (limb_type{ 1 } << partial_bits) - 1;

    // our large integer type
    using big_int_type = big_int<limb_type, limb_count, wide_type>;
    big_int_type frac_bits{ m2 };

    // first find the first non 0 digit
    {
        if (e10 == 1)
        {
            // Our estimate should be accurate when e10 is 1, and this is the most common case.
            const limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
            VX_ASSERT(digit != 0);
            buf[0] = static_cast<C>('0' + digit);
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
            frac_bits.mul_shave_digits(shave, keep_limbs, digit_mask);

            limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);

            if (digit != 0)
            {
                // we over estimated e10 by 1
                --e10;
                exp_changed = true;
            }
            else
            {
                digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
                VX_ASSERT(digit != 0);
            }

            buf[0] = static_cast<C>('0' + digit);
        }
    }

    if (fmt.precision > 0)
    {
        buf[1] = fmt.decimal_point;

        for (size_t i = 0; i < fmt.precision; ++i)
        {
            const limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
            buf[2 + i] = static_cast<C>('0' + digit);
        }
    }

    if (fmt.round)
    {
        const limb_type round_digit = frac_bits.mul_extract(10, keep_limbs, partial_bits);
        if (round_digit >= 5)
        {
            if (scientific_carry_round(buf, leading_char_count))
            {
                ++e10;
                // It is possible to flip flop, so if we decrememnted e10,
                // than incremented, it should go back to the same value.
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
constexpr size_t write_scientific_large(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const size_t leading_char_count = 1 + static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    size_t exp_char_count = 1 + static_cast<size_t>(fmt.force_exp_sign) + digit_count_max3(e10);
    size_t needed = leading_char_count + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    bool exp_changed = false;

    constexpr int max_shift = traits::filled_exponent - traits::exponent_bias - traits::digits;
    constexpr uint32_t total_bits = max_shift + traits::digits;
    constexpr uint32_t limb_count = (total_bits + (limb_bits - 1)) / limb_bits;

    using big_int_type = big_int<limb_type, limb_count, wide_type>;

    const uint_type mantissa = m_bits | (uint_type{ 1 } << traits::mantissa_bits);
    big_int_type int_bits(mantissa, shift);
    size_t max_index = (shift + traits::digits - 1) / limb_bits;

    limb_type round_digit = 0;
    C* back_ptr = buf + leading_char_count;

    const size_t int_digit_count = static_cast<size_t>(e10) + 1;
    const size_t digits_needed = static_cast<size_t>(fmt.precision) + 1;
    size_t zero_fill = 0;

    // first we shave off the digits we don't care about
    if (digits_needed < int_digit_count)
    {
        const size_t trailing_digit_count = int_digit_count - digits_needed;
        int_bits.div_shave_digits(trailing_digit_count - 1, max_index);
        round_digit = int_bits.div_extract(10, max_index);
    }
    else if (digits_needed > int_digit_count)
    {
        // pad with 0s if there are not enough digits
        zero_fill = digits_needed - int_digit_count;
        fill_n_zeros(back_ptr - zero_fill, zero_fill);
        back_ptr -= zero_fill;
    }

    const size_t digits_left = digits_needed - zero_fill;
    for (size_t i = 1; i < digits_left; ++i)
    {
        const limb_type digit = int_bits.div_shrink_extract(10, max_index);
        *(--back_ptr) = static_cast<C>('0' + digit);
    }

    // decimal + final digit
    {
        if (fmt.precision > 0)
        {
            *(--back_ptr) = fmt.decimal_point;
        }
        const limb_type digit = int_bits.div_shrink_extract(10, max_index);
        *(--back_ptr) = static_cast<C>('0' + digit);
    }

    // There may be 1 digit left over in rare cases
    if (int_bits.bits[0] != 0)
    {
        const limb_type digit = int_bits.div_extract(10, 0);
        VX_ASSERT(int_bits.bits[0] == 0);

        round_digit = static_cast<limb_type>(buf[leading_char_count - 1] - '0');

        if (fmt.precision > 0)
        {
            // swap the first digit with the decimal and shift back
            mem::swap(*back_ptr, *(back_ptr + 1));
            mem::move_range(back_ptr + 1, back_ptr, digits_needed); // use digits needed because it includes the decimal
        }

        *back_ptr = static_cast<C>('0' + digit);

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
constexpr size_t write_scientific_mixed(typename float_bits<F>::uint_type m_bits, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const uint_type m2 = m_bits | (uint_type{ 1 } << traits::mantissa_bits);
    const int frac_bit_count = -shift;
    uint_type int_bits = m2 >> frac_bit_count;

    // should be exact
    const size_t int_digit_count = digit_count_unsigned(int_bits);
    int e10 = static_cast<int>(int_digit_count - 1);

    const size_t leading_char_count = 1 + static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    size_t exp_char_count = 1 + static_cast<size_t>(fmt.force_exp_sign) + digit_count_max3(e10);
    size_t needed = leading_char_count + fmt.force_exp_sign + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    bool exp_changed = false;

    const size_t digits_needed = static_cast<size_t>(fmt.precision) + 1;
    const size_t int_digits_needed = (int_digit_count >= digits_needed) ? digits_needed : int_digit_count;
    const size_t frac_digits_needed = digits_needed - int_digits_needed;

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

        C* ptr = buf + int_digits_needed + static_cast<size_t>(fmt.precision > 0);

        // populate integer part
        for (size_t i = 1; i < int_digits_needed; ++i)
        {
            const limb_type digit = int_bits % 10;
            int_bits /= 10;
            *(--ptr) = static_cast<C>('0' + digit);
        }

        // decimal + final digit
        if (fmt.precision > 0)
        {
            *(--ptr) = fmt.decimal_point;
        }
        const limb_type digit = int_bits % 10;
        *(--ptr) = static_cast<C>('0' + digit);
    }

    // frac part + rounding
    if (frac_digits_needed || (int_digits_needed == int_digit_count && fmt.round))
    {
        const uint_type raw_frac_bits = m2 & ((uint_type{ 1 } << frac_bit_count) - 1);
        C* ptr = buf + int_digits_needed + 1;

        if (!raw_frac_bits)
        {
            fill_n_zeros(ptr, frac_digits_needed);
        }
        else
        {
            constexpr uint32_t max_shift = traits::exponent_bias + traits::mantissa_bits - 1;
            constexpr uint32_t limb_count = ((max_shift + (limb_bits - 1)) / limb_bits) + 1;
            using big_int_type = big_int<limb_type, limb_count, wide_type>;

            // extract digit from the bits above
            const uint32_t keep_limbs = -shift / limb_bits;
            const uint32_t partial_bits = -shift % limb_bits;
            const limb_type digit_mask = (limb_type{ 1 } << partial_bits) - 1;

            big_int_type frac_bits = { raw_frac_bits };

            for (size_t i = 0; i < frac_digits_needed; ++i)
            {
                const limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
                *ptr++ = static_cast<C>('0' + digit);
            }

            if (fmt.round)
            {
                round_digit = frac_bits.mul_extract(10, keep_limbs, partial_bits);
            }
        }
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

} // namespace _string_convert_priv

//==============================================================================
// scientific format
//==============================================================================

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr size_t write_float_scientific(F value, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    using traits = typename float_bits<F>::traits;

    if (!buf)
    {
        return 0;
    }

    const float_bits<F> fb(value);
    const auto e_bits = fb.exponent();
    const auto m_bits = fb.mantissa();
    const bool sign_bit = fb.sign();
    const char sign = sign_bit ? '-' : (fmt.force_sign ? '+' : 0);

    // nan
    if (e_bits == traits::filled_exponent && m_bits != 0)
    {
        if (buf_size < 3)
        {
            return 0;
        }

        buf[0] = static_cast<C>(fmt.uppercase ? 'N' : 'n');
        buf[1] = static_cast<C>(fmt.uppercase ? 'A' : 'a');
        buf[2] = buf[0]; // 'N' or 'n'
        return 3;
    }

    if (sign)
    {
        if (buf_size < 2)
        {
            return 0;
        }

        buf[0] = sign;
    }

    size_t n = static_cast<size_t>(sign != 0);

    // inf
    if (e_bits == traits::filled_exponent && m_bits == 0)
    {
        if (buf_size - n < 3)
        {
            return 0;
        }

        buf[n++] = static_cast<C>(fmt.uppercase ? 'I' : 'i');
        buf[n++] = static_cast<C>(fmt.uppercase ? 'N' : 'n');
        buf[n++] = static_cast<C>(fmt.uppercase ? 'F' : 'f');
        return n;
    }

    if (e_bits == 0)
    {
        if (m_bits == 0)
        {
            // zero
            return n + _string_convert_priv::write_scientific_zero<C>(buf + n, buf_size - n, fmt);
        }
        else
        {
            // subnormal
            return n + _string_convert_priv::write_scientific_subnormal<F, C>(m_bits, buf + n, buf_size - n, fmt);
        }
    }

    const int e2 = static_cast<int>(e_bits) - traits::exponent_bias;
    const int shift = e2 - traits::mantissa_bits;

    if (e2 < 0)
    {
        // normal
        const int e10 = _string_convert_priv::normal_pow10<F>(e2);
        return n + _string_convert_priv::write_scientific_normal<F, C>(m_bits, e10, -shift, buf + n, buf_size - n, fmt);
    }

    if (shift >= 0)
    {
        // large integer
        const int e10 = _string_convert_priv::large_integer_pow10<F>(e2);
        return n + _string_convert_priv::write_scientific_large<F, C>(m_bits, e10, shift, buf + n, buf_size - n, fmt);
    }
    else
    {
        // int and float part
        return n + _string_convert_priv::write_scientific_mixed<F, C>(m_bits, shift, buf + n, buf_size - n, fmt);
    }
}

//==============================================================================
// fixed format
//==============================================================================

namespace _string_convert_priv {

template <typename C>
constexpr void round_fixed_normal(C* ptr, size_t precision_char_count) noexcept
{
    // 0 precision case
    if (precision_char_count == 0)
    {
        if (++(ptr[0]) > static_cast<C>('9'))
        {
            ptr[0] = static_cast<C>('1');
        }

        return;
    }

    for (size_t i = precision_char_count; i > 1; --i)
    {
        if (ptr[i] != static_cast<C>('9'))
        {
            ++ptr[i];
            return;
        }

        ptr[i] = static_cast<C>('0');
    }

    if (++ptr[0] > static_cast<C>('9'))
    {
        ptr[0] = static_cast<C>('1');
    }
}

template <typename C>
constexpr bool round_fixed_mixed(C* ptr, size_t int_digit_count, size_t precision) noexcept
{
    const size_t decimal_index = int_digit_count;

    // Fractional digits
    for (size_t i = decimal_index + precision; i > decimal_index; --i)
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

    return true;
}

template <typename C>
constexpr size_t write_fixed_zero(C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    const size_t precision_char_count = static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    const size_t needed = 1 + precision_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    *buf++ = static_cast<C>('0');
    if (fmt.precision > 0)
    {
        *buf++ = fmt.decimal_point;
        fill_n_zeros(buf, fmt.precision);
        buf += fmt.precision;
    }

    return needed;
}

template <typename F, typename C>
constexpr size_t write_fixed_subnormal(typename float_bits<F>::uint_type m_bits, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const int e10 = subnormal_pow10<F>(m_bits);

    const size_t precision_char_count = static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    const size_t needed = 1 + precision_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    constexpr uint32_t max_shift = traits::exponent_bias + traits::digits - 2;
    constexpr uint32_t limb_count = (max_shift + (limb_bits - 1)) / limb_bits;

    constexpr uint32_t keep_limbs = max_shift / limb_bits;
    constexpr uint32_t partial_bits = max_shift % limb_bits;
    constexpr limb_type digit_mask = (limb_type{ 1 } << partial_bits) - 1;

    buf[0] = static_cast<C>('0');
    if (fmt.precision == 0)
    {
        return 1;
    }

    buf[1] = fmt.decimal_point;
    size_t leading_zero_count = static_cast<size_t>(e10) - 1;

    if (fmt.precision < leading_zero_count || (fmt.precision == leading_zero_count && !fmt.round))
    {
        fill_n_zeros(buf + 2, fmt.precision);
        return needed;
    }

    fill_n_zeros(buf + 2, leading_zero_count);
    C* ptr = buf + 2 + leading_zero_count;

    // our large integer type
    using big_int_type = big_int<limb_type, limb_count, wide_type>;
    big_int_type frac_bits = { m_bits };

    // shave off some 0s
    {
        frac_bits.mul_shave_digits_lower(leading_zero_count, keep_limbs, digit_mask);
        const limb_type digit = frac_bits.mul_extract_clear_lower(10, keep_limbs, partial_bits, digit_mask);

        // we may be short by 1 digit
        if (digit == 0)
        {
            ++leading_zero_count;
        }

        *ptr = static_cast<C>('0' + digit);
        ++ptr;
    }

    for (size_t i = leading_zero_count; i < fmt.precision; ++i)
    {
        const limb_type digit = frac_bits.mul_extract_clear_lower(10, keep_limbs, partial_bits, digit_mask);
        *ptr = static_cast<C>('0' + digit);
        ++ptr;
    }

    if (fmt.round)
    {
        const limb_type round_digit = frac_bits.mul_extract_lower(10, keep_limbs, partial_bits);

        if (round_digit >= 5)
        {
            --ptr;

            while (true)
            {
                if (*ptr != C('9'))
                {
                    ++(*ptr);
                    break;
                }

                *ptr = C('0');
                --ptr;
            }
        }
    }

    return needed;
}

template <typename F, typename C>
constexpr size_t write_fixed_large(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    constexpr int max_shift = traits::filled_exponent - traits::exponent_bias - traits::digits;
    constexpr uint32_t total_bits = max_shift + traits::digits;
    constexpr uint32_t limb_count = (total_bits + (limb_bits - 1)) / limb_bits;

    size_t int_digit_count = static_cast<size_t>(e10) + 1;
    const size_t precision_char_count = static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    const size_t needed = int_digit_count + precision_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    const uint_type mantissa = m_bits | (uint_type{ 1 } << traits::mantissa_bits);
    size_t max_index = (shift + traits::digits - 1) / limb_bits;

    using big_int_type = big_int<limb_type, limb_count, wide_type>;
    big_int_type int_bits(mantissa, shift);

    C* ptr = buf + int_digit_count;

    for (size_t i = 0; i < int_digit_count; ++i)
    {
        const limb_type digit = int_bits.div_shrink_extract(10, max_index);
        *(--ptr) = static_cast<C>('0' + digit);
    }

    // There may be 1 digit left over in rare cases
    if (int_bits.bits[0] != 0)
    {
        if (buf_size < (needed + 1))
        {
            return 0;
        }

        const limb_type digit = int_bits.div_extract(10, 0);
        VX_ASSERT(int_bits.bits[0] == 0);

        mem::move_range(buf + 1, buf, int_digit_count);
        buf[0] = static_cast<C>('0' + digit);
        ++int_digit_count;
    }

    if (fmt.precision > 0)
    {
        buf[int_digit_count] = fmt.decimal_point;
        fill_n_zeros(buf + int_digit_count + 1, fmt.precision);
    }

    return int_digit_count + precision_char_count;
}

template <typename F, typename C>
constexpr size_t write_fixed_normal(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const size_t precision_char_count = static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    const size_t needed = 1 + precision_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    buf[0] = static_cast<C>('0');
    if (fmt.precision == 0 && !fmt.round)
    {
        return 1;
    }

    buf[1] = fmt.decimal_point;

    constexpr uint32_t max_shift = traits::exponent_bias + traits::mantissa_bits - 1;
    constexpr uint32_t limb_count = ((max_shift + (limb_bits - 1)) / limb_bits) + 1;

    const uint_type m2 = m_bits | (uint_type{ 1 } << traits::mantissa_bits);

    const uint32_t keep_limbs = shift / limb_bits;
    const uint32_t partial_bits = shift % limb_bits;
    const limb_type digit_mask = (limb_type{ 1 } << partial_bits) - 1;

    // our large integer type
    using big_int_type = big_int<limb_type, limb_count, wide_type>;
    big_int_type frac_bits = { m2 };

    // default for most common case
    size_t leading_zero_count = 0;

    C* ptr = buf + 2;

    // first find the first non 0 digit
    if (e10 > 1)
    {
        // The estimated decimal exponent can either be exact, or 1 too large
        // We subtract 2 here to move the estimate to either exact or 1 too small
        leading_zero_count = static_cast<size_t>(e10) - 2;
        if (fmt.precision < leading_zero_count || (fmt.precision == leading_zero_count && !fmt.round))
        {
            fill_n_zeros(ptr, fmt.precision);
            return needed;
        }

        fill_n_zeros(ptr, leading_zero_count);
        ptr += leading_zero_count;

        frac_bits.mul_shave_digits(leading_zero_count, keep_limbs, digit_mask);
        limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);

        if (digit == 0)
        {
            ++leading_zero_count;
        }

        *ptr = static_cast<C>('0' + digit);
        ++ptr;
    }

    for (size_t i = leading_zero_count; i < fmt.precision; ++i)
    {
        const limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
        *ptr = static_cast<C>('0' + digit);
        ++ptr;
    }

    if (fmt.round)
    {
        const limb_type round_digit = frac_bits.mul_extract(10, keep_limbs, partial_bits);
        if (round_digit >= 5)
        {
            round_fixed_normal<C>(buf, precision_char_count);
        }
    }

    return needed;
}

template <typename F, typename C>
constexpr size_t write_fixed_mixed(typename float_bits<F>::uint_type m_bits, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const uint_type m2 = m_bits | (uint_type{ 1 } << traits::mantissa_bits);
    const int frac_bit_count = -shift;
    uint_type int_bits = m2 >> frac_bit_count;

    // should be exact
    const size_t int_digit_count = digit_count_unsigned(int_bits);
    const int e10 = static_cast<int>(int_digit_count) - 1;

    const size_t precision_char_count = static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    size_t needed = int_digit_count + precision_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    C* ptr = buf + int_digit_count;

    // int digits
    {
        do
        {
            const auto digit = int_bits % 10;
            *--ptr = static_cast<C>('0' + digit);
            int_bits /= 10;

        } while (int_bits);
    }

    if (fmt.precision == 0)
    {
        return needed;
    }

    // float digits
    {
        ptr += int_digit_count;
        *ptr++ = fmt.decimal_point;

        const uint_type raw_frac_bits = m2 & ((uint_type{ 1 } << frac_bit_count) - 1);
        if (!raw_frac_bits)
        {
            fill_n_zeros(ptr, fmt.precision);
            return needed;
        }

        constexpr uint32_t max_shift = traits::exponent_bias + traits::mantissa_bits - 1; // 149
        constexpr uint32_t limb_count = ((max_shift + (limb_bits - 1)) / limb_bits) + 1;

        // extract digit from the bits above
        const uint32_t keep_limbs = -shift / limb_bits;
        const uint32_t partial_bits = -shift % limb_bits;
        const limb_type digit_mask = (limb_type{ 1 } << partial_bits) - 1;

        using big_int_type = big_int<limb_type, limb_count, wide_type>;
        big_int_type frac_bits = { raw_frac_bits };

        for (size_t i = 0; i < fmt.precision; ++i)
        {
            const limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
            *ptr++ = static_cast<C>('0' + digit);
        }

        // round
        if (fmt.round)
        {
            const limb_type round_digit = frac_bits.mul_extract(10, keep_limbs, partial_bits);
            if (round_digit >= 5)
            {
                if (round_fixed_mixed<C>(buf, int_digit_count, fmt.precision))
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

} // namespace _string_convert_priv

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr size_t write_float_fixed(F value, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    using traits = typename float_bits<F>::traits;

    if (!buf)
    {
        return 0;
    }

    const float_bits<F> fb(value);
    const auto e_bits = fb.exponent();
    const auto m_bits = fb.mantissa();
    const bool sign_bit = fb.sign();
    const char sign = sign_bit ? '-' : (fmt.force_sign ? '+' : 0);

    // nan
    if (e_bits == traits::filled_exponent && m_bits != 0)
    {
        if (buf_size < 3)
        {
            return 0;
        }

        buf[0] = static_cast<C>(fmt.uppercase ? 'N' : 'n');
        buf[1] = static_cast<C>(fmt.uppercase ? 'A' : 'a');
        buf[2] = buf[0]; // 'N' or 'n'
        return 3;
    }

    if (sign)
    {
        if (buf_size < 2)
        {
            return 0;
        }

        buf[0] = sign;
    }

    size_t n = static_cast<size_t>(sign != 0);

    // inf
    if (e_bits == traits::filled_exponent && m_bits == 0)
    {
        if (buf_size - n < 3)
        {
            return 0;
        }

        buf[n++] = static_cast<C>(fmt.uppercase ? 'I' : 'i');
        buf[n++] = static_cast<C>(fmt.uppercase ? 'N' : 'n');
        buf[n++] = static_cast<C>(fmt.uppercase ? 'F' : 'f');
        return n;
    }

    if (e_bits == 0)
    {
        if (m_bits == 0)
        {
            // zero
            return n + _string_convert_priv::write_fixed_zero<C>(buf + n, buf_size - n, fmt);
        }
        else
        {
            // subnormal
            return n + _string_convert_priv::write_fixed_subnormal<F, C>(m_bits, buf + n, buf_size - n, fmt);
        }
    }

    const int e2 = static_cast<int>(e_bits) - traits::exponent_bias;
    const int shift = e2 - traits::mantissa_bits;

    if (e2 < 0)
    {
        // normal
        const int e10 = _string_convert_priv::normal_pow10<F>(e2);
        return n + _string_convert_priv::write_fixed_normal<F, C>(m_bits, e10, -shift, buf + n, buf_size - n, fmt);
    }

    if (shift >= 0)
    {
        // large integer
        const int e10 = _string_convert_priv::large_integer_pow10<F>(e2);
        return n + _string_convert_priv::write_fixed_large<F, C>(m_bits, e10, shift, buf + n, buf_size - n, fmt);
    }
    else
    {
        // int and float part
        return n + _string_convert_priv::write_fixed_mixed<F, C>(m_bits, shift, buf + n, buf_size - n, fmt);
    }
}

//==============================================================================
// hex format
//==============================================================================

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr size_t write_float_hex(F value, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    if (!buf)
    {
        return 0;
    }

    const float_bits<F> fb(value);
    const auto e_bits = fb.exponent();
    const auto m_bits = fb.mantissa();
    const bool sign_bit = fb.sign();
    const char sign = sign_bit ? '-' : (fmt.force_sign ? '+' : 0);

    // nan
    if (e_bits == traits::filled_exponent && m_bits != 0)
    {
        if (buf_size < 3)
        {
            return 0;
        }

        buf[0] = static_cast<C>(fmt.uppercase ? 'N' : 'n');
        buf[1] = static_cast<C>(fmt.uppercase ? 'A' : 'a');
        buf[2] = buf[0]; // 'N' or 'n'
        return 3;
    }

    if (sign)
    {
        if (buf_size < 2)
        {
            return 0;
        }

        buf[0] = sign;
    }

    size_t n = static_cast<size_t>(sign != 0);

    // inf
    if (e_bits == traits::filled_exponent && m_bits == 0)
    {
        if (buf_size - n < 3)
        {
            return 0;
        }

        buf[n++] = static_cast<C>(fmt.uppercase ? 'I' : 'i');
        buf[n++] = static_cast<C>(fmt.uppercase ? 'N' : 'n');
        buf[n++] = static_cast<C>(fmt.uppercase ? 'F' : 'f');
        return n;
    }

    const bool subnormal = (e_bits == 0 && m_bits != 0);

    // First calulate the exponent
    const int exp = subnormal ? (1 - traits::exponent_bias) : (e_bits - traits::exponent_bias);
    const char exp_sign = (exp < 0) ? '-' : (fmt.force_exp_sign ? '+' : 0);

    const unsigned int abs_exp = static_cast<unsigned int>(exp < 0 ? -exp : exp);
    const size_t exp_digit_count = _string_convert_priv::digit_count_max3(abs_exp);

    const size_t leading_char_count = 2 + 1 + static_cast<size_t>(fmt.precision > 0) + fmt.precision;
    const size_t exp_char_count = 1 + static_cast<size_t>(exp_sign != 0) + exp_digit_count;
    const size_t needed = leading_char_count + exp_char_count;
    if (buf_size - n < needed)
    {
        return 0;
    }

    buf[n++] = static_cast<C>('0');
    buf[n++] = static_cast<C>(fmt.uppercase ? 'X' : 'x');
    buf[n++] = static_cast<C>('0' + subnormal);

    if (fmt.precision > 0)
    {
        buf[n++] = fmt.decimal_point;

        constexpr uint32_t mbits = traits::mantissa_bits;
        constexpr uint32_t hex_digits = (mbits + 3) / 4;
        constexpr uint32_t shift = hex_digits * 4 - mbits;
        constexpr uint32_t top_shift = (hex_digits - 1) * 4;

        size_t digits_remaining = fmt.precision;
        uint_type frac = m_bits << shift;

        while (frac && digits_remaining)
        {
            const uint32_t digit = static_cast<uint32_t>((frac >> top_shift) & 0xF);
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
        const int digit = exp % 10;
        buf[--pos] = static_cast<C>('0' + digit);
        exp /= 10;

    } while (exp);

    VX_ASSERT(!exp);
    n += exp_digit_count;

    return n;
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
size_t write_float(F value, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    // https://github.com/godotengine/godot/blob/master/core/string/ustring.cpp#L1458
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double support not implemented");

    constexpr size_t max_chars = 1000;
    char tmp[max_chars];

    const size_t precision = fmt.get_used_precision();

    // Build format string: %[+][00][l][format_char]
    // Max length: '%' + '+' + '0' + '0 + 'l' + 'g' + '\0' = 7 chars

    char fmt_str[7];
    char* p = fmt_str;

    *p++ = '%';
    if (fmt.force_sign)
    {
        *p++ = '+';
    }
    *p++ = '.';

    if (precision < 10)
    {
        *p++ = static_cast<char>('0' + precision);
    }
    else
    {
        *p++ = static_cast<char>('0' + (precision / 10));
        *p++ = static_cast<char>('0' + (precision % 10));
    }

    // force long double to demote to double
    *p++ = 'l';

    const char f = static_cast<char>(fmt.format);
    *p++ = fmt.uppercase ? to_upper(f) : f;
    *p = '\0';

    const int n = std::snprintf(tmp, max_chars, fmt_str, value);
    assert(n >= 0);

    if (!buf)
    {
        return static_cast<size_t>(n);
    }

    const size_t copy_count = std::min(static_cast<size_t>(n), buf_size);
    for (size_t i = 0; i < copy_count; ++i)
    {
        if (tmp[i] == '.')
        {
            buf[i] = fmt.decimal_point;
        }
        else
        {
            buf[i] = static_cast<C>(tmp[i]);
        }
    }

    return static_cast<size_t>(n);
}

//==============================================================================

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
auto to_string(I v, const integer_format_options& fmt = {}) noexcept
{
    constexpr size_t buf_size = 65;
    C buf[buf_size];
    const size_t n = write_integer<I, C>(v, buf, buf_size, fmt);

    str::basic_string<C> res(buf, n);
    return res;
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
auto to_string(F v, const integer_format_options& fmt = {}) noexcept
{
    constexpr size_t buf_size = 1000;
    C buf[buf_size];
    const size_t n = write_float<F, C>(v, buf, buf_size, fmt);

    str::basic_string<C> res(buf, n);
    return res;
}

//==============================================================================
// from string
//==============================================================================

//template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& std::is_signed<I>::value&& type_traits::is_char<C>::value)>
//size_t from_string(const C* s, const size_t count, I& out, const numeric_format_options<C>& fmt = {}) noexcept
//{
//    VX_ASSERT(s);
//    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);
//
//    out = 0;
//
//    if (count == 0)
//    {
//        return 0;
//    }
//
//    bool negative = false;
//    size_t i = 0;
//
//    switch (s[i])
//    {
//        case C('-'):
//        {
//            negative = true;
//            VX_FALLTHROUGH;
//        }
//        case C('+'):
//        {
//            ++i;
//            VX_FALLTHROUGH;
//        }
//        default:
//        {
//            break;
//        }
//    }
//
//    using U = typename std::make_unsigned<I>::type;
//    U uout = 0;
//    bool overflow = false;
//
//    while (i < count)
//    {
//        const int digit = char_to_digit(s[i], fmt.base);
//        if (digit < 0)
//        {
//            break;
//        }
//
//        if (!overflow && uout > (numeric_limits<U>::max() - static_cast<U>(digit)) / static_cast<U>(fmt.base))
//        {
//            overflow = true;
//            err::set(err::out_of_range);
//        }
//
//        uout = uout * static_cast<U>(fmt.base) + static_cast<U>(digit);
//        ++i;
//    }
//
//    out = static_cast<I>(negative ? (U(0) - uout) : uout);
//    return i;
//}
//
//template <typename U, typename C = char, VX_REQUIRES(std::is_integral<U>::value&& std::is_unsigned<U>::value&& type_traits::is_char<C>::value)>
//size_t from_string(const C* s, const size_t count, U& out, const numeric_format_options<C>& fmt = {}) noexcept
//{
//    VX_ASSERT(s);
//    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);
//
//    out = 0;
//
//    if (count == 0)
//    {
//        return 0;
//    }
//
//    size_t i = 0;
//
//    if (s[i] == C('+'))
//    {
//        ++i;
//    }
//
//    bool overflow = false;
//
//    while (i < count)
//    {
//        const int digit = char_to_digit(s[i], fmt.base);
//        if (digit < 0)
//        {
//            break;
//        }
//
//        if (!overflow && out > (numeric_limits<U>::max() - static_cast<U>(digit)) / static_cast<U>(fmt.base))
//        {
//            overflow = true;
//            err::set(err::out_of_range);
//        }
//
//        out = out * static_cast<U>(fmt.base) + static_cast<U>(digit);
//        ++i;
//    }
//
//    return i;
//}
//
//template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
//size_t from_string(const C* s, const size_t count, F& out, const numeric_format_options<C>& fmt = {}) noexcept
//{
//    VX_ASSERT(s);
//    out = 0;
//
//    if (count == 0)
//    {
//        return 0;
//    }
//
//    C* endptr = nullptr;
//    errno = 0;
//
//    VX_IF_CONSTEXPR (sizeof(F) <= sizeof(float))
//    {
//        out = static_cast<F>(std::strtof(s, &endptr));
//    }
//    else VX_IF_CONSTEXPR (sizeof(F) <= sizeof(double))
//    {
//        out = static_cast<F>(std::strtod(s, &endptr));
//    }
//    else
//    {
//        out = static_cast<F>(std::strtold(s, &endptr));
//    }
//
//    if (errno == ERANGE)
//    {
//        err::set(err::out_of_range);
//    }
//
//    return static_cast<size_t>(endptr - s);
//}
//
////==============================================================================
//
//template <typename I, typename S, VX_REQUIRES(std::is_integral<I>::value&& is_string_like<S>::value)>
//size_t from_string(const S& s, I& out, const int base = 10) noexcept
//{
//    using C = typename S::value_type;
//    return from_string<I, C>(s.data(), s.size(), out, base);
//}
//
////==============================================================================
//
//template <typename F, typename C, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
//size_t from_string(const C* s, F& out) noexcept
//{
//    const size_t count = str::length(s);
//    return from_string<F, C>(s, count, out);
//}
//
//template <typename F, typename S, VX_REQUIRES(std::is_floating_point<F>::value&& is_string_like<S>::value)>
//size_t from_string(const S& s, F& out) noexcept
//{
//    using C = typename S::value_type;
//    return from_string<F, C>(s.data(), s.size(), out);
//}

} // namespace str
} // namespace vx
