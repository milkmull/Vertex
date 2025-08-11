#pragma once

#include <memory>

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
    using type = unsigned int;

    enum : type
    {
        NONE = 0,             // No permissions

        OWNER_READ = 0400,    // Owner has read permission
        OWNER_WRITE = 0200,   // Owner has write permission
        OWNER_EXEC = 0100,    // Owner has execute permission
        OWNER_ALL = 0700,     // Owner has all permissions

        GROUP_READ = 040,     // Group has read permission
        GROUP_WRITE = 020,    // Group has write permission
        GROUP_EXEC = 010,     // Group has execute permission
        GROUP_ALL = 070,      // Group has all permissions

        OTHERS_READ = 04,     // Others have read permission
        OTHERS_WRITE = 02,    // Others have write permission
        OTHERS_EXEC = 01,     // Others have execute permission
        OTHERS_ALL = 07,      // Others have all permissions

        ALL_READ = OWNER_READ | GROUP_READ | OTHERS_READ,             // All users have read permission
        ALL_WRITE = OWNER_WRITE | GROUP_WRITE | OTHERS_WRITE,         // All users have write permission
        ALL_READ_WRITE = ALL_READ | ALL_WRITE,                        // All users have read and write permissions
        ALL_EXEC = OWNER_EXEC | GROUP_EXEC | OTHERS_EXEC,             // All users have execute permission

        ALL = 0777,           // All permissions for owner, group, and others

        SET_UID = 04000,      // Set user ID on execution
        GET_GID = 02000,      // Set group ID on execution

        MASK = 07777,         // Bitmask for all permissions and special bits
        UNKNOWN = 0xFFFF      // Unknown or invalid permissions
    };
};

enum class file_permission_operator
{
    REPLACE,
    ADD,
    REMOVE
};

/**
 * @brief Updates the permissions of a file or directory.
 *
 * @param p The path to the target file or directory.
 * @param permissions The new permissions to apply.
 * @param op The operation to perform on the current permissions
 * (replace, add, or remove). Defaults to replace.
 * @param follow_symlinks If true, symbolic links will be followed;
 * otherwise, permissions are applied to the link itself.
 *
 * @return True on success; false on failure.
 *
 * @note This operation is platform-dependent. If the target cannot be modified,
 * or the path is invalid, the function will fail.
 */
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

    /**
     * @brief Checks whether the file exists.
     * @return True if the file exists; false otherwise.
     */
    constexpr bool exists() const noexcept { return !(type == file_type::NONE || type == file_type::NOT_FOUND); }

    /**
     * @brief Checks if the file is a regular file.
     * @return True if the file is regular; false otherwise.
     */
    constexpr bool is_regular_file() const noexcept { return type == file_type::REGULAR; }

    /**
     * @brief Checks if the file is a directory.
     * @return True if the file is a directory; false otherwise.
     */
    constexpr bool is_directory() const noexcept { return type == file_type::DIRECTORY; }

    /**
     * @brief Checks if the file is a symbolic link.
     * @return True if the file is a symlink; false otherwise.
     */
    constexpr bool is_symlink() const noexcept { return type == file_type::SYMLINK; }

    /**
     * @brief Checks if the file is not a regular file, directory, or symlink.
     * @return True if the file is of another type; false otherwise.
     */
    constexpr bool is_other() const noexcept { return !(is_regular_file() || is_directory() || is_symlink()); }

};

/**
 * @brief Retrieves information about a file or directory.
 *
 * @param p The path to query.
 * @return A file_info structure containing file metadata. If the file does not exist, type is NOT_FOUND or NONE.
 */
VX_API file_info get_file_info(const path& p);

/**
 * @brief Retrieves information about a file or directory without following symbolic links.
 *
 * @param p The path to query.
 * @return A file_info structure. If the path is a symbolic link, the metadata reflects the link itself.
 */
VX_API file_info get_symlink_info(const path& p);

/**
 * @brief Returns the number of hard links to the file.
 *
 * @param p The path to the file.
 * @return Number of hard links. Returns 0 if the path is invalid or the file does not exist.
 */
VX_API size_t hard_link_count(const path& p);

/**
 * @brief Gets the last modification time of a file or symbolic link.
 *
 * @param p The path to the file.
 * @return The last modification time. Returns an empty time_point if the file does not exist.
 *
 * @note If the file does not exist, an error is set and the return is default-initialized.
 */
