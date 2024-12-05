#pragma once

#include "vertex/util/string/string_type.hpp"

namespace vx {
namespace io {

namespace __detail {

// https://github.com/gcc-mirror/gcc/blob/75405ead52a987adb70576b66a4220ec490c523d/libstdc%2B%2B-v3/include/bits/quoted_string.h

template <typename string_t, typename char_t>
struct quoted_string
{
    static_assert(
        std::is_reference<string_t>::value || std::is_pointer<string_t>::value,
        "String type must be pointer or reference"
        );

    quoted_string(string_t s, char_t del, char_t esc)
        : string(s), delimeter{ del }, escape{ esc } {}

    quoted_string& operator=(quoted_string&) = delete;

    string_t string;
    char_t delimeter;
    char_t escape;
};

template <typename char_t, typename Traits>
inline std::basic_ostream<char_t, Traits>& operator<<(
    std::basic_ostream<char_t, Traits>& os,
    const quoted_string<const char_t*, char_t>& s
)
{
    std::basic_ostringstream<char_t, Traits> oss;

    oss << s.delimeter;
    for (const char_t* c = s.string; *c; ++c)
    {
        if (*c == s.delimeter || *c == s.escape)
        {
            oss << s.escape;
        }
        oss << *c;
    }
    oss << s.delimeter;

    return os << oss.str();
}

template <typename char_t, typename Traits, typename string_t>
inline std::basic_ostream<char_t, Traits>& operator<<(
    std::basic_ostream<char_t, Traits>& os,
    const quoted_string<string_t, char_t>& s
)
{
    std::basic_ostringstream<char_t, Traits> oss;

    oss << s.delimeter;
    for (auto c : s.string)
    {
        if (c == s.delimeter || c == s.escape)
        {
            oss << s.escape;
        }

        oss << c;
    }
    oss << s.delimeter;

    return os << oss.str();
}

template <typename char_t, typename Traits, typename Alloc>
inline std::basic_istream<char_t, Traits>& operator>>(std::basic_istream<char_t, Traits>& is,
    const quoted_string<std::basic_string<char_t, Traits, Alloc>&, char_t>& s
)
{
    char_t c;
    is >> c;
    if (!is.good())
    {
        return is;
    }

    if (c != s.delimeter)
    {
        is.unget();
        is >> s.string;
        return is;
    }

    s.string.clear();
    typename std::ios_base::fmtflags flags = is.flags(is.flags() & ~std::ios_base::skipws);

    do
    {
        is >> c;
        if (!is.good())
        {
            break;
        }
        if (c == s.escape)
        {
            is >> c;
            if (!is.good())
            {
                break;
            }
        }
        else if (c == s.delimeter)
        {
            break;
        }

        s.string += c;

    } while (true);

    is.setf(flags);
    return is;
}

} // namespace __detail

template <typename char_t>
inline auto quoted(
    const char_t* s,
    char_t delimeter = char_t('"'),
    char_t escape = char_t('\\')
)
{
    return __detail::quoted_string<const char_t*, char_t>(s, delimeter, escape);
}

template <typename char_t, typename Traits, typename Alloc>
inline auto quoted(
    const std::basic_string<char_t, Traits, Alloc>& s,
    char_t delimeter = char_t('"'),
    char_t escape = char_t('\\')
)
{
    return __detail::quoted_string<const std::basic_string<char_t, Traits, Alloc>&, char_t>(s, delimeter, escape);
}

template <typename char_t, typename Traits, typename Alloc>
inline auto quoted(
    std::basic_string<char_t, Traits, Alloc>& s,
    char_t delimeter = char_t('"'),
    char_t escape = char_t('\\')
)
{
    return __detail::quoted_string<const std::basic_string<char_t, Traits, Alloc>&, char_t>(s, delimeter, escape);
}

#if defined(__cpp_lib_string_view)

template <typename char_t, typename Traits>
inline auto quoted(
    std::basic_string_view<char_t, Traits>& s,
    char_t delimeter = char_t('"'),
    char_t escape = char_t('\\')
)
{
    return __detail::quoted_string<std::basic_string_view<char_t, Traits>&, char_t>(s, delimeter, escape);
}

#endif // __cpp_lib_string_view

} // namespace io
} // namespace vx