#pragma once

#include "vertex/os/filesystem.hpp"

namespace vx {
namespace os {
namespace filesystem {

#define unsupported(op) VX_UNSUPPORTED("os::filesystem::" op "()")

///////////////////////////////////////////////////////////////////////////////
// File Permissions
///////////////////////////////////////////////////////////////////////////////

inline bool update_file_permissions_impl(
    const path& p,
    typename file_permissions::type permissions,
    file_permission_operator op,
    bool follow_symlinks
)
{
    unsupported("update_file_permissions");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

inline file_info get_file_info_impl(const path& p)
{
    unsupported("get_file_info");
    return {};
}

inline file_info get_symlink_info_impl(const path& p)
{
    unsupported("get_symlink_info");
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Read Symlink
///////////////////////////////////////////////////////////////////////////////

// https://github.com/microsoft/STL/blob/90820002693fe6eaaec2e55884472c654186207e/stl/src/filesystem.cpp#L542

inline path read_symlink_impl(const path& p)
{
    unsupported("read_symlink");
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

inline path get_current_path_impl()
{
    unsupported("get_current_path");
    return {};
}

inline bool set_current_path_impl(const path& p)
{
    unsupported("set_current_path");
    return false;
}

inline path absolute_impl(const path& p)
{
    unsupported("absolute");
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// System Paths
///////////////////////////////////////////////////////////////////////////////

inline path get_temp_path_impl()
{
    unsupported("get_temp_path");
    return {};
}

inline path get_user_folder_impl(user_folder folder)
{
    unsupported("get_user_folder");
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////

inline bool create_file_impl(const path& p)
{
    unsupported("create_file");
    return false;
}

inline bool create_symlink_impl(const path& target, const path& link)
{
    unsupported("create_symlink");
    return false;
}

inline bool create_directory_symlink_impl(const path& target, const path& link)
{
    unsupported("create_directory_symlink");
    return false;
}

inline bool create_directory_impl(const path& p)
{
    unsupported("create_directory");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Copy
///////////////////////////////////////////////////////////////////////////////

// https://en.cppreference.com/w/cpp/filesystem/copy_file

inline bool copy_file_impl(const path& from, const path& to, bool overwrite_existing)
{
    unsupported("copy_file");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Rename
///////////////////////////////////////////////////////////////////////////////

inline bool rename_impl(const path& from, const path& to)
{
    unsupported("rename");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Remove
///////////////////////////////////////////////////////////////////////////////

inline __detail::remove_error remove_impl(const path& p, bool in_recursive_remove)
{
    unsupported("remove");
    return __detail::remove_error::OTHER;
}

///////////////////////////////////////////////////////////////////////////////
// Space
///////////////////////////////////////////////////////////////////////////////

bool space_impl(const path& p, space_info& info)
{
    unsupported("space");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

class directory_iterator::directory_iterator_impl
{
    friend directory_iterator;

public:

    directory_iterator_impl(const path& p) {};
    ~directory_iterator_impl() = default;

public:

    void advance()
    {
        unsupported("directory_itertor::advance");
    }

    bool is_valid() const
    {
        unsupported("directory_itertor::is_valid");
        return false;
    }

private:

    directory_entry m_entry;
};

///////////////////////////////////////////////////////////////////////////////
// Recursive Directory Iterator
///////////////////////////////////////////////////////////////////////////////

class recursive_directory_iterator::recursive_directory_iterator_impl
{
    friend recursive_directory_iterator;

public:

    recursive_directory_iterator_impl(const path& p) {}
    ~recursive_directory_iterator_impl() = default;

public:

    void advance()
    {
        unsupported("recursive_directory_iterator::advance");
    }

    bool is_valid() const
    {
        unsupported("recursive_directory_iterator::is_valid");
        return false;
    }

    size_t depth() const
    {
        unsupported("recursive_directory_iterator::depth");
        return 0;
    }

    bool recursion_pending() const
    {
        unsupported("recursive_directory_iterator::recursion_pending");
        return false;
    }

    void disable_pending_recursion()
    {
        unsupported("recursive_directory_iterator::disable_pending_recursion");
    }

    void pop()
    {
        unsupported("recursive_directory_iterator::pop");
    }

private:

    directory_entry m_entry;
};

#undef unsupported

} // namespace filesystem
} // namespace os
} // namespace vx