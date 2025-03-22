#pragma once

#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/os/shared_library.hpp"
#include "vertex/util/string/string_cast.hpp"

namespace vx {
namespace os {

#define assert_is_loaded(h) VX_ASSERT_MESSAGE(h.get() != NULL, "library not loaded")

class shared_library_impl
{
public:

    static bool load(handle& h, const char* lib)
    {
        VX_ASSERT_MESSAGE(!h.is_valid(), "library already loaded");

        const std::wstring wlib(str::string_cast<wchar_t>(lib));

        h = LoadLibraryW(wlib.c_str());
        if (h == NULL)
        {
            windows::error_message(lib);
            return false;
        }

        return true;
    }

    static bool is_loaded(const handle& h) noexcept
    {
        return h.is_valid();
    }

    static void free(handle& h) noexcept
    {
        assert_is_loaded(h);
        FreeLibrary(reinterpret_cast<HMODULE>(h.release()));
    }

    static void* get_addr(const handle& h, const char* symbol_name) noexcept
    {
        assert_is_loaded(h);
        return GetProcAddress(reinterpret_cast<HMODULE>(h.get()), symbol_name);
    }
};

} // namespace os
} // namespace vx