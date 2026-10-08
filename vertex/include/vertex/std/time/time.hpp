#pragma once

#include "vertex/config/hash.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/error.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/crypto/fnv1a.hpp"

namespace vx {
namespace time {

struct datetime;

//=============================================================================
// duration
//=============================================================================

/**
 * @brief Represents a span of time (stored in nanoseconds).
 *
 * The duration class stores a signed time span in nanoseconds and provides
 * methods to convert between different time units, such as microseconds,
 * milliseconds, seconds, minutes, and hours.
 */
class duration
{
public:

    constexpr duration() noexcept = default;

    /**
     * @brief Constructs a duration from nanoseconds.
     *
     * @param ns The number of nanoseconds.
     */
    constexpr explicit duration(int64_t ns) noexcept : m_count(ns)
    {}

    /**
     * @brief Resets the duration to zero.
     */
    constexpr void zero() noexcept
    {
        m_count = 0;
    }

    /**
     * @brief Checks if the duration is zero.
     */
    constexpr bool is_zero() const noexcept
    {
        return m_count == 0;
    }
    constexpr explicit operator bool() const noexcept
    {
        return !is_zero();
    }

    constexpr bool is_positive() const noexcept
    {
        return m_count > 0;
    }
    constexpr bool is_negative() const noexcept
    {
        return m_count < 0;
    }

    //=============================================================================
    // integer conversions (truncating)
    //=============================================================================

    constexpr int64_t as_nanoseconds() const noexcept
    {
        return m_count;
    }

    constexpr int64_t as_microseconds() const noexcept
    {
        return m_count / 1000ll;
    }

    constexpr int64_t as_milliseconds() const noexcept
    {
        return m_count / 1000000ll;
    }

    constexpr int64_t as_seconds() const noexcept
    {
        return m_count / 1000000000ll;
    }

    constexpr int64_t as_minutes() const noexcept
    {
        return m_count / 60000000000ll;
    }

    constexpr int64_t as_hours() const noexcept
    {
        return m_count / 3600000000000ll;
    }

    //=============================================================================
    // floating point conversions
    //=============================================================================

    constexpr double as_float_nanoseconds() const noexcept
    {
        return static_cast<double>(m_count);
    }

    constexpr double as_float_microseconds() const noexcept
    {
        return static_cast<double>(m_count) / 1000.0;
    }

    constexpr double as_float_milliseconds() const noexcept
    {
        return static_cast<double>(m_count) / 1000000.0;
    }

    constexpr double as_float_seconds() const noexcept
    {
        return static_cast<double>(m_count) / 1000000000.0;
    }

    constexpr double as_float_minutes() const noexcept
    {
        return static_cast<double>(m_count) / 60000000000.0;
    }

    constexpr double as_float_hours() const noexcept
    {
        return static_cast<double>(m_count) / 3600000000000.0;
    }

    //=============================================================================
    // comparison operators
    //=============================================================================

    friend constexpr bool operator==(const duration& lhs, const duration& rhs) noexcept
    {
        return lhs.m_count == rhs.m_count;
    }

    friend constexpr bool operator!=(const duration& lhs, const duration& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const duration& lhs, const duration& rhs) noexcept
    {
        return lhs.m_count < rhs.m_count;
    }

    friend constexpr bool operator>(const duration& lhs, const duration& rhs) noexcept
    {
        return lhs.m_count > rhs.m_count;
    }

    friend constexpr bool operator<=(const duration& lhs, const duration& rhs) noexcept
    {
        return lhs.m_count <= rhs.m_count;
    }

    friend constexpr bool operator>=(const duration& lhs, const duration& rhs) noexcept
    {
        return lhs.m_count >= rhs.m_count;
    }

    //=============================================================================
    // unary operators
    //=============================================================================

    constexpr duration operator+() const noexcept
    {
        return duration(+m_count);
    }

    constexpr duration operator-() const noexcept
    {
        return duration(-m_count);
    }

    //=============================================================================
    // binary arithmetic operators
    //=============================================================================

    friend constexpr duration operator+(const duration& lhs, const duration& rhs) noexcept
    {
        return duration(lhs.m_count + rhs.m_count);
    }

    friend constexpr duration operator-(const duration& lhs, const duration& rhs) noexcept
    {
        return duration(lhs.m_count - rhs.m_count);
    }

