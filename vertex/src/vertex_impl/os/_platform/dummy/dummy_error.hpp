#pragma once

#include "vertex/std/error.hpp"
#include "vertex/os/error.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::" op "()")

inline error_type get_last_error()
{
    unsupported("get_last_error");
    return error_type{};
}

inline void clear_last_error()
{
}

inline size_t format_error(error_type, char*, size_t)
{
    unsupported("format_error");
    return 0;
}

#undef unsupported

} // namespace os
} // namespace vx
