#pragma once

#include <cstdint>

#include "vertex/config/os.hpp"

namespace vx {
namespace os {

#if defined(VX_OS_WINDOWS)

using native_handle = void*;

#else

using native_handle = int;

#endif

inline native_handle invalid_handle() noexcept
{
#if defined(VX_OS_WINDOWS)

    return (native_handle)(intptr_t)(-1);

#else

    return -1;

#endif
}

inline bool is_valid_handle(native_handle h) noexcept
{
#if defined(VX_OS_WINDOWS)
    return h != NULL && h != invalid_handle();
#else
    return h >= 0;
#endif
}

VX_API void close_handle(native_handle h) noexcept;

class handle_guard
{
public:

    explicit handle_guard(native_handle h) noexcept
        : m_h(h)
    {}

    ~handle_guard()
    {
        if (is_valid_handle(m_h))
        {
            close_handle(m_h);
        }
    }

    handle_guard(const handle_guard&) = delete;
    handle_guard& operator=(const handle_guard&) = delete;

    handle_guard(handle_guard&& o) noexcept
        : m_h(o.m_h)
    {
        o.m_h = invalid_handle();
    }

    bool valid() const noexcept
    {
        return is_valid_handle(m_h);
    }

    native_handle get() const noexcept
    {
        return m_h;
    }

private:

    native_handle m_h;
};

} // namespace os
} // namespace vx
