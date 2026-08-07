#pragma once

#include "vertex/std/_priv/format_scan_common.hpp"

namespace vx {
namespace fmt {

//==============================================================================
// result types
//==============================================================================

enum class scan_error
{
    none,
    end_of_input,
    invalid_format,
    invalid_argument,
    invalid_scaned_field,
    result_out_of_range,
    index_mode_mismatch
};

struct scan_result
{
    scan_error err;
    size_t count;
};

//==============================================================================
// scanner
//==============================================================================

template <typename T, typename C, typename Enable = void>
struct scanner
{
    VX_STATIC_ASSERT_MSG((!std::is_same<T, T>::value), "No scanner available");
};

//==============================================================================
// scan context
//==============================================================================

namespace _fmt_priv {

template <typename C>
struct input_reader
{
    using interator = const C*;

    const C* ptr;
    size_t remaining;

    constexpr size_t consumed(const size_t starting_size) const noexcept
    {
        return starting_size - remaining;
    }

    constexpr bool consume(size_t count) noexcept
    {
        const size_t stop = (count < remaining) ? count : remaining;
        ptr += stop;
        remaining -= stop;
        return (stop == count);
    }

    constexpr bool consume_literal(const C* data, size_t count) noexcept
    {
        const size_t n = remaining < count ? remaining : count;

        for (size_t i = 0; i < n; ++i)
        {
            if (ptr[i] != data[i])
            {
                return false;
            }
        }

        ptr += n;
        remaining -= n;

        return n == count;
    }

    constexpr bool consume_characters(size_t count, const C c) noexcept
    {
        const size_t n = remaining < count ? remaining : count;

        for (size_t i = 0; i < n; ++i)
        {
            if (ptr[i] != c)
            {
                return false;
            }
        }

        ptr += n;
        remaining -= n;

        return n == count;
    }

    constexpr size_t consume_whitespace() noexcept
    {
        size_t i = 0;

        while (i < remaining && str::is_space(ptr[i]))
        {
            ++i;
        }

        ptr += i;
        remaining -= i;

        return i;
    }

    constexpr size_t consume_next(const C c) noexcept
    {
        size_t i = 0;

        while (i < remaining && ptr[i] == c)
        {
            ++i;
        }

        ptr += i;
        remaining -= i;

        return i;
    }

    constexpr size_t consume_next_n(const C c, const size_t n) noexcept
    {
        size_t i = 0;
        const size_t stop = (n < remaining) ? n : remaining;
        while (i < stop && ptr[i] == c)
        {
            ++i;
        }
        ptr += i;
        remaining -= i;
        return i;
    }
};

template <typename C>
struct scan_context_creator;

} // namespace _fmt_priv

//==============================================================================

template <typename C>
class basic_scan_context
{
private:

    friend _fmt_priv::scan_context_creator<C>;

    constexpr explicit basic_scan_context(_fmt_priv::input_reader<C>& in) noexcept
        : m_in(in)
    {}

public:

    constexpr size_t consumed(const size_t starting_size) const noexcept
    {
        return m_in.consumed(starting_size);
    }

    constexpr const C* ptr() const noexcept
    {
        return m_in.ptr;
    }

    constexpr const C* ptr() noexcept
    {
        return m_in.ptr;
    }

    constexpr size_t remaining() const noexcept
    {
        return m_in.remaining;
    }

    constexpr bool empty() const noexcept
    {
        return (m_in.remaining == 0);
    }

    constexpr bool consume(size_t n) noexcept
    {
        return m_in.consume(n);
    }

    constexpr size_t consume_next(const C c) noexcept
    {
        return m_in.consume_next(c);
    }

    constexpr size_t consume_whitespace() noexcept
    {
        return m_in.consume_whitespace();
    }

private:

    using scan_callback_t = scan_error (*)(void*, basic_scan_context<C>&);

    struct callback_pair
    {
        scan_callback_t callback;
        void* user_data;

        scan_error operator()(basic_scan_context<C>& ctx) const noexcept
        {
            return callback(user_data, ctx);
        }
    };

