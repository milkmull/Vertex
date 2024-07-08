#pragma once

#include "vertex/app/event/event.h"
#include "../win32_header.h"

namespace vx {
namespace app {

class event::event_impl
{
public:

    static void pump_events(bool process_all);
    static void wait_events();
    static void wait_events_timeout(unsigned int timeout_ms);

};

}
}