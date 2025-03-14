#pragma once

#include "vertex/os/system_memory.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::" op "()")

inline bool get_memory_info_impl(memory_info&)
{
    unsupported("get_memory_info");
    return false;
}

#undef unsupported

} // namespace os
} // namespace vx