#pragma once

#include "vertex/std/_priv/format_scan_common.hpp"

namespace vx {
namespace fmt {

// #define VX_FORMAT_DISABLE_RUNTIME_FORMAT_CHECKS
// #define VX_FORMAT_DISABLE_OUTPUT_SIZE_CHECKS

#if !defined(VX_FORMAT_DISABLE_OUTPUT_SIZE_CHECKS)
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
        return n < remaining;
    }

    constexpr bool push(C c) noexcept
    {
        if (remaining == 0)
        {
            return false;
        }

        *ptr++ = c;
        --remaining;
        return true;
    }

    constexpr bool append(const C* s, size_t n) noexcept
    {
        const size_t write = (remaining < n) ? remaining : n;
        ptr = mem::copy_range(ptr, s, write);
        remaining -= write;

        return (write == n);
    }

    constexpr bool fill(const C c, size_t n) noexcept
    {
        const size_t write = (remaining < n) ? remaining : n;
        ptr = mem::fill_range(ptr, write, c);
        remaining -= write;

        return (write == n);
    }

    constexpr format_error write_padded(
        const basic_format_spec<C>& spec,
        const C* data,
        size_t count,
        alignment default_align) noexcept
    {
        const alignment align = (spec.align == alignment::none) ? default_align : spec.align;
        const auto padding = align_padding(spec.width, count, align);

        bool truncated = false;
        truncated = truncated || !fill(spec.fill, padding.left);
        truncated = truncated || !append(data, count);
        truncated = truncated || !fill(spec.fill, padding.right);

        return truncated
            ? format_error::buffer_too_small
            : format_error::none;
    }
};

template <typename C>
struct format_context_creator;

} // namespace _fmt_priv

