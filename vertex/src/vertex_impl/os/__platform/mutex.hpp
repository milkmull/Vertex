#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_mutex.hpp"
#elif defined(VX_OS_UNIX)
#   include "vertex_impl/os/__platform/unix/unix_mutex.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_mutex.hpp"
#endif