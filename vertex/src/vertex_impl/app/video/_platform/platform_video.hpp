#pragma once

#include "vertex/app/video/config/video_driver.hpp"

#if defined(VX_VIDEO_DRIVER_WINDOWS)
#   include "vertex_impl/app/video/_platform/windows/windows_video.hpp"
#else
#   include "vertex_impl/app/video/_platform/dummy/dummy_video.hpp"
#endif