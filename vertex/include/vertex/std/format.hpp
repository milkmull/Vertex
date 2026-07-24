#pragma once

#include "vertex/std/string.hpp"
#include "vertex/std/string_convert.hpp"

namespace vx {
namespace format {

//==============================================================================
// format spec guide
//
// Syntax:
//
//     {}              next argument
//     {:spec}         formatted argument
//
// Spec format:
//
//     {:fill<^>+0#width.precision[type]}
//
// Alignment:
//     <   left
//     >   right
//     ^   center
//     fill may precede alignment (e.g. {:*_>8})
//
// Flags:
//     +   force sign for numbers (+42)
//     0   zero-pad numeric values (sign stays first: -0007)
//     #   alternate mode (currently float exponent sign)
//
// Width:
//     Minimum output width. Padding uses fill character.
//
// Precision:
//     Strings: maximum characters written.
//     Floats: forwarded precision.
//     Integers: unsupported.
//
// Types:
//
//     Integers:
//         d   decimal (default)
//         x/X hex lowercase/uppercase
//         o   octal
//         b/B binary
//
//     Floats:
//         g/G general (default)
//         f/F fixed
//         e/E scientific
//         a/A hexadecimal
//
//     Strings:
//         width, alignment, fill, precision only
//
//     Pointers:
//         0x-prefixed lowercase hex address;
//         width/alignment/fill supported.
//
// Invalid combinations return invalid_format.
//==============================================================================

//==============================================================================
// result types
//==============================================================================

enum class format_error
{
    none,
    buffer_too_small,
    invalid_format,
    invalid_argument
};

struct format_result
{
    format_error err;
    size_t count;
};

//==============================================================================
// helpers
//==============================================================================

namespace _format_priv {

template <typename C>
struct output_buffer
{
    C* data;
    size_t size;
    size_t count = 0;

    bool push(C ch) noexcept
    {
        if (count == size)
        {
            return false;
        }

        data[count++] = ch;
        return true;
    }

    bool append(const C* s, size_t n) noexcept
    {
        if (count + n > size)
        {
            return false;
        }

        mem::copy_range(data + count, s, n);
        count += n;
        return true;
    }

    bool fill(const C c, size_t n) noexcept
    {
        if (count + n > size)
        {
            return false;
        }

        mem::fill_range(data + count, c, n);
        count += n;
        return true;
    }
};

enum class alignment : char
{
    none = 0,
    left = '<',
    right = '>',
    center = '^'
};

template <typename C>
struct basic_format_spec
{
    C fill = C(' ');
    alignment align = alignment::none;

    size_t width = 0;
    size_t precision = 0;
    bool has_precision = false;

    bool alternate = false;
    bool zero_pad = false;
    bool force_sign = false;

    C type = C{};
};

//==============================================================================
// parsing
//==============================================================================

enum class token_type
{
    literal,
    replacement,
    end
};

template <typename C>
struct basic_format_token
{
    token_type type = token_type::end;

    const C* data = nullptr;
    size_t size = 0;

    basic_format_spec<C> spec;
};

//==============================================================================

template <typename C>
class basic_format_parser
{
public:

    basic_format_parser(const C* data, size_t size) noexcept
        : m_data(data)
        , m_size(size)
        , m_index(0)
    {}

    format_error next(basic_format_token<C>& tok)
    {
        tok = {};

        if (empty())
        {
            tok.type = token_type::end;
            return format_error::none;
        }

        // literal
        if (current() != C('{'))
        {
            const size_t start = m_index;
            while (!empty() && current() != C('{'))
            {
                advance(1);
            }

            tok.type = token_type::literal;
            tok.data = m_data + start;
            tok.size = m_index - start;

            return format_error::none;
        }

        // consume '{'
        advance(1);
        if (empty())
        {
            return format_error::invalid_format;
        }

        // {}
        if (current() == C('}'))
        {
            advance(1);
            tok.type = token_type::replacement;
            return format_error::none;
        }

        // {:...}
        if (current() != C(':'))
        {
            return format_error::invalid_format;
        }

        advance(1);
        const auto err = parse_spec(tok.spec);
        if (err != format_error::none)
        {
            return err;
        }

        if (empty() || current() != C('}'))
        {
            return format_error::invalid_format;
        }

        advance(1);
        tok.type = token_type::replacement;
        return format_error::none;
    }

private:

    bool empty() const noexcept
    {
        return m_index < m_size;
    }

    bool has_size(size_t n) const noexcept
    {
        return m_index + n < m_size;
    }

    C peek(size_t off) const noexcept
    {
        return m_data[m_index + 1];
    }

    C current() const noexcept
    {
        return m_data[m_index];
    }

    void advance(size_t n) noexcept
    {
        m_index += n;
    }

