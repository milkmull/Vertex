#pragma once

#include "vertex/app/video/config/video_backend.hpp"

#if defined(VX_VIDEO_BACKEND_WINDOWS)
#   include "vertex_impl/app/input/_platform/windows/windows_pen.hpp"
#else
#   include "vertex_impl/app/input/_platform/dummy/dummy_pen.hpp"
#endif