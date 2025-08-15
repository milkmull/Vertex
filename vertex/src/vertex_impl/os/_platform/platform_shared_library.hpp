#pragma once

#include "vertex/config/os.hpp"

#if defined(VX_OS_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_shared_library.hpp"
#elif defined(VX_OS_UNIX) && defined(HAVE_DLOPEN)
#   include "vertex_impl/os/_platform/unix/unix_shared_library.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_shared_library.hpp"
#endif