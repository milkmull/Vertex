
#if defined(HAVE_GETRANDOM)
#   include <sys/random.h>
#else
#   include <fcntl.h>
#   include <unistd.h>
#endif

#include "vertex_impl/os/__platform/unix/unix_tools.hpp"

namespace vx {
namespace os {

// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/rand-s?view=msvc-170
// https://github.com/microsoft/STL/blob/25dc2b72b6f594326253cf3a05543f7a5750f802/stl/src/xrngdev.cpp#L10

bool get_entropy_impl(uint8_t* bytes, size_t count)
{
    if (bytes == nullptr)
    {
        return false;
    }
    if (count == 0)
    {
        return true;
    }

#if defined(HAVE_GETRANDOM)

    size_t off = 0;
    while (off != count)
    {
        const ssize_t r = ::getrandom(bytes + off, count - off, 0);
        if (r < 0)
        {
            if (errno == EINTR)
            {
                // Retry on interrupt
                continue;
            }

            unix_::error_message("getrandom()");
            return false;
        }

        off += static_cast<size_t>(r);
    }

#else

    int fd = ::open("/dev/urandom", O_RDONLY);
    if (fd < 0)
    {
        unix_::error_message("open()");
        return false;
    }

    size_t off = 0;
    while (off != count)
    {
        const ssize_t r = ::read(fd, bytes + off, count - off);
        if (r <= 0)
        {
            if (r < 0 && errno == EINTR)
            {
                // Retry on interrupt
                continue;
            }

            unix_::error_message("read()");
            ::close(fd);
            return false;
        }

        off += static_cast<size_t>(r);
    }

    ::close(fd);

#endif

    return true;
}

} // namespace os
} // namespace vx