#include "vertex/system/error.hpp"

#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_filesystem.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_filesystem.hpp"
#endif

namespace vx {
namespace os {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// File Permissions
///////////////////////////////////////////////////////////////////////////////

VX_API bool update_permissions(
    const path& p,
    typename file_permissions::type permissions,
    file_permission_operator op,
    bool follow_symlinks
)
{
    return update_permissions_impl(p, permissions, op, follow_symlinks);
}

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

VX_API file_info get_file_info(const path& p)
{
    return get_file_info_impl(p);
}

VX_API file_info get_symlink_info(const path& p)
{
    return get_symlink_info_impl(p);
}

///////////////////////////////////////////////////////////////////////////////
// Read Symlink
///////////////////////////////////////////////////////////////////////////////

VX_API path read_symlink(const path& p)
{
    return read_symlink_impl(p);
}

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

VX_API path get_current_path()
{
    return get_current_path_impl();
}

VX_API bool set_current_path(const path& p)
{
    return set_current_path_impl(p);
}

VX_API path absolute(const path& p)
{
    return absolute_impl(p);
}

// https://github.com/boostorg/filesystem/blob/30b312e5c0335831af61ad16802e888f5fb344ea/src/operations.cpp#L2570

VX_API path canonical(const path& p)
{
    if (!exists(p))
    {
        VX_ERR(err::FILE_OPEN_FAILED) << "file not found: " << p;
        return {};
    }

    return canonical_impl(p);
}

VX_API path relative(const path& p, const path& base)
{
    const path p_abs = absolute(p);
    const path b_abs = absolute(base);
    return p_abs.lexically_relative(b_abs);
}

VX_API bool equivalent(const path& p1, const path& p2)
{
    return equivalent_impl(p1, p2);
}

///////////////////////////////////////////////////////////////////////////////
// System Paths
///////////////////////////////////////////////////////////////////////////////

VX_API path get_temp_path()
{
    return get_temp_path_impl();
}

VX_API path get_user_folder(user_folder folder)
{
    return get_user_folder_impl(folder);
}

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////

VX_API bool create_file(const path& p)
{
    return create_file_impl(p);
}

VX_API bool create_symlink(const path& target, const path& link)
{
    return create_symlink_impl(target, link);
}

VX_API bool create_directory_symlink(const path& target, const path& link)
{
    return create_directory_symlink_impl(target, link);
}

VX_API bool create_directory(const path& p)
{
    return create_directory_impl(p);
}

VX_API bool create_directories(const path& p)
{
    path tmp = p.root_path();

    for (const auto& dir : p.relative_path())
    {
        tmp /= dir;

        if (!create_directory(tmp))
        {
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Copy
///////////////////////////////////////////////////////////////////////////////

enum class copy_error
{
    FROM_NOT_FOUND,
    EQUIVALENT_PATHS,
    TO_PATH_ALREADY_EXISTS,
    FROM_UNSUPPORTED_TYPE,
    TO_UNSUPPORTED_TYPE
};

static void throw_copy_error(copy_error e, const path& from, const path& to)
{
    std::ostringstream oss;
    oss << "failed to copy " << from << ": ";

    if (e == copy_error::FROM_NOT_FOUND)
    {
        oss << "from file does not exist";
    }
    else if (e == copy_error::EQUIVALENT_PATHS)
    {
        oss << "from and to refer to the same file";
    }
    else if (e == copy_error::TO_PATH_ALREADY_EXISTS)
    {
        oss << "cannot override to path";
    }
    else // e == copy_error::FROM_UNSUPPORTED_TYPE || e == copy_error::TO_UNSUPPORTED_TYPE
    {
        oss << ((e == copy_error::FROM_UNSUPPORTED_TYPE) ? "from" : "to") << " file is unsupported type";
    }

    VX_ERR(err::FILE_OPERATION_FAILED) << oss.str();
}

// https://en.cppreference.com/w/cpp/filesystem/copy_file

VX_API bool copy_file(const path& from, const path& to, bool overwrite_existing)
{
    const file_info from_info = get_file_info(from);

    // If !is_regular_file (either because the source file doesn't exist or because it is not a regular file), report an error
    if (!from_info.exists())
    {
        throw_copy_error(copy_error::FROM_NOT_FOUND, from, to);
        return false;
    }
    if (from_info.type != file_type::REGULAR)
    {
        throw_copy_error(copy_error::FROM_UNSUPPORTED_TYPE, from, to);
        return false;
    }

    const file_info to_info = get_file_info(to);

    if (to_info.exists())
    {
        // Report an error if to and from are the same
        if (equivalent(from, to))
        {
            throw_copy_error(copy_error::EQUIVALENT_PATHS, from, to);
            return false;
        }

        // Report an error if to is not a regular file
        if (to_info.type != file_type::REGULAR)
        {
            throw_copy_error(copy_error::TO_UNSUPPORTED_TYPE, from, to);
            return false;
        }

        // If the file exists and we don't want to override, do nothing
        if (!overwrite_existing)
        {
            return true;
        }
    }

    return copy_file_impl(from, to, overwrite_existing);
}

VX_API bool copy_symlink(const path& from, const path& to)
{
    const path target = read_symlink(from);
    if (target.empty())
    {
        return false;
    }

    return create_symlink(target, to);
}

// https://en.cppreference.com/w/cpp/filesystem/copy

VX_API bool copy(const path& from, const path& to, typename copy_options::type options)
{
    file_info from_info, to_info;

    if ((options & copy_options::COPY_SYMLINKS) || (options & copy_options::SKIP_SYMLINKS))
    {
        from_info = get_symlink_info(from);
        to_info = get_symlink_info(to);
    }
    else
    {
        // Default behavior is to resolve symlinks and copy the target
        from_info = get_file_info(from);
        to_info = get_file_info(to);
    }

    // If from does not exist, reports an error
    if (!from_info.exists())
    {
        throw_copy_error(copy_error::EQUIVALENT_PATHS, from, to);
        return false;
    }

    // If from and to are the same, reports an error
    if (equivalent(from, to))
    {
        throw_copy_error(copy_error::EQUIVALENT_PATHS, from, to);
        return false;
    }

    // If either from or to is not a regular file, a directory, or a symlink, as determined by is_other, reports an error
    if (from_info.is_other())
    {
        throw_copy_error(copy_error::FROM_UNSUPPORTED_TYPE, from, to);
        return false;
    }
    if (to_info.exists() && to_info.is_other())
    {
        throw_copy_error(copy_error::TO_UNSUPPORTED_TYPE, from, to);
        return false;
    }

    // If from is a directory, but to is a regular file, reports an error
    if (from_info.is_directory() && to_info.is_regular_file())
    {
        throw_copy_error(copy_error::TO_UNSUPPORTED_TYPE, from, to);
        return false;
    }

    if (from_info.is_symlink())
    {
        // If skip_symlink is present in options, does nothing
        if (options & copy_options::SKIP_SYMLINKS)
        {
            return true;
        }

        VX_ASSERT(options & copy_options::COPY_SYMLINKS);

        if (to_info.exists())
        {
            throw_copy_error(copy_error::TO_PATH_ALREADY_EXISTS, from, to);
            return false;
        }

        return copy_symlink(from, to);
    }
    else if (from_info.is_regular_file())
    {
        // If directories_only is present in options, does nothing
        if (options & copy_options::DIRECTORIES_ONLY)
        {
            return true;
        }

        // If to is a directory, creates a copy of from as a file in the directory to
        if (to_info.is_directory())
        {
            return copy_file_impl(from, to / from.filename(), (options & copy_options::OVERWRITE_EXISTING));
        }

        return copy_file_impl(from, to, (options & copy_options::OVERWRITE_EXISTING));
    }
    else // from_info.is_directory()
    {
        // Create the directory
        if (!to_info.exists() && !create_directory(to))
        {
            return false;
        }

        if (options & copy_options::RECURSIVE)
        {
            for (const auto& e : directory_iterator(from))
            {
                if (!copy(e.path, to / e.path.filename(), options))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Rename
///////////////////////////////////////////////////////////////////////////////

VX_API bool rename(const path& from, const path& to)
{
    return rename_impl(from, to);
}

///////////////////////////////////////////////////////////////////////////////
// Remove
///////////////////////////////////////////////////////////////////////////////

VX_API bool remove(const path& p)
{
    const auto res = remove_impl(p, false);
    return (res == __detail::remove_error::NONE);
}

static size_t recursive_remove(const path& p)
{
    size_t count = 0;

    for (const auto& dir : directory_iterator(p))
    {
        if (dir.is_directory())
        {
            count += remove_all(dir.path);
        }

        if (remove_impl(dir.path, true) == __detail::remove_error::NONE)
        {
            ++count;
        }
    }

    if (remove_impl(p, true) == __detail::remove_error::NONE)
    {
        ++count;
    }

    return count;
}

VX_API size_t remove_all(const path& p)
{
    switch (remove_impl(p, true))
    {
        case __detail::remove_error::NONE:
        {
            return 1;
        }
        case __detail::remove_error::DIRECTORY_NOT_EMPTY:
        {
            return recursive_remove(p);
        }
        default:
        {
            return 0;
        }

    }
}

///////////////////////////////////////////////////////////////////////////////
// Space
///////////////////////////////////////////////////////////////////////////////

VX_API bool space(const path& p, space_info& info)
{
    return space_impl(p, info);
}

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

VX_API directory_iterator::directory_iterator(const path& p)
    : m_impl(std::make_shared<directory_iterator_impl>(p))
{
    validate();
}

VX_API typename directory_iterator::reference directory_iterator::operator*() const
{
    VX_ASSERT(m_impl);
    return m_impl->m_entry;
}

VX_API typename directory_iterator::pointer directory_iterator::operator->() const
{
    VX_ASSERT(m_impl);
    return &**this;
}

VX_API directory_iterator& directory_iterator::operator++()
{
    VX_ASSERT(m_impl);
    m_impl->advance();
    validate();
    return *this;
}

VX_API bool directory_iterator::is_valid() const noexcept
{
    return m_impl && m_impl->is_valid();
}

void directory_iterator::validate() noexcept
{
    if (!m_impl->is_valid())
    {
        m_impl.reset();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Recursive Directory Iterator
///////////////////////////////////////////////////////////////////////////////

VX_API recursive_directory_iterator::recursive_directory_iterator(const path& p)
    : m_impl(std::make_shared<recursive_directory_iterator_impl>(p))
{
    validate();
}

VX_API typename recursive_directory_iterator::reference recursive_directory_iterator::operator*() const
{
    VX_ASSERT(m_impl);
    return m_impl->m_entry;
}

VX_API typename recursive_directory_iterator::pointer recursive_directory_iterator::operator->() const
{
    VX_ASSERT(m_impl);
    return &**this;
}

VX_API recursive_directory_iterator& recursive_directory_iterator::operator++()
{
    VX_ASSERT(m_impl);
    m_impl->advance();
    validate();
    return *this;
}

VX_API size_t recursive_directory_iterator::depth() const noexcept
{
    VX_ASSERT(m_impl);
    return m_impl->depth();
}

VX_API bool recursive_directory_iterator::recursion_pending() const noexcept
{
    VX_ASSERT(m_impl);
    return m_impl->recursion_pending();
}

VX_API void recursive_directory_iterator::disable_pending_recursion() noexcept
{
    VX_ASSERT(m_impl);
    m_impl->disable_pending_recursion();
}

VX_API void recursive_directory_iterator::pop()
{
    VX_ASSERT(m_impl);
    if (depth() == 0)
    {
        m_impl.reset();
    }
    else
    {
        m_impl->pop();
    }
}

VX_API bool recursive_directory_iterator::is_valid() const noexcept
{
    return m_impl && m_impl->is_valid();
}

void recursive_directory_iterator::validate() noexcept
{
    if (!m_impl->is_valid())
    {
        m_impl.reset();
    }
}

} // namespace filesystem
} // namespace os
} // namespace vx