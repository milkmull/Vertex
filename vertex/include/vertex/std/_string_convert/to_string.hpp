#pragma once

#include "vertex/std/_string_convert/string_convert_common.hpp"

namespace vx {
namespace strconv {

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
// hex
//==============================================================================

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
to_string_result to_hex_string(const void* data, const size_t size, C* buf, const size_t buf_size, const bool uppercase = false) noexcept
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

        *buf++ = uppercase ? _strconv_priv::alnum_to_upper(c1) : c1;
        *buf++ = uppercase ? _strconv_priv::alnum_to_upper(c2) : c2;
    }

    return { needed, to_string_error::none };
}

template <typename S, VX_REQUIRES(str::is_mutable_string_like<S>::value)>
to_string_result to_hex_string(const void* data, const size_t size, S& out) noexcept
{
    using C = typename S::value_type;
    out.resize(2 * size);
    return to_hex_string<C>(data, size, out.data(), out.size());
}

//==============================================================================
// integer
//==============================================================================

struct integer_to_string_format_options
{
    int base = 10;
    bool uppercase = false;
    bool force_sign = false;
};

//==============================================================================

template <typename I>
struct integer_buffer_traits
{
    // Number of binary digits required.
    static constexpr size_t max_precision = sizeof(I) * CHAR_BIT;

    // +1 for sign.
    static constexpr size_t buffer_size = max_precision + (std::is_signed<I>::value ? 1 : 0);
};

//==============================================================================

namespace _strconv_priv {

template <typename I, typename C>
constexpr to_string_result write_integer_base10_impl(I value, C* buf, const size_t buf_size, const bool force_sign) noexcept
{
    using U = typename std::make_unsigned<I>::type;
    constexpr U ubase = static_cast<U>(10);

    U uvalue = static_cast<U>(value);
    char sign = force_sign ? '+' : 0;

    VX_IF_CONSTEXPR (std::is_signed<I>::value)
    {
        if (value < 0)
        {
            // cast through unsigned to avoid UB on min value (-value would overflow)
            uvalue = ~uvalue + 1;
            sign = '-';
        }
    }

    const size_t n_digits = digit_count_unsigned(uvalue);
    const size_t needed = static_cast<size_t>(sign != 0) + n_digits;
    if (buf_size < needed)
    {
        return { 0, to_string_error::buffer_too_small };
    }

    size_t i = needed;

    do
    {
        const char c = get_integer_digit(uvalue % ubase);
        buf[--i] = static_cast<C>(c);
        uvalue /= ubase;

    } while (uvalue);

    if (sign)
    {
        buf[0] = static_cast<C>(sign);
    }

    return { needed, to_string_error::none };
}

template <typename I, typename C, bool Prefix = false>
constexpr to_string_result write_integer_impl(I value, C* buf, const size_t buf_size, const integer_to_string_format_options& fmt) noexcept
{
    VX_ASSERT(2 <= fmt.base);
#if defined(VX_STRING_CONVERT_TO_STRING_BASE_36_SUPPORT)
    VX_ASSERT(fmt.base <= 36);
#else
    VX_ASSERT(fmt.base <= 16);
#endif

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

    size_t prefix_size = 0;
    VX_IF_CONSTEXPR (Prefix)
    {
        switch (fmt.base)
        {
            case 2:
            case 16:
            {
                prefix_size = 2;
                break;
            }
            case 8:
            {
                prefix_size = 1;
                break;
            }
            default:
            {
                break;
            }
        }
    }

    const size_t n_digits = digit_count(uvalue, ubase);
    const size_t needed = static_cast<size_t>(sign != 0) + prefix_size + n_digits;
    if (buf_size < needed)
    {
        return { 0, to_string_error::buffer_too_small };
    }

    size_t i = needed;

    do
    {
        const char c = get_integer_digit(uvalue % ubase);
        buf[--i] = static_cast<C>(fmt.uppercase ? alnum_to_upper(c) : c);
        uvalue /= ubase;

    } while (uvalue);

    VX_IF_CONSTEXPR (Prefix)
    {
        if (prefix_size)
        {
            buf[1] = static_cast<C>('0');
            const C case_mask = make_case_mask<C>(fmt.uppercase);

            if (fmt.base == 2)
            {
                buf[2] = static_cast<C>('b') & case_mask;
            }
            else // if (fmt.base == 16)
            {
                buf[2] = static_cast<C>('x') & case_mask;
            }
        }
    }

    if (sign)
    {
        buf[0] = static_cast<C>(sign);
    }

    return { needed, to_string_error::none };
}

} // namespace _strconv_priv

template <typename I, bool Prefix = false, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result write_integer(I value, C* buf, const size_t buf_size, const integer_to_string_format_options& fmt = {}) noexcept
{
    return _strconv_priv::write_integer_impl<I, C, Prefix>(value, buf, buf_size, fmt);
}

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result write_integer_base10(I value, C* buf, const size_t buf_size, const bool force_sign = false) noexcept
{
    return _strconv_priv::write_integer_base10_impl<I, C>(value, buf, buf_size, force_sign);
}

