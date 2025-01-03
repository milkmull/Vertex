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
    file_type type;
    typename file_permissions::type permissions;

    size_t size;
    size_t hard_link_count;

    time::time_point create_time;
    time::time_point modify_time;
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
    return get_file_info(p).type != file_type::NONE;
}

///////////////////////////////////////////////////////////////////////////////
// Directory Entry
///////////////////////////////////////////////////////////////////////////////

class directory_entry
{
public:

    directory_entry() noexcept = default;
    explicit directory_entry(const os::path& p) : m_path(p), m_info(get_file_info(p)) {}

public:

    void refresh()
    {
        if (!m_path.empty())
        {
            m_info = get_file_info(m_path);
        }
    }

    const os::path& path() const { return m_path; }

    bool exists() const { return m_info.type != file_type::NONE; }

    bool is_regular_file() const { return m_info.type == file_type::REGULAR; }
    bool is_directory() const { return m_info.type == file_type::DIRECTORY; }
    bool is_symlink() const { return m_info.type == file_type::SYMLINK; }
    bool is_other() const { return !(is_regular_file() || is_directory() || is_symlink()); }

    size_t file_size() const { return m_info.size; }
    size_t hard_link_count() const { return m_info.hard_link_count; }

    time::time_point create_time() const { return m_info.create_time; }
    time::time_point modify_time() const { return m_info.modify_time; }

private:

    os::path m_path;
    file_info m_info;
};

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

class directory_iterator
{
public:

private:

    class directory_iterator_impl;
    std::unique_ptr<directory_iterator_impl> m_impl;
};

} // namespace filesystem
} // namespace os
} // namespace vx