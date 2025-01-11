#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_FILESYSTEM)
#   include "vertex_impl/os/_platform/windows/windows_filesystem.hpp"
#else

#endif

namespace vx {
namespace os {
namespace filesystem {

///////////////////////////////////////////////////////////////////////////////
// Path Operators
///////////////////////////////////////////////////////////////////////////////

VX_API path relative(const path& p, const path& base)
{
    const path p_abs = absolute(p);
    const path b_abs = absolute(base);
    return p.lexically_relative(b_abs);
}

VX_API bool equivalent(const path& p1, const path& p2)
{
    return absolute(p1) == absolute(p2);
}

///////////////////////////////////////////////////////////////////////////////
// Directory Iterator
///////////////////////////////////////////////////////////////////////////////

VX_API directory_iterator::directory_iterator(const path& p)
    : m_impl(std::make_shared<directory_iterator_impl>(p))
{
    validate();
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
    validate();
    return *this;
}

void directory_iterator::validate()
{
    if (!m_impl->is_valid())
    {
        m_impl.reset();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Recursive Directory Iterator
///////////////////////////////////////////////////////////////////////////////

VX_API recursive_directory_iterator::recursive_directory_iterator(const path& p)
    : m_impl(std::make_shared<recursive_directory_iterator_impl>(p))
{
    validate();
}

VX_API typename recursive_directory_iterator::reference recursive_directory_iterator::operator*() const noexcept
{
    VX_ASSERT(m_impl);
    return m_impl->m_entry;
}

VX_API typename recursive_directory_iterator::pointer recursive_directory_iterator::operator->() const noexcept
{
    VX_ASSERT(m_impl);
    return &**this;
}

VX_API recursive_directory_iterator& recursive_directory_iterator::operator++()
{
    VX_ASSERT(m_impl);
    m_impl->advance();
    validate();
    return *this;
}

size_t recursive_directory_iterator::depth() const
{
    VX_ASSERT(m_impl);
    return m_impl->depth();
}

bool recursive_directory_iterator::recursion_pending() const
{
    VX_ASSERT(m_impl);
    return m_impl->recursion_pending();
}

void recursive_directory_iterator::disable_pending_recursion()
{
    VX_ASSERT(m_impl);
    m_impl->disable_pending_recursion();
}

void recursive_directory_iterator::pop()
{
    VX_ASSERT(m_impl);
    if (depth() == 0)
    {
        m_impl.reset();
    }
    else
    {
        m_impl->pop();
    }
}

void recursive_directory_iterator::validate()
{
    if (!m_impl->is_valid())
    {
        m_impl.reset();
    }
}

} // namespace filesystem
} // namespace os
} // namespace vx