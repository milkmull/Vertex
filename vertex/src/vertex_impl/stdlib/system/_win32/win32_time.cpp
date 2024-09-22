#include <cassert>

#include "vertex/system/platform.hpp"
#include "vertex/stdlib/system/time.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.hpp"

namespace vx {
namespace time {

time_t get_current_time()
{
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);
    return time_from_windows_file_time(ft.dwLowDateTime, ft.dwHighDateTime);
}

datetime time_to_datetime(time_t ticks, bool local)
{
    uint32_t low, high;
    time_to_windows_file_time(ticks, low, high);
    FILETIME ft{ static_cast<DWORD>(low), static_cast<DWORD>(high) };

    SYSTEMTIME utc_st, local_st;
    SYSTEMTIME* st = NULL;

    datetime dt{};

    if (!FileTimeToSystemTime(&ft, &utc_st))
    {
        return dt;
    }

    if (local)
    {
        SystemTimeToTzSpecificLocalTime(NULL, &utc_st, &local_st);

        FILETIME local_ft{};
        SystemTimeToFileTime(&local_st, &local_ft);

        time_t local_ticks = time_from_windows_file_time(local_ft.dwLowDateTime, local_ft.dwHighDateTime);
        dt.utc_offset_seconds = static_cast<int32_t>(nanoseconds_to_seconds(local_ticks - ticks));
        st = &local_st;
    }
    else
    {
        dt.utc_offset_seconds = 0;
        st = &utc_st;
    }

    dt.year = static_cast<int32_t>(st->wYear);
    dt.month = static_cast<month>(st->wMonth);
    dt.day = static_cast<int32_t>(st->wDay);
    dt.weekday = static_cast<weekday>(st->wDayOfWeek);
    dt.hour = static_cast<int32_t>(st->wHour);
    dt.minute = static_cast<int32_t>(st->wMinute);
    dt.second = static_cast<int32_t>(st->wSecond);
    // Extract nanoseconds within the second
    dt.nanosecond = static_cast<int32_t>(ticks % NS_PER_SEC);

    return dt;
}

uint64_t get_performance_counter()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<uint64_t>(counter.QuadPart);
}

uint64_t get_performance_frequency()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return static_cast<uint64_t>(frequency.QuadPart);
}

///////////////////////////////////////////////////////////////////////////////
// Delay
///////////////////////////////////////////////////////////////////////////////

void delay(uint32_t ms)
{
    Sleep(static_cast<DWORD>(ms));
}

void delay_ns(uint32_t ns)
{
    delay(static_cast<uint32_t>(nanoseconds_to_milliseconds(ns)));
}

///////////////////////////////////////////////////////////////////////////////
// Timer
///////////////////////////////////////////////////////////////////////////////

//time_t timer::start()
//{
//    m_running = true;
//    return (m_start_time = get_ticks());
//}
//
//time_t timer::stop()
//{
//    m_running = false;
//    return (m_stop_time = get_ticks());
//}
//
//void timer::reset()
//{
//    m_running = false;
//    m_start_time = m_stop_time = 0;
//}
//
//bool timer::running() const
//{
//    return m_running;
//}
//
//time_t timer::elapsed() const
//{
//    if (m_running)
//    {
//        return get_ticks() - m_start_time;
//    }
//    return m_stop_time - m_start_time;
//}

}
}

#endif