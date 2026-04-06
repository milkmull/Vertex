#pragma once

#include "vertex/config/assert.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_cast.hpp"
#include "vertex/std/string_utils.hpp"

namespace vx {
namespace str {

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

template <typename C = char, VX_REQUIRES(type_traits::is_char<C>::value)>
size_t to_hex_string(const void* data, const size_t size, C* buf, const size_t buf_size) noexcept
{
    static constexpr C hex[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    const size_t required = 2 * size;
    if (!buf || buf_size < required)
    {
        return 0;
    }

    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    for (size_t i = 0; i < size; ++i)
    {
        *buf++ = hex[(bytes[i] >> 4) & 0xF]; // High nibble
        *buf++ = hex[(bytes[i] >> 0) & 0xF]; // Low nibble
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
// to string
//==============================================================================

struct integer_formatter
{
    uint8_t base = 10;
    bool force_sign = false;
    bool uppercase = false;
};

enum class float_format : char
{
    general = 'g',
    fixed = 'f',
    scientific = 'e',
    hex = 'a'
};

struct float_formatter
{
    float_format format = float_format::fixed;
    int precision = -1;
    bool force_sign = false;
    bool uppercase = false;
};

//==============================================================================

namespace _string_convert_priv {

enum : size_t
{
    max_int_char_count = 65,
    max_float_char_count = 325
};

static size_t char_count(size_t value, int base) noexcept
{
    size_t n = 0;

    do
    {
        value /= base;
        ++n;

    } while (value != 0);

    return n;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
size_t write_unsigned(uint64_t v, C* buf, const integer_formatter& fmt)
{
    static constexpr const C digits[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y', 'z'
    };

    const size_t count = char_count(v, fmt.base);
    size_t i = count;

    do
    {
        const C c = digits[v % fmt.base];
        buf[--i] = fmt.uppercase ? to_upper(c) : c;
        v /= fmt.base;

    } while (v);

    return count;
}

template <typename T, VX_REQUIRES(std::is_floating_point<T>::value)>
inline size_t write_float(double v, char* buf, const float_formatter& fmt) noexcept
{
    // Build format string: %[+][.*][L][format_char]
    // Max length: '%' + '+' + '.*' + 'L' + 'g' + '\0' = 7 chars

    char fmt_str[7];
    char* p = fmt_str;

    *p++ = '%';
    if (fmt.force_sign)
    {
        *p++ = '+';
    }
    if (fmt.precision >= 0)
    {
        *p++ = '.';
        *p++ = '*';
    }
    VX_IF_CONSTEXPR (std::is_same<T, long double>::value)
    {
        *p++ = 'L';
    }

    const char f = static_cast<char>(fmt.format);
    *p++ = fmt.uppercase ? to_upper(f) : f;

    *p = '\0';

    const int n = (fmt.precision < 0)
        ? std::snprintf(buf, max_float_char_count, fmt_str, v)
        : std::snprintf(buf, max_float_char_count, fmt_str, fmt.precision, v);

    return static_cast<size_t>(n);
}

} // namespace _string_convert_priv

//==============================================================================

template <typename I, typename C = char, VX_REQUIRES(type_traits::is_signed_integral<I>::value&& type_traits::is_char<C>::value)>
size_t to_string(I v, C* buf, size_t buf_size, const integer_formatter& fmt = {}) noexcept
{
    VX_ASSERT(buf);
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

    C tmp[_string_convert_priv::max_int_char_count];

    const bool negative = (v < 0);
    size_t count = 0;

    uint64_t uv;

    if (negative || fmt.force_sign)
    {
        tmp[0] = negative ? '-' : '+';
        ++count;

        // cast through unsigned to avoid UB on INT_MIN (-v would overflow)
        uv = static_cast<uint64_t>(~static_cast<uint64_t>(v) + 1);
    }
    else
    {
        uv = static_cast<uint64_t>(v);
    }

    count += _string_convert_priv::write_unsigned(uv, tmp + count, fmt);
    count = std::min(count, buf_size);
    mem::copy(buf, tmp, count);

    return count;
}

template <typename U, typename C = char, VX_REQUIRES(type_traits::is_unsigned_integral<U>::value&& type_traits::is_char<C>::value)>
size_t to_string(U v, C* buf, size_t buf_size, const integer_formatter& fmt = {}) noexcept
{
    VX_ASSERT(buf);
    VX_ASSERT(2 <= fmt.base && fmt.base <= 36);

    C tmp[_string_convert_priv::max_int_char_count];

    size_t count = 0;

    if (fmt.force_sign)
    {
        tmp[0] = '+';
        ++count;
    }

    count += _string_convert_priv::write_unsigned(static_cast<uint64_t>(v), tmp + count, fmt);
    count = std::min(count, buf_size);
    mem::copy(buf, tmp, count);

    return count;
}

//==============================================================================

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
size_t to_string(F v, C* buf, const size_t buf_size, const float_formatter& fmt = {}) noexcept
{
    if (!buf || buf_size == 0)
    {
        return 0;
    }

    VX_IF_CONSTEXPR (std::is_same<C, char>::value)
    {
        return _string_convert_priv::write_float(v, buf, buf_size, fmt);
    }
    else
    {
        char tmp[_string_convert_priv::max_float_char_count];
        size_t count = _string_convert_priv::write_float(v, tmp, fmt);
        count = std::min(count, buf_size);

        string_cast<C>(tmp, count, buf);
        return count;
    }
}

//==============================================================================

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
auto to_string(I v, const integer_formatter& fmt = {}) noexcept
{
    constexpr size_t buf_size = 65;
    C buf[buf_size];
    const size_t n = to_string<I, C>(v, buf, buf_size, fmt);

    str::basic_string<C> res(buf, n);
    return res;
}

template <typename F, typename C = char, VX_REQUIRES(std::is_arithmetic<F>::value&& type_traits::is_char<C>::value)>
auto to_string(F v, const float_formatter& fmt = {}) noexcept
{
    constexpr size_t buf_size = _string_convert_priv::max_float_char_count;
    C buf[buf_size];
    const size_t n = to_string<F, C>(v, buf, buf_size, fmt);

    str::basic_string<C> res(buf, n);
    return res;
}

//==============================================================================
// from string
//==============================================================================

template <typename I, typename C = char, VX_REQUIRES(type_traits::is_signed_integral<I>::value&& type_traits::is_char<C>::value)>
size_t from_string(const C* s, const size_t count, I& out, const bool clamp = false, const int base = 10) noexcept
{
    VX_ASSERT(s);

    if (count == 0)
    {
        return 0;
    }

    bool negative = false;
    size_t i = 0;

    switch (s[i])
    {
        case '-':
        {
            negative = true;
            VX_FALLTHROUGH;
        }
        case '+':
        {
            ++i;
            break;
        }
    }

    using U = typename std::make_unsigned<I>::type;
    U value = 0;
    bool overflow = false;

    while (i < count)
    {
        const int digit = char_to_digit(s[i], base);
        if (digit < 0)
        {
            break;
        }

        if (value > (std::numeric_limits<U>::max() - static_cast<U>(digit)) / static_cast<U>(base))
        {
            overflow = true;
        }

        out = out * static_cast<I>(base) + static_cast<I>(digit);
        ++i;
    }

    if (overflow && clamp)
    {
        out = negative ? std::numeric_limits<I>::min() : std::numeric_limits<I>::max();
    }
    else
    {
        out = negative ? static_cast<I>(-static_cast<U>(value)) : static_cast<I>(value);
    }

    return i;
}

template <typename U, typename C = char, VX_REQUIRES(type_traits::is_unsigned_integral<U>::value&& type_traits::is_char<C>::value)>
size_t from_string(const C* s, const size_t count, U& out, const int base = 10) noexcept
{
    VX_ASSERT(s);

    out = 0;

    if (count == 0)
    {
        return 0;
    }

    size_t i = 0;

    if (s[i] == '+')
    {
        ++i;
    }

    bool overflow = false;

    while (i < count)
    {
        const int digit = char_to_digit(s[i], base);
        if (digit < 0)
        {
            break;
        }

        if (!overflow)
        {
            if (out > (std::numeric_limits<U>::max() - static_cast<U>(digit)) / static_cast<U>(base))
            {
                overflow = true;
                err::set(err::out_of_range);
            }
        }

        out = out * static_cast<U>(base) + static_cast<U>(digit);
        ++i;
    }

    return i;
}

template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
size_t from_string(const C* s, const size_t count, F& out) noexcept
{
    if (!s)
    {
        err::set(err::invalid_argument);
        return 0;
    }

    C* endptr = nullptr;
    errno = 0;
    F result = 0;

    VX_IF_CONSTEXPR (sizeof(F) <= sizeof(float))
    {
        result = static_cast<F>(std::strtof(s, &endptr));
    }
    else VX_IF_CONSTEXPR (sizeof(F) <= sizeof(double))
    {
        result = static_cast<F>(std::strtod(s, &endptr));
    }
    else
    {
        result = static_cast<F>(std::strtold(s, &endptr));
    }

    if (endptr == s)
    {
        err::set(err::invalid_argument);
        return 0;
    }

    if (errno == ERANGE)
    {
        err::set(err::out_of_range);
        return static_cast<size_t>(endptr - s);
    }

    out = result;
    return static_cast<size_t>(endptr - s);
}

//==============================================================================

template <typename I, typename S, bool allow_overflow = false, VX_REQUIRES(std::is_integral<I>::value&& is_string_like<S>::value)>
size_t from_string(const S& s, I& out, const int base = 10) noexcept
{
    using C = typename S::value_type;
    return from_string<I, C, allow_overflow>(s.data(), s.size(), out, base);
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
