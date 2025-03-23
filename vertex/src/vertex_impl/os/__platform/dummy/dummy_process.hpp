#pragma once

#include "vertex/os/process.hpp"

namespace vx {
namespace os {

#define unsupported(op) VX_UNSUPPORTED("os::process::" op "()")

class process::process_impl
{
public:

    process_impl() = default;
    ~process_impl() = default;

public:

    bool start(process*, const config&)
    {
        unsupported("start");
        return false;
    }

    id get_pid() const
    {
        unsupported("get_pid");
        return 0;
    }

    environment get_environment() const
    {
        unsupported("get_environment");
        return {};
    }

    bool is_valid() const
    {
        unsupported("is_valid");
        return false;
    }

    bool is_alive() const
    {
        unsupported("is_alive");
        return false;
    }

    bool is_complete() const
    {
        unsupported("is_complete");
        return false;
    }

    bool join()
    {
        unsupported("join");
        return false;
    }

    bool kill(bool)
    {
        unsupported("kill");
        return false;
    }

    bool get_exit_code(int*) const
    {
        unsupported("get_exit_code");
        return false;
    }
};

#undef unsupported

///////////////////////////////////////////////////////////////////////////////
// this_process
///////////////////////////////////////////////////////////////////////////////

namespace this_process {

#define unsupported(op) VX_UNSUPPORTED("os::this_process::" op "()")

static process::id get_pid_impl()
{
    unsupported("get_pid");
    return 0;
}

static process::environment get_environment_impl()
{
    unsupported("get_environment");
    return {};
}

static std::string get_environment_variable_impl(const std::string&)
{
    unsupported("get_environment_variable");
    return {};
}

static bool set_environment_variable_impl(const std::string&, const std::string&)
{
    unsupported("set_environment_variable");
    return false;
}

static bool clear_environment_variable_impl(const std::string&)
{
    unsupported("clear_environment_variable");
    return false;
}

static io_stream get_stdin_impl()
{
    unsupported("get_stdin");
    return {};
}

static io_stream get_stdout_impl()
{
    unsupported("get_stdout");
    return {};
}

static io_stream get_stderr_impl()
{
    unsupported("get_stderr");
    return {};
}

#undef unsupported

} // namespace this_process

} // namespace os
} // namespace vx