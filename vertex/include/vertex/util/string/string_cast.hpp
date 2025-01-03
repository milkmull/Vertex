#pragma once

#include "vertex/util/encode/utf.hpp"
#include "vertex/util/string/type_traits.hpp"

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// casts
///////////////////////////////////////////////////////////////////////////////

template <typename to_char_t, typename IT1, typename IT2, VX_REQUIRES(type_traits::is_char_iterator<IT1>::value)>
inline IT2 string_cast(IT1 first, IT1 last, IT2 out, to_char_t replacement = 0)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    using from_char_type = typename type_traits::value_type<IT1>::type;
    using to_char_type = to_char_t;

    using encoder_type = utf::utf_traits<to_char_type>;
    using decoder_type = utf::utf_traits<from_char_type>;

    while (first != last)
    {
        uint32_t c{};
        first = decoder_type::decode(first, last, c);
        out = encoder_type::encode(c, out, replacement);
    }

    return out;
}

#if defined(__cpp_lib_string_view)

template <typename to_char_t, typename from_char_t, typename Traits>
inline auto string_cast(
    const std::basic_string_view<from_char_t, Traits>& s,
    to_char_t replacement = 0
)
{
    using from_char_type = from_char_t;
    using to_char_type = to_char_t;

    using encoder_type = utf::utf_traits<to_char_type>;
    using decoder_type = utf::utf_traits<from_char_type>;

    std::basic_string<to_char_type> res;
    res.reserve(s.size());
    string_cast<to_char_type>(s.begin(), s.end(), std::back_inserter(res), replacement);
    return res;
}

#endif // __cpp_lib_string_view

template <typename to_char_t, typename from_char_t, typename Traits, typename Alloc>
inline auto string_cast(
    const std::basic_string<from_char_t, Traits, Alloc>& s,
    to_char_t replacement = 0
)
{
    using from_char_type = from_char_t;
    using to_char_type = to_char_t;

    using encoder_type = utf::utf_traits<to_char_type>;
    using decoder_type = utf::utf_traits<from_char_type>;

    std::basic_string<to_char_type> res;
    res.reserve(s.size());
    string_cast<to_char_type>(s.begin(), s.end(), std::back_inserter(res), replacement);
    return res;
}

template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
inline auto string_cast(IT first, to_char_t replacement = 0)
{
    using from_char_type = typename type_traits::value_type<IT>::type;
    using to_char_type = to_char_t;

    using encoder_type = utf::utf_traits<to_char_type>;
    using decoder_type = utf::utf_traits<from_char_type>;

    // Find the null terminator
    IT last = first;
    while (*last != static_cast<from_char_type>(0)) { ++last; }
    const size_t size = static_cast<size_t>(std::distance(first, last));

    std::basic_string<to_char_type> res;
    res.reserve(size);
    string_cast<to_char_type>(first, last, std::back_inserter(res), replacement);
    return res;
}

} // namespace str
} // namespace vx