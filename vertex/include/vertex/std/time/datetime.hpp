#pragma once

#include "vertex/std/time/time.hpp"

namespace vx {
namespace time {

//=============================================================================
// helpers
//=============================================================================

namespace _datetime_priv {

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
    const uint32_t yoe = static_cast<uint32_t>(year - era * 400); // [0, 399]
    const uint32_t doy = day_of_year(month, day);                 // [0, 365]
    const uint32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;   // [0, 146096]
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
    const uint32_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100); // [0, 365]
    const uint32_t mp = (5 * doy + 2) / 153;                      // [0, 11]
    const uint32_t d = doy - (153 * mp + 2) / 5 + 1;              // [1, 31]
    const uint32_t m = mp < 10 ? mp + 3 : mp - 9;                 // [1, 12]

    return civil{ y + (m <= 2), m, d };
}

// Writes `value` as exactly `Width` zero-padded decimal
// digits and returns the pointer past the last digit
template <typename C, int Width>
constexpr C* write_uint(C* p, uint32_t value) noexcept
{
    for (int i = Width - 1; i >= 0; --i)
    {
        p[i] = static_cast<C>('0' + value % 10);
        value /= 10;
    }

    return p + Width;
}

} // namespace _datetime_priv

//=============================================================================
// datetime enums
//=============================================================================

enum class weekday : int32_t
{
    sunday = 0,
    monday = 1,
    tuesday = 2,
    wednesday = 3,
    thursday = 4,
    friday = 5,
    saturday = 6
};

enum class month : int32_t
{
    january = 1,
    february = 2,
    march = 3,
    april = 4,
    may = 5,
    june = 6,
    july = 7,
    august = 8,
    september = 9,
    october = 10,
    november = 11,
    december = 12
};

// https://docs.godotengine.org/en/stable/classes/class_time.html

//=============================================================================
// Date Helpers
//=============================================================================

/**
 * @brief Checks if a given year is a leap year.
 *
 * @param year The year to check.
 * @return True if the year is a leap year, otherwise false.
 */
constexpr bool is_leap_year(int32_t year) noexcept
{
    // A year is a leap year if it is...
    // divisible by 4...
    // but not divisible by 100...
    // unless it is also divisible by 400
    return !(year % 4) && ((year % 100) || !(year % 400));
}

/**
 * @brief Gets the number of days in a given year.
 *
 * @param year The year to check.
 * @return 366 if the year is a leap year, otherwise 365.
 */
constexpr int32_t get_days_in_year(int32_t year) noexcept
{
    return is_leap_year(year) ? 366 : 365;
}

/**
 * @brief Gets the number of days in a given month of a given year.
 *
 * @param year The year.
 * @param month The month.
 * @return The number of days in the month.
 */
