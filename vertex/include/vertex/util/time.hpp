#pragma once

#include <string>
#include <chrono>
#include <thread>

#include "vertex/core/compiler.hpp"

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

    inline constexpr time_point() : m_count(0) {}
    inline constexpr explicit time_point(int64_t ns) : m_count(ns) {}

public:

    template <typename Rep, typename Period>
    inline constexpr time_point(const std::chrono::duration<Rep, Period>& d)
        : m_count(std::chrono::duration_cast<nanoseconds_t>(d).count()) {}

    template <typename Rep, typename Period>
    inline constexpr operator std::chrono::duration<Rep, Period>() const
    {
        return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(nanoseconds_t(m_count));
    }

public:

    inline constexpr void zero() { m_count = 0; }

    inline constexpr int64_t as_nanoseconds()       const { return m_count; }
    inline constexpr int64_t as_microseconds()      const { return static_cast<microseconds_t>(*this).count(); }
    inline constexpr int64_t as_milliseconds()      const { return static_cast<milliseconds_t>(*this).count(); }
    inline constexpr int64_t as_seconds()           const { return static_cast<seconds_t>(*this).count(); }
    inline constexpr int64_t as_minutes()           const { return static_cast<minutes_t>(*this).count(); }
    inline constexpr int64_t as_hours()             const { return static_cast<hours_t>(*this).count(); }

    inline constexpr double as_float_microseconds() const { return static_cast<float_microseconds_t>(*this).count(); }
    inline constexpr double as_float_milliseconds() const { return static_cast<float_milliseconds_t>(*this).count(); }
    inline constexpr double as_float_seconds()      const { return static_cast<float_seconds_t>(*this).count(); }
    inline constexpr double as_float_minutes()      const { return static_cast<float_minutes_t>(*this).count(); }
    inline constexpr double as_float_hours()        const { return static_cast<float_hours_t>(*this).count(); }

public:

    datetime to_datetime(bool local) const;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Windows
    ///////////////////////////////////////////////////////////////////////////////

    VX_API static time_point from_windows_file_time(uint32_t low, uint32_t high);
    VX_API void to_windows_file_time(uint32_t& low, uint32_t& high) const;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const time_point& lhs, const time_point& rhs)
    {
        return lhs.m_count == rhs.m_count;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const time_point& lhs, const time_point& rhs)
    {
        return !(lhs == rhs);
    }

    friend VX_FORCE_INLINE constexpr bool operator<(const time_point& lhs, const time_point& rhs)
    {
        return lhs.m_count < rhs.m_count;
    }

    friend VX_FORCE_INLINE constexpr bool operator>(const time_point& lhs, const time_point& rhs)
    {
        return lhs.m_count > rhs.m_count;
    }

    friend VX_FORCE_INLINE constexpr bool operator<=(const time_point& lhs, const time_point& rhs)
    {
        return lhs.m_count <= rhs.m_count;
    }

    friend VX_FORCE_INLINE constexpr bool operator>=(const time_point& lhs, const time_point& rhs)
    {
        return lhs.m_count >= rhs.m_count;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr time_point operator+() const
    {
        return time_point(+m_count);
    }

    VX_FORCE_INLINE constexpr time_point operator-() const
    {
        return time_point(-m_count);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    friend VX_FORCE_INLINE constexpr time_point operator+(const time_point& lhs, const time_point& rhs)
    {
        return time_point(lhs.m_count + rhs.m_count);
    }

    // subtraction (-)

    friend VX_FORCE_INLINE constexpr time_point operator-(const time_point& lhs, const time_point& rhs)
    {
        return time_point(lhs.m_count - rhs.m_count);
    }

    // multiplication (*)

    friend VX_FORCE_INLINE constexpr time_point operator*(const time_point& lhs, const time_point& rhs)
    {
        return time_point(lhs.m_count * rhs.m_count);
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr time_point operator*(const time_point& lhs, T rhs)
    {
        return time_point(lhs.m_count * rhs);
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr time_point operator*(T lhs, const time_point& rhs)
    {
        return time_point(lhs * rhs.m_count);
    }

    // division (/)

    friend VX_FORCE_INLINE constexpr time_point operator/(const time_point& lhs, const time_point& rhs)
    {
        return time_point(lhs.m_count / rhs.m_count);
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr time_point operator/(const time_point& lhs, T rhs)
    {
        return time_point(lhs.m_count / rhs);
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr time_point operator/(T lhs, const time_point& rhs)
    {
        return time_point(lhs / rhs.m_count);
    }

    // division (%)

    friend VX_FORCE_INLINE constexpr time_point operator%(const time_point& lhs, const time_point& rhs)
    {
        return time_point(lhs.m_count % rhs.m_count);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    VX_FORCE_INLINE constexpr time_point& operator+=(const time_point& rhs)
    {
        m_count += rhs.m_count;
        return *this;
    }

    // subtraction (-=)

    VX_FORCE_INLINE constexpr time_point& operator-=(const time_point& rhs)
    {
        m_count -= rhs.m_count;
        return *this;
    }

    // multiplication (*=)

    VX_FORCE_INLINE constexpr time_point& operator*=(const time_point& rhs)
    {
        m_count *= rhs.m_count;
        return *this;
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr time_point& operator*=(T rhs)
    {
        m_count *= rhs;
        return *this;
    }

    // division (/=)

    VX_FORCE_INLINE constexpr time_point& operator/=(const time_point& rhs)
    {
        m_count /= rhs.m_count;
        return *this;
    }

    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr time_point& operator/=(T rhs)
    {
        m_count /= rhs;
        return *this;
    }

    // modulo (%=)

    VX_FORCE_INLINE constexpr time_point& operator%=(const time_point& rhs)
    {
        m_count %= rhs.m_count;
        return *this;
    }

private:

    int64_t m_count; // nanoseconds
};

VX_FORCE_INLINE constexpr time_point nanoseconds(const int64_t& ns) { return time_point(std::chrono::nanoseconds(ns)); }
VX_FORCE_INLINE constexpr time_point microseconds(const int64_t& us) { return time_point(std::chrono::microseconds(us)); }
VX_FORCE_INLINE constexpr time_point milliseconds(const int64_t& ms) { return time_point(std::chrono::milliseconds(ms)); }
VX_FORCE_INLINE constexpr time_point seconds(const int64_t& s) { return time_point(std::chrono::seconds(s)); }
VX_FORCE_INLINE constexpr time_point minutes(const int64_t& m) { return time_point(std::chrono::minutes(m)); }
VX_FORCE_INLINE constexpr time_point hours(const int64_t& h) { return time_point(std::chrono::hours(h)); }

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

    time_point to_time_point() const;

    bool is_valid() const;
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