#pragma once

#include <pthread.h>

#include "vertex/os/thread.hpp"
#include "vertex_impl/os/__platform/unix/unix_tools.hpp"
#include "vertex/system/assert.hpp"

namespace vx {
namespace os {
namespace __detail {

// https://github.com/microsoft/STL/blob/0d8f517ae3828284fed594741b847db940167a59/stl/src/cthread.cpp

#define assert_is_running() VX_ASSERT_MESSAGE(is_valid(td), "thread not running")

struct thread_impl
{
    static bool is_valid(const thread::impl_data& td) noexcept
    {
        return td.h.is_valid();
    }

    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170

    static bool start(thread::impl_data& td, void* fn, void* arg)
    {
        VX_ASSERT_MESSAGE(!is_valid(td), "thread already started");

        pthread_attr_t type;

        // Set up the attributes
        if (pthread_attr_init(&type) != 0)
        {
            err::set(err::SYSTEM_ERROR, "thread::start(): couldn't initialize pthread attributes");
            return false;
        }
        pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);

        // Create the thread
        if (pthread_create(th.h.get(), &type, fn, arg) != 0)
        {
            err::set(err::SYSTEM_ERROR, "thread::start(): failed to create thread");
            return false;
        }

        td.thread_id = static_cast<unsigned long>(id);
        return td.h.is_valid();
    }

    static void close(thread::impl_data& td) noexcept
    {
        td.h.close();
        td.thread_id = 0;
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
        return true;
    }

    // returns when thread terminates
    static bool join(thread::impl_data& td) noexcept
    {
        assert_is_running();
        pthread_join(*td.h);
        close(td);
        return true;
    }

    static bool detach(thread::impl_data& td) noexcept
    {
        assert_is_running();
        pthread_detatch(*td.h);
        close(td);
        return true;
    }

    static thread::id get_this_thread_id() noexcept
    {
        return static_cast<thread::id>(pthread_self());
    }
};

} // namespace __detail
} // namespace os
} // namespace vx