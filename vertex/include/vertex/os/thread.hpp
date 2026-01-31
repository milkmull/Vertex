#pragma once

#include <cstdint>

#include "vertex/std/_priv/invoke.hpp"
#include "vertex/std/unique_ptr.hpp"
#include "vertex/std/memory.hpp"

namespace vx {
namespace os {

//=============================================================================
// thread
//=============================================================================

// platform specific thread interface (internal use only)
struct thread_impl;
// platform specific data used by the thread (internal use only)
struct thread_impl_data;

class thread
{
public:

    using id = uint64_t;

    VX_API thread() noexcept;
    VX_API ~thread();

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    VX_API thread(thread&& other) noexcept;
    VX_API thread& operator=(thread&& other) noexcept;

    VX_API void swap(thread& other) noexcept;

public:

    //=============================================================================
    // comparison
    //=============================================================================

    VX_API bool operator==(const thread& other) const noexcept;

    bool operator!=(const thread& other) const noexcept
    {
        return !operator==(other);
    }

private:

    //=============================================================================
    // helpers
    //=============================================================================

    template <typename F, typename Tuple, size_t... I>
    static auto apply_impl(F&& f, Tuple&& t, type_traits::index_sequence<I...>)
        -> decltype(f(std::get<I>(std::forward<Tuple>(t))...))
    {
        return f(std::get<I>(std::forward<Tuple>(t))...);
    }

    template <typename F, typename Tuple>
    static auto apply(F&& f, Tuple&& t)
        -> decltype(apply_impl(
            std::forward<F>(f),
            std::forward<Tuple>(t),
            type_traits::make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>{}))
    {
        return apply_impl(
            std::forward<F>(f),
            std::forward<Tuple>(t),
            type_traits::make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>{});
    }

    // callable_wrapper: holds callable + arguments and invokes them
    template <typename Callable, typename... Args>
    struct callable_wrapper
    {
        using tuple_type = std::tuple<typename std::decay<Callable>::type, typename std::decay<Args>::type...>;

        tuple_type data;

        callable_wrapper(Callable&& c, Args&&... args)
            : data(std::forward<Callable>(c), std::forward<Args>(args)...)
        {}

        void operator()() noexcept
        {
            invoke(std::integral_constant<size_t, 0>{});
        }

    private:

        // Helper to invoke the callable (the first tuple element) with the rest as arguments
        template <size_t... I>
        void invoke(type_traits::index_sequence<I...>) noexcept
        {
            auto& fn = std::get<0>(data);
            apply(fn, std::forward_as_tuple(std::get<I + 1>(data)...));
        }

        void invoke(std::integral_constant<size_t, 0>) noexcept
        {
            constexpr size_t N = std::tuple_size<tuple_type>::value;
            invoke(type_traits::make_index_sequence<N - 1>{});
        }

    public:

#if defined(HAVE_PTHREADS)

        static void* thread_entry(void* arg)
        {
            callable_wrapper* task = static_cast<callable_wrapper*>(arg);
            (*task)(); // Invoke the callable
            return nullptr;
        }

#elif defined(VX_OS_WINDOWS)

        static unsigned int VX_STDCALL thread_entry(void* arg)
        {
            callable_wrapper* task = static_cast<callable_wrapper*>(arg);
            (*task)(); // Invoke the callable
            mem::destroy(task);
            return 0;
        }

#else

        static void thread_entry(void* arg)
        {
            callable_wrapper* task = static_cast<callable_wrapper*>(arg);
            (*task)(); // Invoke the callable
            mem::destroy(task);
        }

#endif
    };

    template <typename T>
    using not_same = type_traits::negation<std::is_same<type_traits::remove_cvref<T>, thread>>;

    //=============================================================================
    // start
    //=============================================================================

    // windows complains if this is not exported even though it is private
    VX_API bool start_impl(void* fn, void* arg);

public:

    template <typename Callable, typename... Args, VX_REQUIRES(not_same<Callable>::value)>
    bool start(Callable&& fn, Args&&... args)
    {
        VX_STATIC_ASSERT_MSG((type_traits::is_invocable<
                                 typename std::decay<Callable>::type,
                                 typename std::decay<Args>::type...>::value),
            "Thread arguments must be invocable after conversion to rvalues");

        using wrapper = callable_wrapper<Callable, Args...>;
        auto w = mem::construct<wrapper>(std::forward<Callable>(fn), std::forward<Args>(args)...);
        if (!w)
        {
            return false;
        }

        if (!start_impl(reinterpret_cast<void*>(wrapper::thread_entry), w))
        {
            mem::destroy(w);
            return false;
        }

        // ownership of w was passed to thread
        return true;
    }

    //=============================================================================
    // helpers
    //=============================================================================

    VX_API bool is_valid() const noexcept;
    VX_API id get_id() const noexcept;

    bool is_joinable() const noexcept
    {
        return is_valid();
    }

    VX_API bool join() noexcept;
    VX_API bool detach() noexcept;

private:

    friend thread_impl;

    using impl_data_ptr = unique_ptr<thread_impl_data>;
    impl_data_ptr m_impl_data_ptr;
};

//=============================================================================
// this_thread
//=============================================================================

namespace this_thread {

VX_API thread::id get_id();

} // namespace this_thread

//=============================================================================
// thread guard
//=============================================================================

class thread_guard
{
public:

    explicit thread_guard(thread&& t) noexcept
        : m_thread(std::move(t))
    {}
    thread_guard(thread_guard&& other) noexcept
        : m_thread(std::move(other.m_thread))
    {}

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
