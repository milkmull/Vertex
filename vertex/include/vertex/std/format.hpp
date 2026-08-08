#pragma once

//==============================================================================
// vx::fmt — compile-time-friendly, allocation-minimal string formatting
//==============================================================================
// OVERVIEW
// ---------
// Python/{fmt}-style "{}"-replacement formatting over raw char buffers.
// Fully usable in constexpr contexts: there is no dynamic dispatch through
// virtual functions and no heap allocation anywhere in the core `format()`
// path — every call site resolves its argument types at compile time via
// variadic templates, producing one branch-free indirect call per
// replacement field (a hand-rolled, allocation-free substitute for a
// vtable) instead of RTTI or `std::function`.
//
// `format()` and its buffer-taking overloads never allocate. Only
// `format_string()` and the convenience `format()` overloads that return
// a `str::basic_string` allocate, and only to grow an output buffer that
// turned out to be too small.
//
// `vx::fmt::scan` (see scan.hpp) is the read-side mirror of this file,
// sharing the same format-string grammar and spec parsing machinery via
// format_scan_common.hpp.
//
//
// SUPPORTED TYPES
// ---------------
// The following builtin argument types are formattable:
//
//   Strings
//     - `const C*`
//     - `C[N]` / `const C[N]`
//     - string-like types recognized by `str::is_string_of<S, C>`
//
//     Supported options:
//       fill, alignment, width, precision, type
//
//     Supported type:
//       `s`       string presentation
//       `?`       escaped/quoted presentation: wraps the value in `"..."`,
//                 escaping `\`, `"`, and control characters. Non-printable
//                 bytes outside ASCII use `\xHH`.
//                 [scan only] does NOT decode escapes — the input must be
//                 a well-formed quoted+escaped token (malformed or
//                 unterminated escapes fail to scan), but the returned
//                 view spans the raw quoted content verbatim, backslashes
//                 included, since string_view scanning is zero-copy.
//
//     With no type specified, strings use the default string presentation.
//
//   Integral types
//     All integral types except `bool` and character types.
//
//     Supported options:
//       fill, alignment, sign, alternate form (`#`), zero padding (`0`),
//       width, type
//
//     Supported types:
//       `d`       decimal (default)
//       `o`       octal
//       `x`       hexadecimal, lowercase
//       `X`       hexadecimal, uppercase
//       `b`       binary, lowercase
//       `B`       binary, uppercase
//
//     Sign options:
//       `+`       always emit a sign for non-negative values
//       ` `       emit a leading space for non-negative values
//
//     `#` enables an alternate prefix for non-decimal formats:
//       binary      `0b` / `0B`
//       octal       `0`
//       hexadecimal `0x` / `0X`
//
//   Characters
//     Character types recognized by `type_traits::is_char`.
//
//     Supported options:
//       fill, alignment, width, type
//
//     Supported type:
//       `c`       character presentation (default)
//       `?`       escaped/quoted presentation: wraps the value in `'...'`
//                 using the same escape rules as strings.
//                 [scan only] DOES decode escapes into the actual
//                 character value, since a single char is owned, not a view.
//
//     Character values may also use the integer presentation types
//     (`d`, `o`, `x`, `X`, `b`, `B`), in which case the value is formatted
//     numerically.
//
//   bool
//     Supported options:
//       fill, alignment, width, type
//
//     Supported types:
//       `s`       textual presentation: `true` / `false` (default)
//       `d`       decimal integer presentation
//       `o`       octal
//       `x`       hexadecimal, lowercase
//       `X`       hexadecimal, uppercase
//       `b`       binary, lowercase
//       `B`       binary, uppercase
//
//     Boolean values use the integer formatter for non-`s` presentations,
//     so the integer formatter's applicable options are also available.
//
//   Floating-point types
//     All types for which `std::is_floating_point<F>::value` is true.
//
//     Supported options:
//       fill, alignment, sign, alternate form (`#`), zero padding (`0`),
//       width, precision, type
//
//     Supported types:
//       `f` / `F`  fixed-point decimal
//       `e` / `E`  scientific notation
//       `g` / `G`  general floating-point notation
//       `a` / `A`  hexadecimal floating-point notation
//
//     `+` forces a sign on non-negative values.
//
//     A space sign requests a leading space on non-negative values.
//
//     `#` causes the generated floating-point representation to force
//     an explicit exponent sign when an exponent is present.
//
//     Precision is passed to the floating-point conversion. If the
//     requested precision exceeds the conversion buffer's native maximum,
//     the formatter extends the required fractional precision with zeros.
//
//   Pointers
//     - `void*`
//     - `T*` for non-character `T`
//     - `std::nullptr_t`
//
//     Supported options:
//       fill, alignment, width, type
//
//     Supported types:
//       `p`       hexadecimal pointer, lowercase
//       `P`       hexadecimal pointer, uppercase
//
//     Pointer values are rendered with a `0x` / `0X` prefix.
//
//
// FORMAT STRING SYNTAX
// --------------------
// Replacement fields use the following forms:
//
//     {}              auto-indexed argument
//     {0}             manually indexed argument
//     {1}             manually indexed argument
//     {:spec}         auto-indexed argument with a format specification
//     {0:spec}        manually indexed argument with a format specification
//
// Auto-indexing and manual indexing cannot be mixed in the same format
// string. For example:
//
//     "{} {}"          // valid
//     "{0} {1}"        // valid
//     "{0} {}"         // invalid: index_mode_mismatch
//
// Literal braces are escaped by doubling them:
//
//     "{{"             // writes `{`
//     "}}"             // writes `}`
//
// Any other text in the format string, including whitespace, is treated
// as literal and copied through unchanged — unlike `vx::fmt::scan`, which
// treats whitespace in its scan string as "match zero or more whitespace
// characters" rather than a literal to copy.
//
//
// FORMAT SPECIFICATION
// --------------------
// Where supported by the argument type, a format specification has the
// following general components:
//
//     fill + alignment
//     sign
//     #
//     0
//     width
//     .precision
//     type
//
// The components are type-dependent. The parser accepts them in the
// following order:
//
//     [fill][alignment][sign][#][0][width][.precision][type]
//
// Not every component is meaningful for every type.
//
// Alignment:
//     `<`       left-align
//     `^`       center-align
//     `>`       right-align
//
// If alignment is omitted, the formatter supplies a type-specific
// default. Strings and characters default to left alignment; numeric
// values default to right alignment.
//
// Fill:
//     A fill character may precede an alignment character:
//
//         `*<10`
//         `.^10`
//
//     Without an explicit fill character, padding uses a space.
//
// Width:
//     A decimal integer specifying the minimum field width.
//
// Precision:
//     A decimal integer following `.`.
//
//     Strings interpret precision as a maximum number of characters.
//     Floating-point values interpret precision as floating-point
//     conversion precision.
//
// Type:
//     The final character of the format specification selects the
//     presentation. The valid type characters depend on the argument:
//
//       strings:       `s`
//       characters:    `c`, or integer types `d/o/x/X/b/B`
//       integers:      `d/o/x/X/b/B`
//       bool:          `s/d/o/x/X/b/B`
//       floating:      `f/F/e/E/g/G/a/A`
//       pointers:      `p/P`
//
// Unsupported type/specification combinations result in
// `format_error::invalid_format`.
//
//
// ERROR REPORTING
// ----------------
// `format_result{ err, count }` is returned by every entry point. `count`
// always reflects the number of bytes actually written to the output
// buffer up to the point of failure (or completion) — never a
// hypothetical "would-have-been" count, and never a hardcoded 0:
//
//   - none               : fully formatted, `count` bytes written
//   - buffer_too_small   : truncated; `count` is how many bytes were
//                          actually written to `out` (<= out_size) before
//                          truncation was detected. The call still runs
//                          to completion so callers can determine how
//                          much space would have been needed by retrying
//                          with a larger buffer (this is how
//                          `format_string()` grows its buffer internally)
//   - invalid_format     : malformed format string, or a formatter's
//                          spec parse failed (unsupported type character,
//                          bad width/precision, etc.); `count` bytes were
//                          written before the failing field
//   - invalid_argument   : a manual index (e.g. "{5}") was out of range
//                          for the number of arguments passed; `count`
//                          bytes were written before the failing field
//   - index_mode_mismatch : the format string mixed "{}" (auto) and
//                          "{N}" (manual) indexing; `count` bytes were
//                          written before the failing field
//
//
// EXTENDING
// ---------
// To format a custom type `T`, specialize:
//
//   template <typename C>
//   struct vx::fmt::formatter<MyType, C>
//   {
//       constexpr bool parse(const basic_parse_context<C>& ctx) noexcept;
//       constexpr format_error format(basic_format_context<C>& ctx,
//                                      const MyType& value) const noexcept;
//   };
//
// `parse()` typically delegates to `ctx.parse_basic_spec(*this)` or one of
// the basic_*_format_spec variants (string/integer/float) if the spec
// grammar you want matches an existing one.
//
// `format()` does the actual writing, through `basic_format_context`'s
// truncation-safe methods: `push`, `append`, `fill`. Formatters whose
// padding should wrap the entire token (strings, characters, bools) can
// use `ctx.write_padded(spec, data, count, default_align)`, which applies
// fill/alignment/width around the whole token in one call. Numeric
// formatters that need padding to sit *between* a sign/prefix and the
// digits (so `-007` zero-pads between `-` and `7`, not before it) should
// use `ctx.write_numeric_prefix(...)` for the leading portion instead,
// then `append`/`fill` the digit body and any right-side padding
// themselves — see the integer and float formatters below for the
// pattern.
//
// A formatter never owns the buffer it writes into; `basic_format_context`
// can only be constructed by `format_context_creator`, so all writes are
// forced through the truncation-safe path — there's no way for a custom
// formatter to write out of bounds.
//
//
// EXAMPLES
// --------
//     "{}"                  // default formatting
//     "{:s}"                // string
//     "{:>10}"              // right-align, width 10
//     "{:*^10}"             // center-align, `*` fill
//     "{:+d}"               // force integer sign
//     "{:#x}"               // hexadecimal with `0x` prefix
//     "{:08x}"              // zero-padded hexadecimal
//     "{:.3}"               // string limited to 3 characters
//     "{:.2f}"              // floating-point precision 2
//     "{:>12.4e}"           // right-aligned scientific notation
//     "{:A}"                // uppercase hexadecimal floating-point
//     "{:P}"                // uppercase hexadecimal pointer
//==============================================================================

