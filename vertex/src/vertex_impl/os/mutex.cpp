#include "vertex_impl/os/_platform/platform_mutex.hpp"

namespace vx {
namespace os {

//=============================================================================
// Mutex
//=============================================================================

using mutex_traits = opaque_storage_traits<mutex_impl>;
#define m_mutex_impl (mutex_traits::get(m_storage))

mutex::mutex() noexcept
{
    mutex_traits::construct(m_storage);
    m_mutex_impl.create();
}

mutex::~mutex() noexcept
{
    m_mutex_impl.destroy();
    mutex_traits::destroy(m_storage);
}

bool mutex::lock() noexcept
{
    const auto current_thread = thread_impl::get_current_native_id();
    if (m_mutex_impl.data.thread == current_thread)
    {
        err::set(err::system_error, "deadlock would occur");
        return false;
    }

    if (!m_mutex_impl.lock())
    {
        return false;
    }

    m_mutex_impl.data.thread = current_thread;
    return true;
}

bool mutex::try_lock() noexcept
{
    const auto current_thread = thread_impl::get_current_native_id();
    if (m_mutex_impl.data.thread == current_thread)
    {
        // deadlock would occur
        return false;
    }

    if (!m_mutex_impl.try_lock())
    {
        return false;
    }

    m_mutex_impl.data.thread = current_thread;
    return true;
}

void mutex::unlock() noexcept
{
    const auto current_thread = thread_impl::get_current_native_id();
    if (m_mutex_impl.data.thread != current_thread)
    {
        VX_ASSERT_MESSAGE(false, "mutex not owned by this thread");
        return;
    }

    // The order of operations is important.
    // First reset the owner so another thread doesn't lock
    // the mutex and set the ownership before we reset it,
    // then release the lock semaphore.
    m_mutex_impl.data.thread = thread_impl::get_invalid_native_id();
    m_mutex_impl.unlock();
}

//=============================================================================
// Recursive Mutex Impl
//=============================================================================

#if !defined(VX_HAVE_PTHREAD_MUTEX_RECURSIVE)

struct recursive_mutex_impl
{
    struct data_t
    {
        mutex_impl mutex;
        size_t count = 0;
    };

    data_t data;

    void create() noexcept
    {
        data.mutex.create();
    }

    void destroy() noexcept
    {
        data.mutex.destroy();
    }

    bool lock() noexcept
    {
        const auto current_thread = thread_impl::get_current_native_id();
        if (data.mutex.data.thread != current_thread)
        {
            if (!data.mutex.lock())
            {
                return false;
            }

            data.mutex.data.thread = current_thread;
        }

        ++data.count;
        return true;
    }

    bool try_lock() noexcept
    {
        const auto current_thread = thread_impl::get_current_native_id();
        if (data.mutex.data.thread != current_thread)
        {
            if (!data.mutex.try_lock())
            {
                return false;
            }

            data.mutex.data.thread = current_thread;
        }

        ++data.count;
        return true;
    }

    void unlock() noexcept
    {
        const auto current_thread = thread_impl::get_current_native_id();
        if (data.mutex.data.thread != current_thread)
        {
            VX_ASSERT_MESSAGE(false, "mutex not owned by this thread");
            return;
        }

        --data.count;
        if (data.count == 0)
        {
            // The order of operations is important.
            // First reset the owner so another thread doesn't lock
            // the mutex and set the ownership before we reset it,
            // then release the lock semaphore.
            data.mutex.data.thread = thread_impl::get_invalid_native_id();
            data.mutex.unlock();
        }
    }
};

#endif // !defined(VX_HAVE_PTHREAD_MUTEX_RECURSIVE)

//=============================================================================
// Recursive Mutex
//=============================================================================

using recursive_mutex_traits = opaque_storage_traits<recursive_mutex_impl>;
#define m_recursive_mutex_impl (recursive_mutex_traits::get(m_storage))

recursive_mutex::recursive_mutex() noexcept
{
    recursive_mutex_traits::construct(m_storage);
    m_recursive_mutex_impl.create();
}

recursive_mutex::~recursive_mutex() noexcept
{
    m_recursive_mutex_impl.destroy();
    recursive_mutex_traits::destroy(m_storage);
}

bool recursive_mutex::lock() noexcept
{
    return m_recursive_mutex_impl.lock();
}

bool recursive_mutex::try_lock() noexcept
{
    return m_recursive_mutex_impl.try_lock();
}

void recursive_mutex::unlock() noexcept
{
    m_recursive_mutex_impl.unlock();
}

} // namespace os
} // namespace vx
