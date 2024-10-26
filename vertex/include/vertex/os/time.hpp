#pragma once

#include <string>
#include <chrono>

#include "vertex/core/compiler.hpp"

namespace vx {
namespace os {
namespace time {

///////////////////////////////////////////////////////////////////////////////
// Enums
///////////////////////////////////////////////////////////////////////////////

enum class weekday : int32_t
{
    SUNDAY      = 0,
    MONDAY      = 1,
    TUESDAY     = 2,
    WEDNESDAY   = 3,
    THURSDAY    = 4,
    FRIDAY      = 5,
    SATURDAY    = 6
};

VX_API const char* weekday_to_string(weekday d);

enum class month : int32_t
{
    JANUARY     = 1,
    FEBRUARY    = 2,
    MARCH       = 3,
    APRIL       = 4,
    MAY         = 5,
    JUNE        = 6,
    JULY        = 7,
    AUGUST      = 8,
    SEPTEMBER   = 9,
    OCTOBER     = 10,
    NOVEMBER    = 11,
    DECEMBER    = 12
};

VX_API const char* month_to_string(month m);

///////////////////////////////////////////////////////////////////////////////
// Types
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Represents time in nanoseconds since the Unix epoch.
///
/// This type alias defines 'time_t' as a 64-bit integer representing the 
/// number of nanoseconds elapsed since the Unix epoch (January 1, 1970).
///////////////////////////////////////////////////////////////////////////////
using time_t = int64_t;

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

    VX_API std::string str() const;
};

// https://docs.godotengine.org/en/stable/classes/class_time.html

///////////////////////////////////////////////////////////////////////////////
// Conversions
///////////////////////////////////////////////////////////////////////////////

enum : uint64_t
{
    UNIX_EPOCH_YEAR_AD  = 1970,

    SEC_PER_MIN         = 60,
    SEC_PER_HOUR        = (60 * 60),
    SEC_PER_DAY         = (60 * 60 * 24),