#include "vertex/std/_priv/format_scan_common.hpp"

namespace vx {
namespace fmt {

//==============================================================================
// result types
//==============================================================================

enum class format_error
{
    none,
    buffer_too_small,
    invalid_format,
    invalid_argument,
    index_mode_mismatch
};

struct format_result
{
    format_error err;
    size_t count;
};

//==============================================================================
// formatter
//==============================================================================

template <typename T, typename C, typename Enable = void>
struct formatter
{
    VX_STATIC_ASSERT_MSG((!std::is_same<T, T>::value), "No formatter available");
    static constexpr bool s_not_formattable = true;
};

//==============================================================================
// traits
//==============================================================================

template <typename T, typename C>
struct is_formattable
{
private:

    template <typename U>
    static auto test(int)
        -> decltype(formatter<U, C>::s_not_formattable,
            std::false_type{});

    template <typename>
    static auto test(...)
        -> std::true_type;

public:

    static constexpr bool value =
        decltype(test<T>(0))::value;
};

template <typename T, typename C>
struct is_builtin_formattable
{
private:

    template <typename U>
    static auto test(int) -> decltype(formatter<U, C>::s_is_builtin,
        std::true_type{});

    template <typename>
    static auto test(...) -> std::false_type;

public:

    static constexpr bool value =
        decltype(test<T>(0))::value;
};

//==============================================================================
// format context
//==============================================================================

namespace _fmt_priv {

struct padding_info
{
    size_t left;
    size_t right;
};

// Splits `width - count` spare characters into left/right padding
// according to `align`. Returns {0, 0} if the content already fills
// or exceeds `width` (no padding needed / possible).
inline constexpr padding_info align_padding(
    const size_t width,
    const size_t count,
    const alignment align) noexcept
{
    if (width <= count)
    {
        return { 0, 0 };
    }

    const size_t padding = width - count;

    switch (align)
    {
        default:
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
        {
            return { padding, 0 };
        }
    }
}

// Non-owning cursor over the caller-supplied output range. Every write
// is clamped to `remaining`, so this type can never write past the end
// of the buffer; once a write would overflow, `truncated` latches true
// and all further writes are silently clamped to 0 additional bytes
// beyond whatever fits.
template <typename C>
struct output_buffer
{
    C* ptr;
    size_t remaining;
    bool truncated;

