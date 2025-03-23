#pragma once

#include "vertex/system/error.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::" op "()")

static bool get_entropy_impl(uint8_t*, size_t)
{
    unsupported("get_entropy");
    return false;
}

#undef unsupported

} // namespace os
} // namespace vx