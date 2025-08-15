#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_random.hpp"
#elif defined(VX_OS_UNIX)
#   include "vertex_impl/os/_platform/unix/unix_random.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_random.hpp"
#endif