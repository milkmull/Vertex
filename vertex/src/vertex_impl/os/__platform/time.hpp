#pragma once

#include "vertex/system/platform_config.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_time.hpp"
#elif defined(VX_PLATFORM_UNIX)
#   include "vertex_impl/os/__platform/unix/unix_time.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_time.hpp"
#endif