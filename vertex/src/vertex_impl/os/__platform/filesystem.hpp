#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_filesystem.hpp"
#elif defined(VX_OS_UNIX)
#   include "vertex_impl/os/__platform/unix/unix_filesystem.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_filesystem.hpp"
#endif