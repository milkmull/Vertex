#include "vertex_impl/system/platform_config.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_windows/windows_library.hpp"
#endif

#include "vertex/system/error.hpp"

namespace vx {
namespace os {

VX_API library::library() {}

library::~library()
{
    free();
}

VX_API library::library(const library& l)
    : m_impl(l.m_impl) {}

VX_API library::library(library&& l) noexcept
    : m_impl(std::move(l.m_impl)) {}

VX_API library& library::operator=(const library& l)
{
    m_impl = l.m_impl;
    return *this;
}

VX_API library& library::operator=(library&& l) noexcept
{
    m_impl = std::move(l.m_impl);
    return *this;
}

VX_API library::library(const std::string& name)
{
    load(name);
}

VX_API bool library::load(const std::string& lib)
{
    if (is_loaded())
    {
        VX_ERR(err::UNSUPPORTED_OPERATION) << "library already loaded";
        return false;
    }

    m_impl = std::make_shared<library_impl>();
    if (!m_impl)
    {
        return false;
    }

    if (!m_impl->load(lib))
    {
        m_impl = nullptr;
        return false;
    }

    return true;
}

VX_API bool library::is_loaded() const
{
    return m_impl && m_impl->is_loaded();
}

VX_API void library::free()
{
    if (m_impl)
    {
        m_impl->free();
        m_impl = nullptr;
    }
}

void* library::get_function_internal(const std::string& name)
{
    return is_loaded() ? m_impl->get_function(name) : nullptr;
}

} // namespace os
} // namespace vx