//==============================================================================
// float
//==============================================================================

template <typename C = char>
struct float_to_string_format_options
{
    float_format format = float_format::general;
    uint32_t precision = _strconv_priv::precision_unspecified;

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

template <typename F>
struct float_buffer_traits
{
    using traits = float_traits<F>;

    // Smallest subnormal is radix^(min_exponent - digits); the number of
    // fractional decimal digits it needs is digits - min_exponent
    // (min_exponent is negative).
    static constexpr size_t max_precision =
        static_cast<size_t>(traits::digits) -
        static_cast<size_t>(traits::min_exponent);

    // +3 for: sign, the ones-place digit not covered by max_exponent10, and
    // the decimal point.
    static constexpr size_t buffer_size =
        max_precision +
        static_cast<size_t>(traits::max_exponent10) + 3;
};

//==============================================================================

// https://github.com/microsoft/STL/blob/f3ae96af460b8fcb7d77c46fc1ad7d312900d1e7/stl/inc/xcharconv_ryu.h#L2390

namespace _strconv_priv {

template <typename F>
struct float_writing_traits
{
    using traits = float_traits<F>;
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

private:

    constexpr limb_type mul_extract(limb_type x) noexcept
    {
        return bits.mul_extract_clear(x, top_limb, lower_bits, upper_mask);
    }

public:

    constexpr limb_type extract_digit() noexcept
    {
        return mul_extract(10);
    }

    constexpr limb_type extract_chunk() noexcept
    {
        return mul_extract(1000000000u);
    }

    constexpr limb_type extract_small_chunk(size_t digits) noexcept
    {
        return mul_extract(pow10_u32(digits));
    }

    constexpr bool has_zero_tail() const noexcept
    {
        // any limb strictly below the current extraction point
        for (uint32_t i = 0; i < top_limb; ++i)
        {
            if (bits.bits[i] != 0)
            {
                return false;
            }
        }

        // bits below (and including) the extraction point in the current top limb
        if ((bits.bits[top_limb] & upper_mask) != 0)
        {
            return false;
        }

        return true;
    }

    constexpr bool round_up(bool last_digit_odd) noexcept
    {
        const limb_type round_digit = extract_digit();

        if (round_digit > 5)
        {
            return true;
        }
        if (round_digit < 5)
        {
            return false;
        }

        // round_digit == 5: only an exact tie if nothing remains below it
        if (!has_zero_tail())
        {
            return true;
        }

        // exact tie -> round to even
        return last_digit_odd;
    }
};

//==============================================================================

template <typename F, bool is_small>
struct float_int_digit_extractor;

template <typename F>
struct float_int_digit_extractor<F, false>
{
    using traits = float_traits<F>;
    using base = float_writing_traits<F>;
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

    constexpr limb_type div_extract_shrink(limb_type x) noexcept
    {
        const limb_type r = bits.div_extract(x, top_limb);
        shrink();
        return r;
    }

public:

