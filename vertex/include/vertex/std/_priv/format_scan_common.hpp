#pragma once

#include "vertex/std/array.hpp"
#include "vertex/std/string_convert.hpp"

namespace vx {
namespace fmt {

// #define VX_FORMAT_DISABLE_FORMAT_STRING_CHECKS
// #define VX_FORMAT_DISABLE_OUTPUT_SIZE_CHECKS

#define _VX_FAIL_IF(cond, ret) \
    do \
    { \
        if ((cond)) \
        { \
            return (ret); \
        } \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#if !defined(VX_FORMAT_DISABLE_FORMAT_STRING_CHECKS)
    #define _FORMAT_RET_IF(cond, ret) _VX_FAIL_IF((cond), (ret))
#else
    #define _FORMAT_RET_IF(cond, ret) VX_ASSERT(!(cond))
#endif

//==============================================================================
// format options
//==============================================================================

enum class alignment : char
{
    none = 0,
    left = '<',
    right = '>',
    center = '^'
};

enum class sign_option : char
{
    none = 0,
    force = '+',
    space = ' '
};

enum class whitespace_mode
{
    greedy,
    bounded,
};

//==============================================================================
// format specs
//==============================================================================

template <typename C>
struct basic_format_spec
{
    C fill = C(' ');
    alignment align = alignment::none;
    size_t width = 0;
    C type = C{};
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
// scan specs
//==============================================================================

template <typename C>
struct basic_scan_spec
{
    C fill = C(' ');
    alignment align = alignment::none;
    alignment default_align = alignment::none;
    size_t width = 0;
};

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

template <typename C>
struct parse_context_creator;

} // namespace _fmt_priv

template <typename C>
class basic_parse_context
{
private:

    friend _fmt_priv::parse_context_creator<C>;

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
        return !((m_data[i] == C('+')) || (m_data[i] == C(' ')) || (m_data[i] == C('#')) || (m_data[i] == C('0')));
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

    constexpr bool parse_basic_width(size_t& i, size_t& width) const noexcept
    {
        if (i < m_size)
        {
            size_t value = 0;
            if (parse_uint(value, i))
            {
                width = value;
                return width != 0;
            }
        }

        return true;
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

    constexpr bool parse_basic_spec(basic_format_spec<C>& spec) const noexcept
    {
        size_t i = 0;

        parse_basic_fill_alignment(i, spec.fill, spec.align);
        _FORMAT_RET_IF(!check_basic_numeric_flags(i), false);
        _FORMAT_RET_IF(!parse_basic_width(i, spec.width), false);
        parse_basic_type(i, spec.type);

        _FORMAT_RET_IF(m_data[i] != C(_fmt_priv::closed_brace), false);
        return true;
    }

    constexpr bool parse_basic_string_spec(basic_string_format_spec<C>& spec) const noexcept
    {
        size_t i = 0;

        parse_basic_fill_alignment(i, spec.fill, spec.align);
        _FORMAT_RET_IF(!parse_basic_width(i, spec.width), false);
        _FORMAT_RET_IF(!parse_basic_precision(i, spec.precision, spec.has_precision), false);
        parse_basic_type(i, spec.type);

        _FORMAT_RET_IF(m_data[i] != C(_fmt_priv::closed_brace), false);
        return true;
    }

    constexpr bool parse_basic_integer_spec(basic_integer_format_spec<C>& spec) const noexcept
    {
        size_t i = 0;

        parse_basic_fill_alignment(i, spec.fill, spec.align);
        parse_basic_numeric_flags(i, spec.alternate, spec.zero_pad, spec.sign);
        _FORMAT_RET_IF(!parse_basic_width(i, spec.width), false);
        parse_basic_type(i, spec.type);

        _FORMAT_RET_IF(m_data[i] != C(_fmt_priv::closed_brace), false);
        return true;
    }

    constexpr bool parse_basic_float_spec(basic_float_format_spec<C>& spec) const noexcept
    {
        size_t i = 0;

        parse_basic_fill_alignment(i, spec.fill, spec.align);
        parse_basic_numeric_flags(i, spec.alternate, spec.zero_pad, spec.sign);
        _FORMAT_RET_IF(!parse_basic_width(i, spec.width), false);
        _FORMAT_RET_IF(!parse_basic_precision(i, spec.precision, spec.has_precision), false);
        parse_basic_type(i, spec.type);

        _FORMAT_RET_IF(m_data[i] != C(_fmt_priv::closed_brace), false);
        return true;
    }

private:

    const C* m_data;
    size_t m_size;
};

using parse_context = basic_parse_context<char>;

//==============================================================================

namespace _fmt_priv {

template <typename C>
struct parse_context_creator
{
    static constexpr auto create(const C* ptr, const size_t size) noexcept
    {
        return basic_parse_context<C>{ ptr, size };
    }
};

//==============================================================================

enum class token_type
{
    literal,
    whitespace,
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

enum class index_mode
{
    default_,
    auto_,
    manual
};

template <typename C>
class basic_format_parser
{
public:

    constexpr basic_format_parser(const C* data, size_t size, const whitespace_mode ws) noexcept
        : m_ptr(data)
        , m_remaning(size)
        , m_mode(index_mode::default_)
        , m_whitespace_mode(ws)
    {}

    constexpr bool update_mode(index_mode mode) noexcept
    {
        if (m_mode == index_mode::default_)
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
            tok.first = m_ptr;

            if ((m_whitespace_mode == whitespace_mode::greedy) && str::is_space(current()))
            {
                tok.type = token_type::whitespace;
                while (!empty() && str::is_space(current()))
                {
                    advance(1);
                }
            }
            else
            {
                tok.type = token_type::literal;
                while (!empty() && (current() != C(open_brace) && current() != C(closed_brace)))
                {
                    advance(1);
                }
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
    index_mode m_mode;
    whitespace_mode m_whitespace_mode;
};

template<typename C>
inline constexpr int parse_integer_base(const C c) noexcept
{
    switch (c)
    {
        case C('\0'):
        case C('d'):
        {
            return 10;
        }
        case C('x'):
        case C('X'):
        {
            return 16;
        }
        case C('o'):
        {
            return 8;
        }
        case C('b'):
        case C('B'):
        {
            return 2;
        }
        default:
        {
            return 0;
        }
    }
}

template <typename C>
struct boolean_strings
{
    static constexpr C true_str[] = { C('t'), C('r'), C('u'), C('e') };
    static constexpr C false_str[] = { C('f'), C('a'), C('l'), C('s'), C('e') };
};

} // namespace _fmt_priv

} // namespace fmt
} // namespace vx
