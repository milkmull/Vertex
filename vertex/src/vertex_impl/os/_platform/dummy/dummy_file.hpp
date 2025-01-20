#pragma once

#include "vertex/os/file.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::file::" op "()")

class file::file_impl
{
public:

    file_impl() = default;
    ~file_impl() = default;

public:

    static bool exists(const path& p)
    {
        unsupported("exists");
        return false;
    }

public:

    bool open(const path& p, mode mode)
    {
        unsupported("open");
        return false;
    }

    bool is_open() const
    {
        unsupported("is_open");
        return false;
    }

    void close()
    {
        unsupported("close");
    }

    size_t size() const
    {
        unsupported("size");
        return 0;
    }

    bool seek(size_t off, stream_position from)
    {
        unsupported("seek");
        return false;
    }

    size_t tell() const
    {
        unsupported("tell");
        return 0;
    }

    size_t read(uint8_t* data, size_t size)
    {
        unsupported("read");
        return 0;
    }

    size_t write(const uint8_t* data, size_t size)
    {
        unsupported("write");
        return 0;
    }

    bool flush()
    {
        unsupported("flush");
        return false;
    }
};

#undef unsupported

} // namespace os
} // namespace vx
