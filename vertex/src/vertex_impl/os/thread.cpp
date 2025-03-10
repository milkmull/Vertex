#include "vertex_impl/os/__platform/thread.hpp"

namespace vx {
namespace os {

// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/src/c%2B%2B11/thread.cc

bool thread::start_impl(void* fn, void* arg)
{
    if (thread_impl::is_valid(m_impl_data))
    {
        VX_ERR(err::UNSUPPORTED_OPERATION) << "thread already started";
        return false;
    }

    return thread_impl::start(m_impl_data, fn, arg);
}

VX_API bool thread::is_valid() const noexcept
{
    return thread_impl::is_valid(m_impl_data);
}

VX_API bool thread::is_alive() const noexcept
{
    return is_valid() && thread_impl::is_alive(m_impl_data);
}

VX_API bool thread::join() noexcept
{
    return is_joinable() && thread_impl::join(m_impl_data);
}

VX_API bool thread::detach() noexcept
{
    return is_joinable() && thread_impl::detach(m_impl_data);
}

VX_API thread::id this_thread::get_id()
{
    return __detail::thread_impl::get_this_thread_id();
}

} // namespace os
} // namespace vx