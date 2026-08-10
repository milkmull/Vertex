#pragma once

#include "vertex/std/_string_convert/string_convert_common.hpp"

namespace vx {
namespace strconv {

//==============================================================================
// result types
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

        if (hi == hex::invalid_value || lo == hex::invalid_value)
        {
            return { 0, from_string_error::invalid_argument };
        }

        bytes[j++] = static_cast<uint8_t>((hi << 4) | lo);
    }

    // Handle a trailing half-byte
    if (i < hex_size)
    {
        const int lo = hex::value(hex[i]);

        if (lo == hex::invalid_value)
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

enum : unsigned char
{
    invalid_digit = 255
};

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr unsigned char char_to_digit(C c, int base = 10) noexcept
{
    unsigned char digit = invalid_digit;

    if (static_cast<C>('0') <= c && c <= static_cast<C>('9'))
    {
        digit = static_cast<unsigned char>(c - static_cast<C>('0'));
    }
    else
    {
        c = str::to_lower_ascii_unchecked(c);
        if (static_cast<C>('a') <= c && c <= static_cast<C>('z'))
        {
            digit = static_cast<unsigned char>(c - static_cast<C>('a')) + 10;
        }
    }

    return (digit < base) ? digit : invalid_digit;
}

//==============================================================================
// integer
//==============================================================================

namespace _strconv_priv {

template <typename I, typename C, bool Sign = true>
constexpr from_string_result parse_integer_base10_impl(const C* s, size_t size, I& value) noexcept
{
    using U = typename std::make_unsigned<I>::type;
    VX_MAYBE_UNUSED bool is_negative = false;
    size_t i = 0;

    VX_IF_CONSTEXPR (Sign)
    {
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
    }

    constexpr U uint_max = static_cast<U>(-1);
    constexpr U int_max = static_cast<U>(uint_max >> 1);
    constexpr U abs_int_min = static_cast<U>(int_max + 1);

    U risky_value;
    U max_digit;

    VX_IF_CONSTEXPR (std::is_signed<I>::value)
    {
        if (is_negative)
        {
            risky_value = static_cast<U>(abs_int_min / 10);
            max_digit = static_cast<U>(abs_int_min % 10);
        }
        else
        {
            risky_value = static_cast<U>(int_max / 10);
            max_digit = static_cast<U>(int_max % 10);
        }
    }
    else
    {
        risky_value = static_cast<U>(uint_max / 10);
        max_digit = static_cast<U>(uint_max % 10);
    }

    const size_t start = i;
    bool overflow = false;

    U uvalue = 0;

    while (i < size)
    {
        const auto digit = static_cast<unsigned char>(s[i] - static_cast<C>('0'));
        if (digit >= 10)
        {
            break;
        }

        if (uvalue < risky_value || (uvalue == risky_value && digit <= max_digit))
        {
            uvalue = static_cast<U>(uvalue * 10 + digit);
        }
        else
        {
            overflow = true;
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

template <typename I, typename C, bool Sign = true, bool Prefix = false>
constexpr from_string_result parse_integer_impl(const C* s, size_t size, I& value, const int base) noexcept
{
    VX_ASSERT(2 <= base);
#if defined(VX_STRING_CONVERT_TO_STRING_BASE_36_SUPPORT)
    VX_ASSERT(base <= 36);
#else
    VX_ASSERT(base <= 16);
#endif

    using U = typename std::make_unsigned<I>::type;
    VX_MAYBE_UNUSED bool is_negative = false;
    size_t i = 0;

    VX_IF_CONSTEXPR (Sign)
    {
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
    }

    VX_IF_CONSTEXPR (Prefix)
    {
        if ((size - i >= 2) && s[i] == C('0'))
        {
            const C x = s[i + 1];

            switch (base)
            {
                case 2:
                {
                    if (x == C('b') || x == C('B'))
                    {
                        i += 2;
                    }
                    break;
                }
                case 16:
                {
                    if (x == C('x') || x == C('X'))
                    {
                        i += 2;
                    }
                    break;
                }
            }
        }
    }

    constexpr U uint_max = static_cast<U>(-1);
    constexpr U int_max = static_cast<U>(uint_max >> 1);
    constexpr U abs_int_min = static_cast<U>(int_max + 1);

    U risky_value;
    U max_digit;

    VX_IF_CONSTEXPR (std::is_signed<I>::value)
    {
        if (is_negative)
        {
            risky_value = static_cast<U>(abs_int_min / base);
            max_digit = static_cast<U>(abs_int_min % base);
        }
        else
        {
            risky_value = static_cast<U>(int_max / base);
            max_digit = static_cast<U>(int_max % base);
        }
    }
    else
    {
        risky_value = static_cast<U>(uint_max / base);
        max_digit = static_cast<U>(uint_max % base);
    }

    const size_t start = i;
    bool overflow = false;

    U uvalue = 0;

    while (i < size)
    {
        const auto digit = char_to_digit(s[i], base);
        if (digit >= base)
        {
            break;
        }

        if (uvalue < risky_value || (uvalue == risky_value && digit <= max_digit))
        {
            uvalue = static_cast<U>(uvalue * base + digit);
        }
        else
        {
            overflow = true;
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

} // namespace _strconv_priv

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
constexpr from_string_result parse_integer(const C* s, size_t size, I& value, const int base = 10) noexcept
{
    return _strconv_priv::parse_integer_impl<I, C>(s, size, value, base);
}

template <typename I, bool Sign = true, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
constexpr from_string_result parse_integer_base10(const C* s, size_t size, I& value) noexcept
{
    return _strconv_priv::parse_integer_base10_impl<I, C, Sign>(s, size, value);
}

//==============================================================================
// float
//==============================================================================

template <typename C = char>
struct float_from_string_format_options
{
    float_format format = float_format::general;

    VX_STATIC_ASSERT_MSG(type_traits::is_char<C>::value, "C must be a character type");
    C decimal_point = static_cast<C>('.');
};

//==============================================================================
// fixed float parsing
//==============================================================================

namespace _strconv_priv {

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
    using traits = float_traits<F>;
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

public:

    constexpr bool div_pow10(size_t count) noexcept
    {
        limb_type remainder_bits = 0;

        size_t top_limb = big_int_type::limb_count - 1;
        shrink(top_limb);

        while (count >= 9)
        {
            remainder_bits |= bits.div_extract(1000000000u, top_limb);
            count -= 9;
            shrink(top_limb);
        }

        if (count)
        {
            remainder_bits |= bits.div_extract(pow10_u32(count), top_limb);
        }

        return remainder_bits == 0;
    }
};

template <typename F>
constexpr uint32_t max_decimal_mantissa_digits() noexcept
{
    using traits = float_traits<F>;

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
    using traits = float_traits<F>;
    using uint_type = typename traits::uint_type;

    uint_type sign_component = is_negative;
    sign_component <<= traits::sign_bit_shift;
    value = bit::bit_cast<F>(sign_component);
}

template <typename F>
constexpr void assemble_float_infinity(const bool is_negative, F& value) noexcept
{
    using traits = float_traits<F>;
    using uint_type = typename traits::uint_type;

    uint_type sign_component = is_negative;
    sign_component <<= traits::sign_bit_shift;

    constexpr uint_type exponent_component = traits::exponent_field_mask;
    value = bit::bit_cast<F>(sign_component | exponent_component);
}

template <typename F>
constexpr void assemble_float_nan(const bool is_negative, const bool is_quiet, F& value) noexcept
{
    using traits = float_traits<F>;
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
    using traits = float_traits<F>;
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
    using traits = float_traits<F>;
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
    using traits = float_traits<F>;
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

    //==================================

    constexpr C get_char(size_t i) const noexcept
    {
        return i < int_digit_count ? int_first[i] : frac_first[i - int_digit_count];
    }

    constexpr C next_char() noexcept
    {
        return get_char(n++);
    }

    //==================================

    constexpr auto get_digit(size_t i) const noexcept
    {
        const auto c = get_char(i);
        return c - static_cast<C>('0');
    }

    constexpr auto get_hex_digit(size_t i) const noexcept
    {
        const auto c = get_char(i);
        return hex::value_unchecked(c);
    }

    //==================================

    constexpr auto next_digit() noexcept
    {
        return get_digit(n++);
    }

    constexpr auto next_hex_digit() noexcept
    {
        return get_hex_digit(n++);
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

    // Accumulate bits into the mantissa buffer_type
    size_t total_digits = stream.total_digits() + 1;
    while (--total_digits && mantissa <= traits::mantissa_with_implicit_bit_mask)
    {
        mantissa *= 16;
        mantissa += stream.next_hex_digit();
        exponent -= 4; // The exponent is in binary; log2(16) == 4
    }

    while (has_zero_tail && total_digits)
    {
        has_zero_tail = stream.next_char() == C('0');
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

template <typename C>
constexpr bool is_nchar(C c) noexcept
{
    if (c == static_cast<C>('_'))
    {
        return true;
    }

    if (static_cast<C>('0') <= c && c <= static_cast<C>('9'))
    {
        return true;
    }

    c = str::to_lower_ascii_unchecked(c);
    if (static_cast<C>('a') <= c && c <= static_cast<C>('z'))
    {
        return true;
    }

    return false;
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
#if defined(VX_STRING_CONVERT_IND_NAN_SUPPORT) || defined(VX_STRING_CONVERT_SNAN_SUPPORT)

            if (str[j] == static_cast<C>(')'))
            {
                // definitely nan(n-char-sequence[opt])
                const size_t seq_len = j - seq_begin;
                i = j + 1;

    #if defined(VX_STRING_CONVERT_IND_NAN_SUPPORT)

                if (seq_len == 3 && starts_with_case_insensitive(str + seq_begin, seq_len, ind, 3))
                {
                    // definitely nan(ind)
                    // The UCRT considers indeterminate NaN to be negative quiet NaN with no payload bits set.
                    // It parses "nan(ind)" and "-nan(ind)" identically.
                    is_negative = true;
                }

    #endif

    #if defined(VX_STRING_CONVERT_SNAN_SUPPORT)

                if (seq_len == 4 && starts_with_case_insensitive(str + seq_begin, seq_len, snan, 4))
                {
                    // definitely nan(snan)
                    is_quiet = false;
                }

    #endif

                break;
            }

#endif

            const C c = str[j];
            if (!is_nchar(c))
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
    using traits = float_traits<F>;
    constexpr size_t max_mantissa_digits = float_reading_traits_base::max_decimal_digits;

    const bool is_hex = fmt.format == float_format::hex;
    const int base{ is_hex ? 16 : 10 };

    const size_t digit_parse_start = i;
    size_t digit_count = 0;
    unsigned int tail_bits = 0;

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
            tail_bits |= digit;
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
                tail_bits |= digit;
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

    // The exponent adjustment holds the number of digits in the mantissa buffer_type that appeared before the radix point.
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

        const size_t exponent_digit_start = j;

        while (j < str_size)
        {
            const auto digit = static_cast<unsigned char>(str[j] - C('0'));
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

        // Only commit the exponent if at least one digit was parsed.
        if (j != exponent_digit_start)
        {
            i = j;

            if (exponent_is_negative)
            {
                exponent = -exponent;
            }
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
        size_t j = frac_digit_start + frac_digit_count - 1;

        // Trim trailing zeros from the fractional span first...
        while (frac_digit_count > 0 && str[j] == C('0'))
        {
            --frac_digit_count;
            --j;
        }

        // ...then, only if the fractional part vanished entirely (absent, or
        // turned out to be all zeros), cascade the trim into the tail of the
        // integer span too -- "100" and "100.00" both need to end up with a
        // single significant digit. "100.5" must NOT hit this branch, since
        // frac_digit_count is 1 there (the "00" in "100" stays untouched --
        // those are internal digits, not trailing ones).
        if (frac_digit_count == 0)
        {
            j = int_digit_start + int_digit_count - 1;
            while (int_digit_count > 0 && str[j] == C('0'))
            {
                --int_digit_count;
                --j;
            }
        }

        // Zero fast-path, deferred until after exponent scanning so the reported
        // position is correct for inputs like "0e999" or "0.0e-5".
        const size_t total_digit_count = int_digit_count + frac_digit_count;
        if (total_digit_count == 0)
        {
            assemble_float_zero(is_negative, value);
            return { i, from_string_error::none };
        }
    }

    //===========================================

    if (exponent_overflow)
    {
        if (exponent_is_negative)
        {
            assemble_float_zero(is_negative, value);
        }
        else
        {
            assemble_float_infinity(is_negative, value);
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
            assemble_float_infinity(is_negative, value);
            return { i, from_string_error::out_of_range }; // Overflow example: "1e+9999"
        }

        if (exponent < minimum_temporary_decimal_exponent ||
            exponent_adjustment < minimum_temporary_decimal_exponent / exponent_adjustment_multiplier)
        {
            assemble_float_zero(is_negative, value);
            return { i, from_string_error::out_of_range }; // Underflow example: "1e-9999"
        }
    }

    exponent += exponent_adjustment * exponent_adjustment_multiplier;
    {
        if (exponent > maximum_temporary_decimal_exponent)
        {
            assemble_float_infinity(is_negative, value);
            return { i, from_string_error::out_of_range };
        }

        if (exponent < minimum_temporary_decimal_exponent)
        {
            assemble_float_zero(is_negative, value);
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
        err = string_to_float_hex(stream, value, is_negative, exponent, tail_bits == 0);
    }
    else
    {
        err = string_to_float_decimal(stream, value, is_negative, exponent, tail_bits == 0);
    }

    return { i, err };
}

template <typename F, typename C>
constexpr from_string_result parse_float_body(
    const C* str,
    size_t str_size,
    size_t i,
    bool is_negative,
    F& value,
    const float_from_string_format_options<C>& fmt) noexcept
{
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
        return _strconv_priv::parse_float_impl(str, str_size, value, fmt, is_negative, i);
    }
    if (folded_start == 'i')
    {
        // possibly inf
        return _strconv_priv::parse_infinity(str, str_size, value, is_negative, i);
    }
    if (folded_start == 'n')
    {
        // possibly nan
        return _strconv_priv::parse_nan(str, str_size, value, is_negative, i);
    }

    // definitely invalid
    return { 0, from_string_error::invalid_argument };
}

template <typename F, typename C, bool Prefix>
constexpr from_string_result parse_float_start(
    const C* str,
    size_t str_size,
    F& value,
    const float_from_string_format_options<C>& fmt) noexcept
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

    VX_IF_CONSTEXPR (Prefix)
    {
        if ((str_size - i >= 2) &&
            (str[i] == C('0')) &&
            (str[i + 1] == C('x') || str[i + 1] == C('X')))
        {
            i += 2;
        }
    }

    return parse_float_body(str, str_size, i, is_negative, value, fmt);
}

} // namespace _strconv_priv

template <typename F, bool Prefix = false, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr from_string_result parse_float(const C* str, size_t str_size, F& value, const float_from_string_format_options<C>& fmt = {}) noexcept
{
    return _strconv_priv::parse_float_start<F, C, Prefix>(str, str_size, value, fmt);
}

//==============================================================================
// from string
//==============================================================================

template <typename I, typename C, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
from_string_result from_string(const C* s, size_t size, I& out, const int base = 10) noexcept
{
    return parse_integer(s, size, out, base);
}

template <typename I, typename C, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
from_string_result from_string(const C* s, I& out, const int base = 10) noexcept
{
    const size_t size = str::length(s);
    return parse_integer(s, size, out, base);
}

template <typename I, typename S, VX_REQUIRES(std::is_integral<I>::value&& str::is_string_like<S>::value)>
from_string_result from_string(const S& s, I& out, const int base = 10) noexcept
{
    return parse_integer(s.data(), s.size(), out, base);
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

template <typename F, typename S, VX_REQUIRES(std::is_floating_point<F>::value&& str::is_string_like<S>::value)>
from_string_result from_string(const S& s, F& out, const float_from_string_format_options<typename S::value_type>& fmt = {}) noexcept
{
    return parse_float(s.data(), s.size(), out, fmt);
}

} // namespace strconv
} // namespace vx
