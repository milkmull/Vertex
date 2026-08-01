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
// format specs
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

template <typename T, typename C, typename Enable = void>
struct formatter
{
    VX_STATIC_ASSERT_MSG((!std::is_same<T, T>::value), "No formatter available");
};

//==============================================================================
// format context
//==============================================================================

template <typename C>
class basic_format_context;

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
    C* ptr;
    size_t remaining;

    constexpr bool reserve(size_t n) noexcept
    {
        _SIZE_RET_IF((n > remaining), false);
        return true;
    }

    constexpr bool push(C c) noexcept
    {
        _SIZE_RET_IF((remaining == 0), false);
        *ptr++ = c;
        --remaining;
        return true;
    }

    constexpr bool append(const C* s, size_t n) noexcept
    {
        _SIZE_RET_IF((n > remaining), false);
        ptr = mem::copy_range(ptr, s, n);
        remaining -= n;
        return true;
    }

    constexpr bool fill(const C c, size_t n) noexcept
    {
        _SIZE_RET_IF((n > remaining), false);
        ptr = mem::fill_range(ptr, c, n);
        remaining -= n;
        return true;
    }

    constexpr format_error write_padded(
        const basic_format_spec<C>& spec,
        const C* data,
        size_t count,
        alignment default_align) noexcept
    {
        const size_t width = (spec.width > count) ? spec.width : count;
        _SIZE_RET_IF((!reserve(width)), format_error::buffer_too_small);

        const size_t total_padding = width - count;
        const alignment align = (spec.align == alignment::none) ? default_align : spec.align;
        const auto padding = align_padding(total_padding, align);

        mem::fill_range(ptr, padding.left, spec.fill);
        ptr += padding.left;

        mem::copy_range(ptr, data, count);
        ptr += count;

        mem::fill_range(ptr, padding.right, spec.fill);
        ptr += padding.right;

        remaining -= width;
        return format_error::none;
    }

    constexpr format_error pad_written_value(
        const basic_format_spec<C>& spec,
        size_t written,
        alignment default_align) noexcept
    {
        const size_t width = (spec.width > written) ? spec.width : written;
        const size_t total_padding = width - written;
        _SIZE_RET_IF((!reserve(total_padding)), format_error::buffer_too_small);

        const alignment align = (spec.align == alignment::none) ? default_align : spec.align;
        const auto padding = align_padding(total_padding, align);

        C* field_start = ptr - written;
        C* left_edge = field_start + padding.left;

        mem::move_range(left_edge, field_start, written);
        mem::fill_range(field_start, padding.left, spec.fill);
        mem::fill_range(left_edge + written, padding.right, spec.fill);

        ptr += total_padding;
        remaining -= total_padding;

        return format_error::none;
    }

    constexpr format_error pad_written_number(
        const basic_integer_format_spec<C>& spec,
        size_t written,
        const str::basic_string_view<C> prefix) noexcept
    {
        const C* prefix_data = prefix.data();
        const size_t prefix_size = prefix.size();

        const size_t total_size = written + prefix_size;
        const size_t width = (spec.width > total_size) ? spec.width : total_size;
        _SIZE_RET_IF((!reserve(width)), format_error::buffer_too_small);

        const size_t total_padding = width - total_size;
        const auto padding = align_padding(total_padding, spec.align);

        const size_t right_shift = padding.left + prefix_size;

        if (right_shift)
        {
            const bool has_sign = *ptr == C('+') || *ptr == C('-');
            const size_t sign_len = static_cast<size_t>(has_sign);

            C* first_digit = ptr + sign_len;

            // open a gap of `right_shift` chars right after the sign; digits slide past it
            mem::move_range(first_digit + right_shift, first_digit, written - sign_len);

            if (spec.zero_pad)
            {
                // [sign][space?][prefix?][zeros...][digits]
                mem::copy_range(first_digit, prefix_data, prefix_size);
                first_digit += prefix_size;
                mem::fill_range(first_digit, padding.left, C('0'));
            }
            else
            {
                // [fill...][sign][space?][prefix?][digits]
                C* insert = first_digit + right_shift - prefix_size;
                mem::copy_range(insert, prefix_data, prefix_size);

                if (has_sign)
                {
                    *(--insert) = *ptr;
                }

                mem::fill_range(ptr, padding.left, spec.fill);
            }
        }

        mem::fill_range(ptr + right_shift + written, padding.right, spec.fill);
        written += total_padding + prefix_size;

        ptr += written;
        remaining -= written;

        return format_error::none;
    }

    constexpr format_error pad_written_pointer(
        const basic_format_spec<C>& spec,
        size_t written) noexcept
    {
        const C prefix[] = { C('0'), C('x') };
        constexpr size_t prefix_size = mem::array_size(prefix);

        const size_t total_size = written + prefix_size;
        const size_t width = (spec.width > total_size) ? spec.width : total_size;
        _SIZE_RET_IF((!reserve(width)), format_error::buffer_too_small);

        const size_t total_padding = width - total_size;
        const auto padding = align_padding(total_padding, spec.align);

        const size_t right_shift = padding.left + prefix_size;
        C* left_edge = ptr + right_shift;

        // open a gap for the prefix and left padding
        mem::move_range(left_edge, ptr, written);

        // copy the prefix
        C* prefix_start = left_edge - prefix_size;
        mem::copy_range(prefix_start, prefix, prefix_size);

        // pad the sides
        mem::fill_range(ptr, padding.left, spec.fill);
        mem::fill_range(left_edge + written, padding.right, spec.fill);
        written += total_padding + prefix_size;

        ptr += written;
        remaining -= written;

        return format_error::none;
    }
};

