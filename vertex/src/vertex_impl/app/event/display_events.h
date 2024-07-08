#pragma once

#include "vertex/app/event/event.h"
#include "vertex/app/video/video.h"

namespace vx {
namespace app {

void post_display_event(event_type type, const video::display& display);

}
}