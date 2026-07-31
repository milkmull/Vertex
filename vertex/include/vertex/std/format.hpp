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

namespace _fmt_priv {

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

} // namespace _fmt_priv

template <typename C>
struct basic_format_spec
{
    C type = C{};
    C fill = C(' ');
    alignment align = alignment::none;
    size_t width = 0;
};

template <typename C>
struct basic_string_format_spec : basic_format_spec<C>
{
    size_t precision = 0;
    bool has_precision = false;
};

template <typename C>
struct basic_integer_format_spec : basic_format_spec<C>
{
    bool alternate = false;
    bool zero_pad = false;
    sign_option sign = sign_option::none;
};

template <typename C>
struct basic_float_format_spec : basic_integer_format_spec<C>
{
    size_t precision = 0;
    bool has_precision = false;
};

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
//           basic_format_context<C>& out,
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
//           basic_format_context<C>& out,
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
//           basic_format_context<C>&,
//           const T&,
//           const basic_format_spec<C>&)
//
//   The formatter:
//
//     - Must return format_error.
//     - Must write output only through basic_format_context.
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

namespace _fmt_priv {

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

} // namespace _fmt_priv

template <typename C>
class basic_format_context
{
public:

    explicit basic_format_context(_fmt_priv::output_buffer<C>& out) noexcept
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
        const basic_integer_format_spec<C>& spec,
        alignment default_align) const noexcept
    {
        return m_out.write_padded(data, size, spec, default_align);
    }

    format_error write_padded(
        const C* data,
        size_t size,
        const basic_string_format_spec<C>& spec,
        alignment default_align) const noexcept
    {
        if (spec.has_precision && size > spec.precision)
        {
            size = spec.precision;
        }

        return m_out.write_padded(data, size, spec, default_align);
    }


private:

    _fmt_priv::output_buffer<C>& m_out;
};

using format_context = basic_format_context<char>;

//==============================================================================
// parsing
//==============================================================================

namespace _fmt_priv {

template <typename C>
struct basic_format_token;

template <typename C>
class basic_format_parser;

enum : char
{
    open_brace = '{',
    closed_brace = '}'
};

template <typename C>
size_t parse_uint_impl(size_t& value, const C* data, const size_t size) noexcept
{
    using U = typename std::remove_reference<decltype(value)>::type;
    const auto res = strconv::_strconv_priv::parse_integer_impl<U, C, false>(
        data,
        size,
        value,
        10);

    return (res.err != strconv::from_string_error::none)
        ? 0
        : res.count;
}

} // namespace _fmt_priv

template <typename C>
class basic_parse_context
{
private:

    friend _fmt_priv::basic_format_token<C>;
    friend _fmt_priv::basic_format_parser<C>;

    basic_parse_context() noexcept
        : m_data(nullptr)
        , m_size(0)
    {}

public:

    constexpr size_t size() const noexcept
    {
        return m_size;
    }

    constexpr const C* data() const noexcept
    {
        return m_data;
    }

    constexpr auto begin() const noexcept
    {
        return pointer_iterator{ m_data };
    }

    constexpr auto end() const noexcept
    {
        return pointer_iterator{ m_data + m_size };
    }

private:

    constexpr bool parse_uint(size_t& value, size_t& i) const noexcept
    {
        const C* ptr = m_data + i;
        const size_t size = m_size - i;

        const size_t count = _fmt_priv::parse_uint_impl<C>(value, ptr, size);
        i += count;

        return (count != 0);
    }

    static constexpr bool parse_alignment(const C c, alignment& align) noexcept
    {
        switch (c)
        {
            case C('<'):
            {
                align = alignment::left;
                return true;
            }
            case C('>'):
            {
                align = alignment::right;
                return true;
            }
            case C('^'):
            {
                align = alignment::center;
                return true;
            }
            default:
            {
                return false;
            }
        }
    }

    constexpr void parse_basic_fill_alignment(size_t& i, C& fill, alignment& align) const noexcept
    {
        // fill + alignment
        if (i + 1 < m_size)
        {
            if (parse_alignment(m_data[i + 1], align))
            {
                fill = m_data[i];
                i += 2;
                return;
            }
        }

        if (parse_alignment(m_data[i], align))
        {
            ++i;
        }
    }

