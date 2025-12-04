#pragma once

#include "vertex/app/video/config/video_backend.hpp"

#if defined(VX_VIDEO_BACKEND_WINDOWS)
#   include "vertex_impl/app/message_box/_platform/windows/windows_message_box.hpp"
#else
#   include "vertex_impl/app/message_box/message_box_internal.hpp"
#endif