#pragma once

#include "vertex/os/path.hpp"
#include "vertex/util/time.hpp"

namespace vx {
namespace os {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// File Types
///////////////////////////////////////////////////////////////////////////////

enum class file_type
{
    NONE,
    NOT_FOUND,
    REGULAR,
    DIRECTORY,
    SYMLINK,
    UNKNOWN
};

///////////////////////////////////////////////////////////////////////////////
// File Permissions
///////////////////////////////////////////////////////////////////////////////

// https://en.cppreference.com/w/cpp/filesystem/perms

struct file_permissions
{
    using type = int;

    enum : type
    {
        NONE = 0,
        OWNER_READ = 0400,
        OWNER_WRITE = 0200,
        OWNER_EXEC = 0100,
        OWNER_ALL = 0700,

        GROUP_READ = 040,
        GROUP_WRITE = 020,
        GROUP_EXEC = 010,
        GROUP_ALL = 070,

        OTHERS_READ = 04,
        OTHERS_WRITE = 02,
        OTHERS_EXEC = 01,
        OTHERS_ALL = 07,

        ALL = 0777,

        SET_UID = 04000,
        GET_GID = 02000,

        MASK = 07777,
        UNKNOWN = 0xFFFF
    };
};

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

struct file_info
{
    file_type type{};
    typename file_permissions::type permissions{};
    size_t size{};
    time::time_point create_time;
    time::time_point modify_time;

    constexpr bool exists() const { return !(type == file_type::NONE || type == file_type::NOT_FOUND); }
    constexpr bool is_regular_file() const { return type == file_type::REGULAR; }
    constexpr bool is_directory() const { return type == file_type::DIRECTORY; }
    constexpr bool is_symlink() const { return type == file_type::SYMLINK; }
    constexpr bool is_other() const { return !(is_regular_file() || is_directory() || is_symlink()); }
};

VX_API file_info get_file_info(const path& p);
VX_API file_info get_symlink_info(const path& p);

///////////////////////////////////////////////////////////////////////////////
// Read Symlink
///////////////////////////////////////////////////////////////////////////////

VX_API path read_symlink(const path& p);

///////////////////////////////////////////////////////////////////////////////
// Exists
///////////////////////////////////////////////////////////////////////////////

inline bool exists(const path& p)
{
    return get_file_info(p).exists();
}

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

VX_API path get_current_path();
VX_API bool set_current_path(const path& p);

VX_API path absolute(const path& p);
VX_API path relative(const path& p, const path& base = get_current_path());

VX_API bool equivalent(const path& p1, const path& p2);

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////

VX_API bool create_file(const path& p);
VX_API bool create_symlink(const path& target, const path& link);
VX_API bool create_directory_symlink(const path& target, const path& link);
VX_API bool create_directory(const path& p);
VX_API bool create_directories(const path& p);

///////////////////////////////////////////////////////////////////////////////
// Copy
///////////////////////////////////////////////////////////////////////////////

struct copy_options
{
    using type = uint32_t;

    enum : type
    {
        NONE                    = 0,

        SKIP_EXISTING           = (1 << 0),
        OVERWRITE_EXISTING      = (1 << 1),

        COPY_SYMLINKS           = (1 << 2),
        SKIP_SYMLINKS           = (1 << 3),

        DIRECTORIES_ONLY        = (1 << 4),
        RECURSIVE               = (1 << 5)
    };
};

VX_API bool copy_file(const path& from, const path& to, bool overwrite_existing = true);
VX_API bool copy_symlink(const path& from, const path& to);
VX_API bool copy(const path& from, const path& to, typename copy_options::type options = copy_options::RECURSIVE);

///////////////////////////////////////////////////////////////////////////////
// Rename
///////////////////////////////////////////////////////////////////////////////

VX_API bool rename(const path& from, const path& to);

///////////////////////////////////////////////////////////////////////////////
// Remove
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

enum class remove_error
{
    NONE,
    FILE_NOT_FOUND,
    DIRECTORY_NOT_EMPTY,
    OTHER
};

} // namespace __detail

VX_API bool remove(const path& p);
VX_API size_t remove_all(const path& p);

///////////////////////////////////////////////////////////////////////////////
// Directory Entry
///////////////////////////////////////////////////////////////////////////////

struct directory_entry
{
    void refresh()
    {
        if (!path.empty())
        {
            info = get_file_info(path);
        }
    }

