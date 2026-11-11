#pragma once

#include "vertex/config/os.hpp"
#include "vertex/os/native_string.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/temporary_string.hpp"

#if defined(VX_TESING_PATH)

    #if defined(_VX_TESTING_WINDOWS_PATH)
        #define VX_WINDOWS_PATH
    #endif

#elif defined(VX_OS_WINDOWS)

    #define VX_WINDOWS_PATH

#endif // VX_TESING_PATH

// https://en.cppreference.com/w/cpp/filesystem

namespace vx {

template <typename T>
class basic_path_view;

namespace _path_priv {

enum class parse_state
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

// anything with data()/size() over C (strings, string views, path views...)
template <typename S, typename C, typename = void>
struct is_path_source : std::false_type
{};

template <typename S, typename C>
struct is_path_source<S, C, std::void_t<decltype(std::declval<const S&>().data()), decltype(std::declval<const S&>().size())>> : std::is_same<std::remove_cv_t<std::remove_pointer_t<decltype(std::declval<const S&>().data())>>, C>
{};

template <typename C>
struct path_traits
{
    using char_type = C;
    using path_string_view = vx::str::basic_string_view<char_type>;
    using path_temporary_string = vx::str::basic_temporary_string<char_type, false>;

    static constexpr char_type path_separator = static_cast<char_type>('/');

#if defined(VX_WINDOWS_PATH)
    static constexpr char_type path_preferred_separator = static_cast<char_type>('\\');
#else
    static constexpr char_type path_preferred_separator = path_separator;
#endif

    static constexpr char_type path_dot = static_cast<char_type>('.');
    static constexpr char_type path_colon = static_cast<char_type>(':');

    static constexpr char_type dot_string[] = { path_dot, 0 };
    static constexpr char_type dotdot_string[] = { path_dot, path_dot, 0 };

    //=============================================================================
    // parsing
    //=============================================================================

    static constexpr bool is_directory_separator(char_type c) noexcept
    {
#if defined(VX_WINDOWS_PATH)
        return c == path_separator || c == path_preferred_separator;
#else
        return c == path_separator;
#endif
    }

    static constexpr bool is_letter(char_type c) noexcept
    {
        return (c >= static_cast<char_type>('A') && c <= static_cast<char_type>('Z')) || (c >= static_cast<char_type>('a') && c <= static_cast<char_type>('z'));
    }

    static size_t find_separator(const char_type* p, size_t off, size_t size) noexcept
    {
        size_t pos = off;

        while (pos < size && !is_directory_separator(p[pos]))
        {
            ++pos;
        }

        return pos;
    }

    static constexpr int char_compare(const char_type c1, const char_type c2) noexcept
    {
        return (is_directory_separator(c1) && is_directory_separator(c2)) ? 0 : (static_cast<int>(c1) - static_cast<int>(c2));
    }

    static bool is_dot(path_string_view v) noexcept
    {
        return v.size() == 1 && v.data()[0] == path_dot;
    }

    static bool is_dotdot(path_string_view v) noexcept
    {
        return v.size() == 2 && v.data()[0] == path_dot && v.data()[1] == path_dot;
    }

    // Start of the last element in s[root_end, end). `end` must not include a trailing separator.
    static size_t last_element_start(const char_type* s, size_t root_end, size_t end) noexcept
    {
        size_t i = end;

        while (i > root_end && !is_directory_separator(s[i - 1]))
        {
            --i;
        }

        return i;
    }

    // https://github.com/boostorg/filesystem/blob/30b312e5c0335831af61ad16802e888f5fb344ea/src/path.cpp#L983
    // https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/stl/inc/filesystem#L371

    // Returns the start position of the root directory (== the size of the root name)
    static size_t find_root_directory_start(
        VX_MAYBE_UNUSED const char_type* first,
        VX_MAYBE_UNUSED size_t size) noexcept
    {
        // Only windows has root names
#if defined(VX_WINDOWS_PATH)

        constexpr char_type question = static_cast<char_type>('?');

        // A valid root name requires at least 2 characters (e.g., "C:")
        if (size < 2)
        {
            return 0;
        }

        // Drive letter root name (e.g., "C:")
        if (is_letter(first[0]) && first[1] == path_colon)
        {
            return 2;
        }

        // If the first character is not a directory separator, there is no root name
        if (!is_directory_separator(first[0]))
        {
            return 0;
        }

        // https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file
        // cases "\\?\*", "\\.\*", "\??\*" where * is anything EXCEPT a slash
        if (size >= 4 && is_directory_separator(first[3]) && (size == 4 || !is_directory_separator(first[4])) && ((is_directory_separator(first[1]) && (first[2] == question || first[2] == path_dot)) || (first[1] == question && first[2] == question)))
        {
            // "\\?\C:*" (https://cplusplus.github.io/LWG/issue3699)
            if (size >= 6 && is_letter(first[4]) && first[5] == path_colon)
            {
                return 6;
            }

            return 3;
        }

        // Paths starting with double slashes "\\*"
        if (is_directory_separator(first[1]))
        {
            if (size == 2 || is_directory_separator(first[2]))
            {
                // "\\" or "\\\*": a root directory followed by redundant separators
                return 0;
            }

            // "\\server*" (UNC path)
            return find_separator(first, 2, size);
        }

#endif // VX_WINDOWS_PATH

        return 0;
    }

    static substring parse_root_name(const char_type* s, size_t size) noexcept
    {
        return substring{ 0, find_root_directory_start(s, size) };
    }

    static substring parse_root_directory(const char_type* s, size_t size) noexcept
    {
        const size_t off = find_root_directory_start(s, size);

        size_t count = 0;
        while (off + count < size && is_directory_separator(s[off + count]))
        {
            ++count;
        }

        return substring{ off, count };
    }

    static substring parse_root_path(const char_type* s, size_t size) noexcept
    {
        const auto root_directory = parse_root_directory(s, size);
        return substring{ 0, root_directory.pos + root_directory.size };
    }

    static substring parse_relative_path(const char_type* s, size_t size) noexcept
    {
        const auto root_path = parse_root_path(s, size);
        return substring{ root_path.size, size - root_path.size };
    }

