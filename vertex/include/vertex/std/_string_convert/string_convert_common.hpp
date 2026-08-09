#pragma once

#include "vertex/config/assert.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/math/float_traits.hpp"
#include "vertex/std/hex.hpp"
#include "vertex/std/string_utils.hpp"
#include "vertex/util/bit.hpp"

namespace vx {
namespace strconv {

enum class float_format : char
{
    general = 'g',
    fixed = 'f',
    scientific = 'e',
    hex = 'a',
};

namespace _strconv_priv {

template <typename C>
constexpr C make_case_mask(bool uppercase) noexcept
{
    return uppercase ? ~C{ 0x20 } : ~C{ 0 };
}

template <typename C>
constexpr C alnum_to_upper(C c) noexcept
{
    if (c <= static_cast<C>('9'))
    {
        return c;
    }

    return str::to_upper_ascii_unchecked(c);
}

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

#if defined(VX_STRING_CONVERT_TO_STRING_BASE_36_SUPPORT)

static constexpr const char base_36_digits[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z'
};

#endif

template <typename U>
inline constexpr char get_integer_digit(U x) noexcept
{
#if defined(VX_STRING_CONVERT_TO_STRING_BASE_36_SUPPORT)

    return base_36_digits[x];

#else

    return hex::digits[x];

#endif // VX_STRING_CONVERT_TO_STRING_BASE_36_SUPPORT
}

enum : uint32_t
{
    precision_unspecified = std::numeric_limits<uint32_t>::max(),

    float_fixed_default_precision = 6,
    float_scientific_default_precision = 6,

    float_hex_default_precision = 6,
    double_hex_default_precision = 13,

    max_float_precision = 1'000'000
};

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
    // should be exact for all pow 10 through at least 5000
    return static_cast<uint32_t>((static_cast<uint64_t>(e) * 14267572527ULL) >> 32) + 1;
}

template <typename F>
constexpr uint32_t subnormal_pow10(const typename float_traits<F>::uint_type m_bits) noexcept
{
    // this estimate may be 1 less than the actual power of 10
    using traits = float_traits<F>;
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
constexpr void fill_n_zeros(C* buf, size_t n) noexcept
{
    mem::fill_range(buf, n, C('0'));
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
        limb_type carry_bits = 0;

        while (count >= 9)
        {
            carry_bits |= mul(1000000000u);
            count -= 9;
        }

        if (count)
        {
            carry_bits |= mul(pow10_u32(count));
        }

        return carry_bits == 0;
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
            mem::move_range(bits + limbshift, bits, limb_count - limbshift);
        }
        else
        {
            const uint32_t rshift = limb_bits - bitshift;

            size_t src = limb_count - limbshift;
            size_t dst = limb_count;

            while (--src > 0)
            {
                bits[--dst] = (bits[src] << bitshift) | (bits[src - 1] >> rshift);
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

} // namespace _strconv_priv
} // namespace strconv
} // namespace vx
