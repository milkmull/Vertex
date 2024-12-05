#pragma once

#include "vertex/system/platform_config.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/util/io/quoted.hpp"

// https://docs.godotengine.org/en/stable/classes/class_fileaccess.html#class-fileaccess
// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_filesystem.h
// https://en.cppreference.com/w/cpp/filesystem

namespace vx {
namespace os {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// path
///////////////////////////////////////////////////////////////////////////////

class path;

#if defined(__VX_OS_WINDOWS_FILESYSTEM)

#   define __VX_FS_TEXT(x) L##x

#else

#   define __VX_FS_TEXT(x) x

#endif // __VX_OS_WINDOWS_FILESYSTEM

namespace __detail {

template <typename path_t>
class path_iterator
{
    friend path;

public:

    using iterator_category = std::input_iterator_tag;
    using value_type = path_t;
    using difference_type = ptrdiff_t;
    using pointer = const path_t*;
    using reference = const path_t&;

public:

    path_iterator() = default;

    path_iterator(const path_t* path) noexcept
        : m_path(path)
    {
        operator++();
    }

    path_iterator(const path_iterator&) = default;
    path_iterator(path_iterator&&) = default;
    path_iterator& operator=(const path_iterator&) = default;
    path_iterator& operator=(path_iterator&&) = default;

private:

    enum state
    {
        BEGIN,
        ROOT_NAME,
        ROOT_DIRECTORY,
        RELATIVE_PATH,
        END
    };

    path_iterator(const path_t* path, state s) noexcept
        : m_path(path), m_position(path->m_path.size()), m_state(s) {}

public:

    reference operator*() const noexcept { return m_element; }
    pointer operator->() const noexcept { return std::addressof(m_element); }

    path_iterator& operator++()
    {
        if (m_state == state::END)
        {
            return *this;
        }

        const auto& path = m_path->native();

        if (m_state <= state::ROOT_NAME)
        {
            const auto root_directory = path_t::parser::parse_root_directory(path);
            
            if (m_state == state::BEGIN && root_directory.pos != 0)
            {
                // The iterator was just initialized and the first element is the root name
                m_element.m_path = path.substr(0, root_directory.pos);
                m_position = root_directory.pos;
                m_state = state::ROOT_NAME;
                return *this;
            }
            else if (root_directory.size != 0)
            {
                // Root directory exists, so next is root directory
                m_element.m_path = path.substr(root_directory.pos, root_directory.size);
                m_position = root_directory.pos + root_directory.size;
                m_state = state::ROOT_DIRECTORY;
                return *this;
            }
        }

        const size_t path_size = path.size();

        if (m_position == path_size)
        {
            // We are at the end of the path
            m_element.clear();
            m_position = path_size;
            m_state = state::END;
            return *this;
        }

        while (path_t::parser::is_directory_separator(path[m_position]))
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
        m_position = path_t::parser::find_separator(path.data(), start, path_size);
        m_element.m_path = path.substr(start, m_position - start);
        m_state = state::RELATIVE_PATH;

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
    state m_state = state::BEGIN;
};

} // namespace __detail

class path
{
public:

#if defined(__VX_OS_WINDOWS_FILESYSTEM)

    using value_type = wchar_t;
    static constexpr value_type separator = L'/';
    static constexpr value_type preferred_separator = L'\\';
    static constexpr value_type dot = L'.';

#else // posix

    using value_type = char;
    static constexpr value_type separator = '/';
    static constexpr value_type preferred_separator = '/';
    static constexpr value_type dot = '.';

#endif // __VX_OS_WINDOWS_FILESYSTEM

    using string_type = std::basic_string<value_type>;

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
    path(IT first, IT last) : m_path(str::string_cast<value_type>(first, last)) {}

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

    path& operator/=(const path& rhs);

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
        const auto filename = parser::parse_filename(m_path);
        m_path.erase(m_path.begin() + filename.pos);
        return *this;
    }

    inline path& remove_extension()
    {
        const auto extension = parser::parse_extension(m_path);
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

    int compare(const path& rhs) const noexcept;

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    int compare(const Src& src) const
    {
        return compare(path(src));
    }

    ///////////////////////////////////////////////////////////////////////////////
    // decomposition
    ///////////////////////////////////////////////////////////////////////////////

    path root_name() const
    {
        const auto p = parser::parse_root_name(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path root_directory() const
    {
        const auto p = parser::parse_root_directory(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path root_path() const
    {
        const auto p = parser::parse_root_path(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path relative_path() const
    {
        const auto p = parser::parse_relative_path(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path parent_path() const
    {
        const auto p = parser::parse_parent_path(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path filename() const
    {
        const auto p = parser::parse_filename(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path stem() const
    {
        const auto p = parser::parse_stem(m_path);
        return m_path.substr(p.pos, p.size);
    }

    path extension() const
    {
        const auto p = parser::parse_extension(m_path);
        return m_path.substr(p.pos, p.size);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // queries
    ///////////////////////////////////////////////////////////////////////////////

    bool empty() const noexcept { return m_path.empty(); }

    bool has_root_path() const      { return parser::parse_root_path(m_path).size != 0; }
    bool has_root_name() const      { return parser::parse_root_name(m_path).size != 0; }
    bool has_root_directory() const { return parser::parse_root_directory(m_path).size != 0; }
    bool has_relative_path() const  { return parser::parse_relative_path(m_path).size != 0; }
    bool has_parent_path() const    { return parser::parse_parent_path(m_path).size != 0; }
    bool has_filename() const       { return parser::parse_filename(m_path).size != 0; }
    bool has_stem() const           { return parser::parse_stem(m_path).size != 0; }
    bool has_extension() const      { return parser::parse_extension(m_path).size != 0; }

    bool is_relative() const { return !is_absolute(); }
    bool is_absolute() const
    {
#if defined(__VX_OS_WINDOWS_FILESYSTEM)
        return has_root_name() && has_root_directory();
#else
        return has_root_directory();
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
        return iterator(this, iterator::state::END);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // non member functions
    ///////////////////////////////////////////////////////////////////////////////

    friend inline bool operator==(const path& lhs, const path& rhs) noexcept
    {
        return lhs.m_path == rhs.m_path;
    }

    friend inline bool operator!=(const path& lhs, const path& rhs) noexcept
    {
        return !(lhs.m_path == rhs.m_path);
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

    ///////////////////////////////////////////////////////////////////////////////
    // parsing
    ///////////////////////////////////////////////////////////////////////////////

    struct parser
    {
        struct substring
        {
            size_t pos;
            size_t size;
        };

        static constexpr bool is_directory_separator(value_type c)
        {
            return c == separator
#if defined(__VX_OS_WINDOWS_FILESYSTEM)
                || c == preferred_separator
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
            return (c >= __VX_FS_TEXT('A') && c <= __VX_FS_TEXT('Z'))
                || (c >= __VX_FS_TEXT('a') && c <= __VX_FS_TEXT('z'));
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

        static size_t find_root_name_off(const value_type* first, size_t size);

        static substring parse_root_name(const string_type& s);
        static substring parse_root_directory(const string_type& s);
        static substring parse_root_path(const string_type& s);
        static substring parse_relative_path(const string_type& s);
        static substring parse_parent_path(const string_type& s);
        static substring parse_filename(const string_type& s);
        static substring parse_stem(const string_type& s);
        static substring parse_extension(const string_type& s);
    };

private:

    string_type m_path;
};

} // namespace filesystem
} // namespace os
} // namespace vx