#pragma once

#include "vertex_impl/os/_platform/unix/unix_tools.hpp"
#include "vertex/os/handle.hpp"

namespace vx {
namespace os {

inline void close_handle_impl(typename handle::native_handle handle) noexcept
{
    ::close(handle);
}

} // namespace os
} // namespace vx
