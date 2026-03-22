#pragma once

#include "vertex/config/assert.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_utils.hpp"
#include "vertex/std/string_cast.hpp"

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
    else if (c >= C('a') && c <= C('z'))
    {
        digit = static_cast<int>(c - C('a')) + 10;
    }
    else if (c >= C('A') && c <= C('Z'))
    {
        digit = static_cast<int>(c - C('A')) + 10;
    }

    return (digit < base) ? digit : -1;
}

//==============================================================================

template <typename IT, typename T, VX_REQUIRES(type_traits::is_iterator<IT>::value&& std::is_integral<T>::value)>
IT parse_digits(IT first, IT last, T& value, size_t* count)
{
    VX_ASSERT(first <= last);

    const IT start = first;
    value = static_cast<T>(0);

    while (first != last && is_digit(*first))
    {
        value = static_cast<T>(value * 10) + static_cast<T>(*first - '0');
        ++first;
    }

    if (count)
    {
        *count = static_cast<size_t>(std::distance(start, first));
    }

    return first;
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
        buf[i * 2 + 0] = hex[(bytes[i] >> 4) & 0xF]; // High nibble
        buf[i * 2 + 1] = hex[(bytes[i] >> 0) & 0xF]; // Low nibble
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
    general     = 'g',
    fixed       = 'f',
    scientific  = 'e',
    hex         = 'a'
};

struct float_formatter
{
    float_format format = float_format::fixed;
    int precision = -1;
    bool force_sign = false;
    bool uppercase = false;
};

//==============================================================================

namespace _to_string_priv {

template <typename U, typename C, VX_REQUIRES(type_traits::is_unsigned_integral<U>::value&& type_traits::is_char<C>::value)>
size_t write_unsigned(U v, C* buf, const size_t buf_size, const integer_formatter& fmt)
{
    static constexpr const C digits[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y', 'z'
    };

    if (v == 0)
    {
        buf[0] = '0';
        return 1;
    }

    size_t i = 0;

    while (v && i < buf_size)
    {
        const char c = digits[v % fmt.base];
        buf[i++] = fmt.uppercase ? to_upper(c) : c;
        v /= fmt.base;
    }

    if (v != 0)
    {
        err::set(err::size_error);
    }

    str::reverse(buf, i);
    return i;
}

template <typename F>
constexpr size_t max_formatted_float_size()
{
    return std::numeric_limits<F>::max_exponent10 // worst case integer part (fixed)
        + std::numeric_limits<F>::max_digits10    // significant digits
        + 6                                       // sign, dot, e+, exp sign
        + 4                                       // max exponent digits (e+4932)
        + 1;                                      // null terminator
}

template <typename F, VX_REQUIRES(std::is_floating_point<F>::value)>
size_t write_float(F v, char* buf, const size_t buf_size, const float_formatter& fmt) noexcept
{
    // Build format string: %[+][.*][format_char]
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

    constexpr bool is_long = sizeof(F) > sizeof(double);
    VX_IF_CONSTEXPR (is_long)
    {
        *p++ = 'L';
    }

    const char f = static_cast<char>(fmt.format);
    *p++ = fmt.uppercase ? to_upper(f) : f;

    *p = '\0';

    int n = 0;
    VX_IF_CONSTEXPR (!is_long)
    {
        n = (fmt.precision < 0)
            ? std::snprintf(buf, buf_size, fmt_str, static_cast<double>(v))
            : std::snprintf(buf, buf_size, fmt_str, fmt.precision, static_cast<double>(v));
    }
    else
    {
        n = (fmt.precision < 0)
            ? std::snprintf(buf, buf_size, fmt_str, v)
            : std::snprintf(buf, buf_size, fmt_str, fmt.precision, v);
    }

    if (n < 0)
    {
        err::set(err::out_of_range);
        return 0;
    }
    if (static_cast<size_t>(n) >= buf_size)
    {
        err::set(err::size_error);
        return buf_size;
    }

    return static_cast<size_t>(n);
}

} // namespace _to_string_priv

