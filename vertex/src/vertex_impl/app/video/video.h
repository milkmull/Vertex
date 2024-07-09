#pragma once

#include "vertex/app/video/video.h"

namespace vx {
namespace app {
namespace video {

struct display_data
{
    device_id id;
    std::string name;

    display_mode desktop_mode;
    display_mode current_mode;
    std::vector<display_mode> modes;

    display_orientation orientation;
    math::vec2 content_scale;
};

struct video_data
{
    std::vector<std::unique_ptr<display>> displays;
    std::vector<std::unique_ptr<window>> windows;
};

extern video_data s_video_data;

}
}
}