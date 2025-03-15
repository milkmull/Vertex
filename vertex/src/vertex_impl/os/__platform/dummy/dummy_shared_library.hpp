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

    static bool load(shared_library_impl_data&, const char*)
    {
        unsupported("load");
        return false;
    }

    static bool is_loaded(const shared_library_impl_data&) noexcept
    {
        unsupported("is_loaded");
        return false;
    }

    static void free(shared_library_impl_data&) noexcept {}

    static void* get_addr(const shared_library_impl_data&, const char*) noexcept
    {
        unsupported("get_addr");
        return nullptr;
    }
};

} // namespace __detail
} // namespace os
} // namespace vx