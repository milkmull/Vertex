#pragma once

#include "vertex/system/error.hpp"

#include <atomic>
#include <functional>
#include <thread>

namespace vx {

class thread
{
public:

    using id = std::thread::id;

    thread() noexcept
        : m_alive(false) {}

    template <typename F, typename... Args>
    thread(F&& func, Args&&... args)
        : m_alive(false)
        , m_function(std::bind(std::forward<F>(func), std::forward<Args>(args)...)) {}

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    thread(thread&& other) noexcept
        : m_thread(std::move(other.m_thread))
        , m_function(std::move(other.m_function))
        , m_alive(other.m_alive.load())
    {
        other.m_alive = false;
    }

    thread& operator=(thread&& other) noexcept
    {
        if (this != &other)
        {
            join();
            m_thread = std::move(other.m_thread);
            m_function = std::move(other.m_function);
            m_alive = other.m_alive.load();
            other.m_alive = false;
        }

        return *this;
    }

    ~thread()
    {
        join();
    }

    void start()
    {
        if (!m_function)
        {
            VX_ERROR(error::error_code::RUNTIME_ERROR) << "no function was set for this thread";
            return;
        }

        if (joinable())
        {
            VX_ERROR(error::error_code::RUNTIME_ERROR) << "thread already running";
            return;
        }

        m_thread = std::thread(
            [this]()
            {
                m_alive = true;
                m_function();
                m_alive = false;
            }
        );
    }

    bool joinable() const noexcept
    {
        return m_thread.joinable();
    }

    void join()
    {
        if (joinable())
        {
            m_thread.join();
        }
    }

    bool is_alive() const noexcept
    {
        return m_alive;
    }

    id get_id() const noexcept
    {
        return m_thread.get_id();
    }

private:

    std::atomic<bool> m_alive;
    std::function<void()> m_function;
    std::thread m_thread;

};

} // namespace vx
