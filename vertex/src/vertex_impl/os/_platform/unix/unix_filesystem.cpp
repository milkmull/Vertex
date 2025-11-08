#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>  // For PATH_MAX
#include <unistd.h>  // For readlink()
#include <sys/statvfs.h> // space

#include "vertex_impl/os/_platform/unix/unix_filesystem.hpp"
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
    file_permissions permissions,
    file_permission_operator op,
    bool follow_symlinks
)
{
    struct stat st {};
    const int res = follow_symlinks ? ::stat(p.c_str(), &st) : ::lstat(p.c_str(), &st);
    if (res != 0)
    {
        unix_::error_message(follow_symlinks ? "stat()" : "lstat()");
        return false;
    }

    // On unix, symlinks don't have meaningful permissions. Attempting to set permissions
    // on a symlink usually results in an error (e.g., 'Operation not supported') because
    // symlinks are simply pointers to other files, and access control is determined by
    // the target file's permissions, not the symlink itself.
    //
    // Therefore, modifying permissions on symlinks is typically unnecessary and unsupported.
    // This function returns success without making any changes if the file is a symlink
    // and we're not following it, as setting permissions on symlinks doesn't affect their behavior.
    if (S_ISLNK(st.st_mode) && !follow_symlinks)
    {
        return true;
    }

    const mode_t current_mode = st.st_mode;
    mode_t new_mode = current_mode;
    const mode_t masked = permissions & file_permissions::mask;

    switch (op)
    {
        case file_permission_operator::replace:
        {
            new_mode = (current_mode & ~file_permissions::all) | masked;
            break;
        }
        case file_permission_operator::add:
        {
            new_mode = current_mode | masked;
            break;
        }
        case file_permission_operator::remove:
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

    if (::chmod(p.c_str(), new_mode) != 0)
    {
        unix_::error_message("chmod()");
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

static inline bool is_directory_internal(const path& p) noexcept
{
    struct stat st {};
    return (::stat(p.c_str(), &st) == 0) && S_ISDIR(st.st_mode);
}

static file_type to_file_type(mode_t mode) noexcept
{
    if (S_ISREG(mode))     return file_type::regular;
    if (S_ISDIR(mode))     return file_type::directory;
    if (S_ISLNK(mode))     return file_type::symlink;

    return file_type::unknown;
}

static file_permissions to_file_permissions(mode_t mode) noexcept
{
    file_permissions result = file_permissions::none;

    if (mode & S_IRUSR) result |= file_permissions::owner_read;
    if (mode & S_IWUSR) result |= file_permissions::owner_write;
    if (mode & S_IXUSR) result |= file_permissions::owner_exec;

    if (mode & S_IRGRP) result |= file_permissions::group_read;
    if (mode & S_IWGRP) result |= file_permissions::group_write;
    if (mode & S_IXGRP) result |= file_permissions::group_exec;

    if (mode & S_IROTH) result |= file_permissions::others_read;
    if (mode & S_IWOTH) result |= file_permissions::others_write;
    if (mode & S_IXOTH) result |= file_permissions::others_exec;

    if (mode & S_ISUID) result |= file_permissions::set_uid;
    if (mode & S_ISGID) result |= file_permissions::get_gid;

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
        S_ISDIR(st.st_mode) ? 0 : static_cast<size_t>(st.st_size),
        to_time_point(st.st_ctim),
        to_time_point(st.st_mtim)
    };
}

file_info get_file_info_impl(const path& p)
{
    struct stat st {};
    if (::stat(p.c_str(), &st) != 0)
    {
        unix_::error_message("stat()");
        return {};
    }

    return file_info_from_stat(st);
}

file_info get_symlink_info_impl(const path& p)
{
    struct stat st {};
    if (::lstat(p.c_str(), &st) != 0)
    {
        unix_::error_message("lstat()");
        return {};
    }

    return file_info_from_stat(st);
}

size_t hard_link_count_impl(const path& p)
{
    struct stat st {};
    if (::stat(p.c_str(), &st) != 0)
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
    if (::utimensat(AT_FDCWD, p.c_str(), times, 0) != 0)
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
    // readlink does NOT null-terminate the result
    char buffer[PATH_MAX];
    const ssize_t size = ::readlink(p.c_str(), buffer, sizeof(buffer));
    if (size < 0)
    {
        unix_::error_message("readlink()");
        return {};
    }

    return path(buffer, buffer + size);
}

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

path get_current_path_impl()
{
    char buffer[PATH_MAX];
    if (::getcwd(buffer, sizeof(buffer)) == NULL)
    {
        unix_::error_message("getcwd()");
        return {};
    }

    return path{ buffer };
}

bool set_current_path_impl(const path& p)
{
    if (::chdir(p.c_str()) != 0)
    {
        unix_::error_message("chdir()");
        return false;
    }

    return true;
}

// https://github.com/boostorg/filesystem/blob/30b312e5c0335831af61ad16802e888f5fb344ea/src/operations.cpp#L2686

path canonical_impl(const path& p)
{
    const path normalized = p.lexically_normal();
    path res;

    // Use realpath to resolve the canonical absolute path
    // realpath automatically resolves symbolic links and returns an absolute path
    char buffer[PATH_MAX];
    if (::realpath(normalized.c_str(), buffer) == NULL)
    {
        unix_::error_message("realpath()");
    }
    else
    {
        res.assign(buffer);
    }

    return res;
}

// https://github.com/boostorg/filesystem/blob/c7e14488032b98ba81ffaf1aa813ada422dd4da1/src/operations.cpp#L3671

bool equivalent_impl(const path& p1, const path& p2)
{
    struct stat s2 {};
    const int e2 = ::stat(p2.c_str(), &s2);
    struct stat s1 {};
    const int e1 = ::stat(p1.c_str(), &s1);

    if (VX_UNLIKELY(e1 != 0 || e2 != 0))
    {
        // if one is invalid and the other isn't then they aren't equivalent,
        // but if both are invalid then it is an error
        if (e1 != 0 && e2 != 0)
        {
            unix_::error_message("stat()");
        }

        return false;
    }

    // According to the POSIX stat specs, "The st_ino and st_dev fields
    // taken together uniquely identify the file within the system."
    return s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino;
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
    return is_directory_internal(tmp_path) ? tmp_path : path{};
}

// https://github.com/libsdl-org/SDL/blob/90fd2a3cbee5b7ead1f0517d7cc0eba1f3059207/src/filesystem/unix/SDL_sysfilesystem.c#L518

static std::string xdg_user_dir_lookup(const char* key)
{
    std::string resolved;

    // Get $HOME environment variable
    const char* home = std::getenv("HOME");
    if (!home) return resolved;

    // Get $XDG_CONFIG_HOME or fallback to ~/.config
    const char* config_home = std::getenv("XDG_CONFIG_HOME");
    const path config_file = config_home
        ? path(config_home) / "user-dirs.dirs"
        : path(home) / ".config/user-dirs.dirs";

    file f;
    if (!f.open(config_file, file::mode::read))
    {
        return resolved;
    }

    const std::string prefix = std::string(key) + "=\"";
    std::string line;

    while (f.read_line(line))
    {
        auto s = line.find(prefix);
        if (s == std::string::npos)
        {
            continue;
        }

        s += prefix.size();
        auto e = line.find('"', s);
        if (e == std::string::npos)
        {
            continue;
        }
        
        const std::string value = line.substr(s, e - s);
        if (str::starts_with(value, "$HOME"))
        {
            // Expand $HOME if path starts with it
            resolved = std::string(home) + '/' + value.substr(6);
        }
        else if (value.front() == '/')
        {
            // Otherwise accept absolute paths
            resolved = value;
        }
        else
        {
            // Ignore other (invalid/unsupported) formats
            continue;
        }

        break;
    }

    return resolved;
}

path get_user_folder_impl(user_folder folder)
{
    const char* home = std::getenv("HOME");
    const char* key = nullptr;

    // According to 'man xdg-user-dir', the possible values are:
    // { DESKTOP, DOWNLOAD, TEMPLATES, PUBLICSHARE, DOCUMENTS, MUSIC, PICTURES, VIDEOS }

    switch (folder)
    {
        case user_folder::home:
        {
            return home;
        }
        case user_folder::desktop:
        {
            key = "XDG_DESKTOP_DIR";
            break;
        }
        case user_folder::documents:
        {
            key = "XDG_DOCUMENTS_DIR";
            break;
        }
        case user_folder::downloads:
        {
            key = "XDG_DOWNLOAD_DIR";
            break;
        }
        case user_folder::music:
        {
            key = "XDG_MUSIC_DIR";
            break;
        }
        case user_folder::pictures:
        {
            key = "XDG_PICTURES_DIR";
            break;
        }
        case user_folder::videos:
        {
            key = "XDG_VIDEOS_DIR";
            break;
        }
    }

    VX_ASSERT(key != nullptr);
    return xdg_user_dir_lookup(key);
}

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////

bool create_file_impl(const path& p)
{
    int fd = ::open(p.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd == -1)
    {
        unix_::error_message("open()");
        return false;
    }

    ::close(fd);
    return true;
}

bool create_directory_impl(const path& p)
{
    // Attempt to create the directory
    if (::mkdir(p.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
    {
        // Directory exists, check if it's a directory
        if (errno == EEXIST && is_directory_internal(p))
        {
            return true;
        }

        unix_::error_message("mkdir()");
        return false;
    }

    return true;
}

bool create_symlink_impl(const path& target, const path& link)
{
    if (::symlink(target.c_str(), link.c_str()) != 0)
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
    if (!overwrite_existing && exists(to))
    {
        err::set(err::system_error, "copy_file(): file already exists");
        return false; // Do not overwrite
    }

    file from_file;
    if (!from_file.open(from, file::mode::read))
    {
        return false;
    }

    file to_file;
    if (!to_file.open(to, file::mode::write))
    {
        return false;
    }

    // Copy the file content in chunks
    constexpr size_t size = 4096;
    char data[size];
    size_t bytes_read, bytes_written;

    while (true)
    {
        bytes_read = from_file.read(data, size);
        if (bytes_read == 0)
        {
            break;
        }

        bytes_written = to_file.write(data, bytes_read);
        if (bytes_written != bytes_read)
        {
            return false;
        }
    }

    return true;
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

static _priv::remove_error remove_directory(const path& p, bool in_recursive_remove) noexcept
{
    if (::rmdir(p.c_str()) != 0)
    {
        const bool not_empty = (errno == ENOTEMPTY || errno == EEXIST);
        if (!not_empty || (not_empty && !in_recursive_remove))
        {
            // don't report an error in recursive remove
            unix_::error_message("rmdir()");
        }

        return not_empty
            ? _priv::remove_error::directory_not_empty
            : _priv::remove_error::other;
    }

    return _priv::remove_error::none;
}

static _priv::remove_error remove_file(const path& p) noexcept
{
    if (::unlink(p.c_str()) != 0)
    {
        unix_::error_message("unlink()");
        return _priv::remove_error::other;
    }

    return _priv::remove_error::none;
}

_priv::remove_error remove_impl(const path& p, bool in_recursive_remove)
{
    struct stat st {};
    if (::lstat(p.c_str(), &st) != 0)
    {
        if (errno == ENOENT || errno == ENOTDIR)
        {
            // path or file is already gone
            return _priv::remove_error::path_not_found;
        }

        unix_::error_message("lstat()");
        return _priv::remove_error::other;
    }

    if (S_ISDIR(st.st_mode))
    {
        return remove_directory(p, in_recursive_remove);
    }

    return remove_file(p);
}

///////////////////////////////////////////////////////////////////////////////
// Space
///////////////////////////////////////////////////////////////////////////////

space_info space_impl(const path& p)
{
    struct statvfs vfs {};
    if (::statvfs(p.c_str(), &vfs) != 0)
    {
        // Handle error: log, throw, or return empty as in Windows version
        unix_::error_message("statvfs()");
        return {};
    }

    // Total capacity in bytes
    const size_t capacity = static_cast<size_t>(vfs.f_blocks) * vfs.f_frsize;
    // Free space (including reserved blocks) in bytes
    const size_t free = static_cast<size_t>(vfs.f_bfree) * vfs.f_frsize;
    // Available space (excluding reserved blocks) in bytes
    const size_t available = static_cast<size_t>(vfs.f_bavail) * vfs.f_frsize;

    return space_info{
        capacity,
        free,
        available
    };
}

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator Helpers
///////////////////////////////////////////////////////////////////////////////

static bool is_dot_or_dotdot(const char* filename)
{
    if (filename[0] != '.')
    {
        return false;
    }

    const char second_char = filename[1];
    if (second_char == 0)
    {
        return true;
    }

    if (second_char != '.')
    {
        return false;
    }

    return filename[2] == 0;
}

static void close_directory_iterator(DIR*& dir)
{
    if (dir != NULL)
    {
        ::closedir(dir);
        dir = NULL;
    }
}

static void update_directory_iterator_entry(const path& p, directory_entry& entry, struct dirent* ent)
{
    entry.path = p / ent->d_name;
    entry.info = get_symlink_info_impl(entry.path);
}

static bool advance_directory_iterator_once(DIR*& dir, struct dirent*& ent)
{
    do
    {
        ent = ::readdir(dir);
        if (ent == NULL)
        {
            close_directory_iterator(dir);
            break;
        }

    } while (is_dot_or_dotdot(ent->d_name));

    return dir != NULL;
}

static void advance_directory_iterator(const path& p, directory_entry& entry, DIR*& dir)
{
    struct dirent* ent = NULL;

    if (advance_directory_iterator_once(dir, ent))
    {
        update_directory_iterator_entry(p, entry, ent);
    }
}

static void open_directory_iterator(const path& p, directory_entry& entry, DIR*& dir)
{
    close_directory_iterator(dir);

    dir = ::opendir(p.c_str());
    if (dir == NULL)
    {
        unix_::error_message("opendir()");
        return;
    }

    advance_directory_iterator(p, entry, dir);
}

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

void directory_iterator::directory_iterator_impl::open()
{
    open_directory_iterator(m_path, m_entry, m_dir);
}

void directory_iterator::directory_iterator_impl::close()
{
    close_directory_iterator(m_dir);
}

void directory_iterator::directory_iterator_impl::advance()
{
    advance_directory_iterator(m_path, m_entry, m_dir);
}

///////////////////////////////////////////////////////////////////////////////
// Recursive Directory Iterator
///////////////////////////////////////////////////////////////////////////////

bool recursive_directory_iterator::recursive_directory_iterator_impl::push_stack()
{
    m_path /= m_entry.path.filename();
    DIR* dir = NULL;
    open_directory_iterator(m_path, m_entry, dir);

    if (dir == NULL)
    {
        m_path.pop_back();
        return false;
    }

    m_stack.push_back(dir);
    return true;
}

void recursive_directory_iterator::recursive_directory_iterator_impl::pop_stack()
{
    m_path.pop_back();
    close_directory_iterator(m_stack.back());
    m_stack.pop_back();
}

void recursive_directory_iterator::recursive_directory_iterator_impl::advance()
{
    DIR** current = &m_stack.back();

    if (m_recursion_pending && push_stack())
    {
        current = &m_stack.back();
    }
    else
    {
        advance_directory_iterator(m_path, m_entry, *current);
    }

    while (*current == NULL)
    {
        pop_stack();
        if (m_stack.empty())
        {
            break;
        }

        current = &m_stack.back();
        advance_directory_iterator(m_path, m_entry, *current);
    }

    m_recursion_pending = m_entry.is_directory();
}

} // namespace filesystem
} // namespace os
} // namespace vx