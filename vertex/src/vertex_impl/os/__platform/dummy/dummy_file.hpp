#pragma once

#include "vertex/os/file.hpp"

namespace vx {
namespace os {
namespace __detail {

#define unsupported(op) VX_UNSUPPORTED("os::file::" op "()")

class file_impl
{
public:

    static bool exists(const path&)
    {
        unsupported("exists");
        return false;
    }

    static bool open(__detail::file_impl_data&, const path&, file::mode)
    {
        unsupported("open");
        return false;
    }

    static bool is_open(const __detail::file_impl_data&)
    {
        unsupported("is_open");
        return false;
    }

    static void close(__detail::file_impl_data&) {}

    static size_t size(const __detail::file_impl_data&)
    {
        unsupported("size");
        return 0;
    }

    static bool resize(__detail::file_impl_data&, size_t)
    {
        unsupported("resize");
        return false;
    }

    static bool seek(__detail::file_impl_data&, int, stream_position)
    {
        unsupported("seek");
        return false;
    }

    static size_t tell(const __detail::file_impl_data&)
    {
        unsupported("tell");
        return 0;
    }

    static bool flush(__detail::file_impl_data&)
    {
        unsupported("flush");
        return false;
    }

    static size_t read(__detail::file_impl_data&, uint8_t*, size_t)
    {
        unsupported("read");
        return 0;
    }

    static size_t write(__detail::file_impl_data&, const uint8_t*, size_t)
    {
        unsupported("write");
        return 0;
    }
};

} // namespace __detail
} // namespace os
} // namespace vx