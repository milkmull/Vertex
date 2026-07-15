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

        *buf++ = upper ? to_upper_ascii_unchecked(c1) : c1;
        *buf++ = upper ? to_upper_ascii_unchecked(c2) : c2;
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
constexpr int char_to_digit(C c, int base = 10) noexcept
{
    int digit = -1;

    if (c >= static_cast<C>('0') && c <= static_cast<C>('9'))
    {
        digit = static_cast<int>(c - static_cast<C>('0'));
    }
    else
    {
        c = to_lower_ascii_unchecked(c);
        if (c >= static_cast<C>('a') && c <= static_cast<C>('z'))
        {
            digit = static_cast<int>(c - static_cast<C>('a')) + base;
        }
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

    VX_STATIC_ASSERTmSG(type_traits::is_char<C>::value, "C must be a character type");
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

// Returns ceil(bits * log10(2)) using the approximation
// log10(2) ≈ 30103 / 100000. This is a slight overestimate
// (~0.0034% from the approximation, plus rounding up).
inline constexpr size_t ceil_log10_pow2(size_t bits) noexcept
{
    return (bits * 30103 + 99999) / 100000;
}

// ceil(log2(10^digit_count)) == ceil(digit_count * log2(10)).
// Uses the integer approximation log2(10) <= 1233/371 (as used by the
// Ryu float-parsing algorithm), which slightly overestimates log2(10)
// (3.32345... vs the true 3.32193...), giving a safe upper bound on
// the number of bits required.
constexpr size_t ceil_log2_pow10(size_t digits) noexcept
{
    return (digits * 1233 + 370) / 371;
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

    constexpr void mul(limb_type x) noexcept
    {
        limb_type carry = 0;
        for (size_t j = 0; j < limb_count; ++j)
        {
            const wide_type product = (static_cast<wide_type>(bits[j]) * static_cast<wide_type>(x)) + static_cast<wide_type>(carry);
            bits[j] = static_cast<limb_type>(product);
            carry = static_cast<limb_type>(product >> limb_bits);
        }
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

    constexpr void mul_shave_digits(size_t count) noexcept
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

    constexpr limb_type div_extract(limb_type x, limb_type r) noexcept
    {
        constexpr uint32_t half = limb_bits / 2;
        constexpr limb_type half_mask = (limb_type{ 1 } << half) - 1;

        limb_type remainder = r;

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

        for (size_t i = limb_count; i-- > limbshift + 1;)
        {
            bits[i] =
                (bits[i - limbshift] << bitshift) |
                (bits[i - limbshift - 1] >> (limb_bits - bitshift));
        }

        bits[limbshift] = bits[0] << bitshift;

        for (size_t i = 0; i < limbshift; ++i)
        {
            bits[i] = 0;
        }
    }

    constexpr void shr(uint32_t shift) noexcept
    {
        const uint32_t limbshift = shift / limb_bits;
        const uint32_t bitshift = shift % limb_bits;

        for (size_t i = 0; i + limbshift + 1 < limb_count; ++i)
        {
            bits[i] =
                (bits[i + limbshift] >> bitshift) |
                (bits[i + limbshift + 1] << (limb_bits - bitshift));
        }

        bits[limb_count - limbshift - 1] =
            bits[limb_count - 1] >> bitshift;

        for (size_t i = limb_count - limbshift; i < limb_count; ++i)
        {
            bits[i] = 0;
        }
    }

    constexpr uint32_t countr_zero() const noexcept
    {
        uint32_t count = 0;

        for (size_t i = 0; i < limb_count; ++i)
        {
            if (bits[i])
            {
                return count + bit::countr_zero(bits[i]);
            }

            count += limb_bits;
        }

        return count;
    }

    constexpr uint32_t countl_zero() const noexcept
    {
        uint32_t count = 0;

        for (size_t i = limb_count; i-- > 0;)
        {
            if (bits[i])
            {
                return count + bit::countl_zero(bits[i]);
            }

            count += limb_bits;
        }

        return count;
    }

    constexpr limb_type div_shave_digits(size_t count) noexcept
    {
        limb_type remainder = 0;

        while (count >= 9)
        {
            remainder = div_extract(1000000000u, remainder);
            count -= 9;
        }

        if (count)
        {
            remainder = div_extract(pow10_u32(count), remainder);
        }

        return remainder;
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
    static constexpr uint32_t maxshift = traits::exponent_bias + traits::mantissa_bits - 1;
    static constexpr uint32_t limb_count = ((maxshift + (limb_bits - 1)) / limb_bits) + 1;

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
        bits.mul_shave_digits(count);
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

    static constexpr uint32_t maxshift = base::maxshift;
    static constexpr uint32_t total_bits = maxshift + traits::digits;
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
    }

    return needed;
}

template <typename F, typename C>
constexpr size_t write_fixed_normal(typename float_bits<F>::uint_type m2, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
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
constexpr size_t write_fixed_large(typename float_bits<F>::uint_type m2, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
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
constexpr size_t write_fixed_mixed(typename float_bits<F>::uint_type m2, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;

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
constexpr size_t write_float_fixed_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
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
            const int shift = static_cast<int>(traits::exponent_bias + traits::mantissa_bits - 1);
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
constexpr size_t write_scientific_normal(typename float_bits<F>::uint_type m2, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
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
constexpr size_t write_scientific_large(typename float_bits<F>::uint_type m2, int e10, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
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
constexpr size_t write_scientific_mixed(typename float_bits<F>::uint_type m2, int shift, C* buf, const size_t buf_size, const float_format_options<C>& fmt, const size_t precision) noexcept
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
constexpr size_t write_float_scientific_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
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
            const int shift = static_cast<int>(traits::exponent_bias + traits::mantissa_bits - 1);
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
constexpr size_t write_float_hex_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_format_options<C>& fmt) noexcept
{
    VX_STATIC_ASSERTmSG((!std::is_same<F, long double>::value), "long double not supported");

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
constexpr size_t write_float_fixed(const F value, C* buf, const size_t buf_size, const float_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERTmSG((!std::is_same<F, long double>::value), "long double not supported");

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

    return n + _string_convert_priv::write_float_fixed_impl<F, C>(bits, buf + n, buf_size - n, fmt);
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr size_t write_float_scientific(const F value, C* buf, const size_t buf_size, const float_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERTmSG((!std::is_same<F, long double>::value), "long double not supported");

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
constexpr size_t write_float_hex(const F value, C* buf, size_t buf_size, const float_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERTmSG((!std::is_same<F, long double>::value), "long double not supported");

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
    VX_STATIC_ASSERTmSG((!std::is_same<F, long double>::value), "long double not supported");

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
    out_of_range,
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
    bool is_negative = false;

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

        is_negative = true;
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

    VX_IF_CONSTEXPR (std::is_signed<I>::value && is_negative)
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

namespace _string_convert_priv {

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

template <typename F>
constexpr void assemble_float_zero(const bool is_negative, F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    uint_type sign_component = is_negative;
    sign_component <<= traits::sign_shift;
    value = bit::bit_cast<F>(sign_component);
}

template <typename F>
constexpr void assemble_float_infinity(const bool is_negative, F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    uint_type sign_component = is_negative;
    sign_component <<= traits::sign_shift;

    constexpr uint_type exponent_component = (traits::exponent_mask >> traits::exponent_shift);
    value = bit::bit_cast<F>(sign_component | exponent_component);
}

template <typename F>
constexpr void assemble_float(const bool is_negative, const typename float_bits<F>::uint_type mantissa, const int exponent, F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    uint_type sign_component = static_cast<uint_type>(is_negative);
    sign_component <<= traits::sign_shift;

    uint_type exponent_component = static_cast<uint_type>(exponent + (traits::exponent_bias - 1));
    exponent_component <<= traits::exponent_shift;

    value = bit::bit_cast<F>(sign_component | (exponent_component + mantissa));
}

template <typename F>
constexpr parse_error assemble_float_shifted(
    typename float_bits<F>::uint_type mantissa,
    int exponent,
    bool is_negative,
    bool has_zero_tail,
    F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    constexpr int min_binary_exponent = traits::min_exponent - 1;
    constexpr int max_binary_exponent = traits::max_exponent - 1;

    // position of the highest set bit (0-based from LSB)
    const uint32_t mantissa_bits = bit::bit_log2(mantissa);
    const int shift = static_cast<int>(traits::mantissa_bits) - static_cast<int>(mantissa_bits);
    const int normal_exponent = exponent - shift;

    if (normal_exponent > max_binary_exponent)
    {
        // Overflow to infinity
        assemble_float_infinity<F>(is_negative, value);
        return parse_error::out_of_range;
    }

    parse_error err = parse_error::none;

    if (normal_exponent < min_binary_exponent)
    {
        // maybe representable as a subnormal, or underflow to zero
        const int subnormal_shift = exponent - min_binary_exponent;
        if (subnormal_shift < 0)
        {
            mantissa = shr_round(mantissa, static_cast<uint32_t>(-subnormal_shift), has_zero_tail);
            if (mantissa == 0)
            {
                // underflow to 0 (still valid)
                err = parse_error::out_of_range;
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
            if (mantissa > traits::mantissa_mask && normal_exponent == max_binary_exponent)
            {
                // overflow to infinity (still valid)
                err = parse_error::out_of_range;
            }
        }
        else
        {
            mantissa <<= shift;
        }
    }

    exponent = normal_exponent;
    assemble_float<F>(is_negative, mantissa, exponent, value);
    return err;
}

template <typename F, typename I>
constexpr parse_error assemble_float_large(
    const I& integer_value,
    uint32_t precision_bits,
    bool is_negative,
    bool has_nonzero_fractional_part,
    F& value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    constexpr int base_exponent = traits::exponent_bias - 1;
    constexpr size_t mantissa_bits = sizeof(uint_type) * CHAR_BIT;
    constexpr size_t limb_bits = I::limb_bits;

    // Extract the top bits to fill the uint type
    uint_type mantissa;
    int exponent = base_exponent;
    bool has_zero_tail = !has_nonzero_fractional_part;

    if (precision_bits <= limb_bits)
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

struct float_string_info
{
    bool is_negative;
    int exponent;

    size_t int_digit_start;
    size_t int_digit_count;

    size_t frac_digit_start;
    size_t frac_digit_count;
};

template <typename F, typename C>
constexpr parse_result string_to_float(const C* str, size_t str_size, F& value, const float_string_info& info, bool has_zero_tail) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    // To generate an N bit mantissa we require N + 1 bits of precision. The extra bit is used to correctly round
    // the mantissa (if there are fewer bits than this available, then that's totally okay;
    // in that case we use what we have and we don't need to round).
    constexpr uint32_t required_bits_of_precision = static_cast<uint32_t>(traits::mantissa_bits + 1);

    // The input is of the form 0.mantissa * 10^exponent, where 'mantissa' are the decimal digits of the mantissa
    // and 'exponent' is the decimal exponent. We decompose the mantissa into two parts: an integer part and a
    // fractional part. If the exponent is positive, then the integer part consists of the first 'exponent' digits,
    // or all present digits if there are fewer digits. If the exponent is zero or negative, then the integer part
    // is empty. In either case, the remaining digits form the fractional part of the mantissa.
    const uint32_t positive_exponent = static_cast<uint32_t>(std::max(0, info.exponent));
    const uint32_t int_digits_present = std::min(positive_exponent, info.int_digit_count);
    const uint32_t int_digits_missing = positive_exponent - int_digits_present;
    const C* const int_first = str + info.int_digit_start;

    const C* const frac_first = str + info.frac_digit_start;
    const uint32_t fractional_digits_present = info.frac_digit_count;
}

} // namespace _string_convert_priv

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr parse_result parse_fixed_float(const C* str, size_t str_size, F& value, const float_format_options<C>& fmt = {}) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    using limb_type = uint32_t;
    using wide_type = uint64_t;
    constexpr uint32_t limb_bits = sizeof(limb_type) * CHAR_BIT;
    constexpr uint32_t max_mantissa_digits = _string_convert_priv::max_decimal_mantissa_digits<F>();

    if (str_size == 0)
    {
        return { 0, parse_error::invalid_argument };
    }

    const bool is_hex = (fmt.format == float_format::hex);
    const int base{ is_hex ? 16 : 10 };

    size_t i = 0;
    bool is_negative = false;

    if (str[0] == C('-') || str[0] == C('+'))
    {
        is_negative = str[0] == C('-');
        ++i;
    }

    if (i == str_size)
    {
        return { i, parse_error::invalid_argument };
    }

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
        const auto digit = hex::value(str[i]);
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

    // Raw span length -- deliberately NOT trimmed yet. This is what the
    // position exponent below is measured against, and trailing-zero
    // trimming must not affect that (e.g. "100" still has its leading digit
    // at position 2, same as untrimmed).
    const size_t int_digit_count = i - int_digit_start;

    // fractional part
    //===========================================

    size_t frac_digit_start = i;
    size_t frac_digit_count = 0;
    size_t frac_leading_zeros = 0;
    bool has_decimal_point = false;

    if (i < str_size && str[i] == fmt.decimal_point)
    {
        ++i;
        has_decimal_point = true;

        // Only trim fractional leading zeros if the integer part had
        // no significant digits (e.g. "0.000123" or ".000123").
        if (int_digit_count == 0)
        {
            while (i < str_size && str[i] == C('0'))
            {
                ++i;
            }
        }

        frac_leading_zeros = i - frac_digit_start;
        frac_digit_start = i;

        while (i < str_size)
        {
            const auto digit = hex::value(str[i]);
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

        frac_digit_count = i - frac_digit_start;
    }

    // Total characters consumed so far, minus the decimal point itself
    // (if present), tells us whether any digit was actually seen —
    // independent of whether trimming later reduced the significant
    // digit counts to zero (e.g. "0", "0.0", "00.000").
    const size_t chars_consumed = i - int_digit_start;
    const bool saw_any_digit = chars_consumed > (has_decimal_point ? 1 : 0);

    if (!saw_any_digit)
    {
        return { i, parse_error::invalid_argument };
    }

    // Position of the most significant digit, in units of the mantissa's
    // own base (digit position -- not yet scaled to whatever base the
    // exponent suffix uses). Uses the RAW int_digit_count, not the trimmed
    // one -- trailing-zero trimming must not shift where the first digit
    // sits. Examples (decimal):
    //   "03333.111" -> 3   (3.333111e3)
    //   "00000.111" -> -1  (1.11e-1)
    //   "00000.001" -> -3  (1e-3)
    //===========================================
    int exponent_adjustment;
    if (int_digit_count > 0)
    {
        exponent_adjustment = static_cast<int>(int_digit_count) - 1;
    }
    else
    {
        // int_digit_count == 0 here implies frac_leading_zeros > 0, since
        // saw_any_digit already ruled out "no digits at all".
        exponent_adjustment = -static_cast<int>(frac_leading_zeros) - 1;
    }

    const char exponent_prefix{ is_hex ? 'p' : 'e' };
    int exponent = 0;
    bool exponent_is_negative = false;
    bool exp_overflow = false;

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

        int explicit_exponent = 0;

        while (j < str_size)
        {
            const auto digit = (str[j] - C('0'));
            if (digit >= 10)
            {
                break;
            }

            if (explicit_exponent < INT_MAX / 10 || (explicit_exponent == INT_MAX / 10 && digit <= INT_MAX % 10))
            {
                explicit_exponent = explicit_exponent * 10 + digit;
            }
            else
            {
                exp_overflow = true;
            }

            ++j;
            i = j;
        }

        if (exponent_is_negative)
        {
            explicit_exponent = -explicit_exponent;
        }
    }

    // chars_format::scientific requires the exponent part to actually be
    // present (chars_format::hex has no such requirement here).
    if (fmt.format == float_format::scientific && i == exponent_scan_start)
    {
        return { i, parse_error::invalid_argument };
    }

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
        size_t int_trimmed_count = int_digit_count;
        if (frac_digit_count == 0)
        {
            while (int_trimmed_count > 0 && str[int_digit_start + int_trimmed_count - 1] == C('0'))
            {
                --int_trimmed_count;
            }
        }

        const size_t total_digit_count = int_trimmed_count + frac_digit_count;

        // Zero fast-path, deferred until after exponent scanning so the reported
        // position is correct for inputs like "0e999" or "0.0e-5".
        if (total_digit_count == 0)
        {
            _string_convert_priv::assemble_float_zero(is_negative, value);
            return { i, parse_error::none };
        }
    }

    if (exp_overflow)
    {
        if (exponent_is_negative)
        {
            _string_convert_priv::assemble_float_zero(is_negative, value);
        }
        else
        {
            _string_convert_priv::assemble_float_infinity(is_negative, value);
        }

        return { i, parse_error::out_of_range };
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
            return { i, parse_error::out_of_range }; // Overflow example: "1e+9999"
        }

        if (exponent < minimum_temporary_decimal_exponent ||
            exponent_adjustment < minimum_temporary_decimal_exponent / exponent_adjustment_multiplier)
        {
            _string_convert_priv::assemble_float_zero(is_negative, value);
            return { i, parse_error::out_of_range }; // Underflow example: "1e-9999"
        }
    }

    exponent += exponent_adjustment * exponent_adjustment_multiplier;
    {
        if (exponent > maximum_temporary_decimal_exponent)
        {
            _string_convert_priv::assemble_float_infinity(is_negative, value);
            return { i, parse_error::out_of_range };
        }

        if (exponent < minimum_temporary_decimal_exponent)
        {
            _string_convert_priv::assemble_float_zero(is_negative, value);
            return { i, parse_error::out_of_range };
        }
    }

    const _string_convert_priv::float_string_info info{
        is_negative,
        exponent,
        int_digit_start,
        int_digit_count,
        frac_digit_start,
        frac_digit_count
    };

    if (is_hex)
    {
    }
    else
    {
        const auto err = _string_convert_priv::string_to_float(str, str_size, value, info, has_zero_tail);
        return { i, err };
    }
}

} // namespace str
} // namespace vx
