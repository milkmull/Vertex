#pragma once

#include "vertex/os/mutex.hpp"
#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/system/assert.hpp"

namespace vx {
namespace os {
namespace __detail {

// https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/inc/mutex
// https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/src/mutex.cpp
// https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/inc/__msvc_threads_core.hpp#L35

class mutex_impl
{
public:

#   define get_srw_lock(data) reinterpret_cast<PSRWLOCK>(&data.critical_section.lock)

    static void create(mutex::impl_data& data) noexcept
    {
        InitializeSRWLock(get_srw_lock(data));
    }

    static void destroy(mutex::impl_data& data) noexcept
    {
        // No cleanup required for SRW Locks.
    }

    static bool lock(mutex::impl_data& data) noexcept
    {
        const thread::id current_thread_id = static_cast<thread::id>(GetCurrentThreadId());

        if (data.thread_id == current_thread_id)
        {
            if (!data.recursive)
            {
                err::set(err::SYSTEM_ERROR, "deadlock would occur");
                return false;
            }

            // Recursive lock by the same thread
        }
        else
        {
            // Acquire the lock exclusively
            AcquireSRWLockExclusive(get_srw_lock(data));
            data.thread_id = current_thread_id;
        }

        // Increment lock count
        ++data.count;
        return true;
    }

    static bool try_lock(mutex::impl_data& data) noexcept
    {
        const thread::id current_thread_id = static_cast<thread::id>(GetCurrentThreadId());

        // If the lock is already held by this thread, or the mutex is
        // already locked, return false for non-recursive behavior
        if (data.thread_id == current_thread_id)
        {
            if (!data.recursive)
            {
                // Deadlock would occur
                return false;
            }

            // Increment lock count for recursive mutex
            ++data.count;
            return true;
        }

        // Try to acquire the lock
        if (TryAcquireSRWLockExclusive(get_srw_lock(data)) != 0)
        {
            // Lock acquired, update the thread ID
            data.thread_id = current_thread_id;
            ++data.count;
            return true;
        }

        // Lock acquisition failed
        return false;
    }

    static void unlock(mutex::impl_data& data) noexcept
    {
        const thread::id current_thread_id = static_cast<thread::id>(GetCurrentThreadId());

        if (data.thread_id != current_thread_id)
        {
            VX_ASSERT_MESSAGE(false, "mutex not owned by this thread");
            return;
        }

        // Leave critical section
        if (--data.count == 0)
        {
            data.thread_id = 0;
            auto srw_lock = get_srw_lock(data);
            _Analysis_assume_lock_held_(*srw_lock);
            ReleaseSRWLockExclusive(srw_lock);
        }
    }

#   undef get_srw_lock
};

} // namespace __detail
} // namespace os
} // namespace vx