inline time::time_point get_modify_time(const path& p)
{
    const file_info info = get_symlink_info(p);
    
    if (!info.exists())
    {
        err::set(err::INVALID_ARGUMENT);
        return time::time_point{};
    }

    return info.modify_time;
}

/**
 * @brief Sets the last modification time of a file.
 *
 * @param p The path to the file.
 * @param t The new modification time.
 * @return True on success; false on failure.
 */
VX_API bool set_modify_time(const path& p, time::time_point t);

///////////////////////////////////////////////////////////////////////////////
// Read Symlink
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Reads the target of a symbolic link.
 *
 * @param p The path to the symbolic link.
 * @return The target path if successful; an empty path if the input is not a valid symlink or the operation fails.
 *
 * @note This function does not resolve chains of symlinks or verify if the target exists.
 */
VX_API path read_symlink(const path& p);

///////////////////////////////////////////////////////////////////////////////
// boolean checks
///////////////////////////////////////////////////////////////////////////////


/**
 * @brief Checks if the file or directory at the given path exists.
 *
 * @param p The path to check.
 * @return True if the file or directory exists; false otherwise.
 */
inline bool exists(const path& p)
{
    return get_file_info(p).exists();
}

/**
 * @brief Checks if the path refers to a regular file.
 *
 * @param p The path to check.
 * @return True if the path refers to a regular file; false otherwise.
 */
inline bool is_regular_file(const path& p)
{
    return get_file_info(p).is_regular_file();
}

/**
 * @brief Checks if the path refers to a directory.
 *
 * @param p The path to check.
 * @return True if the path refers to a directory; false otherwise.
 */
inline bool is_directory(const path& p)
{
    return get_file_info(p).is_directory();
}

/**
 * @brief Checks if the path refers to a symbolic link.
 *
 * @param p The path to check.
 * @return True if the path is a symbolic link; false otherwise.
 */
inline bool is_symlink(const path& p)
{
    return get_symlink_info(p).is_symlink();
}

/**
 * @brief Checks if the path refers to something other than a regular file, directory, or symlink.
 *
 * @param p The path to check.
 * @return True if the path refers to another type of file system object; false otherwise.
 */
inline bool is_other(const path& p)
{
    return get_file_info(p).is_other();
}

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Retrieves the current working directory.
 *
 * @return The current path on success; an empty path if the operation fails.
 */
VX_API path get_current_path();

/**
 * @brief Sets the current working directory.
 *
 * @param p The path to set as the current working directory.
 * @return True on success; false on failure.
 */
VX_API bool set_current_path(const path& p);

// https://en.cppreference.com/w/cpp/filesystem/absolute

/**
 * @brief Returns the absolute form of a path.
 *
 * @param p The input path.
 * @return An absolute path if successful; an empty path on failure.
 *
 * @note The returned path is platform-specific and normalized.
 */
VX_API path absolute(const path& p);

/**
 * @brief Computes the relative path from a base path.
 *
 * @param p The target path.
 * @param base The base path to compute from. Defaults to the current path.
 * @return The relative path from base to p; empty on failure.
 *
 * @note If the paths do not share a common root, this operation may fail.
 */
VX_API path relative(const path& p, const path& base = get_current_path());

/**
 * @brief Resolves a path to its canonical (absolute, normalized) form.
 *
 * @param p The path to resolve.
 * @return A canonical path if successful; an empty path on failure.
 *
 * @note This function resolves symbolic links and removes redundant elements.
 */
VX_API path canonical(const path& p);

/**
 * @brief Determines whether two paths refer to the same file or directory.
 *
 * @param p1 First path.
 * @param p2 Second path.
 * @return True if both paths resolve to the same file or directory; false otherwise.
 *
 * @note This check accounts for symbolic links and filesystem-level identity.
 */
VX_API bool equivalent(const path& p1, const path& p2);

///////////////////////////////////////////////////////////////////////////////
// System Paths
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Retrieves the path to the system temporary directory.
 *
 * @return A valid temporary directory path on success; an empty path on failure.
 *
 * @note This path is determined using platform-specific environment variables or APIs.
 */
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

/**
 * @brief Retrieves the path to a specified user folder.
 *
 * @param folder The user folder to retrieve.
 * @return The corresponding path if available; an empty path on failure.
 *
 * @note This function is platform-specific and may not support all folder types on all systems.
 */
VX_API path get_user_folder(user_folder folder);

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Creates an empty file at the specified path.
 *
 * If the file already exists, it will be truncated.
 *
 * @param p The path where the file will be created.
 * @return True if the file is successfully created or already exists; false on failure.
 */
