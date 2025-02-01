#include "vertex/os/thread.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

VX_API thread::thread() noexcept {}

VX_API thread::~thread() noexcept
{
    join();
}

VX_API thread::thread(thread&& other) noexcept
    : m_thread(std::move(other.m_thread))
    , m_future(std::move(other.m_future)) {}

VX_API thread& thread::operator=(thread&& other) noexcept
{
    if (this != &other)
    {
        join();

        m_thread = std::move(other.m_thread);
        m_future = std::move(other.m_future);
    }

    return *this;
}

VX_API thread::id thread::get_id() const noexcept
{
    return m_thread.get_id();
}

VX_API bool thread::is_valid() const noexcept
{
    return m_future.valid() && is_joinable();
}

VX_API bool thread::is_alive() const noexcept
{
    if (!is_valid())
    {
        return false;
    }

    return m_future.wait_for(std::chrono::seconds(0)) == std::future_status::timeout;
}

VX_API bool thread::is_joinable() const noexcept
{
    return m_thread.joinable();
}

VX_API bool thread::is_complete() const noexcept
{
    if (!is_valid())
    {
        return false;
    }

    return m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

VX_API bool thread::join() noexcept
{
    if (!is_joinable())
    {
        return false;
    }

    m_thread.join();
    return true;
}

VX_API bool thread::detatch() noexcept
{
    if (!is_joinable())
    {
        return false;
    }

    m_thread.detach();
    return true;
}

VX_API thread::id this_thread::get_id() noexcept
{
    return std::this_thread::get_id();
}

} // namespace os
} // namespace vx
