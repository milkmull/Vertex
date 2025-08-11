#pragma once

#include "vertex/os/file.hpp"

namespace vx {
namespace os {
namespace _priv {

#define unsupported(op) VX_UNSUPPORTED("os::file::" op "()")

struct file_impl
{
    static bool exists(const path&)
    {
        unsupported("exists");
        return false;
    }

    static bool open(handle&, const path&, file::mode)
    {
        unsupported("open");
        return false;
    }

    static bool is_open(const handle&)
    {
        unsupported("is_open");
        return false;
    }

    static void close(handle&) {}

    static size_t size(const handle&)
    {
        unsupported("size");
        return 0;
    }

    static bool resize(handle&, size_t)
    {
        unsupported("resize");
        return false;
    }

    static bool seek(handle&, int, stream_position)
    {
        unsupported("seek");
        return false;
    }

    static size_t tell(const handle&)
    {
        unsupported("tell");
        return 0;
    }

    static bool flush(handle&)
    {
        unsupported("flush");
        return false;
    }

    static size_t read(handle&, uint8_t*, size_t)
    {
        unsupported("read");
        return 0;
    }

    static size_t write(handle&, const uint8_t*, size_t)
    {
        unsupported("write");
        return 0;
    }
};

} // namespace _priv
} // namespace os
} // namespace vx