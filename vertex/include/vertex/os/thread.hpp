#pragma once

#include <memory>

#include "vertex/os/handle.hpp"
#include "vertex/util/function/invoke.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// thread
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

class thread_impl;

} // namespace __detail

///////////////////////////////////////////////////////////////////////////////

class thread
{
public:

    using id = uintptr_t;

    VX_API thread() noexcept;
    VX_API ~thread();

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;
    
    VX_API thread(thread&& other) noexcept;
    VX_API thread& operator=(thread&& other) noexcept;

    VX_API void swap(thread& other) noexcept;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // comparison
    ///////////////////////////////////////////////////////////////////////////////

#if defined(VX_USE_PTHREADS)

    // use pthread_equal()
    VX_API bool operator==(const thread& other) const noexcept;

#else

    bool operator==(const thread& other) const noexcept { return m_impl_data.thread_id == other.m_impl_data.thread_id; }

#endif // VX_USE_PTHREADS

    bool operator!=(const thread& other) const noexcept { return !operator==(other); }

private:

    ///////////////////////////////////////////////////////////////////////////////
    // helper types
    ///////////////////////////////////////////////////////////////////////////////

    // Abstract base class for types that wrap arbitrary
    // functors to be invoked in the new thread of execution.
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

#if defined(VX_USE_PTHREADS)

        static void* thread_entry(void* arg)
        {
            std::unique_ptr<invoker> self{ static_cast<invoker*>(arg) };
            (*self)(); // Invoke the callable
            return nullptr;
        }

#elif defined(VX_PLATFORM_WINDOWS)

        static unsigned int __stdcall thread_entry(void* arg)
        {
            std::unique_ptr<invoker> self{ static_cast<invoker*>(arg) };
            (*self)(); // Invoke the callable
            return 0;
        }

#else

        static void thread_entry(void* arg)
        {
            std::unique_ptr<invoker> self{ static_cast<invoker*>(arg) };
            (*self)(); // Invoke the callable
        }

#endif
    };

    template <typename... Args>
    using call_wrapper = invoker<std::tuple<typename std::decay<Args>::type...>>;

    template <typename T>
    using not_same = type_traits::negation<std::is_same<type_traits::remove_cvref<T>, thread>>;

    ///////////////////////////////////////////////////////////////////////////////
    // start
    ///////////////////////////////////////////////////////////////////////////////

    // windows complains if this is not exported even though it is private
    VX_API bool start_impl(void* fn, void* arg);

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

        if (!start_impl(reinterpret_cast<void*>(wrapper::thread_entry), w.get()))
        {
            return false;
        }

        w.release(); // ownership was passed to thread
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // helpers
    ///////////////////////////////////////////////////////////////////////////////

    VX_API bool is_valid() const noexcept;
    VX_API bool is_alive() const noexcept;

    id get_id() const noexcept { return m_impl_data.thread_id; }

    bool is_joinable() const noexcept
    {
        return is_valid();
    }

    VX_API bool join() noexcept;
    VX_API bool detach() noexcept;

private:

    friend __detail::thread_impl;

    struct impl_data
    {
        id thread_id = 0;
#if defined(VX_PLATFORM_WINDOWS)
        handle h;
#endif
    };

    impl_data m_impl_data;
};

namespace this_thread {

VX_API thread::id get_id();

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