    constexpr bool shave_digits(size_t count) noexcept
    {
        limb_type remainder_bits = 0;

        while (count >= 9)
        {
            remainder_bits |= div_extract_shrink(1000000000u);
            count -= 9;
        }

        if (count)
        {
            remainder_bits |= div_extract_shrink(pow10_u32(count));
        }

        return remainder_bits == 0;
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
    using traits = float_traits<F>;
    using base = float_writing_traits<F>;
    using uint_type = typename base::uint_type;
    using limb_type = typename base::limb_type;
    using big_int_type = typename base::big_int_type;

    big_int_type bits;

    constexpr float_int_digit_extractor(const uint_type m2, const int shift) noexcept
        : bits(m2, shift)
    {}

public:

    constexpr bool shave_digits(size_t count) noexcept
    {
        limb_type remainder = 0;
        bool zero_remainder = true;

        while (count >= 9)
        {
            remainder = bits.div_extract(1000000000u);
            zero_remainder = zero_remainder && (remainder == 0);
            count -= 9;
        }

        if (count)
        {
            remainder = bits.div_extract(pow10_u32(count));
            zero_remainder = zero_remainder && (remainder == 0);
        }

        return zero_remainder;
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

template <typename F, typename Extractor>
struct chunk_buffer
{
    using traits = float_traits<F>;
    using base = float_writing_traits<F>;
    using uint_type = typename base::uint_type;
    using limb_type = typename base::limb_type;
    using extractor = Extractor;

    static constexpr uint32_t max_shift = base::max_shift;
    static constexpr uint32_t total_bits = max_shift + traits::digits;
    static constexpr size_t max_chunks = ceil_log10_pow2(total_bits) / 9;

    extractor& ext;
    limb_type chunks[max_chunks];
    size_t chunk_count;

    constexpr chunk_buffer(extractor& ext_) noexcept
        : ext(ext_)
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
};

//==============================================================================

enum class float_write_status
{
    failed,
    finished,
    continue_
};

template <typename F, typename C>
constexpr bool write_float_sign(
    const float_bits<F>& fb,
    C* buf,
    const size_t buf_size,
    bool force_sign,
    size_t& n) noexcept
{
    using traits = float_traits<F>;

    const char sign = fb.sign_bit ? '-' : (force_sign ? '+' : 0);
    if (sign)
    {
        if (buf_size < 2)
        {
            return false;
        }

        buf[n++] = static_cast<C>(sign);
    }

    return true;
}

template <typename F, typename C>
constexpr bool write_float_prefix(
    C* buf,
    const size_t buf_size,
    bool uppercase,
    size_t& n) noexcept
{
    if (buf_size - n < 3)
    {
        return false;
    }

    buf[n++] = static_cast<C>('0');
    buf[n++] = static_cast<C>(uppercase ? 'X' : 'x');

    return true;
}

template <typename F, typename C>
constexpr float_write_status write_float_start(
    const float_bits<F>& fb,
    C* buf,
    const size_t buf_size,
    bool force_sign,
    bool uppercase,
    size_t& n) noexcept
{
    using traits = float_traits<F>;

    if (fb.e_bits == traits::inf_nan_exponent)
    {
        if (buf_size - n < 3)
        {
            return float_write_status::failed;
        }

        const C case_mask = make_case_mask<C>(uppercase);

        const C n_val = static_cast<C>('n') & case_mask;

        // inf
        if (fb.m_bits == 0)
        {
            buf[n++] = static_cast<C>('i') & case_mask;
            buf[n++] = n_val;
            buf[n++] = static_cast<C>('f') & case_mask;
        }
        // nan
        else
        {
            buf[n++] = n_val;
            buf[n++] = static_cast<C>('a') & case_mask;
            buf[n++] = n_val;

#if defined(VX_STRING_CONVERT_IND_NAN_SUPPORT)

            const bool indefinite = fb.sign_bit && (fb.m_bits == traits::quiet_nan_bit_mask);
            if (indefinite)
            {
                if (buf_size - n < 5)
                {
                    return float_write_status::failed;
                }

                buf[n++] = static_cast<C>('(');
                buf[n++] = static_cast<C>('i') & case_mask;
                buf[n++] = n_val;
                buf[n++] = static_cast<C>('d') & case_mask;
                buf[n++] = static_cast<C>(')');
            }

#elif defined(VX_STRING_CONVERT_SNAN_SUPPORT)

            constexpr bool indefinite = false;

#endif

#if defined(VX_STRING_CONVERT_SNAN_SUPPORT)

            const bool signaling = !indefinite && !(fb.m_bits & traits::quiet_nan_bit_mask);
            if (signaling)
            {
                if (buf_size - n < 6)
                {
                    return float_write_status::failed;
                }

                buf[n++] = static_cast<C>('(');
                buf[n++] = static_cast<C>('s') & case_mask;
                buf[n++] = n_val;
                buf[n++] = static_cast<C>('a') & case_mask;
                buf[n++] = n_val;
                buf[n++] = static_cast<C>(')');
            }

#endif
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
    size_t i = int_digit_count + precision;

    // Fractional digits
    while (i > int_digit_count)
    {
        if (ptr[i] != static_cast<C>('9'))
        {
            ++ptr[i];
            return false;
        }

        ptr[i] = static_cast<C>('0');

        --i;
    }

    // Integer digits
    while (i > 0)
    {
        --i;

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
constexpr size_t write_fixed_normal(
    typename float_bits<F>::uint_type m2,
    int e10,
    int shift,
    C* buf,
    const size_t buf_size,
    const float_to_string_format_options<C>& fmt,
    const size_t precision) noexcept
{
    using traits = float_traits<F>;
    using uint_type = typename traits::uint_type;
    using extractor = float_frac_digit_extractor<F>;
    using digit_type = typename extractor::limb_type;

    const size_t precision_char_count = static_cast<size_t>(precision > 0) + precision;
    const size_t needed = 1 + precision_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    buf[0] = static_cast<C>('0');

    digit_type last_digit = 0;
    extractor ext(m2, shift);

    if (precision)
    {
        buf[1] = fmt.decimal_point;
        C* ptr = buf + 2;

        // default for most common case
        size_t leading_zero_count = 0;

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
            last_digit = ext.extract_digit();
            *ptr++ = static_cast<C>(hex::digits[last_digit]);
        }
    }

    if (fmt.round)
    {
        if (ext.round_up((last_digit & digit_type{ 1 }) != 0))
        {
            round_fixed<C>(buf, 1, precision);
        }
    }

    return needed;
}

template <typename F, typename C>
constexpr size_t write_fixed_large(
    typename float_bits<F>::uint_type m2,
    int e10,
    int shift,
    C* buf,
    const size_t buf_size,
    const float_to_string_format_options<C>& fmt,
    const size_t precision) noexcept
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
        using chunk_tracker = chunk_buffer<F, extractor>;
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
            for (size_t j = 0; j < 9; ++j)
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
constexpr size_t write_fixed_mixed(
    typename float_bits<F>::uint_type m2,
    int shift,
    C* buf,
    const size_t buf_size,
    const float_to_string_format_options<C>& fmt,
    const size_t precision) noexcept
{
    using traits = float_traits<F>;
    using uint_type = typename traits::uint_type;
    using extractor = float_frac_digit_extractor<F>;
    using digit_type = typename extractor::limb_type;

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
    while (int_bits)
    {
        const auto digit = int_bits % 10;
        *--ptr = static_cast<C>(hex::digits[digit]);
        int_bits /= 10;
    }

    const uint_type raw_frac_bits = m2 & ((uint_type{ 1 } << frac_bit_count) - 1);
    extractor ext(raw_frac_bits, -shift);

    ptr += int_digit_count;
    digit_type last_digit = static_cast<digit_type>(*(ptr - 1) - static_cast<C>('0'));

    // float digits
    if (precision)
    {
        *ptr++ = fmt.decimal_point;

        for (size_t i = 0; i < precision; ++i)
        {
            last_digit = ext.extract_digit();
            *ptr++ = static_cast<C>(hex::digits[last_digit]);
        }
    }

    // round
    if (fmt.round)
    {
        // get the pointer to the last written digit
        if (ext.round_up((last_digit & digit_type{ 1 }) != 0) &&
            round_fixed<C>(buf, int_digit_count, precision))
        {
            ++needed;
            if (buf_size < needed)
            {
                return 0;
            }

            mem::move_range(buf + 1, buf, needed - 1);
            buf[0] = static_cast<C>('1');
        }
    }

    return needed;
}

template <typename F, typename C>
constexpr size_t write_float_fixed_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt) noexcept
{
    using traits = float_traits<F>;
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
            constexpr int shift = static_cast<int>(traits::exponent_bias) + static_cast<int>(traits::mantissa_bits) - 1;
            return write_fixed_normal<F, C>(fb.m_bits, e10, shift, buf, buf_size, fmt, precision);
        }
    }

    const uint_type m2 = fb.m_bits | (uint_type{ 1 } << traits::mantissa_bits);
    const int e2 = static_cast<int>(fb.e_bits) - traits::exponent_bias;
    const int shift = e2 - traits::mantissa_bits;

    if (e2 < 0)
    {
        // normal
        const int e10 = normal_pow10<F>(e2);
        return write_fixed_normal<F, C>(m2, e10, -shift, buf, buf_size, fmt, precision);
    }

    if (shift >= 0)
    {
        // large integer
        const int e10 = large_integer_pow10<F>(e2);
        return write_fixed_large<F, C>(m2, e10, shift, buf, buf_size, fmt, precision);
    }
    else
    {
        // int and float part
        return write_fixed_mixed<F, C>(m2, shift, buf, buf_size, fmt, precision);
    }
}

//==============================================================================
// scientific helpers
//==============================================================================

template <typename C>
constexpr void write_positive_exponent_block(int e10, C* buf, size_t exp_char_count, bool uppercase, bool force_sign) noexcept
{
    buf[0] = static_cast<C>(uppercase ? 'E' : 'e');

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
constexpr size_t write_scientific_zero(
    C* buf,
    const size_t buf_size,
    const float_to_string_format_options<C>& fmt,
    const size_t precision) noexcept
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
constexpr size_t write_scientific_normal(
    typename float_bits<F>::uint_type m2,
    int e10,
    int shift,
    C* buf,
    const size_t buf_size,
    const float_to_string_format_options<C>& fmt,
    const size_t precision) noexcept
{
    using extractor = float_frac_digit_extractor<F>;
    using digit_type = typename extractor::limb_type;

    // The number of digits in the exponent could possibly shrink due to rounding or
    // off by 1 estimate for the e10. The best we can do here is to exit early if we
    // can't fit all of the leading digits. We will check the exponent space at the end.
    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
    size_t needed = leading_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    extractor ext(m2, shift);
    digit_type last_digit;

    // first find the first non 0 digit
    if (e10 <= 1)
    {
        // Our estimate should be accurate when e10 is 1, and this is the most common case.
        last_digit = ext.extract_digit();
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

        last_digit = ext.extract_digit();
        if (last_digit != 0)
        {
            // we over estimated e10 by 1
            --e10;
        }
        else
        {
            last_digit = ext.extract_digit();
        }
    }

    C* ptr = buf;
    *ptr++ = static_cast<C>(hex::digits[last_digit]);

    if (precision > 0)
    {
        *ptr++ = fmt.decimal_point;

        for (size_t i = 0; i < precision; ++i)
        {
            last_digit = ext.extract_digit();
            *ptr++ = static_cast<C>(hex::digits[last_digit]);
        }
    }

    if (fmt.round)
    {
        if (ext.round_up((last_digit & digit_type{ 1 }) != 0) &&
            scientific_carry_round(buf, leading_char_count))
        {
            --e10;
        }
    }

    const bool exp_is_zero = (e10 == 0);
    const bool exp_sign = (!exp_is_zero || fmt.force_exp_sign);
    const size_t exp_digit_count = digit_count_max3(e10);
    const size_t exp_char_count = 1 + static_cast<size_t>(exp_sign) + exp_digit_count;
    needed += exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    // exponent
    {
        *ptr++ = static_cast<C>(fmt.uppercase ? 'E' : 'e');

        if (!exp_is_zero)
        {
            *ptr++ = static_cast<C>('-');
        }
        else if (fmt.force_exp_sign)
        {
            *ptr++ = static_cast<C>('+');
        }

        ptr += exp_digit_count;

        do
        {
            const int digit = e10 % 10;
            *(--ptr) = static_cast<C>(hex::digits[digit]);
            e10 /= 10;

        } while (e10 > 0);
    }

    return needed;
}

template <typename limb_type>
inline constexpr bool scientific_should_round_up(bool last_digit_odd, limb_type round_digit, bool has_zero_tail) noexcept
{
    if (round_digit > 5)
    {
        return true;
    }
    if (round_digit < 5)
    {
        return false;
    }

    // round_digit == 5: only an exact tie if nothing remains below it
    if (!has_zero_tail)
    {
        return true;
    }

    // exact tie -> round to even
    return last_digit_odd;
}

template <typename F, typename C>
constexpr size_t write_scientific_large(
    typename float_bits<F>::uint_type m2,
    int e10,
    int shift,
    C* buf,
    const size_t buf_size,
    const float_to_string_format_options<C>& fmt,
    const size_t precision) noexcept
{
    using extractor = float_int_digit_extractor_selector<F>;
    using digit_type = typename extractor::limb_type;

    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
    size_t needed = leading_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    extractor ext(m2, shift);
    digit_type round_digit = 0;
    bool has_zero_tail = true;

    C* back_ptr = buf + leading_char_count;

    const size_t int_digit_count = static_cast<size_t>(e10) + 1;
    const size_t digits_needed = static_cast<size_t>(precision) + 1;
    const size_t last_digit_pos = digits_needed + static_cast<size_t>(precision > 0);
    size_t zero_fill = 0;

    // first we shave off the digits we don't care about
    if (digits_needed < int_digit_count)
    {
        const size_t trailing_digit_count = int_digit_count - digits_needed;
        has_zero_tail = ext.shave_digits(trailing_digit_count - 1);
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
        if (fmt.round)
        {
            const digit_type last_last_digit = static_cast<digit_type>(back_ptr[last_digit_pos - 1] - '0');
            has_zero_tail = has_zero_tail && (last_last_digit == 0);
            round_digit = last_last_digit;
        }

        if (precision > 0)
        {
            // swap the first digit with the decimal and shift back
            mem::swap(*back_ptr, *(back_ptr + 1));
            mem::move_range(back_ptr + 1, back_ptr, digits_needed); // use digits needed because it includes the decimal
        }

        const auto digit = ext.extract_last_digit();
        *back_ptr = static_cast<C>(hex::digits[digit]);

        // exponent estimate was too low by 1; adjust
        ++e10;
    }

    if (fmt.round)
    {
        const bool last_digit_odd = (back_ptr[last_digit_pos - 1] & 1) != 0;
        if (scientific_should_round_up((last_digit_odd) != 0, round_digit, has_zero_tail) &&
            scientific_carry_round(buf, leading_char_count))
        {
            ++e10;
        }
    }

    const size_t exp_char_count = 1 + static_cast<size_t>(fmt.force_exp_sign) + digit_count_max3(e10);
    needed += exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    write_positive_exponent_block(e10, buf + leading_char_count, exp_char_count, fmt.uppercase, fmt.force_exp_sign);
    return needed;
}

template <typename F, typename C>
constexpr size_t write_scientific_mixed(
    typename float_bits<F>::uint_type m2,
    int shift,
    C* buf,
    const size_t buf_size,
    const float_to_string_format_options<C>& fmt,
    const size_t precision) noexcept
{
    using traits = float_traits<F>;
    using uint_type = typename traits::uint_type;
    using extractor = float_frac_digit_extractor<F>;
    using digit_type = typename extractor::limb_type;

    const int frac_bit_count = -shift;
    uint_type int_bits = m2 >> frac_bit_count;

    // should be exact
    const size_t int_digit_count = digit_count_unsigned(int_bits);
    int e10 = static_cast<int>(int_digit_count - 1);

    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
    size_t needed = leading_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    const size_t digits_needed = static_cast<size_t>(precision) + 1;
    const size_t int_digits_needed = (int_digit_count >= digits_needed) ? digits_needed : int_digit_count;
    const size_t frac_digits_needed = digits_needed - int_digits_needed;

    digit_type last_digit = 0;
    digit_type round_digit = 0;
    bool has_zero_tail = true;

    // int digits
    {
        // first we shave off the digits we don't care about
        if (int_digits_needed < int_digit_count)
        {
            size_t shave = int_digit_count - int_digits_needed;

            // Batch divide by 10^9 (or 10^18 if using wide_type) to wipe out chunks of digits instantly
            while (shave >= 10)
            {
                has_zero_tail = has_zero_tail && (int_bits % 1000000000u == 0);
                int_bits /= 1000000000u;
                shave -= 9;
            }

            if (shave >= 2)
            {
                const auto div = pow10_u32(shave - 1);
                has_zero_tail = has_zero_tail && (int_bits % div == 0);
                int_bits /= div;
            }

            round_digit = int_bits % 10;
            int_bits /= 10;
        }

        const size_t int_digit_end = int_digits_needed + static_cast<size_t>(precision > 0);
        C* back_ptr = buf + int_digit_end;

        // populate integer part
        size_t remaining = int_digits_needed;
        while (--remaining)
        {
            const auto digit = int_bits % 10;
            int_bits /= 10;
            *(--back_ptr) = static_cast<C>(hex::digits[digit]);
        }

        // decimal + final digit
        if (precision > 0)
        {
            *(--back_ptr) = fmt.decimal_point;
        }
        const auto digit = int_bits % 10;
        *(--back_ptr) = static_cast<C>(hex::digits[digit]);

        if (frac_digits_needed == 0)
        {
            // extract the last digit used for rounding
            last_digit = static_cast<digit_type>(back_ptr[int_digit_end - 1] - '0');
        }
    }

    // frac part + rounding
    if (frac_digits_needed || (int_digits_needed == int_digit_count && fmt.round))
    {
        const uint_type raw_frac_bits = m2 & ((uint_type{ 1 } << frac_bit_count) - 1);
        extractor ext(raw_frac_bits, -shift);

        C* ptr = buf + int_digits_needed + 1;

        for (size_t i = 0; i < frac_digits_needed; ++i)
        {
            last_digit = ext.extract_digit();
            *ptr++ = static_cast<C>(hex::digits[last_digit]);
        }

        if (fmt.round)
        {
            round_digit = ext.extract_digit();
            has_zero_tail = has_zero_tail && ext.has_zero_tail();
        }
    }

    if (fmt.round)
    {
        if (scientific_should_round_up((last_digit & digit_type{ 1 }) != 0, round_digit, has_zero_tail) &&
            scientific_carry_round(buf, leading_char_count))
        {
            ++e10;
        }
    }

    const size_t exp_char_count = 1 + static_cast<size_t>(fmt.force_exp_sign) + digit_count_max3(e10);
    needed += exp_char_count;
    if (buf_size < needed)
    {
        return 0;
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
    using traits = float_traits<F>;
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
            const int e10 = subnormal_pow10<F>(fb.m_bits) + 1;
            constexpr int shift = static_cast<int>(traits::exponent_bias) + static_cast<int>(traits::mantissa_bits) - 1;
            return write_scientific_normal<F, C>(fb.m_bits, e10, shift, buf, buf_size, fmt, precision);
        }
    }

    const uint_type m2 = fb.m_bits | (uint_type{ 1 } << traits::mantissa_bits);
    const int e2 = static_cast<int>(fb.e_bits) - traits::exponent_bias;
    const int shift = e2 - traits::mantissa_bits;

    if (e2 < 0)
    {
        // normal
        const int e10 = normal_pow10<F>(e2);
        return write_scientific_normal<F, C>(m2, e10, -shift, buf, buf_size, fmt, precision);
    }

    if (shift >= 0)
    {
        // large integer
        const int e10 = large_integer_pow10<F>(e2);
        return write_scientific_large<F, C>(m2, e10, shift, buf, buf_size, fmt, precision);
    }
    else
    {
        // int and float part
        return write_scientific_mixed<F, C>(m2, shift, buf, buf_size, fmt, precision);
    }
}

//==============================================================================
// hex format
//==============================================================================

template <typename C>
constexpr C round_hex_low(C c, const C case_mask) noexcept
{
    if (c == static_cast<C>('9'))
    {
        return static_cast<C>('a') & case_mask;
    }
    return c + 1;
}

inline constexpr bool hex_should_round_up(uint32_t last_digit, uint32_t round_digit, bool sticky) noexcept
{
    if (round_digit > 8)
    {
        return true;
    }
    if (round_digit < 8)
    {
        return false;
    }

    // round_digit == 8: only an exact tie if nothing remains below it
    if (sticky)
    {
        return true;
    }

    // exact tie -> round to even
    return ((last_digit & 0x1) != 0);
}

template <typename C>
constexpr void hex_carry_round(C* ptr, size_t leading_char_count, bool uppercase) noexcept
{
    const C case_mask = make_case_mask<C>(uppercase);

    // fractional hex digits (skipped entirely if leading_char_count == 1)
    for (size_t i = leading_char_count - 1; i > 1; --i)
    {
        const C c = ptr[i];
        if (c != (static_cast<C>('f') & case_mask))
        {
            ptr[i] = round_hex_low(c, case_mask);
            return;
        }

        ptr[i] = static_cast<C>('0');
    }

    // leading digit: only ever '0' or '1' going in, so it becomes '1' or '2'
    // going out -- never wraps, never changes digit width, never touches exp.
    ++ptr[0];
}

template <typename F, typename C>
constexpr size_t write_float_hex_impl(const float_bits<F>& fb, C* buf, const size_t buf_size, const float_to_string_format_options<C>& fmt) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    using traits = float_traits<F>;
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

    const bool subnormal = (fb.e_bits == 0 && fb.m_bits != 0);

    const int exp = subnormal
        ? 1 - static_cast<int>(traits::exponent_bias)
        : fb.e_bits
        ? static_cast<int>(fb.e_bits - traits::exponent_bias)
        : 0;

    const bool leading_one = (fb.e_bits != 0);

    constexpr uint32_t mbits = traits::mantissa_bits;
    constexpr uint32_t hex_digits = (mbits + 3) / 4;
    constexpr uint32_t shift = hex_digits * 4 - mbits;
    constexpr uint32_t topshift = (hex_digits - 1) * 4;

    uint_type frac = fb.m_bits << shift;

    const char exp_sign = (exp < 0) ? '-' : (fmt.force_exp_sign ? '+' : 0);
    int abs_exp = (exp < 0) ? -exp : exp;
    const size_t exp_digit_count = base2_exp_digit_count<F>(abs_exp);

    const size_t leading_char_count = 1 + static_cast<size_t>(precision > 0) + precision;
    const size_t exp_char_count = 1 + static_cast<size_t>(exp_sign != 0) + exp_digit_count;
    const size_t needed = leading_char_count + exp_char_count;
    if (buf_size < needed)
    {
        return 0;
    }

    buf[0] = static_cast<C>(hex::digits[leading_one]);

    const size_t avail_digits = (precision < static_cast<size_t>(hex_digits))
        ? precision
        : static_cast<size_t>(hex_digits);

    uint32_t last_digit = leading_one ? 1u : 0u; // used for round-to-even if precision == 0

    if (precision > 0)
    {
        buf[1] = fmt.decimal_point;

        for (size_t i = 0; i < avail_digits; ++i)
        {
            last_digit = static_cast<uint32_t>((frac >> topshift) & 0xF);
            const char c = hex::digits[last_digit];
            buf[2 + i] = static_cast<C>(fmt.uppercase ? _strconv_priv::alnum_to_upper(c) : c);
            frac <<= 4;
        }

        if (precision > avail_digits)
        {
            const size_t zeros = precision - avail_digits;
            fill_n_zeros(buf + 2 + avail_digits, zeros);
        }
    }

    if (fmt.round && precision < static_cast<size_t>(hex_digits))
    {
        // frac is now aligned so the top nibble is the guard digit and
        // everything below it is the sticky region (remaining trailing digits).
        const uint32_t guard = static_cast<uint32_t>((frac >> topshift) & 0xF);
        const bool sticky = (frac & ((uint_type(1) << topshift) - 1)) != 0;

        if (hex_should_round_up(last_digit, guard, sticky))
        {
            hex_carry_round(buf, leading_char_count, fmt.uppercase);
        }
    }

    size_t n = leading_char_count;
    buf[n++] = static_cast<C>(fmt.uppercase ? 'P' : 'p');
    if (exp_sign != 0 || fmt.force_exp_sign)
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

    return needed;
}

//==============================================================================

template <typename F>
constexpr float_format resolve_general_format(const float_bits<F>& fb) noexcept
{
    if (fb.e_bits == 0)
    {
        if (fb.m_bits == 0)
        {
            return float_format::fixed;
        }
        else
        {
            // subnormal
            return float_format::scientific;
        }
    }

    using traits = float_traits<F>;
    const int e2 = static_cast<int>(fb.e_bits - traits::exponent_bias);

    int e10;
    if (e2 < 0)
    {
        e10 = normal_pow10<F>(e2);
    }
    else
    {
        e10 = large_integer_pow10<F>(e2);
    }

    return (e10 < -4 || e10 >= float_scientific_default_precision) ? float_format::scientific : float_format::fixed;
}

} // namespace _strconv_priv

//==============================================================================
// general
//==============================================================================

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result write_float_fixed(const F value, C* buf, size_t buf_size, const float_to_string_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    const _strconv_priv::float_bits<F> bits{ value };
    size_t n = 0;

    if (!_strconv_priv::write_float_sign(bits, buf, buf_size, fmt.force_sign, n))
    {
        return { 0, to_string_error::buffer_too_small };
    }

    using float_write_status = _strconv_priv::float_write_status;
    const float_write_status status = _strconv_priv::write_float_start<F, C>(bits, buf, buf_size, fmt.force_sign, fmt.uppercase, n);
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

    const size_t written = _strconv_priv::write_float_fixed_impl<F, C>(bits, buf, buf_size, fmt);
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

    const _strconv_priv::float_bits<F> bits{ value };
    size_t n = 0;

    if (!_strconv_priv::write_float_sign(bits, buf, buf_size, fmt.force_sign, n))
    {
        return { 0, to_string_error::buffer_too_small };
    }

    using float_write_status = _strconv_priv::float_write_status;
    const float_write_status status = _strconv_priv::write_float_start<F, C>(bits, buf, buf_size, fmt.force_sign, fmt.uppercase, n);
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

    const size_t written = _strconv_priv::write_float_scientific_impl<F, C>(bits, buf, buf_size, fmt);
    if (written == 0)
    {
        return { 0, to_string_error::buffer_too_small };
    }

    return { n + written, to_string_error::none };
}

template <typename F, bool Prefix = false, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result write_float_hex(const F value, C* buf, size_t buf_size, const float_to_string_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    const _strconv_priv::float_bits<F> bits{ value };
    size_t n = 0;

    if (!_strconv_priv::write_float_sign(bits, buf, buf_size, fmt.force_sign, n))
    {
        return { 0, to_string_error::buffer_too_small };
    }

    VX_IF_CONSTEXPR (Prefix)
    {
        if (!_strconv_priv::write_float_prefix(buf, buf_size, fmt.uppercase, n))
        {
            return { 0, to_string_error::buffer_too_small };
        }
    }

    using float_write_status = _strconv_priv::float_write_status;
    const float_write_status status = _strconv_priv::write_float_start<F, C>(bits, buf, buf_size, fmt.force_sign, fmt.uppercase, n);
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

    const size_t written = _strconv_priv::write_float_hex_impl<F, C>(bits, buf, buf_size, fmt);
    if (written == 0)
    {
        return { 0, to_string_error::buffer_too_small };
    }

    return { n + written, to_string_error::none };
}

template <typename F, bool Prefix = false, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result write_float(const F value, C* buf, size_t buf_size, const float_to_string_format_options<C>& fmt = {}) noexcept
{
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double not supported");

    if (fmt.precision != _strconv_priv::precision_unspecified && fmt.precision > _strconv_priv::max_float_precision)
    {
        return { 0, to_string_error::precision_too_large };
    }

    const _strconv_priv::float_bits<F> bits{ value };
    size_t header = 0;
    size_t body = 0;

    if (!_strconv_priv::write_float_sign(bits, buf, buf_size, fmt.force_sign, header))
    {
        return { 0, to_string_error::buffer_too_small };
    }

    VX_IF_CONSTEXPR (Prefix)
    {
        if (!_strconv_priv::write_float_prefix(buf, buf_size, fmt.uppercase, header))
        {
            return { 0, to_string_error::buffer_too_small };
        }
    }

    using float_write_status = _strconv_priv::float_write_status;
    const float_write_status status = _strconv_priv::write_float_start<F, C>(bits, buf, buf_size, fmt.force_sign, fmt.uppercase, header);
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
        format = _strconv_priv::resolve_general_format(bits);
    }

