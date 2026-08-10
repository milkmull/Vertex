#pragma once

//==============================================================================
// vx::fmt — compile-time-friendly, allocation-free scanning of formatted
// input; the read-side mirror of vx::fmt's formatting facility
//==============================================================================
// SUPPORTED TYPES
// ---------------
// The following builtin argument types are scannable:
//
//   string_view
//     - string-like types recognized by `str::is_string_view<S>`
//
//     Supported options:
//       fill, alignment, width, type
//
//     Supported types:
//       `s`       string presentation (default) — skips leading
//                 whitespace (when unbounded), then reads up to the next
//                 whitespace character or the end of input/width
//       `c`       raw character presentation — requires an explicit
//                 width, does not skip or stop at whitespace; reads
//                 exactly `width` characters verbatim
//       `?`       escaped/quoted presentation: expects the input to
//                 begin with `"`, skipping leading whitespace first
//                 (when unbounded), and scans until an unescaped
//                 closing `"`. Recognizes the same escapes `format`
//                 produces (`\\`, `\"`, `\n`, `\t`, `\r`, `\0`, `\xHH`)
//                 well enough to correctly find the closing quote even
//                 when it's preceded by an escaped one — but does NOT
//                 decode them. The input must be a well-formed
//                 quoted+escaped token (a malformed or unterminated
//                 escape sequence fails to scan with
//                 `invalid_scaned_field`), but the returned view spans
//                 the raw quoted content verbatim, backslashes and all,
//                 since string_view scanning is zero-copy and has
//                 nowhere to write a decoded value. Use `char` scanning
//                 with `?` if you need the actual decoded character(s).
//
//   Integral types
//     All integral types except `bool` and character types.
//
//     Supported options:
//       fill, alignment, width, type
//
//     Supported types:
//       `d`       decimal (default)
//       `o`       octal
//       `x`       hexadecimal
//       `X`       hexadecimal (case-insensitive on input; same as `x`)
//       `b`       binary
//       `B`       binary (case-insensitive on input; same as `b`)
//       `c`       reads a single raw character and reinterprets its code
//                 unit as the integer value
//
//     Leading whitespace is skipped first (when unbounded). A leading
//     `+` or `-` is always accepted. For `x`/`X`, `b`/`B`, and `o`, an
//     optional radix prefix (`0x`/`0X`, `0b`/`0B`, `0`) is accepted but
//     not required. Scanning stops at the first character that doesn't
//     belong to the field; it is not an error to have trailing input
//     left over ("42abc" successfully scans 42).
//
//   Characters
//     Character types recognized by `type_traits::is_char`.
//
//     Supported options:
//       fill, alignment, width, type
//
//     Supported type:
//       `c`       character presentation (default) — reads exactly one
//                 raw character; does NOT skip leading whitespace
//       `?`       escaped/quoted presentation: expects a single
//                 character wrapped in `'...'`, using the same escape
//                 grammar as string_view's `?`. Unlike the string_view
//                 case, this DOES decode the escape into the actual
//                 character value — a single `char` is an owned value
//                 rather than a view, so there's nowhere for the
//                 zero-copy constraint to bite. An unrecognized or
//                 truncated escape, or a missing closing `'`, fails
//                 with `invalid_scaned_field`.
//
//     Character values may also use the integer presentation types
//     (`d`, `o`, `x`, `X`, `b`, `B`), in which case the value is parsed
//     numerically (with whitespace skipping, per the integer rules
//     above) and narrowed to the character type.
//
//   bool
//     Supported options:
//       fill, alignment, width, type
//
//     Supported types:
//       (none)    tries the textual form first, then falls back to the
//                 numeric form on failure (default)
//       `s`       textual presentation only: exactly `true` or `false`,
//                 case-sensitive
//       `d/o/x/X/b/B`  numeric presentation only, per the integer rules
//                 above; any nonzero value scans as `true`
//
//   Floating-point types
//     All types for which `std::is_floating_point<F>::value` is true.
//
//     Supported options:
//       fill, alignment, width, type
//
//     Supported types:
//       `f/F`     fixed-point decimal
//       `e/E`     scientific notation
//       `g/G`     general floating-point notation (default)
//       `a/A`     hexadecimal floating-point notation
//
//     Leading whitespace is skipped first (when unbounded). A leading
//     `+`/`-` is accepted for all forms. For `a`/`A`, an optional
//     `0x`/`0X` prefix is accepted but not required. Also recognizes
//     `inf`/`infinity` and `nan`/`nan(payload)`, case-insensitively per
//     the sign rules above. Scanning stops at the first character that
//     doesn't extend the field.
//
//   Pointers
//     - `void*`
//     - `T*` for non-character `T`
//     - `std::nullptr_t`
//
//     Supported options:
//       fill, alignment, width, type
//
//     Supported type:
//       `p`       hexadecimal pointer (default)
//
//     Unlike integers, the `0x`/`0X` prefix is REQUIRED, and no sign
//     (`+`/`-`) is permitted.
//
//
// SCAN STRING SYNTAX
// -------------------
// Identical to the format string syntax: replacement fields are
// `{}` / `{0}` / `{:spec}` / `{0:spec}`, auto- and manual-indexing
// cannot be mixed, and literal braces are escaped by doubling
// (`"{{"` matches a literal `{`).
//
// Any literal text in the scan string (outside of replacement fields)
// must match the input exactly. Whitespace in the scan string (outside
// a replacement field) matches zero or more whitespace characters in
// the input, matching how `std::scanf`-family functions treat format
// string whitespace.
//
//
// SCAN SPECIFICATION
// -------------------
//     [fill][alignment][width][type]
//
// Sign, alternate form (`#`), zero-padding (`0`), and precision (`.N`)
// are not part of the scan spec grammar — those are output-formatting
// concerns with no scanning analog.
//
// Alignment governs how fill characters around the field are matched,
// not produced:
//
//     `<`       expect the value, then fill (unbounded: any amount of
//               trailing fill is consumed; bounded: exactly
//               `width - value_size` trailing fill characters required)
//     `^`       expect fill split evenly before/after the value (an odd
//               leftover fill character goes on the right)
//     `>`       expect fill, then the value
//
// If alignment is omitted:
//   - With no width either: no fill/align logic runs at all — the
//     type's own scanner reads the input directly, using its own
//     whitespace rules (see SUPPORTED TYPES above).
//   - With a width: the type's default alignment is used (left for
//     strings/characters, right for everything else), fill defaults to
//     a space, and the field is bounded to exactly `width` characters —
//     it is an error if there's a mismatch between the required padding
//     and what's actually present, or if fewer than `width` characters
//     remain in the input.
//
// A fill character may precede an alignment character (`*<10`,
// `.^10`); without one, padding is matched against a space. Fill
// requires an explicit alignment character — `{:*}` alone (fill with
// no alignment) is `invalid_format`.
//
// The type character selects presentation, exactly as in format specs;
// see SUPPORTED TYPES for the valid characters per argument type.
// Unsupported type/specification combinations result in
// `scan_error::invalid_format`.
//
//
// ERROR REPORTING
// -----------------
// `scan_result{ err, count }` is returned by every entry point. `count`
// is only meaningful when `err == scan_error::none`, in which case it
// is the total number of input characters consumed:
//
//   - none                  : every field scanned successfully
//   - end_of_input           : the input was exhausted before a literal,
//                             escape, or field could be fully matched
//                             (this also replaces invalid_scaned_field
//                             whenever the input buffer_type is empty at the
//                             point of failure — running out of input
//                             is reported as end_of_input rather than
//                             a content mismatch)
//   - invalid_format         : the scan string itself was malformed
//                             (bad index, bad spec grammar, unsupported
//                             type for the argument, etc.)
//   - invalid_argument       : a manual index (e.g. "{5}") was out of
//                             range for the number of arguments passed
//   - invalid_scaned_field   : input was present but didn't match the
//                             expected literal text, fill/alignment, or
//                             a type's grammar (e.g. non-digit where a
//                             digit was required). For `?` on strings
//                             and characters, this also covers a missing
//                             opening/closing quote and any malformed or
//                             truncated escape sequence.
//   - result_out_of_range    : the field matched the expected grammar
//                             but the value didn't fit in the target
//                             type (integer/float overflow, or a
//                             pointer literal wider than `uintptr_t`)
//   - index_mode_mismatch    : the scan string mixed "{}" (auto) and
//                             "{N}" (manual) indexing
//
//
// EXTENDING
// -----------
// To scan a custom type `T`, specialize:
//
//   template <typename C>
//   struct vx::fmt::scanner<MyType, C>
//   {
//       constexpr bool parse(const basic_parse_context<C>& ctx) noexcept;
//       constexpr scan_error scan_field(basic_scan_context<C>& ctx,
//                                        MyType& value) const noexcept;
//       constexpr scan_error scan(basic_scan_context<C>& ctx,
//                                  MyType& value) const noexcept;
//   };
//
// `parse()` typically delegates to `ctx.parse_basic_spec(*this)`.
// `scan_field()` typically delegates to
// `ctx.scan_field(*this, value, default_align)`, which applies the
// fill/alignment/width machinery described above around a call to
// `scan()`. `scan()` does the actual parsing of the (possibly
// width-bounded) input range via `ctx.in()`/`ctx.remaining()`, and
// must call `ctx.consume(n)` for however many characters it read —
// even on failure, so that `end_of_input` promotion and error
// `count`s stay accurate. `basic_scan_context` also exposes
// `consume_whitespace()`, `consume_character(c)`, and
// `consume_literal(data, count)` as building blocks for matching
// fixed delimiters within a custom grammar.
//
//
// EXAMPLES
// ----------
//     "{}"                  // default scanning
//     "{:s}"                // string, stop at whitespace
//     "{:10c}"              // read exactly 10 raw characters
//     "{:x}"                // hexadecimal integer, optional 0x prefix
//     "{:5}"                // bounded to exactly 5 characters, default
//                           // alignment/fill for the type
//     "{:*^10}"             // center-aligned within a 10-char field,
//                           // '*' fill required on both sides
//     "{:a}"                // hexadecimal floating-point
//     "{:p}"                // pointer, REQUIRES 0x/0X prefix
//     "x = {}, y = {}"      // literal text must match verbatim
//==============================================================================

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
    static constexpr bool s_not_scannable = true;
};

