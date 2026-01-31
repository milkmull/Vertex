#pragma once

#include "vertex/os/mutex.hpp"
#include "vertex_impl/os/_platform/windows/windows_thread.hpp"
#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

namespace vx {
namespace os {

struct mutex_impl
{
    //=============================================================================
    // types
    //=============================================================================

    struct data_t
    {
        SRWLOCK lock = SRWLOCK_INIT;
        DWORD thread = 0;
    };

    //=============================================================================
    // data
    //=============================================================================

    data_t data;

    //=============================================================================
    // functions
    //=============================================================================

    void create() noexcept
    {
        ::InitializeSRWLock(&data.lock);
    }

    void destroy() noexcept
    {
        // No cleanup required for SRW Locks.
    }

    bool lock() noexcept
    {
        // Acquire the lock exclusively
        ::AcquireSRWLockExclusive(&data.lock);
        return true;
    }

    bool try_lock() noexcept
    {
        // Try to acquire the lock
        return (::TryAcquireSRWLockExclusive(&data.lock) != 0);
    }

    void unlock() noexcept
    {
        // Leave critical section
        _Analysis_assume_lock_held_(*data.lock);
        ::ReleaseSRWLockExclusive(&data.lock);
    }
};

} // namespace os
} // namespace vx
