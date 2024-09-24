#pragma once

#include <vector>
#include <string>

#include "vertex/stdlib/system/time.hpp"

// https://docs.godotengine.org/en/stable/classes/class_fileaccess.html#class-fileaccess
// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_filesystem.h
// https://en.cppreference.com/w/cpp/filesystem

namespace vx {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// File Ops
///////////////////////////////////////////////////////////////////////////////

bool exists(const std::string& path);
bool create_file(const std::string& path, bool fail_if_exists = true);
bool create_directory(const std::string& path, bool fail_if_exists = true);
bool rename(const std::string& old_path, const std::string& new_path);
bool remove(const std::string& path);

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

bool copy(const std::string& old_path, const std::string& new_path, copy_options::type options = copy_options::NONE);

///////////////////////////////////////////////////////////////////////////////
// File Info
///////////////////////////////////////////////////////////////////////////////

std::string get_absolute_path(const std::string& path);

std::string get_current_directory();
bool set_current_directory(const std::string& path);

std::vector<std::string> list(const std::string& path, bool absolute = false);

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
    time::time_t create_time;
    time::time_t modify_time;
    time::time_t access_time;
};

path_info get_path_info(const std::string& path);

enum class system_folder
{
    HOME,
    DESKTOP,
    DOCUMENTS,
    DOWNLOADS,
    MUSIC,
    PICTURES,
    VIDEOS
};

std::string get_system_folder(system_folder folder);

}
}