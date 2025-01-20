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

    bool start(process* p, const config& config)
    {
        unsupported("start");
        return false;
    }

    id get_pid() const
    {
        unsupported("get_pid");
        return 0;
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

    bool kill(bool force)
    {
        unsupported("kill");
        return false;
    }

    bool get_exit_code(int* exit_code) const
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

inline process::id get_pid_impl()
{
    unsupported("get_pid");
    return 0;
}

inline process::environment get_environment_impl()
{
    unsupported("get_environment");
    return {};
}

inline std::string get_environment_variable_impl(const std::string& name)
{
    unsupported("get_environment_variable");
    return {};
}

inline bool set_environment_variable_impl(const std::string& name, const std::string& value)
{
    unsupported("set_environment_variable");
    return false;
}

inline bool clear_environment_variable_impl(const std::string& name)
{
    unsupported("clear_environment_variable");
    return false;
}

#undef unsupported

} // namespace this_process

} // namespace os
} // namespace vx