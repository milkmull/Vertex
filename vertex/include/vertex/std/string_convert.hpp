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

enum : size_t
{
    default_float_precision = numeric_limits<size_t>::max(),
    general_float_precision = 6, // matches default precision for %g
    max_float_precision = numeric_limits<double>::max_digits10
};

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
struct numeric_format_options
{
    numeric_format format = numeric_format::general;
    C decimal_point = C('.');
    size_t base = 10;
    size_t precision = default_float_precision;
    bool uppercase = false;
    bool force_sign = false;

    size_t get_used_precision() const noexcept
    {
        if (precision == default_float_precision)
        {
            return general_float_precision; // 6 for all formats
        }
        return std::min(precision, static_cast<size_t>(max_float_precision));
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
constexpr size_t char_count(I value, int base) noexcept
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
constexpr size_t char_count(uint_n<N, Limb, Traits> value, int base) noexcept
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

    constexpr size_t max_chars = sizeof(I) * 8 + 1;
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

    n += char_count(uvalue, ubase);

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

template <typename F, VX_REQUIRES(std::is_floating_point<F>::value)>
void print_float_fixed(F value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    // min shift = (1 - exponent_bias) - mantissa_bits  (negative)
    // denom_bits = -shift = exponent_bias - 1 + mantissa_bits
    constexpr int max_denom_bits = traits::exponent_bias - 1 + traits::mantissa_bits;
    constexpr size_t max_bigint_bits = ((max_denom_bits + 4 + 31) / 32) * 32;
    using BI = uint_n<max_bigint_bits>;

    const float_bits<F> fb(value);

    if (fb.is_nan())
    {
        std::cout << "nan\n";
        return;
    }
    if (fb.is_inf())
    {
        std::cout << (fb.is_negative() ? "-inf" : "inf") << '\n';
        return;
    }
    if (fb.is_zero())
    {
        std::cout << (fb.is_negative() ? "-0" : "0") << '\n';
        return;
    }

    if (fb.is_negative())
    {
        std::cout << '-';
    }

    const uint_type mant = fb.mantissa_with_integer_bit();
    const int shift = fb.shift();

    if (shift >= 0)
    {
        // integer: mant * 2^shift
        BI val(static_cast<uint64_t>(mant));
        val <<= static_cast<size_t>(shift);

        // extract decimal digits via repeated division by 10
        std::string s;
        while (!val.is_zero())
        {
            const auto res = divmod(val, BI(uint64_t(10)));
            s += static_cast<char>('0' + res.remainder.low64());
            val = res.quotient;
        }
        std::reverse(s.begin(), s.end());
        std::cout << s << '\n';
    }
    else
    {
        // fraction: mant / 2^(-shift)
        const int denom_bits = -shift;
        BI numerator(static_cast<uint64_t>(mant));

        // split into integer and fractional parts
        const BI int_part = numerator >> static_cast<size_t>(denom_bits);
        numerator &= (BI::one() << static_cast<size_t>(denom_bits)) - BI::one();

        // print integer part
        if (int_part.is_zero())
        {
            std::cout << '0';
        }
        else
        {
            std::string s;
            BI val = int_part;
            while (!val.is_zero())
            {
                const auto res = divmod(val, BI(uint64_t(10)));
                s += static_cast<char>('0' + res.remainder.low64());
                val = res.quotient;
            }
            std::reverse(s.begin(), s.end());
            std::cout << s;
        }

        std::cout << '.';

        // print fractional part
        std::string frac;
        while (!numerator.is_zero())
        {
            numerator = (numerator << 3) + (numerator << 1); // mul10
            const BI digit = numerator >> static_cast<size_t>(denom_bits);
            numerator &= (BI::one() << static_cast<size_t>(denom_bits)) - BI::one();
            frac += static_cast<char>('0' + digit.low64());
        }
        std::cout << frac << '\n';
    }
}

// https://github.com/microsoft/STL/blob/f3ae96af460b8fcb7d77c46fc1ad7d312900d1e7/stl/inc/xcharconv_ryu.h#L2390

namespace _string_convert_priv {

template <size_t N, typename Limb, typename Traits>
constexpr void mul10(uint_n<N, Limb, Traits>& a) noexcept
{
    // x * 10 = x * 8 + x * 2. Single pass, one carry value.
    // We use the fact that l*10 always fits in ceil(log2(10)) = 4 extra bits,
    // so the carry from limb i into limb i+1 is at most 9 (fits in 4 bits),
    // well within a Limb regardless of limb width.

    Limb carry = {};

    for (size_t i = 0; i < a.limb_count; ++i)
    {
        const Limb l = a.limbs[i];

        // Full product: l * 10 + carry_in.
        // Top bits become carry_out, bottom limb_bits become the new limb.
        // Since carry < 10 always, and l <= max_val, we need l*10 + 9 to fit
        // in 2*limb_bits, which it always does — use mul_wide for correctness.
        Limb lo{}, hi{};
        Traits::mul_wide(l, static_cast<Limb>(10), lo, hi);
        Limb c{};
        Traits::add_carry(lo, carry, Limb{}, a.limbs[i]);
        carry = hi + c;
    }
}

template <size_t N, typename Limb, typename Traits>
constexpr Limb divmod_limb(uint_n<N, Limb, Traits>& a, Limb b, size_t& top) noexcept
{
    Limb rem{};

    for (size_t i = top + 1; i-- > 0;)
    {
#if defined(__HAS_INT128)

        using u128 = unsigned __int128;
        const u128 cur = (static_cast<u128>(rem) << limb_bits) | a.limbs[i];
        a.limbs[i] = static_cast<Limb>(cur / b);
        rem = static_cast<Limb>(cur % b);

#else

        constexpr size_t half = a.limb_bits / 2;
        constexpr Limb half_mask = (Limb{ 1 } << half) - Limb{ 1 };

        const Limb cur_hi = a.limbs[i] >> half;
        const Limb cur_lo = a.limbs[i] & half_mask;

        const Limb d1 = (rem << half) | cur_hi;
        const Limb q1 = d1 / b;
        const Limb r1 = d1 % b;

        const Limb d2 = (r1 << half) | cur_lo;
        const Limb q2 = d2 / b;
        rem = d2 % b;

        a.limbs[i] = (q1 << half) | q2;

#endif
    }

    // shrink top down to the highest remaining non-zero limb
    while (top > 0 && a.limbs[top] == Limb{})
    {
        --top;
    }

    return rem;
}

} // namespace _string_convert_priv

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
constexpr size_t write_float_fixed(F value, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    // The denominator is 2^(-shift) where shift = true_exponent - mantissa_bits.
    // The most negative shift occurs at the smallest true_exponent = -(exponent_bias - 1),
    // giving a maximum denominator of 2^((exponent_bias - 1) + mantissa_bits), which
    // requires (exponent_bias - 1) + mantissa_bits bits to represent.
    // -------------------------------------------------------------------------
    // During digit extraction we repeatedly mul10(remainder) before masking back down.
    // Multiplying by 10 (0b1010) requires 4 bits to represent, so a mul10 on an N-bit
    // value can temporarily produce up to N+4 bits before the mask is applied.
    // We therefore need (exponent_bias - 1) + mantissa_bits + 4 bits in total.
    constexpr int max_bits_needed = (traits::exponent_bias - 1) + traits::mantissa_bits + 4;
    // Round up to the next 64-bit boundary so uint_n gets a whole number of 64-bit limbs
    // (the platform will use 64-bit limbs when __int128 or _umul128 is available).
    constexpr size_t max_bigint_bits = ((max_bits_needed + 63) / 64) * 64;
    using BI = uint_n<max_bigint_bits>;
    using limb_type = typename BI::limb_type;

    if (!buf || buf_size == 0)
    {
        return 0;
    }

    const size_t precision = fmt.precision;
    const bool upper = fmt.uppercase;
    const float_bits<F> fb(value);

    const uint_type mantissa = fb.mantissa_with_integer_bit();
    const int shift = fb.shift();
    const char sign = fb.is_negative() ? '-' : (fmt.force_sign ? '+' : '\0');

    // -------------------------------------------------------------------------
    // Special values: NaN and Inf
    // These are written directly and returned early.
    // -------------------------------------------------------------------------

    if (fb.is_nan())
    {
        if (buf_size < 3)
        {
            return 0;
        }

        buf[0] = upper ? 'N' : 'n';
        buf[1] = upper ? 'A' : 'a';
        buf[2] = buf[0]; // 'N' or 'n'
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
        return n;
    }

    // -------------------------------------------------------------------------
    // Normal path: buf layout while building (before the final reversal):
    //
    //   [0 .. precision-1]  fractional digits (most-significant first)
    //   [precision]         decimal point (if precision > 0)
    //   [precision+1 ..]    integer digits (least-significant first, reversed later)
    //   [n-1]               sign character (if any, reversed last)
    //
    // A single str::reverse(buf, n) at the end puts everything in order.
    // -------------------------------------------------------------------------

    BI numerator(mantissa);
    BI denominator_mask;
    BI remainder;

    // Minimum space needed: precision digits + decimal point + at least one
    // integer digit + optional sign.  We can't know the exact integer width yet,
    // but we can at least guard against obviously-too-small buffers.
    const size_t min_needed = precision + (precision > 0 ? 1 : 0) + 1 + (sign ? 1 : 0);
    if (buf_size < min_needed)
    {
        return 0;
    }

    if (shift >= 0)
    {
        // ----------------------------------------------------------------
        // Integer: value = mantissa * 2^shift — no fractional digits.
        // Shift the numerator left and write `precision` zero digits as
        // placeholders (they will all be "0.000..." after the final
        // reversal since the integer part dominates).
        // ----------------------------------------------------------------
        numerator <<= shift;

        if (VX_IS_CONSTANT_EVALUATED())
        {
            for (size_t i = 0; i < precision; ++i)
            {
                buf[n++] = C('0');
            }
        }
        else
        {
            mem::fill_range(buf, precision, C('0'));
            n = precision;
        }
    }
    else
    {
        // ----------------------------------------------------------------
        // Fraction: value = mantissa / 2^(-shift).
        //
        // denominator_mask = 2^(-shift) - 1  (low -shift bits set).
        // remainder        = mantissa mod denominator (the fractional part).
        // numerator        = mantissa >> (-shift)     (the integer part).
        //
        // To extract each fractional digit we multiply the remainder by 10
        // and read the digit that overflows the denominator's bit range.
        // ----------------------------------------------------------------
        const int denominator_bits = -shift;
        denominator_mask.set_low_bits(denominator_bits);
        remainder = numerator & denominator_mask;
        numerator >>= denominator_bits;

        for (size_t i = 0; i < precision; ++i)
        {
            _string_convert_priv::mul10(remainder);
            // Extract the digit that overflowed past the denominator's range.
            const auto digit = remainder.extract_bits_at(denominator_bits);
            buf[n++] = static_cast<C>('0' + digit);
            remainder &= denominator_mask; // keep only the sub-denominator remainder
        }

        // ----------------------------------------------------------------
        // Rounding: peek at what the (precision+1)-th digit would be.
        // If it is >= 5 we round up by propagating a carry through the
        // fractional digits we just wrote (buf[0..precision-1]).
        // If the carry escapes the fractional part it rolls into the
        // integer portion via ++numerator.
        // ----------------------------------------------------------------
        _string_convert_priv::mul10(remainder);
        const auto rounding_digit = remainder.extract_bits_at(denominator_bits);

        if (rounding_digit >= 5)
        {
            size_t i = n;
            while (i--)
            {
                if (++buf[i] <= C('9'))
                {
                    goto finished_rounding; // carry absorbed
                }
                buf[i] = C('0'); // digit wrapped: continue carry
            }

            // Carry escaped every fractional digit; adjust the integer part.
            ++numerator;
        }
    }

finished_rounding:

    // -------------------------------------------------------------------------
    // Decimal point (written after fractional digits, before integer digits).
    // In the pre-reversal layout the decimal point sits at position `n` and
    // separates the fractional block from the (reversed) integer block above it.
    // -------------------------------------------------------------------------
    if (precision > 0)
    {
        if (n == buf_size)
            return 0;
        // NOTE: fmt.decimal_point is the desired character; to_upper() is
        // applied when uppercase mode is OFF so that a lower-case '.' stays
        // as-is and any locale-specific separator is handled correctly.
        // For the standard '.' case both branches yield the same character.
        buf[n++] = upper ? fmt.decimal_point : to_upper(fmt.decimal_point);
    }

    // -------------------------------------------------------------------------
    // Integer digits — extracted least-significant-first via repeated divmod.
    // divmod_limb() divides `numerator` in-place by 10 and returns the
    // remainder (the next digit). We iterate from the most-significant
    // non-zero limb downward; `top` tracks that boundary so we skip
    // leading-zero limbs quickly.
    // -------------------------------------------------------------------------
    {
        size_t top = BI::limb_count - 1;

        // Advance `top` down to the most-significant non-zero limb.
        while (top > 0 && numerator.limbs[top] == 0)
        {
            --top;
        }

        do
        {
            if (n == buf_size)
                return 0;

            const auto digit = _string_convert_priv::divmod_limb(numerator, limb_type{ 10 }, top);
            buf[n++] = static_cast<C>('0' + digit);

            // Shrink `top` if the leading limb became zero.
            while (top > 0 && numerator.limbs[top] == 0)
            {
                --top;
            }

        } while (numerator.limbs[0] || top > 0);
    }

    // -------------------------------------------------------------------------
    // Sign character (appended last; will be at index 0 after reversal).
    // -------------------------------------------------------------------------
    if (sign)
    {
        if (n == buf_size)
            return 0;
        buf[n++] = static_cast<C>(sign);
    }

    // -------------------------------------------------------------------------
    // Final layout fixup via two-pass reversal.
    //
    // After the loop above the buffer looks like:
    //   [frac_d0 frac_d1 ... frac_dn | '.' | int_dLSB ... int_dMSB | sign]
    //
    // Step 1: reverse the fractional block in place so digits are MSB-first.
    // Step 2: reverse the entire buffer to bring sign and integer to the front.
    //
    // Result:
    //   [sign | int_dMSB ... int_dLSB | '.' | frac_d0 ... frac_dn]
    // -------------------------------------------------------------------------
    str::reverse(buf, precision); // Step 1: fix up fractional digit order
    str::reverse(buf, n);         // Step 2: bring sign+integer to the front

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
