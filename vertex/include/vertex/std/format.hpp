#pragma once

#include "vertex/std/array.hpp"
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
//     {n}             argument at index n
//
//     {:spec}         next argument with formatting
//     {n:spec}        argument at index n with formatting
//
// Examples:
//
//     "{} {}"         automatic indexing
//     "{1} {0}"       indexed arguments
//     "{0:08X}"       indexed + formatting
//     "{:^10}"        automatic + formatting
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
// Notes:
//
//     Automatic ({}) and indexed ({0}) arguments may not be mixed
//     within the same format string.
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
    invalid_argument,
    mode_mismatch
};

struct format_result
{
    format_error err;
    size_t count;
};

//==============================================================================
// format spec
//==============================================================================

enum class alignment : char
{
    none = 0,
    left = '<',
    right = '>',
    center = '^'
};

enum class sign_option
{
    none,
    force,
    space
};

namespace _format_priv {

inline constexpr size_t calulate_field_width(
    const size_t remaining_size,
    const size_t width) noexcept
{
    if (width > 0)
    {
        return (width < remaining_size)
            ? width
            : remaining_size;
    }

    return remaining_size;
}

} // namespace _format_priv

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
    sign_option sign = sign_option::none;

    C type = C{};

    size_t calulate_width(const size_t remaining_size) const noexcept
    {
        return _format_priv::calulate_field_width(remaining_size, width);
    }
};

using format_spec = basic_format_spec<char>;
using wformat_spec = basic_format_spec<wchar_t>;
#if defined(__cpp_lib_char8_t)
using u8format_spec = basic_format_spec<char8_t>;
#endif // defined(__cpp_lib_char8_t)
using u16format_spec = basic_format_spec<char16_t>;
using u32format_spec = basic_format_spec<char32_t>;

//==============================================================================
// formatter
//==============================================================================

template <typename T, typename C, typename Enable = void>
struct formatter;

//==============================================================================
// output context
//==============================================================================

namespace _format_priv {

struct padding_info
{
    size_t left;
    size_t right;
};

inline constexpr padding_info calulate_padding(
    const size_t size,
    const size_t spec_width,
    const alignment align) noexcept
{
    const size_t width = (spec_width > size)
        ? spec_width
        : size;

    const size_t padding = width - size;

    switch (align)
    {
        case alignment::left:
        {
            return { 0, padding };
        }
        case alignment::center:
        {
            const size_t left = padding / 2;
            return { left, padding - left };
        }
        case alignment::right:
        default:
        {
            return { padding, 0 };
        }
    }
}

template <typename C>
struct output_buffer
{
    C* data;
    size_t size;
    size_t count;

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

    format_error write_padded(
        const C* write_data,
        size_t write_data_size,
        const basic_format_spec<C>& spec)
    {
        if (spec.has_precision && write_data_size > spec.precision)
        {
            write_data_size = spec.precision;
        }

        const size_t remaining = size - count;
        if (write_data_size > remaining)
        {
            return format_error::buffer_too_small;
        }

        const auto padding = calulate_padding(write_data_size, spec.width, spec.align);
        C* ptr = data + count;

        mem::fill_range(ptr, padding.left, spec.fill);
        ptr += padding.left;

        mem::copy_range(ptr, write_data, write_data_size);
        ptr += write_data_size;

        mem::fill_range(ptr, padding.right, spec.fill);

        count += write_data_size;
        return format_error::none;
    }
};

} // namespace _format_priv

template <typename C>
class basic_output_context
{
public:

    explicit basic_output_context(_format_priv::output_buffer<C>& out) noexcept
        : m_out(out)
    {}

    const C* current() const noexcept
    {
        return m_out.data;
    }

    C* current() noexcept
    {
        return m_out.data + m_out.count;
    }

    size_t size() const noexcept
    {
        return m_out.size;
    }

    size_t remaining() const noexcept
    {
        return m_out.size - m_out.count;
    }

    size_t written() const noexcept
    {
        return m_out.count;
    }

    bool commit(size_t count) noexcept
    {
        const size_t rem = remaining();
        VX_ASSERT(count <= rem);
        if (count > rem)
        {
            std::abort();
            VX_UNREACHABLE();
        }

        m_out.count += count;
        return true;
    }

    bool push(C ch) noexcept
    {
        return m_out.push(ch);
    }

    bool append(const C* str, size_t len) noexcept
    {
        return m_out.append(str, len);
    }

