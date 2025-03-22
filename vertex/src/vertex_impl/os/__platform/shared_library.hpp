#pragma once

#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_shared_library.hpp"
#elif defined(VX_PLATFORM_UNIX)
#   include "vertex_impl/os/__platform/unix/unix_shared_library.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_shared_library.hpp"
#endif