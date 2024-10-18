#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "_win32/win32_process.hpp"
#else

#endif

namespace vx {
namespace os {

static void process_not_initialized()
{
    VX_ERROR(error::error_code::UNSUPPORTED_OPERATION) << "process not initialized";
}

process::process() {}

process::~process() {}

process::process(process&& p) noexcept
    : m_impl(std::move(p.m_impl)) {}

process& process::operator=(process&& p) noexcept
{
    m_impl = std::move(p.m_impl);
    return *this;
}

bool process::start(const config& config)
{
    if (is_initialized())
    {
        VX_ERROR(error::error_code::UNSUPPORTED_OPERATION) << "process already started";
        return false;
    }

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

bool process::is_initialized() const
{
    return m_impl && m_impl ->is_initialized();
}

process_id process::get_pid() const
{
    if (!is_initialized())
    {
        process_not_initialized();
        return 0;
    }

    return m_impl->get_pid();
}

bool process::is_alive() const
{
    return is_initialized() ? m_impl->is_alive() : false;
}

bool process::kill(bool force)
{
    if (!is_initialized())
    {
        process_not_initialized();
        return false;
    }

    return m_impl->kill(force);
}

bool process::wait(bool block, int* exit_code)
{
    if (!is_initialized())
    {
        process_not_initialized();
        return false;
    }

    return m_impl->wait(block, exit_code);
}

file& process::get_stdin()  { return m_streams[STDIN ]; }
file& process::get_stdout() { return m_streams[STDOUT]; }
file& process::get_stderr() { return m_streams[STDERR]; }

} // namespace os
} // namespace vx