#pragma once

#include <dlfcn.h>

#include "vertex/os/shared_library.hpp"
#include "vertex_impl/os/__platform/unix/unix_tools.hpp"
#include "vertex/system/assert.hpp"

namespace vx {
namespace os {
namespace __detail {

#define assert_is_loaded(h) VX_ASSERT_MESSAGE(h != NULL, "library not loaded")

class shared_library_impl
{
public:

    static bool load(void*& h, const char* lib)
    {
        VX_ASSERT_MESSAGE(!h.is_valid(), "library already loaded");

        // Use RTLD_NOW to load all symbols immediately, adjust if lazy loading is preferred
        h = dlopen(lib, RTLD_NOW);
        if (h == NULL)
        {
            VX_ERR(err::SYSTEM_ERROR) << "failed to load library: " << lib << ": " << dlerror();
            return false;
        }

        return true;
    }

    static bool is_loaded(const void* h) noexcept
    {
        return h != NULL;
    }

    static void free(void*& h) noexcept
    {
        assert_is_loaded(h);
        dlclose(h);
        h = NULL;
    }

    static void* get_addr(void* h, const char* symbol_name) noexcept
    {
        assert_is_loaded(h);
        return dlsym(h, symbol_name);
    }
};

} // namespace __detail
} // namespace os
} // namespace vx