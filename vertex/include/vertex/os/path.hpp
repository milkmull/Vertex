#pragma once

#include "vertex/system/platform_config.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/util/io/quoted.hpp"
#include "vertex/util/crypto/FNV1a.hpp"

// https://docs.godotengine.org/en/stable/classes/class_fileaccess.html#class-fileaccess
// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_filesystem.h
// https://en.cppreference.com/w/cpp/filesystem

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// path
///////////////////////////////////////////////////////////////////////////////

class path;

namespace __detail {

#if defined(__VX_OS_WINDOWS_FILESYSTEM)

using value_type = wchar_t;

#   define PATH_TEXT(x) L##x

#   define PATH_SEPARATOR L'/'
#   define PATH_PREFERRED_SEPARATOR L'\\'
#   define PATH_DOT L'.'

#else

using value_type = char;

#   define PATH_TEXT(x) x

#   define PATH_SEPARATOR '/'
#   define PATH_PREFERRED_SEPARATOR '/'
#   define PATH_DOT '.'

#endif // __VX_OS_WINDOWS_FILESYSTEM

using string_type = std::basic_string<value_type>;

///////////////////////////////////////////////////////////////////////////////
// parsing
///////////////////////////////////////////////////////////////////////////////

struct parser
{
    enum state
    {
        BEGIN,
        ROOT_NAME,
        ROOT_DIRECTORY,
        RELATIVE_PATH,
        END
    };

    struct substring
    {
        size_t pos;
        size_t size;
    };

    static constexpr bool is_directory_separator(value_type c)
    {
        return c == PATH_SEPARATOR
#if defined(__VX_OS_WINDOWS_FILESYSTEM)
            || c == PATH_PREFERRED_SEPARATOR
#endif // __VX_OS_WINDOWS_FILESYSTEM
            ;
    }

    static constexpr bool is_element_separator(value_type c)
    {
        return is_directory_separator(c)
#if defined(__VX_OS_WINDOWS_FILESYSTEM)
            || c == L':'
#endif // __VX_OS_WINDOWS_FILESYSTEM
            ;
    }

    static constexpr bool is_letter(value_type c)
    {
        return (c >= PATH_TEXT('A') && c <= PATH_TEXT('Z'))
            || (c >= PATH_TEXT('a') && c <= PATH_TEXT('z'));
    }

    static inline size_t find_separator(const value_type* p, size_t off, size_t size) noexcept
    {
        size_t pos = off;

        while (pos < size && !is_directory_separator(p[pos]))
        {
            ++pos;
        }

        return pos;
    }

    // Returns a pointer to the end of the root name if it exists, otherwise last
    static size_t find_root_name_end(const value_type* first, size_t size)
    {
        if (size < 2)
        {
            return 0;
        }

#if defined(__VX_OS_WINDOWS_FILESYSTEM)

        if (is_letter(first[0]) && first[1] == L':')
        {
            return 2;
        }

#endif // __VX_OS_WINDOWS_FILESYSTEM

        if (!is_directory_separator(first[0]))
        {
            return 0;
        }

        // case "//", possibly followed by more characters
        if (is_directory_separator(first[1]))
        {
            if (size == 2)
            {
                // The whole path is just a pair of separators "\\"
                return 2;
            }

#if defined(__VX_OS_WINDOWS_FILESYSTEM)

            // https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file
            // cases "\\?\" and "\\.\"
            else if (size >= 4 && (first[2] == L'?' || first[2] == L'.') && is_directory_separator(first[3]))
            {
                return find_separator(first, 3, size);
            }

#endif // __VX_OS_WINDOWS_FILESYSTEM

            else if (is_directory_separator(first[2]))
            {
                // The path starts with three directory separators, which is interpreted as a root directory followed by redundant separators
                return 0;
            }
            else
            {
                // case "//net{/}"
                return find_separator(first, 2, size);
            }
        }

#if defined(__VX_OS_WINDOWS_FILESYSTEM)

        // https://stackoverflow.com/questions/23041983/path-prefixes-and
        // case "\??\" (NT path prefix)
        else if (size >= 4 && first[1] == L'?' && first[2] == L'?' && is_directory_separator(first[3]))
        {
            return find_separator(first, 3, size);
        }

#endif // __VX_OS_WINDOWS_FILESYSTEM

        return 0;
    }

