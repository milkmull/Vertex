#pragma once

#include <algorithm>
#include <vector>

#include "vertex/config/assert.hpp"
#include "vertex/config/flags.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/math/float_bits.hpp"
#include "vertex/std/math/uint_n.hpp"
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
    constexpr C hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

    const size_t required = 2 * size;
    if (!buf || buf_size < required)
    {
        return 0;
    }

    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    for (size_t i = 0; i < size; ++i)
    {
        const C c1 = hex[(bytes[i] >> 4) & 0xF]; // High nibble
        const C c2 = hex[(bytes[i] >> 0) & 0xF]; // Low nibble

        *buf++ = upper ? to_upper(c1) : c1;
        *buf++ = upper ? to_upper(c2) : c2;
    }

    return required;
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
// format specifiers
//==============================================================================

enum class numeric_format : char
{
    fixed = 'f',
    scientific = 'e',
    general = 'g',
    hex = 'a',
};

enum : int32_t
{
    default_fixed_precision = 6,
    default_hex_precision = 13,
    default_scientific_precision = 6,
    max_precision = 1'000'000
};

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
struct numeric_format_options
{
    numeric_format format = numeric_format::general;
    C decimal_point = C('.');
    int32_t base = 10;
    int32_t precision = 0;
    bool uppercase = false;
    bool force_sign = false;
    bool force_exp_sign = false;
    bool round = true;

    int32_t get_used_precision() const noexcept
    {
        //if (precision)
        {
            return (precision <= max_precision) ? precision : max_precision;
        }

        //switch (format)
        //{
        //    case numeric_format::fixed: return default_fixed_precision;
        //    case numeric_format::scientific: return default_scientific_precision;
        //    case numeric_format::hex: return default_hex_precision;
        //}
    }
};

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int char_to_digit(const C c, int base = 10) noexcept
{
    int digit = -1;

    if (c >= C('0') && c <= C('9'))
    {
        digit = static_cast<int>(c - C('0'));
    }
    else
    {
        const C lc = to_lower(c);
        if (lc >= C('a') && lc <= C('z'))
        {
            digit = static_cast<int>(lc - C('a')) + 10;
        }
    }

    return (digit < base) ? digit : -1;
}

//==============================================================================
// to string
//==============================================================================

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

template <size_t N, typename Limb, typename Traits>
constexpr size_t digit_count(uint_n<N, Limb, Traits> value, int base) noexcept
{
    const uint_n<N, Limb, Traits> n_base(base);
    size_t n = 0;

    do
    {
        value /= n_base;
        ++n;

    } while (!value.is_zero());

    return n;
}

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
size_t write_integer(I value, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt, bool null_terminate = false) noexcept
{
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

    constexpr const char digits[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y', 'z'
    };

    constexpr size_t max_chars = sizeof(I) * CHAR_BIT + 1;
    C tmp[max_chars];

    size_t n = 0;

    using U = typename std::make_unsigned<I>::type;
    U uvalue;
    const U ubase = static_cast<U>(fmt.base);

    VX_IF_CONSTEXPR (std::is_signed<I>::value)
    {
        const bool negative = (value < 0);

        if (negative || fmt.force_sign)
        {
            tmp[0] = negative ? '-' : '+';
            ++n;
        }

        if (negative)
        {
            // cast through unsigned to avoid UB on min value (-value would overflow)
            uvalue = static_cast<U>(~static_cast<U>(value) + 1);
        }
        else
        {
            uvalue = static_cast<U>(value);
        }
    }
    else // unsigned
    {
        if (fmt.force_sign)
        {
            tmp[0] = '+';
            ++n;
        }

        uvalue = value;
    }

    n += digit_count(uvalue, ubase);

    if (!buf || !buf_size)
    {
        return n;
    }

    size_t i = n;

    do
    {
        const char c = digits[uvalue % ubase];
        tmp[--i] = fmt.uppercase ? to_upper(c) : c;
        uvalue /= ubase;

    } while (uvalue);

    const size_t copy_count = std::min(n, buf_size);
    mem::copy(buf, tmp, copy_count * sizeof(C));

    return copy_count;
}

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

