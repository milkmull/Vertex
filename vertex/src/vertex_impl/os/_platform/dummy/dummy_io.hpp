#pragma once

#include "vertex/os/io.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::io::" op "()")

static size_t write_raw_impl(stream s, const char* data, size_t size)
{
    unsupported("write_raw");
    return 0;
}

static size_t read_raw_impl(stream s, char* data, size_t size)
{
    unsupported("read_raw");
    return 0;
}

#undef unsupported

} // namespace os
} // namespace vx
