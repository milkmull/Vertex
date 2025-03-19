#pragma once

#include <cstring>

#include "vertex_impl/os/__platform/unix/unix_header.hpp"
#include "vertex/os/handle.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {
namespace unix_ {

///////////////////////////////////////////////////////////////////////////////
// Error Handling
///////////////////////////////////////////////////////////////////////////////

inline int get_last_error() noexcept
{
    return errno;
}

inline void clear_error() noexcept
{
    errno = 0;
}

inline void error_message(const char* msg) noexcept
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