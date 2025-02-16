#pragma once

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/os/filesystem.hpp"

namespace vx {
namespace os {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// File Permissions
///////////////////////////////////////////////////////////////////////////////

bool update_permissions_impl(
    const path& p,
    typename file_permissions::type permissions,
    file_permission_operator op,
    bool follow_symlinks
);

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

file_info get_file_info_impl(const path& p);
file_info get_symlink_info_impl(const path& p);

size_t hard_link_count_impl(const path& p);

///////////////////////////////////////////////////////////////////////////////
// Read Symlink
///////////////////////////////////////////////////////////////////////////////

// https://github.com/microsoft/STL/blob/90820002693fe6eaaec2e55884472c654186207e/stl/src/filesystem.cpp#L542

path read_symlink_impl(const path& p);

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

path get_current_path_impl();
bool set_current_path_impl(const path& p);

path absolute_impl(const path& p);
path canonical_impl(const path& p);

bool equivalent_impl(const path& p1, const path& p2);

///////////////////////////////////////////////////////////////////////////////
// System Paths
///////////////////////////////////////////////////////////////////////////////

path get_temp_path_impl();
path get_user_folder_impl(user_folder folder);

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////

bool create_file_impl(const path& p);
bool create_directory_impl(const path& p);
bool create_symlink_impl(const path& target, const path& link);
bool create_directory_symlink_impl(const path& target, const path& link);
bool create_hard_link_impl(const path& target, const path& link);

///////////////////////////////////////////////////////////////////////////////
// Copy
///////////////////////////////////////////////////////////////////////////////

// https://en.cppreference.com/w/cpp/filesystem/copy_file

bool copy_file_impl(const path& from, const path& to, bool overwrite_existing);

///////////////////////////////////////////////////////////////////////////////
// Rename
///////////////////////////////////////////////////////////////////////////////

bool rename_impl(const path& from, const path& to);

///////////////////////////////////////////////////////////////////////////////
// Remove
///////////////////////////////////////////////////////////////////////////////

__detail::remove_error remove_impl(const path& p, bool in_recursive_remove);

///////////////////////////////////////////////////////////////////////////////
// Space
///////////////////////////////////////////////////////////////////////////////

bool space_impl(const path& p, space_info& info);

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

class directory_iterator::directory_iterator_impl
{
    friend directory_iterator;

public:

    directory_iterator_impl(const path& p)
        : m_path(p)
    {
        open();
    }

    ~directory_iterator_impl()
    {
        close();
    }

private:

    void open();
    void close();

public:

    void advance();
    bool is_valid() const noexcept { return m_handle.is_valid(); }

private:

    const path m_path;
    directory_entry m_entry;
    windows::handle m_handle;
    WIN32_FIND_DATAW m_find_data{};
};

///////////////////////////////////////////////////////////////////////////////
// Recursive Directory Iterator
///////////////////////////////////////////////////////////////////////////////

class recursive_directory_iterator::recursive_directory_iterator_impl
{
    friend recursive_directory_iterator;

public:

    recursive_directory_iterator_impl(const path& p)
        : m_path(p)
    {
        push_stack();
        m_recursion_pending = m_entry.is_directory();
    }

    ~recursive_directory_iterator_impl()
    {
        close();
    }

private:

    void close()
    {
        while (!m_stack.empty())
        {
            pop_stack();
        }
    }

    bool push_stack();
    void pop_stack();

public:

    void advance();
    bool is_valid() const noexcept { return !m_stack.empty() && m_stack.back().is_valid(); }

    size_t depth() const noexcept { return m_stack.size() - 1; }
    bool recursion_pending() const noexcept { return m_recursion_pending; }
    void disable_pending_recursion() noexcept { m_recursion_pending = false; }
    void pop() { pop_stack(); advance(); }

private:

    path m_path;
    directory_entry m_entry;
    std::vector<windows::handle> m_stack;
    WIN32_FIND_DATAW m_find_data{};
    bool m_recursion_pending = false;
};

} // namespace filesystem
} // namespace os
} // namespace vx