#pragma once

#include <pthread.h>

#include "vertex/os/mutex.hpp"
#include "vertex/system/assert.hpp"

namespace vx {
namespace os {

struct mutex_impl
{
    //=============================================================================
    // types
    //=============================================================================

    struct data_t
    {
        pthread_mutex_t mutex;
        typename thread_impl::native_id_t thread = thread_impl::get_invalid_native_id();
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
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        // Use normal mutex (not recursive or error checking)
        pthread_mutex_init(&data.mutex, &attr);
        pthread_mutexattr_destroy(&attr);
    }

    void destroy() noexcept
    {
        pthread_mutex_destroy(&data.mutex);
    }

    bool lock() noexcept
    {
        const int result = pthread_mutex_lock(&data.mutex);
        VX_ASSERT(result == 0);
        return (result == 0);
    }

    bool try_lock() noexcept
    {
        const int result = pthread_mutex_trylock(&data.mutex);
        if (result != 0)
        {
            VX_ASSERT(result == EBUSY);
            return false;
        }

        return true;
    }

    void unlock() noexcept
    {
        pthread_mutex_unlock(&data.mutex);
    }
};

//=============================================================================

#if defined(HAVE_PTHREAD_MUTEX_RECURSIVE)

struct recursive_mutex_impl
{
    struct data_t
    {
        pthread_mutex_t mutex;
    };

    data_t data;

    void create() noexcept
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&data.mutex, &attr);
        pthread_mutexattr_destroy(&attr);
    }

    void destroy() noexcept
    {
        pthread_mutex_destroy(&data.mutex);
    }

    bool lock() noexcept
    {
        const int result = pthread_mutex_lock(&data.mutex);
        VX_ASSERT(result == 0);
        return (result == 0);
    }

    bool try_lock() noexcept
    {
        const int result = pthread_mutex_trylock(&data.mutex);
        if (result != 0)
        {
            VX_ASSERT(result == EBUSY);
            return false;
        }

        return true;
    }

    void unlock() noexcept
    {
        const int result = pthread_mutex_unlock(&data.mutex);
        VX_ASSERT(result == 0);
    }
};

#endif // defined(HAVE_PTHREAD_MUTEX_RECURSIVE)

} // namespace os
} // namespace vx