    bool parse_uint(size_t& value) noexcept
    {
        value = 0;

        if (empty() || !str::is_digit(current()))
        {
            return false;
        }

        do
        {
            const C c = current();
            value = value * 10 + (c - C('0'));
            advance(1);

        } while (!empty() && str::is_digit(current()));

        return true;
    }

    template <typename C>
    format_error parse_spec(basic_format_spec<C>& spec) noexcept
    {
        // fill + alignment
        if (has_size(1))
        {
            switch (peek(1))
            {
                case C('<'):
                {
                    spec.align = alignment::left;
                    break;
                }
                case C('>'):
                {
                    spec.align = alignment::right;
                    break;
                }
                case C('^'):
                {
                    spec.align = alignment::center;
                    break;
                }
                default:
                {
                    break;
                }
            }

            if (spec.align != alignment::none)
            {
                spec.fill = current();
                advance(2);
            }
        }

        // alignment
        if (!empty() && spec.align == alignment::none)
        {
            switch (current())
            {
                case C('<'):
                {
                    spec.align = alignment::left;
                    advance(1);
                    break;
                }
                case C('>'):
                {
                    spec.align = alignment::right;
                    advance(1);
                    break;
                }
                case C('^'):
                {
                    spec.align = alignment::center;
                    advance(1);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        // flags
        while (!empty())
        {
            const C c = current();

            if (c == C('#'))
            {
                spec.alternate = true;
                advance(1);
            }
            else if (c == C('0'))
            {
                spec.zero_pad = true;
                advance(1);
            }
            else if (c == C('+'))
            {
                spec.force_sign = true;
                advance(1);
            }
            else
            {
                break;
            }
        }

        // width
        if (!empty())
        {
            size_t value = 0;
            if (parse_uint(value))
            {
                spec.width = value;
            }
        }

        // precision
        if (!empty() && current() == C('.'))
        {
            advance(1);
            if (!parse_uint(spec.precision))
            {
                return format_error::invalid_format;
            }

            spec.has_precision = true;
        }

        // type
        if (!empty() && current() != C('}'))
        {
            spec.type = current();
            advance(1);

            if (!empty() && current() != C('}'))
            {
                return format_error::invalid_format;
            }
        }

        return format_error::none;
    }

private:

    const C* m_data;
    size_t m_size;
    size_t m_index;
};

//==============================================================================
// format
//==============================================================================

template <typename T, typename C, typename Enable = void>
struct formatter;

template <typename C>
using format_fn =
    format_error (*)(
        output_buffer<C>&,
        const basic_format_spec<C>&,
        const void*);

template <typename T, typename C>
format_error invoke_formatter(
    output_buffer<C>& out,
    const basic_format_spec<C>& spec,
    const void* ptr)
{
    // preserves array-ness, e.g. const char[6]
    using U = typename std::remove_reference<T>::type;
    using DT = typename std::decay<T>::type;

    formatter<DT, C> f;

    // reinterpret as the *actual* stored object type, not the decayed one
    const U& ref = *static_cast<const U*>(ptr);
    return f.format(out, ref, spec);
}

template <typename C, typename... Args>
format_result format_impl(
    C* out,
    size_t out_size,
    const C* fmt,
    size_t fmt_size,
    Args&&... args) noexcept
{
    const void* values[] = { &args... };
    format_fn<C> funcs[] = { &invoke_formatter<Args, C>... };

    basic_format_parser<C> parser(fmt, fmt_size);
    output_buffer<C> buffer{ out, out_size };

    size_t arg_index = 0;
    basic_format_token<C> tok;

    while (true)
    {
        const auto err = parser.next(tok);
        if (err != format_error::none)
        {
            return { err, buffer.count };
        }

        switch (tok.type)
        {
            case token_type::literal:
            {
                if (!buffer.append(tok.data, tok.size))
                {
                    return { format_error::buffer_too_small, buffer.count };
                }

                break;
            }
            case token_type::replacement:
            {
                if (arg_index >= sizeof...(Args))
                {
                    return { format_error::invalid_argument, buffer.count };
                }

                const auto fmt_err = funcs[arg_index](buffer, tok.spec, values[arg_index]);
                if (fmt_err != format_error::none)
                {
                    return { fmt_err, buffer.count };
                }

                ++arg_index;
                break;
            }
            case token_type::end:
            {
                return { format_error::none, buffer.count };
            }
        }
    }
}

//==============================================================================
// writing helpers
//==============================================================================

template <typename C1, typename C2>
format_error write_padded(
    output_buffer<C1>& out,
    const C2* data,
    size_t size,
    const basic_format_spec<C1>& spec)
{
    if (spec.has_precision && size > spec.precision)
    {
        size = spec.precision;
    }

    const size_t width = (spec.width > size)
        ? spec.width
        : size;

    const size_t padding = width - size;

    size_t left = 0;
    size_t right = 0;

    switch (spec.align)
    {
        case alignment::left:
        {
            right = padding;
            break;
        }
        case alignment::center:
        {
            left = padding / 2;
            right = padding - left;
            break;
        }
        case alignment::none:
        case alignment::right:
        {
            left = padding;
            break;
        }
    }

    if (left && !out.fill(spec.fill, left))
    {
        return format_error::buffer_too_small;
    }
    if (!out.append(data, size))
    {
        return format_error::buffer_too_small;
    }
    if (right && !out.fill(spec.fill, right))
    {
        return format_error::buffer_too_small;
    }

    return format_error::none;
}

template <typename C>
void shift_right_in_place(C* data, size_t size, size_t shift) noexcept
{
    for (size_t i = size; i-- > 0;)
    {
        data[i + shift] = data[i];
    }
}

template <typename C>
format_error pad_in_place(
    output_buffer<C>& out,
    size_t size,
    const basic_format_spec<C>& spec) noexcept
{
    C* const data = out.data + out.count;

    const size_t width = (spec.width > size) ? spec.width : size;
    const size_t padding = width - size;

    if (padding == 0)
    {
        // already exactly the right length -- just claim the bytes
        out.count += size;
        return format_error::none;
    }

    // the token currently occupies [out.count, out.count + size); make sure
    // the fully-padded token also fits before we start shifting anything
    if (out.count + width > out.size)
    {
        return format_error::buffer_too_small;
    }

    if (spec.zero_pad && spec.align == alignment::none)
    {
        const bool has_sign = (size > 0) && (data[0] == C('-') || data[0] == C('+'));
        const size_t sign_len = has_sign ? 1 : 0;

        // make room for the zeros right after the sign, then fill them in
        shift_right_in_place(data + sign_len, size - sign_len, padding);

        for (size_t i = 0; i < padding; ++i)
        {
            data[sign_len + i] = C('0');
        }

        out.count += width;
        return format_error::none;
    }

    size_t left = 0;
    size_t right = 0;

    switch (spec.align)
    {
        case alignment::left:
        {
            right = padding;
            break;
        }

        case alignment::center:
        {
            left = padding / 2;
            right = padding - left;
            break;
        }

        case alignment::none:
        case alignment::right:
        {
            left = padding;
            break;
        }
    }

    if (left > 0)
    {
        shift_right_in_place(data, size, left);
    }

    for (size_t i = 0; i < left; ++i)
    {
        data[i] = spec.fill;
    }

    for (size_t i = 0; i < right; ++i)
    {
        data[left + size + i] = spec.fill;
    }

    out.count += width;
    return format_error::none;
}

//==============================================================================
// formatters
//==============================================================================

template <typename S, typename C>
struct formatter<S, C, VX_REQUIRES_TYPE(str::is_string_of<S, C>::value)>
{
    format_error format(
        output_buffer<C>& out,
        const S& value,
        const basic_format_spec<C>& spec) const
    {
        return write_padded(out,
            value.data(),
            value.size(),
            spec);
    }
};

template <typename C>
struct formatter<const C*, C>
{
    format_error format(
        output_buffer<C>& out,
        const C* value,
        const basic_format_spec<C>& spec) const
    {
        const size_t size = str::length(value);
        return write_padded(
            out,
            value,
            size,
            spec);
    }
};

template <typename I, typename C>
struct formatter<I, C, VX_REQUIRES_TYPE(std::is_integral<I>::value && !std::is_same<I, bool>::value)>
{
    format_error format(
        output_buffer<C>& out,
        const I& value,
        const basic_format_spec<C>& spec) const
    {
        if (spec.has_precision)
        {
            return format_error::invalid_format;
        }

        str::integer_to_string_format_options fmt{};
        fmt.force_sign = spec.force_sign;

        switch (spec.type)
        {
            case C('\0'):
            case C('d'):
            {
                fmt.base = 10;
                fmt.uppercase = false;
                break;
            }
            case C('x'):
            case C('X'):
            {
                fmt.base = 16;
                fmt.uppercase = (spec.type == C('X'));
                break;
            }
            case C('o'):
            {
                fmt.base = 8;
                fmt.uppercase = false;
                break;
            }
            case C('b'):
            case C('B'):
            {
                fmt.base = 2;
                break;
            }
            default:
            {
                return format_error::invalid_format;
            }
        }

        C* const data = out.data + out.count;
        const size_t remaining = out.size - out.count;
        const size_t size = spec.width ? std::min(spec.width, remaining) : remaining;
        const str::to_string_result res = str::to_string<I, C>(value, data, size, fmt);

        if (res.err == str::to_string_error::buffer_too_small)
        {
            return format_error::buffer_too_small;
        }
        if (res.err != str::to_string_error::none)
        {
            return format_error::invalid_argument;
        }

        return pad_in_place(out, res.count, spec);
    }
};

template <typename F, typename C>
struct formatter<F, C, VX_REQUIRES_TYPE(std::is_floating_point<F>::value)>
{
    format_error format(
        output_buffer<C>& out,
        const F& value,
        const basic_format_spec<C>& spec) const
    {
        str::float_to_string_format_options<C> fmt{};
        fmt.force_sign = spec.force_sign;
        fmt.force_exp_sign = spec.alternate;

        if (spec.has_precision)
        {
            fmt.precision = static_cast<decltype(fmt.precision)>(spec.precision);
        }

        switch (spec.type)
        {
            case C('\0'):
            case C('g'):
            case C('G'):
            {
                fmt.format = str::float_format::general;
                fmt.uppercase = (spec.type == C('G'));
                break;
            }
            case C('f'):
            case C('F'):
            {
                fmt.format = str::float_format::fixed;
                fmt.uppercase = (spec.type == C('F'));
                break;
            }
            case C('e'):
            case C('E'):
            {
                fmt.format = str::float_format::scientific;
                fmt.uppercase = (spec.type == C('E'));
                break;
            }
            case C('a'):
            case C('A'):
            {
                fmt.format = str::float_format::hex;
                fmt.uppercase = (spec.type == C('A'));
                break;
            }
            default:
            {
                return format_error::invalid_format;
            }
        }

        C* const data = out.data + out.count;
        const size_t remaining = out.size - out.count;
        const size_t size = spec.width ? std::min(spec.width, remaining) : remaining;
        const str::to_string_result res = str::to_string<F, C>(value, data, size, fmt);

        if (res.err == str::to_string_error::buffer_too_small)
        {
            return format_error::buffer_too_small;
        }
        if (res.err == str::to_string_error::precision_too_large)
        {
            return format_error::invalid_format;
        }
        if (res.err != str::to_string_error::none)
        {
            return format_error::invalid_argument;
        }

        return pad_in_place(out, res.count, spec);
    }
};

//==============================================================================
// pointer formatters
//
// Printed as "0x" + lowercase hex address (uppercase, width, fill etc. from
// the spec still apply to the whole "0x..." token, same as a string would).
// force_sign / zero-pad don't really make sense for addresses, so those
// spec fields are ignored here.
//==============================================================================

template <typename C>
struct formatter<const void*, C>
{
    format_error format(
        output_buffer<C>& out,
        const void* value,
        const basic_format_spec<C>& spec) const
    {
        str::integer_to_string_format_options fmt{};
        fmt.base = 16;
        fmt.uppercase = false;

        const uintptr_t addr = reinterpret_cast<uintptr_t>(value);

        // "0x" + hex digits, written directly into out's own remaining
        // space -- no local buffer.
        if (out.size - out.count < 2)
        {
            return format_error::buffer_too_small;
        }

        C* const data = out.data + out.count;
        data[0] = C('0');
        data[1] = C('x');

        const size_t digits_capacity = out.size - out.count - 2;
        const str::to_string_result res = str::to_string<uintptr_t, C>(addr, data + 2, digits_capacity, fmt);

        if (res.err != str::to_string_error::none)
        {
            return format_error::buffer_too_small;
        }

        return pad_in_place(out, 2 + res.count, spec);
    }
};

template <typename C>
struct formatter<void*, C>
{
    format_error format(
        output_buffer<C>& out,
        void* value,
        const basic_format_spec<C>& spec) const
    {
        return formatter<const void*, C>{}.format(out, value, spec);
    }
};

// catch-all for any other typed pointer (int*, MyStruct*, ...) -- printed as
// an address, same as void*. Note this does NOT apply to `const C*` /
// `C*`, since the string-literal-shaped specialization above is a better
// match for that exact pattern and wins via partial ordering.
template <typename T, typename C>
struct formatter<T*, C>
{
    format_error format(
        output_buffer<C>& out,
        T* value,
        const basic_format_spec<C>& spec) const
    {
        return formatter<const void*, C>{}.format(out, static_cast<const void*>(value), spec);
    }
};

} // namespace _format_priv

//==============================================================================

template <typename C, typename... Args, VX_REQUIRES(type_traits::is_char<C>::value)>
format_result format(
    C* out,
    size_t out_size,
    const C* fmt,
    size_t fmt_size,
    Args&&... args) noexcept
{
    return _format_priv::format_impl(
        out,
        out_size,
        fmt,
        fmt_size,
        std::forward<Args>(args)...);
}

} // namespace format
} // namespace vx
