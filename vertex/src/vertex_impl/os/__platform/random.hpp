#pragma once

#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_random.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_random.hpp"
#endif