    constexpr bool check_basic_numeric_flags(size_t& i) const noexcept
    {
        return (m_data[i] == C('+')) || (m_data[i] == C(' ')) || (m_data[i] == C('#')) || (m_data[i] == C('0'));
    }

    constexpr void parse_basic_numeric_flags(size_t& i, bool& alternate, bool& zero_pad, sign_option& sign) const noexcept
    {
        if (i < m_size)
        {
            if (m_data[i] == C('+'))
            {
                sign = sign_option::force;
                ++i;
            }
            else if (m_data[i] == C(' '))
            {
                sign = sign_option::space;
                ++i;
            }
        }

        if (i < m_size && m_data[i] == C('#'))
        {
            alternate = true;
            ++i;
        }

        if (i < m_size && m_data[i] == C('0'))
        {
            zero_pad = true;
            ++i;
        }
    }

    constexpr void parse_basic_width(size_t& i, size_t& width) const noexcept
    {
        if (i < m_size)
        {
            size_t value = 0;
            if (parse_uint(value, i))
            {
                width = value;
            }
        }
    }

    constexpr bool parse_basic_precision(size_t& i, size_t& precision, bool& has_precision) const noexcept
    {
        if ((i < m_size) && (m_data[i] == C('.')))
        {
            ++i;

            const bool parse_result = parse_uint(precision, i);
            _FORMAT_RET_IF((!parse_result), false);
            has_precision = true;
            return true;
        }

        return true;
    }

    constexpr void parse_basic_type(size_t& i, C& type) const noexcept
    {
        if ((i < m_size) && (m_data[i] != C(_fmt_priv::closed_brace)))
        {
            type = m_data[i];
            ++i;
        }
    }

public:

    constexpr format_error parse_basic_spec(basic_format_spec<C>& spec) const noexcept
    {
        size_t i = 0;

        parse_basic_fill_alignment(i, spec.fill, spec.align);
        _FORMAT_RET_IF((check_basic_numeric_flags(i)), format_error::invalid_format);
        parse_basic_width(i, spec.width);
        parse_basic_type(i, spec.type);

        _FORMAT_RET_IF((m_data[i] != C(_fmt_priv::closed_brace)), format_error::invalid_format);
        return format_error::none;
    }

    constexpr format_error parse_basic_string_spec(basic_string_format_spec<C>& spec) const noexcept
    {
        size_t i = 0;

        parse_basic_fill_alignment(i, spec.fill, spec.align);
        parse_basic_width(i, spec.width);
        if (!parse_basic_precision(i, spec.precision, spec.has_precision))
        {
            return format_error::invalid_format;
        }
        parse_basic_type(i, spec.type);

        _FORMAT_RET_IF((m_data[i] != C(_fmt_priv::closed_brace)), format_error::invalid_format);
        return format_error::none;
    }

    constexpr format_error parse_basic_integer_spec(basic_integer_format_spec<C>& spec) const noexcept
    {
        size_t i = 0;

        parse_basic_fill_alignment(i, spec.fill, spec.align);
        parse_basic_numeric_flags(i, spec.alternate, spec.zero_pad, spec.sign);
        parse_basic_width(i, spec.width);
        parse_basic_type(i, spec.type);

        _FORMAT_RET_IF((m_data[i] != C(_fmt_priv::closed_brace)), format_error::invalid_format);
        return format_error::none;
    }

    constexpr format_error parse_basic_float_spec(basic_float_format_spec<C>& spec) const noexcept
    {
        size_t i = 0;

        parse_basic_fill_alignment(i, spec.fill, spec.align);
        parse_basic_numeric_flags(i, spec.alternate, spec.zero_pad, spec.sign);
        parse_basic_width(i, spec.width);
        if (!parse_basic_precision(i, spec.precision, spec.has_precision))
        {
            return format_error::invalid_format;
        }
        parse_basic_type(i, spec.type);

        _FORMAT_RET_IF((m_data[i] != C(_fmt_priv::closed_brace)), format_error::invalid_format);
        return format_error::none;
    }

private:

    const C* m_data;
    size_t m_size;
};

using parse_context = basic_parse_context<char>;

namespace _fmt_priv {

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

    basic_parse_context<C> ctx;
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
            tok.ctx.m_data = m_data + m_index;
            tok.ctx.m_size = 1;

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
            tok.ctx.m_data = m_data + m_index;
            tok.ctx.m_size = 1;

