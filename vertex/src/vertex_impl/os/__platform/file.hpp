#pragma once

#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_file.hpp"
#elif defined(VX_PLATFORM_LINUX)
#   include "vertex_impl/os/__platform/posix/posix_file.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_file.hpp"
#endif