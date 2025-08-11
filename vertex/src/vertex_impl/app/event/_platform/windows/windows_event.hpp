#pragma once

#include "vertex_impl/app/_platform/windows/windows_header.hpp"

namespace vx {
namespace app {
namespace event {

void pump_events_impl(bool process_all);
void wait_events_impl();
void wait_events_timeout_impl(unsigned int timeout_ms);


} // namespace event
} // namespace app
} // namespace vx