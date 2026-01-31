#pragma once

#include <process.h> // _beginthreadex, _endthreadex

#include "vertex/os/thread.hpp"
#include "vertex/system/assert.hpp"
#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

namespace vx {
namespace os {

using native_handle_t = os::handle;
using native_id_t = DWORD;

struct thread_impl_data
{
    native_handle_t handle;
    native_id_t id = 0;
};

// https://github.com/microsoft/STL/blob/0d8f517ae3828284fed594741b847db940167a59/stl/src/cthread.cpp

#define assert_is_running() VX_ASSERT_MESSAGE(is_valid(data), "thread not running")

struct thread_impl
{
    static bool is_valid(const thread_impl_data& data) noexcept
    {
        return data.handle.is_valid();
    }

    static constexpr thread::id convert_id(native_id_t id) noexcept
    {
        return static_cast<thread::id>(id);
    }

    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170

    static bool start(thread_impl_data& data, void* fn, void* arg)
    {
        VX_ASSERT_MESSAGE(!is_valid(data), "thread already started");

        unsigned int id = 0;

        // Windows specific thread start using _beginthreadex
        data.handle = reinterpret_cast<HANDLE>(::_beginthreadex(
            NULL,                                               // Security attributes (use default)
            0,                                                  // Stack size (use default)
            reinterpret_cast<unsigned(VX_STDCALL*)(void*)>(fn), // Entry point function
            arg,                                                // Argument to the thread function
            0,                                                  // Creation flags (0 for default behavior)
            &id                                                 // Return thread ID (optional)
            ));

        data.id = static_cast<native_id_t>(id);
        return data.handle.is_valid();
    }

    static void close(thread_impl_data& data) noexcept
    {
        data.handle.close();
        data.id = 0;
    }

    static void exit(thread_impl_data& data, unsigned int exit_code = 0) noexcept
    {
        assert_is_running();
        ::_endthreadex(exit_code);
        close(data);
    }

    static bool is_alive(const thread_impl_data& data) noexcept
    {
        assert_is_running();

        DWORD exit_code = 0;
        if (!::GetExitCodeThread(data.handle.get(), &exit_code))
        {
            windows::error_message("GetExitCodeThread()");
            return false;
        }

        return (exit_code == STILL_ACTIVE);
    }

    // returns when thread terminates
    static bool join(thread_impl_data& data) noexcept
    {
        assert_is_running();

        const DWORD result = ::WaitForSingleObject(data.handle.get(), INFINITE);
        if (result == WAIT_FAILED)
        {
            windows::error_message("WaitForSingleObject(): WAIT_FAILED");
            return false;
        }

        close(data);
        return true;
    }

    static bool detach(thread_impl_data& data) noexcept
    {
        assert_is_running();
        // tell OS to release thread's resources when it terminates
        close(data);
        return true;
    }

    static bool compare_ids(native_id_t lhs, native_id_t rhs) noexcept
    {
        return lhs == rhs;
    }

    static native_id_t get_this_thread_id() noexcept
    {
        return ::GetCurrentThreadId();
    }

    static bool is_this_thread(native_id_t id) noexcept
    {
        return compare_ids(id, get_this_thread_id());
    }
};

} // namespace os
} // namespace vx