//==============================================================================
// traits
//==============================================================================

template <typename T, typename C>
struct is_scannable
{
private:

    template <typename U>
    static auto test(int)
        -> decltype(scanner<U, C>::s_not_scannable,
            std::false_type{});

    template <typename>
    static auto test(...)
        -> std::true_type;

public:

    static constexpr bool value =
        decltype(test<T>(0))::value;
};

template <typename T, typename C>
struct is_builtin_scannable
{
private:

    template <typename U>
    static auto test(int) -> decltype(scanner<U, C>::s_is_builtin,
        std::true_type{});

    template <typename>
    static auto test(...) -> std::false_type;

public:

    static constexpr bool value =
        decltype(test<T>(0))::value;
};

//==============================================================================
// scan context
//==============================================================================

namespace _fmt_priv {

template <typename C>
struct input_reader
{
    const C* ptr;
    size_t remaining;

    constexpr size_t consumed(const size_t starting_size) const noexcept
    {
        return starting_size - remaining;
    }

    constexpr bool empty() const noexcept
    {
        return remaining == 0;
    }

    // Skip up to n characters unconditionally. Returns true if data remains.
    constexpr bool consume(size_t n) noexcept
    {
        const size_t n_consumed = n < remaining ? n : remaining;
        ptr += n_consumed;
        remaining -= n_consumed;
        return remaining != 0;
    }