    static substring parse_root_name(const string_type& s)
    {
        const size_t size = find_root_name_end(s.c_str(), s.size());
        return substring{ 0, size };
    }

    static substring parse_root_directory(const string_type& s)
    {
        const size_t size = s.size();
        const size_t off = find_root_name_end(s.c_str(), size);

        // Count consecutive directory separators starting from offset.
        size_t count = 0;
        while (off + count < size && is_directory_separator(s[off + count]))
        {
            ++count;
        }

        return substring{ off, count };
    }

    static substring parse_root_path(const string_type& s)
    {
        const auto root_directory = parse_root_directory(s);
        return substring{ 0, root_directory.pos + root_directory.size };
    }

    static substring parse_relative_path(const string_type& s)
    {
        const auto root_path = parse_root_path(s);
        return substring{ root_path.size, s.size() - root_path.size };
    }

    static substring parse_parent_path(const string_type& s)
    {
        const auto relative_path = parse_relative_path(s);
        size_t i = relative_path.pos + relative_path.size;

        // case 1: relative-path ends in a directory-separator,
        // remove the separator to remove "magic empty path".
        // example: R"(/cat/dog/\//\)"

        // case 2: relative-path doesn't end in a directory-separator,
        // remove the filename and last directory-separator to prevent
        // creation of a "magic empty path".
        // example: "/cat/dog"

        while (i > relative_path.pos && !is_directory_separator(s[i - 1]))
        {
            // handle case 2 by removing trailing filename, puts us into case 1
            --i;
        }

        while (i > relative_path.pos && is_directory_separator(s[i - 1]))
        {
            // handle case 1 by removing trailing slashes
            --i;
        }

        return substring{ 0, i };
    }

    static substring parse_filename(const string_type& s)
    {
        const size_t size = s.size();

        // Start searching backward from the end of the string
        for (size_t i = size; i > 0; --i)
        {
            if (is_element_separator(s[i - 1]))
            {
                return substring{ i, size - i };
            }
        }

        // If no directory separator is found, the entire string is the filename
        return substring{ 0, size };
    }

    static substring parse_stem(const string_type& s)
    {
        const auto extension = parse_extension(s);

        // Start searching backward from the end of the string
        for (size_t i = extension.pos; i > 0; --i)
        {
            if (is_element_separator(s[i - 1]))
            {
                return substring{ i, extension.pos - i };
            }
        }

        // If no directory separator is found, the entire string is the stem
        return substring{ 0, extension.pos };
    }

    static substring parse_extension(const string_type& s)
    {
        const size_t size = s.size();

        if (size <= 1)
        {
            return substring{ size, 0 };
        }

        // size >= 2
        size_t i = size;

        if (s[--i] == PATH_DOT)
        {
            if (
                // case "/." or ":."
                is_element_separator(s[i]) ||
                // case "..", "x/..", or "x:.."
                (s[i] == PATH_DOT && (size == 2 || is_element_separator(s[i - 1])))
            )
            {
                return substring{ size, 0 };
            }

            // case "x." or "x.."
            return substring{ size - 1, 1 };
        }

        // Start searching backward from the end of the string
        while (i > 0)
        {
            if (s[--i] == PATH_DOT)
            {
                return substring{ i, size - i };
            }
            else if (is_element_separator(s[i]))
            {
                break;
            }
        }

        return substring{ size, 0 };
    }
};

template <typename path_t>
class path_iterator
{
    friend path_t;

public:

    using iterator_category = std::input_iterator_tag;
    using value_type = path_t;
    using difference_type = ptrdiff_t;
    using pointer = const path_t*;
    using reference = const path_t&;

public:

    path_iterator(const path_t* path) noexcept
        : m_path(path)
    {
        operator++();
    }

    path_iterator() = default;
    ~path_iterator() = default;
    path_iterator(const path_iterator&) = default;
    path_iterator(path_iterator&&) = default;
    path_iterator& operator=(const path_iterator&) = default;
    path_iterator& operator=(path_iterator&&) = default;

private:

    path_iterator(const path_t* path, typename parser::state s) noexcept
        : m_path(path), m_position(path->m_path.size()), m_state(s) {}

public:

    reference operator*() const noexcept { return m_element; }
    pointer operator->() const noexcept { return std::addressof(m_element); }

