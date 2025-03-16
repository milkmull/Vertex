#pragma once

#include "vertex/os/__platform/windows/windows_handle.hpp"

namespace vx {
namespace os {
namespace windows {

struct thread_impl_data
{
    handle h;
    DWORD_ id = 0;
};

} // namespace windows
} // namespace os
} // namespace vx
