#pragma once

#include <string>

#include "vertex/util/time/time.hpp"

namespace vx {
namespace time {

///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

struct civil
{
    int32_t year;
    uint32_t month;
    uint32_t day;
};

constexpr uint32_t day_of_year(int32_t month, int32_t day) noexcept
{
    return static_cast<uint32_t>(
        (153 * (month > 2 ? month - 3 : month + 9) + 2) / 5 + day - 1 // [0, 365]
    ); 
}

// http://howardhinnant.github.io/date_algorithms.html#days_from_civil

constexpr int32_t civil_to_days(int32_t year, int32_t month, int32_t day) noexcept
{
    year -= month <= 2;
    const int32_t era = (year >= 0 ? year : year - 399) / 400;
    const uint32_t yoe = static_cast<uint32_t>(year - era * 400);               // [0, 399]
    const uint32_t doy = day_of_year(month, day);                               // [0, 365]
    const uint32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;                 // [0, 146096]
    const int32_t days = era * 146097 + static_cast<int32_t>(doe) - 719468;

    return days;
}

// https://howardhinnant.github.io/date_algorithms.html#civil_from_days

constexpr civil civil_from_days(int32_t days) noexcept
{
    days += 719468;
    const int32_t era = (days >= 0 ? days : days - 146096) / 146097;
    const uint32_t doe = static_cast<uint32_t>(days - era * 146097);            // [0, 146096]
    const uint32_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365; // [0, 399]
    const int32_t y = static_cast<int32_t>(yoe) + era * 400;
    const uint32_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);               // [0, 365]
    const uint32_t mp = (5 * doy + 2) / 153;                                    // [0, 11]
    const uint32_t d = doy - (153 * mp + 2) / 5 + 1;                            // [1, 31]
    const uint32_t m = mp < 10 ? mp + 3 : mp - 9;                               // [1, 12]

    return civil{ y + (m <= 2), m, d };
}

enum : int64_t
{
    SEC_PER_MIN = 60,
    SEC_PER_HOUR = (SEC_PER_MIN * 60),
    SEC_PER_DAY = (SEC_PER_HOUR * 24),
};

} // namespace __detail

///////////////////////////////////////////////////////////////////////////////
// datetime enums
///////////////////////////////////////////////////////////////////////////////

enum class weekday : int32_t
{
    SUNDAY = 0,
    MONDAY = 1,
    TUESDAY = 2,
    WEDNESDAY = 3,
    THURSDAY = 4,
    FRIDAY = 5,
    SATURDAY = 6
};

enum class month : int32_t
{
    JANUARY = 1,
    FEBRUARY = 2,
    MARCH = 3,
    APRIL = 4,
    MAY = 5,
    JUNE = 6,
    JULY = 7,
    AUGUST = 8,
    SEPTEMBER = 9,
    OCTOBER = 10,
    NOVEMBER = 11,
    DECEMBER = 12
};

// https://docs.godotengine.org/en/stable/classes/class_time.html

///////////////////////////////////////////////////////////////////////////////
// Date Helpers
///////////////////////////////////////////////////////////////////////////////

// A year is a leap year if it is...
// divisible by 4...
// but not divisible by 100...
// unless it is also divisible by 400
constexpr bool is_leap_year(int32_t year) noexcept
{
    return !(year % 4) && ((year % 100) || !(year % 400));
}

constexpr int32_t get_days_in_year(int32_t year) noexcept
{
    return is_leap_year(year) ? 366 : 365;
}

