#pragma once

#include <vector>
#include <string>

#include "vertex/math/geometry/type/recti.h"

namespace vx {
namespace app {
namespace video {

enum class process_dpi_awareness
{
    UNAWARE,
    SYSTEM,
    PER_MONITOR
};

struct display_mode
{
    int width;
    int height;

    int red_bits;
    int green_bits;
    int blue_bits;

    float refresh_rate;
};

struct display
{
    using display_id = size_t;
    display::display_id id;

    bool active;
    std::string name;

    math::recti bounds;
    math::recti work_area;

    float content_scale;
};

//class display
//{
//public:
//
//    using display_id = size_t;
//    explicit display(display_id id);
//
//    display_id id() const;
//    const std::string& name() const;
//    const std::string& gpu_name() const;
//
//    const math::recti& rect() const;
//    const math::recti& work_area() const;
//
//    const std::vector<display_mode>& modes() const;
//    const display_mode& current_mode() const;
//    const display_mode& desktop_mode() const;
//
//    float content_scale() const;
//
//    // const hdr_properties& hdr() const;
//
//    // size_t fullscreen_window() const;
//    // size_t video_device() const;
//
//private:
//
//    display_id m_id;
//
//};

struct driver_info
{
    std::string name;
};

bool init();
void quit();

void poll_displays();
const std::vector<display>& list_displays();

}
}
}