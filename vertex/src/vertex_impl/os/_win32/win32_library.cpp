#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/os/_win32/win32_library.hpp"
#include "vertex/stdlib/string/string.hpp"

namespace vx {
namespace os {

library::library_impl::library_impl()
    : m_handle(NULL) {}

library::library_impl::~library_impl()
{
    free();
}

bool library::library_impl::load(const std::string& lib)
{
    const std::wstring wlib(str::string_to_wstring(lib));

    m_handle = LoadLibraryW(wlib.c_str());
    if (m_handle == NULL)
    {
        WindowsErrorMessage(lib);
        return false;
    }

    return true;
}

bool library::library_impl::is_loaded() const
{
    return m_handle != NULL;
}

void library::library_impl::free()
{
    if (m_handle != NULL)
    {
        FreeLibrary(m_handle);
        m_handle = NULL;
    }
}

void* library::library_impl::get_function(const std::string& name) const
{
    return is_loaded() ? GetProcAddress(m_handle, name.c_str()) : NULL;
}

} // namespace os
} // namespace vx

#endif // VX_PLATFORM_WINDOWS