    // True if `n` more characters can be written without truncating.
    constexpr bool reserve(size_t n) noexcept
    {
        return n <= remaining;
    }

    constexpr bool append(const C* s, size_t n) noexcept
    {
        truncated = truncated || (remaining < n);

        const size_t write = truncated ? remaining : n;
        ptr = mem::copy_range(ptr, s, write);
        remaining -= write;

        return !truncated;
    }

    constexpr bool push(C c) noexcept
    {
        return append(&c, 1);
    }

    constexpr bool fill(const C c, size_t n) noexcept
    {
        truncated = truncated || (remaining < n);

        const size_t write = truncated ? remaining : n;
        ptr = mem::fill_range(ptr, write, c);
        remaining -= write;

        return !truncated;
    }

    // Writes `data` padded to `spec.width` using `spec.fill`, aligned by
    // `spec.align` (falling back to `default_align` when the spec didn't
    // specify one). Used by formatters whose padding wraps the *entire*
    // token (strings, chars, bools) — numeric formatters use
    // `write_numeric_prefix` instead so padding can sit between a sign
    // and its digits.
    constexpr format_error write_padded(
        const basic_format_spec<C>& spec,
        const C* data,
        size_t count,
        alignment default_align) noexcept
    {
        const alignment align = (spec.align == alignment::none) ? default_align : spec.align;
        const auto padding = align_padding(spec.width, count, align);

        fill(spec.fill, padding.left);
        append(data, count);
        fill(spec.fill, padding.right);

        return format_error::none;
    }
};

template <typename C>
struct format_context_creator;

} // namespace _fmt_priv

//==============================================================================

// Thin, non-owning view over an `output_buffer<C>`, passed to every
// `formatter<T, C>::format()` call. Can only be constructed by
// `format_context_creator`, so all writes are forced through the
// truncation-safe methods below.
template <typename C>
class basic_format_context
{
private:

    friend _fmt_priv::format_context_creator<C>;

    constexpr explicit basic_format_context(_fmt_priv::output_buffer<C>& out) noexcept
        : m_out(out)
    {}

public:

    constexpr const C* out() const noexcept
    {
        return m_out.ptr;
    }

    constexpr C* out() noexcept
    {
        return m_out.ptr;
    }

    constexpr size_t remaining() const noexcept
    {
        return m_out.remaining;
    }

    constexpr bool empty() const noexcept
    {
        return (m_out.remaining == 0);
    }

    constexpr bool truncated() const noexcept
    {
        return m_out.truncated;
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
        const basic_format_spec<C>& spec,
        const C* data,
        size_t count,
        alignment default_align) noexcept
    {
        return m_out.write_padded(spec, data, count, default_align);
    }

    // Writes the leading portion of a padded numeric token: the embedded
    // sign byte from `data` (if any), then `prefix`, then left padding —
    // as zero-padding (between sign/prefix and digits) if `zero_pad`, or as
    // ordinary `fill` padding before the sign/prefix otherwise. The caller
    // is responsible for writing the digit body and any right-side padding
    // afterward.
    template <typename C>
    constexpr void write_numeric_prefix(
        const basic_integer_format_spec<C>& spec,
        const C* data,
        size_t sign_size,
        const C* prefix,
        size_t prefix_size,
        size_t left_pad) noexcept
    {
        if (spec.zero_pad)
        {
            append(data, sign_size);
            append(prefix, prefix_size);
            fill(C('0'), left_pad);
        }
        else
        {
            fill(spec.fill, left_pad);
            append(data, sign_size);
            append(prefix, prefix_size);
        }
    }

private:

