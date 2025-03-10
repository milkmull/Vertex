#pragma once

#include "vertex/os/__platform/windows/windows_types.hpp"

namespace vx {
namespace os {
namespace windows {

struct shared_library_impl_data
{
    HMODULE_ handle = NULL;
};

} // namespace windows
} // namespace os
} // namespace vx