VX_API bool create_file(const path& p);

/**
 * @brief Creates a directory at the specified path.
 *
 * If the directory already exists, the function will succeed.
 *
 * @param p The path of the directory to create.
 * @return True if the directory is successfully created or already exists; false on failure.
 */
VX_API bool create_directory(const path& p);

/**
 * @brief Recursively creates a directory and any necessary parent directories.
 *
 * If any directories already exist, they will be ignored.
 *
 * @param p The path to create.
 * @return True if the directory and all parent directories are successfully created or already exist; false on failure.
 */
VX_API bool create_directories(const path& p);

/**
 * @brief Creates a symbolic link at the given link path pointing to the target.
 *
 * If the symlink already exists, it will be overwritten.
 * The target does not need to exist for the symlink to be created.
 *
 * @param target The path the symlink should point to.
 * @param link The path where the symlink will be created.
 * @return True on success; false on failure.
 */
VX_API bool create_symlink(const path& target, const path& link);

/**
 * @brief Creates a symbolic link specifically for a directory.
 *
 * If the symlink already exists, it will be overwritten.
 * The target does not need to exist for the symlink to be created.
 *
 * @param target The target directory the symlink should point to.
 * @param link The path where the symlink will be created.
 * @return True on success; false on failure.
 */
VX_API bool create_directory_symlink(const path& target, const path& link);

/**
 * @brief Creates a hard link to an existing file.
 *
 * Hard links are supported only for regular files and must be created on the same filesystem.
 *
 * @param target The existing file to link to.
 * @param link The path where the new hard link will be created.
 * @return True if the hard link is successfully created; false on failure.
 */
VX_API bool create_hard_link(const path& target, const path& link);

///////////////////////////////////////////////////////////////////////////////
// Copy
///////////////////////////////////////////////////////////////////////////////

struct copy_options
{
    using type = uint32_t;

    enum : type
    {
        NONE                = 0,            // No options set
        OVERWRITE_EXISTING  = (1 << 1),     // Overwrite the destination file if it already exists
        SKIP_SYMLINKS       = (1 << 2),     // Skip copying symlinks
        DIRECTORIES_ONLY    = (1 << 3),     // Copy only directories, skipping files
        RECURSIVE           = (1 << 4)      // Copy directories and their contents recursively
    };
};

/**
 * @brief Copies a regular file to a new location.
 *
 * If the destination file exists, it will be overwritten if the `overwrite_existing` option is set.
 *
 * @param from The source file path to copy from.
 * @param to The destination file path to copy to.
 * @param overwrite_existing Flag indicating whether to overwrite an existing file at the destination.
 * @return True on success; false on failure.
 *
 * @note If the source file does not exist, or if the source is not a regular file, the operation will fail.
 * @note If the destination file exists and `overwrite_existing` is false, no operation will occur.
 */
VX_API bool copy_file(const path& from, const path& to, bool overwrite_existing = true);

/**
 * @brief Copies a symbolic link to a new location.
 *
 * Only the symlink itself is copied, not the target it points to.
 *
 * @param from The source symlink path to copy from.
 * @param to The destination path to copy the symlink to.
 * @return True on success; false on failure.
 *
 * @note The target of the symlink does not need to exist for the symlink copy to succeed.
 */
VX_API bool copy_symlink(const path& from, const path& to);

/**
 * @brief Copies a file or directory to a new location.
 *
 * The copy operation may overwrite the destination if it already exists, depending on the options specified.
 * For directories, all contents will be copied recursively unless the `copy_options::RECURSIVE` flag is not set.
 * The target symlink is copied as well if the source is a symlink.
 *
 * @param from The source path to copy from.
 * @param to The destination path to copy to.
 * @param options Options to control the copy behavior (e.g., overwrite, recursive).
 * @return True on success; false on failure.
 *
 * @note If the source and destination paths are equivalent, the operation will fail.
 * @note If the source is unsupported (not a regular file, symlink, or directory), the operation will fail.
 */
VX_API bool copy(const path& from, const path& to, typename copy_options::type options = copy_options::NONE);

///////////////////////////////////////////////////////////////////////////////
// Rename
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Renames or moves a file or directory.
 *
 * If the destination path already exists, it will be replaced by the source. The operation behaves like a move,
 * so the source will no longer exist after the operation.
 *
 * @param from The path of the file or directory to rename or move.
 * @param to The new path for the file or directory.
 * @return True on success; false on failure.
 */
VX_API bool rename(const path& from, const path& to);

