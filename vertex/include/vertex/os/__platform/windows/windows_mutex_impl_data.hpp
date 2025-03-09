#pragma once

#include "vertex/os/__platform/windows/windows_types.hpp"

namespace vx {
namespace os {
namespace windows {

// https://github.com/microsoft/STL/blob/0d8f517ae3828284fed594741b847db940167a59/stl/inc/mutex#L33

// https://github.com/microsoft/STL/blob/0d8f517ae3828284fed594741b847db940167a59/stl/src/mutex.cpp

// https://github.com/microsoft/STL/blob/0d8f517ae3828284fed594741b847db940167a59/stl/inc/__msvc_threads_core.hpp#L35

struct mutex_impl_data
{
    windows::handle handle;
    DWORD_ id = 0;
};

} // namespace windows
} // namespace os
} // namespace vx
