#pragma once

#include <cstdint>

#include "vertex/app/event/event.hpp"
#include "vertex_impl/app/_platform/windows/windows_header.hpp"

#if defined(MOUSE_MOVED)
#   undef MOUSE_MOVED
#endif

namespace vx {
namespace app {
namespace event {

class events_instance_impl
{
public:

    static void pump_events(bool process_all);
    static void wait_events();
    static void wait_events_timeout(unsigned int timeout_ms);
};

} // namespace event
} // namespace app
} // namespace vx