#pragma once

#include "vertex/os/__platform/windows/windows_types.hpp"

namespace vx {
namespace os {
namespace windows {

struct thread_impl_data
{
    windows::handle handle;
    DWORD_ id = 0;
};

} // namespace windows
} // namespace os
} // namespace vx
