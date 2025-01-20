#pragma once

#include <string>
#include <chrono>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace time {

class time_point;
struct datetime;

///////////////////////////////////////////////////////////////////////////////
// Time
///////////////////////////////////////////////////////////////////////////////

class time_point
{
private:

    using nanoseconds_t = std::chrono::nanoseconds;
    using microseconds_t = std::chrono::microseconds;
    using milliseconds_t = std::chrono::milliseconds;
    using seconds_t = std::chrono::seconds;
    using minutes_t = std::chrono::minutes;
    using hours_t = std::chrono::hours;

    using float_nanoseconds_t = std::chrono::duration<double, std::nano>;
    using float_microseconds_t = std::chrono::duration<double, std::micro>;
    using float_milliseconds_t = std::chrono::duration<double, std::milli>;
    using float_seconds_t = std::chrono::duration<double>;
    using float_minutes_t = std::chrono::duration<double, std::ratio<60, 1>>;
    using float_hours_t = std::chrono::duration<double, std::ratio<3600, 1>>;

public:

    constexpr time_point() noexcept : m_count(0) {}
    constexpr explicit time_point(int64_t ns) noexcept : m_count(ns) {}

public:

    template <typename Rep, typename Period>
    constexpr time_point(const std::chrono::duration<Rep, Period>& d)
        : m_count(std::chrono::duration_cast<nanoseconds_t>(d).count()) {}

    template <typename Rep, typename Period>
    constexpr operator std::chrono::duration<Rep, Period>() const
    {
        return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(nanoseconds_t(m_count));
    }

public:

    constexpr void zero() { m_count = 0; }

    constexpr int64_t as_nanoseconds()       const { return m_count; }
    constexpr int64_t as_microseconds()      const { return static_cast<microseconds_t>(*this).count(); }
    constexpr int64_t as_milliseconds()      const { return static_cast<milliseconds_t>(*this).count(); }
    constexpr int64_t as_seconds()           const { return static_cast<seconds_t>(*this).count(); }
    constexpr int64_t as_minutes()           const { return static_cast<minutes_t>(*this).count(); }
    constexpr int64_t as_hours()             const { return static_cast<hours_t>(*this).count(); }

    constexpr double as_float_microseconds() const { return static_cast<float_microseconds_t>(*this).count(); }
    constexpr double as_float_milliseconds() const { return static_cast<float_milliseconds_t>(*this).count(); }
    constexpr double as_float_seconds()      const { return static_cast<float_seconds_t>(*this).count(); }
    constexpr double as_float_minutes()      const { return static_cast<float_minutes_t>(*this).count(); }
    constexpr double as_float_hours()        const { return static_cast<float_hours_t>(*this).count(); }

public:

    VX_API datetime to_datetime(bool local = true) const;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Windows
    ///////////////////////////////////////////////////////////////////////////////

    // Epoch difference between 1601-01-01 and 1970-01-01 in 100 nanosecond units
#   define DELTA_EPOCH_1601_100NS (11644473600ull * 10000000ull)

    static constexpr time_point from_windows_file_time(uint32_t low, uint32_t high) noexcept
    {
        // Convert FILETIME (100-nanosecond intervals since 1601-01-01) to a 64-bit integer
        uint64_t wtime = (static_cast<uint64_t>(high) << 32) | low;
        // Adjust for Unix epoch and convert to nanoseconds (100-nanosecond intervals to nanoseconds)
        uint64_t ticks = (wtime - DELTA_EPOCH_1601_100NS) * 100;
        // Already in nanoseconds
        return time_point{ static_cast<int64_t>(ticks) };
    }

    constexpr void to_windows_file_time(uint32_t& low, uint32_t& high) const noexcept
    {
        // Convert time_point from nanoseconds since Unix epoch to 100-nanosecond intervals since 1601-01-01
        uint64_t wtime = static_cast<uint64_t>((m_count / 100) + DELTA_EPOCH_1601_100NS);

        // Split the 64-bit ticks into two 32-bit parts for FILETIME
        low = static_cast<uint32_t>(wtime);
        high = static_cast<uint32_t>(wtime >> 32);
    }

#   undef DELTA_EPOCH_1601_100NS

public:

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend constexpr bool operator==(const time_point& lhs, const time_point& rhs)
    {
        return lhs.m_count == rhs.m_count;
    }

