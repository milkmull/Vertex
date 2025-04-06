#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_random.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_random.hpp"
#endif