///////////////////////////////////////////////////////////////////////////////
// Remove
///////////////////////////////////////////////////////////////////////////////

namespace _priv {

enum class remove_error
{
    NONE,
    PATH_NOT_FOUND,
    DIRECTORY_NOT_EMPTY,
    OTHER
};

} // namespace _priv

/**
 * @brief Removes a file or an empty directory.
 *
 * If the path refers to a directory, the directory must be empty for the removal to succeed.
 * If the path refers to a file, the file will be deleted.
 *
 * @param p The path to the file or directory to remove.
 * @return True on success; false on failure.
 *
 * @note If the directory is not empty, the operation will fail.
 */
VX_API bool remove(const path& p);

/**
 * @brief Recursively removes a directory and all its contents.
 *
 * This function removes the directory and all files and subdirectories contained within it.
 * If any files or directories cannot be removed, they will be skipped, and the function will continue removing the rest.
 *
 * @param p The path to the directory to remove.
 * @return The number of files and directories removed.
 */
VX_API size_t remove_all(const path& p);

///////////////////////////////////////////////////////////////////////////////
// Directory Entry
///////////////////////////////////////////////////////////////////////////////

struct directory_entry
{
    /**
     * @brief Refreshes the file information for the entry.
     *
     * This function re-fetches the file information, including type and existence, from the system.
     * It should be called if the underlying file or directory may have changed.
     */
    void refresh()
    {
        if (!path.empty())
        {
            info = get_symlink_info(path);
        }
    }

    /**
     * @brief Checks if the entry exists.
     *
     * @return True if the entry exists, false otherwise.
     */
    constexpr bool exists() const noexcept { return info.exists(); }

    /**
     * @brief Checks if the entry is a regular file.
     *
     * @return True if the entry is a regular file, false otherwise.
     */
    constexpr bool is_regular_file() const noexcept { return info.is_regular_file(); }

    /**
     * @brief Checks if the entry is a directory.
     *
     * @return True if the entry is a directory, false otherwise.
     */
    constexpr bool is_directory() const noexcept { return info.is_directory(); }

    /**
     * @brief Checks if the entry is a symbolic link.
     *
     * @return True if the entry is a symlink, false otherwise.
     */
    constexpr bool is_symlink() const noexcept { return info.is_symlink(); }

    /**
     * @brief Checks if the entry is of an unknown type.
     *
     * @return True if the entry is neither a regular file, directory, nor symlink.
     */
    constexpr bool is_other() const noexcept { return info.is_other(); }

    os::path path;  // The path to the file or directory entry
    file_info info{}; // The file information associated with the path
};

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Iterator for iterating over the entries of a directory.
 *
 * This class provides functionality to iterate through the contents of a directory. It is similar to an input iterator
 * and can be used to access each entry in a directory one by one.
 */
class directory_iterator
{
public:

    using value_type = directory_entry;
    using difference_type = ptrdiff_t;
    using pointer = const directory_entry*;
    using reference = const directory_entry&;
    using iterator_category = std::input_iterator_tag;

public:

    /**
     * @brief Constructs a directory iterator for the specified directory path.
     *
     * This constructor initializes the iterator to start iterating over the given directory.
     *
     * @param p The path to the directory to iterate over.
     */
    VX_API directory_iterator(const path& p);

    directory_iterator() noexcept = default;
    ~directory_iterator() noexcept = default;

    directory_iterator(const directory_iterator&) noexcept = default;
    directory_iterator(directory_iterator&&) noexcept = default;

    directory_iterator& operator=(const directory_iterator&) noexcept = default;
    directory_iterator& operator=(directory_iterator&&) noexcept = default;

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

    /**
     * @brief Checks if the iterator is valid (i.e., it points to a valid directory entry).
     *
     * @return True if the iterator is valid, false otherwise.
     */
    VX_API bool is_valid() const noexcept;

private:

    void validate() noexcept;

private:

    class directory_iterator_impl;
    std::shared_ptr<directory_iterator_impl> m_impl;
};


/**
 * @brief Returns the beginning iterator for a directory iteration.
 *
 * @param it The directory iterator to return.
 * @return The same directory iterator passed as an argument.
 */
inline directory_iterator begin(directory_iterator it) noexcept
{
    return it;
}

/**
 * @brief Returns the end iterator for a directory iteration.
 *
 * @param it The directory iterator to return the end iterator for.
 * @return A default-constructed directory iterator indicating the end.
 */
