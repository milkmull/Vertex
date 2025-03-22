#pragma once

#include "vertex/os/mutex.hpp"

namespace vx {
namespace os {
namespace __detail {

#define unsupported(op) VX_UNSUPPORTED("os::mutex::" op "()")

class mutex_impl
{
public:

    static void create(mutex::impl_data& data) noexcept {}
    static void destroy(mutex::impl_data& data) noexcept {}

    static bool lock(mutex::impl_data& data) noexcept
    {
        unsupported("lock");
        return false;
    }

    static bool try_lock(mutex::impl_data& data) noexcept
    {
        unsupported("try_lock");
        return false;
    }

    static void unlock(mutex::impl_data& data) noexcept
    {
        unsupported("unlock");
    }
};

} // namespace __detail
} // namespace os
} // namespace vx