#include <sys/wait.h>
#include <unistd.h>

#include "vertex_impl/os/__platform/unix/unix_process.hpp"
#include "vertex_impl/os/__platform/unix/unix_file.hpp"
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

static void ignore_signal(int sig)
{
    struct sigaction action {};

    sigaction(SIGPIPE, NULL, &action);

    if (action.sa_handler == SIG_DFL
#if defined(HAVE_SA_SIGACTION)
        && (void (*)(int))action.sa_sigaction == SIG_DFL
#endif
        )
    {
        action.sa_handler = SIG_IGN;
        sigaction(sig, &action, NULL);
    }
}

static bool create_pipe(int fds[2])
{
    if (pipe(fds) < 0)
    {
        unix_::error_message("pipe()");
        return false;
    }

    // Make sure the pipe isn't accidentally inherited by another thread creating a process
    fcntl(fds[READ_END], F_SETFD, fcntl(fds[READ_END], F_GETFD) | FD_CLOEXEC);
    fcntl(fds[WRITE_END], F_SETFD, fcntl(fds[WRITE_END], F_GETFD) | FD_CLOEXEC);

    // Make sure we don't crash if we write when the pipe is closed
    ignore_signal(SIGPIPE);
    return true;
}

// https://github.com/libsdl-org/SDL/blob/f79083d9bb7726b2592e31f68dc25f3a6f337320/src/process/posix/SDL_posixprocess.c

bool process::process_impl::start(process* p, const config& config)
{
    VX_ASSERT_MESSAGE(!is_valid(), "process already configured");

    enum : int
    {
        READ_PIPE = 0,
        WRITE_PIPE = 1
    };

    struct stream_data
    {
        int type;
        io_option option;
        int pipes[2];
        file* redirect;

        int& read_pipe() { return pipes[READ_PIPE]; }
        int& write_pipe() { return pipes[WRITE_PIPE]; }

        int user_pipe_index() const { return (type == STDIN_FILENO) ? WRITE_PIPE : READ_PIPE; }
        int proc_pipe_index() const { return (type == STDIN_FILENO) ? READ_PIPE : WRITE_PIPE; }

        int& user_pipe() { return pipes[user_pipe_index()]; }
        int& proc_pipe() { return pipes[proc_pipe_index()]; }

        file::mode user_file_mode() const { return (type == STDIN_FILENO) ? file::mode::WRITE : file::mode::READ; }
        file::mode proc_file_mode() const { return (type == STDIN_FILENO) ? file::mode::READ : file::mode::WRITE; }

        int proc_posix_file_mode() const { return (type == STDIN_FILENO) ? O_RDONLY : O_WRONLY; }
        int proc_posix_file_permissions() const { return (type == STDIN_FILENO) ? 0 : 0644; }
    };

    stream_data streams[STREAM_COUNT] = {
        // stdin
        {
            STDIN_FILENO,
            config.stdin_option,
            { -1, -1 },
            config.stdin_redirect
        },
        // stdout
        {
            STDOUT_FILENO,
            config.stdout_option,
            { -1, -1 },
            config.stdout_redirect
        },
        // stderr
        {
            STDERR_FILENO,
            config.stderr_option,
            { -1, -1 },
            config.stderr_redirect
        }
    };

    // posix_spawn data
    posix_spawnattr_t attr;
    posix_spawn_file_actions_t fa;

    if (posix_spawnattr_init(&attr) != 0)
    {
        unix_::error_message("posix_spawnattr_init()");
        goto cleanup;
    }

    if (posix_spawn_file_actions_init(&fa) != 0)
    {
        unix_::error_message("posix_spawn_file_actions_init()");
        goto cleanup;
    }

    if (!config.working_directory.empty())
    {
#if defined(HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR)

        if (posix_spawn_file_actions_addchdir(&fa, working_directory.c_str()) != 0)
        {
            unix_::error_message("posix_spawn_file_actions_addchdir()");
            goto cleanup;
        }

#else

        err::set(err::SYSTEM_ERROR, "process::start(): setting the working directory not supported");
        goto cleanup;

#endif
    }

    // If running in the background, redirect all streams to null
    if (config.background)
    {
        for (stream_data& s : streams)
        {
            if (s.option == io_option::INHERIT)
            {
                s.option = io_option::NONE;
            }
        }
    }

    for (int i = 0; i < STREAM_COUNT; ++i)
    {
        stream_data& stream = streams[i];

        switch (stream.option)
        {
            case io_option::NONE:
            {
                if (posix_spawn_file_actions_addopen(
                    &fa,
                    stream.type,
                    "/dev/null",
                    stream.proc_posix_file_mode(),
                    stream.proc_posix_file_permissions()
                ) != 0)
                {
                    unix_::error_message("posix_spawn_file_actions_addopen()");
                    goto cleanup;
                }
            }
            case io_option::CREATE:
            {
                if (!create_pipe(stream.pipes))
                {
                    goto cleanup;
                }

                if (posix_spawn_file_actions_adddup2(&fa, stream.proc_pipe(), stream.type) != 0)
                {
                    unix_::error_message("posix_spawn_file_actions_adddup2()");
                    goto cleanup;
                }

                break;
            }
            case io_option::REDIRECT:
            {
                if (!stream.redirect || !stream.redirect->is_open()) // no write?
                {
                    err::set(err::INVALID_ARGUMENT, "process::start(): redirect stream was null");
                    goto cleanup;
                }

                if ((stream.proc_file_mode() == file::mode::READ && !stream.redirect->can_read()) ||
                    (stream.proc_file_mode() == file::mode::WRITE && !stream.redirect->can_write()))
                {
                    err::set(err::INVALID_ARGUMENT, "process::start(): redirect stream mode is incompatable with expected file mode");
                    goto cleanup;
                }

                const int fd = __detail::file_impl::get_native_handle(*stream.redirect);
                if (posix_spawn_file_actions_adddup2(&fa, fd, stream.type) != 0)
                {
                    unix_::error_message("posix_spawn_file_actions_adddup2()");
                    goto cleanup;
                }

                break;
            }
            case io_option::INHERIT:
            default:
            {
                // Default behaviour
                break;
            }
        }
    }

    if (!AddFileDescriptorCloseActions(&fa))
    {
        unix_::error_message("AddFileDescriptorCloseActions()");
        goto cleanup;
    }

    success = true;

    cleanup:
    {

    }

    return success;
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
    process::environment environment;

    char** strings = environ;
    for (char** current = strings; *current; ++current)
    {
        std::string entry(*current);
        size_t pos = entry.find('=');
        if (pos != std::string::npos)
        {
            std::string name = entry.substr(0, pos);
            std::string value = entry.substr(pos + 1);
            environment.emplace(std::move(name), std::move(value));
        }
    }

    return environment;
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