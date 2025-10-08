#pragma once

#include "vertex/app/video/config.hpp"

#if defined(VX_VIDEO_WINDOWS)
#   include "vertex_impl/app/video/_platform/windows/windows_window.hpp"
#else
#   include "vertex_impl/app/video/_platform/dummy/dummy_window.hpp"
#endif