template <typename C>
struct context_creator;

} // namespace _fmt_priv

template <typename C>
class basic_format_context
{
private:

    friend _fmt_priv::context_creator<C>;

    constexpr explicit basic_format_context(_fmt_priv::output_buffer<C>& out) noexcept
        : m_out(out)
    {}

public:

    constexpr const C* ptr() const noexcept
    {
        return m_out.ptr;
    }

    constexpr C* ptr() noexcept
    {
        return m_out.ptr;
    }

    constexpr size_t remaining() const noexcept
    {
        return m_out.remaining;
    }

    constexpr bool reserve(size_t n) noexcept
    {
        return m_out.reserve(n);
    }

    constexpr bool push(C c) noexcept
    {
        return m_out.push(c);
    }

    constexpr bool append(const C* data, size_t count) noexcept
    {
        return m_out.append(data, count);
    }

    constexpr bool fill(C c, size_t count) noexcept
    {
        return m_out.fill(c, count);
    }

    constexpr format_error write_padded(
        const basic_integer_format_spec<C>& spec,
        const C* data,
        size_t count,
        alignment default_align = alignment::left) noexcept
    {
        return m_out.write_padded(spec, data, count, default_align);
    }

    constexpr format_error write_padded(
        const basic_string_format_spec<C>& spec,
        const C* data,
        size_t count,
        alignment default_align = alignment::left) noexcept
    {
        if (spec.has_precision && count > spec.precision)
        {
            count = spec.precision;
        }

        return m_out.write_padded(spec, data, count, default_align);
    }

    constexpr format_error pad_written_value(
        const basic_format_spec<C>& spec,
        size_t written,
        alignment default_align = alignment::left) noexcept
    {
        return m_out.pad_written_value(spec, written, default_align);
    }

    constexpr format_error pad_written_number(
        const basic_integer_format_spec<C>& spec,
        size_t written,
        const str::basic_string_view<C> prefix = {}) noexcept
    {
        return m_out.pad_written_number(spec, written, prefix);
    }

    constexpr format_error pad_written_pointer(
        const basic_format_spec<C>& spec,
        size_t written) noexcept
    {
        return m_out.pad_written_pointer(spec, written);
    }

private:

    _fmt_priv::output_buffer<C>& m_out;
};

using format_context = basic_format_context<char>;

//==============================================================================
// parsing
//==============================================================================