    _fmt_priv::output_buffer<C>& m_out;
};

using format_context = basic_format_context<char>;

//==============================================================================

namespace _fmt_priv {

template <typename C>
struct format_context_creator
{
    static constexpr auto create(output_buffer<C>& out) noexcept
    {
        return basic_format_context<C>{ out };
    }
};

//==============================================================================
// format
//==============================================================================

// Type-erased "format this argument" function pointer. One instantiation
// of `invoke_formatter<Args, C>` is generated per argument type at each
// `format()` call site; together with the erased `const void*` argument
// array this forms a lightweight, allocation-free substitute for a vtable.
template <typename C>
using format_fn =
    format_error (*)(
        const basic_parse_context<C>&,
        basic_format_context<C>&,
        const void*);

template <typename T, typename C>
constexpr format_error invoke_formatter(
    const basic_parse_context<C>& parse_ctx,
    basic_format_context<C>& format_ctx,
    const void* ptr)
{
    // preserves array-ness, e.g. const char[6]
    using U = typename std::remove_reference<T>::type;
    using DT = typename std::decay<T>::type;

    formatter<DT, C> f;
    if (!f.parse(parse_ctx))
    {
        return format_error::invalid_format;
    }

    // reinterpret as the *actual* stored object type, not the decayed one
    const U& ref = *static_cast<const U*>(ptr);
    return f.format(format_ctx, ref);
}

//==============================================================================

// Core formatting loop: walks tokens from `parser`, copies literals/
// escapes straight through, and dispatches replacement fields to
// `funcs[index]`. All error exits fall through to the single return at
// the bottom so that `count` always reflects the number of bytes actually
// written to `out`, regardless of which error (if any) occurred.
template <typename C>
constexpr format_result format_impl(
    C* out,
    size_t out_size,

    const C* fmt,
    size_t fmt_size,

    const size_t argc,
    const void* const* values,
    const format_fn<C>* funcs) noexcept
{
    basic_format_parser<C> parser{ fmt, fmt_size, whitespace_mode::bounded };
    output_buffer<C> buffer{ out, out_size, false };

    size_t next_arg = 0;
    basic_format_token<C> tok;
    format_error err = format_error::none;

    while (true)
    {
        if (!parser.next(tok))
        {
            err = format_error::invalid_format;
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
                buffer.append(tok.first, tok.calculate_size());
                break;
            }
            case token_type::escaped:
            {
                buffer.push(*tok.first);
                break;
            }
            case token_type::replacement:
            {
                if (!parser.update_mode(tok.has_index ? index_mode::manual : index_mode::auto_))
                {
                    err = format_error::index_mode_mismatch;
                    break;
                }

                const size_t index = tok.has_index ? tok.index : next_arg++;
                if (index >= argc)
                {
                    err = format_error::invalid_argument;
                    break;
                }


                auto parse_ctx = parse_context_creator<C>::create(tok.first, tok.calculate_size());
                auto format_ctx = format_context_creator<C>::create(buffer);
                err = funcs[index](parse_ctx, format_ctx, values[index]);
                break;
            }
            case token_type::end:
            {
                VX_UNREACHABLE();
            }
        }

        if (buffer.truncated)
        {
            err = format_error::buffer_too_small;
        }

        if (err != format_error::none)
        {
            break;
        }
    }

    const size_t count = out_size - buffer.remaining;
    return { err, count };
}

template <typename C, typename... Args>
constexpr format_result format_begin(
    C* out,
    size_t out_size,
    const C* fmt,
    size_t fmt_size,
    Args&&... args) noexcept
{
    constexpr size_t argc = sizeof...(Args);
    const array<const void*, argc> values = { &args... };
    const array<format_fn<C>, argc> funcs = { &invoke_formatter<Args, C>... };

    return format_impl<C>(out, out_size, fmt, fmt_size, argc, values.data(), funcs.data());
}

//==============================================================================

template <typename C, typename T>
constexpr format_result format_simple_begin(
    C* out,
    size_t out_size,
    const T& value) noexcept
{
    using DT = typename std::decay<T>::type;
    formatter<DT, C> f;

    // The builtin types don't do any setup in their parse
    // function, so it can be skipped
    VX_IF_CONSTEXPR (!is_builtin_formattable<T, C>::value)
    {
        const C end = closed_brace;
        auto parse_ctx = parse_context_creator<C>::create(&end, 1);
        if (!f.parse(parse_ctx))
        {
            return { format_error::invalid_format, 0 };
        }
    }

    output_buffer<C> buffer{ out, out_size, false };
    auto format_ctx = format_context_creator<C>::create(buffer);

    auto err = f.format(format_ctx, value);
    if (err == format_error::none && buffer.truncated)
    {
        err = format_error::buffer_too_small;
    }

    const size_t count = out_size - buffer.remaining;
    return { err, count };
}

} // namespace _fmt_priv

//==============================================================================
// string like types
//==============================================================================

namespace _fmt_priv {

#if defined VX_FORMAT_ESCAPED_SUPPORT

template <typename C>
constexpr size_t escaped_string_size(const C* data, size_t count) noexcept
{
    size_t total = 2; // quotes
    C buf[4];

    for (size_t i = 0; i < count; ++i)
    {
        total += _fmt_priv::escape_char_body<C>(data[i], C('"'), buf);
    }

    return total;
}

#endif // VX_FORMAT_ESCAPED_SUPPORT

template <typename C>
struct string_formatter : basic_string_format_spec<C>
{
private:

