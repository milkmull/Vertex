#pragma once

#include "vertex/config/os.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/util/io/quoted.hpp"
#include "vertex/util/crypto/FNV1a.hpp"
#include "vertex/os/native_string.hpp"

#if defined(_VX_TESTING_PATH)

#   if defined(_VX_TESTING_WINDOWS_PATH)
#       define _VX_WINDOWS_PATH
#   endif

#elif defined(VX_OS_WINDOWS)

#   define _VX_WINDOWS_PATH

#endif // _VX_TESTING_PATH

// https://en.cppreference.com/w/cpp/filesystem

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// path
///////////////////////////////////////////////////////////////////////////////

class path;

namespace _priv {

#if defined(_VX_WINDOWS_PATH)

using path_value_type = wchar_t;

#   define _PATH_TEXT(x) L##x

#   define PATH_SEPARATOR L'/'
#   define PATH_PREFERRED_SEPARATOR L'\\'
#   define PATH_DOT L'.'

#else

using path_value_type = char;

#   define _PATH_TEXT(x) x

#   define PATH_SEPARATOR '/'
#   define PATH_PREFERRED_SEPARATOR '/'
#   define PATH_DOT '.'

#endif // _VX_WINDOWS_PATH

using path_string_type = std::basic_string<path_value_type>;

///////////////////////////////////////////////////////////////////////////////
// parsing
///////////////////////////////////////////////////////////////////////////////

namespace path_parser {

enum class state
{
    begin,
    root_name,
    root_directory,
    relative_path,
    end
};

struct substring
{
    size_t pos;
    size_t size;
};

inline constexpr bool is_directory_separator(path_value_type c) noexcept
{
    return c == PATH_SEPARATOR
#if defined(_VX_WINDOWS_PATH)
        || c == PATH_PREFERRED_SEPARATOR
#endif // _VX_WINDOWS_PATH
        ;
}

inline constexpr bool is_letter(path_value_type c) noexcept
{
    return (c >= _PATH_TEXT('A') && c <= _PATH_TEXT('Z'))
        || (c >= _PATH_TEXT('a') && c <= _PATH_TEXT('z'));
}

inline size_t find_separator(const path_value_type* p, size_t off, size_t size) noexcept
{
    size_t pos = off;

    while (pos < size && !is_directory_separator(p[pos]))
    {
        ++pos;
    }

    return pos;
}

inline constexpr int compare(const path_value_type c1, const path_value_type c2) noexcept
{
    return (is_directory_separator(c1) && is_directory_separator(c2)) ? 0 : (c1 - c2);
}

// https://github.com/boostorg/filesystem/blob/30b312e5c0335831af61ad16802e888f5fb344ea/src/path.cpp#L983
// https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/stl/inc/filesystem#L371

// Returns the start position of the root directory
inline size_t find_root_directory_start(
#if defined(_VX_WINDOWS_PATH)
    const path_value_type* first, size_t size
#else
    const path_value_type*, size_t // unused on non-windows systems
#endif // _VX_WINDOWS_PATH
) noexcept
{
    // Only windows has root name
#if defined(_VX_WINDOWS_PATH)

    // A valid root name requires at least 2 characters (e.g., "C:")
    if (size < 2)
    {
        return 0;
    }

    // Windows drive letter root name (e.g., "C:")
    if (is_letter(first[0]) && first[1] == L':')
    {
        return 2;
    }

    // If the first character is not a directory separator, there is no root name
    if (!is_directory_separator(first[0]))
    {
        return 0;
    }

    // https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file
    // https://stackoverflow.com/questions/23041983/path-prefixes-and
    // cases "\\?\*", "\\.\*", "\??\*" where * is anything EXCEPT a slash
    if (size >= 4 && is_directory_separator(first[3]) && (size == 4 || !is_directory_separator(first[4])) // "\**\*"
        && ((is_directory_separator(first[1]) && (first[2] == L'?' || first[2] == L'.')) // "\\?\*", "\\.\*"
        || (first[1] == L'?' && first[2] == L'?'))) // "\??\" (NT path prefix)
    {
        // Special case when we have "\\?\C:*" for example
        // fix for https://cplusplus.github.io/LWG/issue3699
        if (size >= 6 && is_letter(first[4]) && first[5] == ':')
        {
            return 6;
        }

        return 3;
    }

    // Handle paths starting with double slashes "\\*"
    if (is_directory_separator(first[1]))
    {
        if (size == 2 || is_directory_separator(first[2]))
        {
            // The path is either "\\" or "\\\*", which are both interpreted
            // as a root directory followed by redundant separators
            return 0;
        }
        else
        {
            // case "//server*" (UNC path in Windows, network path in POSIX)
            return find_separator(first, 2, size);
        }
    }

#endif // _VX_WINDOWS_PATH

    return 0;
}

inline substring parse_root_name(const path_string_type& s) noexcept
{
    const size_t size = find_root_directory_start(s.c_str(), s.size());
    return substring{ 0, size };
}

inline substring parse_root_directory(const path_string_type& s) noexcept
{
    const size_t size = s.size();
    const size_t off = find_root_directory_start(s.c_str(), size);

    // Count consecutive directory separators starting from off
    size_t count = 0;
    while (off + count < size && is_directory_separator(s[off + count]))
    {
        ++count;
    }

    return substring{ off, count };
}

inline substring parse_root_path(const path_string_type& s) noexcept
{
    const auto root_directory = parse_root_directory(s);
    return substring{ 0, root_directory.pos + root_directory.size };
}

inline substring parse_relative_path(const path_string_type& s) noexcept
{
    const auto root_path = parse_root_path(s);
    return substring{ root_path.size, s.size() - root_path.size };
}

inline substring parse_parent_path(const path_string_type& s) noexcept
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

inline substring parse_filename(const path_string_type& s) noexcept
{
    const size_t size = s.size();
    const size_t root_name_end = find_root_directory_start(s.c_str(), size);

    // Start searching backward from the end of the string
    for (size_t i = size; i > root_name_end; --i)
    {
        if (is_directory_separator(s[i - 1]))
        {
            return substring{ i, size - i };
        }
    }

    // If no directory separator is found, the entire string is the filename
    return substring{ root_name_end, size - root_name_end };
}

inline substring find_extension(const path_string_type& s, size_t filename_start) noexcept
{
#if defined(_VX_WINDOWS_PATH)

    // On Windows, file paths can contain alternate data streams (ADS), 
    // specified using a colon (e.g., "file.txt:stream"). Since the extension 
    // applies only to the main file and not to the stream, we must ensure 
    // we stop parsing at the first ':' after the filename.
    const size_t extension_end = s.find(L':', filename_start);
    const size_t size = (extension_end != path_string_type::npos) ? extension_end : s.size();

#else

    const size_t size = s.size();

#endif // _VX_WINDOWS_PATH

    if ((size - filename_start) <= 1)
    {
        return substring{ size, 0 };
    }

    // size >= 2
    size_t i = size;

    if (s[--i] == PATH_DOT)
    {
        if (i == 1 && s[1] == PATH_DOT)
        {
            // ".."
            return substring{ 2, 0 };
}
        else
        {
            // "x."
            return substring{ i, 1 };
        }
    }

    // Special case: If the filename starts with a dot (e.g., "/.config"), 
    // it has no extension. Because of this, we can skip checking the first
    // character

    while (i > filename_start + 1)
    {
        if (s[--i] == PATH_DOT)
        {
            return substring{ i, size - i };
        }
        else if (is_directory_separator(s[i]))
        {
            break;
        }
    }

    return substring{ size, 0 };
}

inline substring parse_extension(const path_string_type& s) noexcept
{
    return find_extension(s, parse_filename(s).pos);
}

inline substring parse_stem(const path_string_type& s) noexcept
{
    const auto filename = parse_filename(s);
    return substring{ filename.pos, find_extension(s, filename.pos).pos };
}

} // namespace path_parser

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

