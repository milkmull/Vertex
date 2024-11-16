#include "vertex_impl/system/platform_config.hpp"

#if defined(VX_OS_WINDOWS_TIME)

#include "vertex_impl/_platform/_windows/windows_header.hpp"
#include "vertex/os/time.hpp"

namespace vx {

namespace time {

datetime time_point::to_datetime(bool local) const
{
    uint32_t low, high;
    to_windows_file_time(low, high);
    FILETIME ft{ static_cast<DWORD>(low), static_cast<DWORD>(high) };

    SYSTEMTIME utc_st, local_st;
    SYSTEMTIME* st = NULL;

    datetime dt{};

    if (!FileTimeToSystemTime(&ft, &utc_st))
    {
        windows_error_message("FileTimeToSystemTime()");
        return dt;
    }

    if (local)
    {
        if (!SystemTimeToTzSpecificLocalTime(NULL, &utc_st, &local_st))
        {
            windows_error_message("SystemTimeToTzSpecificLocalTime()");
            return dt;
        }

        FILETIME local_ft{};
        if (!SystemTimeToFileTime(&local_st, &local_ft))
        {
            windows_error_message("SystemTimeToFileTime()");
            return dt;
        }

        time_point local_ticks = time_point::from_windows_file_time(local_ft.dwLowDateTime, local_ft.dwHighDateTime);
        // Convert from nanoseconds to seconds
        dt.utc_offset_seconds = static_cast<int32_t>((local_ticks - *this).as_seconds());
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
    dt.nanosecond = static_cast<int32_t>(as_nanoseconds() % std::nano::den);

    return dt;
}

} // namespace time

namespace os {

VX_API time::time_point system_time()
{
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);
    return time::time_point::from_windows_file_time(ft.dwLowDateTime, ft.dwHighDateTime);
}

VX_API int64_t get_performance_counter()
{
    LARGE_INTEGER counter{};
    if (!QueryPerformanceCounter(&counter))
    {
        windows_error_message("QueryPerformanceCounter()");
    }
    return static_cast<int64_t>(counter.QuadPart);
}

VX_API int64_t get_performance_frequency()
{
    LARGE_INTEGER frequency{};
    if (!QueryPerformanceFrequency(&frequency))
    {
        windows_error_message("QueryPerformanceFrequency()");
    }
    return static_cast<int64_t>(frequency.QuadPart);
}

///////////////////////////////////////////////////////////////////////////////
// Delay
///////////////////////////////////////////////////////////////////////////////

VX_API void sleep(const time::time_point& t)
{
    Sleep(static_cast<DWORD>(t.as_milliseconds()));
}

} // namespace os

} // namespace vx

#endif // VX_OS_WINDOWS_TIME