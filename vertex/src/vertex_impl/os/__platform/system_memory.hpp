#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_system_memory.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_system_memory.hpp"
#endif