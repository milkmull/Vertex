#include <unistd.h>
#include <time.h>

#include "vertex_impl/os/__platform/unix/unix_tools.hpp"
#include "vertex/os/time.hpp"
#include "vertex/system/assert.hpp"

namespace vx {
namespace os {

// https://github.com/libsdl-org/SDL/blob/d66483dfccfcdc4e03f719e318c7a76f963f22d9/src/timer/unix/SDL_systimer.c
// https://github.com/libsdl-org/SDL/blob/d66483dfccfcdc4e03f719e318c7a76f963f22d9/src/time/unix/SDL_systime.c

time::datetime time_point_to_datetime_impl(const time::time_point& tp, bool local)
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
#endif
    }
    else
    {
#if defined(HAVE_GMTIME_R)
        tm = gmtime_r(&tval, &tm_storage);
#else
        tm = gmtime(&tval);
#endif
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
        dt.nanosecond = tp.as_nanoseconds() % 1000000000;
        dt.utc_offset_seconds = static_cast<int32_t>(tm->tm_gmtoff);
    }
    else
    {
        unix_::error_message(local ? "localtime()" : "gmtime()");
    }

    return dt;
}

///////////////////////////////////////////////////////////////////////////////
// Current Time
///////////////////////////////////////////////////////////////////////////////

time::time_point system_time_impl() noexcept
{
#if defined(HAVE_CLOCK_GETTIME)

    struct timespec tp {};

    if (clock_gettime(CLOCK_REALTIME, &tp) != 0)
    {
        unix_::error_message("clock_gettime()");
        return 0;
    }

    return time::seconds(tp.tv_sec) + time::nanoseconds(tp.tv_nsec);

#else

    struct timeval tv {};

    if (gettimeofday(&tv, NULL) != 0)
    {
        unix_::error_message("gettimeofday()");
        return 0;
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

static void check_monostatic_time()
{
    struct timespec value;
    if (clock_gettime(VX_MONOTONIC_CLOCK, &value) == 0)
    {
        has_monotonic_time = true;
    }

    checked_monotonic_time = true;
}

#endif // HAVE_CLOCK_GETTIME

int64_t get_performance_counter_impl() noexcept
{
    struct timespec now {};

#if defined(HAVE_CLOCK_GETTIME)

    if (!checked_monotonic_time)
    {
        check_monostatic_time();
    }

    if (has_monotonic_time)
    {
        clock_gettime(VX_MONOTONIC_CLOCK, &now);
    }
    else

#endif // HAVE_CLOCK_GETTIME

    {
        if (gettimeofday(&now, NULL) != 0)
        {
            unix_::error_message("gettimeofday()");
            return 0;
        }
    }

    return (time::seconds(now.tv_sec) + time::microseconds(now.tv_usec)).as_microseconds();
}

int64_t get_performance_frequency_impl() noexcept
{
    struct timespec now {};

#if defined(HAVE_CLOCK_GETTIME)

    if (!checked_monotonic_time)
    {
        check_monostatic_time();
    }

    if (has_monotonic_time)
    {
        return 1000000000; // nanoseconds
    }

#endif // HAVE_CLOCK_GETTIME

    return 1000000; // microseconds
}

///////////////////////////////////////////////////////////////////////////////
// Delay
///////////////////////////////////////////////////////////////////////////////

void sleep_impl(const time::time_point& t) noexcept
{
    ::usleep(static_cast<useconds_t>(t.as_microseconds()));
}

} // namespace os

} // namespace vx