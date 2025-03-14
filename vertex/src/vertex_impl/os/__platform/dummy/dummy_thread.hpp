#pragma once

#include "vertex/os/thread.hpp"

namespace vx {
namespace os {
namespace __detail {

#define unsupported(op) VX_UNSUPPORTED("os::thread::" op "()")

class thread_impl
{
public:

    static bool is_valid(const thread::impl_data& td) noexcept
    {
        return false;
    }

    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=msvc-170

    static bool start(thread::impl_data& td, void* fn, void* arg)
    {
        unsupported("start");
        return false;
    }

    static void close(thread::impl_data& td) noexcept {}
    static void exit(thread::impl_data& td, unsigned int exit_code = 0) noexcept {}

    static bool is_alive(const thread::impl_data& td) noexcept
    {
        unsupported("is_alive");
        return false;
    }

    static thread::id get_id(const thread::impl_data& td) noexcept
    {
        unsupported("get_id");
        return 0;
    }

    // returns when thread terminates
    static bool join(thread::impl_data& td) noexcept
    {
        unsupported("join");
        return false;
    }

    static bool detach(thread::impl_data& td) noexcept
    {
        unsupported("detach");
        return false;
    }

    static thread::id get_this_thread_id() noexcept
    {
        unsupported("this_thread::get_id");
        return 0;
    }
};

} // namespace __detail
} // namespace os
} // namespace vx
