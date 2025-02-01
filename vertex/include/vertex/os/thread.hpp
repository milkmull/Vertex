#pragma once

#include <future>
#include <functional>
#include <thread>

#include "vertex/system/compiler.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

class thread
{
public:

    using id = std::thread::id;

    VX_API thread() noexcept;
    VX_API ~thread() noexcept;

    thread(const thread&) = delete;
    VX_API thread(thread&& other) noexcept;

    thread& operator=(const thread&) = delete;
    VX_API thread& operator=(thread&& other) noexcept;

public:

    template <typename Func, typename... Args>
    bool start(Func&& func, Args&&... args)
    {
        if (is_alive())
        {
            VX_ERR(err::RUNTIME_ERROR) << "thread already running";
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

    VX_API id get_id() const noexcept;

    VX_API bool is_valid() const noexcept;
    VX_API bool is_alive() const noexcept;
    VX_API bool is_joinable() const noexcept;
    VX_API bool is_complete() const noexcept;

    VX_API bool join() noexcept;
    VX_API bool detatch() noexcept;

private:

    std::thread m_thread;
    std::future<void> m_future;
};

namespace this_thread {

VX_API thread::id get_id() noexcept;

} // namespace this_thread

} // namespace os
} // namespace vx