    bool fill(C ch, size_t count) noexcept
    {
        return m_out.fill(ch, count);
    }

    format_error write_padded(
        const C* data,
        size_t size,
        const basic_format_spec<C>& spec) const noexcept
    {
        return m_out.write_padded(data, size, spec);
    }

    // pad after writing the value
    format_error pad_in_place(
        size_t size,
        const basic_format_spec<C>& spec,
        bool is_number = false) noexcept
    {
        return m_out.pad_in_place(size, spec, is_number);
    }

private:

    _format_priv::output_buffer<C>& m_out;
};

using output_context = basic_output_context<char>;
using woutput_context = basic_output_context<wchar_t>;
#if defined(__cpp_lib_char8_t)
using u8output_context = basic_output_context<char8_t>;
#endif // defined(__cpp_lib_char8_t)
using u16output_context = basic_output_context<char16_t>;
using u32output_context = basic_output_context<char32_t>;

//==============================================================================
// parsing
//==============================================================================

namespace _format_priv {

enum : char
{
    open_brace = '{',
    closed_brace = '}'
};

enum class token_type
{
    literal,
    replacement,
    escaped,
    end
};

template <typename C>
struct basic_format_token
{
    token_type type = token_type::end;

    const C* data = nullptr;
    size_t size = 0;

    bool has_index = false;
    size_t index = 0;

    basic_format_spec<C> spec;
};

//==============================================================================

enum class format_mode
{
    default_,
    auto_,
    manual
};

template <typename C>
class basic_format_parser
{
public:

    basic_format_parser(const C* data, size_t size) noexcept
        : m_data(data)
        , m_size(size)
        , m_index(0)
        , m_mode(format_mode::default_)
    {}

    bool update_mode(format_mode mode) noexcept
    {
        if (m_mode == format_mode::default_)
        {
            m_mode = mode;
            return true;
        }

        return m_mode == mode;
    }

