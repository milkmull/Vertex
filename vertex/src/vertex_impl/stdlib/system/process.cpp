#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "_win32/win32_process.hpp"
#else

#endif

namespace vx {
namespace proc {

process::process()
    : m_pid(0) {}

process::~process() {}

process::process(process&& p) noexcept
    : m_pid(p.m_pid)
    , m_impl(std::move(p.m_impl)) {}

process& process::operator=(process&& p) noexcept
{
    m_pid = p.m_pid;
    m_impl = std::move(p.m_impl);
    return *this;
}

bool process::start(const process_config& config)
{
    m_impl = std::make_unique<process_impl>();
    if (!m_impl)
    {
        m_impl = nullptr;
        return false;
    }

    if (!m_impl->start(this, config))
    {
        m_impl = nullptr;
        return false;
    }

    return true;
}

bool process::is_valid() const
{
    return m_impl != nullptr;
}

} // namespace proc
} // namespace vx