//==============================================================================

template <typename I, typename C = char, VX_REQUIRES(type_traits::is_signed_integral<I>::value&& type_traits::is_char<C>::value)>
size_t to_string(I v, C* buf, size_t buf_size, const integer_formatter& fmt = {}) noexcept
{
    if (!buf)
    {
        return 0;
    }

    if (fmt.base < 2 || fmt.base > 36)
    {
        err::set(err::invalid_argument);
        return 0;
    }

    using U = typename std::make_unsigned<I>::type;
    const bool negative = (v < 0);
    size_t count = 0;

    U uv;

    if (negative || fmt.force_sign)
    {
        buf[0] = negative ? '-' : '+';
        ++buf;
        --buf_size;
        ++count;

        // cast through unsigned to avoid UB on INT_MIN (-v would overflow)
        uv = static_cast<U>(~static_cast<U>(v) + 1);
    }
    else
    {
        uv = static_cast<U>(v);
    }

    count += _to_string_priv::write_unsigned(static_cast<U>(v), buf, buf_size, fmt);
    return count;
}

template <typename U, typename C = char, VX_REQUIRES(type_traits::is_unsigned_integral<U>::value&& type_traits::is_char<C>::value)>
size_t to_string(U v, C* buf, size_t buf_size, const integer_formatter& fmt = {}) noexcept
{
    if (!buf)
    {
        return 0;
    }

    if (fmt.base < 2 || fmt.base > 36)
    {
        err::set(err::invalid_argument);
        return 0;
    }

    size_t count = 0;

    if (fmt.force_sign)
    {
        buf[0] = '+';
        ++buf;
        --buf_size;
        ++count;
    }

    count += _to_string_priv::write_unsigned(v, buf, buf_size, fmt);
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
        return _to_string_priv::write_float(v, buf, buf_size, fmt);
    }
    else
    {
        constexpr size_t tmp_size = _to_string_priv::max_formatted_float_size<F>();
        char tmp[tmp_size];
        const size_t n = _to_string_priv::write_float(v, tmp, tmp_size, fmt);

        string_cast<C>(tmp, std::min(n, buf_size), buf);
        return n;
    }
}

//==============================================================================

template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value && type_traits::is_char<C>::value)>
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
    constexpr size_t buf_size = _to_string_priv::max_formatted_float_size<F>();
    C buf[buf_size];
    const size_t n = to_string<F, C>(v, buf, buf_size, fmt);

    str::basic_string<C> res(buf, n);
    return res;
}

//==============================================================================
// from string
//==============================================================================

