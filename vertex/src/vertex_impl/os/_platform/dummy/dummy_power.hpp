#pragma once

#include "vertex/os/power.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::" op "()")

inline bool get_power_info_impl(power_info& info)
{
    unsupported("get_power_info");
    return false;
}

#undef unsupported

} // namespace os
} // namespace vx