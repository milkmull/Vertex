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

        ALL_READ = OWNER_READ | GROUP_READ | OTHERS_READ,
        ALL_WRITE = OWNER_WRITE | GROUP_WRITE | OTHERS_WRITE,
        ALL_READ_WRITE = ALL_READ | ALL_WRITE,
        ALL_EXEC = OWNER_EXEC | GROUP_EXEC | OTHERS_EXEC,

        ALL = 0777,

        SET_UID = 04000,
        GET_GID = 02000,

        MASK = 07777,
        UNKNOWN = 0xFFFF
    };
};

enum class file_permission_operator
{
    REPLACE,
    ADD,
    REMOVE
};

VX_API bool update_permissions(
    const path& p,
    typename file_permissions::type permissions,
    file_permission_operator op = file_permission_operator::REPLACE,
    bool follow_symlinks = false
);

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

struct file_info
{
    file_type type = file_type::NONE;
    typename file_permissions::type permissions = file_permissions::NONE;
    size_t size = 0;
    time::time_point create_time;
    time::time_point modify_time;

    constexpr bool exists() const noexcept { return !(type == file_type::NONE || type == file_type::NOT_FOUND); }
    constexpr bool is_regular_file() const noexcept { return type == file_type::REGULAR; }
    constexpr bool is_directory() const noexcept { return type == file_type::DIRECTORY; }
    constexpr bool is_symlink() const noexcept { return type == file_type::SYMLINK; }
    constexpr bool is_other() const noexcept { return !(is_regular_file() || is_directory() || is_symlink()); }
};

VX_API file_info get_file_info(const path& p);
VX_API file_info get_symlink_info(const path& p);

///////////////////////////////////////////////////////////////////////////////
// Read Symlink
///////////////////////////////////////////////////////////////////////////////

VX_API path read_symlink(const path& p);

///////////////////////////////////////////////////////////////////////////////
// boolean checks
///////////////////////////////////////////////////////////////////////////////

inline bool exists(const path& p)
{
    return get_file_info(p).exists();
}

inline bool is_regular_file(const path& p)
{
    return get_file_info(p).is_regular_file();
}

inline bool is_directory(const path& p)
{
    return get_file_info(p).is_directory();
}

inline bool is_symlink(const path& p)
{
    return get_file_info(p).is_symlink();
}

inline bool is_other(const path& p)
{
    return get_file_info(p).is_other();
}

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

VX_API path get_current_path();
VX_API bool set_current_path(const path& p);

// https://en.cppreference.com/w/cpp/filesystem/absolute

VX_API path absolute(const path& p);
VX_API path relative(const path& p, const path& base = get_current_path());
VX_API path canonical(const path& p);

VX_API bool equivalent(const path& p1, const path& p2);

///////////////////////////////////////////////////////////////////////////////
// System Paths
///////////////////////////////////////////////////////////////////////////////

VX_API path get_temp_path();

enum class user_folder
{
    HOME,
    DESKTOP,
    DOCUMENTS,
    DOWNLOADS,
    MUSIC,
    PICTURES,
    VIDEOS
};

VX_API path get_user_folder(user_folder folder);

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
    PATH_NOT_FOUND,
    DIRECTORY_NOT_EMPTY,
    OTHER
};

} // namespace __detail

VX_API bool remove(const path& p);
VX_API size_t remove_all(const path& p);

///////////////////////////////////////////////////////////////////////////////
// Space
///////////////////////////////////////////////////////////////////////////////

struct space_info
{
    size_t capacity;
    size_t free;
    size_t available;
};

VX_API bool space(const path& p, space_info& info);

///////////////////////////////////////////////////////////////////////////////
// Directory Entry
///////////////////////////////////////////////////////////////////////////////

struct directory_entry
{
    void refresh()
    {
        if (!path.empty())
        {
            info = get_symlink_info(path);
        }
    }

    constexpr bool exists() const noexcept { return info.exists(); }
    constexpr bool is_regular_file() const noexcept { return info.is_regular_file(); }
    constexpr bool is_directory() const noexcept { return info.is_directory(); }
    constexpr bool is_symlink() const noexcept { return info.is_symlink(); }
    constexpr bool is_other() const noexcept { return info.is_other(); }

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

    VX_API directory_iterator() noexcept = default;
    VX_API ~directory_iterator() noexcept = default;

    VX_API directory_iterator(const directory_iterator&) noexcept = default;
    VX_API directory_iterator(directory_iterator&&) noexcept = default;

    VX_API directory_iterator& operator=(const directory_iterator&) noexcept = default;
    VX_API directory_iterator& operator=(directory_iterator&&) noexcept = default;

public:

    VX_API reference operator*() const;
    VX_API pointer operator->() const;

    VX_API directory_iterator& operator++();

    bool operator==(const directory_iterator& rhs) const noexcept
    {
        return m_impl == rhs.m_impl;
    }

    bool operator!=(const directory_iterator& rhs) const noexcept
    {
        return m_impl != rhs.m_impl;
    }

    VX_API bool is_valid() const noexcept;

private:

    void validate() noexcept;

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

    VX_API recursive_directory_iterator() noexcept = default;
    VX_API ~recursive_directory_iterator() noexcept = default;

    VX_API recursive_directory_iterator(const recursive_directory_iterator&) noexcept = default;
    VX_API recursive_directory_iterator(recursive_directory_iterator&&) noexcept = default;

    VX_API recursive_directory_iterator& operator=(const recursive_directory_iterator&) noexcept = default;
    VX_API recursive_directory_iterator& operator=(recursive_directory_iterator&&) noexcept = default;

public:

    VX_API reference operator*() const;
    VX_API pointer operator->() const;

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

    VX_API size_t depth() const noexcept;
    VX_API bool recursion_pending() const noexcept;
    VX_API void disable_pending_recursion() noexcept;
    VX_API void pop();

    VX_API bool is_valid() const noexcept;

private:

    void validate() noexcept;

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

///////////////////////////////////////////////////////////////////////////////
// is_empty
///////////////////////////////////////////////////////////////////////////////

inline bool is_empty(const os::path& p)
{
    const file_info info = get_file_info(p);

    if (info.is_directory())
    {
        return directory_iterator(p) == end(directory_iterator(p));
    }

    return info.size == 0;
}

} // namespace filesystem
} // namespace os
} // namespace vx