    using base = basic_string_format_spec<C>;

#if defined(VX_FORMAT_ESCAPED_SUPPORT)

    constexpr format_error format_escaped(
        basic_format_context<C>& ctx,
        const C* data,
        size_t count) const noexcept
    {
        const size_t total_size = escaped_string_size(data, count);
        const alignment align = (base::align == alignment::none) ? alignment::left : base::align;
        const auto padding = _fmt_priv::align_padding(base::width, total_size, align);

        ctx.fill(base::fill, padding.left);
        ctx.push(C('"'));

        C buf[4];
        for (size_t i = 0; i < count; ++i)
        {
            const size_t n = _fmt_priv::escape_char_body<C>(data[i], C('"'), buf);
            ctx.append(buf, n);
        }

        ctx.push(C('"'));
        ctx.fill(base::fill, padding.right);

        return format_error::none;
    }

#endif // VX_FORMAT_ESCAPED_SUPPORT

public:

    constexpr bool parse(const basic_parse_context<C>& ctx) noexcept
    {
        if (!ctx.parse_basic_string_spec(*this))
        {
            return false;
        }

        return (base::type == C('\0') || base::type == C('s')
#if defined(VX_FORMAT_ESCAPED_SUPPORT)
            || base::type == C('?')
#endif
            );
    }

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const C* data,
        size_t count) const noexcept
    {
        if (base::has_precision && count > base::precision)
        {
            count = base::precision;
        }

#if defined(VX_FORMAT_ESCAPED_SUPPORT)
        if (base::type == C('?'))
        {
            return format_escaped(ctx, data, count);
        }
#endif

        return ctx.write_padded(*this, data, count, alignment::left);
    }
};

} // namespace _fmt_priv

//==============================================================================
// const char*
//==============================================================================

template <typename C>
struct formatter<const C*, C> : _fmt_priv::string_formatter<C>
{
private:

    using base = _fmt_priv::string_formatter<C>;

    friend is_formattable;
    static constexpr bool s_is_builtin = true;

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
// char array
//==============================================================================

template <typename C, size_t N>
struct formatter<const C[N], C> : _fmt_priv::string_formatter<C>
{
private:

    using base = _fmt_priv::string_formatter<C>;

    friend is_formattable;
    static constexpr bool s_is_builtin = true;

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
// string
//==============================================================================

template <typename S, typename C>
struct formatter<S, C, VX_REQUIRES_TYPE(str::is_string_of<S, C>::value)> : _fmt_priv::string_formatter<C>
{
private:

    using base = _fmt_priv::string_formatter<C>;

    friend is_formattable;
    static constexpr bool s_is_builtin = true;

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
// integer
//==============================================================================

namespace _fmt_priv {

template <typename C>
inline constexpr bool make_integer_format_options(
    const basic_integer_format_spec<C>& spec,
    strconv::integer_to_string_format_options& fmt) noexcept
{
    fmt.force_sign = spec.sign == sign_option::force;
    fmt.uppercase = spec.type == C('X') || spec.type == C('B');
    fmt.base = parse_integer_base(spec.type);
    return (fmt.base != 0);
}

template <typename C>
inline constexpr format_error format_wide_integer_core(
    const basic_integer_format_spec<C>& spec,
    basic_format_context<C>& ctx,
    const C* data,
    const size_t count,
    const bool is_non_negative,
    const size_t base) noexcept
{
    C prefix[3]{};
    size_t prefix_size = 0;
    {
        if (spec.sign == sign_option::space && is_non_negative)
        {
            prefix[prefix_size++] = C(' ');
        }
        if (spec.alternate && base != 10)
        {
            prefix[prefix_size++] = C('0');

            if (base != 8)
            {
                prefix[prefix_size++] = spec.type;
            }
        }
    }

    const size_t total_size = prefix_size + count;
    const alignment align = (spec.align == alignment::none) ? alignment::right : spec.align;
    const auto padding = align_padding(spec.width, total_size, align);

    const bool has_sign = (data[0] == C('+') || data[0] == C('-'));
    const size_t sign_size = static_cast<size_t>(has_sign);
    const C* digits = data + sign_size;
    const size_t body_size = count - sign_size;

    ctx.write_numeric_prefix(
        spec,
        data, sign_size,
        prefix, prefix_size,
        padding.left);

    ctx.append(digits, body_size);
    ctx.fill(spec.fill, padding.right);

    return format_error::none;
}

template <typename I, typename C>
inline constexpr format_error format_wide_integer(
    const basic_integer_format_spec<C>& spec,
    basic_format_context<C>& ctx,
    const I& value) noexcept
{
    using traits = strconv::integer_buffer_traits<I>;

    strconv::integer_to_string_format_options fmt{};
    if (!make_integer_format_options<C>(spec, fmt))
    {
        return format_error::invalid_format;
    }

    constexpr size_t buffer_size = (sizeof(I) * CHAR_BIT) + 1; // +1 for sign
    C buffer[buffer_size];

    const auto res = strconv::to_string<I, C>(value, buffer, buffer_size, fmt);
    VX_ASSERT(res.err == strconv::to_string_error::none);

    return format_wide_integer_core<C>(spec, ctx, buffer, res.count, value >= 0, fmt.base);
}

} // namespace _fmt_priv

template <typename I, typename C>
struct formatter<I, C, VX_REQUIRES_TYPE(std::is_integral<I>::value && !type_traits::is_char<I>::value && !std::is_same<I, bool>::value)> : basic_integer_format_spec<C>
{
private:

