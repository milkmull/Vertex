#pragma once

#include <cstring>

#include "vertex_impl/os/__platform/unix/unix_header.hpp"
#include "vertex/os/__platform/unix/unix_handle.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {
namespace unix_ {

///////////////////////////////////////////////////////////////////////////////
// handle wrapper
///////////////////////////////////////////////////////////////////////////////

inline bool is_valid_handle(int h)
{
    return h >= 0;
}

inline void close_handle(int& h)
{
    if (is_valid_handle(h))
    {
        ::close(h);
        h = -1;
    }
}

inline handle::handle() noexcept : m_handle(-1) {}

inline handle::~handle() noexcept { close(); }

inline handle::handle(handle&& h) noexcept : m_handle(h.m_handle)
{
    h.reset();
}

inline handle& handle::operator=(handle&& h) noexcept
{
    if (this != &h)
    {
        m_handle = h.m_handle;
        h.reset();
    }

    return *this;
}

inline handle::handle(const int h) noexcept : m_handle(h) {}

inline handle& handle::operator=(const int h) noexcept
{
    close();
    m_handle = h;
    return *this;
}

inline bool handle::is_valid() const noexcept
{
    return is_valid_handle(m_handle);
}

inline int handle::get() const noexcept { return m_handle; }

inline void handle::reset() noexcept { m_handle = -1; }

inline void handle::close() noexcept
{
    close_handle(m_handle);
}

///////////////////////////////////////////////////////////////////////////////
// Error Handling
///////////////////////////////////////////////////////////////////////////////

inline auto get_last_error()
{
    return errno;
}

inline void clear_error()
{
    errno = 0;
}

inline void error_message(const char* msg)
{
    const size_t msg_size = std::strlen(msg);

    VX_ERR(err::PLATFORM_ERROR)
        << msg
        << ((msg_size == 0) ? "" : ": ")
        << std::strerror(errno);
}

} // namespace unix_
} // namespace os
} // namespace vx