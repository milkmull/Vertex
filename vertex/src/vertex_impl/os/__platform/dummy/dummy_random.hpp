#pragma once

#include "vertex/system/error.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::" op "()")

inline bool get_entropy_impl(uint8_t* bytes, size_t count)
{
    unsupported("get_entropy");
    return false;
}

#undef unsupported

} // namespace os
} // namespace vx