    format_error next(basic_format_token<C>& tok)
    {
        tok = {};

        if (empty())
        {
            tok.type = token_type::end;
            return format_error::none;
        }

        // escaped '}': "}}" -> "}"
        if (current() == C(closed_brace))
        {
            advance(1);

            if (empty() || current() != C(closed_brace))
            {
                return format_error::invalid_format;
            }

            advance(1);

            tok.type = token_type::escaped;
            return format_error::none;
        }

        // literal
        if (current() != C(open_brace))
        {
            const size_t start = m_index;

            while (!empty())
            {
                if (current() == C(open_brace) || current() == C(closed_brace))
                {
                    break;
                }

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

        // escaped '{': "{{" -> "{"
        if (current() == C(open_brace))
        {
            advance(1);

            tok.type = token_type::escaped;
            return format_error::none;
        }

        // {}
        if (current() == C(closed_brace))
        {
            advance(1);

            tok.type = token_type::replacement;
            return format_error::none;
        }

        // optional index
        if (str::is_digit(current()))
        {
            if (!parse_uint(tok.index))
            {
                return format_error::invalid_format;
            }

            tok.has_index = true;
        }

        // {0} or {0:...}
        if (current() == C(closed_brace))
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

        if (empty() || current() != C(closed_brace))
        {
            return format_error::invalid_format;
        }

        advance(1);

        tok.type = token_type::replacement;
        return format_error::none;
    }

    C last() const noexcept
    {
        VX_ASSERT(m_index > 0);
        return m_data[m_index - 1];
    }

private:

    bool empty() const noexcept
    {
        return m_index >= m_size;
    }

    bool has_size(size_t n) const noexcept
    {
        return m_index + n < m_size;
    }

    C peek(size_t off) const noexcept
    {
        return m_data[m_index + off];
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
        const C* ptr = m_data + m_index;
        const size_t size = m_size - m_index;

        using U = typename std::remove_reference<decltype(value)>::type;
        const auto res = str::_string_convert_priv::parse_integer_impl<U, C, false>(
            ptr,
            size,
            value,
            10);

        if (res.err != str::from_string_error::none)
        {
            return false;
        }

        m_index += res.count;
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
                spec.sign = sign_option::force;
                advance(1);
            }
            else if (c == C(' '))
            {
                spec.sign = sign_option::space;
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
    format_mode m_mode;
};

//==============================================================================
// format
//==============================================================================

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
    basic_output_context<C> ctx{ out };

    // reinterpret as the *actual* stored object type, not the decayed one
    const U& ref = *static_cast<const U*>(ptr);
    return f.format(ctx, ref, spec);
}

template <typename C, typename... Args>
format_result format_impl(
    C* out,
    size_t out_size,
    const C* fmt,
    size_t fmt_size,
    Args&&... args) noexcept
{
    constexpr size_t argc = sizeof...(Args);
    const array<const void*, argc> values = { &args... };
    const array<format_fn<C>, argc> funcs = { &invoke_formatter<Args, C>... };

    basic_format_parser<C> parser(fmt, fmt_size);
    output_buffer<C> buffer{ out, out_size, 0 };

    size_t next_arg = 0;
    basic_format_token<C> tok;

    while (true)
    {
        const auto err = parser.next(tok);
        if (err != format_error::none)
        {
            return { err, 0 };
        }

        switch (tok.type)
        {
            case token_type::literal:
            {
                if (!buffer.append(tok.data, tok.size))
                {
                    return { format_error::buffer_too_small, 0 };
                }

                break;
            }
            case token_type::escaped:
            {
                if (!buffer.push(parser.last()))
                {
                    return { format_error::buffer_too_small, 0 };
                }

                break;
            }
            case token_type::replacement:
            {
                if (!parser.update_mode(tok.has_index
                            ? format_mode::manual
                            : format_mode::auto_))
                {
                    return { format_error::mode_mismatch, 0 };
                }

                const size_t index = (tok.has_index)
                    ? tok.index
                    : next_arg++;

                if (index >= argc)
                {
                    return { format_error::invalid_argument, 0 };
                }

                const auto fmt_err = funcs[index](buffer, tok.spec, values[index]);
                if (fmt_err != format_error::none)
                {
                    return { fmt_err, 0 };
                }

                break;
            }
            case token_type::end:
            {
                return { format_error::none, buffer.count };
            }
        }
    }
}

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

//==============================================================================
// formatters
//==============================================================================

template <typename C1, typename C2>
struct formatter<C1, C2, VX_REQUIRES_TYPE(type_traits::is_char<C1>::value)>
{
    format_error format(
        basic_output_context<C2>& out,
        const C2& value,
        const basic_format_spec<C2>& spec) const
    {
        switch (spec.type)
        {
            case C2('\0'):
            case C2('c'):
            {
                VX_IF_CONSTEXPR (std::is_same<C1, C2>::value)
                {
                    return out.write_padded(&value, 1, spec);
                }
                else
                {
                    using traits = utf::utf_traits<C2>;
                    constexpr size_t max_width = traits::max_width();
                    C2 buf[max_width];

                    const auto last = str::string_cast<C2>(&value, 1, buf);
                    const size_t count = static_cast<size_t>(last - buf);

                    return out.write_padded(buf, count, spec);
                }
            }
            default:
            {
                using U = typename std::make_unsigned<C2>::type;
                return formatter<U, C2>{}.format(
                    out,
                    static_cast<U>(value),
                    spec);
            }
        }
    }
};

//==============================================================================

namespace _format_priv {

template <typename C1, typename C2>
struct string_formatter
{
    format_error format(
        basic_output_context<C2>& out,
        const C1* value,
        const size_t size,
        const basic_format_spec<C2>& spec) const
    {
        VX_IF_CONSTEXPR (std::is_same<C1, C2>::value)
        {
            return out.write_padded(
                value,
                size,
                spec);
        }
        else
        {
            const C2* start_pos = out.current();

            using traits = utf::utf_traits<C2>;
            constexpr size_t max_width = traits::max_width();
            C2 buf[max_width];

            const C1* ptr = value;
            const C1* end = value + size;

            while (ptr != end)
            {
                const C2* last = str::char_cast<C2>(ptr, end, buf);
                const size_t count = static_cast<size_t>(last - buf);
                if (!out.append(buf, count))
                {
                    return format_error::buffer_too_small;
                }
            }

            const size_t total_written = static_cast<size_t>(out.current() - start_pos);
            return out.pad_in_place(total_written, spec);
        }
    }
};

} // namespace _format_priv

template <typename C1, typename C2>
struct formatter<const C1*, C2, VX_REQUIRES_TYPE(type_traits::is_char<C1>::value)>
{
    format_error format(
        basic_output_context<C2>& out,
        const C1* value,
        const basic_format_spec<C2>& spec) const
    {
        const size_t size = str::length(value);
        return _format_priv::string_formatter<C1, C2>{}.format(
            out,
            value,
            size,
            spec);
    }
};

template <typename C1, size_t N, typename C2>
struct formatter<C1[N], C2, VX_REQUIRES_TYPE(type_traits::is_char<C1>::value)>
{
    format_error format(
        basic_output_context<C2>& out,
        const C1 (&value)[N],
        const basic_format_spec<C2>& spec) const
    {
        return _format_priv::string_formatter<C1, C2>{}.format(
            out,
            value,
            N,
            spec);
    }
};

//==============================================================================

template <typename S, typename C2>
struct formatter<S, C2, VX_REQUIRES_TYPE(str::is_string_like<S>::value)>
{
    format_error format(
        basic_output_context<C2>& out,
        const S& value,
        const basic_format_spec<C2>& spec) const
    {
        using C1 = typename S::value_type;
        return _format_priv::string_formatter<C1, C2>{}.format(
            out,
            value.data(),
            value.size(),
            spec);
    }
};

//==============================================================================

namespace _format_priv {

template <typename C>
format_error pad_number(
    // pointer to beginning of written data
    C* ptr,
    size_t& written,
    const C prefix,
    const bool add_space,
    const size_t remaining_size,
    const basic_format_spec<C>& spec) noexcept
{
    if (spec.zero_pad && spec.align == alignment::left)
    {
        return format_error::invalid_format;
    }

    const size_t prefix_size = prefix ? 2 : 0;
    const size_t space_size = static_cast<size_t>(add_space);
    const size_t extra_prefix = space_size + prefix_size;
    const size_t total_size = written + extra_prefix;

    const size_t width = (spec.width > total_size) ? spec.width : total_size;
    if (width > remaining_size)
    {
        return format_error::buffer_too_small;
    }

    const bool has_sign = add_space || ptr[0] == C('+') || ptr[0] == C('-');
    const size_t sign_len = static_cast<size_t>(has_sign);

    const auto padding = calulate_padding(total_size - sign_len, width, spec.align);
    const size_t right_shift = padding.left + extra_prefix;

    if (right_shift)
    {
        C* first_digit = ptr + sign_len;

        // make room for the prefix and/or zeros right after the sign
        mem::move_range(first_digit + right_shift, first_digit, written - sign_len);

        if (add_space)
        {
            *first_digit++ = C(' ');
        }

        if (prefix)
        {
            *first_digit++ = C('0');
            *first_digit++ = prefix;
        }

        mem::fill_range(first_digit, padding.left, spec.zero_pad ? C('0') : spec.fill);
    }

    mem::fill_range(ptr + right_shift + written, padding.right, spec.fill);

    written += right_shift + padding.right;
    return format_error::none;
}

} // namespace _format_priv

template <typename I, typename C>
struct formatter<I, C, VX_REQUIRES_TYPE(std::is_integral<I>::value && !type_traits::is_char<I>::value && !std::is_same<I, bool>::value)>
{
    format_error format(
        basic_output_context<C>& out,
        const I& value,
        const basic_format_spec<C>& spec) const
    {
        if (spec.has_precision)
        {
            return format_error::invalid_format;
        }

        str::integer_to_string_format_options fmt{};
        fmt.force_sign = spec.sign == sign_option::force;
        bool prefix = false;

        switch (spec.type)
        {
            case C('\0'):
            case C('d'):
            {
                break;
            }
            case C('x'):
            case C('X'):
            {
                fmt.base = 16;
                fmt.uppercase = (spec.type == C('X'));
                prefix = spec.alternate;
                break;
            }
            case C('o'):
            {
                fmt.base = 8;
                prefix = spec.alternate;
                break;
            }
            case C('b'):
            case C('B'):
            {
                fmt.base = 2;
                fmt.uppercase = (spec.type == C('B'));
                prefix = spec.alternate;
                break;
            }
            default:
            {
                return format_error::invalid_format;
            }
        }

        const size_t remaining = out.remaining();
        C* ptr = out.current();

        const auto res = str::to_string<I, C>(value, ptr, remaining, fmt);
        if (res.err == str::to_string_error::buffer_too_small)
        {
            return format_error::buffer_too_small;
        }

        const bool add_space = (spec.sign == sign_option::space && value >= 0);
        size_t written = res.count;

        const auto fmt_err = _format_priv::pad_number<C>(
            ptr,
            written,
            prefix ? spec.type : 0,
            add_space,
            remaining,
            spec);

        if (fmt_err == format_error::none)
        {
            out.commit(written);
        }

        return fmt_err;
    }
};

template <typename C>
struct formatter<bool, C>
{
    format_error format(
        basic_output_context<C>& out,
        const bool& value,
        const basic_format_spec<C>& spec) const
    {
        switch (spec.type)
        {
            case C('\0'):
            {
                static constexpr C true_str[] = { C('t'), C('r'), C('u'), C('e') };
                static constexpr C false_str[] = { C('f'), C('a'), C('l'), C('s'), C('e') };

                return value
                    ? out.write_padded(true_str, 4, spec)
                    : out.write_padded(false_str, 5, spec);
            }
            default:
            {
                return formatter<unsigned int, C>{}.format(
                    out,
                    value ? 1u : 0u,
                    spec);
            }
        }
    }
};

template <typename F, typename C>
struct formatter<F, C, VX_REQUIRES_TYPE(std::is_floating_point<F>::value)>
{
    format_error format(
        basic_output_context<C>& out,
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

        const C type = spec.type;
        const bool is_uppercase = str::is_upper(type);
        const C lower_type = is_uppercase
            ? str::to_lower_ascii_unchecked(type)
            : type;

        switch (lower_type)
        {
            case C('\0'):
            {
                break;
            }
            case C('g'):
            case C('f'):
            case C('e'):
            case C('a'):
            {
                fmt.format = static_cast<str::float_format>(lower_type);
                break;
            }
            default:
            {
                return format_error::invalid_format;
            }
        }

        fmt.uppercase = is_uppercase;

        const size_t remaining = out.remaining();
        C* ptr = out.current();

        const auto res = str::to_string<F, C>(value, ptr, remaining, fmt);
        if (res.err == str::to_string_error::buffer_too_small)
        {
            return format_error::buffer_too_small;
        }
        if (res.err != str::to_string_error::none)
        {
            return format_error::invalid_argument;
        }

        const bool prefix = (lower_type == C('a'));
        const bool add_space = (spec.sign == sign_option::space && !std::signbit(value));
        size_t written = res.count;

        const auto fmt_err = _format_priv::pad_number<C>(
            ptr,
            written,
            prefix ? (type + 23) : 0, // x or X
            add_space,
            remaining,
            spec);

        if (fmt_err == format_error::none)
        {
            out.commit(written);
        }

        return fmt_err;
    }
};

//==============================================================================

template <typename C>
struct formatter<const void*, C>
{
    format_error format(
        basic_output_context<C>& out,
        const void* value,
        const basic_format_spec<C>& spec) const
    {
        str::integer_to_string_format_options fmt{};
        fmt.base = 16;
        fmt.uppercase = spec.alternate;

        const uintptr_t addr = reinterpret_cast<uintptr_t>(value);

        const size_t remaining = out.remaining();
        C* ptr = out.current();

        const auto res = str::to_string<uintptr_t, C>(addr, ptr, remaining, fmt);
        if (res.err == str::to_string_error::buffer_too_small)
        {
            return format_error::buffer_too_small;
        }

        size_t written = res.count;
        const auto fmt_err = _format_priv::pad_number<C>(
            ptr,
            written,
            fmt.uppercase ? C('X') : C('x'),
            false,
            remaining,
            spec);

        if (fmt_err == format_error::none)
        {
            out.commit(written);
        }

        return fmt_err;
    }
};

template <typename C>
struct formatter<void*, C>
{
    format_error format(
        basic_output_context<C>& out,
        void* value,
        const basic_format_spec<C>& spec) const
    {
        return formatter<const void*, C>{}.format(out, value, spec);
    }
};

template <typename T, typename C>
struct formatter<T*, C, VX_REQUIRES_TYPE(!type_traits::is_char<T>::value)>
{
    format_error format(
        basic_output_context<C>& out,
        T* value,
        const basic_format_spec<C>& spec) const
    {
        return formatter<const void*, C>{}.format(out, static_cast<const void*>(value), spec);
    }
};

template <typename C>
struct formatter<std::nullptr_t, C>
{
    format_error format(
        basic_output_context<C>& out,
        std::nullptr_t,
        const basic_format_spec<C>& spec) const
    {
        return formatter<const void*, C>{}.format(
            out,
            nullptr,
            spec);
    }
};

} // namespace format
} // namespace vx
