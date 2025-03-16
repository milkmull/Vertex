#pragma once

#include "vertex/os/__platform/unix/unix_handle.hpp"

namespace vx {
namespace os {
namespace unix {

struct file_impl_data
{
    handle h;
};

} // namespace unix
} // namespace os
} // namespace vx