    path_iterator& operator++()
    {
        if (m_state == parser::state::END)
        {
            return *this;
        }

        const auto& path = m_path->native();

        if (m_state <= parser::state::ROOT_NAME)
        {
            const auto root_directory = parser::parse_root_directory(path);
            
            if (m_state == parser::state::BEGIN && root_directory.pos != 0)
            {
                // The iterator was just initialized and the first element is the root name
                m_element.m_path = path.substr(0, root_directory.pos);
                m_position = root_directory.pos;
                m_state = parser::state::ROOT_NAME;
                return *this;
            }
            else if (root_directory.size != 0)
            {
                // Root directory exists, so next is root directory
                m_element.m_path = path.substr(root_directory.pos, root_directory.size);
                m_position = root_directory.pos + root_directory.size;
                m_state = parser::state::ROOT_DIRECTORY;
                return *this;
            }
        }

        const size_t path_size = path.size();

        if (m_position == path_size)
        {
            // We are at the end of the path
            m_element.clear();
            m_position = path_size;
            m_state = parser::state::END;
            return *this;
        }

        while (parser::is_directory_separator(path[m_position]))
        {
            // Advance to the start of the following path element
            if (++m_position == path_size)
            {
                // "magic" empty element selected
                m_element.clear();
                return *this;
            }
        }
        
        const size_t start = m_position;
        m_position = parser::find_separator(path.data(), start, path_size);
        m_element.m_path = path.substr(start, m_position - start);
        m_state = parser::state::RELATIVE_PATH;

        return *this;
    }

    friend bool operator==(const path_iterator& lhs, const path_iterator& rhs)
    {
        return lhs.m_path == rhs.m_path
            && lhs.m_position == rhs.m_position
            && lhs.m_state == rhs.m_state;
    }

    friend bool operator!=(const path_iterator& lhs, const path_iterator& rhs)
    {
        return !(lhs == rhs);
    }

private:

    const path_t* m_path;
    path_t m_element;
    size_t m_position = 0;
    typename parser::state m_state = parser::state::BEGIN;
};

} // namespace __detail

class path
{
public:

    using value_type = typename __detail::value_type;
    using string_type = typename __detail::string_type;

    static constexpr value_type separator = PATH_SEPARATOR;
    static constexpr value_type preferred_separator = PATH_PREFERRED_SEPARATOR;
    static constexpr value_type dot = PATH_DOT;

    using iterator = __detail::path_iterator<path>;
    using const_iterator = iterator;

    friend iterator;

public:

    path() noexcept = default;
    ~path() = default;

    path(const path&) = default;
    path(path&&) noexcept = default;

    path& operator=(const path&) = default;
    path& operator=(path&&) noexcept = default;

public:

    path(string_type&& s) : m_path(std::move(s)) {}

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path(const Src& src) : m_path(str::string_cast<value_type>(src)) {}

