#pragma once

#include <thread>

#include "vertex/system/error.hpp"
#include "vertex/util/type_traits.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// thread
///////////////////////////////////////////////////////////////////////////////

class thread
{
public:

    using id = typename std::thread::id;
    static constexpr id INVALID_ID = id{};

    thread() = default;
    ~thread() = default;

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;
    
    thread(thread&& other) noexcept = default;
    thread& operator=(thread&& other) noexcept = default;

    void swap(thread& other) noexcept { m_thread.swap(other.m_thread); }

private:

    template <typename T>
    using not_same = type_traits::negation<std::is_same<type_traits::remove_cvref<T>, thread>>;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // start
    ///////////////////////////////////////////////////////////////////////////////

    template <typename Callable, typename... Args, VX_REQUIRES(not_same<Callable>::value)>
    bool start(Callable&& fn, Args&&... args)
    {
        static_assert(type_traits::is_invocable<
                typename std::decay<Callable>::type,
                typename std::decay<Args>::type...
            >::value,
            "thread arguments must be invocable after conversion to rvalues"
        );

        if (m_thread.joinable())
        {
            err::set(err::UNSUPPORTED_OPERATION, "thread already started");
            return false;
        }

        m_thread = std::thread(std::forward<Callable>(fn), std::forward<Args>(args)...);
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // helpers
    ///////////////////////////////////////////////////////////////////////////////

    bool is_valid() const noexcept { return m_thread.joinable(); }
    id get_id() const noexcept { return is_valid() ? m_thread.get_id() : INVALID_ID; }
    bool is_joinable() const noexcept { return m_thread.joinable(); }

    bool join() noexcept
    {
        if (!m_thread.joinable())
        {
            return false;
        }

        m_thread.join();
        return true;
    }

    bool detach() noexcept
    {
        if (!m_thread.joinable())
        {
            return false;
        }

        m_thread.detach();
        return true;
    }

private:

    std::thread m_thread;
};

namespace this_thread {

inline thread::id get_id() { return std::this_thread::get_id(); }

} // namespace this_thread

///////////////////////////////////////////////////////////////////////////////
// thread guard
///////////////////////////////////////////////////////////////////////////////

class thread_guard
{
public:

    explicit thread_guard(thread&& t) noexcept : m_thread(std::move(t)) {}
    thread_guard(thread_guard&& other) noexcept : m_thread(std::move(other.m_thread)) {}

    thread_guard(const thread_guard&) = delete;
    thread_guard& operator=(const thread_guard&) = delete;

    ~thread_guard()
    {
        if (m_thread.is_joinable())
        {
            m_thread.join();
        }
    }

    const os::thread& get_thread() const noexcept
    {
        return m_thread;
    }

    os::thread release() noexcept
    {
        return std::move(m_thread);
    }

private:

    os::thread m_thread;
};

} // namespace os
} // namespace vx
