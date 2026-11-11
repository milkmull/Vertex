#pragma once

#include <cstdint>

#include "vertex/config/os.hpp"
#include "vertex/std/error.hpp"

namespace vx {
namespace os {

using nanoseconds = int64_t;

//=============================================================================
// Current Time
//=============================================================================

VX_API nanoseconds system_time();

//=============================================================================
// Ticks
//=============================================================================

VX_API int64_t get_performance_counter();
VX_API int64_t get_performance_frequency();

//=============================================================================
// Sleep
//=============================================================================

VX_API void sleep(nanoseconds d);

//=============================================================================
// Datetime
//=============================================================================

struct datetime_fields
{
    int32_t year = 0;               // year
    int32_t month = 1;              // month [1-12]
    int32_t day = 1;                // day [1-31]
    int32_t hour = 0;               // hour [0-23]
    int32_t minute = 0;             // minute [0-59]
    int32_t second = 0;             // second [0-59] leap seconds not supported
    int32_t nanosecond = 0;         // nanosecond [0-999999999]
    int32_t utc_offset_seconds = 0; // seconds east of utc [-43200-50400]
};

VX_API success system_time_to_datetime(nanoseconds ns, bool local, datetime_fields& out);

} // namespace os
} // namespace vx
