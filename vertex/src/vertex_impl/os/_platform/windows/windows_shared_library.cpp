#include "vertex_impl/os/_platform/windows/windows_shared_library.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace os {

shared_library::shared_library_impl::shared_library_impl()
    : m_handle(NULL) {}

shared_library::shared_library_impl::~shared_library_impl()
{
    free();
}

bool shared_library::shared_library_impl::load(const std::string& lib)
{
    const std::wstring wlib(str::string_cast<wchar_t>(lib));

    m_handle = LoadLibraryW(wlib.c_str());
    if (m_handle == NULL)
    {
        windows::error_message(lib);
        return false;
    }

    return true;
}

bool shared_library::shared_library_impl::is_loaded() const
{
    return m_handle != NULL;
}

void shared_library::shared_library_impl::free()
{
    if (m_handle != NULL)
    {
        FreeLibrary(m_handle);
        m_handle = NULL;
    }
}

void* shared_library::shared_library_impl::get_function(const std::string& name) const
{
    return is_loaded() ? GetProcAddress(m_handle, name.c_str()) : NULL;
}

} // namespace os
} // namespace vx