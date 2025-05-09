#pragma once

#include "vertex/util/encode/utf.hpp"
#include "vertex/util/string/type_traits.hpp"

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// casts
///////////////////////////////////////////////////////////////////////////////

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
template <typename to_char_t, typename IT1, typename IT2, VX_REQUIRES(type_traits::is_char_iterator<IT1>::value)>
inline IT2 string_cast(IT1 first, IT1 last, IT2 out, to_char_t replacement = '?')
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
 * @brief Converts a character range from one encoding to a new string.
 *
 * @tparam to_char_t Destination character type.
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the input range.
 * @param last Iterator to the end of the input range.
 * @param replacement Replacement character to use for invalid sequences.
 * @return A new std::basic_string<to_char_t> containing the converted characters.
 */
template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
inline auto string_cast(IT first, IT last, to_char_t replacement = '?')
{
    std::basic_string<to_char_t> res;
    const size_t size = static_cast<size_t>(std::distance(first, last));
    res.reserve(size);
    string_cast<to_char_t>(first, last, std::back_inserter(res), replacement);
    return res;
}

/**
 * @brief Converts a std::basic_string from one character type to another.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @tparam Traits Character traits of the source string.
 * @tparam Alloc Allocator type of the source string.
 * @param s Input string.
 * @param replacement Replacement character to use for invalid sequences.
 * @return A new std::basic_string<to_char_t> with converted content.
 */
template <typename to_char_t, typename from_char_t, typename Traits, typename Alloc>
inline auto string_cast(
    const std::basic_string<from_char_t, Traits, Alloc>& s,
    to_char_t replacement = 0
)
{
    std::basic_string<to_char_t> res;
    res.reserve(s.size());
    string_cast<to_char_t>(s.begin(), s.end(), std::back_inserter(res), replacement);
    return res;
}

#if defined(__cpp_lib_string_view)

/**
 * @brief Converts a std::basic_string_view from one character type to another.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @tparam Traits Character traits of the source string view.
 * @param s Input string view.
 * @param replacement Replacement character to use for invalid sequences.
 * @return A new std::basic_string<to_char_t> with converted content.
 */
template <typename to_char_t, typename from_char_t, typename Traits>
inline auto string_cast(
    const std::basic_string_view<from_char_t, Traits>& s,
    to_char_t replacement = '?'
)
{
    std::basic_string<to_char_t> res;
    res.reserve(s.size());
    string_cast<to_char_t>(s.begin(), s.end(), std::back_inserter(res), replacement);
    return res;
}

#endif // __cpp_lib_string_view

/**
 * @brief Converts a null-terminated character sequence to another character type string.
 *
 * @tparam to_char_t Destination character type.
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the null-terminated input range.
 * @param replacement Replacement character to use for invalid sequences.
 * @return A new std::basic_string<to_char_t> containing the converted characters.
 */
template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
inline auto string_cast(IT first, to_char_t replacement = '?')
{
    using from_char_t = typename type_traits::value_type<IT>::type;

    // Find the null terminator
    IT last = first;
    while (*last != static_cast<from_char_t>(0)) { ++last; }
    const size_t size = static_cast<size_t>(std::distance(first, last));

    std::basic_string<to_char_t> res;
    res.reserve(size);
    string_cast<to_char_t>(first, last, std::back_inserter(res), replacement);
    return res;
}

} // namespace str
} // namespace vx