//==============================================================================

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
                    const size_t count = out_size - buffer.remaining;
                    return { format_error::buffer_too_small, count };
                }

                break;
            }
            case token_type::escaped:
            {
                if (!buffer.push(*tok.first))
                {
                    const size_t count = out_size - buffer.remaining;
                    return { format_error::buffer_too_small, count };
                }

                break;
            }
            case token_type::replacement:
            {
                const bool update_mode_check = parser.update_mode(tok.has_index ? index_mode::manual : index_mode::auto_);
                _FORMAT_RET_IF(!update_mode_check, (format_result{ format_error::index_mode_mismatch, 0 }));

                const size_t index = (tok.has_index)
                    ? tok.index
                    : next_arg++;

                _FORMAT_RET_IF((index >= argc), (format_result{ format_error::invalid_argument, 0 }));

                auto parse_ctx = parse_context_creator<C>::create(tok.first, tok.calculate_size());
                auto format_ctx = format_context_creator<C>::create(buffer);
                const auto fmt_err = funcs[index](parse_ctx, format_ctx, values[index]);
                if (fmt_err != format_error::none)
                {
                    const size_t count = out_size - buffer.remaining;
                    return { fmt_err, count };
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

} // namespace _fmt_priv

//==============================================================================
// string like types
//==============================================================================

namespace _fmt_priv {

template <typename C>
struct string_formatter : basic_string_format_spec<C>
{
private:

    using base = basic_string_format_spec<C>;

public:

    constexpr bool parse(const basic_parse_context<C>& ctx) noexcept
    {
        return ctx.parse_basic_string_spec(*this);
    }

    constexpr format_error format(
        basic_format_context<C>& ctx,
        const C* data,
        size_t count) const noexcept
    {
        _FORMAT_RET_IF((base::type != 0 && base::type != C('s')), format_error::invalid_format);

        if (base::has_precision && count > base::precision)
        {
            count = base::precision;
        }

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
inline constexpr strconv::integer_to_string_format_options make_integer_format_options(
    const basic_integer_format_spec<C>& spec,
    format_error& err) noexcept
{
    strconv::integer_to_string_format_options fmt{};
    fmt.force_sign = spec.sign == sign_option::force;
    fmt.uppercase = spec.type == C('X') || spec.type == C('B');
    fmt.base = parse_integer_base(spec.type);
    err = (fmt.base == 0) ? format_error::invalid_format : format_error::none;
    return fmt;
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

    bool truncated = false;

    const bool has_sign = (data[0] == C('+') || data[0] == C('-'));
    const size_t sign_size = static_cast<size_t>(has_sign);
    const C* digits = data + sign_size;
    size_t body_size = count - sign_size;

    if (spec.zero_pad)
    {
        truncated = truncated || !ctx.append(data, sign_size);
        truncated = truncated || !ctx.append(prefix, prefix_size);
        truncated = truncated || !ctx.fill(C('0'), padding.left);
    }
    else
    {
        truncated = truncated || !ctx.fill(spec.fill, padding.left);
        truncated = truncated || !ctx.append(data, sign_size);
        truncated = truncated || !ctx.append(prefix, prefix_size);
    }

    truncated = truncated || !ctx.append(digits, body_size);
    truncated = truncated || !ctx.fill(spec.fill, padding.right);

    return truncated
        ? format_error::buffer_too_small
        : format_error::none;
}

template <typename I, typename C>
inline constexpr format_error format_wide_integer(
    const basic_integer_format_spec<C>& spec,
    basic_format_context<C>& ctx,
    const I& value) noexcept
{
    format_error err = format_error::none;
    const auto fmt = make_integer_format_options<C>(spec, err);
    _FORMAT_RET_IF((err != format_error::none), err);

    constexpr size_t buffer_size = 65; // sign + 64 bits
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

    using base = formatter<typename std::make_unsigned<C>::type, C>;

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
inline constexpr format_error make_float_format_details(
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
            details.precision_info.extra_precision = max_precision - spec.precision;
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
            return format_error::invalid_format;
        }
    }

    details.fmt.format = static_cast<strconv::float_format>(lower_type);
    details.fmt.uppercase = is_uppercase;

    if (is_uppercase)
    {
        details.precision_info.exponent = str::to_upper_ascii_unchecked(details.precision_info.exponent);
    }

    return format_error::none;
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
    C prefix[3]{};
    size_t prefix_size = 0;
    {
        if (spec.sign == sign_option::space && !is_negative)
        {
            prefix[prefix_size++] = C(' ');
        }
        if (details.fmt.format == strconv::float_format::hex)
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

    bool truncated = false;

    const bool has_sign = (data[0] == C('+') || data[0] == C('-'));
    const size_t sign_size = static_cast<size_t>(has_sign);
    const C* digits = data + sign_size;
    size_t body_size = count - sign_size;

    if (spec.zero_pad)
    {
        truncated = truncated || !ctx.append(data, sign_size);
        truncated = truncated || !ctx.append(prefix, prefix_size);
        truncated = truncated || !ctx.fill(C('0'), padding.left);
    }
    else
    {
        truncated = truncated || !ctx.fill(spec.fill, padding.left);
        truncated = truncated || !ctx.append(data, sign_size);
        truncated = truncated || !ctx.append(prefix, prefix_size);
    }

    if (details.precision_info.extra_precision)
    {
        const size_t head_count = extra_precision_off - sign_size;

        truncated = truncated || !ctx.append(digits, head_count);
        truncated = truncated || !ctx.fill(C('0'), details.precision_info.extra_precision);

        body_size -= head_count;
        digits += details.precision_info.extra_precision;
    }

    truncated = truncated || !ctx.append(digits, body_size);
    truncated = truncated || !ctx.fill(spec.fill, padding.right);

    return truncated
        ? format_error::buffer_too_small
        : format_error::none;
}

template <typename F, typename C>
inline constexpr format_error format_float(
    const basic_float_format_spec<C>& spec,
    basic_format_context<C>& ctx,
    const F& value) noexcept
{
    using traits = float_buffer_traits<F>;

    float_format_details<C> details;
    const format_error opt_err = make_float_format_details<C>(
        spec,
        details,
        traits::max_precision);

    _FORMAT_RET_IF((opt_err != format_error::none), opt_err);

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
        _FORMAT_RET_IF((base::type != 0 && base::type != C('p')), format_error::invalid_format);
        
        strconv::integer_to_string_format_options fmt{};
        fmt.base = 16;
        
        const uintptr_t addr = reinterpret_cast<uintptr_t>(value);
        
        constexpr size_t data_size = sizeof(uintptr_t) * 2; // 2 hex digits per byte, no sign
        C data[data_size];
        
        const auto res = strconv::to_string<uintptr_t, C>(addr, data, data_size, fmt);
        VX_ASSERT(res.err == strconv::to_string_error::none); // buffer is always large enough

        constexpr C prefix[] = { C('0'), C('x') };
        constexpr size_t prefix_size = mem::array_size(prefix);
        
        const size_t total_size = res.count + prefix_size;
        const auto padding = _fmt_priv::align_padding(base::width, total_size, base::align);

        bool truncated = false;
        truncated = truncated || !ctx.fill(base::fill, padding.left);
        truncated = truncated || !ctx.append(prefix, prefix_size);
        truncated = truncated || !ctx.append(data, res.count);
        truncated = truncated || !ctx.fill(base::fill, padding.right);

        return truncated
            ? format_error::buffer_too_small
            : format_error::none;
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
    return _fmt_priv::format_begin(
        out,
        out_size,
        fmt,
        fmt_size,
        std::forward<Args>(args)...);
}

template <typename FMT, typename S, typename... Args, VX_REQUIRES(str::is_string_like<FMT>::value&& str::is_mutable_string_like<S>::value&& str::is_string_compatible<FMT, S>::value)>
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

template <typename C, typename S, typename... Args, VX_REQUIRES(type_traits::is_char<C>::value&& str::is_mutable_string_like<S>::value&& str::is_string_of<S, C>::value)>
format_result format_string(
    const C* fmt,
    const size_t fmt_size,
    S& out,
    Args&&... args)
{
    return format_string(
        str::basic_string_view<C>(fmt, fmt_size),
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

} // namespace fmt
} // namespace vx