    constexpr scan_error scan_bounded_field(
        const size_t width,
        const C fill,
        const alignment align,
        const callback_pair callback) noexcept
    {
        const size_t r = remaining();
        const size_t max_width = r < width ? r : width;
        scan_error err = scan_error::none;

        // create a new input bound to the width
        _fmt_priv::input_reader<C> field_in{ ptr(), max_width };
        basic_scan_context<C> field_ctx{ field_in };

        switch (align)
        {
            case alignment::none:
            case alignment::left:
            {
                err = callback(field_ctx);
                const size_t value_size = field_in.consumed(max_width);
                consume(value_size);

                if (err != scan_error::none)
                {
                    return err;
                }

                const size_t expected_right_pad = width - value_size;
                if (!m_in.consume_characters(expected_right_pad, fill))
                {
                    return scan_error::invalid_scaned_field;
                }

                break;
            }
            case alignment::right:
            {
                const size_t left_pad = field_in.consume_next(fill);

                err = callback(field_ctx);
                const size_t total_consumed = field_in.consumed(max_width);
                consume(total_consumed);

                const size_t value_size = total_consumed - left_pad;
                const size_t expected_left_pad = width - value_size;
                if (left_pad != expected_left_pad)
                {
                    return scan_error::invalid_scaned_field;
                }

                if (err != scan_error::none)
                {
                    return err;
                }

                break;
            }
            case alignment::center:
            {
                const size_t left_pad = field_in.consume_next(fill);

                err = callback(field_ctx);
                const size_t total_consumed = field_in.consumed(max_width);
                consume(total_consumed);

                const size_t value_size = total_consumed - left_pad;
                const size_t expected_left_pad = (width - value_size) / 2;
                if (left_pad != expected_left_pad)
                {
                    return scan_error::invalid_scaned_field;
                }

                if (err != scan_error::none)
                {
                    return err;
                }

                const size_t expected_right_pad = width - value_size - expected_left_pad;
                if (!m_in.consume_characters(expected_right_pad, fill))
                {
                    return scan_error::invalid_scaned_field;
                }

                break;
            }
        }

        return scan_error::none;
    }

    constexpr scan_error scan_unbounded_field(
        const C fill,
        const alignment align,
        const callback_pair callback) noexcept
    {
        scan_error err = scan_error::none;

        switch (align)
        {
            case alignment::none:
            case alignment::left:
            {
                err = callback(*this);
                if (err != scan_error::none)
                {
                    return err;
                }

                m_in.consume_next(fill);
                break;
            }
            case alignment::right:
            {
                m_in.consume_next(fill);

                err = callback(*this);
                if (err != scan_error::none)
                {
                    return err;
                }

                break;
            }
            case alignment::center:
            {
                const size_t left_pad = m_in.consume_next(fill);

                err = callback(*this);
                if (err != scan_error::none)
                {
                    return err;
                }

                // with unbounded width and center alignment,
                // padding is expected to be the same on both sides
                if (!m_in.consume_characters(left_pad, fill))
                {
                    return scan_error::invalid_scaned_field;
                }

                break;
            }
        }

        return scan_error::none;
    }

    constexpr scan_error scan_field_begin(
        const basic_scan_spec<C>& spec,
        const callback_pair callback,
        const alignment default_align) noexcept
    {
        // Neither width nor align specified: no fill/align logic runs;
        // the type's own scanner handles input directly (own whitespace rules apply)
        if (spec.width == 0 && spec.align == alignment::none)
        {
            return callback(*this);
        }

        basic_format_spec_base<C> bounded_spec;

        // Width specified, no align: default alignment for the type is used
        // default fill (' ') is used, matching is bounded/exact
        if (spec.width != 0 && spec.align == alignment::none)
        {
            return scan_bounded_field(
                spec.width, spec.fill, default_align,
                callback);
        }

        // Both width and align specified: bounded, exact matching —
        // n = width - value_len determines required fill counts
        if (spec.width != 0 && spec.align != alignment::none)
        {
            return scan_bounded_field(
                spec.width, spec.fill, spec.align,
                callback);
        }

        // Align specified, no width: width is unbounded, fill defaults to ' ' if not
        // explicitly given, matching is greedy/unbounded (symmetric-only for center)
        if (spec.width == 0 && spec.align != alignment::none)
        {
            return scan_unbounded_field(
                spec.fill, spec.align,
                callback);
        }

        return scan_error::none;
    }

    template <typename T>
    struct scan_callback_wrapper
    {
        struct callback_data
        {
            const scanner<T, C>& scn;
            T& value;
        };

        static scan_error invoke(void* user_data, basic_scan_context<C>& ctx)
        {
            auto* data = static_cast<callback_data*>(user_data);
            return data->scn.scan(ctx, data->value);
        }
    };

public:

