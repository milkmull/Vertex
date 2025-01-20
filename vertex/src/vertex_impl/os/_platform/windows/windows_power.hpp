#pragma once

#include "vertex/os/power.hpp"
#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

namespace vx {
namespace os {

// https://learn.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-system_power_status

bool get_power_info_impl(power_info& info);

} // namespace os
} // namespace vx