    buf += header;
    buf_size -= header;

    switch (format)
    {
        default:
        case float_format::fixed:
        {
            body = _strconv_priv::write_float_fixed_impl(bits, buf, buf_size, fmt);
            break;
        }
        case float_format::scientific:
        {
            body = _strconv_priv::write_float_scientific_impl(bits, buf, buf_size, fmt);
            break;
        }
        case float_format::hex:
        {
            body = _strconv_priv::write_float_hex_impl(bits, buf, buf_size, fmt);
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
// to string
//==============================================================================

template <typename I, typename C, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
constexpr to_string_result to_string(I value, C* buf, size_t buf_size, const integer_to_string_format_options& fmt = {}) noexcept
{
    return write_integer(value, buf, buf_size, fmt);
}

template <typename I, typename S, VX_REQUIRES(std::is_integral<I>::value&& str::is_mutable_string_like<S>::value)>
to_string_result to_string(I value, S& out, const integer_to_string_format_options& fmt = {}) noexcept
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

    size_t count = _strconv_priv::digit_count(uvalue, ubase) + static_cast<size_t>(sign);
    out.resize(count, 0);

    do
    {
        const char c = _strconv_priv::get_integer_digit(uvalue % ubase);
        out[--count] = static_cast<C>(fmt.uppercase ? _strconv_priv::alnum_to_upper(c) : c);
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

template <typename F, typename S, VX_REQUIRES(std::is_floating_point<F>::value&& str::is_mutable_string_like<S>::value)>
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

    out.resize(res.count);
    return res;
}

} // namespace strconv
} // namespace vx