    template <typename T>
    constexpr scan_error scan_field(
        const scanner<T, C>& scn,
        T& value,
        const alignment default_align) noexcept
    {
        VX_ASSERT(!empty());
        using callback_data = typename scan_callback_wrapper<T>::callback_data;
        callback_data data{ scn, value };
        callback_pair callback{ scan_callback_wrapper<T>::invoke, &data };

        return scan_field_begin(scn, callback, default_align);
    }

    template <typename T>
    constexpr scan_error scan_field(
        const scanner<T, C>& scn,
        T& value) noexcept
    {
        return scan_field(scn, value, scn.default_align);
    }

    template <typename T>
    constexpr bool try_scan_field(
        const scanner<T, C>& scn,
        T& value,
        const alignment default_align) noexcept
    {
        const size_t r = remaining();
        _fmt_priv::input_reader<C> field_in{ ptr(), r };
        basic_scan_context<C> field_ctx{ field_in };

        const auto err = field_ctx.scan_field(scn, value);
        if (err != scan_error::none)
        {
            return false;
        }

        const size_t value_size = field_in.consumed(r);
        consume(value_size);
        return true;
    }

    template <typename T>
    constexpr bool try_scan_field(
        const scanner<T, C>& scn,
        T& value) noexcept
    {
        return try_scan_field(scn, value, scn.default_align);
    }

private:

    _fmt_priv::input_reader<C>& m_in;
};

using scan_context = basic_scan_context<char>;

//==============================================================================

namespace _fmt_priv {

template <typename C>
struct scan_context_creator
{
    static constexpr auto create(input_reader<C>& in) noexcept
    {
        return basic_scan_context<C>{ in };
    }
};

//==============================================================================
// scan
//==============================================================================

template <typename C>
using scan_fn =
    scan_error (*)(
        const basic_parse_context<C>&,
        basic_scan_context<C>&,
        void*);

template <typename T, typename C>
constexpr scan_error invoke_scanner(
    const basic_parse_context<C>& parse_ctx,
    basic_scan_context<C>& scan_ctx,
    void* ptr)
{
    // preserves array-ness, e.g. const char[6]
    using U = typename std::remove_reference<T>::type;
    using DT = typename std::decay<T>::type;

    scanner<DT, C> s;

    auto err = s.parse(parse_ctx);
    if (err != scan_error::none)
    {
        return err;
    }

    // reinterpret as the *actual* stored object type, not the decayed one
    U& ref = *static_cast<U*>(ptr);
    err = s.scan_field(scan_ctx, ref);
    return err;
}

template <typename T, typename C>
constexpr scan_error invoke_scanner_consteval(
    const basic_parse_context<C>& parse_ctx,
    basic_scan_context<C>& scan_ctx,
    T& value) noexcept
{
    using U = std::remove_reference_t<T>;
    using DT = std::remove_cv_t<U>;

    scanner<DT, C> s;

    const auto err = s.parse(parse_ctx);
    if (err != scan_error::none)
    {
        return err;
    }

    return s.scan(scan_ctx, value);
}

//==============================================================================

template <typename C, typename... Args>
constexpr scan_result scan_impl(
    const C* in,
    size_t in_size,
    const C* fmt,
    size_t fmt_size,
    const scan_options& ops,
    Args&&... args) noexcept
{
    constexpr size_t argc = sizeof...(Args);
    const array<void*, argc> values = { &args... };
    const array<scan_fn<C>, argc> funcs = { &invoke_scanner<Args, C>... };

    basic_format_parser<C> parser{ fmt, fmt_size, ops.ws_mode };
    input_reader<C> input{ in, in_size };

    size_t next_arg = 0;
    basic_format_token<C> tok;

    scan_error err = scan_error::none;
    bool scanning = true;
    bool format_error = false;

    while (scanning && !format_error)
    {
        if (!parser.next(tok))
        {
            format_error = true;
            break;
        }

        switch (tok.type)
        {
            case token_type::literal:
            {
                if (input.remaining == 0)
                {
                    err = scan_error::end_of_input;
                    break;
                }

                if (!input.consume_literal(tok.first, tok.calculate_size()))
                {
                    err = scan_error::invalid_scaned_field;
                    break;
                }
                break;
            }
            case token_type::whitespace:
            {
                input.consume_whitespace();
                break;
            }
            case token_type::escaped:
            {
                if (input.remaining == 0)
                {
                    err = scan_error::end_of_input;
                    break;
                }

                if (!input.consume_characters(1, *tok.first))
                {
                    err = scan_error::invalid_scaned_field;
                    scanning = false;
                }
                break;
            }
            case token_type::replacement:
            {
                if (input.remaining == 0)
                {
                    err = scan_error::end_of_input;
                    break;
                }

                // verify index mode
                if (!parser.update_mode(tok.has_index ? index_mode::manual : index_mode::auto_))
                {
                    err = scan_error::index_mode_mismatch;
                    scanning = false;
                    break;
                }

                const size_t index = (tok.has_index)
                    ? tok.index
                    : next_arg++;

                // verify arg index
                if (index >= argc)
                {
                    err = scan_error::invalid_argument;
                    scanning = false;
                    break;
                }

                auto parse_ctx = context_creator<C>::create_parse_context(tok.first, tok.calculate_size());
                auto scan_ctx = context_creator<C>::create_scan_context(input);
                err = funcs[index](parse_ctx, scan_ctx, values[index]);

                if (err == scan_error::invalid_scaned_field && input.remaining == 0)
                {
                    // we consumed all of the input and did not find what we needed
                    err = scan_error::end_of_input;
                }

                if (err != scan_error::none)
                {
                    scanning = false;
                }

                break;
            }
            case token_type::end:
            {
                scanning = false;
                break;
            }
        }
    }

    if (format_error)
    {
        err = scan_error::invalid_format;
    }

    const size_t count = input.consumed(in_size);
    return { err, count };
}

template <size_t I = 0, typename Tuple, typename C>
constexpr scan_error scan_arg(
    input_reader<C>& input,
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

            return invoke_scanner_consteval(
                input,
                parse_ctx,
                std::get<I>(std::forward<Tuple>(tuple)));
        }

