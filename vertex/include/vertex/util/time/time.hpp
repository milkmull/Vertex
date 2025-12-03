#pragma once

#include "vertex/config/type_traits.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/hash.hpp"

namespace vx {
namespace time {

struct datetime;

///////////////////////////////////////////////////////////////////////////////
// Time
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Represents a time point (in nanoseconds).
 *
 * The time_point class stores a time value in nanoseconds and provides various
 * methods to convert between different time units, such as microseconds,
 * milliseconds, seconds, minutes, and hours.
 */
class time_point
{
public:

    time_point() = default;

    /**
     * @brief Constructs a time_point from nanoseconds.
     *
     * @param ns The number of nanoseconds to initialize the time_point.
     */
    constexpr explicit time_point(int64_t ns) noexcept : m_count(ns) {}

public:

    /**
     * @brief Resets the time_point to zero.
     *
     * This function sets the time_point to represent zero time.
     */
    constexpr void zero() noexcept { m_count = 0; }

    /**
     * @brief Checks if the time_point is zero.
     *
     * @return True if the time_point is zero, false otherwise.
     */
    constexpr bool is_zero() const noexcept { return m_count == 0; }
    constexpr explicit operator bool() const noexcept { return !is_zero(); }

    constexpr bool is_positive() const noexcept { return m_count > 0; }
    constexpr bool is_negative() const noexcept { return m_count < 0; }

    /**
     * @brief Retrieves the time in nanoseconds.
     *
     * @return The time in nanoseconds.
     */
    constexpr int64_t as_nanoseconds() const noexcept { return m_count; }

    /**
     * @brief Retrieves the time in microseconds.
     *
     * @return The time in microseconds.
     */
    constexpr int64_t as_microseconds() const noexcept { return m_count / 1000ll; }

    /**
     * @brief Retrieves the time in milliseconds.
     *
     * @return The time in milliseconds.
     */
    constexpr int64_t as_milliseconds() const noexcept { return m_count / 1000000ll; }

    /**
     * @brief Retrieves the time in seconds.
     *
     * @return The time in seconds.
     */
    constexpr int64_t as_seconds() const noexcept { return m_count / 1000000000ll; }

    /**
     * @brief Retrieves the time in minutes.
     *
     * @return The time in minutes.
     */
    constexpr int64_t as_minutes() const noexcept { return m_count / 60000000000ll; }

    /**
     * @brief Retrieves the time in hours.
     *
     * @return The time in hours.
     */
    constexpr int64_t as_hours() const noexcept { return m_count / 3600000000000ll; }

    /**
     * @brief Retrieves the time as a floating point value in nanoseconds.
     *
     * @return The time in nanoseconds as a double.
     */
    constexpr double as_float_nanoseconds() const noexcept { return static_cast<double>(m_count); }

    /**
     * @brief Retrieves the time as a floating point value in microseconds.
     *
     * @return The time in microseconds as a double.
     */
    constexpr double as_float_microseconds() const noexcept { return static_cast<double>(m_count) / 1000.0; }

    /**
     * @brief Retrieves the time as a floating point value in milliseconds.
     *
     * @return The time in milliseconds as a double.
     */
    constexpr double as_float_milliseconds() const noexcept { return static_cast<double>(m_count) / 1000000.0; }

    /**
     * @brief Retrieves the time as a floating point value in seconds.
     *
     * @return The time in seconds as a double.
     */
    constexpr double as_float_seconds() const noexcept { return static_cast<double>(m_count) / 1000000000.0; }

    /**
     * @brief Retrieves the time as a floating point value in minutes.
     *
     * @return The time in minutes as a double.
     */
    constexpr double as_float_minutes() const noexcept { return static_cast<double>(m_count) / 60000000000.0; }

    /**
     * @brief Retrieves the time as a floating point value in hours.
     *
     * @return The time in hours as a double.
     */
    constexpr double as_float_hours() const noexcept { return static_cast<double>(m_count) / 3600000000000.0; }


public:

    /**
     * @brief Converts the time_point to a datetime object.
     *
     * @param local If true, converts to local time; otherwise, converts to UTC.
     * @return A datetime object representing the time_point.
     */
    VX_API datetime to_datetime(bool local = true) const;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend constexpr bool operator==(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_count == rhs.m_count;
    }

    friend constexpr bool operator!=(const time_point& lhs, const time_point& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_count < rhs.m_count;
    }

    friend constexpr bool operator>(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_count > rhs.m_count;
    }

    friend constexpr bool operator<=(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_count <= rhs.m_count;
    }

