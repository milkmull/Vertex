#pragma once

#include <sys/types.h> // pid_t

#include "vertex/os/process.hpp"
#include "vertex_impl/os/__platform/unix/unix_tools.hpp"

namespace vx {
namespace os {

class process::process_impl
{
public:

    process_impl() noexcept {}
    ~process_impl();

public:

    bool start(process* p, const config& config);

    id get_pid() const noexcept { return static_cast<id>(m_pid); }

private:

    enum class wait_status
    {
        ALIVE,
        COMPLETE,
        FAILED
    };

    wait_status wait(bool block, int default_background_exit_code) const;

public:

    bool is_valid() const noexcept { return m_pid > 0; }
    bool is_alive() const;
    bool is_complete() const;

    bool join();
    bool kill(bool force);

    bool get_exit_code(int* exit_code) const;

private:

    pid_t m_pid = -1; // Process ID returned by fork()
    bool m_background = false;
    mutable bool m_complete = false;
    mutable int m_exit_code = 1;
};

///////////////////////////////////////////////////////////////////////////////
// this_process
///////////////////////////////////////////////////////////////////////////////

namespace this_process {

process::id get_pid_impl() noexcept;

process::environment get_environment_impl();
std::string get_environment_variable_impl(const std::string& name);
bool set_environment_variable_impl(const std::string& name, const std::string& value);
bool clear_environment_variable_impl(const std::string& name);

io_stream get_stdin_impl();
io_stream get_stdout_impl();
io_stream get_stderr_impl();

} // namespace this_process

} // namespace os
} // namespace vx