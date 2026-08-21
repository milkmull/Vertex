#pragma once

#include <cerrno>

#include "vertex/os/error.hpp"
#include "vertex/std/char_traits.hpp"
#include "vertex/system/error.hpp"
#include "vertex_impl/os/_platform/unix/unix_header.hpp"

namespace vx {
namespace os {

inline error_type get_last_error_impl()
{
    return errno;
}

inline void clear_last_error_impl() noexcept
{
    errno = 0;
}

inline size_t format_error_impl(error_type error, char* buffer, size_t size)
{
    using traits = str::char_traits<char>;

    const char* message = std::strerror(static_cast<int>(error));
    const size_t count = traits::length(message);

    if (count == 0)
    {
        return 0;
    }

    const size_t copy_count = std::min(size, count);
    mem::copy(buffer, message, copy_count);

    return copy_count;
}

} // namespace os
} // namespace vx
