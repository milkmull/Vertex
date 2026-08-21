#pragma once

#include "vertex/config/type_traits.hpp"
#include "vertex/std/iterator.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/utf.hpp"

namespace vx {
namespace str {

//==============================================================================
// char_cast
//==============================================================================
// Converts a single encoded character. Not part of the string_cast family
// below - a different job (one code point vs a whole range) - left as its
// own small primitive rather than folded in.
//==============================================================================

/**
 * @brief Converts a single encoded character from one encoding to another.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @param first Reference to the current input pointer. Advanced after decoding.
 * @param last Pointer past the end of the input range.
 * @param out Output pointer.
 * @param replacement Replacement character for invalid sequences.
 * @return Pointer after the last written output character.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
to_char_t* char_cast(
    const from_char_t*& first,
    const from_char_t* last,
    to_char_t* out,
    to_char_t replacement = to_char_t('?'))
{
    using encoder = utf::utf_traits<to_char_t>;
    using decoder = utf::utf_traits<from_char_t>;

    utf::code_point c{};
    first = decoder::decode(first, last, c);

    return encoder::encode(c, out, replacement);
}

//==============================================================================
// string_cast - core #1: unbounded
//==============================================================================
// Converts an entire [first, last) range, writing through an output iterator
// until the source is exhausted. No bound is placed on the output side - safe
// for a growing container via back_inserter, NOT safe for a fixed-capacity
// buffer (a raw pointer is a valid output iterator here, but nothing stops
// this from writing past its capacity). Use the bounded overload below for
// a fixed-size destination.
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
template <typename to_char_t, typename IT1, typename IT2, VX_REQUIRES(type_traits::is_char_iter<IT1>::value)>
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
// string_cast - core #2: bounded
//==============================================================================
// The only overload safe to use with a fixed-capacity destination buffer.
// Never writes past dst_size, even if that means stopping before the whole
// source is consumed. A code point is only ever written whole - if its
// encoded form would not fully fit in the remaining space, conversion stops
// before it rather than emitting a partial/corrupt sequence.
//==============================================================================

/**
 * @brief Converts a character range from one encoding to another, writing
 * into a caller-provided buffer of limited capacity.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @param src Pointer to the beginning of the input range.
 * @param src_size Number of characters in the input range.
 * @param dst Pointer to the output buffer.
 * @param dst_size Capacity, in to_char_t elements, of the output buffer.
 * @param replacement Replacement character to use for invalid sequences.
 * @return Pointer after the last written output character.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
to_char_t* string_cast(
    const from_char_t* src,
    size_t src_size,
    to_char_t* dst,
    size_t dst_size,
    to_char_t replacement = to_char_t('?'))
{
    using encoder = utf::utf_traits<to_char_t>;
    using decoder = utf::utf_traits<from_char_t>;

    const from_char_t* first = src;
    const from_char_t* const last = src + src_size;
    to_char_t* out = dst;
    const to_char_t* const out_end = dst + dst_size;

    // No supported encoding needs more than 4 code units per code point
    // (UTF-8 worst case). Encoding into scratch first lets us measure a
    // code point's size before committing it, so we never write a partial
    // one when space runs out.
    to_char_t scratch[4];

    while (first != last && out != out_end)
    {
        const from_char_t* next = first;
        utf::code_point c{};
        next = decoder::decode(next, last, c);

        to_char_t* scratch_end = encoder::encode(c, scratch, replacement);
        const size_t n = static_cast<size_t>(scratch_end - scratch);

        if (static_cast<size_t>(out_end - out) < n)
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

    return out;
}

//==============================================================================
// string_cast - convenience wrappers producing a new string
//==============================================================================
// All three forward to core #1 with back_inserter - unbounded output is safe
// here because the destination is a growing container, not a fixed buffer.
//==============================================================================

/**
 * @brief Converts a character range from one encoding to a new string.
 *
 * @tparam to_char_t Destination character type.
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the input range.
 * @param last Iterator to the end of the input range.
 * @param replacement Replacement character to use for invalid sequences.
 * @return A new basic_string<to_char_t> containing the converted characters.
 */
template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iter<IT>::value)>
auto string_cast(IT first, IT last, to_char_t replacement = to_char_t('?'))
{
    basic_string<to_char_t> res;
    res.reserve(static_cast<size_t>(std::distance(first, last)));
    string_cast<to_char_t>(first, last, back_inserter(res), replacement);
    return res;
}

/**
 * @brief Converts a std::basic_string-like object from one character type to another.
 *
 * @tparam S Stringlike type.
 * @param s Input string.
 * @param replacement Replacement character to use for invalid sequences.
 * @return A new basic_string<to_char_t> with converted content.
 */
template <typename to_char_t, typename S, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& is_string_like<S>::value)>
auto string_cast(const S& s, to_char_t replacement = to_char_t('?'))
{
    return string_cast<to_char_t>(s.begin(), s.end(), replacement);
}

/**
 * @brief Converts a null-terminated character sequence to a new string.
 *
 * @tparam to_char_t Destination character type.
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the null-terminated input range.
 * @param replacement Replacement character to use for invalid sequences.
 * @return A new basic_string<to_char_t> containing the converted characters.
 */
template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iter<IT>::value)>
auto string_cast(IT first, to_char_t replacement = to_char_t('?'))
{
    using from_char_t = typename type_traits::value_type<IT>::type;

    IT last = first;
    while (*last != static_cast<from_char_t>(0))
    {
        ++last;
    }

    return string_cast<to_char_t>(first, last, replacement);
}

} // namespace str
} // namespace vx
