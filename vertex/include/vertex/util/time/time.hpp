#pragma once

#include <chrono>

#include "vertex/util/type_traits.hpp"
#include "vertex/system/compiler.hpp"

namespace vx {
namespace time {

struct datetime;

///////////////////////////////////////////////////////////////////////////////
// Time
///////////////////////////////////////////////////////////////////////////////

class time_point
{
public:

    constexpr time_point() noexcept : m_count(0) {}
    constexpr explicit time_point(int64_t ns) noexcept : m_count(ns) {}

public:

    template <typename Rep, typename Period>
    constexpr time_point(const std::chrono::duration<Rep, Period>& d) noexcept(
        noexcept(std::chrono::duration_cast<std::chrono::nanoseconds>)
        )
        : m_count(std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()) {}

    template <typename Rep, typename Period>
    constexpr operator std::chrono::duration<Rep, Period>() const noexcept(
        noexcept(std::chrono::duration_cast<std::chrono::duration<Rep, Period>>)
    )
    {
        return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
            std::chrono::nanoseconds(m_count)
        );
    }

public:

    constexpr void zero() noexcept { m_count = 0; }
    constexpr bool is_zero() const noexcept { return m_count == 0; }

    constexpr int64_t as_nanoseconds()       const noexcept { return m_count; }
    constexpr int64_t as_microseconds()      const noexcept { return m_count / 1000ll; }
    constexpr int64_t as_milliseconds()      const noexcept { return m_count / 1000000ll; }
    constexpr int64_t as_seconds()           const noexcept { return m_count / 1000000000ll; }
    constexpr int64_t as_minutes()           const noexcept { return m_count / 60000000000ll; }
    constexpr int64_t as_hours()             const noexcept { return m_count / 3600000000000ll; }

    constexpr double as_float_nanoseconds()  const noexcept { return static_cast<double>(m_count); }
    constexpr double as_float_microseconds() const noexcept { return static_cast<double>(m_count) / 1000.0; }
    constexpr double as_float_milliseconds() const noexcept { return static_cast<double>(m_count) / 1000000.0; }
    constexpr double as_float_seconds()      const noexcept { return static_cast<double>(m_count) / 1000000000.0; }
    constexpr double as_float_minutes()      const noexcept { return static_cast<double>(m_count) / 60000000000.0; }
    constexpr double as_float_hours()        const noexcept { return static_cast<double>(m_count) / 3600000000000.0; }

public:

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

    int64_t m_count; // nanoseconds
};

constexpr time_point nanoseconds(int64_t ns)    noexcept { return time_point(ns); }
constexpr time_point microseconds(int64_t us)   noexcept { return time_point(us * 1000ll); }
constexpr time_point milliseconds(int64_t ms)   noexcept { return time_point(ms * 1000000ll); }
constexpr time_point seconds(int64_t s)         noexcept { return time_point(s * 1000000000ll); }
constexpr time_point minutes(int64_t m)         noexcept { return time_point(m * 60000000000ll); }
constexpr time_point hours(int64_t h)           noexcept { return time_point(h * 3600000000000ll); }

enum : int64_t
{
    milliseconds_per_second = seconds(1).as_milliseconds(),
    microseconds_per_second = seconds(1).as_microseconds(),
    nanoseconds_per_second  = seconds(1).as_nanoseconds()
};

} // namespace time
} // namespace vx