    static substring parse_parent_path(const char_type* s, size_t size) noexcept
    {
        const auto relative_path = parse_relative_path(s, size);
        size_t i = relative_path.pos + relative_path.size;

        // case 1: relative-path ends in a directory-path_separator,
        // remove the path_separator to remove "magic empty path".
        // example: R"(/cat/dog/\//\)"

        // case 2: relative-path doesn't end in a directory-path_separator,
        // remove the filename and last directory-path_separator to prevent
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

    static substring parse_filename(const char_type* s, size_t size) noexcept
    {
        const size_t root_name_end = find_root_directory_start(s, size);

        // Start searching backward from the end of the string
        for (size_t i = size; i > root_name_end; --i)
        {
            if (is_directory_separator(s[i - 1]))
            {
                return substring{ i, size - i };
            }
        }

        // If no directory path_separator is found, the entire string is the filename
        return substring{ root_name_end, size - root_name_end };
    }

    static substring find_extension(const char_type* s, size_t size, size_t filename_start) noexcept
    {
#if defined(VX_WINDOWS_PATH)

        // On Windows, file paths can contain alternate data streams (ADS),
        // specified using a colon (e.g., "file.txt:stream"). Since the extension
        // applies only to the main file and not to the stream, we must ensure
        // we stop parsing at the first ':' after the filename.
        for (size_t k = filename_start; k < size; ++k)
        {
            if (s[k] == path_colon)
            {
                size = k;
                break;
            }
        }

#endif // VX_WINDOWS_PATH

        // A filename of 0 or 1 characters has no extension
        if ((size - filename_start) <= 1)
        {
            return substring{ size, 0 };
        }

        // size >= 2
        size_t i = size;

        if (s[--i] == path_dot)
        {
            if (i == filename_start + 1 && s[filename_start] == path_dot)
            {
                // filename is ".."
                return substring{ size, 0 };
            }

            // "x."
            return substring{ i, 1 };
        }

        // If the filename starts with a dot (e.g., "/.config") it has no
        // extension, so the first character is never examined.
        while (i > filename_start + 1)
        {
            if (s[--i] == path_dot)
            {
                return substring{ i, size - i };
            }
        }

        return substring{ size, 0 };
    }

    static substring parse_extension(const char_type* s, size_t size) noexcept
    {
        return find_extension(s, size, parse_filename(s, size).pos);
    }

    static substring parse_stem(const char_type* s, size_t size) noexcept
    {
        const auto filename = parse_filename(s, size);
        const auto extension = find_extension(s, size, filename.pos);
        return substring{ filename.pos, extension.pos - filename.pos };
    }

    static bool is_absolute(const char_type* s, size_t size) noexcept
    {
#if defined(VX_WINDOWS_PATH)

        // "X:/cat" is absolute, "X:cat" is not
        if (size >= 2 && is_letter(s[0]) && s[1] == path_colon)
        {
            return size >= 3 && is_directory_separator(s[2]);
        }

        return find_root_directory_start(s, size) != 0;

#else

        return (size > 0) && is_directory_separator(s[0]);

#endif // VX_WINDOWS_PATH
    }

    //=============================================================================
    // compare
    //=============================================================================

    // Element-wise comparison, as described by std::filesystem::path::compare.
    // Runs of separators compare equal to a single separator.
    static int compare(
        const char_type* const l,
        size_t ln,
        const char_type* const r,
        size_t rn) noexcept
    {

        const auto lroot = parse_root_directory(l, ln);
        const auto rroot = parse_root_directory(r, rn);

        size_t li = 0;
        size_t ri = 0;

#if defined(VX_WINDOWS_PATH)

        // If root_name().native().compare(p.root_name().native()) is nonzero, returns that value.
        while (li < lroot.pos && ri < rroot.pos)
        {
            const int c = char_compare(l[li], r[ri]);
            if (c != 0)
            {
                return c;
            }

            ++li;
            ++ri;
        }

        if (li < lroot.pos)
        {
            return 1;
        }
        if (ri < rroot.pos)
        {
            return -1;
        }

#endif // VX_WINDOWS_PATH

        // Otherwise, if has_root_directory() != p.has_root_directory(),
        // returns a value less than zero if has_root_directory() is false
        // and a value greater than zero otherwise.
        const int root_cmp = static_cast<int>(lroot.size != 0) - static_cast<int>(rroot.size != 0);
        if (root_cmp != 0)
        {
            return root_cmp;
        }

        // Otherwise returns a value less than, equal to or greater than 0
        // if the relative portion of the path (relative_path()) is respectively
        // lexicographically less than, equal to or greater than the relative
        // portion of p (p.relative_path()). Comparison is performed element-wise,
        // as if by iterating both paths from begin() to end() and comparing the
        // result of native() for each element.

        for (;;)
        {
            const bool lhs_end = li == ln;
            const bool rhs_end = ri == rn;
            if (lhs_end || rhs_end)
            {
                return static_cast<int>(rhs_end) - static_cast<int>(lhs_end);
            }

            const bool lhs_sep = is_directory_separator(l[li]);
            const bool rhs_sep = is_directory_separator(r[ri]);
            if (lhs_sep != rhs_sep)
            {
                return static_cast<int>(rhs_sep) - static_cast<int>(lhs_sep);
            }

            if (lhs_sep)
            {
                while (li < ln && is_directory_separator(l[li]))
                {
                    ++li;
                }
                while (ri < rn && is_directory_separator(r[ri]))
                {
                    ++ri;
                }
                continue;
            }

            const int c = static_cast<int>(l[li]) - static_cast<int>(r[ri]);
            if (c != 0)
            {
                return c;
            }

            ++li;
            ++ri;
        }
    }
};

//=============================================================================
// iterator
//=============================================================================

template <typename Owner>
class path_iterator
{
    using C = typename Owner::value_type;
    using traits = path_traits<C>;
    using view_type = basic_path_view<C>;

public:

    using iterator_category = std::input_iterator_tag;
    using value_type = view_type;
    using difference_type = ptrdiff_t;
    using pointer = const view_type*;
    using reference = const view_type&;

public:

