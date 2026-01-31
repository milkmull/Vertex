#include <exception>

#include "vertex_impl/os/_platform/platform_thread.hpp"

namespace vx {
namespace os {

// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/src/c%2B%2B11/thread.cc

using traits = opaque_storage_traits<thread_impl>;
#define m_impl (traits::get(m_storage))

thread::thread() noexcept
{
    traits::construct(m_storage);
}

thread::~thread()
{
    if (is_joinable())
    {
        std::terminate();
    }

    traits::destroy(m_storage);
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
    std::swap(m_storage, other.m_storage);
}

bool thread::operator==(const thread& other) const noexcept
{
    return m_impl.compare(traits::get(other.m_storage));
}

bool thread::start_impl(void* fn, void* arg)
{
    if (is_valid())
    {
        err::set(err::unsupported_operation, "thread already started");
        return false;
    }

    traits::construct(m_storage);

    if (!m_impl.start(fn, arg))
    {
        traits::destroy(m_storage);
        return false;
    }

    return true;
}

bool thread::is_valid() const noexcept
{
    return m_impl.is_valid();
}

thread::id thread::get_id() const noexcept
{
    return is_valid() ? m_impl.get_native_id() : invalid_id;
}

bool thread::join() noexcept
{
    if (!is_joinable())
    {
        return false;
    }

    if (m_impl.is_current_thread())
    {
        err::set(err::system_error, "deadlock would occur");
        return false;
    }

    return m_impl.join();
}

bool thread::detach() noexcept
{
    return is_joinable() && m_impl.detach();
}

thread::id this_thread::get_id()
{
    return thread_impl::get_current_id();
}

} // namespace os
} // namespace vx
