#pragma once

#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/os/shared_library.hpp"
#include "vertex/util/string/string_cast.hpp"

namespace vx {
namespace os {
namespace __detail {

#define assert_is_loaded(h) VX_ASSERT_MESSAGE(h != NULL, "library not loaded")

class shared_library_impl
{
public:

    static bool load(shared_library_impl_data& sld, const char* lib)
    {
        VX_ASSERT_MESSAGE(sld.handle == NULL, "library already open");

        const std::wstring wlib(str::string_cast<wchar_t>(lib));

        sld.handle = LoadLibraryW(wlib.c_str());
        if (sld.handle == NULL)
        {
            windows::error_message(lib);
            return false;
        }

        return true;
    }

    static bool is_loaded(const shared_library_impl_data& sld) noexcept
    {
        return sld.handle != NULL;
    }

    static void free(shared_library_impl_data& sld) noexcept
    {
        assert_is_loaded(sld.handle);

        FreeLibrary(reinterpret_cast<HMODULE>(sld.handle));
        sld.handle = NULL;
    }

    static void* get_addr(const shared_library_impl_data& sld, const char* symbol_name) noexcept
    {
        assert_is_loaded(sld.handle);

        return GetProcAddress(reinterpret_cast<HMODULE>(sld.handle), symbol_name);
    }
};

} // namespace __detail
} // namespace os
} // namespace vx