    path_iterator() = default;

    // begin iterator
    explicit path_iterator(const Owner* owner) noexcept
        : m_owner(owner)
    {
        operator++();
    }

    // end iterator
    path_iterator(const Owner* owner, parse_state s) noexcept
        : m_owner(owner), m_position(owner->size()), m_state(s)
    {}

    reference operator*() const noexcept
    {
        return m_element;
    }

    pointer operator->() const noexcept
    {
        return &m_element;
    }

    parse_state state() const noexcept
    {
        return m_state;
    }

    // offset in the path just past the current element
    size_t position() const noexcept
    {
        return m_position;
    }

    path_iterator& operator++()
    {
        VX_ASSERT(m_owner != nullptr);

        if (m_state == parse_state::end)
        {
            return *this;
        }

        const C* const data = m_owner->data();
        const size_t path_size = m_owner->size();

        if (m_state <= parse_state::root_name)
        {
            const auto root_directory = traits::parse_root_directory(data, path_size);

            if (m_state == parse_state::begin && root_directory.pos != 0)
            {
                // The iterator was just initialized and the first element is the root name
                m_element = view_type(data, root_directory.pos);
                m_position = root_directory.pos;
                m_state = parse_state::root_name;
                return *this;
            }
            else if (root_directory.size != 0)
            {
                // Root directory exists, so next is root directory
                m_element = view_type(data + root_directory.pos, 1);
                m_position = root_directory.pos + root_directory.size;
                m_state = parse_state::root_directory;
                return *this;
            }
        }

        if (m_position == path_size)
        {
            // We are at the end of the path
            m_element = view_type();
            m_state = parse_state::end;
            return *this;
        }

        while (traits::is_directory_separator(data[m_position]))
        {
            // Advance to the start of the following path element
            if (++m_position == path_size)
            {
                // "magic" empty element selected
                m_element = view_type();
                m_state = parse_state::relative_path;
                return *this;
            }
        }

        const size_t start = m_position;
        m_position = traits::find_separator(data, start, path_size);
        m_element = view_type(data + start, m_position - start);
        m_state = parse_state::relative_path;

        return *this;
    }

    path_iterator operator++(int) noexcept
    {
        path_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    friend bool operator==(const path_iterator& lhs, const path_iterator& rhs) noexcept
    {
        return lhs.m_owner == rhs.m_owner && lhs.m_position == rhs.m_position && lhs.m_state == rhs.m_state;
    }

    friend bool operator!=(const path_iterator& lhs, const path_iterator& rhs) noexcept
    {
        return !(lhs == rhs);
    }

private:

    const Owner* m_owner = nullptr;
    view_type m_element;
    size_t m_position = 0;
    parse_state m_state = parse_state::begin;
};

} // namespace _path_priv

//=============================================================================
// basic_path_view
//=============================================================================

/**
 * @class basic_path_view
 * @brief A non-owning, non-allocating view of a path, with all of the path
 * decomposition, query, comparison and iteration operations.
 *
 * This is the type to pass around when a function only needs to *read* a path.
 * `basic_path` converts to it implicitly; nothing else does. Constructing a
 * view from a raw string is `explicit`, so that a string argument is never
 * ambiguous between a view and the owning temporary_string catch-all:
 * `path_view{ "/a/b" }` borrows, while passing `"/a/b"` to a function that
 * takes a `tmp_string` converts as needed.
 *
 * The viewed characters must outlive the view. Decomposition members return
 * views of the same characters.
 */

//=============================================================================
// basic_path
//=============================================================================

template <typename T>
class basic_path_view
{
    VX_STATIC_ASSERT_MSG(type_traits::is_char<T>::value, "T must be character type");
    using traits = _path_priv::path_traits<T>;

public:

    using value_type = T;
    using string_view_type = typename traits::path_string_view;
    using size_type = size_t;

    using iterator = _path_priv::path_iterator<T>;
    using const_iterator = iterator;

    //=========================================================================
    // construction
    //=========================================================================

    basic_path_view() noexcept = default;
    basic_path_view(const basic_path_view&) noexcept = default;
    basic_path_view& operator=(const basic_path_view&) noexcept = default;

    basic_path_view(const T* s, size_type count) noexcept
        : m_view(s, count)
    {}

    /// null-terminated; a null pointer is the empty view
    explicit basic_path_view(const T* s) noexcept
        : m_view(s, str::length(s))
    {}

    /// any string / string view over T
    template <typename S, VX_REQUIRES(_path_priv::is_path_source<S, T>::value)>
    explicit basic_path_view(const S& s) noexcept
        : m_view(s.data(), s.size())
    {}

    //=========================================================================
    // observers
    //=========================================================================

    const T* data() const noexcept
    {
        return m_view.data();
    }

    size_type size() const noexcept
    {
        return m_view.size();
    }

    bool empty() const noexcept
    {
        return m_view.size() == 0;
    }

    /// The underlying characters (not necessarily null-terminated).
    string_view_type native() const noexcept
    {
        return m_view;
    }

    //=========================================================================
    // decomposition
    //=========================================================================

private:

    basic_path_view sub(const _path_priv::substring& s) const noexcept
    {
        return basic_path_view(m_view.data() + s.pos, s.size);
    }

public:

    basic_path_view root_name() const noexcept
    {
        return sub(traits::parse_root_name(data(), size()));
    }

    basic_path_view root_directory() const noexcept
    {
        return sub(traits::parse_root_directory(data(), size()));
    }

    basic_path_view root_path() const noexcept
    {
        return sub(traits::parse_root_path(data(), size()));
    }

    basic_path_view relative_path() const noexcept
    {
        return sub(traits::parse_relative_path(data(), size()));
    }

    basic_path_view parent_path() const noexcept
    {
        return sub(traits::parse_parent_path(data(), size()));
    }

    basic_path_view filename() const noexcept
    {
        return sub(traits::parse_filename(data(), size()));
    }

    /// The filename without its extension ("file.txt" -> "file").
    basic_path_view stem() const noexcept
    {
        return sub(traits::parse_stem(data(), size()));
    }