    template <typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
    path(IT first, IT last)
    {
        str::string_cast<value_type>(first, last, std::back_inserter(m_path));
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    inline path& operator=(const Src& rhs)
    {
        m_path = str::string_cast<value_type>(rhs);
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    inline path& assign(const Src& src)
    {
        return operator=(src);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
    inline path& assign(IT first, IT last)
    {
        return operator=(path(first, last));
    }

    ///////////////////////////////////////////////////////////////////////////////
    // concatenation
    ///////////////////////////////////////////////////////////////////////////////

    // https://en.cppreference.com/w/cpp/filesystem/path/append
    // https://github.com/microsoft/STL/blob/fec1c8b6a13e5411edebbddc3ad98258f5e282d2/stl/inc/filesystem#L712

    path& operator/=(const path& rhs)
    {
        // if rhs.is_absolute(), then op=(rhs)
        if (rhs.is_absolute())
        {
            return operator=(rhs);
        }

        const value_type* lhs_first = m_path.c_str();
        const size_t lhs_size = m_path.size();
        const auto lhs_root_name = __detail::parser::parse_root_name(m_path);

        const value_type* rhs_first = rhs.m_path.c_str();
        const size_t rhs_size = rhs.m_path.size();
        const auto rhs_root_name = __detail::parser::parse_root_name(rhs.m_path);

        // if rhs.has_root_name() && rhs.root_name() != root_name(), then op=(rhs)
        if (rhs_root_name.size != 0 && !std::equal(
            lhs_first + lhs_root_name.pos, lhs_first + (lhs_root_name.pos + lhs_root_name.size),
            rhs_first + rhs_root_name.pos, rhs_first + (rhs_root_name.pos + rhs_root_name.size)))
        {
            return operator=(rhs);
        }

        // if rhs.has_root_directory() removes any root directory and relative path from *this
        if (rhs_root_name.pos + rhs_root_name.size != rhs_size &&
            __detail::parser::is_directory_separator(rhs.m_path[rhs_root_name.pos + rhs_root_name.size]))
        {
            m_path.erase(lhs_root_name.size);
        }
        else
        {
            // Otherwise, if (!has_root_directory && is_absolute) || has_filename appends path::preferred_separator
            if (lhs_root_name.size == lhs_size)
            {
                // Here, !has_root_directory && !has_filename
                // Going through our root_name kinds:
                // X: can't be absolute here, since those paths are absolute only when has_root_directory
                // \\?\ can't exist without has_root_directory
                // \\server can be absolute here
                if (lhs_root_name.size >= 3)
                {
                    m_path.push_back(preferred_separator);
                }
            }
            else
            {
                // Here, has_root_directory || has_filename
                // If there is a trailing slash, the trailing slash might be part of root_directory.
                // If it is, has_root_directory && !has_filename, so the test fails.
                // If there is a trailing slash not part of root_directory, then !has_filename, so only
                // (!has_root_directory && is_absolute) remains
                // Going through our root_name kinds:
                // X:cat\ needs a root_directory to be absolute
                // \\server\cat must have a root_directory to exist with a relative_path
                // \\?\ must have a root_directory to exist
                // As a result, the test fails if there is a trailing slash.
                // If there is no trailing slash, then has_filename, so the test passes.
                // Therefore, the test passes if and only if there is no trailing slash.
                if (!__detail::parser::is_directory_separator(m_path[lhs_size - 1]))
                {
                    m_path.push_back(preferred_separator);
                }
            }
        }

        // Then appends the native format pathname of rhs, omitting any root-name from its generic format
        // pathname, to the native format pathname.
        m_path.append(rhs.m_path);
        return *this;
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path& operator/=(const Src& rhs)
    {
        return operator/=(path(rhs));
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path& append(const Src& src)
    {
        return operator/=(path(src));
    }

    template <typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
    path& append(IT first, IT last)
    {
        return operator/=(path(first, last));
    }

    path& operator+=(const path& rhs)
    {
        return operator+=(rhs.m_path);
    }

    path& operator+=(const string_type& rhs)
    {
        m_path += rhs;
        return *this;
    }

    template <typename char_t>
    path& operator+=(const char_t rhs)
    {
        return operator+=(path(&rhs, &rhs + 1));
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path& concat(const Src& src)
    {
        return operator+=(path(src));
    }

    template <typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
    path& concat(IT first, IT last)
    {
        return operator+=(path(first, last));
    }

    ///////////////////////////////////////////////////////////////////////////////
    // modifiers
    ///////////////////////////////////////////////////////////////////////////////

    inline void clear() { m_path.clear(); }

    inline path& make_preferred()
    {
        std::replace(m_path.begin(), m_path.end(), separator, preferred_separator);
        return *this;
    }

    inline path& remove_filename()
    {
        const auto filename = __detail::parser::parse_filename(m_path);
        m_path.erase(m_path.begin() + filename.pos);
        return *this;
    }

    inline path& remove_extension()
    {
        const auto extension = __detail::parser::parse_extension(m_path);
        m_path.erase(m_path.begin() + extension.pos);
        return *this;
    }

    inline path& replace_extension(const path& new_extension)
    {
        *this = remove_extension();
        if (!new_extension.m_path.empty() && new_extension.m_path[0] != dot)
        {
            m_path.push_back(dot);
        }
        return operator+=(new_extension);
    }

    inline void swap(path& p) noexcept
    {
        m_path.swap(p.m_path);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // format observers
    ///////////////////////////////////////////////////////////////////////////////

    inline const value_type* c_str() const noexcept { return m_path.c_str(); }
    inline const string_type& native() const noexcept { return m_path; }
    inline operator string_type() const { return m_path; }

    template <
        typename char_t = char,
        typename Traits = std::char_traits<char_t>,
        typename Alloc = std::allocator<char_t>
    >
    std::basic_string<char_t, Traits, Alloc> string() const
    {
        return str::string_cast<char_t>(m_path);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // compare
    ///////////////////////////////////////////////////////////////////////////////

    int compare(const path& rhs) const
    {
        return compare(rhs.m_path);
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    int compare(const Src& src) const
    {
        return compare(path(src));
    }

    int compare(const string_type& rhs) const noexcept
    {
        const auto lhs_root_directory = __detail::parser::parse_root_directory(m_path);
        const auto rhs_root_directory = __detail::parser::parse_root_directory(rhs);

        const auto lhs_first = m_path.begin();
        const auto rhs_first = rhs.begin();

        const auto lhs_root_name_end = lhs_first + lhs_root_directory.pos;
        const auto rhs_root_name_end = rhs_first + rhs_root_directory.pos;

        // If root_name().native().compare(p.root_name().native()) is nonzero,
        // returns that value.
        const auto pair = std::mismatch(lhs_first, lhs_root_name_end, rhs_first, rhs_root_name_end);
        if (pair.first != lhs_root_name_end || pair.second != rhs_root_name_end)
        {
            return *pair.first - *pair.second;
        }

        // Otherwise, if has_root_directory() != p.has_root_directory(),
        // returns a value less than zero if has_root_directory() is false
        // and a value greater than zero otherwise.
        const int root_directory_cmp = static_cast<int>(lhs_root_directory.size != 0) - static_cast<int>(rhs_root_directory.size != 0);
        if (root_directory_cmp != 0)
        {
            return root_directory_cmp;
        }

        // Otherwise returns a value less than, equal to or greater than 0
        // if the relative portion of the path (relative_path()) is respectively
        // lexicographically less than, equal to or greater than the relative
        // portion of p (p.relative_path()). Comparison is performed element-wise,
        // as if by iterating both paths from begin() to end() and comparing the
        // result of native() for each element.

        auto lhs_it = pair.first;
        auto rhs_it = pair.second;

        const auto lhs_last = m_path.end();
        const auto rhs_last = rhs.end();

        while (true)
        {
            const bool lhs_empty = lhs_it == lhs_last;
            const bool rhs_empty = rhs_it == rhs_last;
            const int empty_cmp = static_cast<int>(lhs_empty) - static_cast<int>(rhs_empty);
            if (lhs_empty || empty_cmp != 0)
            {
                return empty_cmp;
            }

            const bool lhs_is_sep = __detail::parser::is_directory_separator(*lhs_it);
            const bool rhs_is_sep = __detail::parser::is_directory_separator(*rhs_it);
            const int sep_cmp = static_cast<int>(lhs_is_sep) - static_cast<int>(rhs_is_sep);
            if (sep_cmp != 0)
            {
                return sep_cmp;
            }

            if (lhs_is_sep)
            {
                lhs_it = std::find_if_not(lhs_it + 1, lhs_last, __detail::parser::is_directory_separator);
                rhs_it = std::find_if_not(rhs_it + 1, rhs_last, __detail::parser::is_directory_separator);
                continue;
            }

            const int cmp = static_cast<int>(*lhs_it) - static_cast<int>(*rhs_it);
            if (cmp != 0)
            {
                return cmp;
            }

            ++lhs_it;
            ++rhs_it;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // generation
    ///////////////////////////////////////////////////////////////////////////////

    // https://en.cppreference.com/w/cpp/filesystem/path

    path lexically_normal() const
    {
        using parse_state = typename __detail::parser::state;

        if (empty())
        {
            // 1. If the path is empty, stop (normal form of an empty path is an empty path).
            return path();
        }

        string_type normalized;
        normalized.reserve(m_path.size());

        std::vector<size_t> stack;
        stack.reserve(15);
        bool has_root_directory = false;

#       define last_is_dotdot() ( \
            normalized.size() >= 3 && \
            normalized.compare(stack.back(), 2, PATH_TEXT("..")) == 0 && \
            normalized.back() == preferred_separator \
        )

        for (auto it = begin(); it != end(); ++it)
        {
            if (it.m_state == parse_state::ROOT_NAME)
            {
                // 3. Replace each slash character in the root-name with path::preferred_separator.
                for (const value_type c : it->native())
                {
                    normalized.push_back((c == separator) ? preferred_separator : c);
                }

                continue;
            }

            else if (it.m_state == parse_state::ROOT_DIRECTORY)
            {
                // Root-directory will only ever be made up of separators.
                // We can just replace it with a single preferred separator.
                normalized.push_back(preferred_separator);
                has_root_directory = true;
                continue;
            }

            const string_type p = it->native();
            
            if (p.empty())
            {
                continue;
            }

            // 4. Remove each dot and any immediately following directory-separator.
            else if (p == PATH_TEXT("."))
            {
                continue;
            }

            else if (p == PATH_TEXT(".."))
            {
                // 5. Remove each non-dot-dot filename immediately followed by a
                // directory-separator and a dot-dot, along with any immediately
                // following directory-separator.
                if (!stack.empty() && !last_is_dotdot())
                {
                    normalized.erase(stack.back());
                    stack.pop_back();
                    continue;
                }

                // 6. If there is root-directory, remove all dot-dots and
                // any directory-separators immediately following them.
                else if (has_root_directory)
                {
                    continue;
                }
            }

            stack.push_back(normalized.size());
            normalized.append(p);

            // 2. Replace each directory-separator (which may consist of
            // multiple slashes) with a single path::preferred_separator.
            if (__detail::parser::is_directory_separator(m_path[it.m_position]))
            {
                normalized.push_back(preferred_separator);
            }
        }

        // 8. If the path is empty, add a dot (normal form of ./ is .).
        if (normalized.empty())
        {
            normalized.push_back(dot);
        }

        // 7. If the last filename is dot-dot, remove any trailing directory-separator.
        if (!stack.empty() && last_is_dotdot())
        {
            normalized.pop_back();
        }

#       undef last_is_dotdot

        return path(normalized);
    }

     // https://en.cppreference.com/w/cpp/filesystem/path/lexically_normal

    path lexically_relative(const path& base) const
    {
        // If root_name() != base.root_name() or 
        // is_absolute() != base.is_absolute() or 
        // (!has_root_directory() && base.has_root_directory()) or
        // any filename in relative_path() or
        // base.relative_path() can be interpreted as a root-name,
        // returns a default-constructed path.

        if (root_name() != base.root_name() ||
            is_absolute() != base.is_absolute() ||
            (!has_root_directory() && base.has_root_directory()))
        {
            return path();
        }

        // Otherwise, first determines the first mismatched element
        // of *this and base as if by
        // auto [a, b] = mismatch(begin(), end(), base.begin(), base.end()).

        const auto this_last = end();
        const auto base_last = base.end();

        auto pair = std::mismatch(begin(), this_last, base.begin(), base_last);

        // If a == end() and b == base.end(), returns path(".").
        if (pair.first == this_last && pair.second == base_last)
        {
            return path(PATH_TEXT("."));
        }

        // Otherwise, define N as the number of nonempty filename elements
        // that are neither dot nor dot-dot in [b, base.end()), minus the
        // number of dot-dot filename elements.

        int n = 0;

        for (; pair.second != base_last; ++pair.second)
        {
            if (pair.second->empty())
            {
                continue;
            }
            else if (*pair.second == PATH_TEXT(".."))
            {
                --n;
            }
            else if (*pair.second != PATH_TEXT("."))
            {
                ++n;
            }
        }

        // If N < 0, returns a default-constructed path
        if (n < 0)
        {
            return path();
        }

        // If N = 0 and a == end() || a->empty(), returns path("."),
        if (n == 0 && (pair.first == this_last || pair.first->empty()))
        {
            return path(PATH_TEXT("."));
        }

        // Otherwise returns an object composed from a default-constructed
        // path() followed by N applications of operator/=(path("..")),
        // followed by one application of operator/= for each element in
        // the half-open range[a, end()).

        path ret;

        while (n--)
        {
            ret /= PATH_TEXT("..");
        }

        for (; pair.first != this_last; ++pair.first)
        {
            ret /= *pair.first;
        }

        return ret;
    }

    path lexically_proximate(const path& base) const
    {
        const path& p = lexically_relative(base);
        return !p.empty() ? p : *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // decomposition
    ///////////////////////////////////////////////////////////////////////////////

    path root_name() const
    {
        const auto p = __detail::parser::parse_root_name(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path root_directory() const
    {
        const auto p = __detail::parser::parse_root_directory(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path root_path() const
    {
        const auto p = __detail::parser::parse_root_path(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path relative_path() const
    {
        const auto p = __detail::parser::parse_relative_path(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path parent_path() const
    {
        const auto p = __detail::parser::parse_parent_path(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path filename() const
    {
        const auto p = __detail::parser::parse_filename(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path stem() const
    {
        const auto p = __detail::parser::parse_stem(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path extension() const
    {
        const auto p = __detail::parser::parse_extension(m_path);
        return m_path.substr(p.pos, p.size);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // queries
    ///////////////////////////////////////////////////////////////////////////////

    bool empty() const noexcept { return m_path.empty(); }

    bool has_root_path() const      { return __detail::parser::parse_root_path(m_path).size != 0; }
    bool has_root_name() const      { return __detail::parser::parse_root_name(m_path).size != 0; }
    bool has_root_directory() const { return __detail::parser::parse_root_directory(m_path).size != 0; }
    bool has_relative_path() const  { return __detail::parser::parse_relative_path(m_path).size != 0; }
    bool has_parent_path() const    { return __detail::parser::parse_parent_path(m_path).size != 0; }
    bool has_filename() const       { return __detail::parser::parse_filename(m_path).size != 0; }
    bool has_stem() const           { return __detail::parser::parse_stem(m_path).size != 0; }
    bool has_extension() const      { return __detail::parser::parse_extension(m_path).size != 0; }

    bool is_relative() const { return !is_absolute(); }
     
    bool is_absolute() const
    {
        const auto root_directory = __detail::parser::parse_root_directory(m_path);

#if defined(__VX_OS_WINDOWS_FILESYSTEM)
        return root_directory.pos != 0 && root_directory.size != 0;
#else
        return root_directory.size != 0;
#endif // __VX_OS_WINDOWS_FILESYSTEM
    }

    ///////////////////////////////////////////////////////////////////////////////
    // iterators
    ///////////////////////////////////////////////////////////////////////////////

    inline iterator begin() const
    {
        return iterator(this);
    }

    inline iterator end() const
    {
        return iterator(this, __detail::parser::state::END);
    }

    inline iterator cbegin() const
    {
        return begin();
    }

    inline iterator cend() const
    {
        return end();
    }

    ///////////////////////////////////////////////////////////////////////////////
    // non member functions
    ///////////////////////////////////////////////////////////////////////////////

    friend inline bool operator==(const path& lhs, const path& rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    friend inline bool operator!=(const path& lhs, const path& rhs) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    friend inline bool operator>(const path& lhs, const path& rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    friend inline bool operator<(const path& lhs, const path& rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    friend inline bool operator>=(const path& lhs, const path& rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    friend inline bool operator<=(const path& lhs, const path& rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    friend inline path operator/(const path& lhs, const path& rhs)
    {
        return path(lhs) /= rhs;
    }

    template <typename char_t, typename Traits>
    friend inline std::basic_ostream<char_t, Traits>& operator<<(
        std::basic_ostream<char_t, Traits>& os,
        const path& p
    )
    {
        return os << io::quoted(p.string<char_t, Traits>());
    }

    template <typename char_t, typename Traits>
    friend inline std::basic_istream<char_t, Traits>& operator>>(
        std::basic_istream<char_t, Traits>& is,
        path& p
    )
    {
        std::basic_string<char_t, Traits> s;
        if (is >> io::quoted(s))
        {
            p = std::move(s);
        }
        return is;
    }

private:

    string_type m_path;
};

#undef PATH_TEXT
#undef PATH_SEPARATOR
#undef PATH_PREFERRED_SEPARATOR
#undef PATH_DOT

} // namespace os
} // namespace vx

namespace std {

///////////////////////////////////////////////////////////////////////////////
// hashing
///////////////////////////////////////////////////////////////////////////////

// https://github.com/microsoft/STL/blob/fec1c8b6a13e5411edebbddc3ad98258f5e282d2/stl/inc/filesystem#L1634

template <>
struct hash<vx::os::path>
{
    size_t operator()(const vx::os::path& p) noexcept
    {
        vx::crypto::FNV1a fnv1a;

        const auto& text = p.native();
        const size_t size = text.size();
        auto it = text.data();
        const auto last = it + size;

        const auto root_name_end = vx::os::__detail::parser::find_root_name_end(it, size);
        fnv1a.update(it, it += root_name_end);

        bool last_was_slash = false;
        for (; it != last; ++it)
        {
            if (vx::os::__detail::parser::is_directory_separator(*it))
            {
                if (!last_was_slash)
                {
                    fnv1a.update(vx::os::path::preferred_separator);
                    last_was_slash = true;
                }
            }
            else
            {
                fnv1a.update(*it);
                last_was_slash = false;
            }
        }

        return fnv1a.result();
    }
};

} // namespace std