namespace _fmt_priv {

enum : char
{
    open_brace = '{',
    closed_brace = '}'
};

template <typename C>
constexpr size_t parse_uint_impl(size_t& value, const C* data, const size_t size) noexcept
{
    using U = typename std::remove_reference<decltype(value)>::type;
    size_t i = 0;

    constexpr U uint_max = static_cast<U>(-1);
    constexpr U risky_value = static_cast<U>(uint_max / 10);
    constexpr U max_digit = static_cast<U>(uint_max % 10);

    value = 0;

    while (i < size)
    {
        const C c = data[i];
        if (!(static_cast<C>('0') <= c && c <= static_cast<C>('9')))
        {
            // invalid character, return what we have
            break;
        }

        const unsigned char digit = static_cast<unsigned char>(c - static_cast<C>('0'));
        if (value < risky_value || (value == risky_value && digit <= max_digit))
        {
            value = static_cast<U>(value * 10 + digit);
        }
        else
        {
            // value overflowed, invalid format
            return 0;
        }

        ++i;
    }

    return i;
}

} // namespace _fmt_priv

template <typename C>
class basic_parse_context
{
private:

    friend _fmt_priv::context_creator<C>;

    constexpr explicit basic_parse_context(const C* ptr, const size_t size) noexcept
        : m_data(ptr)
        , m_size(size)
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

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================

namespace _fmt_priv {

template <typename C>
struct context_creator
{
    static constexpr auto create_parse_context(const C* ptr, const size_t size) noexcept
    {
        return basic_parse_context<C>{ ptr, size };
    }

    static constexpr auto create_format_context(output_buffer<C>& out) noexcept
    {
        return basic_format_context<C>{ out };
    }
};

//==============================================================================

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

    const C* first = nullptr;
    const C* last = nullptr;

    bool has_index = false;
    size_t index = 0;

    constexpr size_t calculate_size() const noexcept
    {
        return static_cast<size_t>(last - first);
    }
};

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

    constexpr basic_format_parser(const C* data, size_t size) noexcept
        : m_ptr(data)
        , m_remaning(size)
        , m_mode(format_mode::default_)
    {}

    constexpr bool update_mode(format_mode mode) noexcept
    {
        if (m_mode == format_mode::default_)
        {
            m_mode = mode;
            return true;
        }

        return m_mode == mode;
    }

    constexpr bool next(basic_format_token<C>& tok)
    {
        tok.has_index = false;

        if (empty())
        {
            tok.type = token_type::end;
            return true;
        }

        // escaped '}': "}}" -> "}"
        if (current() == C(closed_brace))
        {
            tok.type = token_type::escaped;
            tok.first = m_ptr;

            advance(1);
            _FORMAT_RET_IF((empty() || current() != C(closed_brace)), false);
            advance(1);

            return true;
        }

        // literal
        if (current() != C(open_brace))
        {
            tok.type = token_type::literal;
            tok.first = m_ptr;

            while (!empty())
            {
                if (current() == C(open_brace) || current() == C(closed_brace))
                {
                    break;
                }

                advance(1);
            }

            tok.last = m_ptr;
            return true;
        }

        // consume '{'
        advance(1);
        _FORMAT_RET_IF(empty(), false);

        // escaped '{': "{{" -> "{"
        if (current() == C(open_brace))
        {
            tok.type = token_type::escaped;
            tok.first = m_ptr;

            advance(1);
            return true;
        }

        // {}
        if (current() == C(closed_brace))
        {
            tok.type = token_type::replacement;
            tok.first = m_ptr;

            advance(1);

            tok.last = m_ptr;
            return true;
        }

        // optional index
        if (str::is_digit(current()))
        {
            if (!parse_uint(tok.index))
            {
                return false;
            }

            tok.has_index = true;
        }

        // {0} or {0:...}
        if (current() == C(closed_brace))
        {
            tok.type = token_type::replacement;
            tok.first = m_ptr;

            advance(1);

            tok.last = m_ptr;
            return true;
        }

        // {:...}
        _FORMAT_RET_IF((current() != C(':')), false);
        advance(1);

        tok.type = token_type::replacement;
        tok.first = m_ptr;
        tok.last = nullptr;

        while (!empty())
        {
            if (current() == C(closed_brace))
            {
                tok.last = m_ptr;
                break;
            }

            advance(1);
        }

        _FORMAT_RET_IF(tok.last == nullptr, false);
        advance(1);

        return true;
    }

private:

    constexpr bool empty() const noexcept
    {
        return m_remaning == 0;
    }

    constexpr bool has_size(size_t n) const noexcept
    {
        return m_remaning > n;
    }

    constexpr C peek(size_t off) const noexcept
    {
        return m_ptr[off];
    }

