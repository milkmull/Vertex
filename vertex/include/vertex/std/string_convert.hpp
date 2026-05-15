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
    static constexpr C hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

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
    default_float_precision = std::numeric_limits<size_t>::max(),
    general_float_precision = 6, // matches default precision for %g
    max_float_precision = 32
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

namespace _string_convert_priv {

enum : size_t
{
    // 1 (sign) + 309 (max double integer digits) + 1 (decimal point)
    // + max_digits10 (17) for full round-trip precision
    max_float_char_count = 311 + max_float_precision
};

}; // namespace _string_convert_priv

//==============================================================================
// to string
//==============================================================================

template <typename I, VX_REQUIRES(std::is_integral<I>::value)>
constexpr size_t char_count(I value, int base) noexcept
{
    size_t n = 0;

    do
    {
        value /= base;
        ++n;

    } while (value != 0);

    return n;
}

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
size_t write_integer(I value, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt, bool null_terminate = false) noexcept
{
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

    static constexpr const char digits[] = {
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
void write_float_fixed(F value) noexcept
{
    using traits = typename float_bits<F>::traits;
    using uint_type = typename traits::uint_type;

    // min shift = (1 - exponent_bias) - mantissa_bits  (negative)
    // denom_bits = -shift = exponent_bias - 1 + mantissa_bits
    static constexpr int max_denom_bits = traits::exponent_bias - 1 + traits::mantissa_bits;
    static constexpr size_t max_bigint_bits = ((max_denom_bits + 4 + 31) / 32) * 32;
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

        std::cout << "0.";

        std::string frac;
        while (!numerator.is_zero())
        {
            numerator = (numerator << 3) + (numerator << 1); // mul10
            BI digit = numerator >> static_cast<size_t>(denom_bits);
            numerator &= (BI::one() << static_cast<size_t>(denom_bits)) - BI::one(); // remainder
            frac += static_cast<char>('0' + digit.low64());
        }

        std::cout << frac << '\n';
    }
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
size_t write_float(F value, C* buf, const size_t buf_size, const numeric_format_options<C>& fmt) noexcept
{
    // https://github.com/godotengine/godot/blob/master/core/string/ustring.cpp#L1458
    VX_STATIC_ASSERT_MSG((!std::is_same<F, long double>::value), "long double support not implemented");

    constexpr size_t max_chars = _string_convert_priv::max_float_char_count;
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
    constexpr size_t buf_size = _string_convert_priv::max_float_char_count;
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

        if (!overflow && uout > (std::numeric_limits<U>::max() - static_cast<U>(digit)) / static_cast<U>(fmt.base))
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

        if (!overflow && out > (std::numeric_limits<U>::max() - static_cast<U>(digit)) / static_cast<U>(fmt.base))
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
