#include <exception>

#include "vertex_impl/os/_platform/platform_thread.hpp"

namespace vx {
namespace os {

// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/src/c%2B%2B11/thread.cc

thread::thread() noexcept
{}

thread::~thread()
{
    if (is_joinable())
    {
        std::terminate();
    }
}

thread::thread(thread&& other) noexcept
{
    swap(other);
}

thread& thread::operator=(thread&& other) noexcept
{
    if (is_joinable())
    {
        std::terminate();
    }

    swap(other);
    return *this;
}

void thread::swap(thread& other) noexcept
{
    std::swap(m_impl_data_ptr, other.m_impl_data_ptr);
}

bool thread::start_impl(void* fn, void* arg)
{
    if (is_valid())
    {
        err::set(err::unsupported_operation, "thread already started");
        return false;
    }

    m_impl_data_ptr = make_unique<thread_impl_data>();
    if (!m_impl_data_ptr)
    {
        return false;
    }

    if (!thread_impl::start(*m_impl_data_ptr, fn, arg))
    {
        m_impl_data_ptr.reset();
        return false;
    }

    return true;
}

bool thread::is_valid() const noexcept
{
    return m_impl_data_ptr && thread_impl::is_valid(*m_impl_data_ptr);
}

thread::id thread::get_id() const noexcept
{
    return is_valid() ? thread_impl::convert_id(m_impl_data_ptr->id) : 0;
}

bool thread::join() noexcept
{
    if (!is_joinable())
    {
        return false;
    }

    const auto id = m_impl_data_ptr->id;
    if (thread_impl::is_this_thread(id))
    {
        err::set(err::system_error, "deadlock would occur");
        return false;
    }

    return thread_impl::join(*m_impl_data_ptr);
}

bool thread::detach() noexcept
{
    return is_joinable() && thread_impl::detach(*m_impl_data_ptr);
}

thread::id this_thread::get_id()
{
    return thread_impl::convert_id(thread_impl::get_this_thread_id());
}

} // namespace os
} // namespace vx
