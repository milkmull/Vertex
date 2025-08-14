#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/app/video/_platform/windows/windows_window.hpp"
#else
#   include "vertex_impl/app/video/_platform/dummy/dummy_window.hpp"
#endif