    constexpr C current() const noexcept
    {
        return *m_ptr;
    }

    constexpr void advance(size_t n) noexcept
    {
        m_ptr += n;
        m_remaning -= n;
    }

    constexpr bool parse_uint(size_t& value) noexcept
    {
        const size_t count = parse_uint_impl<C>(value, m_ptr, m_remaning);
        advance(count);
        return (count != 0);
    }

private:

    const C* m_ptr;
    size_t m_remaning;
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
constexpr format_error invoke_formatter(
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

    auto format_ctx = context_creator<C>::create_format_context(out);

    // reinterpret as the *actual* stored object type, not the decayed one
    const U& ref = *static_cast<const U*>(ptr);
    return f.format(format_ctx, ref);
}

template <typename T, typename C>
constexpr format_error invoke_formatter_consteval(
    output_buffer<C>& out,
    const basic_parse_context<C>& parse_ctx,
    const T& value) noexcept
{
    using U = std::remove_reference_t<T>;
    using DT = std::remove_cv_t<U>;

    formatter<DT, C> f;

    const auto err = f.parse(parse_ctx);
    if (err != format_error::none)
    {
        return err;
    }

    auto format_ctx = context_creator<C>::create_format_context(out);
    return f.format(format_ctx, value);
}

//==============================================================================

template <typename C, typename... Args>
constexpr format_result format_impl(
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
    output_buffer<C> buffer{ out, out_size };

    size_t next_arg = 0;
    basic_format_token<C> tok;

    while (true)
    {
        if (!parser.next(tok))
        {
            return { format_error::invalid_format, 0 };
        }

        switch (tok.type)
        {
            case token_type::literal:
            {
                if (!buffer.append(tok.first, tok.calculate_size()))
                {
                    return { format_error::buffer_too_small, 0 };
                }

                break;
            }
            case token_type::escaped:
            {
                if (!buffer.push(*tok.first))
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

                auto parse_ctx = context_creator<C>::create_parse_context(tok.first, tok.calculate_size());
                const auto fmt_err = funcs[index](buffer, parse_ctx, values[index]);
                if (fmt_err != format_error::none)
                {
                    return { fmt_err, 0 };
                }

                break;
            }
            case token_type::end:
            {
                const size_t count = out_size - buffer.remaining;
                return { format_error::none, count };
            }
        }
    }
}

template <size_t I = 0, typename Tuple, typename C>
constexpr format_error format_arg(
    output_buffer<C>& buffer,
    const basic_format_token<C>& tok,
    Tuple&& tuple,
    size_t index)
{
    VX_IF_CONSTEXPR (I < std::tuple_size<typename std::remove_reference<Tuple>::type>::value)
    {
        if (I == index)
        {
            auto parse_ctx =
                context_creator<C>::create_parse_context(
                    tok.first,
                    tok.calculate_size());

            return invoke_formatter_consteval(
                buffer,
                parse_ctx,
                std::get<I>(std::forward<Tuple>(tuple)));
        }

        return format_arg<I + 1>(
            buffer,
            tok,
            std::forward<Tuple>(tuple),
            index);
    }
    else
    {
        return format_error::invalid_argument;
    }
}

template <typename C, typename... Args>
constexpr format_result format_impl_consteval(
    C* out,
    size_t out_size,
    const C* fmt,
    size_t fmt_size,
    Args&&... args) noexcept
{
    constexpr size_t argc = sizeof...(Args);

    auto values = std::forward_as_tuple(std::forward<Args>(args)...);

    basic_format_parser<C> parser(fmt, fmt_size);
    output_buffer<C> buffer{ out, out_size };

    size_t next_arg = 0;
    basic_format_token<C> tok;

    while (true)
    {
        if (!parser.next(tok))
        {
            return { format_error::invalid_format, 0 };
        }

        switch (tok.type)
        {
            case token_type::literal:
            {
                if (!buffer.append(tok.first, tok.calculate_size()))
                {
                    return { format_error::buffer_too_small, 0 };
                }

                break;
            }
            case token_type::escaped:
            {
                if (!buffer.push(*tok.first))
                {
                    return { format_error::buffer_too_small, 0 };
                }

                break;
            }
            case token_type::replacement:
            {
                _FORMAT_RET_IF(
                    (!parser.update_mode(
                        tok.has_index
                            ? format_mode::manual
                            : format_mode::auto_)),
                    (format_result{ format_error::mode_mismatch, 0 }));

                const size_t index = tok.has_index
                    ? tok.index
                    : next_arg++;

                _FORMAT_RET_IF(
                    (index >= argc),
                    (format_result{ format_error::invalid_argument, 0 }));

                const auto fmt_err = format_arg(
                    buffer,
                    tok,
                    values,
                    index);

                if (fmt_err != format_error::none)
                {
                    return { fmt_err, 0 };
                }

                break;
            }
            case token_type::end:
            {
                const size_t count = out_size - buffer.remaining;
                return { format_error::none, count };
            }
        }
    }
}

} // namespace _fmt_priv

