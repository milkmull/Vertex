#pragma once

#include <cstddef>
#include <type_traits>

#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/iterator.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/utf.hpp"

namespace vx {
namespace str {

//==============================================================================
// string_cast_size
//==============================================================================
// Number of to_char_t units string_cast would write for [src, src + src_size).
// Does not include a terminator. Invalid sequences count as one unit (the
// replacement), matching what the cast functions emit.
//==============================================================================

/**
 * @brief Counts the units needed to convert an iterator range.
 *
 * This is the implementation; the pointer + size overload funnels here.
 * Requires a multipass (forward or better) iterator, since the range is
 * walked once here and again by the conversion.
 *
 * @tparam to_char_t Destination character type.
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the input range.
 * @param last Iterator to the end of the input range.
 * @return Number of to_char_t units the conversion produces, excluding any terminator.
 */
template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char_iter<IT>::value)>
size_t string_cast_size(IT first, IT last) noexcept
{
    using from_char_t = typename type_traits::value_type<IT>::type;
    using encoder = utf::utf_traits<to_char_t>;
    using decoder = utf::utf_traits<from_char_t>;

    size_t total = 0;

    while (first != last)
    {
        utf::code_point c{};
        first = decoder::decode(first, last, c);
        total += utf::is_valid_codepoint(c) ? encoder::width(c) : 1;
    }

    return total;
}

/**
 * @brief Counts the units needed to convert a range to another encoding.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @param src Pointer to the beginning of the input range (may be null if src_size is 0).
 * @param src_size Number of characters in the input range.
 * @return Number of to_char_t units the conversion produces, excluding any terminator.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
size_t string_cast_size(const from_char_t* src, size_t src_size) noexcept
{
    return string_cast_size<to_char_t>(src, src + src_size);
}

/**
 * @brief Counts the units needed to convert an iterator range, assuming it is pure ASCII.
 *
 * This is the implementation; the pointer + size overload funnels here.
 * Precondition: every unit in the range is < 0x80. Conversion is then 1:1, so
 * the result is simply the range length. If the precondition is violated, the
 * result is still the range length and is consistent with
 * string_cast_assume_ascii, but not with the general string_cast.
 *
 * @tparam to_char_t Destination character type (unused, kept for symmetry).
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the input range.
 * @param last Iterator to the end of the input range.
 * @return std::distance(first, last).
 */
template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char_iter<IT>::value)>
constexpr size_t string_cast_size_assume_ascii(IT first, IT last) noexcept
{
#if VX_DEBUG

    using utype = typename utf::utf_traits<typename type_traits::value_type<IT>::type>::utype;
    for (IT it = first; it != last; ++it)
    {
        VX_ASSERT(static_cast<utype>(*it) < 0x80);
    }

#endif // VX_DEBUG

    return static_cast<size_t>(vx::distance(first, last));
}