inline directory_iterator end(directory_iterator) noexcept
{
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Recursive Directory Iterator
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Iterator for recursively iterating over the entries of a directory and its subdirectories.
 *
 * This class allows iterating over all files and subdirectories within a given directory, including the files in
 * nested directories. It is similar to an input iterator and can be used to traverse directories recursively.
 */
class recursive_directory_iterator
{
public:

    using value_type = directory_entry;
    using difference_type = ptrdiff_t;
    using pointer = const directory_entry*;
    using reference = const directory_entry&;
    using iterator_category = std::input_iterator_tag;

public:

    /**
     * @brief Constructs a recursive directory iterator for the specified directory path.
     *
     * This constructor initializes the iterator to start iterating over the given directory and its subdirectories.
     *
     * @param p The path to the directory to iterate over.
     */
    VX_API recursive_directory_iterator(const path& p);

    recursive_directory_iterator() noexcept = default;
    ~recursive_directory_iterator() noexcept = default;

    recursive_directory_iterator(const recursive_directory_iterator&) noexcept = default;
    recursive_directory_iterator(recursive_directory_iterator&&) noexcept = default;

    recursive_directory_iterator& operator=(const recursive_directory_iterator&) noexcept = default;
    recursive_directory_iterator& operator=(recursive_directory_iterator&&) noexcept = default;

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

    /**
     * @brief Returns the depth of the current directory entry in the recursive directory structure.
     *
     * @return The current depth of the directory.
     */
    VX_API size_t depth() const noexcept;

    /**
     * @brief Checks if there are pending directories for recursion.
     *
     * @return True if there are directories pending for recursion, false otherwise.
     */
    VX_API bool recursion_pending() const noexcept;

    /**
     * @brief Disables pending recursion.
     *
     * This function stops recursion for directories that are yet to be processed.
     */
    VX_API void disable_pending_recursion() noexcept;

    /**
     * @brief Pops the current directory from the recursion stack.
     *
     * This removes the current directory from the recursion stack, effectively "going up" one level in the directory tree.
     */
    VX_API void pop();

    /**
     * @brief Checks if the iterator is valid (i.e., it points to a valid directory entry).
     *
     * @return True if the iterator is valid, false otherwise.
     */
    VX_API bool is_valid() const noexcept;

private:

    void validate() noexcept;

private:

    class recursive_directory_iterator_impl;
    std::shared_ptr<recursive_directory_iterator_impl> m_impl;
};

/**
 * @brief Returns the beginning iterator for a recursive directory iteration.
 *
 * @param it The recursive directory iterator to return.
 * @return The same recursive directory iterator passed as an argument.
 */
inline recursive_directory_iterator begin(recursive_directory_iterator it) noexcept
{
    return it;
}

/**
 * @brief Returns the end iterator for a recursive directory iteration.
 *
 * @param it The recursive directory iterator to return the end iterator for.
 * @return A default-constructed recursive directory iterator indicating the end.
 */
inline recursive_directory_iterator end(recursive_directory_iterator) noexcept
{
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Space
///////////////////////////////////////////////////////////////////////////////

struct space_info
{
    size_t capacity;   // Total space available on the disk or volume (in bytes)
    size_t free;       // Free space available on the disk or volume (in bytes)
    size_t available;  // Available space for non-root users (in bytes)
};

/**
 * @brief Retrieves the disk space information for the directory or volume that contains the specified path.
 *
 * This function provides details about the total space, free space, and available space (for non-root users) on the
 * volume containing the specified path. The information is returned in a `space_info` struct.
 *
 * @param p The path whose containing volume's space information is to be retrieved.
 * @return A `space_info` struct containing the total, free, and available space (in bytes).
 */
VX_API space_info space(const path& p);

///////////////////////////////////////////////////////////////////////////////
// is_empty
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if the specified file or directory is empty.
 *
 * This function determines if the given path refers to an empty directory or an empty file.
 * For a file, the check is based on its size being zero. For a directory, the check is done
 * by confirming whether the directory contains any entries.
 *
 * @param p The path to the file or directory to check.
 * @return `true` if the file or directory is empty, `false` otherwise.
 *
 * @note If the specified path does not exist, an error is set and the function returns `false`.
 */
inline bool is_empty(const os::path& p)
{
    const file_info info = get_file_info(p);

    if (!info.exists())
    {
        err::set(err::INVALID_ARGUMENT);
        return false;
    }

    return info.is_directory() ? !directory_iterator(p).is_valid() : (info.size == 0);
}

} // namespace filesystem
} // namespace os
} // namespace vx