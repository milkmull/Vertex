#pragma once

#include "vertex/system/platform_config.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex/os/__platform/windows/windows_file_impl_data.hpp"
#elif defined(VX_PLATFORM_UNIX)
#   include "vertex/os/__platform/unix/unix_file_impl_data.hpp"
#else
#   include "vertex/os/__platform/dummy/dummy_file_impl_data.hpp"
#endif // VX_PLATFORM_*

namespace vx {
namespace os {
namespace __detail {

#if defined(VX_PLATFORM_WINDOWS)

using file_impl_data = ::vx::os::windows::file_impl_data;

#elif defined(VX_PLATFORM_UNIX)

using file_impl_data = ::vx::os::unix::file_impl_data;

#else

using file_impl_data = ::vx::os::dummy::file_impl_data;

#endif // VX_PLATFORM_WINDOWS

} // namespace __detail
} // namespace os
} // namespace vx