constexpr int32_t get_days_in_month(int32_t year, month month) noexcept
{
    constexpr uint8_t dim[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    const int32_t days = dim[static_cast<int32_t>(month) - 1];
    return (month == month::february) ? (days + is_leap_year(year)) : days;
}

/**
 * @brief Gets the day of the year (1-365 or 1-366 depending on leap year) for a given month and day.
 *
 * @param year The year.
 * @param month The month.
 * @param day The day.
 * @return The day of the year [1-366].
 */
constexpr int32_t get_day_of_year(int32_t year, month month, int32_t day) noexcept
{
    const int32_t doy = static_cast<int32_t>(_datetime_priv::day_of_year(static_cast<int32_t>(month), day));
    // The algorithm considers March 1 to be the first day of the year, so we offset by Jan + Feb days
    return (doy > 305) ? (doy - 306) : (doy + 59 + static_cast<int32_t>(is_leap_year(year)));
}

// https://howardhinnant.github.io/date_algorithms.html#weekday_from_days

/**
 * @brief Gets the day of the week for a given date.
 *
 * @param year The year.
 * @param month The month.
 * @param day The day.
 * @return The weekday enum corresponding to the given date.
 */
constexpr weekday get_day_of_week(int32_t year, month month, int32_t day) noexcept
{
    const int32_t days = _datetime_priv::civil_to_days(year, static_cast<int32_t>(month), day);
    return static_cast<weekday>(days >= -4 ? (days + 4) % 7 : (days + 5) % 7 + 6);
}

//=============================================================================
// datetime
//=============================================================================

// https://en.cppreference.com/w/cpp/chrono/c/tm

/**
 * @struct datetime
 * @brief A structure representing a full date-time, including year, month, day, hour, minute, second, nanoseconds, and UTC offset.
 */
struct datetime
{
    int32_t year = 0;                              // year
    time::month month = time::month::january;      // month [1-12]
    int32_t day = 0;                               // day [1-31]
    time::weekday weekday = time::weekday::sunday; // weekday [0-6]
    int32_t hour = 0;                              // hour [0-23]
    int32_t minute = 0;                            // minute [0-59]
    int32_t second = 0;                            // second [0-59] leap seconds not supported
    int32_t nanosecond = 0;                        // nanosecond [0-999999999]
    int32_t utc_offset_seconds = 0;                // seconds east of utc [-43200-50400]

    enum class field
    {
        none,
        year,
        month,
        day,
        hour,
        minute,
        second,
        nanosecond,
        utc_offset
    };

    constexpr field find_invalid_field() const noexcept
    {
        if (year < 0 || year > 9999)
        {
            return field::year;
        }

        const int32_t m = static_cast<int32_t>(month);
        if (m < 1 || m > 12)
        {
            return field::month;
        }
        if (day < 1 || day > get_days_in_month(year, month))
        {
            return field::day;
        }
        if (hour < 0 || hour > 23)
        {
            return field::hour;
        }
        if (minute < 0 || minute > 59)
        {
            return field::minute;
        }
        if (second < 0 || second > 59) // leap second not supported
        {
            return field::second;
        }
        if (nanosecond < 0 || nanosecond > 999999999)
        {
            return field::nanosecond;
        }
        if (utc_offset_seconds < -43200 || utc_offset_seconds > 50400)
        {
            return field::utc_offset;
        }

        return field::none;
    }

    constexpr bool is_valid() const noexcept
    {
        return find_invalid_field() == field::none;
    }

    // "YYYY-MM-DDTHH:MM:SSZ" = 20 characters
    static constexpr size_t iso8601_utc_size = 20;
    // "YYYY-MM-DDTHH:MM:SS+HH:MM" = 25 characters (no null terminator)
    static constexpr size_t iso8601_max_size = 25;

    // https://en.wikipedia.org/wiki/ISO_8601
    // https://dateutil.readthedocs.io/en/stable/parser.html#dateutil.parser.isoparse

    /**
     * @brief Formats the datetime as ISO 8601 into a caller-provided buffer.
     *
     * Writes "YYYY-MM-DDTHH:MM:SS±HH:MM" or "YYYY-MM-DDTHH:MM:SSZ" (for UTC).
     * No null terminator is written.
     *
     * @param out Destination buffer.
     * @param out_size Capacity of the destination buffer, in characters.
     * @pre is_valid() (asserted in debug builds)
     * @return The number of characters written, or err::size_error if out_size is
     *         less than the required length (20 for UTC, 25 otherwise).
     *         The buffer is left untouched on error.
     */
    template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
    constexpr expected<size_t, error> to_string(C* out, size_t out_size) const noexcept
    {
        VX_ASSERT(is_valid());

        const bool is_utc = (utc_offset_seconds == 0);
        if (out_size < (is_utc ? iso8601_utc_size : iso8601_max_size))
        {
            return unexpected(err::size_error);
        }

        // is_valid() guarantees every field fits its width below
        C* p = out;

        p = _datetime_priv::write_uint<C, 4>(p, static_cast<uint32_t>(year));
        *p++ = static_cast<C>('-');
        p = _datetime_priv::write_uint<C, 2>(p, static_cast<uint32_t>(month));
        *p++ = static_cast<C>('-');
        p = _datetime_priv::write_uint<C, 2>(p, static_cast<uint32_t>(day));
        *p++ = static_cast<C>('T');
        p = _datetime_priv::write_uint<C, 2>(p, static_cast<uint32_t>(hour));
        *p++ = static_cast<C>(':');
        p = _datetime_priv::write_uint<C, 2>(p, static_cast<uint32_t>(minute));
        *p++ = static_cast<C>(':');
        p = _datetime_priv::write_uint<C, 2>(p, static_cast<uint32_t>(second));

        if (is_utc)
        {
            *p++ = static_cast<C>('Z');
        }
        else
        {
            // is_valid() guarantees the offset is within [-12:00, +14:00]
            const int32_t abs_offset = (utc_offset_seconds < 0) ? -utc_offset_seconds : utc_offset_seconds;

            *p++ = static_cast<C>((utc_offset_seconds < 0) ? '-' : '+');
            p = _datetime_priv::write_uint<C, 2>(p, static_cast<uint32_t>(abs_offset / seconds_per_hour));
            *p++ = static_cast<C>(':');
            p = _datetime_priv::write_uint<C, 2>(p, static_cast<uint32_t>((abs_offset % seconds_per_hour) / seconds_per_minute));
        }

        return static_cast<size_t>(p - out);
    }

    template <size_t N, typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
    constexpr expected<size_t, error> to_string(C (&out)[N]) const noexcept
    {
        return to_string(out, N);
    }

    /**
     * @brief Converts the datetime to UTC by adjusting the UTC offset.
     *
     * @return A new datetime object in UTC.
     */
    constexpr datetime to_utc() const noexcept
    {
        const int32_t total_days = _datetime_priv::civil_to_days(year, static_cast<int32_t>(month), day);

        // Convert local time to seconds, adjust by utc offset
        const int64_t total_seconds = (static_cast<int64_t>(total_days) * seconds_per_day +
                                          hour * seconds_per_hour +
                                          minute * seconds_per_minute +
                                          second) -
            utc_offset_seconds;

        // Convert back to UTC date and time
        int32_t new_days = static_cast<int32_t>(total_seconds / seconds_per_day);
        int32_t remaining_seconds = static_cast<int32_t>(total_seconds % seconds_per_day);
        if (remaining_seconds < 0)
        {
            --new_days;
            remaining_seconds += seconds_per_day;
        }

        datetime utc_dt{};

        // Convert days since epoch back to year, month, and day
        const _datetime_priv::civil civil = _datetime_priv::civil_from_days(new_days);
        utc_dt.year = civil.year;
        utc_dt.month = static_cast<time::month>(civil.month);
        utc_dt.day = static_cast<int32_t>(civil.day);
        utc_dt.weekday = get_day_of_week(utc_dt.year, utc_dt.month, utc_dt.day);

        utc_dt.hour = remaining_seconds / seconds_per_hour;
        remaining_seconds %= seconds_per_hour;
        utc_dt.minute = remaining_seconds / seconds_per_minute;
        utc_dt.second = remaining_seconds % seconds_per_minute;
        utc_dt.nanosecond = nanosecond; // nanoseconds remain unchanged

        return utc_dt;
    }

    /**
     * @brief Converts the datetime to a time_point.
     *
     * @return The corresponding time_point representation of the datetime.
     */
    constexpr time_point to_time_point() const noexcept
    {
        // Days since the Unix epoch (1970-01-01)
        const int32_t days_since_epoch = _datetime_priv::civil_to_days(year, static_cast<int32_t>(month), day);

        // Local wall-clock time as seconds since the epoch
        const int64_t local_seconds = (static_cast<int64_t>(days_since_epoch) * seconds_per_day +
            hour * seconds_per_hour +
            minute * seconds_per_minute +
            second);

        // Subtract the UTC offset to get the UTC instant, then add the sub-second part
        return time_point(seconds(local_seconds - utc_offset_seconds) + nanoseconds(nanosecond));
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

//=============================================================================
// String Helpers
//=============================================================================

/**
 * @brief Converts a weekday enum to its corresponding string representation.
 *
 * This function takes a `weekday` enum value and returns the corresponding
 * string representing the name of the weekday (e.g., "Sunday", "Monday", etc.).
 *
 * @param weekday The weekday enum value to convert.
 * @return A string representing the name of the weekday.
 */
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

/**
 * @brief Converts a month enum to its corresponding string representation.
 *
 * This function takes a `month` enum value and returns the corresponding
 * string representing the name of the month (e.g., "January", "February", etc.).
 *
 * @param month The month enum value to convert.
 * @return A string representing the name of the month.
 */
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
