#pragma once

#include "vertex/config/type_traits.hpp"
#include "vertex/std/utf.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/iterator.hpp"

namespace vx {
namespace str {

//==============================================================================
// casts
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

/**
 * @brief Converts a character range from one encoding to another.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @param src Pointer to the beginning of the input range.
 * @param size Number of characters in the input range.
 * @param dst Pointer to the output buffer.
 * @param replacement Replacement character to use for invalid sequences.
 * @return Pointer after the last written character.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
to_char_t* string_cast(const from_char_t* src, size_t size, to_char_t* dst, to_char_t replacement = to_char_t('?'))
{
    return string_cast<to_char_t>(src, src + size, dst, replacement);
}

/**
 * @brief Converts a character range from one encoding to a new string.
 *
 * @tparam to_char_t Destination character type.
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the input range.
 * @param last Iterator to the end of the input range.
 * @param replacement Replacement character to use for invalid sequences.
 * @return A new std::basic_string<to_char_t> containing the converted characters.
 */
template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iter<IT>::value)>
auto string_cast(IT first, IT last, to_char_t replacement = to_char_t('?'))
{
    basic_string<to_char_t> res;
    const size_t size = static_cast<size_t>(std::distance(first, last));
    res.reserve(size);
    string_cast<to_char_t>(first, last, back_inserter(res), replacement);
    return res;
}

/**
 * @brief Converts a std::basic_string from one character type to another.
 *
 * @tparam S Stringlike type.
 * @param s Input string.
 * @param replacement Replacement character to use for invalid sequences.
 * @return A new basic_string<to_char_t> with converted content.
 */
template <typename to_char_t, typename S, VX_REQUIRES(type_traits::is_char<to_char_t>::value && is_string_like<S>::value)>
auto string_cast(
    const S& s,
    to_char_t replacement = 0)
{
    basic_string<to_char_t> res;
    res.reserve(s.size());
    string_cast<to_char_t>(s.begin(), s.end(), back_inserter(res), replacement);
    return res;
}

/**
 * @brief Converts a null-terminated character sequence to another character type string.
 *
 * @tparam to_char_t Destination character type.
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the null-terminated input range.
 * @param replacement Replacement character to use for invalid sequences.
 * @return A new std::basic_string<to_char_t> containing the converted characters.
 */
template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iter<IT>::value)>
auto string_cast(IT first, to_char_t replacement = to_char_t('?'))
{
    using from_char_t = typename type_traits::value_type<IT>::type;

    // Find the null terminator
    IT last = first;
    while (*last != static_cast<from_char_t>(0))
    {
        ++last;
    }
    const size_t size = static_cast<size_t>(std::distance(first, last));

    basic_string<to_char_t> res;
    res.reserve(size);
    string_cast<to_char_t>(first, last, back_inserter(res), replacement);
    return res;
}

} // namespace str
} // namespace vx
