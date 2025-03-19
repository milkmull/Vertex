#pragma once

#if defined(VX_OS_TIME_IMPL)
#   error "time implementation has already been included!"
#endif
#define VX_OS_TIME_IMPL

#include <sys/time.h>
#include <time.h>

#include "vertex/os/time.hpp"
#include "vertex/system/assert.hpp"

namespace vx {
namespace os {

// https://github.com/libsdl-org/SDL/blob/d66483dfccfcdc4e03f719e318c7a76f963f22d9/src/timer/unix/SDL_systimer.c
// https://github.com/libsdl-org/SDL/blob/d66483dfccfcdc4e03f719e318c7a76f963f22d9/src/time/unix/SDL_systime.c

static time::datetime time_point_to_datetime_impl(const time::time_point& tp, bool local)
{
#if defined(HAVE_GMTIME_R) || defined(HAVE_LOCALTIME_R)
    struct tm tm_storage {};
#endif

    struct tm* tm = NULL;
    const time_t tval = static_cast<time_t>(tp.as_seconds());

    if (local)
    {
#if defined(HAVE_LOCALTIME_R)
        tm = localtime_r(&tval, &tm_storage);
#else
        tm = localtime(&tval);
#endif // HAVE_LOCALTIME_R
    }
    else
    {
#if defined(HAVE_GMTIME_R)
        tm = gmtime_r(&tval, &tm_storage);
#else
        tm = gmtime(&tval);
#endif // HAVE_GMTIME_R
    }

    time::datetime dt{};

    if (tm)
    {
        dt.year = tm->tm_year + 1900;
        dt.month = static_cast<time::month>(tm->tm_mon + 1);
        dt.day = tm->tm_mday;
        dt.weekday = static_cast<time::weekday>(tm->tm_wday);
        dt.hour = tm->tm_hour;
        dt.minute = tm->tm_min;
        dt.second = tm->tm_sec;
        dt.nanosecond = static_cast<int32_t>(tp.as_nanoseconds() % time::nanoseconds_per_second);
        dt.utc_offset_seconds = static_cast<int32_t>(tm->tm_gmtoff);
    }
    else
    {
        unix_::error_message(
            local ?
#       if defined(HAVE_LOCALTIME_R)
            "localtime_r()"
#       else
            "localtime()"
#       endif // HAVE_LOCALTIME_R
            :
#       if defined(HAVE_GMTIME_R)
            "gmtime_r()"
#       else
            "gmtime()"
#       endif // HAVE_GMTIME_R
        );
    }

    return dt;
}

///////////////////////////////////////////////////////////////////////////////
// Current Time
///////////////////////////////////////////////////////////////////////////////

static time::time_point system_time_impl() noexcept
{
#if defined(HAVE_CLOCK_GETTIME)

    struct timespec tp {};

    if (clock_gettime(CLOCK_REALTIME, &tp) != 0)
    {
        unix_::error_message("clock_gettime()");
        return {};
    }

    return time::seconds(tp.tv_sec) + time::nanoseconds(tp.tv_nsec);

#else

    struct timeval tv {};

    if (gettimeofday(&tv, NULL) != 0)
    {
        unix_::error_message("gettimeofday()");
        return {};
    }

    //tv.tv_sec = std::min(tv.tv_sec, SDL_NS_TO_SECONDS(SDL_MAX_TIME) - 1);
    return time::seconds(tv.tv_sec) + time::microseconds(tv.tv_usec);

#endif // HAVE_CLOCK_GETTIME
}

///////////////////////////////////////////////////////////////////////////////
// System Clock
///////////////////////////////////////////////////////////////////////////////

// Use CLOCK_MONOTONIC_RAW, if available, which is not subject to adjustment by NTP
#if defined(HAVE_CLOCK_GETTIME)

#    if defined(CLOCK_MONOTONIC_RAW)
#        define VX_MONOTONIC_CLOCK CLOCK_MONOTONIC_RAW
#    else
#        define VX_MONOTONIC_CLOCK CLOCK_MONOTONIC
#    endif

static bool checked_monotonic_time = false;
static bool has_monotonic_time = false;

static void check_monostatic_time() noexcept
{
    struct timespec value;
    if (clock_gettime(VX_MONOTONIC_CLOCK, &value) == 0)
    {
        has_monotonic_time = true;
    }

    checked_monotonic_time = true;
}

#endif // HAVE_CLOCK_GETTIME

static int64_t get_performance_counter_impl() noexcept
{
#if defined(HAVE_CLOCK_GETTIME)

    if (!checked_monotonic_time)
    {
        check_monostatic_time();
    }

    if (has_monotonic_time)
    {
        // return value is nanoseconds
        struct timespec now {};
        clock_gettime(VX_MONOTONIC_CLOCK, &now);
        return time::seconds(now.tv_sec).as_nanoseconds() + now.tv_nsec;
    }

#endif // HAVE_CLOCK_GETTIME

    // return value is microseconds
    struct timeval now {};
    gettimeofday(&now, NULL);
    return time::seconds(now.tv_sec).as_microseconds() + now.tv_usec;
}

static int64_t get_performance_frequency_impl() noexcept
{
#if defined(HAVE_CLOCK_GETTIME)

    if (!checked_monotonic_time)
    {
        check_monostatic_time();
    }

    if (has_monotonic_time)
    {
        return time::nanoseconds_per_second;
    }

#endif // HAVE_CLOCK_GETTIME

    return time::microseconds_per_second;
}

///////////////////////////////////////////////////////////////////////////////
// Delay
///////////////////////////////////////////////////////////////////////////////

static void sleep_impl(const time::time_point& t) noexcept
{
    int was_error = 0;

#if defined(HAVE_NANOSLEEP)

    struct timespec tv, remaining {};

    remaining.tv_sec = static_cast<time_t>(t.as_nanoseconds() / time::nanoseconds_per_second);
    remaining.tv_nsec = static_cast<long>(t.as_nanoseconds() % time::nanoseconds_per_second);

    do
    {
        errno = 0;

        tv.tv_sec = remaining.tv_sec;
        tv.tv_nsec = remaining.tv_nsec;
        was_error = nanosleep(&tv, &remaining);

    } while (was_error && (errno == EINTR));


#else

    struct timeval tv {};
    time::time_point then, now, elapsed;
    time::time_point ns = t;

    then = os::get_ticks();

    do
    {
        errno = 0;

        // Calculate the time interval left (in case of interrupt)
        now = os::get_ticks();
        elapsed = (now - then);
        then = now;

        if (elapsed >= ns)
        {
            break;
        }

        ns -= elapsed;
        tv.tv_sec = (ns.as_nanoseconds() / time::nanoseconds_per_second);
        tv.tv_usec = time::nanoseconds((ns.as_nanoseconds() % time::nanoseconds_per_second)).as_microseconds();

        was_error = select(0, NULL, NULL, NULL, &tv);

    } while (was_error && (errno == EINTR));

#endif
}

} // namespace os
} // namespace vx