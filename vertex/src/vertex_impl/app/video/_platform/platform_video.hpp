#pragma once

#include "vertex/app/video/config.hpp"

#if defined(VX_VIDEO_WINDOWS)
#   include "vertex_impl/app/video/_platform/windows/windows_video.hpp"
#else
#   include "vertex_impl/app/video/_platform/dummy/dummy_video.hpp"
#endif