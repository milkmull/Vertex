#pragma once

#include "vertex/app/video/video.h"

namespace vx {
namespace app {
namespace video {

struct display_mode_data
{
    math::vec2i resolution;

    int bpp;
    pixel::pixel_format pixel_format;
    float pixel_density;

    float refresh_rate;

    bool operator==(const display_mode_data& dmd) const
    {
        return resolution == dmd.resolution
            && pixel_format == dmd.pixel_format
            && pixel_density == dmd.pixel_density
            && refresh_rate == dmd.refresh_rate;
    }

    bool operator!=(const display_mode_data& dmd) const
    {
        return !((*this) == dmd);
    }
};

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
};

extern video_data s_video_data;

}
}
}