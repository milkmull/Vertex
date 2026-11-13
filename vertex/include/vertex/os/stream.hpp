#pragma once

#include "vertex/os/handle.hpp"
#include "vertex/std/error/error_code.hpp"

namespace vx {
namespace os {

enum class stream
{
    in = 0,
    out,
    err
};

VX_API native_handle get_stream_handle(stream s);

struct rw_result
{
    size_t bytes; // bytes successfully transferred
    error err;    // error terminating the operation, or none
};

namespace _stream_priv {

VX_API rw_result write_impl(native_handle h, const uint8_t* data, size_t size);

inline rw_result safe_write(native_handle h, const uint8_t* data, size_t size)
{
    return write_impl(h, data, size);
}

} // namespace _stream_priv

} // namespace os
} // namespace vx
