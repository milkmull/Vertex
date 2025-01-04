#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_FILESYSTEM)
#   include "vertex_impl/os/_platform/windows/windows_filesystem.hpp"
#else

#endif

namespace vx {
namespace os {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

VX_API directory_iterator::directory_iterator(const path& p)
    : m_impl(std::make_unique<directory_iterator_impl>(p))
{
    skip_dots();
}

VX_API typename directory_iterator::reference directory_iterator::operator*() const noexcept
{
    VX_ASSERT(m_impl);
    return m_impl->m_entry;
}

VX_API typename directory_iterator::pointer directory_iterator::operator->() const noexcept
{
    VX_ASSERT(m_impl);
    return &**this;
}

VX_API directory_iterator& directory_iterator::operator++()
{
    VX_ASSERT(m_impl);
    m_impl->advance();
    skip_dots();
    return *this;
}

void directory_iterator::skip_dots()
{
    while (m_impl->m_entry.path.filename().is_dot_or_dotdot())
    {
        m_impl->advance();
    }

    if (!m_impl->is_valid())
    {
        m_impl.reset();
    }
}

} // namespace filesystem
} // namespace os
} // namespace vx