/**
 * @brief Counts the units needed to convert a range, assuming it is pure ASCII.
 *
 * Precondition: every unit in the range is < 0x80. See the iterator overload.
 *
 * @tparam to_char_t Destination character type (unused, kept for symmetry).
 * @tparam from_char_t Source character type.
 * @param src Pointer to the beginning of the input range (may be null if src_size is 0).
 * @param src_size Number of characters in the input range.
 * @return src_size.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
constexpr size_t string_cast_size_assume_ascii(const from_char_t* src, size_t src_size) noexcept
{
    return string_cast_size_assume_ascii<to_char_t>(src, src + src_size);
}

//==============================================================================
// char_cast
//==============================================================================
// Converts one encoded character (one code point). A different job from the
// string_cast family, so it stays its own small primitive.
//==============================================================================

/**
 * @brief Converts a single encoded character from one encoding to another.
 *
 * Precondition: first != last. Writes between 1 and encoder::max_width()
 * units to out; the caller must guarantee that much space. An invalid or
 * truncated sequence writes one replacement unit and still consumes at least
 * one source unit, so loops calling this always make progress.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @param first Reference to the current input pointer. Advanced past the consumed units.
 * @param last Pointer past the end of the input range.
 * @param out Output pointer.
 * @param replacement Replacement unit for invalid sequences.
 * @return Pointer past the last written output unit.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
to_char_t* char_cast(
    const from_char_t*& first,
    const from_char_t* last,
    to_char_t* out,
    to_char_t replacement = to_char_t('?')) noexcept
{
    using encoder = utf::utf_traits<to_char_t>;
    using decoder = utf::utf_traits<from_char_t>;

    utf::code_point c{};
    first = decoder::decode(first, last, c);

    return encoder::encode(c, out, replacement);
}

/**
 * @brief Converts a single ASCII character to another encoding.
 *
 * Precondition: first != last and *first < 0x80. Consumes exactly one source
 * unit and writes exactly one output unit, with no decode or validation.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @param first Reference to the current input pointer. Advanced by one.
 * @param out Output pointer; must have room for one unit.
 * @return out + 1.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
to_char_t* char_cast_assume_ascii(const from_char_t*& first, to_char_t* out) noexcept
{
    using utype = typename utf::utf_traits<from_char_t>::utype;

    VX_ASSERT(static_cast<utype>(*first) < 0x80);

    // go through utype first so a signed char never sign-extends
    *out = static_cast<to_char_t>(static_cast<utype>(*first));
    ++first;
    return out + 1;
}

//==============================================================================
// string_cast (core #1): bounded, contiguous
//==============================================================================
// Every higher-level string_cast form reduces to this (ptr + size in, ptr + capacity out).
// Never writes past dst_size and never writes a partial code point. Does not
// write a terminator; the caller owns that (size with string_cast_size, allocate +1).
//==============================================================================

/**
 * @brief Result of a bounded conversion.
 */
template <typename from_char_t, typename to_char_t>
struct string_cast_result
{
    const from_char_t* src_end; ///< First unconsumed source unit.
    to_char_t* dst_end;         ///< One past the last written output unit.
    bool complete;              ///< True if the whole source was consumed.

    explicit operator bool() const noexcept
    {
        return complete;
    }
};

