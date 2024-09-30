#pragma once

#include "vertex/stdlib/string/string.hpp"

namespace vx {
namespace str {

std::string format(const std::string& fmt, const std::string* args, size_t count);

template <typename... Args>
std::string format(const std::string& fmt, Args&&... args)
{
    const std::string sargs[sizeof...(Args)] = { str::to_string(std::forward<Args>(args))... };
    return format(fmt, sargs, sizeof...(Args));
}

}
}