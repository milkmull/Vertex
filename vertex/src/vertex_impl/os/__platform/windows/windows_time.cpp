#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/os/time.hpp"

namespace vx {
namespace os {

time::datetime time_point_to_datetime_impl(const time::time_point& tp, bool local)
{
    FILETIME ft{};
    windows::time_point_to_file_time(tp, ft.dwLowDateTime, ft.dwHighDateTime);

    SYSTEMTIME utc_st, local_st;
    SYSTEMTIME* st = NULL;

    time::datetime dt{};

    if (!::FileTimeToSystemTime(&ft, &utc_st))
    {
        windows::error_message("FileTimeToSystemTime()");
        return dt;
    }

    if (local)
    {
        if (!::SystemTimeToTzSpecificLocalTime(NULL, &utc_st, &local_st))
        {
            windows::error_message("SystemTimeToTzSpecificLocalTime()");
            return dt;
        }

        FILETIME local_ft{};
        if (!::SystemTimeToFileTime(&local_st, &local_ft))
        {
            windows::error_message("SystemTimeToFileTime()");
            return dt;
        }

        time::time_point local_ticks = windows::time_point_from_file_time(local_ft.dwLowDateTime, local_ft.dwHighDateTime);
        // Convert from nanoseconds to seconds
        dt.utc_offset_seconds = static_cast<int32_t>((local_ticks - tp).as_seconds());
        st = &local_st;
    }
    else
    {
        dt.utc_offset_seconds = 0;
        st = &utc_st;
    }

    dt.year = static_cast<int32_t>(st->wYear);
    dt.month = static_cast<time::month>(st->wMonth);
    dt.day = static_cast<int32_t>(st->wDay);
    dt.weekday = static_cast<time::weekday>(st->wDayOfWeek);
    dt.hour = static_cast<int32_t>(st->wHour);
    dt.minute = static_cast<int32_t>(st->wMinute);
    dt.second = static_cast<int32_t>(st->wSecond);
    // Extract nanoseconds within the second
    dt.nanosecond = static_cast<int32_t>(tp.as_nanoseconds() % time::nanoseconds_per_second);

    return dt;
}

///////////////////////////////////////////////////////////////////////////////
// Current Time
///////////////////////////////////////////////////////////////////////////////

time::time_point system_time_impl() noexcept
{
    FILETIME ft{};
    ::GetSystemTimePreciseAsFileTime(&ft);
    return windows::time_point_from_file_time(ft.dwLowDateTime, ft.dwHighDateTime);
}

int64_t get_performance_counter_impl() noexcept
{
    LARGE_INTEGER counter{};
    ::QueryPerformanceCounter(&counter);
    return static_cast<int64_t>(counter.QuadPart);
}

int64_t get_performance_frequency_impl() noexcept
{
    LARGE_INTEGER frequency{};
    ::QueryPerformanceFrequency(&frequency);
    return static_cast<int64_t>(frequency.QuadPart);
}

///////////////////////////////////////////////////////////////////////////////
// Delay
///////////////////////////////////////////////////////////////////////////////

void sleep_impl(const time::time_point& t) noexcept
{
    ::Sleep(static_cast<DWORD>(t.as_milliseconds()));
}

} // namespace os

} // namespace vx