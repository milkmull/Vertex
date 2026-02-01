#include <exception>
#include <thread>

#include "vertex_impl/os/_platform/platform_thread.hpp"

namespace vx {
namespace os {

// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/src/c%2B%2B11/thread.cc

thread::thread() noexcept
    : m_storage(thread_impl{})
{}

thread::~thread()
{
    if (is_joinable())
    {
        std::terminate();
    }

    m_storage.destroy<thread_impl>();
}

thread::thread(thread&& other) noexcept
{
    auto& other_ref = m_storage.get<thread_impl>();
    m_storage.construct<thread_impl>(std::move(other_ref));
    other_ref.close();
}

thread& thread::operator=(thread&& other) noexcept
{
    if (*this != other)
    {
        if (is_joinable())
        {
            std::terminate();
        }

        auto& ref1 = m_storage.get<thread_impl>();
        auto& ref2 = other.m_storage.get<thread_impl>();

        ref1 = std::move(ref2);
        ref2.close();
    }
    return *this;
}

void thread::swap(thread& other) noexcept
{
    auto& ref1 = m_storage.get<thread_impl>();
    auto& ref2 = other.m_storage.get<thread_impl>();
    std::swap(ref1, ref2);
}

bool thread::operator==(const thread& other) const noexcept
{
    if (!is_valid() || !other.is_valid())
    {
        return false;
    }

    auto& ref1 = m_storage.get<thread_impl>();
    auto& ref2 = other.m_storage.get<thread_impl>();
    return ref1.compare(ref2);
}

bool thread::start_impl(thread_fn_t fn, void* arg)
{
    if (is_valid())
    {
        err::set(err::unsupported_operation, "thread already started");
        return false;
    }

    auto& ref = m_storage.get<thread_impl>();
    if (!ref.start(fn, arg))
    {
        ref.close();
        return false;
    }

    return true;
}

bool thread::is_valid() const noexcept
{
    auto& ref = m_storage.get<thread_impl>();
    return ref.is_valid();
}

thread_id thread::get_id() const noexcept
{
    auto& ref = m_storage.get<thread_impl>();
    return is_valid() ? ref.get_id() : invalid_thread_id;
}

bool thread::join() noexcept
{
    if (!is_joinable())
    {
        return false;
    }

    auto& ref = m_storage.get<thread_impl>();
    if (ref.is_current_thread())
    {
        err::set(err::system_error, "deadlock would occur");
        return false;
    }

    if (!ref.join())
    {
        return false;
    }

    ref.close();
    return true;
}

bool thread::detach() noexcept
{
    if (!is_joinable())
    {
        return false;
    }

    auto& ref = m_storage.get<thread_impl>();
    if (!ref.detach())
    {
        return false;
    }

    ref.close();
    return true;
}

thread_id this_thread::get_id()
{
    return thread_impl::convert_native_id(thread_impl::get_current_native_id());
}

} // namespace os
} // namespace vx
