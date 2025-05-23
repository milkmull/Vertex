#pragma once

#include "vertex/os/time.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Time
///////////////////////////////////////////////////////////////////////////////

static time::datetime time_point_to_datetime_impl(const time::time_point&, bool)
{
    VX_UNSUPPORTED("time_point::to_datetime()");
    return {};
}

#define unsupported(op) VX_UNSUPPORTED("os::" op "()")

///////////////////////////////////////////////////////////////////////////////
// Current Time
///////////////////////////////////////////////////////////////////////////////

static time::time_point system_time_impl()
{
    unsupported("system_time");
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Ticks
///////////////////////////////////////////////////////////////////////////////

static int64_t get_performance_counter_impl()
{
    unsupported("get_performance_counter");
    return 0;
}

static int64_t get_performance_frequency_impl()
{
    unsupported("get_performance_frequency");
    return 1; // return 1 to avoid division by 0
}

///////////////////////////////////////////////////////////////////////////////
// Sleep
///////////////////////////////////////////////////////////////////////////////

static void sleep_impl(const time::time_point&)
{
    unsupported("sleep");
}

#undef unsupported

} // namespace os
} // namespace vx