#pragma once

#include "vertex/std/array.hpp"
#include "vertex/std/float_bits.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_convert.hpp"

namespace vx {
namespace fmt {

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
//     * literals are left aligned by default, numbers are right aligned by default
//     * for odd center alignment, the extra character goes to the right of the string
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
// Optional performance switches.
//
// VX_FORMAT_DISABLE_RUNTIME_FORMAT_CHECKS
//   Removes validation of format-string syntax.
//
// VX_FORMAT_DISABLE_RUNTIME_SIZE_CHECKS
//   Removes output buffer bounds checks.
//
// These options are intended for environments where all format strings
// and buffer sizes are statically controlled.
//
// Using these options with untrusted input may result in undefined behavior.
//==============================================================================

// #define VX_FORMAT_DISABLE_RUNTIME_FORMAT_CHECKS
// #define VX_FORMAT_DISABLE_RUNTIME_SIZE_CHECKS

#define _VX_FAIL_IF(cond, ret) \
    do \
    { \
        if ((cond)) \
        { \
            return (ret); \
        } \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#if !defined(VX_FORMAT_DISABLE_RUNTIME_FORMAT_CHECKS)
    #define _FORMAT_RET_IF(cond, ret) _VX_FAIL_IF((cond), (ret))
#else
    #define _FORMAT_RET_IF(cond, ret) VX_ASSERT(!(cond))
#endif

#if !defined(VX_FORMAT_DISABLE_RUNTIME_SIZE_CHECKS)
    #define _SIZE_RET_IF(cond, ret) _VX_FAIL_IF((cond), (ret))
#else
    #define _SIZE_RET_IF(cond, ret) VX_ASSERT(!(cond))
#endif

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

//==============================================================================
// Custom formatter extension point.
//
// formatter<T, C, Enable = void> defines how a value of type T is written using
// character type C.
//
// The third template parameter, Enable, exists to support conditional
// formatter implementations using SFINAE.
//
// Basic specialization:
//
//   struct my_type
//   {
//       int value;
//   };
//
//   template <typename C>
//   struct vx::format::formatter<my_type, C>
//   {
//       format_error format(
//           basic_output_context<C>& out,
//           const my_type& value,
//           const basic_format_spec<C>& spec) const
//       {
//           return out.write_padded(
//               value_text,
//               value_size,
//               spec,
//               alignment::left);
//       }
//   };
//
// Conditional specialization:
//
//   template <typename T, typename C>
//   struct vx::format::formatter<
//       T,
//       C,
//       VX_REQUIRES_TYPE(std::is_enum<T>::value)>
//   {
//       format_error format(
//           basic_output_context<C>& out,
//           const T& value,
//           const basic_format_spec<C>& spec) const
//       {
//           ...
//       }
//   };
//
// Requirements:
//
//   formatter<T,C,Enable> must provide:
//
//       format(
//           basic_output_context<C>&,
//           const T&,
//           const basic_format_spec<C>&)
//
//   The formatter:
//
//     - Must return format_error.
//     - Must write output only through basic_output_context.
//     - Must respect the supplied format specification.
//     - Must not access the output buffer directly.
//
// The Enable parameter should normally be left as void for direct
// specializations. It is intended for generic formatters that apply to a
// family of types.
//
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

inline constexpr padding_info align_padding(
    const size_t padding,
    const alignment align) noexcept
{
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
        _SIZE_RET_IF((count == size), false);
        data[count++] = ch;
        return true;
    }

    bool append(const C* s, size_t n) noexcept
    {
        _SIZE_RET_IF((count + n > size), false);
        mem::copy_range(data + count, s, n);
        count += n;
        return true;
    }

    bool fill(const C c, size_t n) noexcept
    {
        _SIZE_RET_IF((count + n > size), false);
        mem::fill_range(data + count, c, n);
        count += n;
        return true;
    }

    format_error write_padded(
        const C* write_data,
        size_t write_data_size,
        const basic_format_spec<C>& spec,
        alignment default_align) noexcept
    {
        if (spec.has_precision && write_data_size > spec.precision)
        {
            write_data_size = spec.precision;
        }

        const size_t width = (spec.width > write_data_size) ? spec.width : write_data_size;
        const size_t remaining = size - count;
        _SIZE_RET_IF(width > remaining, format_error::buffer_too_small);

        const size_t total_padding = width - write_data_size;
        const alignment align = spec.align == alignment::none ? default_align : spec.align;
        const auto padding = align_padding(total_padding, align);

        C* ptr = data + count;

        mem::fill_range(ptr, padding.left, spec.fill);
        ptr += padding.left;

        mem::copy_range(ptr, write_data, write_data_size);
        ptr += write_data_size;

        mem::fill_range(ptr, padding.right, spec.fill);

        count += width;
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
        const basic_format_spec<C>& spec,
        alignment default_align) const noexcept
    {
        return m_out.write_padded(data, size, spec, default_align);
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

            _FORMAT_RET_IF((empty() || current() != C(closed_brace)), format_error::invalid_format);
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
        _FORMAT_RET_IF(empty(), format_error::invalid_format);

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
        _FORMAT_RET_IF((current() != C(':')), format_error::invalid_format);
        advance(1);

        const auto err = parse_spec(tok.spec);
        if (err != format_error::none)
        {
            return err;
        }

        _FORMAT_RET_IF((empty() || current() != C(closed_brace)), format_error::invalid_format);
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
        if (!empty() && current() != C(closed_brace))
        {
            spec.type = current();
            advance(1);
            _FORMAT_RET_IF((!empty() && current() != C(closed_brace)), format_error::invalid_format);
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
                _FORMAT_RET_IF(
                    (!parser.update_mode(tok.has_index ? format_mode::manual : format_mode::auto_)),
                    (format_result{ format_error::mode_mismatch, 0 }));

                const size_t index = (tok.has_index)
                    ? tok.index
                    : next_arg++;

                _FORMAT_RET_IF((index >= argc), (format_result{ format_error::invalid_argument, 0 }));

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
        const C1& value,
        const basic_format_spec<C2>& spec) const noexcept
    {
        VX_STATIC_ASSERT_MSG(sizeof(C1) <= sizeof(C2), "Narrowing conversion not allowed");

        _FORMAT_RET_IF((spec.has_precision), format_error::invalid_format);

        switch (spec.type)
        {
            case C2('\0'):
            case C2('c'):
            {
                VX_IF_CONSTEXPR (std::is_same<C1, C2>::value)
                {
                    return out.write_padded(&value, 1, spec, alignment::left);
                }
                else
                {
                    const C2 dest_value = static_cast<C2>(value);
                    return out.write_padded(&dest_value, 1, spec, alignment::left);
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

template <typename C>
struct string_formatter
{
    format_error format(
        basic_output_context<C>& out,
        const C* value,
        const size_t size,
        const basic_format_spec<C>& spec) const noexcept
    {
        _FORMAT_RET_IF((spec.sign != sign_option::none), format_error::invalid_format);
        _FORMAT_RET_IF((spec.alternate), format_error::invalid_format);
        _FORMAT_RET_IF((spec.type != 0 && spec.type != C('s')), format_error::invalid_format);

        return out.write_padded(
            value,
            size,
            spec,
            alignment::left);
    }
};

} // namespace _format_priv

template <typename C>
struct formatter<const C*, C>
{
    format_error format(
        basic_output_context<C>& out,
        const C* value,
        const basic_format_spec<C>& spec) const noexcept
    {
        const size_t size = str::length(value);
        return _format_priv::string_formatter<C>{}.format(
            out,
            value,
            size,
            spec);
    }
};

template <typename C, size_t N>
struct formatter<C[N], C>
{
    format_error format(
        basic_output_context<C>& out,
        const C (&value)[N],
        const basic_format_spec<C>& spec) const noexcept
    {
        return _format_priv::string_formatter<C>{}.format(
            out,
            value,
            N,
            spec);
    }
};

//==============================================================================

template <typename S, typename C>
struct formatter<S, C, VX_REQUIRES_TYPE(str::is_string_of<S, C>::value)>
{
    format_error format(
        basic_output_context<C>& out,
        const S& value,
        const basic_format_spec<C>& spec) const noexcept
    {
        return _format_priv::string_formatter<C>{}.format(
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
    const C* prefix,
    const size_t prefix_size,
    const size_t remaining_size,
    const basic_format_spec<C>& spec) noexcept
{
    const size_t total_size = written + prefix_size;
    const size_t width = (spec.width > total_size) ? spec.width : total_size;
    _SIZE_RET_IF(width > remaining_size, format_error::buffer_too_small);

    const size_t total_padding = width - total_size;
    const auto padding = align_padding(total_padding, spec.align);

    const size_t right_shift = padding.left + prefix_size;

    if (right_shift)
    {
        const bool has_sign = ptr[0] == C('+') || ptr[0] == C('-');
        const size_t sign_len = static_cast<size_t>(has_sign);

        C* first_digit = ptr + sign_len;

        // open a gap of `right_shift` chars right after the sign; digits slide past it
        mem::move_range(first_digit + right_shift, first_digit, written - sign_len);

        if (spec.zero_pad)
        {
            // [sign][space?][prefix?][zeros...][digits]
            mem::copy_range(first_digit, prefix, prefix_size);
            first_digit += prefix_size;
            mem::fill_range(first_digit, padding.left, C('0'));
        }
        else
        {
            // [fill...][sign][space?][prefix?][digits]
            C* insert = first_digit + right_shift - prefix_size;
            mem::copy_range(insert, prefix, prefix_size);

            if (has_sign)
            {
                *(--insert) = ptr[0];
            }

            mem::fill_range(ptr, padding.left, spec.fill);
        }
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
        const basic_format_spec<C>& spec) const noexcept
    {
        _FORMAT_RET_IF((spec.has_precision), format_error::invalid_format);

        str::integer_to_string_format_options fmt{};
        fmt.force_sign = spec.sign == sign_option::force;
        fmt.uppercase = spec.type == C('X') || spec.type == C('B');

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
                break;
            }
            case C('o'):
            {
                fmt.base = 8;
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
                _FORMAT_RET_IF(true, format_error::invalid_format);
            }
        }

        C prefix[3];
        size_t prefix_size = 0;
        {
            if (spec.sign == sign_option::space && value >= 0)
            {
                prefix[prefix_size++] = C(' ');
            }
            if (spec.alternate && fmt.base != 10)
            {
                prefix[prefix_size++] = C('0');

                if (fmt.base != 8)
                {
                    prefix[prefix_size++] = spec.type;
                }
            }
        }

        const size_t remaining = out.remaining();
        C* ptr = out.current();

        const auto res = str::to_string<I, C>(value, ptr, remaining, fmt);
        _SIZE_RET_IF((res.err == str::to_string_error::buffer_too_small), format_error::buffer_too_small);

        size_t written = res.count;

        const auto fmt_err = _format_priv::pad_number<C>(
            ptr,
            written,
            prefix,
            prefix_size,
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
        const basic_format_spec<C>& spec) const noexcept
    {
        if (spec.has_precision)
        {
            return format_error::invalid_format;
        }

        switch (spec.type)
        {
            case C('\0'):
            case C('s'):
            {
                static constexpr C true_str[] = { C('t'), C('r'), C('u'), C('e') };
                static constexpr C false_str[] = { C('f'), C('a'), C('l'), C('s'), C('e') };

                if (value)
                {
                    return out.write_padded(
                        true_str,
                        mem::array_size(true_str),
                        spec,
                        alignment::left);
                }
                else
                {
                    return out.write_padded(
                        false_str,
                        mem::array_size(false_str),
                        spec,
                        alignment::left);
                }
            }
            case C('d'):
            {
                return formatter<unsigned int, C>{}.format(
                    out,
                    static_cast<unsigned int>(value),
                    spec);
            }
            default:
            {
                _FORMAT_RET_IF(true, format_error::invalid_format);
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
        const basic_format_spec<C>& spec) const noexcept
    {
        str::float_to_string_format_options<C> fmt{};
        fmt.force_sign = spec.sign == sign_option::force;
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
            case C('g'):
            {
                break;
            }
            case C('f'):
            case C('e'):
            case C('a'):
            {
                fmt.format = static_cast<str::float_format>(lower_type);
                break;
            }
            default:
            {
                _FORMAT_RET_IF(true, format_error::invalid_format);
            }
        }

        fmt.uppercase = is_uppercase;

        const size_t remaining = out.remaining();
        C* ptr = out.current();

        const auto res = str::to_string<F, C>(value, ptr, remaining, fmt);
        _SIZE_RET_IF((res.err == str::to_string_error::buffer_too_small), format_error::buffer_too_small);
        _FORMAT_RET_IF((res.err != str::to_string_error::none), format_error::invalid_format);

        C prefix[3];
        size_t prefix_size = 0;
        {
            if (spec.sign == sign_option::space && !signbit(value))
            {
                prefix[prefix_size++] = C(' ');
            }
            if (fmt.format == str::float_format::hex)
            {
                prefix[prefix_size++] = C('0');
                prefix[prefix_size++] = fmt.uppercase ? C('X') : C('x');
            }
        }

        size_t written = res.count;

        const auto fmt_err = _format_priv::pad_number<C>(
            ptr,
            written,
            prefix,
            prefix_size,
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
        const basic_format_spec<C>& spec) const noexcept
    {
        _FORMAT_RET_IF((spec.has_precision), format_error::invalid_format);
        _FORMAT_RET_IF((spec.sign != sign_option::none), format_error::invalid_format);
        _FORMAT_RET_IF((spec.alternate), format_error::invalid_format);
        _FORMAT_RET_IF((spec.type != 0 && spec.type != C('p')), format_error::invalid_format);

        str::integer_to_string_format_options fmt{};
        fmt.base = 16;
        fmt.uppercase = spec.alternate;

        const uintptr_t addr = reinterpret_cast<uintptr_t>(value);

        const size_t remaining = out.remaining();
        C* ptr = out.current();

        const auto res = str::to_string<uintptr_t, C>(addr, ptr, remaining, fmt);
        _SIZE_RET_IF((res.err == str::to_string_error::buffer_too_small), format_error::buffer_too_small);

        const C prefix[] = {
            C('0'),
            fmt.uppercase ? C('X') : C('x')
        };

        size_t written = res.count;
        const auto fmt_err = _format_priv::pad_number<C>(
            ptr,
            written,
            prefix,
            mem::array_size(prefix),
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
        const basic_format_spec<C>& spec) const noexcept
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
        const basic_format_spec<C>& spec) const noexcept
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
        const basic_format_spec<C>& spec) const noexcept
    {
        return formatter<const void*, C>{}.format(
            out,
            nullptr,
            spec);
    }
};

#if defined(_VX_FAIL_IF)
    #undef _VX_FAIL_IF
#endif

#if defined(_FORMAT_RET_IF)
    #undef _FORMAT_RET_IF
#endif

#if defined(_SIZE_RET_IF)
    #undef _SIZE_RET_IF
#endif

} // namespace fmt
} // namespace vx
