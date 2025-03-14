#pragma once

#include "vertex/os/file.hpp"

namespace vx {
namespace os {
namespace __detail {

#define unsupported(op) VX_UNSUPPORTED("os::file::" op "()")

class file_impl
{
public:

    static bool exists(const path& p)
    {
        unsupported("exists");
        return false;
    }

    static bool open(__detail::file_impl_data& fd, const path& p, file::mode mode)
    {
        unsupported("open");
        return false;
    }

    static bool is_open(const __detail::file_impl_data& fd)
    {
        unsupported("is_open");
        return false;
    }

    static void close(__detail::file_impl_data& fd) {}

    static size_t size(const __detail::file_impl_data& fd)
    {
        unsupported("size");
        return 0;
    }

    static bool resize(__detail::file_impl_data& fd, size_t size)
    {
        unsupported("resize");
        return false;
    }

    static bool seek(__detail::file_impl_data& fd, int off, stream_position from)
    {
        unsupported("seek");
        return false;
    }

    static size_t tell(const __detail::file_impl_data& fd)
    {
        unsupported("tell");
        return 0;
    }

    static bool flush(__detail::file_impl_data& fd)
    {
        unsupported("flush");
        return false;
    }

    static size_t read(__detail::file_impl_data& fd, uint8_t* data, size_t size)
    {
        unsupported("read");
        return 0;
    }

    static size_t write(__detail::file_impl_data& fd, const uint8_t* data, size_t size)
    {
        unsupported("write");
        return 0;
    }
};

} // namespace __detail
} // namespace os
} // namespace vx