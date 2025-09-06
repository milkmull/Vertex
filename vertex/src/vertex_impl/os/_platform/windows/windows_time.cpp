#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/os/time.hpp"
#include "vertex/os/shared_library.hpp"

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
// Sleep
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/timer/windows/SDL_systimer.c

// CREATE_WAITABLE_TIMER_HIGH_RESOLUTION flag was added in Windows 10 version 1803.
#if !defined(CREATE_WAITABLE_TIMER_HIGH_RESOLUTION)
#   define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

using CreateWaitableTimerExW_t = HANDLE(WINAPI*)(LPSECURITY_ATTRIBUTES lpTimerAttributes, LPCWSTR lpTimerName, DWORD dwFlags, DWORD dwDesiredAccess);
using SetWaitableTimerEx_t = BOOL(WINAPI*)(HANDLE hTimer, const LARGE_INTEGER* lpDueTime, LONG lPeriod, PTIMERAPCROUTINE pfnCompletionRoutine, LPVOID lpArgToCompletionRoutine, PREASON_CONTEXT WakeContext, ULONG TolerableDelay);

struct waitable_timer_tools
{
    os::shared_library kernel32;
    bool initialized = false;

    CreateWaitableTimerExW_t CreateWaitableTimerExW = nullptr;
    SetWaitableTimerEx_t SetWaitableTimerEx = nullptr;

    bool available() const noexcept { return CreateWaitableTimerExW && SetWaitableTimerEx; }
};

static waitable_timer_tools s_wtt;

static HANDLE get_waitable_timer()
{
    static thread_local os::handle stl_handle = NULL;

    if (!s_wtt.initialized)
    {
        if (s_wtt.kernel32.load("kernel32.dll"))
        {
            s_wtt.CreateWaitableTimerExW = s_wtt.kernel32.get<CreateWaitableTimerExW_t>("CreateWaitableTimerExW_t");
            s_wtt.SetWaitableTimerEx = s_wtt.kernel32.get<SetWaitableTimerEx_t>("SetWaitableTimerEx");

            if (!s_wtt.available())
            {
                s_wtt.kernel32.free();
            }
        }

        s_wtt.initialized = true;
    }

    if (s_wtt.available() && !stl_handle.is_valid())
    {
        stl_handle = s_wtt.CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    }

    return stl_handle.get();
}

static HANDLE get_waitable_event()
{
    static thread_local os::handle stl_handle = NULL;

    if (!stl_handle.is_valid())
    {
        stl_handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    return stl_handle.get();
}

// Sleep implementation that chooses the most precise available method:
//
// 1. Prefer waitable timers (high precision on Win10+).
// 2. If unavailable, fall back to WaitForSingleObjectEx on an event.
// 3. Last resort, use ::Sleep (least precise, but always available).

void sleep_impl(const time::time_point& t) noexcept
{
    const HANDLE timer = get_waitable_timer();
    if (timer)
    {
        LARGE_INTEGER due_time{};

        // Convert to negative 100-ns intervals (relative time).
        // Clamp to avoid overflow if input duration is very large.
        int64_t ns = t.as_nanoseconds();
        if (ns / 100 > std::numeric_limits<LONGLONG>::max())
        {
            ns = std::numeric_limits<LONGLONG>::max() * 100;
        }
        due_time.QuadPart = -(static_cast<LONGLONG>(ns / 100));

        if (s_wtt.SetWaitableTimerEx(timer, &due_time, 0, NULL, NULL, NULL, 0))
        {
            ::WaitForSingleObject(timer, INFINITE);
        }

        return;
    }

    int64_t ms = t.as_milliseconds();
    if (ms > std::numeric_limits<DWORD>::max())
    {
        ms = std::numeric_limits<DWORD>::max();
    }

    const DWORD delay = static_cast<DWORD>(ms);

    const HANDLE event = get_waitable_event();
    if (event)
    {
        ::WaitForSingleObjectEx(event, delay, FALSE);
        return;
    }

    ::Sleep(delay);
}

} // namespace os

} // namespace vx