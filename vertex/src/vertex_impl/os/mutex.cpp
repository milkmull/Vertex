#include "vertex_impl/os/__platform/mutex.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Mutex
///////////////////////////////////////////////////////////////////////////////

VX_API mutex::mutex() noexcept
{
    m_impl_data.recursive = false;
    __detail::mutex_impl::create(m_impl_data);
}

VX_API bool mutex::lock() noexcept
{
    return __detail::mutex_impl::lock(m_impl_data);
}

VX_API bool mutex::try_lock() noexcept
{
    return __detail::mutex_impl::try_lock(m_impl_data);
}

VX_API void mutex::unlock() noexcept
{
    __detail::mutex_impl::unlock(m_impl_data);
}

///////////////////////////////////////////////////////////////////////////////
// Recursive Mutex
///////////////////////////////////////////////////////////////////////////////

VX_API recursive_mutex::recursive_mutex() noexcept
{
    m_impl_data.recursive = true;
    __detail::mutex_impl::create(m_impl_data);
}

VX_API bool recursive_mutex::lock() noexcept
{
    return __detail::mutex_impl::lock(m_impl_data);
}

VX_API bool recursive_mutex::try_lock() noexcept
{
    return __detail::mutex_impl::try_lock(m_impl_data);
}

VX_API void recursive_mutex::unlock() noexcept
{
    __detail::mutex_impl::unlock(m_impl_data);
}

} // namespace os
} // namespace vx