    path_iterator(const path_t* path)
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

    path_iterator(const path_t* path, typename path_parser::state s) noexcept
        : m_path(path), m_position(path->m_path.size()), m_state(s) {}

public:

    reference operator*() const noexcept { return m_element; }
    pointer operator->() const noexcept { return std::addressof(m_element); }

    path_iterator& operator++()
    {
        if (m_state == path_parser::state::end)
        {
            return *this;
        }

        const auto& path = m_path->native();

        if (m_state <= path_parser::state::root_name)
        {
            const auto root_directory = path_parser::parse_root_directory(path);
            
            if (m_state == path_parser::state::begin && root_directory.pos != 0)
            {
                // The iterator was just initialized and the first element is the root name
                m_element.m_path = path.substr(0, root_directory.pos);
                m_position = root_directory.pos;
                m_state = path_parser::state::root_name;
                return *this;
            }
            else if (root_directory.size != 0)
            {
                // Root directory exists, so next is root directory
                m_element.m_path = path.substr(root_directory.pos, 1);
                m_position = root_directory.pos + root_directory.size;
                m_state = path_parser::state::root_directory;
                return *this;
            }
        }

        const size_t path_size = path.size();

        if (m_position == path_size)
        {
            // We are at the end of the path
            m_element.clear();
            m_position = path_size;
            m_state = path_parser::state::end;
            return *this;
        }

        while (path_parser::is_directory_separator(path[m_position]))
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
        m_position = path_parser::find_separator(path.data(), start, path_size);
        m_element.m_path = path.substr(start, m_position - start);
        m_state = path_parser::state::relative_path;

        return *this;
    }

    path_iterator operator++(int)
    {
        path_iterator tmp = *this;
        ++*this;
        return tmp;
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

    const path_t* m_path = nullptr;
    path_t m_element;
    size_t m_position = 0;
    path_parser::state m_state = path_parser::state::begin;
};

} // namespace _priv

/**
 * @class path
 * @brief A class representing a file system path with cross-platform support.
 *
 * This class provides an abstraction for file system paths, offering methods for
 * constructing paths, traversing components, and performing common file system operations.
 * It supports iterating through path elements, handling path separators, and other utilities.
 */
class path
{
public:

    using value_type = _priv::path_value_type;
    using string_type = _priv::path_string_type;

    static constexpr value_type separator = PATH_SEPARATOR;
    static constexpr value_type preferred_separator = PATH_PREFERRED_SEPARATOR;
    static constexpr value_type dot = PATH_DOT;

