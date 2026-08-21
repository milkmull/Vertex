#pragma once

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

namespace vx {
namespace os {

inline void close_handle_impl(typename handle::native_handle handle) noexcept
{
    ::CloseHandle(handle);
}

} // namespace os
} // namespace vx
