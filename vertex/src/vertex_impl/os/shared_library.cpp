#include <unordered_map>

#include "vertex/system/error.hpp"

#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_shared_library.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_shared_library.hpp"
#endif

namespace vx {
namespace os {

VX_API shared_library::shared_library() {}

shared_library::~shared_library()
{
    free();
}

VX_API shared_library::shared_library(const shared_library& l)
    : m_impl(l.m_impl) {}

VX_API shared_library::shared_library(shared_library&& l) noexcept
    : m_impl(std::move(l.m_impl)) {}

VX_API shared_library& shared_library::operator=(const shared_library& l)
{
    m_impl = l.m_impl;
    return *this;
}

VX_API shared_library& shared_library::operator=(shared_library&& l) noexcept
{
    m_impl = std::move(l.m_impl);
    return *this;
}

VX_API shared_library::shared_library(const std::string& name)
{
    load(name);
}

VX_API bool shared_library::load(const std::string& lib)
{
    if (is_loaded())
    {
        VX_ERR(err::UNSUPPORTED_OPERATION) << "shared_library already loaded";
        return false;
    }

    m_impl = std::make_shared<shared_library_impl>();
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

VX_API bool shared_library::is_loaded() const
{
    return m_impl && m_impl->is_loaded();
}

VX_API void shared_library::free()
{
    if (m_impl)
    {
        m_impl->free();
        m_impl = nullptr;
    }
}

void* shared_library::get_function_internal(const std::string& name)
{
    return is_loaded() ? m_impl->get_function(name) : nullptr;
}

} // namespace os
} // namespace vx