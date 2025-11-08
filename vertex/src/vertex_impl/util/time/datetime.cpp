#include "vertex/util/time/datetime.hpp"
#include "vertex/system/error.hpp"
#include "vertex/system/assert.hpp"

namespace vx {
namespace time {

///////////////////////////////////////////////////////////////////////////////
// Datetime
///////////////////////////////////////////////////////////////////////////////

static bool check_datetime(const datetime& dt, err::code code) noexcept
{
    if (dt.hour < 0 || dt.hour > 23)
    {
        if (code != err::none)
        {
            err::set(code, "hour");
        }
        return false;
    }
    if (dt.minute < 0 || dt.minute > 59)
    {
        if (code != err::none)
        {
            err::set(code, "minute");
        }
        return false;
    }
    if (dt.second < 0 || dt.second > 59) // leap second not supported
    {
        if (code != err::none)
        {
            err::set(code, "second");
        }
        return false;
    }
    if (dt.nanosecond < 0 || dt.nanosecond > 1000000000)
    {
        if (code != err::none)
        {
            err::set(code, "nanosecond");
        }
        return false;
    }

    return true;
}

VX_API bool datetime::is_valid() const noexcept
{
    return check_datetime(*this, err::none);
}

// https://en.wikipedia.org/wiki/ISO_8601
// https://dateutil.readthedocs.io/en/stable/parser.html#dateutil.parser.isoparse

VX_API std::string datetime::to_string() const
{
    // YYYY-MM-DDTHH:MM:SS±HH:MM or YYYY-MM-DDTHH:MM:SSZ
    char buffer[26]{};

    if (!check_datetime(*this, err::invalid_argument))
    {
        return buffer;
    }

    // Format the date and time in ISO 8601 format
    std::sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d",
        year,
        static_cast<int32_t>(month),
        day,
        hour,
        minute,
        second
    );

    // If the offset is 0, append 'Z' (for UTC)
    if (utc_offset_seconds == 0)
    {
        buffer[19] = 'Z';
    }
    else
    {
        // Convert offset to hours and minutes
        // The utc_offset_seconds represents the offset from UTC in seconds.
        // After taking the absolute value, the offset is divided into hours and minutes.
        // - offset_hours: We limit the range to 0-14 because the maximum valid UTC offset
        //   is UTC+14:00, corresponding to 50400 seconds. This ensures we only work within
        //   the valid time zone range from UTC-12:00 to UTC+14:00 (i.e., total of 27 hours offset).
        // - offset_minutes: Ranges from 0 to 59 (representing the remaining minutes after extracting hours).

        const int32_t abs_offset_seconds = std::abs(utc_offset_seconds);
        const int32_t offset_hours = static_cast<int32_t>(abs_offset_seconds / seconds_per_hour) % 15; // Limit to range [0, 14]
        const int32_t offset_minutes = static_cast<int32_t>(abs_offset_seconds % seconds_per_hour) / seconds_per_minute;

        buffer[19] = (utc_offset_seconds < 0) ? '-' : '+';
        std::sprintf(&buffer[20], "%02d:%02d",
            offset_hours,
            offset_minutes
        );
    }

    return buffer;
}

} // namespace time
} // namespace vx