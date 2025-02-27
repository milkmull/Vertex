#pragma once

#include "vertex/os/shared_library.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::shared_library::" op "()")

class shared_library::shared_library_impl
{
public:

    shared_library_impl() = default;
    ~shared_library_impl() = default;

public:

    bool load(const std::string& lib)
    {
        unsupported("load");
        return false;
    }

    bool is_loaded() const
    {
        unsupported("is_loaded");
        return false;
    }

    void free()
    {
        unsupported("free");
    }

    path location() const
    {
        unsupported("location");
        return {};
    }

    void* get_addr(const std::string& name) const
    {
        unsupported("get_addr");
        return nullptr;
    }
};

#undef unsupported

} // namespace os
} // namespace vx