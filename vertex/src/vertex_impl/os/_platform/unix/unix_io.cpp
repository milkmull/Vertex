#include "vertex_impl/os/_platform/unix/unix_tools.hpp"
#include "vertex/os/io.hpp"

#include <cerrno>
#include <unistd.h>

namespace vx {
namespace os {

static int get_fd(stream s)
{
    switch (s)
    {
        case stream::in:
        {
            return STDIN_FILENO;
        }
        case stream::out:
        {
            return STDOUT_FILENO;
        }
        case stream::err:
        {
            return STDERR_FILENO;
        }
    }

    return -1;
}

size_t write_raw_impl(stream s, const char* data, size_t size)
{
    const int fd = get_fd(s);
    size_t total = 0;

    while (size > 0)
    {
        const ssize_t written = ::write(fd, data, size);

        if (written < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            unix_::error_message("write");
            break;
        }

        if (written == 0)
        {
            break;
        }

        data += written;
        size -= static_cast<size_t>(written);
        total += static_cast<size_t>(written);
    }

    return total;
}

size_t read_raw_impl(stream s, char* data, size_t size)
{
    const int fd = get_fd(s);
    size_t total = 0;

    while (size > 0)
    {
        const ssize_t read = ::read(fd, data, size);

        if (read < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            unix_::error_message("read");
            break;
        }

        if (read == 0)
        {
            // EOF
            break;
        }

        data += read;
        size -= static_cast<size_t>(read);
        total += static_cast<size_t>(read);
    }

    return total;
}

} // namespace os
} // namespace vx