    // Scaling by a scalar. Floating point scalars are computed in floating
    // point and truncated back to whole nanoseconds.

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    friend constexpr duration operator*(const duration& lhs, T rhs) noexcept
    {
        return duration(static_cast<int64_t>(lhs.m_count * rhs));
    }

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    friend constexpr duration operator*(T lhs, const duration& rhs) noexcept
    {
        return duration(static_cast<int64_t>(lhs * rhs.m_count));
    }

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    friend constexpr duration operator/(const duration& lhs, T rhs) noexcept
    {
        return duration(static_cast<int64_t>(lhs.m_count / rhs));
    }

    // duration / duration is a dimensionless ratio, not a duration
    friend constexpr int64_t operator/(const duration& lhs, const duration& rhs) noexcept
    {
        return lhs.m_count / rhs.m_count;
    }

    friend constexpr duration operator%(const duration& lhs, const duration& rhs) noexcept
    {
        return duration(lhs.m_count % rhs.m_count);
    }

    //=============================================================================
    // compound assignment operators
    //=============================================================================

    constexpr duration& operator+=(const duration& rhs) noexcept
    {
        m_count += rhs.m_count;
        return *this;
    }

    constexpr duration& operator-=(const duration& rhs) noexcept
    {
        m_count -= rhs.m_count;
        return *this;
    }

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    constexpr duration& operator*=(T rhs) noexcept
    {
        m_count = static_cast<int64_t>(m_count * rhs);
        return *this;
    }

    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
    constexpr duration& operator/=(T rhs) noexcept
    {
        m_count = static_cast<int64_t>(m_count / rhs);
        return *this;
    }

    constexpr duration& operator%=(const duration& rhs) noexcept
    {
        m_count %= rhs.m_count;
        return *this;
    }

    //=============================================================================
    // swap
    //=============================================================================

    constexpr void swap(duration& other) noexcept
    {
        const int64_t tmp = m_count;
        m_count = other.m_count;
        other.m_count = tmp;
    }

private:

    int64_t m_count = 0; // nanoseconds
};

/**
 * @brief Swaps two duration values.
 */
inline constexpr void swap(duration& lhs, duration& rhs) noexcept
{
    lhs.swap(rhs);
}

//=============================================================================
// special values
//=============================================================================

/**
 * @brief Creates a duration of zero nanoseconds.
 */
inline constexpr duration zero() noexcept
{
    return duration(0);
}

#ifdef min
    #undef min
#endif

#ifdef max
    #undef max
#endif

/**
 * @brief Creates the largest representable duration (INT64_MAX nanoseconds).
 */
inline constexpr duration max() noexcept
{
    return duration(INT64_MAX);
}

/**
 * @brief Creates the smallest representable duration (INT64_MIN nanoseconds).
 */
inline constexpr duration min() noexcept
{
    return duration(INT64_MIN);
}

//=============================================================================
// unit factory functions
//=============================================================================

inline constexpr duration nanoseconds(int64_t ns) noexcept
{
    return duration(ns);
}

inline constexpr duration microseconds(int64_t us) noexcept
{
    return duration(us * 1000ll);
}

inline constexpr duration milliseconds(int64_t ms) noexcept
{
    return duration(ms * 1000000ll);
}

inline constexpr duration seconds(int64_t s) noexcept
{
    return duration(s * 1000000000ll);
}

inline constexpr duration minutes(int64_t m) noexcept
{
    return duration(m * 60000000000ll);
}

inline constexpr duration hours(int64_t h) noexcept
{
    return duration(h * 3600000000000ll);
}

//=============================================================================
// unit constants
//=============================================================================

enum : int64_t
{
    milliseconds_per_second = seconds(1).as_milliseconds(),
    microseconds_per_second = seconds(1).as_microseconds(),
    nanoseconds_per_second = seconds(1).as_nanoseconds(),
    nanoseconds_per_millisecond = milliseconds(1).as_nanoseconds(),

    seconds_per_minute = minutes(1).as_seconds(),
    seconds_per_hour = hours(1).as_seconds(),
    seconds_per_day = seconds_per_hour * 24
};

//=============================================================================
// time_point
//=============================================================================

/**
 * @brief Represents a point in time, stored as a duration since the Unix epoch
 * (1970-01-01T00:00:00Z).
 *
 * Unlike duration, a time_point can only be shifted by a duration, and two
 * time_points can only be subtracted to produce a duration.
 */
class time_point
{
public:

