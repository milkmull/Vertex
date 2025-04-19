#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_thread.hpp"
#elif defined(VX_OS_UNIX)
#   include "vertex_impl/os/__platform/unix/unix_thread.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_thread.hpp"
#endif