    /// The extension including the dot ("file.txt" -> ".txt").
    basic_path_view extension() const noexcept
    {
        return sub(traits::parse_extension(data(), size()));
    }

    //=========================================================================
    // queries
    //=========================================================================

    bool is_dot() const noexcept
    {
        return traits::is_dot(m_view);
    }

    bool is_dotdot() const noexcept
    {
        return traits::is_dotdot(m_view);
    }

    bool is_dot_or_dotdot() const noexcept
    {
        return is_dot() || is_dotdot();
    }

    bool has_root_name() const noexcept
    {
        return traits::parse_root_name(data(), size()).size != 0;
    }

    bool has_root_directory() const noexcept
    {
        return traits::parse_root_directory(data(), size()).size != 0;
    }

    bool has_root_path() const noexcept
    {
        return traits::parse_root_path(data(), size()).size != 0;
    }

    bool has_relative_path() const noexcept
    {
        return traits::parse_relative_path(data(), size()).size != 0;
    }

    bool has_parent_path() const noexcept
    {
        return traits::parse_parent_path(data(), size()).size != 0;
    }

    bool has_filename() const noexcept
    {
        return traits::parse_filename(data(), size()).size != 0;
    }

    bool has_stem() const noexcept
    {
        return traits::parse_stem(data(), size()).size != 0;
    }

    bool has_extension() const noexcept
    {
        return traits::parse_extension(data(), size()).size != 0;
    }

    /// Windows: needs a root directory ("C:\x", "\\server\x"; "C:x" and "\x" are not absolute).
    /// POSIX: starts with a separator.
    bool is_absolute() const noexcept
    {
        return traits::is_absolute(data(), size());
    }

    bool is_relative() const noexcept
    {
        return !is_absolute();
    }

    //=========================================================================
    // compare
    //=========================================================================

    // https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/stl/inc/filesystem#L1017

    /// Element-wise comparison: negative, zero or positive.
    int compare(basic_path_view rhs) const noexcept
    {
        return traits::compare(data(), size(), rhs.data(), rhs.size());
    }

