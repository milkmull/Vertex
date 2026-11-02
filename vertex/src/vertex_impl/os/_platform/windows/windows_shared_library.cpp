#pragma once

#include "vertex/os/shared_library.hpp"
#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

namespace vx {
namespace os {

success load_shared_library(shared_library_handle& h, native_cstring lib)
{
    h = ::LoadLibraryW(lib);

    if (h == NULL)
    {
        err::set_last_os_error();
        return err::system_error;
    }

    return success{};
}

void free_shared_library(shared_library_handle& h)
{
    ::FreeLibrary(reinterpret_cast<HMODULE>(h));
    h = NULL;
}

void* get_shared_library_symbol(shared_library_handle& h, shared_library_symbol_cstring symbol_name)
{
    return ::GetProcAddress(reinterpret_cast<HMODULE>(h), symbol_name);
}

} // namespace os
} // namespace vx
