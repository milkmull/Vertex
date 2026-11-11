#pragma once

#include <cstdint>

#include "vertex/config/flags.hpp"
#include "vertex/os/handle.hpp"
#include "vertex/os/io.hpp"
#include "vertex/os/native_string.hpp"
#include "vertex/std/temporary_string.hpp"
#include "vertex/std/time/time.hpp"

namespace vx {
namespace os {

//=============================================================================
// File Types
//=============================================================================

enum class file_type
{
    none, // not queried
    not_found,
    regular,
    directory,
    symlink,
    block,
    character,
    fifo,
    socket,
    unknown
};

//=============================================================================
// File Permissions
//=============================================================================

// https://en.cppreference.com/w/cpp/filesystem/file_permissions

VX_FLAGS_UT_DECLARE_BEGIN(file_permissions, unsigned int){
    none = 0, // No permissions

    owner_read = 0400,  // Owner has read permission
    owner_write = 0200, // Owner has write permission
    owner_exec = 0100,  // Owner has execute permission
    owner_all = 0700,   // Owner has all permissions

    group_read = 040,  // Group has read permission
    group_write = 020, // Group has write permission
    group_exec = 010,  // Group has execute permission
    group_all = 070,   // Group has all permissions

    others_read = 04,  // Others have read permission
    others_write = 02, // Others have write permission
    others_exec = 01,  // Others have execute permission
    others_all = 07,   // Others have all permissions

    all_read = owner_read | group_read | others_read,     // All users have read permission
    all_write = owner_write | group_write | others_write, // All users have write permission
    all_read_write = all_read | all_write,                // All users have read and write permissions
    all_exec = owner_exec | group_exec | others_exec,     // All users have execute permission

    all = 0777, // All permissions for owner, group, and others

    set_uid = 04000, // Set user ID on execution
    set_gid = 02000, // Set group ID on execution

    mask = 07777,    // Bitmask for all permissions and special bits
    unknown = 0xffff // Unknown or invalid permissions
} VX_FLAGS_DECLARE_END(file_permissions);

enum class file_permission_operator
{
    replace,
    add,
    remove
};

using file_time = time::time_point;

struct file_id
{
    uint64_t device; // st_dev / volume serial number
    uint64_t index;  // st_ino / file index (high and low combined)

    bool operator==(const file_id& o) const noexcept
    {
        return device == o.device && index == o.index;
    }
};

struct file_stat
{
    file_type type;
    file_permissions permissions;
    uint64_t size;
    uint64_t hard_links;
    file_time modified;
    file_time accessed;
    file_time changed; // ctime on POSIX, creation/change time on Windows
    file_id id;        // used to implement equivalent()
};

struct space_info
{
    uint64_t capacity;
    uint64_t free;
    uint64_t available; // free space available to the calling user
};

struct directory_entry
{
    const native_cstring name; // entry name only, not the full path ("." and ".." are skipped)
    file_type type;            // from d_type / WIN32_FIND_DATA, or unknown if the OS did not report it
};

enum class copy_options
{
    none,
    overwrite_existing,
    fail_if_exists
};

//=========================================================================
// metadata
//=========================================================================

VX_API expected<file_stat, error> stat(const native_cstring p);  // follows symlinks
VX_API expected<file_stat, error> lstat(const native_cstring p); // does not follow symlinks
VX_API expected<file_stat, error> fstat(native_handle h);

VX_API success set_permissions(const native_cstring p, file_permissions mode, bool follow_symlinks = true);
VX_API success set_permissions(native_handle h, file_permissions mode);

VX_API success set_modified_time(const native_cstring p, file_time t);
VX_API success set_modified_time(native_handle h, file_time t);

// io.hpp has flush; truncate is data-stream related but lives here until you add it to io
VX_API expected<size_t, error> file_size(const native_cstring p);
VX_API success resize_file(const native_cstring p, uint64_t size);

//=========================================================================
// files and directories
//=========================================================================

VX_API bool exists(const native_cstring p);

VX_API success create_directory(const native_cstring p, file_permissions mode = file_permissions::all); // single level
VX_API success remove_file(const native_cstring p);                                                     // unlink, not for directories
VX_API success remove_directory(const native_cstring p);                                                // rmdir, must be empty
VX_API success rename(const native_cstring from, const native_cstring to);                              // replaces target where the OS allows

//=========================================================================
// directory iteration
//=========================================================================

struct directory; // opaque, defined per platform in the .cpp
using directory_handle = directory*;

struct directory_entry
{
    native_cstring name; // valid until the next read_directory / close_directory (drop the const)
    file_type type;
};

VX_API expected<directory_handle, error> open_directory(const native_cstring p);
VX_API expected<bool, error> read_directory(directory_handle d, directory_entry& entry); // false at end
VX_API success close_directory(directory_handle d);

//=========================================================================
// links
//=========================================================================

VX_API success create_symlink(const native_cstring target, const native_cstring link, bool target_is_directory);
VX_API success create_hard_link(const native_cstring target, const native_cstring link);
VX_API expected<size_t, error> read_symlink(const native_cstring p, native_char* buf, size_t size);

//=========================================================================
// path resolution
//=========================================================================

VX_API expected<size_t, error> canonical(const native_cstring p, native_char* buf, size_t size);
VX_API expected<size_t, error> current_directory(native_char* buf, size_t size);
VX_API expected<size_t, error> temp_directory(native_char* buf, size_t size);

enum class user_folder
{
    home,
    desktop,
    documents,
    downloads,
    music,
    pictures,
    videos
};

VX_API expected<size_t, error> user_directory(user_folder folder, native_char* buf, size_t cap);

//=========================================================================
// space and copy
//=========================================================================

VX_API expected<space_info, error> space(const native_cstring p);

// fast path: reflink / copy_file_range / sendfile / CopyFileEx
// returns not_supported if no fast path exists so the front end can fall back to read/write
VX_API success copy_file_native(const native_cstring from, const native_cstring to, copy_options options);

} // namespace os
} // namespace vx
