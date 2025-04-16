#include "vertex_impl/os/__platform/windows/windows_process.hpp"
#include "vertex_impl/os/__platform/windows/windows_file.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/system/error.hpp"
#include "vertex/system/assert.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// process
///////////////////////////////////////////////////////////////////////////////

process::process_impl::~process_impl()
{

}

// https://github.com/libsdl-org/SDL/blob/f79083d9bb7726b2592e31f68dc25f3a6f337320/src/process/posix/SDL_posixprocess.c

// https://github.com/libsdl-org/SDL/blob/f79083d9bb7726b2592e31f68dc25f3a6f337320/src/process/windows/SDL_windowsprocess.c#L93

bool process::process_impl::start(process* p, const config& config)
{
    VX_ASSERT_MESSAGE(!is_valid(), "process already configured");
    return false;
}

#define assert_process_configured() VX_ASSERT_MESSAGE(is_valid(), "process not configured")

bool process::process_impl::is_alive() const
{
    assert_process_configured();

    int status;
    const pid_t result = ::waitpid(m_pid, &status, WNOHANG);

    if (result == 0)
    {
        // Still running
        return true;
    }
    else if (result == m_pid)
    {
        // Finished
        return false;
    }
    else
    {
        unix_::error_message("waitpid(WNOHANG)");
        return false;
    }
}

bool process::process_impl::is_complete() const
{
    assert_process_configured();

    int status;
    pid_t result = ::waitpid(m_pid, &status, WNOHANG);

    if (result == -1)
    {
        unix_::error_message("waitpid(WNOHANG)");
        return false;
    }

    return (result == m_pid);
}

bool process::process_impl::join()
{
    assert_process_configured();

    int status;
    pid_t result = ::waitpid(m_pid, &status, 0);

    if (result == -1)
    {
        unix_::error_message("waitpid(0)");
        return false;
    }

    return true;
}

bool process::process_impl::kill(bool force)
{
    assert_process_configured();

    const int signal = force ? SIGKILL : SIGTERM;
    if (::kill(m_pid, signal) != 0)
    {
        unix_::error_message("kill()");
        return false;
    }

    return join();
}

bool process::process_impl::get_exit_code(int* exit_code) const
{
    assert_process_configured();

    int status;
    pid_t result = ::waitpid(m_pid, &status, WNOHANG);

    if (result == -1)
    {
        unix_::error_message("waitpid(WNOHANG)");
        return false;
    }

    if (exit_code && result == m_pid)
    {
        if (WIFEXITED(status))
        {
            *exit_code = WEXITSTATUS(status);
            return true;
        }
        else if (WIFSIGNALED(status))
        {
            *exit_code = 128 + WTERMSIG(status); // match shell convention
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// this_process
///////////////////////////////////////////////////////////////////////////////

process::id this_process::get_pid_impl() noexcept
{
    return static_cast<process::id>(::getpid());
}

process::environment this_process::get_environment_impl()
{
    return {};
}

std::string this_process::get_environment_variable_impl(const std::string& name)
{
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : std::string{};
}

bool this_process::set_environment_variable_impl(const std::string& name, const std::string& value)
{
    if (::setenv(name.c_str(), value.c_str(), 1) != 0)
    {
        unix_::error_message("setenv()");
        return false;
    }
    return true;
}

bool this_process::clear_environment_variable_impl(const std::string& name)
{
    if (::unsetenv(name.c_str()) != 0)
    {
        unix_::error_message("unsetenv()");
        return false;
    }
    return true;
}

static io_stream get_stream_handle(int fd)
{
    // Duplicate the file descriptor to ensure it's independent (optional)
    int dup_fd = ::dup(fd);
    if (dup_fd == -1)
    {
        unix_::error_message("dup()");
    }

    return __detail::file_impl::from_native_handle(
        dup_fd,
        (fd == STDIN_FILENO) ? io_stream::mode::READ : io_stream::mode::WRITE
    );
}

io_stream this_process::get_stdin_impl()
{
    return get_stream_handle(STDIN_FILENO);
}

io_stream this_process::get_stdout_impl()
{
    return get_stream_handle(STDOUT_FILENO);
}

io_stream this_process::get_stderr_impl()
{
    return get_stream_handle(STDERR_FILENO);
}

} // namespace os
} // namespace vx