            advance(1);

            tok.type = token_type::replacement;
            return format_error::none;
        }

        // {:...}
        _FORMAT_RET_IF((current() != C(':')), format_error::invalid_format);
        advance(1);

        const size_t spec_start_index = m_index;
        size_t spec_end_index = 0;

        while (!empty())
        {
            if (current() == C(closed_brace))
            {
                spec_end_index = m_index;
                break;
            }

            advance(1);
        }

        _FORMAT_RET_IF(spec_end_index == 0, format_error::invalid_format);
        advance(1);

        tok.ctx.m_data = m_data + spec_start_index;
        tok.ctx.m_size = spec_end_index - spec_start_index;
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

        const size_t count = parse_uint_impl<C>(value, ptr, size);
        m_index += count;

        return (count != 0);
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
        const basic_parse_context<C>&,
        const void*);

template <typename T, typename C>
format_error invoke_formatter(
    output_buffer<C>& out,
    const basic_parse_context<C>& parse_ctx,
    const void* ptr)
{
    // preserves array-ness, e.g. const char[6]
    using U = typename std::remove_reference<T>::type;
    using DT = typename std::decay<T>::type;

    formatter<DT, C> f;

    const auto err = f.parse(parse_ctx);
    if (err != format_error::none)
    {
        return err;
    }

    basic_format_context<C> format_ctx{ out };

    // reinterpret as the *actual* stored object type, not the decayed one
    const U& ref = *static_cast<const U*>(ptr);
    return f.format(format_ctx, ref);
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

                const auto fmt_err = funcs[index](buffer, tok.ctx, values[index]);
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

} // namespace _fmt_priv

//==============================================================================

template <typename C, typename... Args, VX_REQUIRES(type_traits::is_char<C>::value)>
format_result format(
    C* out,
    size_t out_size,
    const C* fmt,
    size_t fmt_size,
    Args&&... args) noexcept
{
    return _fmt_priv::format_impl(
        out,
        out_size,
        fmt,
        fmt_size,
        std::forward<Args>(args)...);
}

//==============================================================================
// integer formatters
//==============================================================================

namespace _fmt_priv {

template <typename C>
format_error pad_number(
    // pointer to beginning of written data
    C* ptr,
    size_t& written,
    const C* prefix,
    const size_t prefix_size,
    const size_t remaining_size,
    const basic_integer_format_spec<C>& spec) noexcept
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

template <typename C>
format_error pad_pointer(
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
        C* first_digit = ptr;

        // open a gap of `right_shift` chars right after the sign; digits slide past it
        mem::move_range(first_digit + right_shift, first_digit, written);

        // [fill...][prefix][digits]
        C* insert = first_digit + right_shift - prefix_size;
        mem::copy_range(insert, prefix, prefix_size);
        mem::fill_range(ptr, padding.left, spec.fill);
    }

    mem::fill_range(ptr + right_shift + written, padding.right, spec.fill);

    written += right_shift + padding.right;
    return format_error::none;
}

} // namespace _fmt_priv

template <typename I, typename C>
struct formatter<I, C, VX_REQUIRES_TYPE(std::is_integral<I>::value && !type_traits::is_char<I>::value && !std::is_same<I, bool>::value)> : basic_integer_format_spec<C>
{
private:

    using base = basic_integer_format_spec<C>;

public:

    format_error parse(const basic_parse_context<C>& ctx)
    {
        return ctx.parse_basic_integer_spec(*this);
    }

    format_error format(
        basic_format_context<C>& ctx,
        const I& value) const noexcept
    {
        strconv::integer_to_string_format_options fmt{};
        fmt.force_sign = base::sign == sign_option::force;
        fmt.uppercase = base::type == C('X') || base::type == C('B');

        switch (base::type)
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
            if (base::sign == sign_option::space && value >= 0)
            {
                prefix[prefix_size++] = C(' ');
            }
            if (base::alternate && fmt.base != 10)
            {
                prefix[prefix_size++] = C('0');

                if (fmt.base != 8)
                {
                    prefix[prefix_size++] = base::type;
                }
            }
        }

        const size_t remaining = ctx.remaining();
        C* ptr = ctx.current();

        const auto res = strconv::to_string<I, C>(value, ptr, remaining, fmt);
        _SIZE_RET_IF((res.err == strconv::to_string_error::buffer_too_small), format_error::buffer_too_small);

