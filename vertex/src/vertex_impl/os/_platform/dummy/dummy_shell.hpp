#pragma once

#include "vertex/os/shell.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::" op "()")

inline bool shell_open_impl(const std::string& uri)
{
    unsupported("shell_open");
    return false;
}

#undef unsupported

} // namespace os
} // namespace vx