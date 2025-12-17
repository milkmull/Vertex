#pragma once

#include <dlfcn.h>

#include "vertex/os/shared_library.hpp"
#include "vertex_impl/os/_platform/unix/unix_tools.hpp"
#include "vertex/system/assert.hpp"

// https://github.com/libsdl-org/SDL/blob/6b13d69105721ac54cd7224b0493065c57776c89/cmake/sdlchecks.cmake#L45

namespace vx {
namespace os {
namespace _priv {

#define assert_is_loaded(h) VX_ASSERT_MESSAGE(h != NULL, "library not loaded")

struct shared_library_impl
{
    static bool load(void*& h, const char* lib)
    {
        VX_ASSERT_MESSAGE(h == NULL, "library already loaded");

        // Use RTLD_NOW to load all symbols immediately, adjust if lazy loading is preferred
        h = ::dlopen(lib, RTLD_LAZY);
        if (h == NULL)
        {
            //VX_ERR(err::system_error) << "failed to load library: " << lib << ": " << ::dlerror();
            return false;
        }

        return true;
    }

    static void free(void*& h) noexcept
    {
        assert_is_loaded(h);
        ::dlclose(h);
        h = NULL;
    }

    static void* get_addr(void* h, const char* symbol_name) noexcept
    {
        assert_is_loaded(h);
        return ::dlsym(h, symbol_name);
    }
};

} // namespace _priv
} // namespace os
} // namespace vx
