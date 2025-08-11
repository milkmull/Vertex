#pragma once

#include "vertex/os/filesystem.hpp"

namespace vx {
namespace os {
namespace filesystem {

#define unsupported(op) VX_UNSUPPORTED("os::filesystem::" op "()")

///////////////////////////////////////////////////////////////////////////////
// File Permissions
///////////////////////////////////////////////////////////////////////////////

static bool update_permissions_impl(
    const path&,
    typename file_permissions::type,
    file_permission_operator,
    bool
)
{
    unsupported("update_permissions");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

static file_info get_file_info_impl(const path&)
{
    unsupported("get_file_info");
    return {};
}

static file_info get_symlink_info_impl(const path&)
{
    unsupported("get_symlink_info");
    return {};
}

static size_t hard_link_count_impl(const path&)
{
    unsupported("hard_link_count");
    return 0;
}

static bool set_modify_time_impl(const path&, time::time_point)
{
    unsupported("set_modify_time_impl");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Read Symlink
///////////////////////////////////////////////////////////////////////////////

// https://github.com/microsoft/STL/blob/90820002693fe6eaaec2e55884472c654186207e/stl/src/filesystem.cpp#L542

static path read_symlink_impl(const path&)
{
    unsupported("read_symlink");
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

static path get_current_path_impl()
{
    unsupported("get_current_path");
    return {};
}

static bool set_current_path_impl(const path&)
{
    unsupported("set_current_path");
    return false;
}

static path canonical_impl(const path&)
{
    unsupported("canonical");
    return {};
}

static bool equivalent_impl(const path&, const path&)
{
    unsupported("equivalent");
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// System Paths
///////////////////////////////////////////////////////////////////////////////

static path get_temp_path_impl()
{
    unsupported("get_temp_path");
    return {};
}

static path get_user_folder_impl(user_folder)
{
    unsupported("get_user_folder");
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////

static bool create_file_impl(const path&)
{
    unsupported("create_file");
    return false;
}

static bool create_directory_impl(const path&)
{
    unsupported("create_directory");
    return false;
}

static bool create_symlink_impl(const path&, const path&)
{
    unsupported("create_symlink");
    return false;
}

static bool create_directory_symlink_impl(const path&, const path&)
{
    unsupported("create_directory_symlink");
    return false;
}

static bool create_hard_link_impl(const path&, const path&)
{
    unsupported("create_hard_link");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Copy
///////////////////////////////////////////////////////////////////////////////

// https://en.cppreference.com/w/cpp/filesystem/copy_file

static bool copy_file_impl(const path&, const path&, bool)
{
    unsupported("copy_file");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Rename
///////////////////////////////////////////////////////////////////////////////

static bool rename_impl(const path&, const path&)
{
    unsupported("rename");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Remove
///////////////////////////////////////////////////////////////////////////////

static _priv::remove_error remove_impl(const path&, bool)
{
    unsupported("remove");
    return _priv::remove_error::OTHER;
}

///////////////////////////////////////////////////////////////////////////////
// Space
///////////////////////////////////////////////////////////////////////////////

static space_info space_impl(const path&)
{
    unsupported("space");
    return space_info{};
}

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

class directory_iterator::directory_iterator_impl
{
    friend directory_iterator;

public:

    directory_iterator_impl(const path&) {};
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

    recursive_directory_iterator_impl(const path&) {}
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