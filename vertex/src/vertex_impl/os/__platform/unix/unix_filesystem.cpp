#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>  // For PATH_MAX
#include <unistd.h>  // For readlink()

#include "vertex_impl/os/__platform/unix/unix_filesystem.hpp"
#include "vertex/os/file.hpp"
#include "vertex/system/error.hpp"

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
)
{
    struct stat st {};
    int res = follow_symlinks ? stat(p.c_str(), &st) : lstat(p.c_str(), &st);

    if (res != 0)
    {
        unix_::error_message(follow_symlinks ? "stat()" : "lstat()");
        return false;
    }

    const mode_t current_mode = st.st_mode;
    mode_t new_mode = current_mode;
    const mode_t masked = permissions & file_permissions::MASK;

    switch (op)
    {
        case file_permission_operator::REPLACE:
        {
            new_mode = (current_mode & ~file_permissions::MASK) | masked;
            break;
        }
        case file_permission_operator::ADD:
        {
            new_mode = current_mode | masked;
            break;
        }
        case file_permission_operator::REMOVE:
        {
            new_mode = current_mode & ~masked;
            break;
        }
    }

    // Only update if necessary
    if (new_mode == current_mode)
    {
        return true;
    }

    res = follow_symlinks ? chmod(p.c_str(), new_mode) : fchmodat(AT_FDCWD, p.c_str(), new_mode, AT_SYMLINK_NOFOLLOW);
    if (res != 0)
    {
        unix_::error_message(follow_symlinks ? "chmod()" : "fchmodat()");
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

static file_type to_file_type(mode_t mode) noexcept
{
    if (S_ISREG(mode))     return file_type::REGULAR;
    if (S_ISDIR(mode))     return file_type::DIRECTORY;
    if (S_ISLNK(mode))     return file_type::SYMLINK;

    return file_type::UNKNOWN;
}

static file_permissions::type to_file_permissions(mode_t mode) noexcept
{
    file_permissions::type result = file_permissions::NONE;

    if (mode & S_IRUSR) result |= file_permissions::OWNER_READ;
    if (mode & S_IWUSR) result |= file_permissions::OWNER_WRITE;
    if (mode & S_IXUSR) result |= file_permissions::OWNER_EXEC;

    if (mode & S_IRGRP) result |= file_permissions::GROUP_READ;
    if (mode & S_IWGRP) result |= file_permissions::GROUP_WRITE;
    if (mode & S_IXGRP) result |= file_permissions::GROUP_EXEC;

    if (mode & S_IROTH) result |= file_permissions::OTHERS_READ;
    if (mode & S_IWOTH) result |= file_permissions::OTHERS_WRITE;
    if (mode & S_IXOTH) result |= file_permissions::OTHERS_EXEC;

    if (mode & S_ISUID) result |= file_permissions::SET_UID;
    if (mode & S_ISGID) result |= file_permissions::GET_GID;

    return result;
}

static time::time_point to_time_point(const timespec& ts) noexcept
{
    return time::seconds(ts.tv_sec) + time::nanoseconds(ts.tv_nsec);
}

static file_info file_info_from_stat(const struct stat& st) noexcept
{
    return file_info{
        to_file_type(st.st_mode),
        to_file_permissions(st.st_mode),
        static_cast<size_t>(st.st_size),
        to_time_point(st.st_ctim),
        to_time_point(st.st_mtim)
    };
}

file_info get_file_info_impl(const path& p)
{
    struct stat st {};
    if (stat(p.c_str(), &st) != 0)
    {
        unix_::error_message("stat()");
        return {};
    }

    return file_info_from_stat(st);
}

file_info get_symlink_info_impl(const path& p)
{
    struct stat st {};
    if (lstat(p.c_str(), &st) != 0)
    {
        unix_::error_message("lstat()");
        return {};
    }

    return file_info_from_stat(st);
}

size_t hard_link_count_impl(const path& p)
{
    struct stat st {};
    if (stat(p.c_str(), &st) != 0)
    {
        unix_::error_message("stat()");
        return 0;
    }

    return static_cast<size_t>(st.st_nlink);
}

bool set_modify_time_impl(const path& p, time::time_point t)
{
    struct timespec times[2];

    // Leave access time unchanged
    times[0].tv_nsec = UTIME_OMIT;

    times[1].tv_sec = static_cast<time_t>(t.as_seconds());
    times[1].tv_nsec = static_cast<long>(t.as_nanoseconds() % time::nanoseconds_per_second);

    // Use AT_FDCWD to refer to the path directly
    if (utimensat(AT_FDCWD, p.c_str(), times, 0) != 0)
    {
        unix_::error_message("utimensat()");
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Read Symlink
///////////////////////////////////////////////////////////////////////////////

path read_symlink_impl(const path& p)
{
    std::vector<char> buffer(PATH_MAX);

    // readlink does NOT null-terminate the result
    const ssize_t len = readlink(p.c_str(), buffer.data(), buffer.size());

    if (len < 0)
    {
        unix_::error_message("readlink()");
        return {};
    }

    return path(std::string(buffer.data(), static_cast<size_t>(len)));
}

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

path get_current_path_impl()
{
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) == NULL)
    {
        unix_::error_message("getcwd()");
        return {};
    }

    return path{ buffer };
}

bool set_current_path_impl(const path& p)
{
    if (chdir(p.c_str()) != 0)
    {
        unix_::error_message("chdir()");
        return false;
    }

    return true;
}

path absolute_impl(const path& p)
{
    char buffer[PATH_MAX];

    // realpath() returns the absolute pathname or NULL on error
    char* absolute_path = realpath(p.c_str(), buffer);
    if (absolute_path == NULL)
    {
        unix_::error_message("realpath()");
        return {};
    }

    return path{ absolute_path };
}

// https://github.com/boostorg/filesystem/blob/30b312e5c0335831af61ad16802e888f5fb344ea/src/operations.cpp#L2686

path canonical_impl(const path& p)
{
    return {};
}

bool equivalent_impl(const path& p1, const path& p2)
{
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// System Paths
///////////////////////////////////////////////////////////////////////////////

path get_temp_path_impl()
{
    const char* env_list[] = { "TMPDIR", "TMP", "TEMP", "TEMPDIR" };

#if defined(__ANDROID__)
    const char* default_tmp = "/data/local/tmp";
#else
    const char* default_tmp = "/tmp";
#endif

    const char* tmp = nullptr;
    for (const char* env : env_list)
    {
        tmp = std::getenv(env);
        if (tmp)
        {
            break;
        }
    }

    const path tmp_path{ tmp ? tmp : default_tmp };
    return is_directory(tmp_path) ? tmp_path : path{};
}

path get_user_folder_impl(user_folder folder)
{
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////

bool create_file_impl(const path& p)
{
    // Open the file for writing, creating it if it doesn't exist
    int fd = open(p.c_str(), O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    if (fd == -1)
    {
        unix_::error_message("open()");
        return false;
    }

    // Close the file descriptor
    close(fd);
    return true;
}

bool create_directory_impl(const path& p)
{
    // Attempt to create the directory
    if (mkdir(p.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
    {
        if (errno == EEXIST)
        {
            // Directory exists, check if it's a directory
            struct stat st;
            if (stat(p.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
            {
                return true;
            }
        }

        unix_::error_message("mkdir()");
        return false;
    }

    return true;
}

bool create_symlink_impl(const path& target, const path& link)
{
    if (symlink(target.c_str(), link.c_str()) != 0)
    {
        unix_::error_message("symlink()");
        return false;
    }

    return true;
}

bool create_directory_symlink_impl(const path& target, const path& link)
{
    // Simply call create_symlink_impl since there is no need for a flag to indicate directories on Unix
    return create_symlink_impl(target, link);
}

bool create_hard_link_impl(const path& target, const path& link)
{
    if (::link(target.c_str(), link.c_str()) != 0)
    {
        unix_::error_message("link()");
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Copy
///////////////////////////////////////////////////////////////////////////////

bool copy_file_impl(const path& from, const path& to, bool overwrite_existing)
{
    // Check if the destination file exists
    if (!overwrite_existing)
    {
        struct stat buffer;
        if (stat(to.c_str(), &buffer) == 0) // File exists
        {
            return false; // Do not overwrite
        }
    }

    // Open the source file for reading
    int src_fd = open(from.c_str(), O_RDONLY);
    if (src_fd == -1)
    {
        unix_::error_message("open()");
        return false;
    }

    // Open (or create) the destination file for writing
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    int dest_fd = open(to.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (dest_fd == -1)
    {
        unix_::error_message("open()");
        close(src_fd);
        return false;
    }

    // Copy the file content in chunks
    const size_t buffer_size = 4096;
    char buffer[buffer_size];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(src_fd, buffer, buffer_size)) > 0)
    {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read)
        {
            unix_::error_message("write()");
            close(src_fd);
            close(dest_fd);
            return false;
        }
    }

    if (bytes_read == -1)
    {
        unix_::error_message("read()");
    }

    close(src_fd);
    close(dest_fd);

    return bytes_read != -1; // Return true if no error occurred
}

///////////////////////////////////////////////////////////////////////////////
// Rename
///////////////////////////////////////////////////////////////////////////////

// https://github.com/microsoft/STL/blob/04081522161549b18fa0b0fae0d2a531ee967958/stl/src/filesystem.cpp#L720

bool rename_impl(const path& from, const path& to)
{
    if (::rename(from.c_str(), to.c_str()) != 0)
    {
        unix_::error_message("rename()");
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Remove
///////////////////////////////////////////////////////////////////////////////

__detail::remove_error remove_impl(const path& p, bool in_recursive_remove)
{
    return __detail::remove_error::NONE;
}

///////////////////////////////////////////////////////////////////////////////
// Space
///////////////////////////////////////////////////////////////////////////////

space_info space_impl(const path& p)
{
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator Helpers
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

void directory_iterator::directory_iterator_impl::open()
{
}

void directory_iterator::directory_iterator_impl::close()
{
}

void directory_iterator::directory_iterator_impl::advance()
{
}

///////////////////////////////////////////////////////////////////////////////
// Recursive Directory Iterator
///////////////////////////////////////////////////////////////////////////////

bool recursive_directory_iterator::recursive_directory_iterator_impl::push_stack()
{
    return false;
}

void recursive_directory_iterator::recursive_directory_iterator_impl::pop_stack()
{
}

void recursive_directory_iterator::recursive_directory_iterator_impl::advance()
{
}

} // namespace filesystem
} // namespace os
} // namespace vx