        return scan_arg<I + 1>(
            input,
            tok,
            std::forward<Tuple>(tuple),
            index);
    }
    else
    {
        return scan_error::invalid_argument;
    }
}

template <typename C, typename... Args>
constexpr scan_result scan_impl_consteval(
    const C* in,
    size_t in_size,
    const C* fmt,
    size_t fmt_size,
    const scan_options& ops,
    Args&&... args) noexcept
{
    //constexpr size_t argc = sizeof...(Args);
    //
    //auto values = std::forward_as_tuple(std::forward<Args>(args)...);
    //
    //basic_format_parser<C> parser{ fmt, fmt_size, ops.ws_mode };
    //input_reader<C> input{ in, in_size };
    //
    //size_t next_arg = 0;
    //basic_format_token<C> tok;
    //
    //while (true)
    //{
    //    if (!parser.next(tok))
    //    {
    //        return { scan_error::invalid_format, 0 };
    //    }
    //
    //    switch (tok.type)
    //    {
    //        case token_type::literal:
    //        {
    //            if (!input.consume_literal(tok.first, tok.calculate_size()))
    //            {
    //                return { scan_error::invalid_format, 0 };
    //            }
    //
    //            break;
    //        }
    //        case token_type::escaped:
    //        {
    //            if (!input.consume_characters(2, *tok.first))
    //            {
    //                return { scan_error::invalid_format, 0 };
    //            }
    //
    //            break;
    //        }
    //        case token_type::replacement:
    //        {
    //            //_FORMAT_RET_IF(
    //            //    (!parser.update_mode(
    //            //        tok.has_index
    //            //            ? index_mode::manual
    //            //            : index_mode::auto_)),
    //            //    (scan_error{ scan_error::index_mode_mismatch, 0 }));
    //            //
    //            //const size_t index = tok.has_index
    //            //    ? tok.index
    //            //    : next_arg++;
    //            //
    //            //_FORMAT_RET_IF(
    //            //    (index >= argc),
    //            //    (scan_error{ scan_error::invalid_argument, 0 }));
    //            //
    //            //const auto fmt_err = scan_arg(
    //            //    input,
    //            //    tok,
    //            //    values,
    //            //    index);
    //            //
    //            //if (fmt_err != scan_error::none)
    //            //{
    //            //    return { fmt_err, 0 };
    //            //}
    //
    //            break;
    //        }
    //        case token_type::end:
    //        {
    //            const size_t count = in_size - input.remaining;
    //            return { scan_error::none, count };
    //        }
    //    }
    //}

    return { scan_error::none, 0 };
}

} // namespace _fmt_priv