//==============================================================================
// integer formatters
//==============================================================================

template <typename I, typename C>
struct formatter<I, C, VX_REQUIRES_TYPE(std::is_integral<I>::value && !type_traits::is_char<I>::value && !std::is_same<I, bool>::value)> : basic_integer_format_spec<C>
{
private:

    using base = basic_integer_format_spec<C>;

public:

    constexpr format_error parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_integer_spec(*this);
    }

    constexpr format_error format(
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

        const size_t remaining = ctx.remaining();
        C* ptr = ctx.ptr();

        const auto res = strconv::to_string<I, C>(value, ptr, remaining, fmt);
        _SIZE_RET_IF((res.err == strconv::to_string_error::buffer_too_small), format_error::buffer_too_small);

        C prefix[3]{};
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

        const str::basic_string_view<C> prefix_view{ prefix, prefix_size };
        return ctx.pad_written_number(*this, res.count, prefix_view);
    }
};

//==============================================================================

template <typename C1, typename C2>
struct formatter<C1, C2, VX_REQUIRES_TYPE(type_traits::is_char<C1>::value)> : formatter<typename std::make_unsigned<C2>::type, C2>
{
private:

    using base = formatter<typename std::make_unsigned<C2>::type, C2>;

public:

    constexpr format_error format(
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
                    return ctx.write_padded(*this, &value, 1, alignment::left);
                }
                else
                {
                    const C2 dest_value = static_cast<C2>(value);
                    return ctx.write_padded(*this, &dest_value, 1, alignment::left);
                }
            }
            default:
            {
                return base::format(ctx, value);
            }
        }
    }
};

//==============================================================================

template <typename C>
struct formatter<bool, C> : formatter<unsigned int, C>
{
private:

    using base = formatter<unsigned int, C>;

