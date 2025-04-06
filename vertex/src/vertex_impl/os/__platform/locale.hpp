#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_locale.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_locale.hpp"
#endif