constexpr int32_t get_days_in_month(int32_t year, month month) noexcept
{
    constexpr uint8_t dim[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    const int32_t days = dim[static_cast<int32_t>(month) - 1];
    return (month == month::FEBRUARY) ? (days + is_leap_year(year)) : days;
}

constexpr int32_t get_day_of_year(int32_t year, month month, int32_t day) noexcept
{
    const int32_t doy = static_cast<int32_t>(__detail::day_of_year(static_cast<int32_t>(month), day));
    // The algorithm considers March 1 to be the first day of the year, so we offset by Jan + Feb days
    return (doy > 305) ? (doy - 306) : (doy + 59 + static_cast<int32_t>(is_leap_year(year)));
}

// https://howardhinnant.github.io/date_algorithms.html#weekday_from_days

constexpr weekday get_day_of_week(int32_t year, month month, int32_t day) noexcept
{
    const int32_t days = __detail::civil_to_days(year, static_cast<int32_t>(month), day);
    return static_cast<weekday>(days >= -4 ? (days + 4) % 7 : (days + 5) % 7 + 6);
}

///////////////////////////////////////////////////////////////////////////////
// datetime
///////////////////////////////////////////////////////////////////////////////

// https://en.cppreference.com/w/cpp/chrono/c/tm

struct datetime
{
    int32_t year;                   // year
    time::month month;              // month [1-12]
    int32_t day;                    // day [1-31]
    time::weekday weekday;          // weekday [0-6]
    int32_t hour;                   // hour [0-23]
    int32_t minute;                 // minute [0-59]
    int32_t second;                 // second [0-59] leap seconds not supported
    int32_t nanosecond;             // nanosecond [0-999999999]
    int32_t utc_offset_seconds;     // seconds east of utc [-43200-50400]

    VX_API bool is_valid() const noexcept;
    VX_API std::string to_string() const;

    constexpr datetime to_utc() const noexcept
    {
        const int32_t total_days = __detail::civil_to_days(year, static_cast<int32_t>(month), day);

        // Convert local time to seconds, adjust by utc offset
        const int64_t total_seconds = (
            total_days * __detail::SEC_PER_DAY +
            hour       * __detail::SEC_PER_HOUR +
            minute     * __detail::SEC_PER_MIN +
            second
        ) - utc_offset_seconds;

        // Convert back to UTC date and time
        int32_t new_days = static_cast<int32_t>(total_seconds / __detail::SEC_PER_DAY);
        int32_t remaining_seconds = static_cast<int32_t>(total_seconds % __detail::SEC_PER_DAY);
        if (remaining_seconds < 0)
        {
            --new_days;
            remaining_seconds += __detail::SEC_PER_DAY;
        }

        datetime utc_dt{};

        // Convert days since epoch back to year, month, and day
        const __detail::civil civil = __detail::civil_from_days(new_days);
        utc_dt.year = civil.year;
        utc_dt.month = static_cast<time::month>(civil.month);
        utc_dt.day = static_cast<int32_t>(civil.day);
        utc_dt.weekday = get_day_of_week(utc_dt.year, utc_dt.month, utc_dt.day);

        utc_dt.hour = remaining_seconds / __detail::SEC_PER_HOUR;
        remaining_seconds %= __detail::SEC_PER_HOUR;
        utc_dt.minute = remaining_seconds / __detail::SEC_PER_MIN;
        utc_dt.second = remaining_seconds % __detail::SEC_PER_MIN;
        utc_dt.nanosecond = nanosecond; // nanoseconds remain unchanged

        return utc_dt;
    }

    constexpr time_point to_time_point() const noexcept
    {
        time_point t;

        // Calculate days since the Unix epoch (1970-01-01)
        const int32_t days_since_epoch = __detail::civil_to_days(year, static_cast<int32_t>(month), day);
        // Calculate the total seconds since the Unix epoch (1970-01-01)
        const int64_t seconds_since_epoch = (
            days_since_epoch * __detail::SEC_PER_DAY +
            hour             * __detail::SEC_PER_HOUR +
            minute           * __detail::SEC_PER_MIN +
            second
        );
        // Convert to nanoseconds and add the nanosecond component
        t = seconds(seconds_since_epoch) + nanoseconds(nanosecond);
        // Adjust for UTC offset
        t -= seconds(utc_offset_seconds);

        return t;
    }

    constexpr bool operator==(const datetime& rhs) const noexcept
    {
        return to_time_point() == rhs.to_time_point();
    }

    constexpr bool operator!=(const datetime& rhs) const noexcept
    {
        return !(*this == rhs);
    }
};

///////////////////////////////////////////////////////////////////////////////
// Strng Helpers
///////////////////////////////////////////////////////////////////////////////

constexpr const char* weekday_to_string(weekday weekday)
{
    constexpr const char* conversion_table[] = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
    };

    return conversion_table[static_cast<int>(weekday)];
}

constexpr const char* month_to_string(month month)
{
    constexpr const char* conversion_table[] = {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
    };

    return conversion_table[static_cast<int>(month) - 1];
}

} // namespace time
} // namespace vx