/**
 * @brief Converts [src, src + src_size) into a buffer of dst_size units.
 *
 * Invalid sequences become one `replacement` unit. Stops early, with
 * complete == false, if the next code point's encoding doesn't fit.
 *
 * @param src Source pointer (may be null if src_size is 0).
 * @param src_size Number of source units.
 * @param dst Destination pointer (may be null if dst_size is 0).
 * @param dst_size Destination capacity in to_char_t units.
 * @param replacement Unit written for invalid sequences.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
string_cast_result<from_char_t, to_char_t> string_cast(
    const from_char_t* src,
    size_t src_size,
    to_char_t* dst,
    size_t dst_size,
    to_char_t replacement = to_char_t('?')) noexcept
{
    using encoder = utf::utf_traits<to_char_t>;
    using decoder = utf::utf_traits<from_char_t>;

    const from_char_t* first = src;
    const from_char_t* const last = src + src_size;
    to_char_t* out = dst;
    to_char_t* const out_end = dst + dst_size;

    to_char_t scratch[encoder::max_width()];

    while (first != last)
    {
        const size_t remaining = static_cast<size_t>(out_end - out);

        if (remaining >= encoder::max_width())
        {
            // worst case fits: encode straight into dst
            utf::code_point c{};
            first = decoder::decode(first, last, c);
            out = encoder::encode(c, out, replacement);
            continue;
        }

        // tail: measure through scratch so a partial code point is never written
        const from_char_t* next = first;
        utf::code_point c{};
        next = decoder::decode(next, last, c);

        const size_t n = static_cast<size_t>(encoder::encode(c, scratch, replacement) - scratch);
        if (n > remaining)
        {
            break;
        }

        for (size_t i = 0; i < n; ++i)
        {
            out[i] = scratch[i];
        }

        out += n;
        first = next;
    }

    return { first, out, first == last };
}

/**
 * @brief Converts a range assuming it is pure ASCII (every unit < 0x80).
 *
 * Conversion is 1:1, so this is a straight widening/narrowing copy with no
 * decode, validation, or replacement. Writes min(src_size, dst_size) units.
 *
 * Contract violations are memory-safe: the copy is still bounded by dst_size,
 * and non-ASCII units are copied as their truncated value (wrong output, never
 * an overrun). The result matches string_cast_size_assume_ascii. A debug build
 * asserts on violations.
 *
 * @param src Source pointer (may be null if src_size is 0).
 * @param src_size Number of source units.
 * @param dst Destination pointer (may be null if dst_size is 0).
 * @param dst_size Destination capacity in to_char_t units.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
string_cast_result<from_char_t, to_char_t> string_cast_assume_ascii(
    const from_char_t* src,
    size_t src_size,
    to_char_t* dst,
    size_t dst_size) noexcept
{
    using utype = typename utf::utf_traits<from_char_t>::utype;
    const size_t n = src_size < dst_size ? src_size : dst_size;

    for (size_t i = 0; i < n; ++i)
    {
        const utype u = static_cast<utype>(src[i]);
        VX_ASSERT(u < 0x80);
        dst[i] = static_cast<to_char_t>(u);
    }

    return { src + n, dst + n, n == src_size };
}

//==============================================================================
// string_cast (core #2): unbounded, iterator based
//==============================================================================
// Converts an entire [first, last) range, writing through an output iterator
// until the source is exhausted. No bound is placed on the output side - safe
// for a growing container via back_inserter, NOT safe for a fixed-capacity
// buffer. Use the bounded core above for a fixed-size destination.
//
// IT2 is constrained to be a non-character type so that a call like
// string_cast<char>(it, it, '?') resolves to the string-returning overload
// (replacement) rather than treating '?' as an output iterator.
//==============================================================================

/**
 * @brief Converts a character range from one encoding to another.
 *
 * @tparam to_char_t Destination character type.
 * @tparam IT1 Input iterator type.
 * @tparam IT2 Output iterator type.
 * @param first Iterator to the beginning of the input range.
 * @param last Iterator to the end of the input range.
 * @param out Output iterator to write converted characters.
 * @param replacement Replacement character to use for invalid sequences.
 * @return Output iterator after the last written character.
 */
template <typename to_char_t, typename IT1, typename IT2, VX_REQUIRES(type_traits::is_char_iter<IT1>::value && !type_traits::is_char<IT2>::value)>
IT2 string_cast(IT1 first, IT1 last, IT2 out, to_char_t replacement = to_char_t('?'))
{
    VX_ITERATOR_VALID_RANGE(first, last);

    using from_char_t = typename type_traits::value_type<IT1>::type;
    using encoder = utf::utf_traits<to_char_t>;
    using decoder = utf::utf_traits<from_char_t>;

    while (first != last)
    {
        utf::code_point c{};
        first = decoder::decode(first, last, c);
        out = encoder::encode(c, out, replacement);
    }

    return out;
}

//==============================================================================
// string_cast (string-returning overloads)
//==============================================================================
// Every contiguous input reduces to (ptr, size) and goes through
// string_cast_size + string_cast (core #1), so there is one allocation and no
// growth. Non-contiguous iterators use the unbounded back_inserter path.
//==============================================================================

namespace _string_cast_priv {

template <typename to_char_t, typename from_char_t>
basic_string<to_char_t> cast_to_string(const from_char_t* src, size_t n, to_char_t replacement)
{
    const size_t out_size = string_cast_size<to_char_t>(src, n);

    basic_string<to_char_t> res;
    res.resize(out_size);

    string_cast<to_char_t>(src, n, res.data(), out_size, replacement);
    return res;
}

template <typename to_char_t, typename from_char_t>
basic_string<to_char_t> cast_to_string_assume_ascii(const from_char_t* src, size_t n)
{
    basic_string<to_char_t> res;
    res.resize(string_cast_size_assume_ascii<to_char_t>(src, n));

    string_cast_assume_ascii<to_char_t>(src, n, res.data(), res.size());
    return res;
}

// size argument: any integer type that is not itself a character type, so that
// string_cast<char>(ptr, 5) means "size 5" and string_cast<char>(ptr, '?') means
// "null-terminated, replacement '?'"
template <typename T>
using is_size_arg = std::bool_constant<std::is_integral<T>::value && !type_traits::is_char<T>::value>;

} // namespace _string_cast_priv

