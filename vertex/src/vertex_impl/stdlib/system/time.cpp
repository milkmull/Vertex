#include <sstream>
#include <iomanip>

#include "vertex/stdlib/system/time.h"
#include "vertex/stdlib/string/string.h"
#include "vertex/math/math/fn/fn_common.h"

namespace vx {
namespace time {

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
    static const uint8_t DAYS_IN_MONTH[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    if (month < month::JANUARY || month > month::DECEMBER)
    {
        error::set_error(error::error_code::INVALID_ARGUMENT);
        return 0;
    }

    int32_t days = DAYS_IN_MONTH[static_cast<int32_t>(month)];
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
        error::set_error(error::error_code::INVALID_ARGUMENT);
        return 0;
    }
    if (day < 1 || day > days_in_year(year))
    {
        error::set_error(error::error_code::INVALID_ARGUMENT);
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
        error::set_error(error::error_code::INVALID_ARGUMENT);
        return weekday::SUNDAY;
    }
    if (day < 1 || day > days_in_year(year))
    {
        error::set_error(error::error_code::INVALID_ARGUMENT);
        return weekday::SUNDAY;
    }

    weekday dow;
    civil_to_days(year, month, day, &dow, nullptr);

    return dow;
}

///////////////////////////////////////////////////////////////////////////////
// Datetime
///////////////////////////////////////////////////////////////////////////////

time_t datetime_to_time(const datetime& dt)
{
    if (!is_valid_datetime(dt))
    {
        error::set_error(error::error_code::INVALID_ARGUMENT);
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

bool is_valid_datetime(const datetime& dt)
{
    if (dt.month < month::JANUARY || dt.month > month::DECEMBER)
    {
        return false;
    }
    if (dt.hour < 0 || dt.hour > 23)
    {
        return false;
    }
    if (dt.minute < 0 || dt.minute > 59)
    {
        return false;
    }
    if (dt.second < 0 || dt.second > 59) // leap second not supported
    {
        return false;
    }
    if (dt.nanosecond < 0 || dt.nanosecond > NS_PER_SEC)
    {
        return false;
    }

    return true;
}

// https://en.wikipedia.org/wiki/ISO_8601
// https://dateutil.readthedocs.io/en/stable/parser.html#dateutil.parser.isoparse

datetime datetime_from_string(const std::string& s)
{
    datetime dt{};
    if (s.empty())
    {
        error::set_error(error::error_code::INVALID_ARGUMENT);
        return dt;
    }

    std::vector<std::string> date_time;
    const std::string* date = nullptr;
    const std::string* time = nullptr;

    if (s.find('T') != std::string::npos)
    {
        date_time = str::split(s, "T");
        if (date_time.size() != 2)
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return dt;
        }

        date = &date_time[0];
        time = &date_time[1];
    }
    else if (s.find('-') != std::string::npos)
    {
        date = &s;
    }
    else if (s.find(':') != std::string::npos)
    {
        time = &s;
    }
    else
    {
        error::set_error(error::error_code::INVALID_ARGUMENT);
        return dt;
    }

    if (date)
    {
        std::vector<std::string> ymd = str::split(*date, "-");
        if (ymd.size() < 3 || ymd.size() > 4)
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return dt;
        }

        bool negative_year = (ymd.size() == 4) && (ymd[0].empty());
        size_t offset = negative_year ? 1 : 0;

        if (ymd[0 + offset].size() != 4 || !str::to_int(ymd[0 + offset], dt.year))
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return dt;
        }
        if (negative_year)
        {
            dt.year *= -1;
        }
        if (ymd[1 + offset].size() != 2 || !str::to_int(ymd[1 + offset], *reinterpret_cast<int32_t*>(&dt.month)))
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return dt;
        }
        if (ymd[2 + offset].size() != 2 || !str::to_int(ymd[2 + offset], dt.day))
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return dt;
        }

        dt.weekday = get_day_of_week(dt.year, dt.month, dt.day);
    }
    if (time)
    {
        std::vector<std::string> hms = str::split(*time, ":");
        if (hms.size() != 3)
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return dt;
        }

        // Remove UTC if present
        if (!hms[2].empty() && hms[2].back() == 'Z')
        {
            hms[2].pop_back();
        }

        if (hms[0].size() != 2 || !str::to_int(hms[0], dt.hour))
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return dt;
        }
        if (hms[1].size() != 2 || !str::to_int(hms[1], dt.minute))
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return dt;
        }
        if (hms[2].size() != 2 || !str::to_int(hms[2], dt.second))
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return dt;
        }
    }

    return dt;
}

std::string datetime_to_string(const datetime& dt)
{
    if (!is_valid_datetime(dt))
    {
        error::set_error(error::error_code::INVALID_ARGUMENT, "Invalid datetime");
        return "";
    }

    std::ostringstream oss;
    const int32_t imonth = static_cast<int32_t>(dt.month);

    // Format the date and time in ISO 8601 format
    oss << std::setw(4) << std::setfill('0') << dt.year     << '-'
        << std::setw(2) << std::setfill('0') << imonth      << '-'
        << std::setw(2) << std::setfill('0') << dt.day      << 'T'
        << std::setw(2) << std::setfill('0') << dt.hour     << ':'
        << std::setw(2) << std::setfill('0') << dt.minute   << ':'
        << std::setw(2) << std::setfill('0') << dt.second;

    if (dt.utc_offset_seconds == 0)
    {
        oss << 'Z'; // Z indicates UTC
    }

    return oss.str();
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
        uint32_t gcd;

        gcd = math::gcd(static_cast<uint32_t>(MS_PER_SEC), static_cast<uint32_t>(ticks_per_second));
        tick_numerator_ms = static_cast<uint32_t>(MS_PER_SEC / gcd);
        tick_denominator_ms = static_cast<uint32_t>(ticks_per_second / gcd);

        gcd = math::gcd(static_cast<uint32_t>(NS_PER_SEC), static_cast<uint32_t>(ticks_per_second));
        tick_numerator_ns = static_cast<uint32_t>(NS_PER_SEC / gcd);
        tick_denominator_ns = static_cast<uint32_t>(ticks_per_second / gcd);

        start_ticks = get_performance_counter();
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

}
}