//template <typename I, typename C = char, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
//size_t from_string(const C* s, const size_t count, I& out, const int base = 10) noexcept
//{
//    if (!s || count == 0)
//    {
//        err::set(err::invalid_argument);
//        return 0;
//    }
//
//#if VX_CPP_STANDARD >= 17 && 0
//
//    const char* first = reinterpret_cast<const char*>(s);
//    const char* last = first + count;
//
//    const std::from_chars_result res = std::from_chars(first, last, out, base);
//
//    if (res.ec == std::errc::invalid_argument)
//    {
//        err::set(err::invalid_argument);
//        return 0;
//    }
//    if (res.ec == std::errc::result_out_of_range)
//    {
//        err::set(err::out_of_range);
//        return static_cast<size_t>(res.ptr - first);
//    }
//
//    return static_cast<size_t>(res.ptr - first);
//
//#elif VX_CPP_STANDARD >= 14
//
//    // C++14: use strtoll/strtoull for full 64-bit coverage
//    const char* end = s + count;
//
//    VX_IF_CONSTEXPR (std::is_signed<I>::value)
//    {
//        const long long val = std::strtoll(buf, &end, base);
//        if (end == buf)
//        {
//            err::set(err::invalid_argument);
//            return 0;
//        }
//        if (errno == ERANGE || val < static_cast<long long>(std::numeric_limits<I>::min()) || val > static_cast<long long>(std::numeric_limits<I>::max()))
//        {
//            err::set(err::out_of_range);
//            return static_cast<size_t>(end - buf);
//        }
//        out = static_cast<I>(val);
//    }
//    else
//    {
//        const unsigned long long val = std::strtoull(buf, &end, base);
//        if (end == buf)
//        {
//            err::set(err::invalid_argument);
//            return 0;
//        }
//        if (errno == ERANGE || val > static_cast<unsigned long long>(std::numeric_limits<I>::max()))
//        {
//            err::set(err::out_of_range);
//            return static_cast<size_t>(end - buf);
//        }
//        out = static_cast<I>(val);
//    }
//
//#endif
//
//
//    bool negative = false;
//    size_t i = 0;
//
//    switch (s[i])
//    {
//        case '-':
//        {
//            negative = true;
//            VX_FALLTHROUGH;
//        }
//        case '+':
//        {
//            ++i;
//            break;
//        }
//    }
//
//    if (i == count)
//
//
//        C* endptr = nullptr;
//    errno = 0;
//
//    constexpr size_t buf_size = _to_string_priv::buffer_size<N>::value;
//    C buf[buf_size] = {};
//    mem::copy_range(buf, s, std::max(count, buf_size));
//
//    if (str::is)
//
//        I result = 0;
//
//    VX_IF_CONSTEXPR (std::is_signed<I>::value)
//    {
//        VX_IF_CONSTEXPR (sizeof(I) <= sizeof(long))
//        {
//            result = static_cast<I>(std::strtol(s, &endptr, base));
//        }
//        else
//        {
//            result = static_cast<I>(std::strtoll(s, &endptr, base));
//        }
//    }
//    else
//    {
//        VX_IF_CONSTEXPR (sizeof(I) <= sizeof(unsigned long))
//        {
//            result = static_cast<I>(std::strtoul(s, &endptr, base));
//        }
//        else
//        {
//            result = static_cast<I>(std::strtoull(s, &endptr, base));
//        }
//    }
//
//    if (endptr == s)
//    {
//        err::set(err::invalid_argument);
//    }
//
//    if (errno == ERANGE)
//    {
//        err::set(err::out_of_range);
//    }
//
//    out = result;
//    return static_cast<size_t>(endptr - s);
//}
//
//template <typename F, typename C = char, VX_REQUIRES(std::is_floating_point<F>::value&& type_traits::is_char<C>::value)>
//size_t from_string(const C* s, const size_t count, F& out) noexcept
//{
//    if (!s || count == 0)
//    {
//        err::set(err::invalid_argument);
//        return 0;
//    }
//
//    C* endptr = nullptr;
//    errno = 0;
//
//    F result = 0;
//
//    VX_IF_CONSTEXPR (sizeof(F) <= sizeof(float))
//    {
//        result = static_cast<F>(std::strtof(s, &endptr));
//    }
//    else VX_IF_CONSTEXPR (sizeof(F) <= sizeof(double))
//    {
//        result = static_cast<F>(std::strtod(s, &endptr));
//    }
//    else
//    {
//        result = static_cast<F>(std::strtold(s, &endptr));
//    }
//
//    if (endptr == s)
//    {
//        err::set(err::invalid_argument);
//    }
//
//    if (errno == ERANGE)
//    {
//        err::set(err::out_of_range);
//    }
//
//    out = result;
//    return static_cast<size_t>(endptr - s);
//}

//==============================================================================

template <typename I, typename C, VX_REQUIRES(std::is_integral<I>::value&& type_traits::is_char<C>::value)>
size_t from_string(const C* s, I& out, const int base = 10) noexcept
{
    const size_t count = str::length(s);
    return from_string<I, C>(s, count, out, base);
}

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
