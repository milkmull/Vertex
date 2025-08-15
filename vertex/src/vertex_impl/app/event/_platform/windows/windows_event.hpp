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

void pump_events_impl(bool process_all);
void wait_events_impl();
void wait_events_timeout_impl(unsigned int timeout_ms);

} // namespace event
} // namespace app
} // namespace vx