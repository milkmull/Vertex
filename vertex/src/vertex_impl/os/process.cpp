#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "_win32/win32_process.hpp"
#else

#endif

#include <algorithm>

namespace vx {
namespace os {

process::process() {}

process::~process() {}

process::process(process&& other) noexcept
    : m_impl(std::move(other.m_impl))
{
    std::move(
        std::begin(other.m_streams),
        std::end(other.m_streams),
        std::begin(m_streams)
    );

    other.m_impl = nullptr;
}

process& process::operator=(process&& other) noexcept
{
    if (this != &other)
    {
        m_impl = std::move(other.m_impl);
        std::move(
            std::begin(other.m_streams),
            std::end(other.m_streams),
            std::begin(m_streams)
        );

        other.m_impl = nullptr;
    }

    return *this;
}

void swap(process& lhs, process& rhs) noexcept
{
    std::swap(lhs.m_impl, rhs.m_impl);
    std::swap_ranges(
        std::begin(rhs.m_streams),
        std::end(rhs.m_streams),
        std::begin(lhs.m_streams)
    );
}

bool process::start(const config& config)
{
    if (is_valid())
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

process::id process::get_pid() const
{
    if (!is_valid())
    {
        return 0;
    }

    return m_impl->get_pid();
}

bool process::is_valid() const
{
    return m_impl && m_impl->is_valid();
}

bool process::is_alive() const
{
    return is_valid() ? m_impl->is_alive() : false;
}

bool process::is_complete() const
{
    return is_valid() ? m_impl->is_complete() : false;
}

bool process::join()
{
    if (!is_valid())
    {
        return false;
    }

    return m_impl->join();
}

bool process::kill(bool force)
{
    if (!is_valid())
    {
        return false;
    }

    return m_impl->kill(force);
}

bool process::get_exit_code(int* exit_code) const
{
    if (!is_complete())
    {
        return false;
    }

    return m_impl->get_exit_code(exit_code);
}

file& process::get_stdin()  { return m_streams[STDIN ]; }
file& process::get_stdout() { return m_streams[STDOUT]; }
file& process::get_stderr() { return m_streams[STDERR]; }

} // namespace os
} // namespace vx