#pragma once

#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_time.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_time.hpp"
#endif