    using iterator = _priv::path_iterator<path>;
    using const_iterator = iterator;

    friend iterator;

public:

    path() noexcept = default;
    ~path() = default;

    path(const path&) = default;
    path(path&&) noexcept = default;

    path& operator=(const path&) = default;
    path& operator=(path&&) noexcept = default;

    void swap(path& other) noexcept { std::swap(m_path, other.m_path); }

public:

    path(string_type&& s) noexcept : m_path(std::move(s)) {}

    path(const native_string& src) : m_path(src.native()) {}

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path(const Src& src) : m_path(str::string_cast<value_type>(src)) {}

    template <typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
    path(IT first, IT last)
    {
        str::string_cast<value_type>(first, last, std::back_inserter(m_path));
    }

    path& operator=(string_type&& s) noexcept
    {
        m_path = std::move(s);
        return *this;
    }

    path& assign(string_type&& s) noexcept
    {
        m_path = std::move(s);
        return *this;
    }

    path& operator=(const native_string& rhs)
    {
        m_path = rhs.native();
        return *this;
    }

    path& assign(const native_string& rhs)
    {
        m_path = rhs.native();
        return *this;
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path& operator=(const Src& rhs)
    {
        m_path = str::string_cast<value_type>(rhs);
        return *this;
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path& assign(const Src& rhs)
    {
        m_path = str::string_cast<value_type>(rhs);
        return *this;
    }

    template <typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
    path& assign(IT first, IT last)
    {
        m_path = str::string_cast<value_type>(first, last);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // concatenation
    ///////////////////////////////////////////////////////////////////////////////

    // https://en.cppreference.com/w/cpp/filesystem/path/append
    // https://github.com/microsoft/STL/blob/fec1c8b6a13e5411edebbddc3ad98258f5e282d2/stl/inc/filesystem#L712

    /**
     * @brief Appends a path to the current path, modifying the current path.
     *
     * This operator overload implements the path concatenation (using the `/` operator)
     * to append the given `rhs` path to the current `path` object. It handles various
     * conditions related to absolute and relative paths, ensuring the correct behavior
     * based on the type and structure of `rhs`.
     *
     * - If `rhs` is an absolute path, the current path is replaced by `rhs`.
     * - If `rhs` has a root directory, the current path may be cleared to form a new absolute path.
     * - If `rhs` does not contain a root directory, it is appended relative to the current path.
     * - It also ensures that directory separators are handled correctly, especially on Windows.
     *
     * @param rhs The path to append to the current path. This path is typically relative to the current path,
     * but it may also be absolute, in which case it may replace the current path.
     * @return A reference to the modified `path` object.
     */
    path& operator/=(const path& rhs)
    {
        // if rhs.is_absolute(), then op=(rhs)
        if (rhs.is_absolute())
        {
            return operator=(rhs);
        }

        const size_t rhs_size = rhs.m_path.size();
        const value_type* rhs_first = rhs.m_path.data();

        const size_t lhs_size = m_path.size();

#if defined(_VX_WINDOWS_PATH)

        const value_type* lhs_first = m_path.data();

        const auto lhs_root_name = _priv::path_parser::parse_root_name(m_path);
        const auto rhs_root_name = _priv::path_parser::parse_root_name(rhs.m_path);

        // if rhs.has_root_name() && rhs.root_name() != root_name(), then op=(rhs)
        if (rhs_root_name.size != 0 && !std::equal(
            lhs_first + lhs_root_name.pos, lhs_first + (lhs_root_name.pos + lhs_root_name.size),
            rhs_first + rhs_root_name.pos, rhs_first + (rhs_root_name.pos + rhs_root_name.size)))
        {
            return operator=(rhs);
        }

        const size_t rhs_start = rhs_root_name.size;
#else
        // No root names on POSIX
        constexpr size_t rhs_start = 0;
#endif // _VX_WINDOWS_PATH

        // if rhs.has_root_directory() removes any root directory and relative path from *this
        if (rhs_start != rhs_size && _priv::path_parser::is_directory_separator(rhs.m_path[rhs_start]))
        {
#if defined(_VX_WINDOWS_PATH)
            m_path.erase(lhs_root_name.size);
#else
            m_path.clear();
#endif // _VX_WINDOWS_PATH
        }
        else
        {
#if defined(_VX_WINDOWS_PATH)
            // Otherwise, if (!has_root_directory() && is_absolute()) || has_filename() appends path::preferred_separator
            if (lhs_root_name.size == lhs_size)
            {
                // Here, !has_root_directory() && !has_filename()
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
#endif // _VX_WINDOWS_PATH
            {
                // Here, has_root_directory() || has_filename()
                // If there is a trailing slash, the trailing slash might be part of root_directory.
                // If it is, has_root_directory() && !has_filename(), so the test fails.
                // If there is a trailing slash not part of root_directory, then !has_filename(), so only
                // (!has_root_directory() && is_absolute()) remains
                // Going through our root_name kinds:
                // X:cat\ needs a root_directory to be absolute
                // \\server\cat must have a root_directory to exist with a relative_path
                // \\?\ must have a root_directory to exist
                // As a result, the test fails if there is a trailing slash.
                // If there is no trailing slash, then has_filename, so the test passes.
                // Therefore, the test passes if and only if there is no trailing slash.
                if (lhs_size != 0 && !_priv::path_parser::is_directory_separator(m_path.back()))
                {
                    m_path.push_back(preferred_separator);
                }
            }
        }

        // Then appends the native format pathname of rhs, omitting any root-name from its generic format
        // pathname, to the native format pathname.
        m_path.append(rhs_first + rhs_start, rhs_size - rhs_start);
        return *this;
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path& operator/=(const Src& rhs)
    {
        return operator/=(path{ rhs });
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path& append(const Src& rhs)
    {
        return operator/=(path{ rhs });
    }

    template <typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
    path& append(IT first, IT last)
    {
        return operator/=(path{ first, last });
    }

    /**
     * @brief Appends another path to the current path.
     *
     * This operator overload appends the `m_path` of another `path` object (`rhs`) to the current `path` object.
     * It internally calls the string-based `operator+=` to perform the actual concatenation.
     *
     * @param rhs The path object whose path (`m_path`) is appended to the current path.
     * @return A reference to the modified `path` object.
     */
    path& operator+=(const path& rhs)
    {
        return operator+=(rhs.m_path);
    }

    path& operator+=(const string_type& rhs)
    {
        m_path += rhs;
        return *this;
    }

#if defined(__cpp_lib_string_view)

    path& operator+=(const std::basic_string_view<value_type> rhs)
    {
        m_path += rhs;
        return *this;
    }

#endif // __cpp_lib_string_view

    path& operator+=(const value_type* rhs)
    {
        m_path += rhs;
        return *this;
    }

    path& operator+=(const value_type rhs)
    {
        m_path += rhs;
        return *this;
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path& operator+=(const Src& rhs)
    {
        return operator+=(path{ rhs }.m_path);
    }

    template <typename Src, VX_REQUIRES(type_traits::is_char<Src>::value)>
    path& operator+=(const Src& rhs)
    {
        return operator+=(path{ &rhs, &rhs + 1 }.m_path);
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    path& concat(const Src& rhs)
    {
        return operator+=(path{ rhs }.m_path);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
    path& concat(IT first, IT last)
    {
        return operator+=(path{ first, last }.m_path);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // modifiers
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Clears the current path.
     *
     * This function clears the internal `m_path` string, effectively making the path empty.
     * It is a noexcept operation, meaning it guarantees not to throw exceptions.
     */
    inline void clear() noexcept { m_path.clear(); }

    /**
     * @brief Converts the path to use the preferred separator.
     *
     * This function replaces all occurrences of the `separator` in the current path with the
     * `preferred_separator`. The `preferred_separator` is typically the separator that is
     * recommended or native for the platform (e.g., `/` on UNIX-like systems, `\` on Windows).
     *
     * @return A reference to the modified `path` object.
     */
    inline path& make_preferred()
    {
        std::replace(m_path.begin(), m_path.end(), separator, preferred_separator);
        return *this;
    }

    /**
     * @brief Removes the filename from the path.
     *
     * This function removes the filename (if any) from the current path, leaving only the
     * directory part. It uses the `path_parser` to determine the position of the filename
     * and then erases it from the internal `m_path`.
     *
     * @return A reference to the modified `path` object.
     */
    inline path& remove_filename()
    {
        const auto filename = _priv::path_parser::parse_filename(m_path);
        m_path.erase(filename.pos);
        return *this;
    }

    /**
     * @brief Removes the extension from the path.
     *
     * This function removes the extension (if any) from the filename in the path. It uses
     * the `path_parser` to find the extension and erase it from the internal `m_path`.
     *
     * @return A reference to the modified `path` object.
     */
    inline path& remove_extension()
    {
        const auto extension = _priv::path_parser::parse_extension(m_path);
        m_path.erase(extension.pos);
        return *this;
    }

    /**
     * @brief Removes the last component of the path.
     *
     * This function removes the last component (e.g., filename or directory) of the path. It
     * handles both root paths and directory separators appropriately. If the path is rooted,
     * it ensures that the root directory is preserved. It effectively truncates the path, removing
     * the last segment or the trailing directory separators.
     *
     * @return A reference to the modified `path` object.
     */
    inline path& pop_back()
    {
        const auto root_directory = _priv::path_parser::parse_root_directory(m_path.c_str());
        const size_t root_path_end = (root_directory.size > 0) ? root_directory.pos + 1 : root_directory.pos;

        size_t new_end = m_path.size();
        while (new_end != root_path_end && !_priv::path_parser::is_directory_separator(m_path[new_end - 1]))
        {
            --new_end;
        }
        while (new_end != root_path_end && _priv::path_parser::is_directory_separator(m_path[new_end - 1]))
        {
            --new_end;
        }

        m_path.erase(new_end);
        return *this;
    }

    /**
     * @brief Replaces the extension of the path with a new extension.
     *
     * This function removes the current file extension (if any) and appends the new extension provided
     * in `new_extension`. The extension is prepended with a dot (`.`) unless it already starts with one.
     *
     * @param new_extension The new extension to replace the current one. If empty, the extension is removed.
     * @return A reference to the modified `path` object.
     */
    inline path& replace_extension(const path& new_extension)
    {
        *this = remove_extension();
        if (!new_extension.m_path.empty() && new_extension.m_path[0] != dot)
        {
            m_path.push_back(dot);
        }
        return operator+=(new_extension);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // format observers
    ///////////////////////////////////////////////////////////////////////////////

    inline const value_type* c_str() const noexcept { return m_path.c_str(); }
    inline const string_type& native() const noexcept { return m_path; }
    inline operator string_type() const { return m_path; }
    inline operator native_string() const { return native_string{ m_path }; }

    /**
     * @brief Converts the path to a string representation.
     *
     * This template function converts the internal path (`m_path`) to a `std::basic_string` of the specified
     * character type (`char_t`). It uses `str::string_cast` to perform the conversion.
     *
     * @tparam char_t The character type to use for the string representation. Defaults to `char`.
     * @tparam Traits The character traits type to use. Defaults to `std::char_traits<char_t>`.
     * @tparam Alloc The allocator type to use. Defaults to `std::allocator<char_t>`.
     *
     * @return A `std::basic_string<char_t, Traits, Alloc>` representing the current path.
     */
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

    // https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/stl/inc/filesystem#L1017

    /**
     * @brief Compares the current path with another path.
     *
     * This function compares the current path (`*this`) with another path (`rhs`). The comparison is done
     * lexicographically, taking into account various factors such as the root directory, root name, and relative
     * path elements. It returns a negative, zero, or positive value based on the result of the comparison.
     *
     * The comparison follows these steps:
     * - It first compares the root directories of both paths.
     * - If the root directories are the same, it compares the paths element by element, considering directory
     *   separators and relative path components.
     * - It handles the case where the paths differ in terms of having a root directory or not, and ensures
     *   directory separators are treated correctly.
     *
     * @param rhs The path to compare with the current path.
     * @return A negative value if the current path is lexicographically less than `rhs`,
     * zero if both paths are equal, or a positive value if the current path is greater than `rhs`.
     */
    int compare(const string_type& rhs) const
    {
        const auto lhs_root_directory = _priv::path_parser::parse_root_directory(m_path);
        const auto rhs_root_directory = _priv::path_parser::parse_root_directory(rhs);

        auto lhs_it = m_path.begin();
        auto rhs_it = rhs.begin();

#if defined(_VX_WINDOWS_PATH)

        const auto lhs_root_name_end = lhs_it + lhs_root_directory.pos;
        const auto rhs_root_name_end = rhs_it + rhs_root_directory.pos;

        // If root_name().native().compare(p.root_name().native()) is nonzero, returns that value.
        while (lhs_it != lhs_root_name_end && rhs_it != rhs_root_name_end)
        {
            const int root_name_cmp = _priv::path_parser::compare(*lhs_it, *rhs_it);
            if (root_name_cmp != 0)
            {
                return root_name_cmp;
            }

            ++lhs_it;
            ++rhs_it;
        }

        if (lhs_it != lhs_root_name_end)
        {
            // lhs is longer, so it is greater
            return 1;
        }
        if (rhs_it != rhs_root_name_end)
        {
            // rhs is longer, so it is greater
            return -1;
        }

        lhs_it = lhs_root_name_end;
        rhs_it = rhs_root_name_end;

#endif // _VX_WINDOWS_PATH

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

        const auto lhs_last = m_path.end();
        const auto rhs_last = rhs.end();

        while (true)
        {
            const bool lhs_empty = lhs_it == lhs_last;
            const bool rhs_empty = rhs_it == rhs_last;
            const int empty_cmp = static_cast<int>(rhs_empty) - static_cast<int>(lhs_empty);
            if (lhs_empty || empty_cmp != 0)
            {
                return empty_cmp;
            }

            const bool lhs_is_sep = _priv::path_parser::is_directory_separator(*lhs_it);
            const bool rhs_is_sep = _priv::path_parser::is_directory_separator(*rhs_it);
            const int sep_cmp = static_cast<int>(rhs_is_sep) - static_cast<int>(lhs_is_sep);
            if (sep_cmp != 0)
            {
                return sep_cmp;
            }

            if (lhs_is_sep)
            {
                lhs_it = std::find_if_not(lhs_it + 1, lhs_last, _priv::path_parser::is_directory_separator);
                rhs_it = std::find_if_not(rhs_it + 1, rhs_last, _priv::path_parser::is_directory_separator);
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

    /**
     * @brief Returns a lexicographically normalized version of the path.
     *
     * This function returns a new `path` object that is lexicographically normalized. It removes redundant
     * components such as `.` (current directory) and `..` (parent directory), and ensures that the path is
     * in a consistent format. It also consolidates multiple consecutive directory separators into a single
     * separator and replaces root directory separators with the preferred separator.
     *
     * The normalization process involves the following steps:
     * 1. Replace each slash character in the root-name with `path::preferred_separator`.
     * 2. Replace consecutive directory separators with a single `preferred_separator`.
     * 3. Remove any instances of `.` (current directory) and any immediately following directory separators.
     * 4. Remove any instances of `..` (parent directory) and the corresponding previous directory components
     *    unless there is a root directory.
     * 5. If the path is empty, it adds a `.` (representing the current directory).
     * 6. If the last component is `..`, it removes any trailing directory separator.
     *
     * @return A new `path` object that represents the lexically normalized version of the current path.
     */
    path lexically_normal() const
    {
        using parse_state = _priv::path_parser::state;

        if (m_path.empty())
        {
            return path();
        }

        string_type normalized;
        normalized.reserve(m_path.size());

        std::vector<size_t> stack;
        stack.reserve(15);
        bool has_root_directory = false;

#       define last_is_dotdot() ( \
            normalized.size() >= 3 && \
            normalized.compare(stack.back(), 2, _PATH_TEXT("..")) == 0 && \
            normalized.back() == preferred_separator \
        )

        for (auto it = begin(); it != end(); ++it)
        {
            if (it.m_state == parse_state::root_name)
            {
                // 3. Replace each slash character in the root-name with path::preferred_separator.
                for (const value_type c : it->native())
                {
                    normalized.push_back((c == separator) ? preferred_separator : c);
                }

                continue;
            }

            else if (it.m_state == parse_state::root_directory)
            {
                // Root-directory will only ever be made up of separators.
                // We can just replace it with a single preferred separator.
                normalized.push_back(preferred_separator);
                has_root_directory = true;
                continue;
            }

            const string_type& p = it->native();
            if (p.empty())
            {
                continue;
            }

            // 4. Remove each dot and any immediately following directory-separator.
            else if (p == _PATH_TEXT("."))
            {
                continue;
            }

            else if (p == _PATH_TEXT(".."))
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
            if (_priv::path_parser::is_directory_separator(m_path[it.m_position]))
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
    // https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/stl/inc/filesystem#L1684

    /**
     * @brief Computes the lexicographical relative path from the given base path.
     *
     * This function computes the relative path from the current path to the specified `base` path.
     * The function follows the algorithm to determine the first mismatched element between the two paths.
     * Based on this mismatch, it calculates the necessary relative path in terms of `..` (parent directory)
     * and other elements. If the paths cannot be normalized into a relative path, it returns a default-constructed path.
     *
     * The function first checks various conditions that could prevent computing the relative path:
     * - The root names are different between the current path and the base path.
     * - The paths are not both absolute or both relative.
     * - The current path has no root directory while the base path has one.
     * - Any filename in the relative portion of the paths could be interpreted as a root-name.
     *
     * If any of these conditions are true, a default-constructed path is returned.
     * Otherwise, the relative path is calculated as follows:
     * 1. Find the first mismatched element between the current path and the base path.
     * 2. Calculate the number of parent directory (`..`) steps required to go from the base path
     *    to the first mismatch.
     * 3. Append the remaining elements from the current path after the mismatch to form the relative path.
     *
     * @param base The base path to compute the relative path from.
     * @return A new `path` object representing the relative path from the base path.
     */
    path lexically_relative(const path& base) const
    {
        path ret;

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
            return ret;
        }

        // Otherwise, first determines the first mismatched element
        // of *this and base as if by
        // auto [a, b] = mismatch(begin(), end(), base.begin(), base.end()).

        auto a_it = begin();
        auto b_it = base.begin();
        const auto this_last = end();
        const auto base_last = base.end();

        // same as std::mismatch()
        while (a_it != this_last && b_it != base_last)
        {
            if (*a_it != *b_it)
            {
                break;
            }

            ++a_it;
            ++b_it;
        }

        // If a == end() and b == base.end(), returns path(".").
        if (a_it == this_last && b_it == base_last)
        {
            ret = _PATH_TEXT(".");
            return ret;
        }

        // Otherwise, define N as the number of nonempty filename elements
        // that are neither dot nor dot-dot in [b, base.end()), minus the
        // number of dot-dot filename elements.

        int n = 0;

        for (; b_it != base_last; ++b_it)
        {
            if (b_it->empty())
            {
                continue;
            }
            else if (*b_it == _PATH_TEXT(".."))
            {
                --n;
            }
            else if (*b_it != _PATH_TEXT("."))
            {
                ++n;
            }
        }

        // If N < 0, returns a default-constructed path
        if (n < 0)
        {
            return ret;
        }

        // If N = 0 and a == end() || a->empty(), returns path("."),
        if (n == 0 && (a_it == this_last || a_it->empty()))
        {
            ret = _PATH_TEXT(".");
            return ret;
        }

        // Otherwise returns an object composed from a default-constructed
        // path() followed by N applications of operator/=(path("..")),
        // followed by one application of operator/= for each element in
        // the half-open range[a, end()).

        while (n--)
        {
            ret /= _PATH_TEXT("..");
        }

        // To fix this isse we use += instead of /= here
        // https://cplusplus.github.io/LWG/issue3070
        for (; a_it != this_last; ++a_it)
        {
            if (!ret.empty() && ret.m_path.back() != preferred_separator)
            {
                ret += preferred_separator;
            }

            ret += *a_it;
        }

        return ret;
    }

    /**
     * @brief Computes the lexicographical proximate path from the given base path.
     *
     * This function computes the proximate path to the current path with respect to a specified `base` path.
     * It first attempts to calculate the relative path using the `lexically_relative()` function. If the relative
     * path is empty (indicating that the paths are the same or close enough), it simply returns the current path.
     * Otherwise, it returns the relative path, making it easier to represent the proximity of the two paths.
     *
     * @param base The base path to compute the proximate path from.
     * @return A new `path` object representing the proximate path from the base path, or the current path if no relative path exists.
     */
    path lexically_proximate(const path& base) const
    {
        const path& p = lexically_relative(base);
        return !p.empty() ? p : *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // decomposition
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Returns the root name of the path.
     *
     * The root name typically represents the drive or partition part of a path (e.g., "C:" on Windows or "/" on Unix-like systems).
     * It extracts the root name of the path using the internal path parser and returns it as a `path` object.
     *
     * @return A `path` object representing the root name of the path.
     */
    path root_name() const
    {
        const auto p = _priv::path_parser::parse_root_name(m_path);
        return m_path.substr(p.pos, p.size);
    }

    /**
     * @brief Returns the root directory of the path.
     *
     * The root directory represents the directory immediately under the root of the filesystem (e.g., "/" or "C:\" on Windows).
     * This function parses and returns the root directory of the path as a `path` object.
     *
     * @return A `path` object representing the root directory of the path.
     */
    path root_directory() const
    {
        const auto p = _priv::path_parser::parse_root_directory(m_path);
        return m_path.substr(p.pos, p.size);
    }

    /**
     * @brief Returns the root path of the path.
     *
     * The root path includes both the root name (e.g., "C:") and the root directory (e.g., "\").
     * This function returns the combined root part of the path as a `path` object.
     *
     * @return A `path` object representing the root path.
     */
    path root_path() const
    {
        const auto p = _priv::path_parser::parse_root_path(m_path);
        return m_path.substr(p.pos, p.size);
    }

    /**
     * @brief Returns the relative path of the current path.
     *
     * The relative path is the portion of the path that does not include the root part (i.e., the path excluding the root name, root directory, and the root path).
     * This function extracts and returns the relative portion of the path as a `path` object.
     *
     * @return A `path` object representing the relative path.
     */
    path relative_path() const
    {
        const auto p = _priv::path_parser::parse_relative_path(m_path);
        return m_path.substr(p.pos, p.size);
    }

    /**
     * @brief Returns the parent path of the current path.
     *
     * The parent path is the portion of the path that points to the parent directory of the current directory.
     * This function extracts and returns the parent path as a `path` object.
     *
     * @return A `path` object representing the parent path.
     */
    path parent_path() const
    {
        const auto p = _priv::path_parser::parse_parent_path(m_path);
        return m_path.substr(p.pos, p.size);
    }

    /**
     * @brief Returns the filename component of the path.
     *
     * The filename represents the final element in the path, typically a file or directory name.
     * This function extracts and returns the filename portion of the path as a `path` object.
     *
     * @return A `path` object representing the filename of the path.
     */
    path filename() const
    {
        const auto p = _priv::path_parser::parse_filename(m_path);
        return m_path.substr(p.pos, p.size);
    }

    /**
     * @brief Returns the stem of the filename (i.e., the filename without its extension).
     *
     * The stem is the part of the filename excluding its extension. For example, for a path like "file.txt",
     * the stem would be "file".
     * This function extracts and returns the stem as a `path` object.
     *
     * @return A `path` object representing the stem of the filename.
     */
    path stem() const
    {
        const auto p = _priv::path_parser::parse_stem(m_path);
        return m_path.substr(p.pos, p.size);
    }

    /**
     * @brief Returns the extension of the filename in the path.
     *
     * The extension is the part of the filename that follows the last period (e.g., ".txt" in "file.txt").
     * This function extracts and returns the extension of the filename as a `path` object.
     *
     * @return A `path` object representing the extension of the filename.
     */
    path extension() const
    {
        const auto p = _priv::path_parser::parse_extension(m_path);
        return m_path.substr(p.pos, p.size);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // queries
    ///////////////////////////////////////////////////////////////////////////////

    bool empty() const noexcept { return m_path.empty(); }
    size_t size() const noexcept { return m_path.size(); }

    /**
     * @brief Checks if the path is either a dot (`"."`) or dot-dot (`".."`).
     *
     * This function checks if the current path represents the current directory (`"."`) or the parent directory (`".."`).
     * These are special paths used for navigating the filesystem.
     *
     * @return `true` if the path is `.` or `..`, `false` otherwise.
     */
    bool is_dot_or_dotdot() const noexcept { return m_path == _PATH_TEXT(".") || m_path == _PATH_TEXT(".."); }

    /**
     * @brief Checks if the path has a root path.
     *
     * This function checks if the path includes a root path, which represents the absolute beginning of a filesystem path.
     *
     * @return `true` if the path has a root path, `false` otherwise.
     */
    bool has_root_path() const noexcept { return _priv::path_parser::parse_root_path(m_path).size != 0; }

    /**
     * @brief Checks if the path has a root name.
     *
     * This function checks if the path includes a root name, such as a drive letter or a root directory on Unix-like systems.
     *
     * @return `true` if the path has a root name, `false` otherwise.
     */
    bool has_root_name() const noexcept { return _priv::path_parser::parse_root_name(m_path).size != 0; }

    /**
     * @brief Checks if the path has a root directory.
     *
     * This function checks if the path includes a root directory, like `/` on Unix-like systems or `C:\` on Windows.
     *
     * @return `true` if the path has a root directory, `false` otherwise.
     */
    bool has_root_directory() const noexcept { return _priv::path_parser::parse_root_directory(m_path).size != 0; }

    /**
     * @brief Checks if the path has a relative path.
     *
     * This function checks if the path includes a relative path, meaning it does not start with a root directory.
     *
     * @return `true` if the path has a relative path, `false` otherwise.
     */
    bool has_relative_path() const noexcept { return _priv::path_parser::parse_relative_path(m_path).size != 0; }

    /**
     * @brief Checks if the path has a parent path.
     *
     * This function checks if the path includes a parent path, which typically points to the parent directory.
     *
     * @return `true` if the path has a parent path, `false` otherwise.
     */
    bool has_parent_path() const noexcept { return _priv::path_parser::parse_parent_path(m_path).size != 0; }

    /**
     * @brief Checks if the path has a filename.
     *
     * This function checks if the path includes a filename, meaning it ends with a file or directory name.
     *
     * @return `true` if the path has a filename, `false` otherwise.
     */
    bool has_filename() const noexcept { return _priv::path_parser::parse_filename(m_path).size != 0; }

    /**
     * @brief Checks if the path has a stem.
     *
     * This function checks if the path includes a stem, which is the filename without the extension.
     *
     * @return `true` if the path has a stem, `false` otherwise.
     */
    bool has_stem() const noexcept { return _priv::path_parser::parse_stem(m_path).size != 0; }

    /**
     * @brief Checks if the path has an extension.
     *
     * This function checks if the path includes a file extension (e.g., `.txt`).
     *
     * @return `true` if the path has an extension, `false` otherwise.
     */
    bool has_extension() const noexcept { return _priv::path_parser::parse_extension(m_path).size != 0; }

    /**
     * @brief Checks if the path is relative.
     *
     * A relative path is one that does not begin with a root directory or drive letter.
     * This function checks if the path does not have an absolute form.
     *
     * @return `true` if the path is relative, `false` if it is absolute.
     */
    bool is_relative() const noexcept { return !is_absolute(); }
     
    // https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/stl/inc/filesystem#L1165
    // https://github.com/gcc-mirror/gcc/blob/e8262c9041feddd7446840a9532cf458452f3587/libstdc%2B%2B-v3/include/bits/fs_path.h#L1333

    /**
     * @brief Checks if the path is absolute.
     *
     * An absolute path is one that starts with a root directory or drive letter (e.g., `C:/` on Windows or `/` on Unix-like systems).
     * This function checks if the path follows that structure, indicating it's an absolute path.
     *
     * On Windows, the function checks if the path has a drive letter followed by a colon and a directory separator.
     * On Unix-like systems, it checks if the path starts with a directory separator.
     *
     * @return `true` if the path is absolute, `false` if it is relative.
     */
    bool is_absolute() const noexcept
    {
#if defined(_VX_WINDOWS_PATH)

        const size_t size = m_path.size();
        // Has drive letter "X:*"
        if (size >= 2 && _priv::path_parser::is_letter(m_path[0]))
        {
            // Check for "X:/cat" but not "X:cat"
            return size >= 3 && _priv::path_parser::is_directory_separator(m_path[2]);
        }

        return _priv::path_parser::find_root_directory_start(m_path.data(), size) != 0;

#else

        return !m_path.empty() && _priv::path_parser::is_directory_separator(m_path[0]);

#endif // _VX_WINDOWS_PATH
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
        return iterator(this, _priv::path_parser::state::end);
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

    friend inline bool operator==(const path& lhs, const path& rhs)
    {
        return lhs.compare(rhs) == 0;
    }

    friend inline bool operator!=(const path& lhs, const path& rhs)
    {
        return lhs.compare(rhs) != 0;
    }

    friend inline bool operator>(const path& lhs, const path& rhs)
    {
        return lhs.compare(rhs) > 0;
    }

    friend inline bool operator<(const path& lhs, const path& rhs)
    {
        return lhs.compare(rhs) < 0;
    }

    friend inline bool operator>=(const path& lhs, const path& rhs)
    {
        return lhs.compare(rhs) >= 0;
    }

    friend inline bool operator<=(const path& lhs, const path& rhs)
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
    size_t operator()(const vx::os::path& p) const noexcept
    {
        vx::crypto::fnv1a fnv1a;

        const auto& text = p.native();
        const size_t size = text.size();
        auto it = text.data();
        const auto last = it + size;

#   if defined (_VX_WINDOWS_PATH)

        const auto root_name_end = vx::os::_priv::path_parser::find_root_directory_start(it, size);
        fnv1a.update(it, it += root_name_end);

#   endif // _VX_WINDOWS_PATH

        bool last_was_slash = false;
        for (; it != last; ++it)
        {
            if (vx::os::_priv::path_parser::is_directory_separator(*it))
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

#undef _PATH_TEXT
#undef _VX_WINDOWS_PATH

} // namespace std