static constexpr uint32_t pow10_table[] = {
    1u, 10u, 100u, 1000u, 10000u, 100000u, 1000000u, 10000000u, 100000000u
};

inline constexpr uint32_t pow10_u32(size_t n) noexcept
{
    VX_ASSERT(n <= 9);
    VX_ASSERT(n > 0);
    return pow10_table[n - 1];
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

inline constexpr uint32_t decimal_length(const uint32_t v) noexcept
{
    // Max value allowed by this overload is 999,999,999 (9 digits)
    VX_ASSERT(v < 1000000000);
    // clang-format off

    if (v >= 100000000) return 9;
    if (v >= 10000000)  return 8;
    if (v >= 1000000)   return 7;
    if (v >= 100000)    return 6;
    if (v >= 10000)     return 5;
    if (v >= 1000)      return 4;
    if (v >= 100)       return 3;
    if (v >= 10)        return 2;
    return 1;

    // clang-format on
}

inline constexpr uint32_t decimal_length(uint64_t v) noexcept
{
    if (v < 1000000000)
    {
        return decimal_length(static_cast<uint32_t>(v));
    }

    uint32_t n = 0;

    do
    {
        v /= 10;
        ++n;

    } while (v);

    return n;
}

template <typename limb_type_, uint32_t limb_count_, typename wide_type_>
struct big_int
{
    using limb_type = limb_type_;
    using wide_type = wide_type_;
    static constexpr uint32_t limb_count = limb_count_;
    static constexpr size_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    limb_type bits[limb_count];

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

        // Post-loop scan to update max_index cleanly
        while (max_index > 0 && bits[max_index] == 0)
        {
            --max_index;
        }

        return remainder;
    }
};

} // namespace _string_convert_priv

//==============================================================================
// scientific helpers
//==============================================================================

