#pragma once

#include <filesystem>

// https://docs.godotengine.org/en/stable/classes/class_fileaccess.html#class-fileaccess

namespace vx {
namespace filesystem {

inline bool exists(const std::string& path)
{
    return std::filesystem::exists(path);
}

}
}