//==============================================================================
// integer
//==============================================================================

template <typename I, typename C>
struct scanner<I, C, VX_REQUIRES_TYPE(std::is_integral<I>::value && !type_traits::is_char<I>::value && !std::is_same<I, bool>::value)> : basic_scan_spec<C>
{
private:

    using base = basic_scan_spec<C>;

public:

    constexpr scan_error parse(const basic_parse_context<C>& ctx) noexcept
    {
        const auto err = ctx.parse_basic_spec(*this);
        if (err != format_error::none)
        {
            return scan_error::invalid_format;
        }

        base::default_align = (base::type == C('c'))
            ? alignment::left
            : alignment::right;

        return scan_error::none;
    }

    constexpr scan_error scan_field(basic_scan_context<C>& ctx, I& value) const noexcept
    {
        return ctx.scan_field(*this, value);
    }

    constexpr scan_error scan(
        basic_scan_context<C>& ctx,
        I& value) const noexcept
    {
        if (ctx.empty())
        {
            return scan_error::invalid_scaned_field;
        }

        if (base::type == C('c'))
        {
            value = static_cast<I>(*ctx.ptr());
            ctx.consume(1);
            return scan_error::none;
        }

        if (base::width == 0)
        {
            ctx.consume_whitespace();
        }

        const C* ptr = ctx.ptr();
        const size_t remaining = ctx.remaining();

        const int b = _fmt_priv::parse_integer_base(base::type);
        _FORMAT_RET_IF((b == 0), scan_error::invalid_format);

        const auto res = strconv::_strconv_priv::parse_integer_impl<I, C, true, true>(
            ptr, remaining,
            value, b);

        ctx.consume(res.count);

        if (res.err == strconv::from_string_error::out_of_range)
        {
            return scan_error::result_out_of_range;
        }

        return (res.err == strconv::from_string_error::none)
            ? scan_error::none
            : scan_error::invalid_scaned_field;
    }
};

//==============================================================================
// character
//==============================================================================

template <typename C1, typename C2>
struct scanner<C1, C2, VX_REQUIRES_TYPE(type_traits::is_char<C1>::value)> : scanner<typename std::make_unsigned<C1>::type, C2>
{
private:

    VX_STATIC_ASSERT_MSG(sizeof(C1) <= sizeof(C2), "Narrowing conversion not allowed");
    using U = typename std::make_unsigned<C1>::type;
    using base = scanner<U, C2>;

public:

    constexpr scan_error parse(const basic_parse_context<C2>& ctx) noexcept
    {
        const auto err = ctx.parse_basic_spec(*this);
        if (err != format_error::none)
        {
            return scan_error::invalid_format;
        }

        if (base::type == C2('\0'))
        {
            base::type = C2('c');
        }

        base::default_align = (base::type == C2('c'))
            ? alignment::left
            : alignment::right;

        return scan_error::none;
    }

    constexpr scan_error scan_field(
        basic_scan_context<C2>& ctx,
        C1& value) const noexcept
    {
        U uvalue;
        const auto err = base::scan_field(ctx, uvalue);
        if (err != scan_error::none)
        {
            return err;
        }

        value = static_cast<C2>(uvalue);
        return scan_error::none;
    }
};

//==============================================================================
// bool
//==============================================================================

template <typename C>
struct scanner<bool, C, VX_REQUIRES_TYPE(type_traits::is_char<C>::value)> : scanner<int, C>
{
private:

    using I = int;
    using base = scanner<I, C>;

    static constexpr C true_str[] = { C('t'), C('r'), C('u'), C('e') };
    static constexpr C false_str[] = { C('f'), C('a'), C('l'), C('s'), C('e') };

public:

    constexpr scan_error scan_field(
        basic_scan_context<C>& ctx,
        bool& value) const noexcept
    {
        if (base::type == C('s'))
        {
            return ctx.scan_field(*this, value, alignment::left);
        }

        if (base::type == C('\0'))
        {
            // try string first
            if (ctx.try_scan_field(*this, value, alignment::left))
            {
                return scan_error::none;
            }
        }

        I ivalue;
        const auto err = base::scan_field(ctx, ivalue);
        if (err != scan_error::none)
        {
            return err;
        }

        value = static_cast<bool>(ivalue);
        return scan_error::none;
    }