    friend constexpr bool operator>=(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_count >= rhs.m_count;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    constexpr time_point operator+() const noexcept
    {
        return time_point(+m_count);
    }

    constexpr time_point operator-() const noexcept
    {
        return time_point(-m_count);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    friend constexpr time_point operator+(const time_point& lhs, const time_point& rhs) noexcept
    {
        return time_point(lhs.m_count + rhs.m_count);
    }

    // subtraction (-)

    friend constexpr time_point operator-(const time_point& lhs, const time_point& rhs) noexcept
    {
        return time_point(lhs.m_count - rhs.m_count);
    }

    // multiplication (*)

    friend constexpr time_point operator*(const time_point& lhs, const time_point& rhs) noexcept
    {
        return time_point(lhs.m_count * rhs.m_count);
    }

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    friend constexpr time_point operator*(const time_point& lhs, T rhs) noexcept
    {
        return time_point(lhs.m_count * rhs);
    }

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    friend constexpr time_point operator*(T lhs, const time_point& rhs) noexcept
    {
        return time_point(lhs * rhs.m_count);
    }

    // division (/)

    friend constexpr time_point operator/(const time_point& lhs, const time_point& rhs) noexcept
    {
        return time_point(lhs.m_count / rhs.m_count);
    }

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    friend constexpr time_point operator/(const time_point& lhs, T rhs) noexcept
    {
        return time_point(lhs.m_count / rhs);
    }

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    friend constexpr time_point operator/(T lhs, const time_point& rhs) noexcept
    {
        return time_point(lhs / rhs.m_count);
    }

    // division (%)

    friend constexpr time_point operator%(const time_point& lhs, const time_point& rhs) noexcept
    {
        return time_point(lhs.m_count % rhs.m_count);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    constexpr time_point& operator+=(const time_point& rhs) noexcept
    {
        m_count += rhs.m_count;
        return *this;
    }

    // subtraction (-=)

    constexpr time_point& operator-=(const time_point& rhs) noexcept
    {
        m_count -= rhs.m_count;
        return *this;
    }

    // multiplication (*=)

    constexpr time_point& operator*=(const time_point& rhs) noexcept
    {
        m_count *= rhs.m_count;
        return *this;
    }

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    constexpr time_point& operator*=(T rhs) noexcept
    {
        m_count *= rhs;
        return *this;
    }

    // division (/=)

    constexpr time_point& operator/=(const time_point& rhs) noexcept
    {
        m_count /= rhs.m_count;
        return *this;
    }

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    constexpr time_point& operator/=(T rhs) noexcept
    {
        m_count /= rhs;
        return *this;
    }

    // modulo (%=)

    constexpr time_point& operator%=(const time_point& rhs) noexcept
    {
        m_count %= rhs.m_count;
        return *this;
    }

private:

    int64_t m_count = 0; // nanoseconds
};

// Factory functions for creating time_point from specific time units

constexpr time_point zero() noexcept { return time_point(0); }

#ifdef min
#   undef min
#endif

#ifdef max
#   undef max
#endif

constexpr time_point max() noexcept { return time_point(INT64_MAX); }
constexpr time_point min() noexcept { return time_point(INT64_MIN); }

/**
 * @brief Creates a time_point from nanoseconds.
 *
 * @param ns The number of nanoseconds.
 * @return A time_point representing the given nanoseconds.
 */
constexpr time_point nanoseconds(int64_t ns) noexcept { return time_point(ns); }

/**
 * @brief Creates a time_point from microseconds.
 *
 * @param us The number of microseconds.
 * @return A time_point representing the given microseconds.
 */
constexpr time_point microseconds(int64_t us) noexcept { return time_point(us * 1000ll); }

/**
 * @brief Creates a time_point from milliseconds.
 *
 * @param ms The number of milliseconds.
 * @return A time_point representing the given milliseconds.
 */
constexpr time_point milliseconds(int64_t ms) noexcept { return time_point(ms * 1000000ll); }

/**
 * @brief Creates a time_point from seconds.
 *
 * @param s The number of seconds.
 * @return A time_point representing the given seconds.
 */
constexpr time_point seconds(int64_t s) noexcept { return time_point(s * 1000000000ll); }

/**
 * @brief Creates a time_point from minutes.
 *
 * @param m The number of minutes.
 * @return A time_point representing the given minutes.
 */
constexpr time_point minutes(int64_t m) noexcept { return time_point(m * 60000000000ll); }

/**
 * @brief Creates a time_point from hours.
 *
 * @param h The number of hours.
 * @return A time_point representing the given hours.
 */
constexpr time_point hours(int64_t h) noexcept { return time_point(h * 3600000000000ll); }

enum : int64_t
{
    milliseconds_per_second = seconds(1).as_milliseconds(),
    microseconds_per_second = seconds(1).as_microseconds(),
    nanoseconds_per_second  = seconds(1).as_nanoseconds(),
    nanoseconds_per_millisecond  = milliseconds(1).as_nanoseconds(),

    seconds_per_minute = minutes(1).as_seconds(),
    seconds_per_hour = hours(1).as_seconds(),
    seconds_per_day = seconds_per_hour * 24
};

} // namespace time
} // namespace vx

///////////////////////////////////////////////////////////////////////////////
// hash
///////////////////////////////////////////////////////////////////////////////

namespace std {

template <>
struct hash<vx::time::time_point>
{
    size_t operator()(const vx::time::time_point& t) const noexcept
    {
        using ns_type = decltype(t.as_nanoseconds());
        return std::hash<ns_type>{}(t.as_nanoseconds());
    }
};

} // namespace std