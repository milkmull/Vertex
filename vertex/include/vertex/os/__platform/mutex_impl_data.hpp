#pragma once

#include "vertex/system/platform_config.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex/os/__platform/windows/windows_thread_impl_data.hpp"
#else
#   include "vertex/os/__platform/dummy/dummy_thread_impl_data.hpp"
#endif // VX_PLATFORM_WINDOWS

namespace vx {
namespace os {
namespace __detail {

#if defined(VX_PLATFORM_WINDOWS)

using thread_impl_data = ::vx::os::windows::thread_impl_data;

#else

using thread_impl_data = ::vx::os::dummy::thread_impl_data;

#endif // VX_PLATFORM_WINDOWS

} // namespace __detail
} // namespace os
} // namespace vx