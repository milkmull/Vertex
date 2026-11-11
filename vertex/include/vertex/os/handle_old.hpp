#pragma once

#include <cstdint>

#include "vertex/config/os.hpp"

namespace vx {
namespace os {

class handle
{
public:

#if defined(VX_OS_WINDOWS)

    using native_handle = void*;
#   define VX_INVALID_HANDLE (native_handle)(intptr_t)(-1)

#else

    using native_handle = int;
#   define VX_INVALID_HANDLE -1

#endif

    static bool is_valid_handle(native_handle h) noexcept
    {
#   if defined(VX_OS_WINDOWS)
        return h != NULL && h != VX_INVALID_HANDLE;
#   else
        return h >= 0;
#   endif
    }

public:

    handle() noexcept : m_handle(VX_INVALID_HANDLE) {}
    ~handle() noexcept { close(); }

    handle(const handle&) = default;
    handle& operator=(const handle&) = default;

    handle(handle&& h) noexcept : m_handle(h.m_handle)
    {
        h.reset();
    }

    handle& operator=(handle&& h) noexcept
    {
        if (this != &h)
        {
            m_handle = h.m_handle;
            h.reset();
        }

        return *this;
    }

    handle(const native_handle h) noexcept : m_handle(h) {}

    handle& operator=(const native_handle h) noexcept
    {
        close();
        m_handle = h;
        return *this;
    }

public:

    bool operator==(const handle& h) { return m_handle == h.m_handle; }
    bool operator!=(const handle& h) { return m_handle != h.m_handle; }

public:

    bool is_valid() const noexcept { return is_valid_handle(m_handle); }

    native_handle& get() noexcept { return m_handle; }
    const native_handle& get() const noexcept { return m_handle; }

    void reset() noexcept { m_handle = VX_INVALID_HANDLE; }

    void close() noexcept
    {
        if (is_valid())
        {
            close_impl();
        }
        reset();
    }

    native_handle release() noexcept
    {
        native_handle h = m_handle;
        reset();
        return h;
    }

private:

#if defined(VX_OS_DUMMY)
    void close_impl() noexcept {}
#else
    void close_impl() noexcept;
#endif

    native_handle m_handle;
};

} // namespace os
} // namespace vx