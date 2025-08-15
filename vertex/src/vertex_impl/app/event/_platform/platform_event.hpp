#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/app/event/_platform/windows/windows_event.hpp"
#else
#   include "vertex_impl/app/event/_platform/dummy/dummy_event.hpp"
#endif