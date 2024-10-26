#include "vertex/os/time.hpp"
#include "vertex/core/assert.hpp"
#include "vertex/core/platform_defines.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace os {
namespace time {

///////////////////////////////////////////////////////////////////////////////
// Enums
///////////////////////////////////////////////////////////////////////////////

const char* weekday_to_string(weekday d)
{
    switch (d)
    {
        default:
        case weekday::SUNDAY:       return "Sunday";
        case weekday::MONDAY:       return "Monday";
        case weekday::TUESDAY:      return "Tuesady";
        case weekday::WEDNESDAY:    return "Wednesday";
        case weekday::THURSDAY:     return "Thurday";
        case weekday::FRIDAY:       return "Friday";
        case weekday::SATURDAY:     return "Saturday";
    }
}

const char* month_to_string(month m)
{
    switch (m)
    {
        default:
        case month::JANUARY:        return "January";
        case month::FEBRUARY:       return "February";
        case month::MARCH:          return "March";
        case month::APRIL:          return "April";
        case month::MAY:            return "May";
        case month::JUNE:           return "June";
        case month::JULY:           return "July";
        case month::AUGUST:         return "August";
        case month::SEPTEMBER:      return "September";
        case month::OCTOBER:        return "October";
        case month::NOVEMBER:       return "November";
        case month::DECEMBER:       return "December";
    }
}

///////////////////////////////////////////////////////////////////////////////
// Date Helpers
///////////////////////////////////////////////////////////////////////////////

// A year is a leap year if it is...
// divisible by 4...
// but not divisible by 100...
// unless it is also divisible by 400
#define IS_LEAP_YEAR(year) !(year % 4) && ((year % 100) || !(year % 400))

// http://howardhinnant.github.io/date_algorithms.html#days_from_civil

static int32_t civil_to_days(int32_t year, month month, int32_t day, weekday* day_of_week, int32_t* day_of_year)
{
    const int32_t imonth = static_cast<int32_t>(month);

    year -= imonth <= 2;
    const int32_t era = (year >= 0 ? year : year - 399) / 400;
    const uint32_t yoe = static_cast<uint32_t>(year - era * 400);                               // [0, 399]
    const uint32_t doy = (153 * (imonth > 2 ? imonth - 3 : imonth + 9) + 2) / 5 + day - 1;      // [0, 365]
    const uint32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;                                 // [0, 146096]
    const int32_t z = era * 146097 + static_cast<int32_t>(doe) - 719468;

    if (day_of_week)
    {
        // weekday_from_days algorithm
        *day_of_week = static_cast<weekday>(z >= -4 ? (z + 4) % 7 : (z + 5) % 7 + 6);
    }
    if (day_of_year)
    {
        // The algorithm considers March 1 to be the first day of the year, so we offset by Jan + Feb days
        if (doy > 305)
        {
            *day_of_year = static_cast<int32_t>(doy) - 306;
        }
        else
        {
            *day_of_year = static_cast<int32_t>(doy) + 59 + static_cast<int32_t>(IS_LEAP_YEAR(year));
        }
    }

    return z;
}

bool is_leap_year(int32_t year)
{
    return IS_LEAP_YEAR(year);
}

int32_t days_in_year(int32_t year)
{
    return is_leap_year(year) ? 366 : 365;
}

int32_t days_in_month(int32_t year, month month)
{
    static const uint8_t days_in_month[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    if (month < month::JANUARY || month > month::DECEMBER)
    {
        err::set(err::INVALID_ARGUMENT);
        return 0;
    }

    int32_t days = days_in_month[static_cast<int32_t>(month)];
    if (month == month::FEBRUARY && is_leap_year(year))
    {
        ++days;
    }

    return days;
}

int32_t get_day_of_year(int32_t year, month month, int32_t day)
{
    if (month < month::JANUARY || month > month::DECEMBER)
    {
        err::set(err::INVALID_ARGUMENT);
        return 0;
    }
    if (day < 1 || day > days_in_year(year))
    {
        err::set(err::INVALID_ARGUMENT);
        return 0;
    }

    int32_t doy;
    civil_to_days(year, month, day, nullptr, &doy);

    return doy;
}

weekday get_day_of_week(int32_t year, month month, int32_t day)
{
    if (month < month::JANUARY || month > month::DECEMBER)
    {
        err::set(err::INVALID_ARGUMENT);
        return weekday::SUNDAY;
    }
    if (day < 1 || day > days_in_year(year))
    {
        err::set(err::INVALID_ARGUMENT);
        return weekday::SUNDAY;
    }

    weekday dow;
    civil_to_days(year, month, day, &dow, nullptr);

    return dow;
}

///////////////////////////////////////////////////////////////////////////////
// Datetime
///////////////////////////////////////////////////////////////////////////////

static bool check_datetime(const datetime& dt, err::code code)
{
    if (dt.month < month::JANUARY || dt.month > month::DECEMBER)
    {
        if (code != err::NONE)
        {
            VX_ERR(code) << "invalid month: " << static_cast<int32_t>(dt.month);
        }
        return false;
    }
    if (dt.hour < 0 || dt.hour > 23)
    {
        if (code != err::NONE)
        {
            VX_ERR(code) << "invalid hour: " << dt.hour;
        }
        return false;
    }
    if (dt.minute < 0 || dt.minute > 59)
    {
        if (code != err::NONE)
        {
            VX_ERR(code) << "invalid minute: " << dt.minute;
        }
        return false;
    }
    if (dt.second < 0 || dt.second > 59) // leap second not supported
    {
        if (code != err::NONE)
        {
            VX_ERR(code) << "invalid second: " << dt.second;
        }
        return false;
    }
    if (dt.nanosecond < 0 || dt.nanosecond > NS_PER_SEC)
    {
        if (code != err::NONE)
        {
            VX_ERR(code) << "invalid nanosecond: " << dt.nanosecond;
        }
        return false;
    }

    return true;
}

bool is_valid_datetime(const datetime& dt)
{
    return check_datetime(dt, err::NONE);
}

time_t datetime_to_time(const datetime& dt)
{
    if (!check_datetime(dt, err::INVALID_ARGUMENT))
    {
        return 0;
    }

    // Calculate days since the Unix epoch (1970-01-01)
    int32_t days_since_epoch = civil_to_days(dt.year, dt.month, dt.day, nullptr, nullptr);

    // Calculate the total seconds since the Unix epoch (1970-01-01)
    int64_t seconds_since_epoch = days_since_epoch * SEC_PER_DAY + dt.hour * SEC_PER_HOUR + dt.minute * SEC_PER_MIN + dt.second;

    // Convert to nanoseconds and add the nanosecond component
    time_t ticks = seconds_to_nanoseconds(seconds_since_epoch) + dt.nanosecond;

    // Adjust for UTC offset
    ticks -= seconds_to_nanoseconds(dt.utc_offset_seconds);

    return ticks;
}

// https://en.wikipedia.org/wiki/ISO_8601
// https://dateutil.readthedocs.io/en/stable/parser.html#dateutil.parser.isoparse

std::string datetime::str() const
{
    return datetime_to_string(*this);
}

// https://en.wikipedia.org/wiki/ISO_8601

std::string datetime_to_string(const datetime& dt)
{
    if (!check_datetime(dt, err::INVALID_ARGUMENT))
    {
        return std::string();
    }

    // YYYY-MM-DDTHH:MM:SSZ
    char buffer[21]{};

    // Format the date and time in ISO 8601 format
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d",
        dt.year,
        static_cast<int32_t>(dt.month),
        dt.day,
        dt.hour,
        dt.minute,
        dt.second
    );

    // If the offset is 0, append 'Z' (for UTC)
    if (dt.utc_offset_seconds == 0)
    {
        buffer[19] = 'Z';
    }

    return buffer;
}

///////////////////////////////////////////////////////////////////////////////
// Windows
///////////////////////////////////////////////////////////////////////////////

// Epoch difference between 1601-01-01 and 1970-01-01 in 100 nanosecond units
#define DELTA_EPOCH_1601_100NS (11644473600ull * 10000000ull)

time_t time_from_windows_file_time(uint32_t low, uint32_t high)
{
    // Convert FILETIME (100-nanosecond intervals since 1601-01-01) to a 64-bit integer
    uint64_t wtime = (static_cast<uint64_t>(high) << 32) | low;
    // Adjust for Unix epoch and convert to nanoseconds (100-nanosecond intervals to nanoseconds)
    uint64_t ticks = (wtime - DELTA_EPOCH_1601_100NS) * 100;
    return static_cast<time_t>(ticks);
}

void time_to_windows_file_time(time_t ticks, uint32_t& low, uint32_t& high)
{
    // Convert timestamp from nanoseconds since Unix epoch to 100-nanosecond intervals since 1601-01-01
    uint64_t wtime = static_cast<uint64_t>((ticks / 100) + DELTA_EPOCH_1601_100NS);

    // Split the 64-bit ticks into two 32-bit parts for FILETIME
    low = static_cast<uint32_t>(wtime);
    high = static_cast<uint32_t>(wtime >> 32);
}

///////////////////////////////////////////////////////////////////////////////
// Ticks
///////////////////////////////////////////////////////////////////////////////

struct ticks_data
{
    uint32_t tick_numerator_ms;
    uint32_t tick_denominator_ms;

