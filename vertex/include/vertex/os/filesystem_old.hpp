#pragma once

#include <vector>
#include <string>

#include "vertex/os/path.hpp"
#include "vertex/os/time.hpp"

// https://docs.godotengine.org/en/stable/classes/class_fileaccess.html#class-fileaccess
// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_filesystem.h
// https://en.cppreference.com/w/cpp/filesystem

namespace vx {
namespace os {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// File Ops
///////////////////////////////////////////////////////////////////////////////

VX_API bool exists(const std::string& path);
VX_API bool create_directory(const std::string& path);
VX_API bool create_directories(const std::string& path);
VX_API bool rename(const std::string& old_path, const std::string& new_path, bool overwrite_existing = false);
VX_API bool remove(const std::string& path);

struct copy_options
{
    using type = uint32_t;

    enum : type
    {
        NONE               = 0,
        SKIP_EXISTING      = (1 << 0),
        OVERWRITE_EXISTING = (1 << 1),
        RECURSIVE          = (1 << 2),
        DIRECTORY_ONLY     = (1 << 3)
    };
};

VX_API bool copy(const std::string& old_path, const std::string& new_path, copy_options::type options = copy_options::NONE);

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

VX_API std::string get_absolute_path(const std::string& path);

VX_API std::string get_current_directory();
VX_API bool change_current_directory(const std::string& path);

VX_API std::vector<std::string> list();
VX_API std::vector<std::string> list(const std::string& path, bool absolute = false);

enum class path_type
{
    NONE,
    FILE,
    DIRECTORY,
    OTHER
};

struct path_info
{
    path_type type;
    size_t size;
    time::time_point create_time;
    time::time_point modify_time;
    time::time_point access_time;
};

VX_API path_info get_path_info(const std::string& path);

// GetTempPath2W
VX_API std::string get_app_data_folder(const std::string& app);

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

//VX_API std::string get_user_folder(user_folder folder);

} // namespace filesystem
} // namespace os
} // namespace vx