    static constexpr C true_str[] = { C('t'), C('r'), C('u'), C('e') };
    static constexpr C false_str[] = { C('f'), C('a'), C('l'), C('s'), C('e') };

public:

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const bool& value) const noexcept
    {
        switch (base::type)
        {
            case C('\0'):
            case C('s'):
            {
                if (value)
                {
                    return ctx.write_padded(
                        *this,
                        true_str,
                        mem::array_size(true_str),
                        alignment::left);
                }
                else
                {
                    return ctx.write_padded(
                        *this,
                        false_str,
                        mem::array_size(false_str),
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

    constexpr format_error parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_string_spec(*this);
    }

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const C* value,
        const size_t size) const noexcept
    {
        _FORMAT_RET_IF((base::type != 0 && base::type != C('s')), format_error::invalid_format);

        return ctx.write_padded(
            *this,
            value,
            size);
    }
};

} // namespace _fmt_priv

//==============================================================================

template <typename C>
struct formatter<const C*, C> : _fmt_priv::string_formatter<C>
{
private:

    using base = _fmt_priv::string_formatter<C>;

public:

    constexpr format_error format(
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

//==============================================================================

template <typename C, size_t N>
struct formatter<const C[N], C> : _fmt_priv::string_formatter<C>
{
private:

    using base = _fmt_priv::string_formatter<C>;

public:

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const C (&value)[N]) const noexcept
    {
        return base::format(
            ctx,
            value,
            N);
    }
};

template <typename C, size_t N>
struct formatter<C[N], C> : formatter<const C[N], C>
{};

//==============================================================================

template <typename S, typename C>
struct formatter<S, C, VX_REQUIRES_TYPE(str::is_string_of<S, C>::value)> : _fmt_priv::string_formatter<C>
{
private:

    using base = _fmt_priv::string_formatter<C>;

public:

    constexpr format_error format(
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

    constexpr format_error format(
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
        C* ptr = ctx.ptr();

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

        const str::basic_string_view<C> prefix_view{ prefix, prefix_size };
        return ctx.pad_written_number(*this, res.count, prefix_view);
    }
};

//==============================================================================
// pointer formatters
//==============================================================================

template <typename C>
struct formatter<void*, C> : basic_format_spec<C>
{
private:

    using base = basic_format_spec<C>;

public:

    constexpr format_error parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_spec(*this);
    }

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const void* value) const noexcept
    {
        _FORMAT_RET_IF((base::type != 0 && base::type != C('p')), format_error::invalid_format);

        strconv::integer_to_string_format_options fmt{};
        fmt.base = 16;

        const uintptr_t addr = reinterpret_cast<uintptr_t>(value);

        const size_t remaining = ctx.remaining();
        C* ptr = ctx.ptr();

        const auto res = strconv::to_string<uintptr_t, C>(addr, ptr, remaining, fmt);
        _SIZE_RET_IF((res.err == strconv::to_string_error::buffer_too_small), format_error::buffer_too_small);

        return ctx.pad_written_pointer(*this, res.count);
    }
};

//==============================================================================

template <typename T, typename C>
struct formatter<T*, C, VX_REQUIRES_TYPE(!type_traits::is_char<T>::value)> : formatter<const void*, C>
{};

//==============================================================================

template <typename C>
struct formatter<std::nullptr_t, C> : formatter<const void*, C>
{};

//==============================================================================
// format
//==============================================================================

template <typename C, typename... Args, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr format_result format(
    C* out,
    size_t out_size,
    const C* fmt,
    size_t fmt_size,
    Args&&... args) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return _fmt_priv::format_impl_consteval(
            out,
            out_size,
            fmt,
            fmt_size,
            std::forward<Args>(args)...);
    }
    else
    {
        return _fmt_priv::format_impl(
            out,
            out_size,
            fmt,
            fmt_size,
            std::forward<Args>(args)...);
    }
}

template <typename FMT, typename S, typename... Args, VX_REQUIRES(
    str::is_string_like<FMT>::value &&
    str::is_mutable_string_like<S>::value &&
    str::is_string_compatible<FMT, S>::value)>
format_result format_string(
    const FMT& fmt,
    S& out,
    Args&&... args)
{
    if (out.empty())
    {
        out.resize(64);
    }

    for (;;)
    {
        const auto res = format(
            out.data(),
            out.size(),
            fmt.data(),
            fmt.size(),
            std::forward<Args>(args)...);

        if (res.err == format_error::none)
        {
            out.resize(res.count);
            return res;
        }

        if (res.err != format_error::buffer_too_small)
        {
            return res;
        }

        // Grow and retry
        VX_ASSERT(out.size());
        out.resize(out.size() * 2);
    }
}

template <typename C, typename FMT, typename S, typename... Args, VX_REQUIRES(
    type_traits::is_char<C>::value &&
    str::is_string_of<FMT, C>::value &&
    str::is_mutable_string_like<S>::value &&
    str::is_string_compatible<FMT, S>::value)>
format_result format_string(
    const C* fmt,
    S& out,
    Args&&... args)
{
    return format_string(
        str::basic_string_view<C>(fmt),
        out,
        std::forward<Args>(args)...);
}

template <typename S, typename... Args, VX_REQUIRES(str::is_string_like<S>::value)>
auto format(
    const S& fmt,
    Args&&... args)
{
    using C = typename S::value_type;
    str::basic_string<C> out;

    format_string(
        fmt,
        out,
        std::forward<Args>(args)...);

    return out;
}

template <typename C, typename... Args, VX_REQUIRES(type_traits::is_char<C>::value)>
str::basic_string<C> format(
    const C* fmt,
    Args&&... args)
{
    str::basic_string<C> out;

    format_string(
        str::basic_string_view<C>(fmt),
        out,
        std::forward<Args>(args)...);

    return out;
}

//==============================================================================
//==============================================================================

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
