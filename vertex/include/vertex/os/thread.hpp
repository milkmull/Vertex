#pragma once

#include "vertex/system/error.hpp"

#include <future>
#include <functional>
#include <thread>

namespace vx {
namespace os {

class thread
{
public:

    using id = std::thread::id;

    thread() noexcept;
    ~thread();

    thread(const thread&) = delete;
    thread(thread&& other) noexcept;

    thread& operator=(const thread&) = delete;
    thread& operator=(thread&& other) noexcept;

    friend void swap(thread& lhs, thread& rhs) noexcept;

public:

    template <typename Func, typename... Args>
    bool start(Func&& func, Args&&... args)
    {
        if (is_alive())
        {
            VX_ERROR(error::error_code::RUNTIME_ERROR) << "thread already running";
            return false;
        }

        // Create a promise and get the associated future
        std::promise<void> promise;
        m_future = promise.get_future();

        auto bound_func = std::bind(
            std::forward<Func>(func),
            std::forward<Args>(args)...
        );

        m_thread = std::thread(
            [this, f = std::move(bound_func), p = std::move(promise)]() mutable
            {
                f();
                p.set_value();
            }
        );

        return true;
    }

    id get_id() const noexcept;

    bool is_valid() const noexcept;
    bool is_alive() const noexcept;
    bool is_joinable() const noexcept;
    bool is_complete() const noexcept;

    bool join();
    bool detatch();

private:

    std::thread m_thread;
    std::future<void> m_future;
};

namespace this_thread {

inline thread::id get_id() noexcept;

} // namespace this_thread

} // namespace os
} // namespace vx