namespace _string_convert_priv {

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
constexpr size_t write_scientific_zero(C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
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
constexpr size_t write_scientific_subnormal(typename float_bits<F>::uint_type m_bits, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const int leading = bit::countl_zero(m_bits) - (sizeof(uint_type) * CHAR_BIT - traits::mantissa_bits);
    const int e2 = 1 - traits::exponent_bias - leading;
    int e10 = -(log10_pow5(-e2) + e2);

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
        size_t shave = static_cast<size_t>(e10);

        while (shave > 9)
        {
            frac_bits.mul_clear_lower(1000000000u, keep_limbs, digit_mask);
            shave -= 9;
        }

        if (shave > 1)
        {
            frac_bits.mul_clear_lower(pow10_u32(shave), keep_limbs, digit_mask);
        }

        limb_type digit = frac_bits.mul_extract_clear_lower(10, keep_limbs, partial_bits, digit_mask);

        // we may be short by 1 digit
        if (digit == 0)
        {
            digit = frac_bits.mul_extract_clear_lower(10, keep_limbs, partial_bits, digit_mask);
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
constexpr size_t write_scientific_normal(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
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
        limb_type digit = 0;

        if (e10 == 1)
        {
            // Our estimate should be accurate when e10 is 1, and this is the most common case.
            digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
            VX_ASSERT(digit != 0);
        }
        else
        {
            size_t shave = static_cast<size_t>(e10);

            while (shave > 10)
            {
                frac_bits.mul_clear(1000000000u, keep_limbs, digit_mask);
                shave -= 9;
            }

            if (shave > 1)
            {
                frac_bits.mul_clear_lower(pow10_u32(shave), keep_limbs, digit_mask);
            }

            if (digit != 0)
            {
                --e10;
                exp_changed = true;
            }
            else
            {
                digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
                VX_ASSERT(digit != 0);
            }
        }

        buf[0] = static_cast<C>('0' + digit);
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
constexpr size_t write_scientific_large(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
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
        size_t shave = int_digit_count - digits_needed;

        // Batch divide by 10^9 (or 10^18 if using wide_type) to wipe out chunks of digits instantly
        while (shave > 9)
        {
            int_bits.div_shrink(1000000000u, max_index);
            shave -= 9;
        }

        if (shave > 1)
        {
            int_bits.div_shrink(pow10_u32(shave), max_index);
        }

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
constexpr size_t write_scientific_mixed(typename float_bits<F>::uint_type m_bits, int shift, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
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
    const size_t int_digit_count = decimal_length(int_bits);
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
constexpr size_t write_float_scientific(F value, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    if (!buf || buf_size == 0)
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

    size_t n = sign ? 1 : 0;

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
        const int e10 = -(_string_convert_priv::log10_pow5(-e2) + e2);
        return n + _string_convert_priv::write_scientific_normal<F, C>(m_bits, e10, -shift, buf + n, buf_size - n, fmt);
    }

    if (shift >= 0)
    {
        // large integer
        const int e10 = _string_convert_priv::log10_pow2(e2);
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
constexpr size_t write_fixed_zero(C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
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
constexpr size_t write_fixed_subnormal(typename float_bits<F>::uint_type m_bits, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const int leading = bit::countl_zero(m_bits) - (sizeof(uint_type) * CHAR_BIT - traits::mantissa_bits);
    const int e2 = 1 - traits::exponent_bias - leading;
    const int e10 = -(log10_pow5(-e2) + e2);

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
    size_t leading_zero_count = e10;

    if (fmt.precision <= leading_zero_count)
    {
        fill_n_zeros(buf + 2, fmt.precision);
        return needed;
    }

    fill_n_zeros(buf + 2, leading_zero_count);

    // our large integer type
    using big_int_type = big_int<limb_type, limb_count, wide_type>;
    big_int_type frac_bits = { m_bits };

    C* ptr = buf + 2 + leading_zero_count;

    // shave off some 0s, we may be short in our estimate by 1 but the final loop will catch it
    {
        size_t shave = leading_zero_count;

        while (shave > 9)
        {
            frac_bits.mul_clear_lower(1000000000u, keep_limbs, digit_mask);
            shave -= 9;
        }
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
constexpr size_t write_fixed_large(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
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
constexpr size_t write_fixed_normal(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
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

    C* ptr = buf;

    *ptr++ = static_cast<C>('0');
    if (fmt.precision == 0 && !fmt.round)
    {
        return 1;
    }

    *ptr++ = fmt.decimal_point;

    constexpr uint32_t max_shift = traits::exponent_bias + traits::mantissa_bits - 1;
    constexpr uint32_t limb_count = ((max_shift + (limb_bits - 1)) / limb_bits) + 1;

    const uint_type m2 = m_bits | (uint_type{ 1 } << traits::mantissa_bits);

    const uint32_t keep_limbs = shift / limb_bits;
    const uint32_t partial_bits = shift % limb_bits;
    const limb_type digit_mask = (limb_type{ 1 } << partial_bits) - 1;

    // our large integer type
    using big_int_type = big_int<limb_type, limb_count, wide_type>;
    big_int_type frac_bits = { m2 };

    for (size_t i = 0; i < fmt.precision; ++i)
    {
        const limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
        *ptr++ = static_cast<C>('0' + digit);

#undef extract_and_clear
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
constexpr size_t write_fixed_mixed(typename float_bits<F>::uint_type m_bits, int shift, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
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
    const size_t int_digit_count = decimal_length(int_bits);
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
                if (round_fixed_mixed<C>(ptr, int_digit_count, fmt.precision, fmt.decimal_point))
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
constexpr size_t write_float_fixed(F value, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<float>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    if (!buf || buf_size == 0)
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

    size_t n = sign ? 1 : 0;

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
            return n + _string_convert_priv::write_fixed_zero<C>(buf, buf_size, fmt);
        }
        else
        {
            // subnormal
            return n + _string_convert_priv::write_fixed_subnormal<F, C>(m_bits, buf, buf_size, fmt);
        }
    }

    const int e2 = static_cast<int>(e_bits) - traits::exponent_bias;
    const int shift = e2 - traits::mantissa_bits;

    if (e2 < 0)
    {
        // normal
        const int e10 = -(_string_convert_priv::log10_pow5(-e2) + e2);
        return n + _string_convert_priv::write_fixed_normal<F, C>(m_bits, e10, -shift, buf + n, buf_size - n, fmt);
    }

    if (shift >= 0)
    {
        // large integer
        const int e10 = _string_convert_priv::log10_pow2(e2);
        return n + _string_convert_priv::write_fixed_large<F, C>(m_bits, e10, shift, buf + n, buf_size - n, fmt);
    }
    else
    {
        // int and float part
        return n + _string_convert_priv::write_fixed_mixed<F, C>(m_bits, shift, buf + n, buf_size - n, fmt);
    }
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr size_t write_float_hex(F value, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    if (!buf || buf_size == 0)
    {
        return 0;
    }

    const size_t precision = fmt.precision;
    const bool upper = fmt.uppercase;
    const float_bits<F> fb(value);
    const char sign = fb.is_negative() ? '-' : (fmt.force_sign ? '+' : '\0');

    // -------------------------------------------------------------------------
    // Special values
    // -------------------------------------------------------------------------

    if (fb.is_nan())
    {
        if (buf_size < 3)
        {
            return 0;
        }

        buf[0] = upper ? 'N' : 'n';
        buf[1] = upper ? 'A' : 'a';
        buf[2] = buf[0];
        return 3;
    }

    size_t n = 0;

    if (fb.is_inf())
    {
        const size_t needed = (sign ? 1 : 0) + 3; // optional sign + "inf"
        if (buf_size - n < needed)
        {
            return 0;
        }

        if (sign)
        {
            buf[n++] = sign;
        }

        buf[n++] = upper ? 'I' : 'i';
        buf[n++] = upper ? 'N' : 'n';
        buf[n++] = upper ? 'F' : 'f';
        return needed;
    }

    // -------------------------------------------------------------------------
    // Hex float format: [sign] 0x h.hhhh p [exp_sign] exp
    //
    // Following printf %a convention:
    //   - Normal values:    1.mantissa_hex * 2^true_exponent  (integer bit explicit)
    //   - Subnormal values: 0.mantissa_hex * 2^-(exponent_bias-1)  (raw, no renormalization)
    //   - Zero:             0x0[.000...]p0
    //
    // The binary exponent is the true (unbiased) exponent for normals, or
    // -(exponent_bias - 1) for subnormals (same as printf %a).
    //
    // fmt.precision controls the number of hex digits after the point.
    // Each hex digit represents exactly 4 mantissa bits.
    // -------------------------------------------------------------------------

    const bool is_subnormal = fb.is_subnormal();
    const bool is_zero = fb.is_zero();

    // True exponent: for normals this is the stored exponent - bias.
    // For subnormals, printf convention uses -(exponent_bias - 1).
    const int true_exp = fb.true_exponent();

    // Mantissa to print: for normals, the stored mantissa bits (integer bit
    // is implied and printed as the leading hex digit '1').
    // For subnormals, the stored mantissa bits (leading hex digit is '0').
    const uint_type mantissa = fb.mantissa();       // without integer bit
    const uint_type leading = is_subnormal ? 0 : 1; // the digit before the point

    // -------------------------------------------------------------------------
    // Compute exponent sign and absolute value.
    // -------------------------------------------------------------------------
    const char exp_sign = true_exp < 0 ? '-' : (fmt.force_sign ? '+' : '\0');
    const int exp_abs = true_exp < 0 ? -true_exp : true_exp;
    const size_t exp_digit_count = digit_count(exp_abs, 10);

    // -------------------------------------------------------------------------
    // Size check.
    // [sign] 0x [leading] [. precision_digits] p [exp_sign] [exp_digits]
    // -------------------------------------------------------------------------
    const size_t needed = (sign ? 1 : 0) + 2  // "0x" or "0X"
        + 1                                   // leading hex digit
        + (precision > 0 ? 1 + precision : 0) // '.' + hex digits
        + 1                                   // 'p' or 'P'
        + (exp_sign ? 1 : 0) + exp_digit_count;

    if (buf_size < needed)
    {
        return 0;
    }

    // -------------------------------------------------------------------------
    // Write prefix: [sign] 0x
    // -------------------------------------------------------------------------
    if (sign)
    {
        buf[n++] = sign;
    }

    buf[n++] = C('0');
    buf[n++] = upper ? C('X') : C('x');

    // -------------------------------------------------------------------------
    // Write leading digit (1 for normals, 0 for subnormals/zero).
    // -------------------------------------------------------------------------
    buf[n++] = static_cast<C>('0' + leading);

    // -------------------------------------------------------------------------
    // Write fractional hex digits.
    //
    // The mantissa is stored left-aligned within uint_type for extraction:
    // we shift left so the most significant nibble is always in the top 4 bits
    // of a working value, then extract nibble by nibble.
    //
    // mantissa_bits may not be a multiple of 4, so we align to a nibble
    // boundary by shifting left (padding with zeros on the right).
    // -------------------------------------------------------------------------
    if (precision > 0)
    {
        buf[n++] = fmt.decimal_point;

        // Left-align the stored mantissa to the top of uint_type.
        // The mantissa occupies the low mantissa_bits bits of uint_type, so shifting
        // left by (total_bits - mantissa_bits) places the most significant mantissa
        // bit at the top of the integer. Each subsequent m <<= 4 then rolls the
        // next nibble into the top 4 bits for extraction, giving digits MSB-first
        // without any separate nibble-boundary alignment step.
        const int top_align = static_cast<int>(sizeof(uint_type) * CHAR_BIT) - static_cast<int>(traits::mantissa_bits);
        uint_type m = mantissa << top_align;

        constexpr char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

        // How many full nibbles are available from the stored mantissa.
        // Beyond full_hex_digits the value is zero (no more mantissa bits).
        for (size_t i = 0; i < precision; ++i)
        {
            // Extract the top nibble.
            const int nibble_shift = static_cast<int>(sizeof(uint_type) * CHAR_BIT) - 4;
            const uint_type nibble = (m >> nibble_shift) & 0xF;
            const char hex_digit = hex[nibble];
            buf[n++] = static_cast<C>(hex_digit);
            m <<= 4;
        }
    }

    // -------------------------------------------------------------------------
    // Write exponent: p [exp_sign] exp_digits
    // -------------------------------------------------------------------------
    buf[n++] = upper ? C('P') : C('p');
    if (exp_sign)
    {
        buf[n++] = exp_sign;
    }

    // Write exponent digits LSB-first then reverse, same as scientific.
    const size_t exp_start = n;
    {
        int e = exp_abs;
        do
        {
            buf[n++] = static_cast<C>('0' + e % 10);
            e /= 10;
        } while (e);
    }

    // reverse exponent digits to MSB-first
    str::reverse(buf + exp_start, n - exp_start);

    return n;
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
size_t write_float(F value, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
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

    if (!buf || !buf_size)
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
auto to_string(I v, const numeric_format_options<C>& fmt = {}) noexcept
{
    constexpr size_t buf_size = 65;
    C buf[buf_size];
    const size_t n = write_integer<I, C>(v, buf, buf_size, fmt);

    str::basic_string<C> res(buf, n);
    return res;
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
auto to_string(F v, const numeric_format_options<C>& fmt = {}) noexcept
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

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& std::is_signed<I>::value&& type_traits::is_char<C>::value)>
size_t from_string(const C* s, const size_t count, I& out, const numeric_format_options<C>& fmt = {}) noexcept
{
    VX_ASSERT(s);
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

    out = 0;

    if (count == 0)
    {
        return 0;
    }

    bool negative = false;
    size_t i = 0;

    switch (s[i])
    {
        case C('-'):
        {
            negative = true;
            VX_FALLTHROUGH;
        }
        case C('+'):
        {
            ++i;
            VX_FALLTHROUGH;
        }
        default:
        {
            break;
        }
    }

    using U = typename std::make_unsigned<I>::type;
    U uout = 0;
    bool overflow = false;

    while (i < count)
    {
        const int digit = char_to_digit(s[i], fmt.base);
        if (digit < 0)
        {
            break;
        }

        if (!overflow && uout > (numeric_limits<U>::max() - static_cast<U>(digit)) / static_cast<U>(fmt.base))
        {
            overflow = true;
            err::set(err::out_of_range);
        }

        uout = uout * static_cast<U>(fmt.base) + static_cast<U>(digit);
        ++i;
    }

    out = static_cast<I>(negative ? (U(0) - uout) : uout);
    return i;
}

template <typename U, typename C = char, VX_REQUIRES(std::is_integral<U>::value&& std::is_unsigned<U>::value&& type_traits::is_char<C>::value)>
size_t from_string(const C* s, const size_t count, U& out, const numeric_format_options<C>& fmt = {}) noexcept
{
    VX_ASSERT(s);
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

    out = 0;

    if (count == 0)
    {
        return 0;
    }

    size_t i = 0;

    if (s[i] == C('+'))
    {
        ++i;
    }

    bool overflow = false;

    while (i < count)
    {
        const int digit = char_to_digit(s[i], fmt.base);
        if (digit < 0)
        {
            break;
        }

        if (!overflow && out > (numeric_limits<U>::max() - static_cast<U>(digit)) / static_cast<U>(fmt.base))
        {
            overflow = true;
            err::set(err::out_of_range);
        }

        out = out * static_cast<U>(fmt.base) + static_cast<U>(digit);
        ++i;
    }

    return i;
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
size_t from_string(const C* s, const size_t count, F& out, const numeric_format_options<C>& fmt = {}) noexcept
{
    VX_ASSERT(s);
    out = 0;

    if (count == 0)
    {
        return 0;
    }

    C* endptr = nullptr;
    errno = 0;

    VX_IF_CONSTEXPR (sizeof(F) <= sizeof(float))
    {
        out = static_cast<F>(std::strtof(s, &endptr));
    }
    else VX_IF_CONSTEXPR (sizeof(F) <= sizeof(double))
    {
        out = static_cast<F>(std::strtod(s, &endptr));
    }
    else
    {
        out = static_cast<F>(std::strtold(s, &endptr));
    }

    if (errno == ERANGE)
    {
        err::set(err::out_of_range);
    }

    return static_cast<size_t>(endptr - s);
}

//==============================================================================

template <typename I, typename S, VX_REQUIRES(std::is_integral<I>::value&& is_string_like<S>::value)>
size_t from_string(const S& s, I& out, const int base = 10) noexcept
{
    using C = typename S::value_type;
    return from_string<I, C>(s.data(), s.size(), out, base);
}

//==============================================================================

template <typename F, typename C, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
size_t from_string(const C* s, F& out) noexcept
{
    const size_t count = str::length(s);
    return from_string<F, C>(s, count, out);
}

template <typename F, typename S, VX_REQUIRES(std::is_floating_point<F>::value&& is_string_like<S>::value)>
size_t from_string(const S& s, F& out) noexcept
{
    using C = typename S::value_type;
    return from_string<F, C>(s.data(), s.size(), out);
}

} // namespace str
} // namespace vx