    constexpr scan_error scan(
        basic_scan_context<C>& ctx,
        bool& value) const noexcept
    {
        VX_ASSERT(base::type == C('s') || base::type == C('\0'));

        if (base::width == 0)
        {
            ctx.consume_whitespace();
        }

        const C* ptr = ctx.ptr();
        const size_t remaining = ctx.remaining();
        if (remaining < 4)
        {
            return scan_error::invalid_scaned_field;
        }

        if (str::compare(ptr, 4, true_str, 4) == 0)
        {
            value = true;
            ctx.consume(4);
            return scan_error::none;
        }
        if (remaining >= 5 && str::compare(ptr, 5, false_str, 5) == 0)
        {
            value = false;
            ctx.consume(5);
            return scan_error::none;
        }

        return scan_error::invalid_scaned_field;
    }
};

//==============================================================================
// string like types
//==============================================================================

namespace _fmt_priv {

template <typename C>
struct string_scanner : basic_scan_spec<C>
{
private:

    using base = basic_scan_spec<C>;

public:

    constexpr scan_error parse(const basic_parse_context<C>& ctx) noexcept
    {
        const auto err = ctx.parse_basic_spec(*this);
        if (err != format_error::none)
        {
            return scan_error::invalid_format;
        }

        if (base::type == C('c') && !base::width)
        {
            return scan_error::invalid_format;
        }

        base::default_align = alignment::left;
        return scan_error::none;
    }

    static constexpr size_t find_size(const C* ptr, const size_t size) noexcept
    {
        size_t i = 0;
        while (i < size && !str::is_space(ptr[i]))
        {
            ++i;
        }

        return i;
    }
};

} // namespace _fmt_priv

//==============================================================================

template <typename S, typename C>
struct scanner<S, C, VX_REQUIRES_TYPE(str::is_string_of<S, C>::value)> : _fmt_priv::string_scanner<C>
{
private:

    using base = _fmt_priv::string_scanner<C>;

public:

    constexpr scan_error scan_field(
        basic_scan_context<C>& ctx,
        S& value) const noexcept
    {
        return ctx.scan_field(*this, value);
    }

    constexpr scan_error scan(
        basic_scan_context<C>& ctx,
        S& value) const noexcept
    {
        if (base::type != C('c') && base::width == 0)
        {
            if (!(base::type == C('\0') || base::type == C('s')))
            {
                return scan_error::invalid_format;
            }

            ctx.consume_whitespace();
        }

        const C* ptr = ctx.ptr();
        const size_t remaining = ctx.remaining();

        const size_t count = (base::type == C('c'))
            ? base::width
            : base::find_size(ptr, remaining);

        if (count == 0)
        {
            return scan_error::invalid_scaned_field;
        }

        value = S{ ptr, count };
        ctx.consume(count);

        return scan_error::none;
    }
};

//==============================================================================
// float
//==============================================================================

template <typename F, typename C>
struct scanner<F, C, VX_REQUIRES_TYPE(std::is_floating_point<F>::value)> : basic_scan_spec<C>
{
private:

    using base = basic_scan_spec<C>;

public:

    constexpr scan_error parse(const basic_parse_context<C>& ctx) noexcept
    {
        const auto err = ctx.parse_basic_spec(*this);
        if (err != format_error::none)
        {
            return scan_error::invalid_format;
        }

        base::default_align = alignment::right;
        return scan_error::none;
    }

    constexpr scan_error scan_field(basic_scan_context<C>& ctx, F& value) const noexcept
    {
        return ctx.scan_field(*this, value);
    }

    constexpr scan_error scan(
        basic_scan_context<C>& ctx,
        F& value) const noexcept
    {
        strconv::float_from_string_format_options<C> fmt{};

        const bool is_uppercase = str::is_upper(base::type);
        const C lower_type = is_uppercase
            ? str::to_lower_ascii_unchecked(base::type)
            : base::type;

        switch (lower_type)
        {
            case C('\0'):
            case C('g'):
            case C('f'):
            case C('e'):
            case C('a'):
            {
                fmt.format = static_cast<strconv::float_format>(lower_type);
                break;
            }
            default:
            {
                _FORMAT_RET_IF(true, scan_error::invalid_format);
            }
        }

        if (base::width == 0)
        {
            ctx.consume_whitespace();
        }

        const C* ptr = ctx.ptr();
        const size_t remaining = ctx.remaining();

        strconv::from_string_result res;

        if (lower_type == C('a'))
        {
            res = strconv::_strconv_priv::parse_float_start<F, C, true>(
                ptr, remaining,
                value, fmt);
        }
        else
        {
            res = strconv::_strconv_priv::parse_float_start<F, C, false>(
                ptr, remaining,
                value, fmt);
        }

        ctx.consume(res.count);

        if (res.err == strconv::from_string_error::out_of_range)
        {
            return scan_error::result_out_of_range;
        }

        return (res.err == strconv::from_string_error::none)
            ? scan_error::none
            : scan_error::invalid_scaned_field;
    }
};