        size_t written = res.count;

        const auto fmt_err = _fmt_priv::pad_number<C>(
            ptr,
            written,
            prefix,
            prefix_size,
            remaining,
            *this);

        if (fmt_err == format_error::none)
        {
            ctx.commit(written);
        }

        return fmt_err;
    }
};

template <typename C1, typename C2>
struct formatter<C1, C2, VX_REQUIRES_TYPE(type_traits::is_char<C1>::value)> : formatter<typename std::make_unsigned<C2>::type, C2>
{
private:

    using base = formatter<typename std::make_unsigned<C2>::type, C2>;

public:

    format_error format(
        basic_format_context<C2>& ctx,
        const C1& value) const noexcept
    {
        VX_STATIC_ASSERT_MSG(sizeof(C1) <= sizeof(C2), "Narrowing conversion not allowed");

        switch (base::type)
        {
            case C2('\0'):
            case C2('c'):
            {
                VX_IF_CONSTEXPR (std::is_same<C1, C2>::value)
                {
                    return ctx.write_padded(&value, 1, *this, alignment::left);
                }
                else
                {
                    const C2 dest_value = static_cast<C2>(value);
                    return ctx.write_padded(&dest_value, 1, *this, alignment::left);
                }
            }
            default:
            {
                return base::format(ctx, value);
            }
        }
    }
};

template <typename C>
struct formatter<bool, C> : formatter<unsigned int, C>
{
private:

    using base = formatter<unsigned int, C>;

public:

    format_error format(
        basic_format_context<C>& ctx,
        const bool& value) const noexcept
    {
        switch (base::type)
        {
            case C('\0'):
            case C('s'):
            {
                static constexpr C true_str[] = { C('t'), C('r'), C('u'), C('e') };
                static constexpr C false_str[] = { C('f'), C('a'), C('l'), C('s'), C('e') };

                if (value)
                {
                    return ctx.write_padded(
                        true_str,
                        mem::array_size(true_str),
                        *this,
                        alignment::left);
                }
                else
                {
                    return ctx.write_padded(
                        false_str,
                        mem::array_size(false_str),
                        *this,
                        alignment::left);
                }
            }
            case C('d'):
            {
                return base::format(ctx, value);
            }
            default:
            {
                _FORMAT_RET_IF(true, format_error::invalid_format);
            }
        }
    }
};

//==============================================================================
// string formatters
//==============================================================================

namespace _fmt_priv {

template <typename C>
struct string_formatter : basic_string_format_spec<C>
{
private:

    using base = basic_string_format_spec<C>;

public:

    format_error parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_string_spec(*this);
    }

    format_error format(
        basic_format_context<C>& ctx,
        const C* value,
        const size_t size) const noexcept
    {
        _FORMAT_RET_IF((base::type != 0 && base::type != C('s')), format_error::invalid_format);

        return ctx.write_padded(
            value,
            size,
            *this,
            alignment::left);
    }
};

} // namespace _fmt_priv

template <typename C>
struct formatter<const C*, C> : _fmt_priv::string_formatter<C>
{
private:

    using base = _fmt_priv::string_formatter<C>;

public:

    format_error format(
        basic_format_context<C>& ctx,
        const C* value) const noexcept
    {
        const size_t size = str::length(value);
        return base::format(
            ctx,
            value,
            size);
    }
};

template <typename C, size_t N>
struct formatter<C[N], C> : _fmt_priv::string_formatter<C>
{
private:

    using base = _fmt_priv::string_formatter<C>;

public:

    format_error format(
        basic_format_context<C>& ctx,
        const C (&value)[N]) const noexcept
    {
        return base::format(
            ctx,
            value,
            N,
            *this);
    }
};

//==============================================================================

template <typename S, typename C>
struct formatter<S, C, VX_REQUIRES_TYPE(str::is_string_of<S, C>::value)> : _fmt_priv::string_formatter<C>
{
private:

    using base = _fmt_priv::string_formatter<C>;

public:

    format_error format(
        basic_format_context<C>& ctx,
        const S& value) const noexcept
    {
        return base::format(
            ctx,
            value.data(),
            value.size());
    }
};

//==============================================================================
// float formatters
//==============================================================================

