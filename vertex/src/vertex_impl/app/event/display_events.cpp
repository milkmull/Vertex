#include "display_events.h"

namespace vx {
namespace app {

void post_display_event(event_type type, const video::display& display)
{
    event e{};
    e.type = type;
    e.display_event.display_id = display.id();
    event::post_event(e);

    switch (type)
    {
        case event_type::DISPLAY_ADDED:
        {
            break;
        }
        case event_type::DISPLAY_REMOVED:
        {
            break;
        }
        case event_type::DISPLAY_MOVED:
        {
            break;
        }
        case event_type::DISPLAY_ORIENTATION_CHANGED:
        {
            break;
        }
        case event_type::DISPLAY_CONTENT_SCALE_CHANGED:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}

}
}