#include "vertex/os/filesystem/path.hpp"

namespace vx {
namespace os {
namespace filesystem {

using value_type = typename path::value_type;

///////////////////////////////////////////////////////////////////////////////
// parsers
///////////////////////////////////////////////////////////////////////////////

// Returns a pointer to the end of the root name if it exists, otherwise last
size_t path::parser::find_root_name_off(const value_type* first, size_t size)
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

path::parser::substring path::parser::parse_root_name(const string_type& s)
{
    const size_t size = find_root_name_off(s.c_str(), s.size());
    return substring{ 0, size };
}

path::parser::substring path::parser::parse_root_directory(const string_type& s)
{
    const size_t size = s.size();
    const size_t off = find_root_name_off(s.c_str(), size);

    // Count consecutive directory separators starting from offset.
    size_t count = 0;
    while (off + count < size && is_directory_separator(s[off + count]))
    {
        ++count;
    }

    return substring{ off, count };
}

path::parser::substring path::parser::parse_root_path(const string_type& s)
{
    const auto root_directory = parse_root_directory(s);
    return substring{ 0, root_directory.pos + root_directory.size };
}

path::parser::substring path::parser::parse_relative_path(const string_type& s)
{
    const auto root_path = parse_root_path(s);
    return substring{ root_path.size, s.size() - root_path.size };
}

path::parser::substring path::parser::parse_parent_path(const string_type& s)
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

path::parser::substring path::parser::parse_filename(const string_type& s)
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

path::parser::substring path::parser::parse_stem(const string_type& s)
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

path::parser::substring path::parser::parse_extension(const string_type& s)
{
    const size_t size = s.size();

    if (size <= 1)
    {
        return substring{ size, 0 };
    }

    // size >= 2
    size_t i = size;

    if (s[--i] == dot)
    {
        if (
            // case "/." or ":."
            is_element_separator(s[i]) ||                                
            // case "..", "x/..", or "x:.."
            (s[i] == dot && (size == 2 || is_element_separator(s[i - 1])))
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
        if (s[--i] == dot)
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

///////////////////////////////////////////////////////////////////////////////
// concatenation
///////////////////////////////////////////////////////////////////////////////

// https://en.cppreference.com/w/cpp/filesystem/path/append
// https://github.com/microsoft/STL/blob/fec1c8b6a13e5411edebbddc3ad98258f5e282d2/stl/inc/filesystem#L712

path& path::operator/=(const path& rhs)
{
    // if rhs.is_absolute(), then op=(rhs)
    if (rhs.is_absolute())
    {
        return operator=(rhs);
    }

    const value_type* lhs_first = m_path.c_str();
    const size_t lhs_size = m_path.size();
    const auto lhs_root_name = parser::parse_root_name(m_path);

    const value_type* rhs_first = rhs.m_path.c_str();
    const size_t rhs_size = rhs.m_path.size();
    const auto rhs_root_name = parser::parse_root_name(rhs.m_path);

    // if rhs.has_root_name() && rhs.root_name() != root_name(), then op=(rhs)
    if (rhs_root_name.size != 0 && !std::equal(
        lhs_first + lhs_root_name.pos, lhs_first + (lhs_root_name.pos + lhs_root_name.size),
        rhs_first + rhs_root_name.pos, rhs_first + (rhs_root_name.pos + rhs_root_name.size)))
    {
        return operator=(rhs);
    }

    // if rhs.has_root_directory() removes any root directory and relative path from *this
    if (rhs_root_name.pos + rhs_root_name.size != rhs_size &&
        parser::is_directory_separator(rhs.m_path[rhs_root_name.pos + rhs_root_name.size]))
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
            if (!parser::is_directory_separator(m_path[lhs_size - 1]))
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

///////////////////////////////////////////////////////////////////////////////
// decomposition
///////////////////////////////////////////////////////////////////////////////



} // namespace filesystem
} // namespace os
} // namespace vx