    uint32_t tick_numerator_ns;
    uint32_t tick_denominator_ns;

    uint64_t start_ticks;

    ticks_data()
    {
        uint64_t ticks_per_second = get_performance_frequency();
        assert(ticks_per_second <= static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()));

#       define GCD(a, b, gcd) \
        do \
        { \
            while (b) \
            { \
                uint32_t temp = b; \
                b = a % b; \
                a = temp; \
            } \
            gcd = a; \
        } while (VX_NULL_WHILE_LOOP_CONDITION) \

        uint32_t a, b, gcd;

        a = static_cast<uint32_t>(MS_PER_SEC);
        b = static_cast<uint32_t>(ticks_per_second);
        GCD(a, b, gcd);
        tick_numerator_ms = static_cast<uint32_t>(MS_PER_SEC / gcd);
        tick_denominator_ms = static_cast<uint32_t>(ticks_per_second / gcd);
    
        a = static_cast<uint32_t>(NS_PER_SEC);
        b = static_cast<uint32_t>(ticks_per_second);
        GCD(a, b, gcd);
        tick_numerator_ns = static_cast<uint32_t>(NS_PER_SEC / gcd);
        tick_denominator_ns = static_cast<uint32_t>(ticks_per_second / gcd);
    
        start_ticks = get_performance_counter();

#       undef GCD
    }
};

static ticks_data s_ticks_data{};

uint64_t get_ticks()
{
    uint64_t start_ticks = get_performance_counter() - s_ticks_data.start_ticks;
    uint64_t ticks = start_ticks * s_ticks_data.tick_numerator_ms;
    assert(ticks >= start_ticks);
    ticks /= s_ticks_data.tick_denominator_ms;
    return ticks;
}

uint64_t get_ticks_ns()
{
    uint64_t start_ticks = get_performance_counter() - s_ticks_data.start_ticks;
    uint64_t ticks = start_ticks * s_ticks_data.tick_numerator_ns;
    assert(ticks >= start_ticks);
    ticks /= s_ticks_data.tick_denominator_ns;
    return ticks;
}

} // namespace time
} // namespace os
} // namespace vx