#include <unordered_map>

#include "vertex_impl/os/__platform/shared_library.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace os {

VX_API bool shared_library::load(const char* lib)
{
    if (is_loaded())
    {
        VX_ERR(err::UNSUPPORTED_OPERATION) << "shared_library already loaded";
        return false;
    }

    return shared_library_impl::load(m_impl_data, lib);
}

VX_API bool shared_library::is_loaded() const noexcept
{
    return shared_library_impl::is_loaded(m_impl_data);
}

VX_API void shared_library::free() noexcept
{
    if (is_loaded())
    {
        shared_library_impl::free(m_impl_data);
    }
}

void* shared_library::get_void(const char* symbol_name) const noexcept
{
    return is_loaded() ? shared_library_impl::get_addr(m_impl_data, symbol_name) : nullptr;
}

} // namespace os
} // namespace vx