    MS_PER_SEC          = 1000,            // Milliseconds per second
    US_PER_SEC          = 1000000,         // Microseconds per second
    NS_PER_SEC          = 1000000000,      // Nanoseconds per second
    NS_PER_MS           = 1000000,         // Nanoseconds per millisecond
    NS_PER_US           = 1000             // Nanoseconds per microsecond
};

inline constexpr uint64_t seconds_to_nanoseconds(uint64_t seconds)
{
    return seconds * NS_PER_SEC;
}

inline constexpr uint64_t nanoseconds_to_seconds(uint64_t nanoseconds)
{
    return nanoseconds / NS_PER_SEC;
}

inline constexpr uint64_t milliseconds_to_nanoseconds(uint64_t milliseconds)
{
    return milliseconds * NS_PER_MS;
}

inline constexpr uint64_t nanoseconds_to_milliseconds(uint64_t nanoseconds)
{
    return nanoseconds / NS_PER_MS;
}

inline constexpr int64_t microseconds_to_nanoseconds(int64_t microseconds)
{
    return microseconds * NS_PER_US;
}

inline constexpr int64_t nanoseconds_to_microseconds(int64_t nanoseconds)
{
    return nanoseconds / NS_PER_US;
}

///////////////////////////////////////////////////////////////////////////////
// Current time
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets the current time in nanoseconds since the Unix epoch.
///
/// This function retrieves the current system time and returns it as 
/// a 'time_t', representing the number of nanoseconds since the Unix epoch 
/// (January 1, 1970). The specific implementation may vary depending on the platform.
///
/// @return The current time as a 'time_t', representing nanoseconds since 
/// the Unix epoch.
///////////////////////////////////////////////////////////////////////////////
VX_API time_t get_current_time();

///////////////////////////////////////////////////////////////////////////////
// Date Helpers
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Determines if a given year is a leap year.
///
/// This function checks whether the specified year is a leap year according 
/// to the Gregorian calendar rules.
///
/// @param year The year to check.
/// 
/// @return 'true' if the year is a leap year, 'false' otherwise.
///////////////////////////////////////////////////////////////////////////////
VX_API bool is_leap_year(int32_t year);

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets the number of days in a given year.
///
/// This function returns the total number of days in the specified year, 
/// taking into account whether it is a leap year.
///
/// @param year The year to check.
/// 
/// @return The number of days in the year (365 or 366).
///////////////////////////////////////////////////////////////////////////////
VX_API int32_t days_in_year(int32_t year);

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets the number of days in a specific month of a given year.
///
/// This function returns the number of days in the specified month and year.
/// It considers leap years when calculating the days in February.
///
/// @param year The year to check.
/// @param month The month to check.
/// 
/// @return The number of days in the month, or 0 if the month is invalid.
///////////////////////////////////////////////////////////////////////////////
VX_API int32_t days_in_month(int32_t year, month month);

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets the day of the year for a specific date.
///
/// This function calculates the day of the year (1 to 365 or 366) for the 
/// given date. It checks for the validity of the month and day.
///
/// @param year The year of the date.
/// @param month The month of the date.
/// @param day The day of the month.
/// 
/// @return The day of the year, or 0 if the date is invalid.
///////////////////////////////////////////////////////////////////////////////
VX_API int32_t get_day_of_year(int32_t year, month month, int32_t day);

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets the day of the week for a specific date.
///
/// This function calculates the day of the week for the given date. It checks 
/// for the validity of the month and day.
///
/// @param year The year of the date.
/// @param month The month of the date.
/// @param day The day of the month.
/// 
/// @return The day of the week as a 'weekday' enum, or 'weekday::SUNDAY' if 
/// the date is invalid.
///////////////////////////////////////////////////////////////////////////////
VX_API weekday get_day_of_week(int32_t year, month month, int32_t day);

///////////////////////////////////////////////////////////////////////////////
// Datetime
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Validates a 'datetime' structure.
///
/// This function checks if the provided 'datetime' structure contains valid 
/// values for the year, month, day, hour, minute, second, and nanosecond fields.
///
/// @param dt The 'datetime' structure to validate.
/// 
/// @return 'true' if the 'datetime' is valid, 'false' otherwise.
///////////////////////////////////////////////////////////////////////////////
VX_API bool is_valid_datetime(const datetime& dt);

///////////////////////////////////////////////////////////////////////////////
/// @brief Converts a timestamp to a 'datetime' structure.
///
/// This function converts a 'time_t' value, representing the number of 
/// nanoseconds since the Unix epoch, to a 'datetime' structure. The 
/// conversion can be done to either UTC or local time.
///
/// @param ticks The timestamp in nanoseconds since the Unix epoch.
/// @param local If 'true', converts to local time; if 'false', converts to UTC.
///
/// @return A 'datetime' structure representing the converted date and time.
///////////////////////////////////////////////////////////////////////////////
VX_API datetime time_to_datetime(time_t ticks, bool local);

///////////////////////////////////////////////////////////////////////////////
/// @brief Converts a 'datetime' structure to a timestamp.
///
/// This function converts a 'datetime' structure to a 'time_t' value, representing 
/// the number of nanoseconds since the Unix epoch. It checks the validity of 
/// the 'datetime' before performing the conversion.
///
/// @param dt The 'datetime' structure to convert.
/// 
/// @return The corresponding timestamp in nanoseconds, or 0 if the 'datetime' 
/// is invalid.
///////////////////////////////////////////////////////////////////////////////
VX_API time_t datetime_to_time(const datetime& dt);

///////////////////////////////////////////////////////////////////////////////
/// @brief Converts a 'datetime' structure to an ISO 8601 formatted string.
///
/// This function converts the provided 'datetime' structure to a string formatted 
/// according to the ISO 8601 standard. If the 'datetime' is invalid, an empty 
/// string is returned.
///
/// @param dt The 'datetime' structure to convert.
/// 
/// @return An ISO 8601 formatted string representing the 'datetime', or an 
/// empty string if the 'datetime' is invalid.
///////////////////////////////////////////////////////////////////////////////
VX_API std::string datetime_to_string(const datetime& dt);

///////////////////////////////////////////////////////////////////////////////
// Windows
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Converts Windows FILETIME to Unix time.
///
/// This function takes a Windows FILETIME, which represents the number of 
/// 100-nanosecond intervals since January 1, 1601, and converts it to Unix 
/// time, which is the number of seconds since January 1, 1970.
///
/// @param low The lower 32 bits of the FILETIME.
/// @param high The higher 32 bits of the FILETIME.
/// 
/// @return The corresponding Unix time as a 'time_t'.
///////////////////////////////////////////////////////////////////////////////
VX_API time_t time_from_windows_file_time(uint32_t low, uint32_t high);

///////////////////////////////////////////////////////////////////////////////
/// @brief Converts Unix time to Windows FILETIME.
///
/// This function converts Unix time, which is the number of seconds since 
/// January 1, 1970, to Windows FILETIME, which represents the number of 
/// 100-nanosecond intervals since January 1, 1601.
///
/// @param ticks The Unix time in nanoseconds.
/// @param[out] low The lower 32 bits of the resulting FILETIME.
/// @param[out] high The higher 32 bits of the resulting FILETIME.
///////////////////////////////////////////////////////////////////////////////
VX_API void time_to_windows_file_time(time_t ticks, uint32_t& low, uint32_t& high);

///////////////////////////////////////////////////////////////////////////////
// Ticks
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Retrieves the current value of a high-resolution performance counter.
///
/// This function returns the current value of a high-resolution performance 
/// counter, which can be used for precise time measurements.
///
/// @return The current value of the performance counter.
///////////////////////////////////////////////////////////////////////////////
VX_API uint64_t get_performance_counter();

///////////////////////////////////////////////////////////////////////////////
/// @brief Retrieves the frequency of the high-resolution performance counter.
///
/// This function returns the frequency of the high-resolution performance counter, 
/// which represents the number of counts per second. This value is used to convert 
/// the counter value to time units.
///
/// @return The frequency of the performance counter in counts per second.
///////////////////////////////////////////////////////////////////////////////
VX_API uint64_t get_performance_frequency();

///////////////////////////////////////////////////////////////////////////////
/// @brief Retrieves the number of ticks in milliseconds since the start time.
///
/// This function calculates the number of ticks in milliseconds that have 
/// elapsed since the 'ticks_data' structure was initialized.
///
/// @return The number of ticks in milliseconds.
///////////////////////////////////////////////////////////////////////////////
VX_API uint64_t get_ticks();

///////////////////////////////////////////////////////////////////////////////
/// @brief Retrieves the number of ticks in nanoseconds since the start time.
///
/// This function calculates the number of ticks in nanoseconds that have 
/// elapsed since the 'ticks_data' structure was initialized.
///
/// @return The number of ticks in nanoseconds.
///////////////////////////////////////////////////////////////////////////////
VX_API uint64_t get_ticks_ns();

///////////////////////////////////////////////////////////////////////////////
// Delay
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Pauses execution for a specified number of milliseconds.
///
/// This function suspends the execution of the current thread for the given 
/// duration, measured in milliseconds.
///
/// @param ms The number of milliseconds to delay.
///////////////////////////////////////////////////////////////////////////////
VX_API void sleep(uint32_t ms);

///////////////////////////////////////////////////////////////////////////////
/// @brief Pauses execution for a specified number of nanoseconds.
///
/// This function converts the given nanoseconds to milliseconds and then 
/// suspends the execution of the current thread for that duration.
///
/// @param ns The number of nanoseconds to delay.
///////////////////////////////////////////////////////////////////////////////
VX_API void sleep_ns(uint32_t ns);

} // namespace time
} // namespace os
} // namespace vx