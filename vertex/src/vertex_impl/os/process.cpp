#include "vertex_impl/os/__platform/process.hpp"
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

VX_API bool process::start(const config& cfg)
{
    if (is_valid())
    {
        err::set(err::UNSUPPORTED_OPERATION, "process already started");
        return false;
    }

    if (cfg.args.empty())
    {
        err::set(err::INVALID_ARGUMENT, "no arguments were specified");
        return false;
    }

    m_impl = std::make_unique<process_impl>();
    if (!m_impl)
    {
        m_impl = nullptr;
        return false;
    }

    if (!m_impl->start(this, cfg))
    {
        m_impl = nullptr;
        return false;
    }

    return true;
}

VX_API process::id process::get_pid() const
{
    return is_valid() ? m_impl->get_pid() : 0;
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
    return is_valid() ? m_impl->join() : false;
}

VX_API bool process::kill(bool force)
{
    return is_valid() ? m_impl->kill(force) : false;
}

VX_API bool process::get_exit_code(int* exit_code) const
{
    return (exit_code && is_complete()) ? m_impl->get_exit_code(exit_code) : false;
}

VX_API io_stream& process::get_stdin()  { return m_streams[STDIN ]; }
VX_API io_stream& process::get_stdout() { return m_streams[STDOUT]; }
VX_API io_stream& process::get_stderr() { return m_streams[STDERR]; }

///////////////////////////////////////////////////////////////////////////////
// this_process
///////////////////////////////////////////////////////////////////////////////

namespace this_process {

VX_API process::id get_pid()
{
    return get_pid_impl();
}

VX_API process::environment get_environment()
{
    return get_environment_impl();
}

VX_API std::string get_environment_variable(const std::string& name)
{
    return get_environment_variable_impl(name);
}

VX_API bool set_environment_variable(const std::string& name, const std::string& value)
{
    return set_environment_variable_impl(name, value);
}

VX_API bool clear_environment_variable(const std::string& name)
{
    return clear_environment_variable_impl(name);
}

VX_API io_stream get_stdin()
{
    return get_stdin_impl();
}

VX_API io_stream get_stdout()
{
    return get_stdout_impl();
}

VX_API io_stream get_stderr()
{
    return get_stderr_impl();
}

} // namespace this_process

} // namespace os
} // namespace vx