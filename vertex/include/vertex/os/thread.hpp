#pragma once

#include <future>
#include <thread>

#include "vertex/os/__platform/thread_impl_data.hpp"
#include "vertex/util/function/invoke.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

namespace __detail {

// Abstract base class for types that wrap arbitrary functors to be
// invoked in the new thread of execution.
struct thread_state
{
    virtual ~thread_state() = default;
    virtual void run() = 0;
};

template <typename Callable>
struct thread_state_impl : public thread_state
{
    Callable fn;

    template <typename... Args>
    thread_state_impl(Args&&... args) : fn(std::forward<Args>(args)...) {}

    void run() { fn(); }
};

} // namespace __detail

///////////////////////////////////////////////////////////////////////////////
// thread
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

class thread_impl;

} // namespace __detail

class thread
{
public:

    using id = uint32_t;

    thread() noexcept = default;
    ~thread() { if (is_joinable()) { std::terminate(); } }

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;
    
    thread(thread&& other) noexcept { swap(other); }

    thread& operator=(thread&& other) noexcept
    {
        if (is_joinable()) { std::terminate(); }
        swap(other);
        return *this;
    }

    void swap(thread& other) noexcept { std::swap(m_impl_data, other.m_impl_data); }

private:

    // https://github.com/microsoft/STL/blob/main/stl/inc/thread#L73
    // https://github.com/gcc-mirror/gcc/blob/3014f8787196d7c0d15d24195c8f07167968ff55/libstdc%2B%2B-v3/include/bits/std_thread.h

    template <typename Tuple>
    struct invoker
    {
    public:

        template <typename... Args>
        explicit invoker(Args&&... args) : invoke_data(std::forward<Args>(args)...) {}

    private:

        Tuple invoke_data;

        template <typename> struct result;

        template <typename Fn, typename... Args>
        struct result<std::tuple<Fn, Args...>> : type_traits::invoke_result<Fn, Args...> {};

        template <size_t... In>
        typename result<Tuple>::type invoke(type_traits::index_sequence<In...>)
        {
            return fn::invoke(std::get<In>(std::move(invoke_data))...);
        }

        auto operator()()
        {
            using In = type_traits::make_index_sequence<std::tuple_size<Tuple>::value>;
            return invoke(In{});
        }

    public:

#   if defined(VX_PLATFORM_WINDOWS)

        static unsigned int __stdcall thread_entry(void* arg)
        {
            std::unique_ptr<invoker> self{ static_cast<invoker*>(arg) };
            (*self)(); // Invoke the callable
            return 0;
        }

#   else

        static void* thread_entry(void* arg)
        {
            std::unique_ptr<invoker> self{ static_cast<invoker*>(arg) };
            (*self)(); // Invoke the callable
            return nullptr;
        }

#   endif // VX_PLATFORM_WINDOWS
    };

    template <typename... Args>
    using call_wrapper = invoker<std::tuple<typename std::decay<Args>::type...>>;

    template <typename T>
    using not_same = type_traits::negation<std::is_same<type_traits::remove_cvref<T>, thread>>;

    bool start_impl(void* fn, void* arg);

public:

    template <typename Callable, typename... Args, VX_REQUIRES(not_same<Callable>::value)>
    bool start(Callable&& fn, Args&&... args)
    {
        static_assert(type_traits::is_invocable<
                typename std::decay<Callable>::type,
                typename std::decay<Args>::type...
            >::value,
            "thread arguments must be invocable after conversion to rvalues"
        );

        using wrapper = call_wrapper<Callable, Args...>;
        std::unique_ptr<wrapper> w = std::make_unique<wrapper>(
            std::forward<Callable>(fn),
            std::forward<Args>(args)...
        );

        if (!start_impl(wrapper::thread_entry, w.get()))
        {
            return false;
        }

        w.release(); // ownership was passed to thread
        return true;
    }

    id get_id() const noexcept
    {
        return is_valid() ? static_cast<id>(m_impl_data.id) : 0;
    }

    VX_API bool is_valid() const noexcept;
    VX_API bool is_alive() const noexcept;

    bool is_joinable() const noexcept
    {
        return is_valid();
    }

    VX_API bool join() noexcept;
    VX_API bool detach() noexcept;

private:

    using thread_impl = __detail::thread_impl;
    friend thread_impl;

    using impl_data = __detail::thread_impl_data;
    impl_data m_impl_data;
};

//class thread
//{
//public:
//
//    using id = std::thread::id;
//
//    thread() noexcept = default;
//    ~thread() noexcept = default;
//
//    thread(const thread&) = delete;
//    thread& operator=(const thread&) = delete;
//
//    thread(thread&& other) noexcept
//        : m_thread(std::move(other.m_thread))
//        , m_future(std::move(other.m_future)) {}
//
//    thread& operator=(thread&& other) noexcept
//    {
//        if (this != &other)
//        {
//            join();
//
//            m_thread = std::move(other.m_thread);
//            m_future = std::move(other.m_future);
//        }
//
//        return *this;
//    }
//
//public:
//
//    template <typename Func, typename... Args>
//    bool start(Func&& func, Args&&... args)
//    {
//        if (is_alive())
//        {
//            VX_ERR(err::RUNTIME_ERROR) << "thread already running";
//            return false;
//        }
//
//        // Create a promise and get the associated future
//        std::promise<void> promise;
//        m_future = promise.get_future();
//
//        m_thread = std::thread(
//            [this](std::packaged_task<void()> f, std::promise<void> p) mutable
//            {
//                f();
//                p.set_value(); // Signal that the thread has completed execution
//            },
//            std::packaged_task<void()>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...)),
//            std::move(promise)
//        );
//
//        return true;
//    }
//
//    id get_id() const noexcept
//    {
//        return m_thread.get_id();
//    }
//
//    bool is_valid() const noexcept
//    {
//        return m_future.valid() && is_joinable();
//    }
//
//    bool is_alive() const noexcept
//    {
//        return is_valid()
//            && m_future.wait_for(std::chrono::seconds(0)) == std::future_status::timeout;
//    }
//
//    bool is_joinable() const noexcept
//    {
//        return m_thread.joinable();
//    }
//
//    bool is_complete() const noexcept
//    {
//        return !is_joinable() && m_future.valid()
//            && m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
//    }
//
//    bool join() noexcept
//    {
//        if (!is_joinable())
//        {
//            return false;
//        }
//
//        m_thread.join();
//        return true;
//    }
//
//    bool detach() noexcept
//    {
//        if (!is_joinable())
//        {
//            return false;
//        }
//
//        m_thread.detach();
//        return true;
//    }
//
//private:
//
//    std::thread m_thread;
//    std::future<void> m_future;
//};
//
//namespace this_thread {
//
//inline thread::id get_id() noexcept
//{
//    return std::this_thread::get_id();
//}
//
//} // namespace this_thread

} // namespace os
} // namespace vx
