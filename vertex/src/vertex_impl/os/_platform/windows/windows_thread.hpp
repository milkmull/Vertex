#pragma once

#include <process.h> // _beginthreadex, _endthreadex

#include "vertex/os/thread.hpp"
#include "vertex/system/assert.hpp"
#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

namespace vx {
namespace os {

#define assert_is_running() VX_ASSERT_MESSAGE(is_valid(), "thread not running")

struct thread_impl
{
    //=============================================================================
    // types
    //=============================================================================

    using native_id_t = DWORD;

    struct data_t
    {
        os::handle handle;
        native_id_t id = 0;
    };

    //=============================================================================
    // data
    //=============================================================================

    data_t data;

    //=============================================================================
    // id helpers
    //=============================================================================

    static constexpr typename thread::id convert_native_id(native_id_t id) noexcept
    {
        return static_cast<typename thread::id>(id);
    }

    static bool compare_native_id(native_id_t lhs, native_id_t rhs) noexcept
    {
        return lhs == rhs;
    }

    static native_id_t get_current_native_id() noexcept
    {
        return ::GetCurrentThreadId();
    }

    static constexpr native_id_t get_invalid_native_id() noexcept
    {
        return static_cast<native_id_t>(0);
    }

    //=============================================================================
    // functions
    //=============================================================================

    bool is_valid() const noexcept
    {
        return data.handle.is_valid();
    }

    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170

    bool compare(const thread_impl& other) const noexcept
    {
        assert_is_running();
        return data.id == other.data.id;
    }

    bool start(void* fn, void* arg)
    {
        VX_ASSERT_MESSAGE(!is_valid(), "thread already started");

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

        if (!data.handle.is_valid())
        {
            return false;
        }

        data.id = static_cast<native_id_t>(id);
        return true;
    }

    void close() noexcept
    {
        data.handle.close();
        data.id = get_invalid_native_id();
    }

    VX_MAYBE_UNUSED void exit(unsigned int exit_code = 0) noexcept
    {
        assert_is_running();
        ::_endthreadex(exit_code);
        close();
    }

    // returns when thread terminates
    bool join() noexcept
    {
        assert_is_running();

        const DWORD result = ::WaitForSingleObject(data.handle.get(), INFINITE);
        if (result == WAIT_FAILED)
        {
            windows::error_message("WaitForSingleObject(): WAIT_FAILED");
            return false;
        }

        return true;
    }

    bool detach() noexcept
    {
        assert_is_running();
        // tell OS to release thread's resources when it terminates
        return true;
    }

    thread::id get_id() const noexcept
    {
        assert_is_running();
        return convert_native_id(data.id);
    }

    bool is_current_thread() const noexcept
    {
        return compare_native_id(data.id, get_current_native_id());
    }
};

} // namespace os
} // namespace vx