template <typename F, typename C>
struct formatter<F, C, VX_REQUIRES_TYPE(std::is_floating_point<F>::value)> : basic_float_format_spec<C>
{
private:

    using base = basic_float_format_spec<C>;

public:

    constexpr format_error parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_float_spec(*this);
    }

    format_error format(
        basic_format_context<C>& ctx,
        const F& value) const noexcept
    {
        strconv::float_to_string_format_options<C> fmt{};
        fmt.force_sign = base::sign == sign_option::force;
        fmt.force_exp_sign = base::alternate;

        if (base::has_precision)
        {
            fmt.precision = static_cast<decltype(fmt.precision)>(base::precision);
        }

        const bool is_uppercase = str::is_upper(base::type);
        const C lower_type = is_uppercase
            ? str::to_lower_ascii_unchecked(base::type)
            : base::type;

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
                fmt.format = static_cast<strconv::float_format>(lower_type);
                break;
            }
            default:
            {
                _FORMAT_RET_IF(true, format_error::invalid_format);
            }
        }

        fmt.uppercase = is_uppercase;

        const size_t remaining = ctx.remaining();
        C* ptr = ctx.current();

        const auto res = strconv::to_string<F, C>(value, ptr, remaining, fmt);
        _SIZE_RET_IF((res.err == strconv::to_string_error::buffer_too_small), format_error::buffer_too_small);
        _FORMAT_RET_IF((res.err != strconv::to_string_error::none), format_error::invalid_format);

        C prefix[3];
        size_t prefix_size = 0;
        {
            if (base::sign == sign_option::space && !signbit(value))
            {
                prefix[prefix_size++] = C(' ');
            }
            if (fmt.format == strconv::float_format::hex)
            {
                prefix[prefix_size++] = C('0');
                prefix[prefix_size++] = fmt.uppercase ? C('X') : C('x');
            }
        }

        size_t written = res.count;

        const auto fmt_err = _fmt_priv::pad_number<C>(
            ptr,
            written,
            prefix,
            prefix_size,
            remaining,
            *this);

        if (fmt_err == format_error::none)
        {
            ctx.commit(written);
        }

        return fmt_err;
    }
};

//==============================================================================
// pointer formatters
//==============================================================================

template <typename C>
struct formatter<const void*, C> : basic_format_spec<C>
{
private:

    using base = basic_format_spec<C>;

public:

    constexpr format_error parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_spec(*this);
    }

    format_error format(
        basic_format_context<C>& ctx,
        const void* value) const noexcept
    {
        _FORMAT_RET_IF((base::type != 0 && base::type != C('p')), format_error::invalid_format);

        strconv::integer_to_string_format_options fmt{};
        fmt.base = 16;

        const uintptr_t addr = reinterpret_cast<uintptr_t>(value);

        const size_t remaining = ctx.remaining();
        C* ptr = ctx.current();

        const auto res = strconv::to_string<uintptr_t, C>(addr, ptr, remaining, fmt);
        _SIZE_RET_IF((res.err == strconv::to_string_error::buffer_too_small), format_error::buffer_too_small);

        constexpr C prefix[] = { C('0'), C('x') };

        size_t written = res.count;
        const auto fmt_err = _fmt_priv::pad_pointer<C>(
            ptr,
            written,
            prefix,
            mem::array_size(prefix),
            remaining,
            *this);

        if (fmt_err == format_error::none)
        {
            ctx.commit(written);
        }

        return fmt_err;
    }
};

template <typename C>
struct formatter<void*, C> : formatter<const void*, C>
{
private:

    using base = formatter<const void*, C>;

public:

    format_error format(
        basic_format_context<C>& ctx,
        void* value) const noexcept
    {
        return base::format(ctx, value);
    }
};

template <typename T, typename C>
struct formatter<T*, C, VX_REQUIRES_TYPE(!type_traits::is_char<T>::value)> : formatter<const void*, C>
{
private:

    using base = formatter<const void*, C>;

public:

    format_error format(
        basic_format_context<C>& ctx,
        T* value) const noexcept
    {
        return base::format(ctx, value);
    }
};

template <typename C>
struct formatter<std::nullptr_t, C> : formatter<const void*, C>
{
private:

    using base = formatter<const void*, C>;

public:

    format_error format(
        basic_format_context<C>& ctx,
        std::nullptr_t) const noexcept
    {
        return base::format(ctx, nullptr);
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
