#pragma once

#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_file.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_file.hpp"
#endif