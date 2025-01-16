#include <algorithm>

#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_PROCESS)
#   include "vertex_impl/os/_platform/windows/windows_process.hpp"
#else

#endif

#include "vertex/system/error.hpp"

namespace vx {
namespace os {

VX_API process::process() {}

VX_API process::~process() {}

VX_API process::process(process&& other) noexcept
    : m_impl(std::move(other.m_impl))
{
    std::move(
        std::begin(other.m_streams),
        std::end(other.m_streams),
        std::begin(m_streams)
    );

    other.m_impl = nullptr;
}

VX_API process& process::operator=(process&& other) noexcept
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

VX_API bool process::start(const config& config)
{
    if (is_valid())
    {
        VX_ERR(err::UNSUPPORTED_OPERATION) << "process already started";
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

VX_API process::id process::get_pid() const
{
    if (!is_valid())
    {
        return 0;
    }

    return m_impl->get_pid();
}

VX_API bool process::is_valid() const
{
    return m_impl && m_impl->is_valid();
}

VX_API bool process::is_alive() const
{
    return is_valid() ? m_impl->is_alive() : false;
}

VX_API bool process::is_complete() const
{
    return is_valid() ? m_impl->is_complete() : false;
}

VX_API bool process::join()
{
    if (!is_valid())
    {
        return false;
    }

    return m_impl->join();
}

VX_API bool process::kill(bool force)
{
    if (!is_valid())
    {
        return false;
    }

    return m_impl->kill(force);
}

VX_API bool process::get_exit_code(int* exit_code) const
{
    if (!is_complete())
    {
        return false;
    }

    return m_impl->get_exit_code(exit_code);
}

VX_API io_stream& process::get_stdin()  { return m_streams[STDIN ]; }
VX_API io_stream& process::get_stdout() { return m_streams[STDOUT]; }
VX_API io_stream& process::get_stderr() { return m_streams[STDERR]; }

} // namespace os
} // namespace vx