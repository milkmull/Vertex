#pragma once

#include "vertex/os/shared_library.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {
namespace _priv {

#define unsupported(op) VX_UNSUPPORTED("os::shared_library::" op "()")

struct shared_library_impl
{
    static bool load(void*&, const char*)
    {
        unsupported("load");
        return false;
    }

    static void free(void*&) noexcept {}

    static void* get_addr(void*, const char*) noexcept
    {
        unsupported("get_addr");
        return nullptr;
    }
};

} // namespace _priv
} // namespace os
} // namespace vx