    constexpr time_point() noexcept = default;

    /**
     * @brief Constructs a time_point from a duration since the Unix epoch.
     */
    constexpr explicit time_point(duration since_epoch) noexcept
        : m_since_epoch(since_epoch)
    {}

    /**
     * @brief Gets the duration since the Unix epoch.
     */
    constexpr duration time_since_epoch() const noexcept
    {
        return m_since_epoch;
    }

    /**
     * @brief Converts the time_point to a datetime object.
     *
     * @param local If true, converts to local time; otherwise, converts to UTC.
     * @return A datetime object representing the time_point.
     */
    VX_API expected<datetime, error> to_datetime(bool local = true) const;

    //=============================================================================
    // comparison operators
    //=============================================================================

    friend constexpr bool operator==(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_since_epoch == rhs.m_since_epoch;
    }

    friend constexpr bool operator!=(const time_point& lhs, const time_point& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_since_epoch < rhs.m_since_epoch;
    }

    friend constexpr bool operator>(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_since_epoch > rhs.m_since_epoch;
    }

    friend constexpr bool operator<=(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_since_epoch <= rhs.m_since_epoch;
    }

    friend constexpr bool operator>=(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_since_epoch >= rhs.m_since_epoch;
    }

    //=============================================================================
    // arithmetic operators
    //=============================================================================

    // time_point + duration -> time_point

    friend constexpr time_point operator+(const time_point& lhs, const duration& rhs) noexcept
    {
        return time_point(lhs.m_since_epoch + rhs);
    }

    friend constexpr time_point operator+(const duration& lhs, const time_point& rhs) noexcept
    {
        return time_point(lhs + rhs.m_since_epoch);
    }

    // time_point - duration -> time_point

    friend constexpr time_point operator-(const time_point& lhs, const duration& rhs) noexcept
    {
        return time_point(lhs.m_since_epoch - rhs);
    }

    // time_point - time_point -> duration

    friend constexpr duration operator-(const time_point& lhs, const time_point& rhs) noexcept
    {
        return lhs.m_since_epoch - rhs.m_since_epoch;
    }

    constexpr time_point& operator+=(const duration& rhs) noexcept
    {
        m_since_epoch += rhs;
        return *this;
    }

    constexpr time_point& operator-=(const duration& rhs) noexcept
    {
        m_since_epoch -= rhs;
        return *this;
    }

    //=============================================================================
    // swap
    //=============================================================================

    constexpr void swap(time_point& other) noexcept
    {
        m_since_epoch.swap(other.m_since_epoch);
    }

private:

    duration m_since_epoch;
};

/**
 * @brief Swaps two time_point values.
 */
inline constexpr void swap(time_point& lhs, time_point& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace time
} // namespace vx

//=============================================================================
// hash
//=============================================================================

namespace vx {

template <typename T>
struct hash;

template <>
struct hash<vx::time::duration>
{
    size_t operator()(const vx::time::duration& d) const noexcept
    {
        fnv1a fnv;
        fnv.update(d.as_nanoseconds());
        return fnv.result();
    }
};

template <>
struct hash<vx::time::time_point>
{
    size_t operator()(const vx::time::time_point& t) const noexcept
    {
        fnv1a fnv;
        fnv.update(t.time_since_epoch().as_nanoseconds());
        return fnv.result();
    }
};

} // namespace vx

namespace std {

//=============================================================================
// numeric_limits
//=============================================================================

template <>
struct numeric_limits<vx::time::duration>
{
    static constexpr bool is_specialized = true;

    static constexpr vx::time::duration min() noexcept
    {
        return vx::time::min();
    }
    static constexpr vx::time::duration max() noexcept
    {
        return vx::time::max();
    }
    static constexpr vx::time::duration lowest() noexcept
    {
        return vx::time::min();
    }
};

template <>
struct numeric_limits<vx::time::time_point>
{
    static constexpr bool is_specialized = true;

    static constexpr vx::time::time_point min() noexcept
    {
        return vx::time::time_point(vx::time::min());
    }
    static constexpr vx::time::time_point max() noexcept
    {
        return vx::time::time_point(vx::time::max());
    }
    static constexpr vx::time::time_point lowest() noexcept
    {
        return vx::time::time_point(vx::time::min());
    }
};

} // namespace std