//==============================================================================
// pointer
//==============================================================================

template <typename C>
struct scanner<void*, C> : basic_scan_spec<C>
{
private:

    using base = basic_scan_spec<C>;
    using U = uintptr_t;

    static constexpr scan_result scan_pointer(const C* s, size_t size, U& value) noexcept
    {
        size_t i = 0;

        if (size <= 2)
        {
            return { scan_error::invalid_scaned_field, 0 };
        }

        if (s[i] != C('0'))
        {
            return { scan_error::invalid_scaned_field, 0 };
        }

        ++i;

        if (!(s[i] == C('x') || s[i] == C('X')))
        {
            return { scan_error::invalid_scaned_field, 0 };
        }

        ++i;

        constexpr U uint_max = static_cast<U>(-1);
        constexpr U risky_value = static_cast<U>(uint_max / 16);
        constexpr U max_digit = static_cast<U>(uint_max % 16);

        const size_t start = i;
        bool overflow = false;

        U uvalue = 0;

        while (i < size)
        {
            const auto digit = strconv::char_to_digit(s[i], 16);
            if (digit == strconv::invalid_digit)
            {
                break;
            }

            if (uvalue < risky_value || (uvalue == risky_value && digit <= max_digit))
            {
                uvalue = static_cast<U>(uvalue * 16 + digit);
            }
            else
            {
                overflow = true;
            }

            ++i;
        }

        if (i == start)
        {
            return { scan_error::invalid_scaned_field, 0 };
        }
        if (overflow)
        {
            return { scan_error::result_out_of_range, i };
        }

        value = uvalue;
        return { scan_error::none, i };
    }

public:

    constexpr scan_error parse(const basic_parse_context<C>& ctx) noexcept
    {
        const auto err = ctx.parse_basic_spec(*this);
        if (err != format_error::none)
        {
            return scan_error::invalid_format;
        }

        if (!(base::type == C('\0') || base::type == C('p')))
        {
            return scan_error::invalid_format;
        }

        base::default_align = alignment::right;
        return scan_error::none;
    }

    constexpr scan_error scan_field(basic_scan_context<C>& ctx, void*& value) noexcept
    {
        return ctx.scan_field(*this, value);
    }

    constexpr scan_error scan(
        basic_scan_context<C>& ctx,
        void*& value) const noexcept
    {
        if (base::width == 0)
        {
            ctx.consume_whitespace();
        }

        const C* ptr = ctx.ptr();
        const size_t remaining = ctx.remaining();

        U uvalue;
        const auto res = scan_pointer(ptr, remaining, uvalue);
        ctx.consume(res.count);

        if (res.err != scan_error::none)
        {
            return res.err;
        }

        value = reinterpret_cast<void*>(uvalue);
        return scan_error::none;
    }
};

//==============================================================================

template <typename T, typename C>
struct scanner<T*, C, VX_REQUIRES_TYPE(!type_traits::is_char<T>::value)> : scanner<void*, C>
{};

//==============================================================================

template <typename C>
struct scanner<std::nullptr_t, C> : scanner<void*, C>
{};

//==============================================================================
// scan
//==============================================================================

template <typename C, typename... Args, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr scan_result scan(
    const C* in,
    size_t in_size,
    const C* fmt,
    size_t fmt_size,
    Args&&... args) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return _fmt_priv::scan_impl_consteval(
            in,
            in_size,
            fmt,
            fmt_size,
            {},
            std::forward<Args>(args)...);
    }
    else
    {
        return _fmt_priv::scan_impl(
            in,
            in_size,
            fmt,
            fmt_size,
            {},
            std::forward<Args>(args)...);
    }
}

} // namespace fmt
} // namespace vx
