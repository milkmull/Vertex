#include "vertex_impl/os/_platform/platform_mutex.hpp"

namespace vx {
namespace os {

//=============================================================================
// Mutex
//=============================================================================

mutex::mutex() noexcept
    : m_storage(mutex_impl{})
{
    auto& ref = m_storage.get<mutex_impl>();
    ref.create();
}

mutex::~mutex() noexcept
{
    auto& ref = m_storage.get<mutex_impl>();
    ref.destroy();
    m_storage.destroy<mutex_impl>();
}

bool mutex::lock() noexcept
{
    auto& ref = m_storage.get<mutex_impl>();

    const auto current_thread = thread_impl::get_current_native_id();
    if (ref.data.thread == current_thread)
    {
        err::set(err::system_error, "deadlock would occur");
        return false;
    }

    if (!ref.lock())
    {
        return false;
    }

    ref.data.thread = current_thread;
    return true;
}

bool mutex::try_lock() noexcept
{
    auto& ref = m_storage.get<mutex_impl>();

    const auto current_thread = thread_impl::get_current_native_id();
    if (ref.data.thread == current_thread)
    {
        // deadlock would occur
        return false;
    }

    if (!ref.try_lock())
    {
        return false;
    }

    ref.data.thread = current_thread;
    return true;
}

void mutex::unlock() noexcept
{
    auto& ref = m_storage.get<mutex_impl>();

    const auto current_thread = thread_impl::get_current_native_id();
    if (ref.data.thread != current_thread)
    {
        VX_ASSERT_MESSAGE(false, "mutex not owned by this thread");
        return;
    }

    // The order of operations is important.
    // First reset the owner so another thread doesn't lock
    // the mutex and set the ownership before we reset it,
    // then release the lock semaphore.
    ref.data.thread = thread_impl::get_invalid_native_id();
    ref.unlock();
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

recursive_mutex::recursive_mutex() noexcept
    : m_storage(recursive_mutex_impl{})
{
    auto& ref = m_storage.get<recursive_mutex_impl>();
    ref.create();
}

recursive_mutex::~recursive_mutex() noexcept
{
    auto& ref = m_storage.get<recursive_mutex_impl>();
    ref.destroy();
    m_storage.destroy<recursive_mutex_impl>();
}

bool recursive_mutex::lock() noexcept
{
    auto& ref = m_storage.get<recursive_mutex_impl>();
    return ref.lock();
}

bool recursive_mutex::try_lock() noexcept
{
    auto& ref = m_storage.get<recursive_mutex_impl>();
    return ref.try_lock();
}

void recursive_mutex::unlock() noexcept
{
    auto& ref = m_storage.get<recursive_mutex_impl>();
    ref.unlock();
}

} // namespace os
} // namespace vx
