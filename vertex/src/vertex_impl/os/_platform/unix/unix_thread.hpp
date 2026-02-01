#pragma once

#include <pthread.h>

#include "vertex/os/thread.hpp"
#include "vertex/system/assert.hpp"
#include "vertex_impl/os/_platform/unix/unix_tools.hpp"

namespace vx {
namespace os {

#define assert_is_running() VX_ASSERT_MESSAGE(is_valid(), "thread not running")

struct thread_impl
{
    //=============================================================================
    // types
    //=============================================================================

    using native_id_t = pthread_t;

    struct data_t
    {
        pthread_t handle = 0;
    };

    //=============================================================================
    // data
    //=============================================================================

    data_t data;

    //=============================================================================
    // id helpers
    //=============================================================================

    static constexpr thread_id convert_native_id(native_id_t id) noexcept
    {
        // pthread_t is usually a pointer or unsigned long, so cast to uintptr_t then to thread_id
        return static_cast<thread_id>(reinterpret_cast<uintptr_t>(id));
    }

    static bool compare_native_id(native_id_t lhs, native_id_t rhs) noexcept
    {
        return pthread_equal(lhs, rhs) != 0;
    }

    static native_id_t get_current_native_id() noexcept
    {
        return pthread_self();
    }

    static constexpr native_id_t get_invalid_native_id() noexcept
    {
        // default initialized pthread_t (usually 0 or null)
        return native_id_t{};
    }

    //=============================================================================
    // functions
    //=============================================================================

    bool is_valid() const noexcept
    {
        return !compare_native_id(data.handle, get_invalid_native_id());
    }

    bool compare(const thread_impl& other) const noexcept
    {
        assert_is_running();
        return compare_native_id(data.handle, other.data.handle);
    }

    bool start(thread::thread_fn_t fn, void* arg)
    {
        VX_ASSERT_MESSAGE(!is_valid(), "thread already started");

        pthread_t thread;
        const int result = pthread_create(&thread, nullptr, fn, arg);
        if (result != 0)
        {
            unix_::error_message("pthread_create()");
            return false;
        }

        data.handle = thread;
        return true;
    }

    void close() noexcept
    {
        data.handle = get_invalid_native_id();
    }

    VX_MAYBE_UNUSED void exit(unsigned int exit_code = 0) noexcept
    {
        assert_is_running();
        pthread_exit(reinterpret_cast<void*>(static_cast<uintptr_t>(exit_code)));
        close();
    }

    bool join() noexcept
    {
        assert_is_running();

        const int result = pthread_join(data.handle, nullptr);
        if (result != 0)
        {
            unix_::error_message("pthread_join()");
            return false;
        }

        return true;
    }

    bool detach() noexcept
    {
        assert_is_running();

        const int result = pthread_detach(data.handle);
        if (result != 0)
        {
            unix_::error_message("pthread_detach()");
            return false;
        }

        return true;
    }

    thread_id get_id() const noexcept
    {
        assert_is_running();
        return convert_native_id(data.handle);
    }

    bool is_current_thread() const noexcept
    {
        return compare_native_id(data.handle, get_current_native_id());
    }
};

} // namespace os
} // namespace vx
