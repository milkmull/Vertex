#pragma once

#include <string>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace time {

class time_point;

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

///////////////////////////////////////////////////////////////////////////////
// datetime
///////////////////////////////////////////////////////////////////////////////

// https://en.cppreference.com/w/cpp/chrono/c/tm

struct datetime
{
    int32_t year;                   // year
    month month;                    // month [1-12]
    int32_t day;                    // day [1-31]
    weekday weekday;                // weekday [0-6]
    int32_t hour;                   // hour [0-23]
    int32_t minute;                 // minute [0-59]
    int32_t second;                 // second [0-59] leap seconds not supported
    int32_t nanosecond;             // nanosecond [0-999999999]
    int32_t utc_offset_seconds;     // seconds east of utc

    VX_API bool is_valid() const;
    VX_API time_point to_time_point() const;
    VX_API std::string to_string() const;
};

// https://docs.godotengine.org/en/stable/classes/class_time.html

///////////////////////////////////////////////////////////////////////////////
// Date Helpers
///////////////////////////////////////////////////////////////////////////////

VX_API bool is_leap_year(int32_t year);
VX_API int32_t get_days_in_year(int32_t year);
VX_API int32_t get_days_in_month(int32_t year, month month);
VX_API int32_t get_day_of_year(int32_t year, month month, int32_t day);
VX_API weekday get_day_of_week(int32_t year, month month, int32_t day);

} // namespace time
} // namespace vx