    using base = basic_integer_format_spec<C>;

    friend is_formattable;
    static constexpr bool s_is_builtin = true;

public:

    constexpr bool parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_integer_spec(*this);
    }

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const I& value) const noexcept
    {
        using WI = typename std::conditional<std::is_signed<I>::value, int64_t, uint64_t>::type;
        const WI wvalue = static_cast<WI>(value);
        return _fmt_priv::format_wide_integer(*this, ctx, wvalue);
    }
};

//==============================================================================
// character
//==============================================================================

template <typename Cin, typename C>
struct formatter<Cin, C, VX_REQUIRES_TYPE(type_traits::is_char<Cin>::value)> : formatter<typename std::make_unsigned<C>::type, C>
{
private:

    using U = typename std::make_unsigned<C>::type;
    using base = formatter<U, C>;

    friend is_formattable;
    static constexpr bool s_is_builtin = true;

#if defined(VX_FORMAT_ESCAPED_SUPPORT)

    constexpr format_error format_escaped(
        basic_format_context<C>& ctx,
        const C value) const noexcept
    {
        C body[4];
        const size_t body_size = _fmt_priv::escape_char_body<C>(value, C('\''), body);

        const size_t total_size = body_size + 2; // quotes
        const alignment align = (base::align == alignment::none) ? alignment::left : base::align;
        const auto padding = _fmt_priv::align_padding(base::width, total_size, align);

        ctx.fill(base::fill, padding.left);
        ctx.push(C('\''));
        ctx.append(body, body_size);
        ctx.push(C('\''));
        ctx.fill(base::fill, padding.right);

        return format_error::none;
    }

#endif

public:

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const Cin& value) const noexcept
    {
        VX_STATIC_ASSERT_MSG(sizeof(Cin) <= sizeof(C), "Narrowing conversion not allowed");

        switch (base::type)
        {
            case C('\0'):
            case C('c'):
            {
                const C dest_value = static_cast<C>(value);
                return ctx.write_padded(*this, &dest_value, 1, alignment::left);
            }
#if defined(VX_FORMAT_ESCAPED_SUPPORT)
            case C('?'):
            {
                return format_escaped(ctx, static_cast<C>(value));
            }
#endif // VX_FORMAT_ESCAPED_SUPPORT
            default:
            {
                return base::format(ctx, value);
            }
        }
    }
};

//==============================================================================
// bool
//==============================================================================

template <typename C>
struct formatter<bool, C> : formatter<int, C>
{
private:

    using base = formatter<int, C>;

