#pragma once

#include "vertex_impl/_platform/_win32/win32_header.h"
#include "vertex/app/event/event.h"

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