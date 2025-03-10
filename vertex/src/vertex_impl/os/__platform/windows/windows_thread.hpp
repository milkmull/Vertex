#pragma once

#include <process.h>

#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/os/thread.hpp"
#include "vertex/system/assert.hpp"

namespace vx {
namespace os {
namespace __detail {

// https://github.com/microsoft/STL/blob/0d8f517ae3828284fed594741b847db940167a59/stl/src/cthread.cpp

#define assert_is_running() VX_ASSERT_MESSAGE(is_valid(td), "thread not running")

class thread_impl
{
public:

    static bool is_valid(const thread::impl_data& td) noexcept
    {
        return td.handle.is_valid();
    }

    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170

    static bool start(thread::impl_data& td, void* fn, void* arg)
    {
        VX_ASSERT_MESSAGE(!is_valid(td), "thread already started");

        // Windows specific thread start using _beginthreadex
        td.handle = reinterpret_cast<HANDLE>(_beginthreadex(
            NULL,                                               // Security attributes (use default)
            0,                                                  // Stack size (use default)
            reinterpret_cast<unsigned(__stdcall*)(void*)>(fn),  // Entry point function
            arg,                                                // Argument to the thread function
            0,                                                  // Creation flags (0 for default behavior)
            reinterpret_cast<unsigned int*>(&td.id)             // Return thread ID (optional)
        ));

        return td.handle.is_valid();
    }

    static void close(thread::impl_data& td) noexcept
    {
        td.handle.close();
        td.id = 0;
    }

    static void exit(thread::impl_data& td, unsigned int exit_code = 0) noexcept
    {
        assert_is_running();
        _endthreadex(exit_code);
        close(td);
    }

    static bool is_alive(const thread::impl_data& td) noexcept
    {
        assert_is_running();

        DWORD exit_code = 0;
        if (!GetExitCodeThread(td.handle.get(), &exit_code))
        {
            windows::error_message("GetExitCodeThread()");
            return false;
        }

        return (exit_code == STILL_ACTIVE);
    }

    // returns when thread terminates
    static bool join(thread::impl_data& td) noexcept
    {
        assert_is_running();

        const DWORD result = WaitForSingleObject(
            td.handle.get(),
            INFINITE
        );

        if (result == WAIT_FAILED)
        {
            windows::error_message("WaitForSingleObject(): WAIT_FAILED");
            return false;
        }

        close(td);
        return true;
    }

    static bool detach(thread::impl_data& td) noexcept
    {
        assert_is_running();
        // tell OS to release thread's resources when it terminates
        close(td);
        return true;
    }

    static thread::id get_this_thread_id() noexcept
    {
        return static_cast<thread::id>(GetCurrentThreadId());
    }
};

} // namespace __detail
} // namespace os
} // namespace vx
