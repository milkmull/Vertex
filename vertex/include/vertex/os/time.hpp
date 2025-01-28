#pragma once

#include "vertex/util/time/time.hpp"
#include "vertex/util/time/datetime.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Current Time
///////////////////////////////////////////////////////////////////////////////

VX_API time::time_point system_time();

///////////////////////////////////////////////////////////////////////////////
// Ticks
///////////////////////////////////////////////////////////////////////////////

VX_API time::time_point get_ticks();

VX_API int64_t get_performance_counter();
VX_API int64_t get_performance_frequency();

///////////////////////////////////////////////////////////////////////////////
// Sleep
///////////////////////////////////////////////////////////////////////////////

VX_API void sleep(const time::time_point& t);

} // namespace os
} // namespace vx