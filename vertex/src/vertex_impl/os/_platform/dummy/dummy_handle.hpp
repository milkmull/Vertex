#pragma once

#include "vertex/os/handle.hpp"

namespace vx {
namespace os {

inline void close_handle_impl(typename handle::native_handle) noexcept
{
}

} // namespace os
} // namespace vx