    friend is_formattable;
    static constexpr bool s_is_builtin = true;

public:

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const bool& value) const noexcept
    {
        using boolean_string_t = _fmt_priv::boolean_strings<C>;

        switch (base::type)
        {
            case C('\0'):
            case C('s'):
            {
                if (value)
                {
                    return ctx.write_padded(
                        *this,
                        boolean_string_t::true_str,
                        mem::array_size(boolean_string_t::true_str),
                        alignment::left);
                }
                else
                {
                    return ctx.write_padded(
                        *this,
                        boolean_string_t::false_str,
                        mem::array_size(boolean_string_t::false_str),
                        alignment::left);
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
// float
//==============================================================================

namespace _fmt_priv {

template <typename C>
struct float_precision_info
{
    C exponent = C('e');
    size_t extra_precision = 0;
};

template <typename C>
struct float_format_details
{
    strconv::float_to_string_format_options<C> fmt;
    float_precision_info<C> precision_info;
};

template <typename F>
struct float_buffer_traits
{
    using traits = vx::float_traits<F>;

    // Smallest subnormal is radix^(min_exponent - digits); the number of
    // fractional decimal digits it needs is digits - min_exponent
    // (min_exponent is negative).
    static constexpr size_t max_precision =
        static_cast<size_t>(traits::digits) -
        static_cast<size_t>(traits::min_exponent);

    // +3 for: sign, the ones-place digit not covered by max_exponent10, and
    // the decimal point.
    static constexpr size_t buffer_size =
        max_precision +
        static_cast<size_t>(traits::max_exponent10) + 3;
};

template <typename C>
inline constexpr bool make_float_format_details(
    const basic_float_format_spec<C>& spec,
    float_format_details<C>& details,
    const size_t max_precision) noexcept
{
    details.fmt.force_sign = spec.sign == sign_option::force;
    details.fmt.force_exp_sign = spec.alternate;

    if (spec.has_precision)
    {
        if (spec.precision > max_precision)
        {
            details.fmt.precision = static_cast<uint32_t>(max_precision);
            details.precision_info.extra_precision = spec.precision - max_precision;
        }
        else
        {
            details.fmt.precision = static_cast<uint32_t>(spec.precision);
        }
    }

    const C lower_type = spec.type ? str::to_lower_ascii_unchecked(spec.type) : C('\0');
    const bool is_uppercase = lower_type != spec.type;

    switch (lower_type)
    {
        case C('a'):
        {
            details.precision_info.exponent = C('p');
            VX_FALLTHROUGH;
        }
        case C('\0'):
        case C('g'):
        case C('f'):
        case C('e'):
        {
            break;
        }
        default:
        {
            return false;
        }
    }

    details.fmt.format = static_cast<strconv::float_format>(lower_type);
    details.fmt.uppercase = is_uppercase;

    if (is_uppercase)
    {
        details.precision_info.exponent = str::to_upper_ascii_unchecked(details.precision_info.exponent);
    }

    return true;
}

template <typename C>
inline constexpr format_error format_float_core(
    const basic_float_format_spec<C>& spec,
    basic_format_context<C>& ctx,
    const C* data,
    const size_t count,
    const bool is_negative,
    const float_format_details<C>& details) noexcept
{
    VX_ASSERT(count);
    const bool is_special = !str::is_digit(data[count - 1]);

    C prefix[3]{};
    size_t prefix_size = 0;
    {
        if (spec.sign == sign_option::space && !is_negative)
        {
            prefix[prefix_size++] = C(' ');
        }
        if (!is_special && details.fmt.format == strconv::float_format::hex)
        {
            prefix[prefix_size++] = C('0');
            prefix[prefix_size++] = details.fmt.uppercase ? C('X') : C('x');
        }
    }

    size_t extra_precision_off = count;

    // General format may or may not have an
    // exponent, so we must check just in case
    if (details.fmt.format != strconv::float_format::fixed)
    {
        for (size_t i = count; i > 0; --i)
        {
            if (data[i - 1] == details.precision_info.exponent)
            {
                extra_precision_off = i - 1;
                break;
            }
        }
    }

    const size_t total_size = prefix_size + count + details.precision_info.extra_precision;
    const alignment align = (spec.align == alignment::none) ? alignment::right : spec.align;
    const auto padding = align_padding(spec.width, total_size, align);

    const bool has_sign = (data[0] == C('+') || data[0] == C('-'));
    const size_t sign_size = static_cast<size_t>(has_sign);
    const C* digits = data + sign_size;
    size_t body_size = count - sign_size;

    ctx.write_numeric_prefix(
        spec,
        data, sign_size,
        prefix, prefix_size,
        padding.left);

    if (!is_special && details.precision_info.extra_precision)
    {
        const size_t head_count = extra_precision_off - sign_size;

        ctx.append(digits, head_count);
        ctx.fill(C('0'), details.precision_info.extra_precision);

        body_size -= head_count;
        digits += head_count;
    }

    ctx.append(digits, body_size);
    ctx.fill(spec.fill, padding.right);

    return format_error::none;
}

template <typename F, typename C>
inline constexpr format_error format_float(
    const basic_float_format_spec<C>& spec,
    basic_format_context<C>& ctx,
    const F& value) noexcept
{
    using traits = strconv::float_buffer_traits<F>;

    float_format_details<C> details;
    if (!make_float_format_details<C>(
            spec,
            details,
            traits::max_precision))
    {
        return format_error::invalid_format;
    }

    constexpr size_t buffer_size = traits::buffer_size;
    C buffer[buffer_size];

    const auto res = strconv::to_string<F, C>(value, buffer, buffer_size, details.fmt);
    VX_ASSERT(res.err == strconv::to_string_error::none); // buffer is always large enough

    return format_float_core<C>(spec, ctx, buffer, res.count, signbit(value), details);
}

} // namespace _fmt_priv

template <typename F, typename C>
struct formatter<F, C, VX_REQUIRES_TYPE(std::is_floating_point<F>::value)> : basic_float_format_spec<C>
{
private:

    using base = basic_float_format_spec<C>;

public:

    constexpr bool parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_float_spec(*this);
    }

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const F& value) const noexcept
    {
        return _fmt_priv::format_float<F, C>(*this, ctx, value);
    }
};

//==============================================================================
// pointer
//==============================================================================

template <typename C>
struct formatter<void*, C> : basic_format_spec<C>
{
private:

    using base = basic_format_spec<C>;

public:

    constexpr bool parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_spec(*this);
    }

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const void* value) const noexcept
    {
        strconv::integer_to_string_format_options fmt{};
        fmt.base = 16;
        fmt.uppercase = (base::type == C('P'));

        switch (base::type)
        {
            case C('\0'):
            case C('p'):
            case C('P'):
            {
                break;
            }
            default:
            {
                return format_error::invalid_format;
            }
        }

        const uintptr_t addr = reinterpret_cast<uintptr_t>(value);

        constexpr size_t data_size = sizeof(uintptr_t) * 2; // 2 hex digits per byte, no sign
        C data[data_size];

        const auto res = strconv::to_string<uintptr_t, C>(addr, data, data_size, fmt);
        VX_ASSERT(res.err == strconv::to_string_error::none); // buffer is always large enough

        const C prefix[] = { C('0'), fmt.uppercase ? C('X') : C('x') };
        constexpr size_t prefix_size = mem::array_size(prefix);

        const size_t total_size = res.count + prefix_size;
        const auto padding = _fmt_priv::align_padding(base::width, total_size, base::align);

        ctx.fill(base::fill, padding.left);
        ctx.append(prefix, prefix_size);
        ctx.append(data, res.count);
        ctx.fill(base::fill, padding.right);

        return format_error::none;
    }
};

//==============================================================================

template <typename T, typename C>
struct formatter<T*, C, VX_REQUIRES_TYPE(!type_traits::is_char<T>::value)> : formatter<void*, C>
{};

//==============================================================================

template <typename C>
struct formatter<std::nullptr_t, C> : formatter<void*, C>
{};

//==============================================================================
// format
//==============================================================================

//------------------------------------------------------------------------------
// Format a single value into a caller-provided buffer
//------------------------------------------------------------------------------

