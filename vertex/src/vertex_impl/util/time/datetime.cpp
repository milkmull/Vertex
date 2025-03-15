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
        if (code != err::NONE)
        {
            err::set(code, "hour");
        }
        return false;
    }
    if (dt.minute < 0 || dt.minute > 59)
    {
        if (code != err::NONE)
        {
            err::set(code, "minute");
        }
        return false;
    }
    if (dt.second < 0 || dt.second > 59) // leap second not supported
    {
        if (code != err::NONE)
        {
            err::set(code, "second");
        }
        return false;
    }
    if (dt.nanosecond < 0 || dt.nanosecond > 1000000000)
    {
        if (code != err::NONE)
        {
            err::set(code, "nanosecond");
        }
        return false;
    }

    return true;
}

VX_API bool datetime::is_valid() const noexcept
{
    return check_datetime(*this, err::NONE);
}

// https://en.wikipedia.org/wiki/ISO_8601
// https://dateutil.readthedocs.io/en/stable/parser.html#dateutil.parser.isoparse

VX_API std::string datetime::to_string() const
{
    // YYYY-MM-DDTHH:MM:SS±HH:MM or YYYY-MM-DDTHH:MM:SSZ
    char buffer[26]{};

    if (!check_datetime(*this, err::INVALID_ARGUMENT))
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
        const int32_t abs_offset_seconds = std::abs(utc_offset_seconds);
        const int32_t offset_hours = abs_offset_seconds / __detail::SEC_PER_HOUR;
        const int32_t offset_minutes = static_cast<int32_t>(abs_offset_seconds % __detail::SEC_PER_HOUR) / __detail::SEC_PER_MIN;

        buffer[19] = (utc_offset_seconds < 0) ? '-' : '+';
        std::snprintf(&buffer[20], 6, "%02d:%02d",
            offset_hours,
            offset_minutes
        );
    }

    return buffer;
}

} // namespace time
} // namespace vx