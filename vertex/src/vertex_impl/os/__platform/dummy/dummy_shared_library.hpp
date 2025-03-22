#pragma once

#include "vertex/os/shared_library.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {
namespace __detail {

#define unsupported(op) VX_UNSUPPORTED("os::shared_library::" op "()")

class shared_library_impl
{
public:

    static bool load(handle&, const char*)
    {
        unsupported("load");
        return false;
    }

    static bool is_loaded(const handle&) noexcept
    {
        unsupported("is_loaded");
        return false;
    }

    static void free(handle&) noexcept {}

    static void* get_addr(const handle&, const char*) noexcept
    {
        unsupported("get_addr");
        return nullptr;
    }
};

} // namespace __detail
} // namespace os
} // namespace vx