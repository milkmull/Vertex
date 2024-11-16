#include "vertex/os/thread.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

thread::thread() noexcept {}

thread::~thread()
{
    join();
}

thread::thread(thread&& other) noexcept
    : m_thread(std::move(other.m_thread))
    , m_future(std::move(other.m_future)) {}

thread& thread::operator=(thread&& other) noexcept
{
    if (this != &other)
    {
        join();

        m_thread = std::move(other.m_thread);
        m_future = std::move(other.m_future);
    }

    return *this;
}

void swap(thread& lhs, thread& rhs) noexcept
{
    std::swap(lhs.m_thread, rhs.m_thread);
    std::swap(lhs.m_future, rhs.m_future);
}

thread::id thread::get_id() const noexcept
{
    return m_thread.get_id();
}

bool thread::is_valid() const noexcept
{
    return m_future.valid() && is_joinable();
}

bool thread::is_alive() const noexcept
{
    if (!is_valid())
    {
        return false;
    }

    return m_future.wait_for(std::chrono::seconds(0)) == std::future_status::timeout;
}

bool thread::is_joinable() const noexcept
{
    return m_thread.joinable();
}

bool thread::is_complete() const noexcept
{
    if (!is_valid())
    {
        return false;
    }

    return m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

bool thread::join()
{
    if (!is_joinable())
    {
        return false;
    }

    m_thread.join();
    return true;
}

bool thread::detatch()
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
