#include "vertex_impl/os/_platform/platform_shared_library.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

VX_API bool shared_library::load(const char* lib)
{
    if (is_loaded())
    {
        err::set(err::unsupported_operation, "shared library already loaded");
        return false;
    }

    return _priv::shared_library_impl::load(m_handle, lib);
}

VX_API void shared_library::free() noexcept
{
    if (is_loaded())
    {
        _priv::shared_library_impl::free(m_handle);
    }
}

void* shared_library::get_void(const char* symbol_name) const noexcept
{
    return is_loaded() ? _priv::shared_library_impl::get_addr(m_handle, symbol_name) : nullptr;
}

} // namespace os
} // namespace vx