    friend bool operator==(basic_path_view lhs, basic_path_view rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    friend bool operator!=(basic_path_view lhs, basic_path_view rhs) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    friend bool operator<(basic_path_view lhs, basic_path_view rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    friend bool operator<=(basic_path_view lhs, basic_path_view rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    friend bool operator>(basic_path_view lhs, basic_path_view rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    friend bool operator>=(basic_path_view lhs, basic_path_view rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() const noexcept
    {
        return iterator(*this);
    }

    iterator end() const noexcept
    {
        return iterator(*this, _path_priv::parse_state::end);
    }

    iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator cend() const noexcept
    {
        return end();
    }

private:

    string_view_type m_view;
};

//=============================================================================
// basic_path
//=============================================================================

/**
 * @class basic_path
 * @brief A cross-platform file system path stored as a string.
 *
 * Error handling follows the string model (no exceptions):
 * - Mutating functions that can allocate return `success`. Where practical they
 *   are atomic: on failure the path is unchanged.
 * - `create(...)` returns `expected<basic_path, error>`; constructors and
 *   operators (`/=`, `+=`, `/`, `=`) VX_VERIFY the result.
 * - `lexically_*` and `string<>()` allocate, so they return `expected`.
 * - Decomposition, queries, comparison and iteration are provided by
 *   `basic_path_view` (this class converts to it implicitly and forwards the
 *   same members). They never allocate; use `create(view)` for an owning copy.
 *
 * Input conventions:
 * - `path_view` parameters take paths and views, with no copy.
 * - `tmp_string` parameters are the catch-all for everything else: literals,
 *   strings, string views, other character widths.
 * Raw strings never convert implicitly to `path_view`, so the two overloads
 * are never ambiguous.
 *
 * @tparam Allocator allocator forwarded to the underlying string; its
 * value_type selects the character type.
 */
template <typename T, typename Allocator = mem::default_allocator<T>>
class basic_path
{
    VX_STATIC_ASSERT_MSG(type_traits::is_char<T>::value, "T must be character type");
    VX_STATIC_ASSERT_MSG(
        (std::is_same<T, typename Allocator::value_type>::value),
        "Allocator value type must match T");

    using traits = _path_priv::path_traits<T>;

public:

    using value_type = T;
    using allocator_type = Allocator;
    using string_type = str::basic_string<value_type, Allocator>;
    using view_type = basic_path_view<value_type>;
    using size_type = size_t;

    using string_view_type = typename traits::path_string_view;
    using tmp_string = typename traits::path_temporary_string;

    using iterator = typename view_type::iterator;
    using const_iterator = iterator;

    //=========================================================================
    // constants
    //=========================================================================

    /// Generic separator, always '/'
    static constexpr value_type separator = traits::path_separator;

    /// Native preferred separator: '\' on Windows, '/' elsewhere
    static constexpr value_type preferred_separator = traits::path_preferred_separator;

    /// '.'
    static constexpr value_type dot = traits::path_dot;

    /// ':' (drive / alternate data stream delimiter on Windows)
    static constexpr value_type colon = traits::path_colon;

    /// "." and ".." as null-terminated strings
    static constexpr const value_type* dot_string = traits::dot_string;
    static constexpr const value_type* dotdot_string = traits::dotdot_string;

#if defined(VX_WINDOWS_PATH)
    static constexpr bool windows_style = true;
#else
    static constexpr bool windows_style = false;
#endif

    /// True for every character this path treats as a directory separator
    static constexpr bool is_separator(value_type c) noexcept
    {
        return traits::is_directory_separator(c);
    }

private:

    // only the exact owning string type may be moved in; everything else
    // (literals, views, lvalue strings, foreign widths) goes through tmp_string
    template <typename S>
    struct is_owned_string : std::is_same<S, string_type>
    {};

    template <typename S>
    struct is_tmp_source : std::integral_constant<bool, std::is_constructible<tmp_string, const S&>::value && !std::is_same<S, basic_path>::value>
    {};

    template <typename C2>
    struct is_foreign_char : std::integral_constant<bool, type_traits::is_char<C2>::value && !std::is_same<C2, value_type>::value>
    {};

    // validates before the string is constructed from it
    static const value_type* checked_cstr(const tmp_string& s) noexcept
    {
        VX_VERIFY(s.is_valid());
        return s.c_str();
    }

    //=========================================================================
    // special members
    //=========================================================================

    basic_path() = default;
    ~basic_path() = default;

    explicit basic_path(const allocator_type& alloc)
        : m_path(alloc)
    {}

    basic_path(const basic_path&) = default;
    basic_path(basic_path&&) noexcept = default;

    basic_path(const basic_path& other, const allocator_type& alloc)
        : m_path(other.m_path, alloc)
    {}

    basic_path(basic_path&& other, const allocator_type& alloc) noexcept
        : m_path(std::move(other.m_path), alloc)
    {}

    basic_path& operator=(const basic_path&) = default;
    basic_path& operator=(basic_path&&) noexcept = default;

    void swap(basic_path& other) noexcept
    {
        vx::swap(m_path, other.m_path);
    }

    friend void swap(basic_path& lhs, basic_path& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    //=========================================================================
    // construction
    //=========================================================================

    /// Takes ownership of an existing string (no copy, cannot fail).
    template <typename S, VX_REQUIRES(is_owned_string<S>::value)>
    explicit basic_path(S&& s) noexcept
        : m_path(std::move(s))
    {}

    /// Copies a view (VX_VERIFYs on failure).
    explicit basic_path(view_type v, const allocator_type& alloc = allocator_type())
        : m_path(v.native(), alloc)
    {}

    /// Catch-all: anything a temporary_string can be built from (VX_VERIFYs on failure).
    template <typename S, VX_REQUIRES(is_tmp_source<S>::value)>
    basic_path(const S& src, const allocator_type& alloc = allocator_type())
        : m_path(checked_cstr(tmp_string(src)), alloc)
    {}

    //=========================================================================
    // fallible construction
    //=========================================================================

    static expected<basic_path, error> create(const allocator_type& alloc = allocator_type())
    {
        return basic_path(alloc);
    }

    static expected<basic_path, error> create(view_type v, const allocator_type& alloc = allocator_type())
    {
        auto s = string_type::create(v.native(), alloc);
        VX_RET_UNEXPECTED_ERR_IF(!s, s.error());

        return basic_path(std::move(s.value()));
    }

    static expected<basic_path, error> create(const tmp_string& src, const allocator_type& alloc = allocator_type())
    {
        VX_RET_UNEXPECTED_ERR_IF(!src.is_valid(), err::out_of_memory);

        auto s = string_type::create(src.c_str(), alloc);
        VX_RET_UNEXPECTED_ERR_IF(!s, s.error());

        return basic_path(std::move(s.value()));
    }

    template <typename S, VX_REQUIRES(is_owned_string<S>::value)>
    static expected<basic_path, error> create(S&& s) noexcept
    {
        return basic_path(std::move(s));
    }

    //=========================================================================
    // assignment
    //=========================================================================

    template <typename S, VX_REQUIRES(is_owned_string<S>::value)>
    basic_path& operator=(S&& s) noexcept
    {
        m_path = std::move(s);
        return *this;
    }

    basic_path& operator=(const tmp_string& rhs)
    {
        const auto ok = assign(rhs);
        VX_VERIFY(ok);
        return *this;
    }

    template <typename S, VX_REQUIRES(is_owned_string<S>::value)>
    success assign(S&& s) noexcept
    {
        m_path = std::move(s);
        return success{};
    }

    success assign(view_type v)
    {
        return m_path.assign(v.native());
    }

    success assign(const tmp_string& src)
    {
        VX_RET_ERR_IF(!src.is_valid(), err::out_of_memory);
        return m_path.assign(src.c_str());
    }

    //=========================================================================
    // appending (operator/=, append)
    //=========================================================================

    // https://en.cppreference.com/w/cpp/filesystem/path/append
    // https://github.com/microsoft/STL/blob/fec1c8b6a13e5411edebbddc3ad98258f5e282d2/stl/inc/filesystem#L712

private:

    // true if p points into this path's buffer
    bool aliases(const value_type* p) const noexcept
    {
        std::less_equal<const value_type*> le;
        return le(m_path.data(), p) && le(p, m_path.data() + m_path.size());
    }

    // Replaces m_path[pos, end) with (optional dot) + [s, s + n).
    // Reserves first, so on failure the path is untouched.
    success replace_suffix(size_type pos, const value_type* s, size_type n, bool add_dot)
    {
        VX_ASSERT(!aliases(s));
        VX_RET_ERR_IF(n > m_path.max_size() - pos, err::size_error);

        const auto reserved = m_path.reserve(pos + (add_dot ? 1 : 0) + n);
        VX_RET_ERR_IF(!reserved, reserved);

        // cannot fail from here on
        auto ops = m_path.unsafe();
        ops.erase_assume_in_range(pos, m_path.size() - pos);
        if (add_dot)
        {
            ops.push_back_assume_capacity(dot);
        }
        ops.append_assume_capacity(s, n);
        return success{};
    }

    // rhs must not alias m_path
    success append_view_noalias(view_type rhs)
    {
        // if rhs.is_absolute(), then op=(rhs)
        if (rhs.is_absolute())
        {
            return m_path.assign(rhs.native());
        }

        const size_type lhs_size = m_path.size();
        size_type keep = lhs_size; // how much of *this survives

#if defined(VX_WINDOWS_PATH)

        const view_type lhs_root_name = view().root_name();
        const view_type rhs_root_name = rhs.root_name();

        // if rhs.has_root_name() && rhs.root_name() != root_name(), then op=(rhs)
        if (!rhs_root_name.empty() && lhs_root_name.compare(rhs_root_name) != 0)
        {
            return m_path.assign(rhs.native());
        }

        const size_type rhs_start = rhs_root_name.size();
        const size_type root_name_size = lhs_root_name.size();

#else

        // No root names on POSIX
        constexpr size_type rhs_start = 0;
        constexpr size_type root_name_size = 0;

#endif // VX_WINDOWS_PATH

        bool need_sep = false;

        if (rhs_start != rhs.size() && traits::is_directory_separator(rhs.data()[rhs_start]))
        {
            // rhs.has_root_directory(): drop everything but the root name
            keep = root_name_size;
        }
        else
        {
#if defined(VX_WINDOWS_PATH)
            if (root_name_size == lhs_size)
            {
                // !has_root_directory() && !has_filename(): only "\\server" style
                // root names (3+ chars) need a separator, "X:" must not get one
                need_sep = root_name_size >= 3;
            }
            else
            {
                need_sep = !traits::is_directory_separator(m_path.back());
            }
#else
            need_sep = lhs_size != 0 && !traits::is_directory_separator(m_path.back());
#endif // VX_WINDOWS_PATH
        }

        const value_type* const rhs_data = rhs.data() + rhs_start;
        const size_type rhs_count = rhs.size() - rhs_start;

        VX_RET_ERR_IF(rhs_count > m_path.max_size() - keep, err::size_error);

        const auto reserved = m_path.reserve(keep + (need_sep ? 1 : 0) + rhs_count);
        VX_RET_ERR_IF(!reserved, reserved);

        // cannot fail from here on
        auto ops = m_path.unsafe();
        if (keep < lhs_size)
        {
            ops.erase_assume_in_range(keep, lhs_size - keep);
        }
        if (need_sep)
        {
            ops.push_back_assume_capacity(preferred_separator);
        }
        ops.append_assume_capacity(rhs_data, rhs_count);

        return success{};
    }

    success append_view(view_type rhs)
    {
        VX_ASSERT(!aliases(rhs.data()));
        return append_view_noalias(rhs);
    }

public:

    /**
     * Appends `rhs` with a directory separator (std::filesystem semantics):
     * - absolute `rhs` (or one with a different root name) replaces *this
     * - `rhs` with a root directory replaces everything but the root name
     * - otherwise a separator is inserted when *this has a filename
     */
    success append(const basic_path& rhs)
    {
        return append_view(rhs.m_path.view());
    }

    success append(const tmp_string& rhs)
    {
        VX_RET_ERR_IF(!rhs.is_valid(), err::out_of_memory);
        return append_view(string_view_type(rhs.c_str(), rhs.size()));
    }

    basic_path& operator/=(const basic_path& rhs)
    {
        const auto ok = append(rhs);
        VX_VERIFY(ok);
        return *this;
    }

    basic_path& operator/=(const tmp_string& rhs)
    {
        const auto ok = append(rhs);
        VX_VERIFY(ok);
        return *this;
    }

    //=========================================================================
    // concatenation (operator+=, concat): no separator is inserted
    //=========================================================================

    success concat(const basic_path& rhs)
    {
        return m_path.append(rhs.m_path);
    }

    success concat(const tmp_string& rhs)
    {
        VX_RET_ERR_IF(!rhs.is_valid(), err::out_of_memory);
        return m_path.append(rhs.c_str());
    }

    success concat(const value_type c)
    {
        return m_path.push_back(c);
    }

    template <typename C2, VX_REQUIRES(is_foreign_char<C2>::value)>
    success concat(const C2 c)
    {
        return concat(tmp_string{ str::basic_string_view<C2>(&c, 1) });
    }

    basic_path& operator+=(const basic_path& rhs)
    {
        const auto ok = concat(rhs);
        VX_VERIFY(ok);
        return *this;
    }

    basic_path& operator+=(const tmp_string& rhs)
    {
        const auto ok = concat(rhs);
        VX_VERIFY(ok);
        return *this;
    }

    basic_path& operator+=(const value_type c)
    {
        const auto ok = concat(c);
        VX_VERIFY(ok);
        return *this;
    }

    template <typename C2, VX_REQUIRES(is_foreign_char<C2>::value)>
    basic_path& operator+=(const C2 c)
    {
        const auto ok = concat(c);
        VX_VERIFY(ok);
        return *this;
    }

    //=========================================================================
    // modifiers
    //=========================================================================

    void clear() noexcept
    {
        m_path.clear();
    }

    success reserve(size_type new_capacity)
    {
        return m_path.reserve(new_capacity);
    }

    /// Replaces every generic separator with the preferred one (no-op on POSIX).
    basic_path& make_preferred() noexcept
    {
#if defined(VX_WINDOWS_PATH)

        value_type* p = m_path.data();
        for (size_type i = 0, n = m_path.size(); i < n; ++i)
        {
            if (p[i] == separator)
            {
                p[i] = preferred_separator;
            }
        }

#endif // VX_WINDOWS_PATH

        return *this;
    }

    /// Removes the filename, leaving any trailing separator ("/a/b" -> "/a/").
    basic_path& remove_filename() noexcept
    {
        const auto filename = traits::parse_filename(m_path.data(), m_path.size());
        m_path.unsafe().erase_assume_in_range(filename.pos, m_path.size() - filename.pos);
        return *this;
    }

    /// Removes the extension ("a/b.txt" -> "a/b").
    basic_path& remove_extension() noexcept
    {
        const auto extension = traits::parse_extension(m_path.data(), m_path.size());
        m_path.unsafe().erase_assume_in_range(extension.pos, m_path.size() - extension.pos);
        return *this;
    }

    /// Removes the last component and the separators before it, keeping the root.
    basic_path& pop_back() noexcept
    {
        const auto root_directory = traits::parse_root_directory(m_path.data(), m_path.size());
        const size_type root_path_end = (root_directory.size > 0) ? root_directory.pos + 1 : root_directory.pos;

        size_type new_end = m_path.size();
        while (new_end > root_path_end && !traits::is_directory_separator(m_path[new_end - 1]))
        {
            --new_end;
        }
        while (new_end > root_path_end && traits::is_directory_separator(m_path[new_end - 1]))
        {
            --new_end;
        }

        m_path.unsafe().erase_assume_in_range(new_end, m_path.size() - new_end);
        return *this;
    }

    /// Replaces the extension. A leading '.' is added if missing; empty removes it.
    /// Atomic: on failure the path is unchanged.
    success replace_extension(view_type new_extension)
    {
        const bool add_dot = !new_extension.empty() && new_extension.data()[0] != dot;
        const auto extension = traits::parse_extension(m_path.data(), m_path.size());

        return replace_suffix(extension.pos, new_extension.data(), new_extension.size(), add_dot);
    }

    success replace_extension(const tmp_string& new_extension)
    {
        VX_RET_ERR_IF(!new_extension.is_valid(), err::out_of_memory);
        return replace_extension(view_type(new_extension.c_str(), new_extension.size()));
    }

    /// Replaces the filename. Atomic: on failure the path is unchanged.
    success replace_filename(view_type new_filename)
    {
        const auto filename = traits::parse_filename(m_path.data(), m_path.size());
        return replace_suffix(filename.pos, new_filename.data(), new_filename.size(), false);
    }

    success replace_filename(const tmp_string& new_filename)
    {
        VX_RET_ERR_IF(!new_filename.is_valid(), err::out_of_memory);
        return replace_filename(view_type(new_filename.c_str(), new_filename.size()));
    }

    //=========================================================================
    // format observers
    //=========================================================================

    const value_type* c_str() const noexcept
    {
        return m_path.c_str();
    }

    const string_type& native() const noexcept
    {
        return m_path;
    }

    view_type view() const noexcept
    {
        return view_type(m_path.data(), m_path.size());
    }

    /// Paths convert to views implicitly; this is what the view-taking APIs use.
    operator view_type() const noexcept
    {
        return view();
    }

    allocator_type get_allocator() const noexcept
    {
        return m_path.get_allocator();
    }

    /// Converts to another character width (e.g. UTF-8 `char` from a UTF-16 native path).
    template <typename char_t = char, typename Alloc2 = mem::default_allocator<char_t>>
    expected<str::basic_string<char_t, Alloc2>, error> string(const Alloc2& alloc = Alloc2()) const
    {
        const str::basic_temporary_string<char_t, false> tmp{ m_path.view() };
        VX_RET_UNEXPECTED_ERR_IF(!tmp.is_valid(), err::out_of_memory);

        return str::basic_string<char_t, Alloc2>::create(tmp.c_str(), alloc);
    }

    //=========================================================================
    // generation
    //=========================================================================

    /**
     * Lexically normalized copy (no filesystem access):
     * root-name separators become preferred, separator runs collapse, "." elements
     * and "x/.." pairs are removed, "/.." at the root is dropped, an empty result
     * becomes ".", and no separator trails a final "..".
     */
    expected<basic_path, error> lexically_normal() const
    {
        using parse_state = _path_priv::parse_state;

        basic_path result(m_path.get_allocator());
        if (m_path.empty())
        {
            return result;
        }

        // The output is never longer than the input (every character written
        // corresponds to a source character), so one reserve covers everything
        // and the rest of the function cannot fail.
        const auto reserved = result.m_path.reserve(m_path.size());
        VX_RET_UNEXPECTED_ERR_IF(!reserved, reserved);

        string_type& out = result.m_path;
        auto ops = out.unsafe();

        size_type root_end = 0;
        bool has_root_directory = false;

        const view_type src = view();
        const auto last = src.end();
        for (auto it = src.begin(); it != last; ++it)
        {
            const view_type e = *it;

            if (it.state() == parse_state::root_name)
            {
                for (size_type i = 0; i < e.size(); ++i)
                {
                    const value_type c = e.data()[i];
                    ops.push_back_assume_capacity(traits::is_directory_separator(c) ? preferred_separator : c);
                }

                root_end = out.size();
                continue;
            }

            if (it.state() == parse_state::root_directory)
            {
                // a run of separators collapses to one preferred separator
                ops.push_back_assume_capacity(preferred_separator);
                has_root_directory = true;
                root_end = out.size();
                continue;
            }

            if (e.empty() || e.is_dot())
            {
                continue;
            }

            if (e.is_dotdot())
            {
                if (out.size() > root_end)
                {
                    // the previous element is always terminated by a separator
                    const size_type prev_end = out.size() - 1;
                    const size_type prev_start = traits::last_element_start(out.data(), root_end, prev_end);

                    if (!view_type(out.data() + prev_start, prev_end - prev_start).is_dotdot())
                    {
                        // "x/.." cancels out
                        ops.erase_assume_in_range(prev_start, out.size() - prev_start);
                        continue;
                    }
                }
                else if (has_root_directory)
                {
                    // "/.." == "/"
                    continue;
                }
            }

            ops.append_assume_capacity(e.data(), e.size());

            // keep a separator only if one followed this element in the source
            const size_type next = it.position();
            if (next < src.size() && traits::is_directory_separator(src.data()[next]))
            {
                ops.push_back_assume_capacity(preferred_separator);
            }
        }

        if (out.empty())
        {
            // normal form of "./" is "."
            ops.push_back_assume_capacity(dot);
        }
        else if (out.size() > root_end && traits::is_directory_separator(out.back()))
        {
            // no trailing separator after a final ".."
            const size_type e_end = out.size() - 1;
            const size_type e_start = traits::last_element_start(out.data(), root_end, e_end);

            if (view_type(out.data() + e_start, e_end - e_start).is_dotdot())
            {
                ops.pop_back_assume_nonempty();
            }
        }

        return result;
    }

    // https://en.cppreference.com/w/cpp/filesystem/path/lexically_normal
    // https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/stl/inc/filesystem#L1684

    /**
     * Path that leads from `base` to *this, computed lexically. Returns an
     * empty path (not an error) when no such relative path exists.
     */
    expected<basic_path, error> lexically_relative(view_type base) const
    {
        basic_path ret(m_path.get_allocator());

        const view_type self = view();

        // Different root names, mixed absolute/relative, or a root directory
        // only on `base` -> empty path.
        // (Filenames that could be mistaken for a root name are not checked.)
        if (self.root_name().compare(base.root_name()) != 0 || self.is_absolute() != base.is_absolute() || (!self.has_root_directory() && base.has_root_directory()))
        {
            return ret;
        }

        // first mismatching element, as if by std::mismatch
        auto a_it = self.begin();
        auto b_it = base.begin();
        const auto a_end = self.end();
        const auto b_end = base.end();

        while (a_it != a_end && b_it != b_end && a_it->compare(*b_it) == 0)
        {
            ++a_it;
            ++b_it;
        }

        if (a_it == a_end && b_it == b_end)
        {
            const auto ok = ret.m_path.assign(dot_string);
            VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
            return ret;
        }

        // N = non-empty, non-"." elements left in base, minus the ".." elements
        int n = 0;
        for (; b_it != b_end; ++b_it)
        {
            if (b_it->empty())
            {
                continue;
            }
            else if (b_it->is_dotdot())
            {
                --n;
            }
            else if (!b_it->is_dot())
            {
                ++n;
            }
        }

        if (n < 0)
        {
            return ret;
        }

        if (n == 0 && (a_it == a_end || a_it->empty()))
        {
            const auto ok = ret.m_path.assign(dot_string);
            VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
            return ret;
        }

        // N times "..", then every remaining element of *this
        while (n-- > 0)
        {
            if (!ret.empty())
            {
                const auto sep_ok = ret.m_path.push_back(preferred_separator);
                VX_RET_UNEXPECTED_ERR_IF(!sep_ok, sep_ok);
            }

            const auto ok = ret.m_path.append(dotdot_string);
            VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        }

        // += rather than /= (https://cplusplus.github.io/LWG/issue3070)
        for (; a_it != a_end; ++a_it)
        {
            if (!ret.empty() && ret.m_path.back() != preferred_separator)
            {
                const auto sep_ok = ret.m_path.push_back(preferred_separator);
                VX_RET_UNEXPECTED_ERR_IF(!sep_ok, sep_ok);
            }

            const auto ok = ret.m_path.append(a_it->data(), a_it->size());
            VX_RET_UNEXPECTED_ERR_IF(!ok, ok);
        }

        return ret;
    }

    /// lexically_relative(base), or a copy of *this if that is empty.
    expected<basic_path, error> lexically_proximate(view_type base) const
    {
        auto relative = lexically_relative(base);
        VX_RET_UNEXPECTED_ERR_IF(!relative, relative.error());

        if (!relative.value().empty())
        {
            return relative;
        }

        return create(view(), m_path.get_allocator());
    }

    //=========================================================================
    // compare
    //=========================================================================

    // https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/stl/inc/filesystem#L1017

    /// Element-wise comparison: negative, zero or positive.
    int compare(view_type rhs) const noexcept
    {
        return view().compare(rhs);
    }

    //=========================================================================
    // decomposition (non-owning views into this path)
    //=========================================================================

    view_type root_name() const noexcept
    { return view().root_name(); }
    view_type root_directory() const noexcept
    { return view().root_directory(); }
    view_type root_path() const noexcept
    { return view().root_path(); }
    view_type relative_path() const noexcept
    { return view().relative_path(); }
    view_type parent_path() const noexcept
    { return view().parent_path(); }
    view_type filename() const noexcept
    { return view().filename(); }
    view_type stem() const noexcept
    { return view().stem(); }
    view_type extension() const noexcept
    { return view().extension(); }


    //=========================================================================
    // queries
    //=========================================================================

    bool empty() const noexcept
    { return m_path.empty(); }
    size_type size() const noexcept
    { return m_path.size(); }
    size_type capacity() const noexcept
    { return m_path.capacity(); }

    bool is_dot_or_dotdot() const noexcept
    { return view().is_dot_or_dotdot(); }
    bool has_root_name() const noexcept
    { return view().has_root_name(); }
    bool has_root_directory() const noexcept
    { return view().has_root_directory(); }
    bool has_root_path() const noexcept
    { return view().has_root_path(); }
    bool has_relative_path() const noexcept
    { return view().has_relative_path(); }
    bool has_parent_path() const noexcept
    { return view().has_parent_path(); }
    bool has_filename() const noexcept
    { return view().has_filename(); }
    bool has_stem() const noexcept
    { return view().has_stem(); }
    bool has_extension() const noexcept
    { return view().has_extension(); }
    bool is_absolute() const noexcept
    { return view().is_absolute(); }
    bool is_relative() const noexcept
    { return view().is_relative(); }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() const noexcept
    { return view().begin(); }
    iterator end() const noexcept
    { return view().end(); }
    iterator cbegin() const noexcept
    { return begin(); }
    iterator cend() const noexcept
    { return end(); }

    //=========================================================================
    // non member functions
    //=========================================================================

    // path/path comparisons are exact matches; path/view mixes use the view operators

    friend bool operator==(const basic_path& lhs, const basic_path& rhs) noexcept
    {
        return lhs.compare(rhs.view()) == 0;
    }

    friend bool operator!=(const basic_path& lhs, const basic_path& rhs) noexcept
    {
        return lhs.compare(rhs.view()) != 0;
    }

    friend bool operator<(const basic_path& lhs, const basic_path& rhs) noexcept
    {
        return lhs.compare(rhs.view()) < 0;
    }

    friend bool operator<=(const basic_path& lhs, const basic_path& rhs) noexcept
    {
        return lhs.compare(rhs.view()) <= 0;
    }

    friend bool operator>(const basic_path& lhs, const basic_path& rhs) noexcept
    {
        return lhs.compare(rhs.view()) > 0;
    }

    friend bool operator>=(const basic_path& lhs, const basic_path& rhs) noexcept
    {
        return lhs.compare(rhs.view()) >= 0;
    }

    friend basic_path operator/(const basic_path& lhs, const basic_path& rhs)
    {
        basic_path result(lhs);
        result /= rhs.view();
        return result;
    }

    friend basic_path operator/(const basic_path& lhs, view_type rhs)
    {
        basic_path result(lhs);
        result /= rhs;
        return result;
    }

    friend basic_path operator/(const basic_path& lhs, const tmp_string& rhs)
    {
        basic_path result(lhs);
        result /= rhs;
        return result;
    }

private:

    string_type m_path;
};

using path = basic_path<os::native_char>;

} // namespace vx