    constexpr bool is_regular_file() const { return info.is_regular_file(); }
    constexpr bool is_directory() const { return info.is_directory(); }
    constexpr bool is_symlink() const { return info.is_symlink(); }
    constexpr bool is_other() const { return info.is_other(); }

    os::path path;
    file_info info;
};

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

class directory_iterator
{
public:

    using value_type = directory_entry;
    using difference_type = ptrdiff_t;
    using pointer = const directory_entry*;
    using reference = const directory_entry&;
    using iterator_category = std::input_iterator_tag;

public:

    VX_API directory_iterator(const path& p);

    directory_iterator() noexcept = default;
    ~directory_iterator() noexcept = default;

    directory_iterator(const directory_iterator&) noexcept = default;
    directory_iterator(directory_iterator&&) noexcept = default;

    directory_iterator& operator=(const directory_iterator&) noexcept = default;
    directory_iterator& operator=(directory_iterator&&) noexcept = default;

public:

    VX_API reference operator*() const noexcept;
    VX_API pointer operator->() const noexcept;

    VX_API directory_iterator& operator++();

    bool operator==(const directory_iterator& rhs) const noexcept
    {
        return m_impl == rhs.m_impl;
    }

    bool operator!=(const directory_iterator& rhs) const noexcept
    {
        return m_impl != rhs.m_impl;
    }

private:

    void validate();

private:

    class directory_iterator_impl;
    std::shared_ptr<directory_iterator_impl> m_impl;
};

inline directory_iterator begin(directory_iterator it) noexcept
{
    return it;
}

inline directory_iterator end(directory_iterator) noexcept
{
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Recursive Directory Iterator
///////////////////////////////////////////////////////////////////////////////

class recursive_directory_iterator
{
public:

    using value_type = directory_entry;
    using difference_type = ptrdiff_t;
    using pointer = const directory_entry*;
    using reference = const directory_entry&;
    using iterator_category = std::input_iterator_tag;

public:

    VX_API recursive_directory_iterator(const path& p);

    recursive_directory_iterator() noexcept = default;
    ~recursive_directory_iterator() noexcept = default;

    recursive_directory_iterator(const recursive_directory_iterator&) noexcept = default;
    recursive_directory_iterator(recursive_directory_iterator&&) noexcept = default;

    recursive_directory_iterator& operator=(const recursive_directory_iterator&) noexcept = default;
    recursive_directory_iterator& operator=(recursive_directory_iterator&&) noexcept = default;

public:

    VX_API reference operator*() const noexcept;
    VX_API pointer operator->() const noexcept;

    VX_API recursive_directory_iterator& operator++();

    bool operator==(const recursive_directory_iterator& rhs) const noexcept
    {
        return m_impl == rhs.m_impl;
    }

    bool operator!=(const recursive_directory_iterator& rhs) const noexcept
    {
        return m_impl != rhs.m_impl;
    }

public:

    size_t depth() const;

    bool recursion_pending() const;
    void disable_pending_recursion();

    void pop();

private:

    void validate();

private:

    class recursive_directory_iterator_impl;
    std::shared_ptr<recursive_directory_iterator_impl> m_impl;
};

inline recursive_directory_iterator begin(recursive_directory_iterator it) noexcept
{
    return it;
}

inline recursive_directory_iterator end(recursive_directory_iterator) noexcept
{
    return {};
}

} // namespace filesystem
} // namespace os
} // namespace vx