    friend constexpr bool operator!=(const time_point& lhs, const time_point& rhs)
    {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const time_point& lhs, const time_point& rhs)
    {
        return lhs.m_count < rhs.m_count;
    }

    friend constexpr bool operator>(const time_point& lhs, const time_point& rhs)
    {
        return lhs.m_count > rhs.m_count;
    }

    friend constexpr bool operator<=(const time_point& lhs, const time_point& rhs)
    {
        return lhs.m_count <= rhs.m_count;
    }

    friend constexpr bool operator>=(const time_point& lhs, const time_point& rhs)
    {
        return lhs.m_count >= rhs.m_count;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    constexpr time_point operator+() const
    {
        return time_point(+m_count);
    }

    constexpr time_point operator-() const
    {
        return time_point(-m_count);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    friend constexpr time_point operator+(const time_point& lhs, const time_point& rhs)
    {
        return time_point(lhs.m_count + rhs.m_count);
    }

    // subtraction (-)

    friend constexpr time_point operator-(const time_point& lhs, const time_point& rhs)
    {
        return time_point(lhs.m_count - rhs.m_count);
    }

    // multiplication (*)

    friend constexpr time_point operator*(const time_point& lhs, const time_point& rhs)
    {
        return time_point(lhs.m_count * rhs.m_count);
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    friend constexpr time_point operator*(const time_point& lhs, T rhs)
    {
        return time_point(lhs.m_count * rhs);
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    friend constexpr time_point operator*(T lhs, const time_point& rhs)
    {
        return time_point(lhs * rhs.m_count);
    }

    // division (/)

    friend constexpr time_point operator/(const time_point& lhs, const time_point& rhs)
    {
        return time_point(lhs.m_count / rhs.m_count);
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    friend constexpr time_point operator/(const time_point& lhs, T rhs)
    {
        return time_point(lhs.m_count / rhs);
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    friend constexpr time_point operator/(T lhs, const time_point& rhs)
    {
        return time_point(lhs / rhs.m_count);
    }

    // division (%)

    friend constexpr time_point operator%(const time_point& lhs, const time_point& rhs)
    {
        return time_point(lhs.m_count % rhs.m_count);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    constexpr time_point& operator+=(const time_point& rhs)
    {
        m_count += rhs.m_count;
        return *this;
    }

    // subtraction (-=)

    constexpr time_point& operator-=(const time_point& rhs)
    {
        m_count -= rhs.m_count;
        return *this;
    }

    // multiplication (*=)

    constexpr time_point& operator*=(const time_point& rhs)
    {
        m_count *= rhs.m_count;
        return *this;
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    constexpr time_point& operator*=(T rhs)
    {
        m_count *= rhs;
        return *this;
    }

    // division (/=)

    constexpr time_point& operator/=(const time_point& rhs)
    {
        m_count /= rhs.m_count;
        return *this;
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    constexpr time_point& operator/=(T rhs)
    {
        m_count /= rhs;
        return *this;
    }

    // modulo (%=)

    constexpr time_point& operator%=(const time_point& rhs)
    {
        m_count %= rhs.m_count;
        return *this;
    }

private:

    int64_t m_count; // nanoseconds
};

constexpr time_point nanoseconds(const int64_t& ns) { return time_point(std::chrono::nanoseconds(ns)); }
constexpr time_point microseconds(const int64_t& us) { return time_point(std::chrono::microseconds(us)); }
constexpr time_point milliseconds(const int64_t& ms) { return time_point(std::chrono::milliseconds(ms)); }
constexpr time_point seconds(const int64_t& s) { return time_point(std::chrono::seconds(s)); }
constexpr time_point minutes(const int64_t& m) { return time_point(std::chrono::minutes(m)); }
constexpr time_point hours(const int64_t& h) { return time_point(std::chrono::hours(h)); }

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