template <
    typename C,
    typename T,
    VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr format_result format_simple(
    C* out,
    size_t out_size,
    const T& value) noexcept
{
    return _fmt_priv::format_simple_begin(
        out,
        out_size,
        value);
}

//------------------------------------------------------------------------------
// Format into a caller-provided buffer
//------------------------------------------------------------------------------

template <
    typename C,
    typename... Args,
    VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr format_result format(
    C* out,
    size_t out_size,
    const C* fmt,
    size_t fmt_size,
    Args&&... args) noexcept
{
    return _fmt_priv::format_begin(
        out,
        out_size,
        fmt,
        fmt_size,
        std::forward<Args>(args)...);
}

template <
    typename C,
    typename... Args,
    VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr format_result format(
    C* out,
    size_t out_size,
    str::basic_string_view<C> fmt,
    Args&&... args) noexcept
{
    return format(
        out,
        out_size,
        fmt.data(),
        fmt.size(),
        std::forward<Args>(args)...);
}

template <
    typename C,
    size_t N,
    typename... Args,
    VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr format_result format(
    C* out,
    size_t out_size,
    const C (&fmt)[N],
    Args&&... args) noexcept
{
    static_assert(N > 0, "Format string must not be empty.");

    return format(
        out,
        out_size,
        fmt,
        N - 1,
        std::forward<Args>(args)...);
}

template <
    typename C,
    size_t OutN,
    typename... Args,
    VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr format_result format(
    C (&out)[OutN],
    str::basic_string_view<C> fmt,
    Args&&... args) noexcept
{
    return format(
        out,
        OutN,
        fmt,
        std::forward<Args>(args)...);
}

template <
    typename C,
    size_t OutN,
    size_t FmtN,
    typename... Args,
    VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr format_result format(
    C (&out)[OutN],
    const C (&fmt)[FmtN],
    Args&&... args) noexcept
{
    return format(
        out,
        OutN,
        fmt,
        std::forward<Args>(args)...);
}

//------------------------------------------------------------------------------
// Format a single value into an existing string
//------------------------------------------------------------------------------

template <
    typename S,
    typename T,
    VX_REQUIRES(str::is_mutable_string_like<S>::value)>
format_result format_simple(
    S& out,
    const T& value)
{
    if (out.empty())
    {
        out.resize(64);
    }

    for (;;)
    {
        const auto res = format_simple(
            out.data(),
            out.size(),
            value);

        if (res.err != format_error::buffer_too_small)
        {
            out.resize(res.count);
            return res;
        }

        VX_ASSERT(out.size());
        out.resize(out.size() * 2);
    }
}

//------------------------------------------------------------------------------
// Format into an existing string
//------------------------------------------------------------------------------

template <
    typename FMT,
    typename S,
    typename... Args,
    VX_REQUIRES(
        str::is_string_like<FMT>::value&&
            str::is_mutable_string_like<S>::value&&
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

        if (res.err != format_error::buffer_too_small)
        {
            out.resize(res.count);
            return res;
        }

        VX_ASSERT(out.size());
        out.resize(out.size() * 2);
    }
}

template <
    typename C,
    typename S,
    typename... Args,
    VX_REQUIRES(
        type_traits::is_char<C>::value&&
            str::is_mutable_string_like<S>::value&&
                str::is_string_of<S, C>::value)>
format_result format_string(
    const C* fmt,
    size_t fmt_size,
    S& out,
    Args&&... args)
{
    return format_string(
        str::basic_string_view<C>(fmt, fmt_size),
        out,
        std::forward<Args>(args)...);
}

template <
    typename C,
    size_t N,
    typename S,
    typename... Args,
    VX_REQUIRES(
        type_traits::is_char<C>::value&&
            str::is_mutable_string_like<S>::value&&
                str::is_string_of<S, C>::value)>
format_result format_string(
    const C (&fmt)[N],
    S& out,
    Args&&... args)
{
    VX_STATIC_ASSERT_MSG(N > 0, "Format string must not be empty.");

    return format_string(
        str::basic_string_view<C>(fmt, N - 1),
        out,
        std::forward<Args>(args)...);
}

//------------------------------------------------------------------------------
// Format a single value into a newly-created string
//------------------------------------------------------------------------------

template <typename T>
str::basic_string<char> format_simple(const T& value)
{
    str::basic_string<char> out;

    format_simple(
        out,
        value);

    return out;
}

template <
    typename C,
    typename T,
    VX_REQUIRES(type_traits::is_char<C>::value)>
str::basic_string<C> format_simple_as(const T& value)
{
    str::basic_string<C> out;

    format_simple(
        out,
        value);

    return out;
}

//------------------------------------------------------------------------------
// Format into a newly-created string
//------------------------------------------------------------------------------

template <
    typename S,
    typename... Args,
    VX_REQUIRES(str::is_string_like<S>::value)>
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

template <
    typename C,
    size_t N,
    typename... Args,
    VX_REQUIRES(type_traits::is_char<C>::value)>
str::basic_string<C> format(
    const C (&fmt)[N],
    Args&&... args)
{
    str::basic_string<C> out;

    format_string(
        str::basic_string_view<C>(fmt, N - 1),
        out,
        std::forward<Args>(args)...);

    return out;
}

template <
    typename C,
    typename... Args,
    VX_REQUIRES(type_traits::is_char<C>::value)>
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

} // namespace fmt
} // namespace vx