    // Consumes the longest matching prefix of `count` copies of `c`
    // (bounded by whatever's left in the buffer_type). Returns true only if
    // the full run of `count` was matched.
    constexpr bool consume_repeated(size_t count, const C c) noexcept
    {
        const size_t n = count < remaining ? count : remaining;
        size_t i = 0;

        for (; i < n; ++i)
        {
            if (ptr[i] != c)
            {
                break;
            }
        }

        ptr += i;
        remaining -= i;

        return i == count;
    }

    constexpr bool consume_character(const C c) noexcept
    {
        return consume_repeated(1, c);
    }

    // Consumes the longest matching prefix shared with data[0..count)
    // (bounded by whatever's left in the buffer_type). Returns true only if
    // the full literal was matched.
    constexpr bool consume_literal(const C* data, size_t count) noexcept
    {
        const size_t n = count < remaining ? count : remaining;

        size_t i = 0;
        for (; i < n; ++i)
        {
            if (ptr[i] != data[i])
            {
                break;
            }
        }

        ptr += i;
        remaining -= i;

        return i == count;
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

    constexpr size_t consume_characters(const C c) noexcept
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

    constexpr const C* in() const noexcept
    {
        return m_in.ptr;
    }

    constexpr const C* in() noexcept
    {
        return m_in.ptr;
    }

    constexpr size_t remaining() const noexcept
    {
        return m_in.remaining;
    }

    constexpr bool empty() const noexcept
    {
        return m_in.empty();
    }

    constexpr bool consume(size_t n) noexcept
    {
        return m_in.consume(n);
    }

    constexpr size_t consume_whitespace() noexcept
    {
        return m_in.consume_whitespace();
    }

    constexpr bool consume_character(const C c) noexcept
    {
        return m_in.consume_character(c);
    }

    constexpr bool consume_literal(const C* data, size_t count) noexcept
    {
        return m_in.consume_literal(data, count);
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
        _fmt_priv::input_reader<C> field_in{ in(), max_width };
        basic_scan_context<C> field_ctx{ field_in };

        switch (align)
        {
            default:
            case alignment::left:
            {
                err = callback(field_ctx);
                const size_t total_consumed = field_in.consumed(max_width);
                consume(total_consumed);

                if (err != scan_error::none)
                {
                    return err;
                }

                const size_t expected_right_pad = width - total_consumed;
                if (!m_in.consume_repeated(expected_right_pad, fill))
                {
                    return scan_error::invalid_scaned_field;
                }

                break;
            }
            case alignment::right:
            {
                const size_t left_pad = field_in.consume_characters(fill);

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
                const size_t left_pad = field_in.consume_characters(fill);

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
                if (!m_in.consume_repeated(expected_right_pad, fill))
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
            default:
            case alignment::left:
            {
                err = callback(*this);
                if (err != scan_error::none)
                {
                    return err;
                }

                m_in.consume_characters(fill);
                break;
            }
            case alignment::right:
            {
                m_in.consume_characters(fill);

                err = callback(*this);
                if (err != scan_error::none)
                {
                    return err;
                }

                break;
            }
            case alignment::center:
            {
                const size_t left_pad = m_in.consume_characters(fill);

                err = callback(*this);
                if (err != scan_error::none)
                {
                    return err;
                }

                // with unbounded width and center alignment,
                // padding is expected to be the same on both sides
                if (!m_in.consume_repeated(left_pad, fill))
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

        basic_format_spec<C> bounded_spec;

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
    constexpr bool try_scan_field(
        const scanner<T, C>& scn,
        T& value,
        const alignment default_align) noexcept
    {
        const size_t r = remaining();
        _fmt_priv::input_reader<C> field_in{ in(), r };
        basic_scan_context<C> field_ctx{ field_in };

        const auto err = field_ctx.scan_field(scn, value, default_align);
        if (err != scan_error::none)
        {
            return false;
        }

        const size_t value_size = field_in.consumed(r);
        consume(value_size);
        return true;
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

    if (!s.parse(parse_ctx))
    {
        return scan_error::invalid_format;
    }

    // reinterpret as the *actual* stored object type, not the decayed one
    U& ref = *static_cast<U*>(ptr);
    return s.scan_field(scan_ctx, ref);
}

//==============================================================================

template <typename C, typename... Args>
constexpr scan_result scan_impl(
    const C* in,
    const size_t in_size,

    const C* fmt,
    const size_t fmt_size,

    const whitespace_mode ws_mode,

    const size_t argc,
    void* const* values,
    const scan_fn<C>* funcs) noexcept
{
    basic_format_parser<C> parser{ fmt, fmt_size, ws_mode };
    input_reader<C> input{ in, in_size };

    size_t next_arg = 0;
    basic_format_token<C> tok;
    scan_error err = scan_error::none;

    while (true)
    {
        if (!parser.next(tok))
        {
            err = scan_error::invalid_format;
            break;
        }

        if (tok.type == token_type::end)
        {
            break;
        }

        switch (tok.type)
        {
            case token_type::literal:
            {
                if (!input.consume_literal(tok.first, tok.calculate_size()))
                {
                    err = scan_error::end_of_input;
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
                if (!input.consume_character(*tok.first))
                {
                    err = scan_error::end_of_input;
                }
                break;
            }
            case token_type::replacement:
            {
                if (input.empty())
                {
                    err = scan_error::end_of_input;
                    break;
                }

                // verify index mode
                if (!parser.update_mode(tok.has_index ? index_mode::manual : index_mode::auto_))
                {
                    err = scan_error::index_mode_mismatch;
                    break;
                }

                const size_t index = tok.has_index ? tok.index : next_arg++;
                if (index >= argc)
                {
                    err = scan_error::invalid_argument;
                    break;
                }

                auto parse_ctx = parse_context_creator<C>::create(tok.first, tok.calculate_size());
                auto scan_ctx = scan_context_creator<C>::create(input);
                err = funcs[index](parse_ctx, scan_ctx, values[index]);
                break;
            }
            case token_type::end:
            {
                VX_UNREACHABLE();
            }
        }

        if (err == scan_error::invalid_scaned_field && input.empty())
        {
            err = scan_error::end_of_input;
        }

        if (err != scan_error::none)
        {
            break;
        }
    }

    const size_t count = in_size - input.remaining;
    return { err, count };
}

template <typename C, typename... Args>
constexpr scan_result scan_begin(
    const C* in,
    const size_t in_size,
    const C* fmt,
    const size_t fmt_size,
    const whitespace_mode ws_mode,
    Args&&... args) noexcept
{
    constexpr size_t argc = sizeof...(Args);
    const array<void*, argc> values = { &args... };
    const array<scan_fn<C>, argc> funcs = { &invoke_scanner<Args, C>... };

    return scan_impl<C>(in, in_size, fmt, fmt_size, ws_mode, argc, values.data(), funcs.data());
}

template <typename C, typename T>
constexpr scan_result scan_simple_begin(
    const C* in,
    size_t in_size,
    T& value) noexcept
{
    using DT = typename std::decay<T>::type;
    scanner<DT, C> s;

    // The builtin types don't do any setup in their parse
    // function, so it can be skipped
    VX_IF_CONSTEXPR (!is_builtin_scannable<T, C>::value)
    {
        const C end = closed_brace;
        auto parse_ctx = parse_context_creator<C>::create(&end, 1);
        if (!s.parse(parse_ctx))
        {
            return { scan_error::invalid_format, 0 };
        }
    }

    input_reader<C> input{ in, in_size };
    auto scan_ctx = scan_context_creator<C>::create(input);

    const auto err = s.scan(scan_ctx, value);

    const size_t count = in_size - input.remaining;
    return { err, count };
}

} // namespace _fmt_priv

//==============================================================================
// basic scanner
//==============================================================================

template <typename T, typename C>
struct basic_scanner : basic_scan_spec<C>
{
private:

    using base = basic_scan_spec<C>;

public:

    constexpr bool parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_spec(*this);
    }
};

//==============================================================================
// string like types
//==============================================================================

namespace _fmt_priv {

template <typename C>
static constexpr size_t find_string_size(const C* ptr, const size_t size) noexcept
{
    size_t i = 0;
    while (i < size && !str::is_space(ptr[i]))
    {
        ++i;
    }

    return i;
}

} // namespace _fmt_priv

//==============================================================================

template <typename S, typename C>
struct scanner<S, C, VX_REQUIRES_TYPE(str::is_string_view<S>::value&& std::is_same<typename S::value_type, C>::value)> : basic_scanner<S, C>
{
private:

    using base = basic_scanner<S, C>;

    friend is_scannable;
    static constexpr bool s_is_builtin = true;

#if defined(VX_FORMAT_ESCAPED_SUPPORT)

    constexpr scan_error scan_escaped(basic_scan_context<C>& ctx, S& value) const noexcept
    {
        if (base::width == 0)
        {
            ctx.consume_whitespace();
        }

        if (!ctx.consume_character(C('"')))
        {
            return scan_error::invalid_scaned_field;
        }

        const C* start = ctx.in();
        const size_t remaining = ctx.remaining();
        size_t i = 0;
        bool closed = false;

        while (i < remaining)
        {
            const C c = start[i];

            if (c == C('"'))
            {
                closed = true;
                break;
            }

            if (c == C('\\'))
            {
                C decoded;
                const size_t n = _fmt_priv::unescape_char_body<C>(start + i + 1, remaining - i - 1, decoded);

                if (n == 0)
                {
                    return scan_error::invalid_scaned_field;
                }

                i += 1 + n;
                continue;
            }

            ++i;
        }

        if (!closed)
        {
            return scan_error::invalid_scaned_field;
        }

        value = S{ start, i };
        ctx.consume(i);
        ctx.consume(1); // closing quote, already verified present

        return scan_error::none;
    }

#endif // VX_FORMAT_ESCAPED_SUPPORT

public:

    constexpr bool parse(const basic_parse_context<C>& ctx) noexcept
    {
        if (!base::parse(ctx))
        {
            return false;
        }

        if (base::type == C('c') && !base::width)
        {
            return false;
        }

        return !base::type || (base::type == C('s') || base::type == C('c')
#if defined(VX_FORMAT_ESCAPED_SUPPORT)
                                  || base::type == C('?')
#endif // VX_FORMAT_ESCAPED_SUPPORT
                              );
    }

    constexpr scan_error scan_field(
        basic_scan_context<C>& ctx,
        S& value) const noexcept
    {
        return ctx.scan_field(*this, value, alignment::left);
    }

    constexpr scan_error scan(
        basic_scan_context<C>& ctx,
        S& value) const noexcept
    {
#if defined(VX_FORMAT_ESCAPED_SUPPORT)

        if (base::type == C('?'))
        {
            return scan_escaped(ctx, value);
        }

#endif // VX_FORMAT_ESCAPED_SUPPORT

        if (base::type != C('c') && base::width == 0)
        {
            ctx.consume_whitespace();
        }

        const C* ptr = ctx.in();
        const size_t remaining = ctx.remaining();

        const size_t count = (base::type == C('c'))
            ? base::width
            : _fmt_priv::find_string_size(ptr, remaining);

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
// integer
//==============================================================================

template <typename I, typename C>
struct scanner<I, C, VX_REQUIRES_TYPE(std::is_integral<I>::value && !type_traits::is_char<I>::value && !std::is_same<I, bool>::value)> : basic_scanner<I, C>
{
private:

    using base = basic_scan_spec<C>;

    friend is_scannable;
    static constexpr bool s_is_builtin = true;

public:

    constexpr scan_error scan_field(basic_scan_context<C>& ctx, I& value) const noexcept
    {
        const alignment default_alignment = (base::type == C('c'))
            ? alignment::left
            : alignment::right;

        return ctx.scan_field(*this, value, default_alignment);
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
            value = static_cast<I>(*ctx.in());
            ctx.consume(1);
            return scan_error::none;
        }

        if (base::width == 0)
        {
            ctx.consume_whitespace();
        }

        const C* ptr = ctx.in();
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

template <typename Cin, typename C>
struct scanner<Cin, C, VX_REQUIRES_TYPE(type_traits::is_char<Cin>::value)> : scanner<typename std::make_unsigned<Cin>::type, C>
{
private:

    VX_STATIC_ASSERT_MSG(sizeof(Cin) <= sizeof(C), "Narrowing conversion not allowed");
    using U = typename std::make_unsigned<Cin>::type;
    using base = scanner<U, C>;

    friend is_scannable;
    static constexpr bool s_is_builtin = true;

#if defined(VX_FORMAT_ESCAPED_SUPPORT)

    constexpr scan_error scan_escaped(basic_scan_context<C>& ctx, Cin& value) const noexcept
    {
        if (!ctx.consume_character(C('\'')))
        {
            return scan_error::invalid_scaned_field;
        }

        if (ctx.empty())
        {
            return scan_error::invalid_scaned_field;
        }

        C decoded;
        const C c = *ctx.in();

        if (c == C('\\'))
        {
            ctx.consume(1);

            const size_t n = _fmt_priv::unescape_char_body<C>(ctx.in(), ctx.remaining(), decoded);
            if (n == 0)
            {
                return scan_error::invalid_scaned_field;
            }

            ctx.consume(n);
        }
        else
        {
            decoded = c;
            ctx.consume(1);
        }

        if (!ctx.consume_character(C('\'')))
        {
            return scan_error::invalid_scaned_field;
        }

        value = static_cast<Cin>(decoded);
        return scan_error::none;
    }

#endif // VX_FORMAT_ESCAPED_SUPPORT

public:

    constexpr bool parse(const basic_parse_context<C>& ctx) noexcept
    {
        if (!ctx.parse_basic_spec(*this))
        {
            return false;
        }

        if (base::type == C('\0'))
        {
            base::type = C('c');
        }

        return true;
    }

    constexpr scan_error scan_field(
        basic_scan_context<C>& ctx,
        Cin& value) const noexcept
    {
        if (base::type == C('?'))
        {
#if defined(VX_FORMAT_ESCAPED_SUPPORT)
            return ctx.scan_field(*this, value, alignment::left);
#else
            return scan_error::invalid_format;
#endif // VX_FORMAT_ESCAPED_SUPPORT
        }

        U uvalue;
        const auto err = base::scan_field(ctx, uvalue);

        if (err == scan_error::none)
        {
            value = static_cast<C>(uvalue);
        }

        return err;
    }

#if defined(VX_FORMAT_ESCAPED_SUPPORT)

    constexpr scan_error scan(
        basic_scan_context<C>& ctx,
        Cin& value) const noexcept
    {
        return scan_escaped(ctx, value);
    }

#endif // VX_FORMAT_ESCAPED_SUPPORT
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

    friend is_scannable;
    static constexpr bool s_is_builtin = true;

    using boolean_string_t = _fmt_priv::boolean_strings<C>;

    static bool case_compare(const C* in, const C* cmp, size_t count) noexcept
    {
        for (; 0 < count; --count, ++in, ++cmp)
        {
            const C in_lower = str::to_lower(*in);
            if (in_lower != *cmp)
            {
                return false;
            }
        }

        return true;
    }

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

        if (err == scan_error::none)
        {
            value = static_cast<bool>(ivalue);
        }

        return err;
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

        constexpr size_t true_size = mem::array_size(boolean_string_t::true_str);
        constexpr size_t false_size = mem::array_size(boolean_string_t::false_str);

        const C* ptr = ctx.in();
        const size_t remaining = ctx.remaining();

        if (remaining < true_size)
        {
            return scan_error::invalid_scaned_field;
        }

        if (case_compare(ptr, boolean_string_t::true_str, true_size))
        {
            value = true;
            ctx.consume(true_size);
            return scan_error::none;
        }
        if (remaining >= false_size && case_compare(ptr, boolean_string_t::false_str, false_size))
        {
            value = false;
            ctx.consume(false_size);
            return scan_error::none;
        }

        return scan_error::invalid_scaned_field;
    }
};

//==============================================================================
// float
//==============================================================================

template <typename F, typename C>
struct scanner<F, C, VX_REQUIRES_TYPE(std::is_floating_point<F>::value)> : basic_scanner<F, C>
{
private:

    using base = basic_scanner<F, C>;

    friend is_scannable;
    static constexpr bool s_is_builtin = true;

    static constexpr strconv::from_string_result parse_float(
        const C* ptr,
        const size_t count,
        F& value,
        const strconv::float_from_string_format_options<C>& fmt) noexcept
    {
        if (count == 0)
        {
            return { 0, strconv::from_string_error::invalid_argument };
        }

        size_t i = 0;
        bool is_negative = false;

        if (ptr[0] == C('-') || ptr[0] == C('+'))
        {
            if (count == 1)
            {
                return { 0, strconv::from_string_error::invalid_argument };
            }

            ++i;
            is_negative = ptr[0] == C('-');
        }

        if ((count - i >= 2) &&
            (ptr[i] == C('0')) &&
            (ptr[i + 1] == C('x') || ptr[i + 1] == C('X')))
        {
            i += 2;
        }

        auto res = strconv::parse_float(ptr + i, count - i, value, fmt);

        if (res.err == strconv::from_string_error::none)
        {
            if (is_negative)
            {
                value = -value;
            }

            res.count += i;
        }

        return res;
    }

public:

    constexpr scan_error scan_field(
        basic_scan_context<C>& ctx,
        F& value) const noexcept
    {
        return ctx.scan_field(*this, value, alignment::right);
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

        const C* ptr = ctx.in();
        const size_t remaining = ctx.remaining();

        const auto res = parse_float(ptr, remaining, value, fmt);
        ctx.consume(res.count);

        if (res.err == strconv::from_string_error::out_of_range)
        {
            return scan_error::result_out_of_range;
        }

        return res.err == strconv::from_string_error::none
            ? scan_error::none
            : scan_error::invalid_scaned_field;
    }
};

//==============================================================================
// pointer
//==============================================================================

template <typename C>
struct scanner<void*, C> : basic_scanner<void*, C>
{
private:

    using base = basic_scanner<void*, C>;
    using U = uintptr_t;

    friend is_scannable;
    static constexpr bool s_is_builtin = true;

    static constexpr scan_result scan_pointer(const C* s, size_t size, U& value) noexcept
    {
        if (size <= 2)
        {
            return { scan_error::invalid_scaned_field, 0 };
        }

        if (s[0] != C('0') || (s[1] != C('x') && s[1] != C('X')))
        {
            return { scan_error::invalid_scaned_field, 0 };
        }

        constexpr U uint_max = static_cast<U>(-1);
        constexpr U risky_value = static_cast<U>(uint_max / 16);
        constexpr U max_digit = static_cast<U>(uint_max % 16);

        constexpr size_t start = 2;
        size_t i = start;
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

    constexpr scan_error scan_field(
        basic_scan_context<C>& ctx,
        void*& value) const noexcept
    {
        return ctx.scan_field(*this, value, alignment::right);
    }

    constexpr bool parse(const basic_parse_context<C>& ctx) noexcept
    {
        if (!ctx.parse_basic_spec(*this))
        {
            return false;
        }

        return (base::type == C('\0') || base::type == C('p'));
    }

    constexpr scan_error scan(
        basic_scan_context<C>& ctx,
        void*& value) const noexcept
    {
        if (base::width == 0)
        {
            ctx.consume_whitespace();
        }

        const C* ptr = ctx.in();
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

//------------------------------------------------------------------------------
// Scan a single value from a caller-provided buffer_type
//------------------------------------------------------------------------------

template <
    typename C,
    typename T,
    VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr scan_result scan_simple(
    const C* in,
    size_t in_size,
    T& value) noexcept
{
    return _fmt_priv::scan_simple_begin(
        in,
        in_size,
        value);
}

//------------------------------------------------------------------------------
// Scan from a caller-provided buffer_type
//------------------------------------------------------------------------------

template <
    typename C,
    typename... Args,
    VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr scan_result scan(
    const C* in,
    size_t in_size,
    const C* fmt,
    size_t fmt_size,
    Args&&... args) noexcept
{
    return _fmt_priv::scan_begin(
        in,
        in_size,
        fmt,
        fmt_size,
        whitespace_mode::greedy,
        std::forward<Args>(args)...);
}

//------------------------------------------------------------------------------
// Scan a single value from a string-like input
//------------------------------------------------------------------------------

template <
    typename S,
    typename T,
    VX_REQUIRES(str::is_string_like<S>::value)>
scan_result scan_simple(
    const S& in,
    T& value)
{
    using C = typename S::value_type;

    return scan_simple<C>(
        in.data(),
        in.size(),
        value);
}

//------------------------------------------------------------------------------
// Scan from a string-like input
//------------------------------------------------------------------------------

template <
    typename S,
    typename FMT,
    typename... Args,
    VX_REQUIRES(
        str::is_string_like<S>::value&&
            str::is_string_like<FMT>::value&&
                str::is_string_compatible<S, FMT>::value)>
scan_result scan_string(
    const S& in,
    const FMT& fmt,
    Args&&... args)
{
    return scan(
        in.data(),
        in.size(),
        fmt.data(),
        fmt.size(),
        std::forward<Args>(args)...);
}

} // namespace fmt
} // namespace vx
