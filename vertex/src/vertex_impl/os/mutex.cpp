#include "vertex_impl/os/_platform/platform_mutex.hpp"

namespace vx {
namespace os {

using traits = opaque_storage_traits<mutex_impl>;
#define m_impl (traits::get(m_storage))

//=============================================================================
// Mutex
//=============================================================================

mutex::mutex() noexcept
{
    traits::construct(m_storage);
    m_impl.create();
}

mutex::~mutex() noexcept
{
    m_impl.destroy();
    traits::destroy(m_storage);
}

bool mutex::lock() noexcept
{
    const auto current_thread = thread_impl::get_current_native_id();
    if (m_impl.data.thread == current_thread)
    {
        err::set(err::system_error, "deadlock would occur");
        return false;
    }

    if (!m_impl.lock())
    {
        return false;
    }

    m_impl.data.thread = current_thread;
    return true;
}

bool mutex::try_lock() noexcept
{
    const auto current_thread = thread_impl::get_current_native_id();
    if (m_impl.data.thread == current_thread)
    {
        // deadlock would occur
        return false;
    }

    if (!m_impl.try_lock())
    {
        return false;
    }

    m_impl.data.thread = current_thread;
    return true;
}

void mutex::unlock() noexcept
{
    const auto current_thread = thread_impl::get_current_native_id();
    if (m_impl.data.thread != current_thread)
    {
        VX_ASSERT_MESSAGE(false, "mutex not owned by this thread");
        return;
    }

    m_impl.unlock();
    m_impl.data.thread = thread_impl::get_invalid_native_id();
}

//=============================================================================
// Recursive Mutex
//=============================================================================

//recursive_mutex::recursive_mutex() noexcept
//{
//    m_impl_data.recursive = true;
//    __detail::mutex_impl::create(m_impl_data);
//}
//
//bool recursive_mutex::lock() noexcept
//{
//    return __detail::mutex_impl::lock(m_impl_data);
//}
//
//bool recursive_mutex::try_lock() noexcept
//{
//    return __detail::mutex_impl::try_lock(m_impl_data);
//}
//
//void recursive_mutex::unlock() noexcept
//{
//    __detail::mutex_impl::unlock(m_impl_data);
//}

} // namespace os
} // namespace vx