//-- pointer + size ------------------------------------------------------------

template <typename to_char_t, typename from_char_t, typename size_t_like, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value&& _string_cast_priv::is_size_arg<size_t_like>::value)>
auto string_cast(const from_char_t* src, size_t_like size, to_char_t replacement = to_char_t('?'))
{
    return _string_cast_priv::cast_to_string<to_char_t>(src, static_cast<size_t>(size), replacement);
}

template <typename to_char_t, typename from_char_t, typename size_t_like, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value&& _string_cast_priv::is_size_arg<size_t_like>::value)>
auto string_cast_assume_ascii(const from_char_t* src, size_t_like size)
{
    return _string_cast_priv::cast_to_string_assume_ascii<to_char_t>(src, static_cast<size_t>(size));
}

//-- pointer range -------------------------------------------------------------

template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
auto string_cast(const from_char_t* first, const from_char_t* last, to_char_t replacement = to_char_t('?'))
{
    VX_ITERATOR_VALID_RANGE(first, last);
    return _string_cast_priv::cast_to_string<to_char_t>(first, static_cast<size_t>(last - first), replacement);
}

template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
auto string_cast_assume_ascii(const from_char_t* first, const from_char_t* last)
{
    VX_ITERATOR_VALID_RANGE(first, last);
    return _string_cast_priv::cast_to_string_assume_ascii<to_char_t>(first, static_cast<size_t>(last - first));
}

//-- null-terminated -----------------------------------------------------------

template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
auto string_cast(const from_char_t* cstr, to_char_t replacement = to_char_t('?'))
{
    using traits = str::char_traits<from_char_t>;
    return _string_cast_priv::cast_to_string<to_char_t>(cstr, cstr ? traits::length(cstr) : 0, replacement);
}

template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
auto string_cast_assume_ascii(const from_char_t* cstr)
{
    using traits = str::char_traits<from_char_t>;
    return _string_cast_priv::cast_to_string_assume_ascii<to_char_t>(cstr, cstr ? traits::length(cstr) : 0);
}

//-- string-like (anything contiguous with data() and size()) -----------------

template <typename to_char_t, typename S, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& is_string_like<S>::value)>
auto string_cast(const S& s, to_char_t replacement = to_char_t('?'))
{
    return _string_cast_priv::cast_to_string<to_char_t>(s.data(), s.size(), replacement);
}

template <typename to_char_t, typename S, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& is_string_like<S>::value)>
auto string_cast_assume_ascii(const S& s)
{
    return _string_cast_priv::cast_to_string_assume_ascii<to_char_t>(s.data(), s.size());
}

//-- generic iterators (slow path, not contiguous) ----------------------------

template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iter<IT>::value && !std::is_pointer<IT>::value)>
auto string_cast(IT first, IT last, to_char_t replacement = to_char_t('?'))
{
    basic_string<to_char_t> res;
    res.reserve(string_cast_size<to_char_t>(first, last)); // exact: one allocation, no regrowth
    string_cast<to_char_t>(first, last, back_inserter(res), replacement);
    return res;
}

template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iter<IT>::value && !std::is_pointer<IT>::value)>
auto string_cast_assume_ascii(IT first, IT last)
{
    using utype = typename utf::utf_traits<typename type_traits::value_type<IT>::type>::utype;

    basic_string<to_char_t> res;
    res.reserve(static_cast<size_t>(std::distance(first, last))); // exact for 1:1

    for (; first != last; ++first)
    {
        const utype u = static_cast<utype>(*first);
        VX_ASSERT(u < 0x80);
        res.push_back(static_cast<to_char_t>(u));
    }
    return res;
}

} // namespace str
} // namespace vx
