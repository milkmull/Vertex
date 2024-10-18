#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_win32/win32_library.hpp"
#endif

#include "vertex/system/error.hpp"

namespace vx {
namespace os {

library::library() {}

library::~library()
{
    free();
}

library::library(const library& l)
    : m_impl(l.m_impl) {}

library::library(library&& l) noexcept
    : m_impl(std::move(l.m_impl)) {}

library& library::operator=(const library& l)
{
    m_impl = l.m_impl;
    return *this;
}

library& library::operator=(library&& l) noexcept
{
    m_impl = std::move(l.m_impl);
    return *this;
}

library::library(const std::string& name)
{
    load(name);
}

bool library::load(const std::string& lib)
{
    if (is_loaded())
    {
        VX_ERROR(error::error_code::UNSUPPORTED_OPERATION) << "library already loaded";
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

bool library::is_loaded() const
{
    return m_impl && m_impl->is_loaded();
}

void library::free()
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