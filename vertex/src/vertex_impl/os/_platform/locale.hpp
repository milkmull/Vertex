#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_locale.hpp"
#elif defined(VX_OS_UNIX)
#   include "vertex_impl/os/_platform/unix/unix_locale.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_locale.hpp"
#endif