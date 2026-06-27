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

} // namespace _string_convert_priv

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
constexpr size_t write_integer(I value, C* buf, const size_t buf_size, const integer_format_options& fmt = {}) noexcept
{
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

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
        const char c = _string_convert_priv::int_digits[uvalue % ubase];
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

    float_fixed_default_precision = 6,
    float_scientific_default_precision = 6,

    float_hex_default_precision = 6,
    double_hex_default_precision = 13,

    max_float_precision = 1'000'000
};

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
        m_bits = (bits & traits::mantissa_mask);
        e_bits = (bits & traits::exponent_mask) >> traits::mantissa_bits;
        sign_bit = (bits & traits::sign_mask) != 0;
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

    limb_type bits[limb_count_];

    constexpr big_int() noexcept
        : bits{}
    {
    }

    constexpr big_int(limb_type x) noexcept
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
            const wide_type product = (static_cast<wide_type>(bits[j]) * static_cast<wide_type>(x)) + static_cast<wide_type>(carry);
            bits[j] = static_cast<limb_type>(product);
            carry = static_cast<limb_type>(product >> limb_bits);
        }

        return carry;
    }

    inline constexpr limb_type extract_digit_lower(size_t top_limb, limb_type partial_bits) const noexcept
    {
        return bits[top_limb] >> partial_bits;
    }

    inline constexpr limb_type extract_digit(limb_type carry, size_t top_limb, limb_type partial_bits) const noexcept
    {
        const limb_type lo = extract_digit_lower(top_limb, partial_bits);
        const limb_type hi = carry << (limb_bits - partial_bits);
        return lo | hi;
    }

    inline constexpr limb_type mul_extract_lower(limb_type x, size_t top_limb, limb_type partial_bits) noexcept
    {
        mul(x);
        return extract_digit_lower(top_limb, partial_bits);
    }

    inline constexpr limb_type mul_extract(limb_type x, size_t top_limb, limb_type partial_bits) noexcept
    {
        const limb_type carry = mul(x);
        return extract_digit(carry, top_limb, partial_bits);
    }

    inline constexpr limb_type mul_extract_clear_lower(limb_type x, size_t top_limb, limb_type partial_bits, limb_type mask) noexcept
    {
        const limb_type digit = mul_extract_lower(x, top_limb, partial_bits);
        bits[top_limb] &= mask;
        return digit;
    }

    inline constexpr limb_type mul_extract_clear(limb_type x, size_t top_limb, limb_type partial_bits, limb_type mask) noexcept
    {
        const limb_type digit = mul_extract(x, top_limb, partial_bits);
        bits[top_limb] &= mask;
        return digit;
    }

    inline constexpr void mul_shave_digits(size_t count) noexcept
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

    inline constexpr limb_type div_extract(limb_type x, size_t start_index) noexcept
    {
        constexpr uint32_t half = limb_bits / 2;
        constexpr limb_type half_mask = (limb_type{ 1 } << half) - limb_type{ 1 };

        limb_type remainder = 0;

        for (size_t i = start_index + 1; i-- > 0;)
        {
            const limb_type hi = bits[i] >> half;
            const limb_type lo = bits[i] & half_mask;

            const wide_type d1 = (static_cast<wide_type>(remainder) << half) | hi;
            const limb_type q1 = static_cast<limb_type>(d1 / x);
            const limb_type r1 = static_cast<limb_type>(d1 % x);

            const wide_type d2 = (static_cast<wide_type>(r1) << half) | lo;
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

    inline constexpr limb_type div_extract_shrink(limb_type x, size_t& max_index) noexcept
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

template <typename F, typename C>
constexpr float_write_status write_float_start(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_format_options<C>& fmt, size_t& n) noexcept
{
    using traits = typename float_bits<F>::traits;

    if (!buf)
    {
        return float_write_status::finished;
    }

    // nan
    if (fb.e_bits == traits::filled_exponent && fb.m_bits != 0)
    {
        if (buf_size < 3)
        {
            return float_write_status::failed;
        }

        buf[0] = static_cast<C>(fmt.uppercase ? 'N' : 'n');
        buf[1] = static_cast<C>(fmt.uppercase ? 'A' : 'a');
        buf[2] = buf[0]; // 'N' or 'n'

        n = 3;
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

    // inf
    if (fb.e_bits == traits::filled_exponent && fb.m_bits == 0)
    {
        if (buf_size - n < 3)
        {
            return float_write_status::failed;
        }

        buf[n++] = static_cast<C>(fmt.uppercase ? 'I' : 'i');
        buf[n++] = static_cast<C>(fmt.uppercase ? 'N' : 'n');
        buf[n++] = static_cast<C>(fmt.uppercase ? 'F' : 'f');

        return float_write_status::finished;
    }

    return float_write_status::continue_;
}

//==============================================================================
// fixed format
//==============================================================================

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
constexpr size_t write_fixed_zero(C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
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
        buf += precision;
    }

    return needed;
}

template <typename F, typename C>
constexpr size_t write_fixed_subnormal(typename float_bits<F>::uint_type m_bits, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const int e10 = subnormal_pow10<F>(m_bits);

    const size_t precision_char_count = static_cast<size_t>(precision > 0) + precision;
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
    if (precision == 0)
    {
        return 1;
    }

    buf[1] = fmt.decimal_point;
    size_t leading_zero_count = static_cast<size_t>(e10) - 1;

    if (precision < leading_zero_count || (precision == leading_zero_count && !fmt.round))
    {
        fill_n_zeros(buf + 2, precision);
        return needed;
    }

    fill_n_zeros(buf + 2, leading_zero_count);
    C* ptr = buf + 2 + leading_zero_count;

    // our large integer type
    using big_int_type = big_int<limb_type, limb_count, wide_type>;
    big_int_type frac_bits = { m_bits };

    // shave off some 0s
    {
        frac_bits.mul_shave_digits(leading_zero_count);
        const limb_type digit = frac_bits.mul_extract_clear_lower(10, keep_limbs, partial_bits, digit_mask);

        // we may be short by 1 digit
        if (digit == 0)
        {
            ++leading_zero_count;
        }

        *ptr = static_cast<C>(hex::digits[digit]);
        ++ptr;
    }

    for (size_t i = leading_zero_count; i < precision; ++i)
    {
        const limb_type digit = frac_bits.mul_extract_clear_lower(10, keep_limbs, partial_bits, digit_mask);
        *ptr = static_cast<C>(hex::digits[digit]);
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
constexpr size_t write_fixed_normal(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

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
        if (precision < leading_zero_count || (precision == leading_zero_count && !fmt.round))
        {
            fill_n_zeros(ptr, precision);
            return needed;
        }

        fill_n_zeros(ptr, leading_zero_count);
        ptr += leading_zero_count;

        frac_bits.mul_shave_digits(leading_zero_count);
        const limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);

        if (digit == 0)
        {
            ++leading_zero_count;
        }

        *ptr = static_cast<C>(hex::digits[digit]);
        ++ptr;
    }

    for (size_t i = leading_zero_count; i < precision; ++i)
    {
        const limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
        *ptr = static_cast<C>(hex::digits[digit]);
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
constexpr size_t write_fixed_large(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
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
    const size_t precision_char_count = static_cast<size_t>(precision > 0) + precision;
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
        const limb_type digit = int_bits.div_extract_shrink(10, max_index);
        *(--ptr) = static_cast<C>(hex::digits[digit]);
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
constexpr size_t write_fixed_mixed(typename float_bits<F>::uint_type m_bits, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
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

    const size_t precision_char_count = static_cast<size_t>(precision > 0) + precision;
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
            *--ptr = static_cast<C>(hex::digits[digit]);
            int_bits /= 10;

        } while (int_bits);
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
        if (!raw_frac_bits)
        {
            fill_n_zeros(ptr, precision);
            return needed;
        }

        constexpr uint32_t max_shift = traits::exponent_bias + traits::mantissa_bits - 1;
        constexpr uint32_t limb_count = ((max_shift + (limb_bits - 1)) / limb_bits) + 1;

        // extract digit from the bits above
        const uint32_t keep_limbs = -shift / limb_bits;
        const uint32_t partial_bits = -shift % limb_bits;
        const limb_type digit_mask = (limb_type{ 1 } << partial_bits) - 1;

        using big_int_type = big_int<limb_type, limb_count, wide_type>;
        big_int_type frac_bits = { raw_frac_bits };

        for (size_t i = 0; i < precision; ++i)
        {
            const limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
            *ptr++ = static_cast<C>(hex::digits[digit]);
        }

        // round
        if (fmt.round)
        {
            const limb_type round_digit = frac_bits.mul_extract(10, keep_limbs, partial_bits);
            if (round_digit >= 5)
            {
                if (round_fixed_mixed<C>(buf, int_digit_count, precision))
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
constexpr size_t write_float_fixed_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;

    const size_t precision = fmt.precision == precision_unspecified ? float_fixed_default_precision : fmt.precision;

    if (fb.e_bits == 0)
    {
        if (fb.m_bits == 0)
        {
            // zero
            return _string_convert_priv::write_fixed_zero<C>(buf, buf_size, fmt, precision);
        }
        else
        {
            // subnormal
            return _string_convert_priv::write_fixed_subnormal<F, C>(fb.m_bits, buf, buf_size, fmt, precision);
        }
    }

    const int e2 = static_cast<int>(fb.e_bits) - traits::exponent_bias;
    const int shift = e2 - traits::mantissa_bits;

    if (e2 < 0)
    {
        // normal
        const int e10 = _string_convert_priv::normal_pow10<F>(e2);
        return _string_convert_priv::write_fixed_normal<F, C>(fb.m_bits, e10, -shift, buf, buf_size, fmt, precision);
    }

    if (shift >= 0)
    {
        // large integer
        const int e10 = _string_convert_priv::large_integer_pow10<F>(e2);
        return _string_convert_priv::write_fixed_large<F, C>(fb.m_bits, e10, shift, buf, buf_size, fmt, precision);
    }
    else
    {
        // int and float part
        return _string_convert_priv::write_fixed_mixed<F, C>(fb.m_bits, shift, buf, buf_size, fmt, precision);
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
constexpr size_t write_scientific_zero(C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
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
constexpr size_t write_scientific_subnormal(typename float_bits<F>::uint_type m_bits, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    int e10 = subnormal_pow10<F>(m_bits);

    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
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
        frac_bits.mul_shave_digits(leading_zero_count);
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
        buf[0] = static_cast<C>(hex::digits[digit]);
    }

    if (precision > 0)
    {
        buf[1] = fmt.decimal_point;

        C* ptr = buf + 2;
        for (size_t i = 0; i < precision; ++i)
        {
            const limb_type digit = frac_bits.mul_extract_clear_lower(10, keep_limbs, partial_bits, digit_mask);
            ptr[i] = static_cast<C>(hex::digits[digit]);
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
constexpr size_t write_scientific_normal(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
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
            frac_bits.mul_shave_digits(shave);

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

            buf[0] = static_cast<C>(hex::digits[digit]);
        }
    }

    if (precision > 0)
    {
        buf[1] = fmt.decimal_point;

        for (size_t i = 0; i < precision; ++i)
        {
            const limb_type digit = frac_bits.mul_extract_clear(10, keep_limbs, partial_bits, digit_mask);
            buf[2 + i] = static_cast<C>(hex::digits[digit]);
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
constexpr size_t write_scientific_large(typename float_bits<F>::uint_type m_bits, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
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
    const size_t digits_needed = static_cast<size_t>(precision) + 1;
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
        const limb_type digit = int_bits.div_extract_shrink(10, max_index);
        *(--back_ptr) = static_cast<C>(hex::digits[digit]);
    }

    // decimal + final digit
    {
        if (precision > 0)
        {
            *(--back_ptr) = fmt.decimal_point;
        }
        const limb_type digit = int_bits.div_extract_shrink(10, max_index);
        *(--back_ptr) = static_cast<C>(hex::digits[digit]);
    }

    // There may be 1 digit left over in rare cases
    if (int_bits.bits[0] != 0)
    {
        const limb_type digit = int_bits.div_extract(10, 0);
        VX_ASSERT(int_bits.bits[0] == 0);

        round_digit = static_cast<limb_type>(buf[leading_char_count - 1] - '0');

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
constexpr size_t write_scientific_mixed(typename float_bits<F>::uint_type m_bits, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
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

    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
    size_t exp_char_count = 1 + static_cast<size_t>(fmt.force_exp_sign) + digit_count_max3(e10);
    size_t needed = leading_char_count + fmt.force_exp_sign + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    bool exp_changed = false;

    const size_t digits_needed = static_cast<size_t>(precision) + 1;
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

        C* ptr = buf + int_digits_needed + static_cast<size_t>(precision > 0);

        // populate integer part
        for (size_t i = 1; i < int_digits_needed; ++i)
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
                *ptr++ = static_cast<C>(hex::digits[digit]);
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

//==============================================================================
// scientific format
//==============================================================================

template <typename F, typename C>
constexpr size_t write_float_scientific_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;

    const size_t precision = fmt.precision == precision_unspecified ? float_scientific_default_precision : fmt.precision;

    if (fb.e_bits == 0)
    {
        if (fb.m_bits == 0)
        {
            // zero
            return _string_convert_priv::write_scientific_zero<C>(buf, buf_size, fmt, precision);
        }
        else
        {
            // subnormal
            return _string_convert_priv::write_scientific_subnormal<F, C>(fb.m_bits, buf, buf_size, fmt, precision);
        }
    }

    const int e2 = static_cast<int>(fb.e_bits) - traits::exponent_bias;
    const int shift = e2 - traits::mantissa_bits;

    if (e2 < 0)
    {
        // normal
        const int e10 = _string_convert_priv::normal_pow10<F>(e2);
        return _string_convert_priv::write_scientific_normal<F, C>(fb.m_bits, e10, -shift, buf, buf_size, fmt, precision);
    }

    if (shift >= 0)
    {
        // large integer
        const int e10 = _string_convert_priv::large_integer_pow10<F>(e2);
        return _string_convert_priv::write_scientific_large<F, C>(fb.m_bits, e10, shift, buf, buf_size, fmt, precision);
    }
    else
    {
        // int and float part
        return _string_convert_priv::write_scientific_mixed<F, C>(fb.m_bits, shift, buf, buf_size, fmt, precision);
    }
}

//==============================================================================
// hex format
//==============================================================================

template <typename F, typename C>
constexpr size_t write_float_hex_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
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

    size_t n = 0;
    const bool subnormal = fb.is_subnormal();

    // First calulate the exponent
    const int exp = static_cast<int>(subnormal ? (1 - traits::exponent_bias) : (fb.e_bits - traits::exponent_bias));
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

    buf[n++] = static_cast<C>('0' + !subnormal);

    if (precision > 0)
    {
        buf[n++] = fmt.decimal_point;

        constexpr uint32_t mbits = traits::mantissa_bits;
        constexpr uint32_t hex_digits = (mbits + 3) / 4;
        constexpr uint32_t shift = hex_digits * 4 - mbits;
        constexpr uint32_t top_shift = (hex_digits - 1) * 4;

        size_t digits_remaining = precision;
        uint_type frac = fb.m_bits << shift;

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
constexpr size_t write_float_fixed(const F value, C* buf, const size_t buf_size, const float_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    const _string_convert_priv::float_bits<F> bits{ value };
    size_t n = 0;

    using float_write_status = _string_convert_priv::float_write_status;
    const float_write_status status = _string_convert_priv::write_float_start<F, C>(bits, buf, buf_size, fmt, n);
    if (status == float_write_status::failed)
    {
        return 0;
    }
    if (status == float_write_status::finished)
    {
        return n;
    }

    return _string_convert_priv::write_float_fixed_impl<F, C>(bits, buf + n, buf_size - n, fmt);
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr size_t write_float_scientific(const F value, C* buf, const size_t buf_size, const float_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    const _string_convert_priv::float_bits<F> bits{ value };
    size_t n = 0;

    using float_write_status = _string_convert_priv::float_write_status;
    const float_write_status status = _string_convert_priv::write_float_start<F, C>(bits, buf, buf_size, fmt, n);
    if (status == float_write_status::failed)
    {
        return 0;
    }
    if (status == float_write_status::finished)
    {
        return n;
    }

    return n + _string_convert_priv::write_float_scientific_impl<F, C>(bits, buf + n, buf_size - n, fmt);
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr size_t write_float_hex(const F value, C* buf, const size_t buf_size, const float_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    const _string_convert_priv::float_bits<F> bits{ value };
    size_t n = 0;

    using float_write_status = _string_convert_priv::float_write_status;
    const float_write_status status = _string_convert_priv::write_float_start<F, C>(bits, buf, buf_size, fmt, n);
    if (status == float_write_status::failed)
    {
        return 0;
    }
    if (status == float_write_status::finished)
    {
        return n;
    }

    buf += n;
    buf_size -= n;

    return n + _string_convert_priv::write_float_hex_impl<F, C>(bits, buf, buf_size, fmt);
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
size_t write_float(const F value, C* buf, size_t buf_size, const float_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    if (fmt.precision != _string_convert_priv::precision_unspecified && fmt.precision > _string_convert_priv::max_float_precision)
    {
        err::set(err::invalid_argument, "fmt.precision");
        return 0;
    }

    const _string_convert_priv::float_bits<F> bits{ value };
    size_t header = 0;
    size_t body = 0;

    using float_write_status = _string_convert_priv::float_write_status;
    const float_write_status status = _string_convert_priv::write_float_start<F, C>(bits, buf, buf_size, fmt, header);
    if (status == float_write_status::failed)
    {
        return 0;
    }
    if (status == float_write_status::finished)
    {
        return header;
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

    return body ? (header + body) : 0;
}

//==============================================================================

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
auto to_string(I value, const integer_format_options& fmt = {}) noexcept
{
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
    str::basic_string<C> buf(count, 0);

    do
    {
        const char c = _string_convert_priv::int_digits[uvalue % ubase];
        buf[--count] = fmt.uppercase ? to_upper(c) : c;
        uvalue /= ubase;

    } while (uvalue);

    if (sign)
    {
        buf[0] = static_cast<C>(sign);
    }

    return buf;
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
auto to_string(F value, const float_format_options<C>& fmt = {}) noexcept
{
    size_t buf_size = 24;
    str::basic_string<C> buf(buf_size, 0);

    err::clear();
    size_t n = write_float<F, C>(value, buf.data(), buf_size, fmt);
    if (err::get_code() == err::invalid_argument)
    {
        // so we don't spin for infinity
        return buf;
    }

    while (n == 0)
    {
        buf_size *= 2;
        buf.resize(buf_size);
        n = write_float<F, C>(value, buf.data(), buf_size, fmt);
    }

    buf.shrink_to_fit();
    return buf;
}

//==============================================================================
// data from hex
//==============================================================================

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t from_hex_string(const C* hex, size_t hex_size, void* buf, size_t buf_size) noexcept
{
    if (!hex || !buf)
    {
        return 0;
    }

    const size_t needed = (hex_size + 1) / 2;
    if (buf_size < needed)
    {
        return 0;
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
            return 0;
        }

        bytes[j++] = static_cast<uint8_t>((hi << 4) | lo);
    }

    // Handle a trailing half-byte
    if (i < hex_size)
    {
        const int lo = hex::value(hex[i]);

        if (lo < 0)
        {
            return 0;
        }

        bytes[j++] = static_cast<uint8_t>(lo);
    }

    return needed;
}

enum class parse_error
{
    none,
    null_pointer,
    invalid_argument,
    overflow,
};

struct parse_result
{
    size_t count = 0;
    parse_error err = parse_error::none;
};

//==============================================================================
// integer parsing
//==============================================================================

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
constexpr parse_result parse_integer(const C* str, size_t str_size, I& value, const integer_format_options& fmt = {}) noexcept
{
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

    parse_result res;

    if (!str)
    {
        res.err = parse_error::null_pointer;
        return res;
    }

    using U = typename std::make_unsigned<I>::type;
    bool negative = false;

    if (str[0] == C('+'))
    {
        ++res.count;
    }
    else if (str[0] == C('-'))
    {
        VX_IF_CONSTEXPR (!std::is_signed<I>::value)
        {
            res.err = parse_error::invalid_argument;
            return res;
        }

        negative = true;
        ++res.count;
    }

    if (res.count == str_size)
    {
        res.err = parse_error::invalid_argument;
        return res;
    }

    U uvalue = 0;

    while (res.count < str_size)
    {
        const int digit = char_to_digit(str[res.count]);
        if (digit < 0 || digit >= fmt.base)
        {
            res.err = parse_error::invalid_argument;
            return res;
        }

        uvalue = uvalue * static_cast<U>(fmt.base) + static_cast<U>(digit);
        ++res.count;
    }

    VX_IF_CONSTEXPR (std::is_signed<I>::value && negative)
    {
        // Convert back to two's complement representation.
        value = static_cast<I>(~uvalue + 1);
    }
    else
    {
        value = static_cast<I>(uvalue);
    }

    return res;
}

//==============================================================================
// fixed float parsing
//==============================================================================

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr parse_result parse_fixed_float(const C* str, size_t str_size, F& value, const float_format_options<C>& fmt = {}) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

    constexpr uint32_t max_shift = traits::exponent_bias + traits::mantissa_bits - 1;
    constexpr uint32_t limb_count = ((max_shift + (limb_bits - 1)) / limb_bits) + 1;

    parse_result res;

    if (!str)
    {
        res.err = parse_error::null_pointer;
        return res;
    }

    bool negative = false;

    if (str[0] == C('+'))
    {
        ++res.count;
    }
    else if (str[0] == C('-'))
    {
        negative = true;
        ++res.count;
    }

    if (res.count == str_size)
    {
        res.err = parse_error::invalid_argument;
        return res;
    }

    size_t int_digit_count = 0;

    // int part
    {
        while (true)
        {
            const C c = str[++res.count];

            if (c == fmt.decimal_point)
            {
                if (res.count == str_size)
                {
                    return res;
                }

                break;
            }

            if (!(static_cast<C>('0') <= c && c <= static_cast<C>('9')))
            {
                res.err = parse_error::invalid_argument;
                return res;
            }

            ++int_digit_count;
            if (res.count == str_size)
            {
                return res;
            }
        }
    }

    // frac part
    {
        // our large integer type
        using big_int_type = _string_convert_priv::big_int<limb_type, limb_count, wide_type>;
        big_int_type frac_bits;

        while (true)
        {
            const C c = str[++res.count];

            if (!(static_cast<C>('0') <= c && c <= static_cast<C>('9')))
            {
                res.err = parse_error::invalid_argument;
                return res;
            }

            const limb_type digit = static_cast<limb_type>(c - static_cast<C>('0'));
            frac_bits.insert_digit(digit);

            if (res.count == str_size)
            {
                break;
            }
        }
    }
}

} // namespace str
} // namespace vx
