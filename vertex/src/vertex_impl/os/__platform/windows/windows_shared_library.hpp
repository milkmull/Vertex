#pragma once

#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/os/shared_library.hpp"
#include "vertex/util/string/string_cast.hpp"

namespace vx {
namespace os {
namespace __detail {

#define assert_is_loaded(h) VX_ASSERT_MESSAGE(h != NULL, "library not loaded")

struct shared_library_impl
{
    static bool load(void*& h, const char* lib)
    {
        VX_ASSERT_MESSAGE(h == NULL, "library already loaded");

        const std::wstring wlib(str::string_cast<wchar_t>(lib));

        h = LoadLibraryW(wlib.c_str());
        if (h == NULL)
        {
            windows::error_message(lib);
            return false;
        }

        return true;
    }

    static void free(void*& h) noexcept
    {
        assert_is_loaded(h);
        FreeLibrary(reinterpret_cast<HMODULE>(h));
        h = NULL;
    }

    static void* get_addr(void* h, const char* symbol_name) noexcept
    {
        assert_is_loaded(h);
        return GetProcAddress(reinterpret_cast<HMODULE>(h), symbol_name);
    }
};

} // namespace __detail
} // namespace os
} // namespace vx