#include <sys/wait.h>
#include <unistd.h>
#include <spawn.h>
#include <dirent.h> // DIR

#include "vertex_impl/os/_platform/unix/unix_process.hpp"
#include "vertex_impl/os/_platform/unix/unix_file.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/os/time.hpp"
#include "vertex/system/error.hpp"
#include "vertex/system/assert.hpp"

#if defined(HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR_NP) && \
    !defined(HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR)
#   define HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR
#   define posix_spawn_file_actions_addchdir posix_spawn_file_actions_addchdir_np
#endif

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
    ::sigaction(sig, NULL, &action);

    // Only change if the disposition is the default
    if (!(action.sa_flags & SA_SIGINFO) && action.sa_handler == SIG_DFL)
    {
        struct sigaction new_action {};
        new_action.sa_handler = SIG_IGN;
        ::sigemptyset(&new_action.sa_mask);
        ::sigaction(sig, &new_action, NULL);
    }
}

// https://github.com/libsdl-org/SDL/blob/90fd2a3cbee5b7ead1f0517d7cc0eba1f3059207/src/process/posix/SDL_posixprocess.c#L125

static bool add_file_descriptor_close_actions(posix_spawn_file_actions_t* fa)
{
    DIR* dir = ::opendir("/proc/self/fd");
    if (dir)
    {
        struct dirent* entry;
        while ((entry = ::readdir(dir)) != nullptr)
        {
            // Skip . and ..
            if (entry->d_name[0] == '.')
            {
                continue;
            }

            // Skip stdin/out/err
            const int fd = std::atoi(entry->d_name);
            if (fd <= STDERR_FILENO)
            {
                continue;
            }

            // skip invalid or CLOEXEC
            const int flags = ::fcntl(fd, F_GETFD);
            if (flags < 0 || (flags & FD_CLOEXEC))
            {
                continue;
            }

            if (::posix_spawn_file_actions_addclose(fa, fd) != 0)
            {
                ::closedir(dir);
                unix_::error_message("posix_spawn_file_actions_addclose()");
                return false;
            }
        }

        ::closedir(dir);
    }
    else
    {
        // Fallback if /proc/self/fd is unavailable
        const long max_fd = ::sysconf(_SC_OPEN_MAX);
        for (int fd = static_cast<int>(max_fd - 1); fd > STDERR_FILENO; --fd)
        {
            // skip invalid or CLOEXEC
            const int flags = ::fcntl(fd, F_GETFD);
            if (flags < 0 || (flags & FD_CLOEXEC))
            {
                continue;
            }

            if (::posix_spawn_file_actions_addclose(fa, fd) != 0)
            {
                unix_::error_message("posix_spawn_file_actions_addclose()");
                return false;
            }
        }
    }

    return true;
}

// https://github.com/libsdl-org/SDL/blob/f79083d9bb7726b2592e31f68dc25f3a6f337320/src/process/posix/SDL_posixprocess.c

bool process::process_impl::start(process* p, const config& config)
{
    VX_ASSERT_MESSAGE(!is_valid(), "process already configured");

    // Marked volatile to prevent clobbering during vfork; the child shares the parent's memory,
    // so compilers may optimize out or reorder accesses unsafely if not volatile.
    volatile bool success = false;

    // Base class should check that args is not empty
    VX_ASSERT(!config.args.empty());

    // Setup args
    // We must allocate this on the heap because it will be used in the child process
    const size_t arg_count = config.args.size();
    std::string* arg_strings = new std::string[arg_count + 1];
    char** args = new char* [arg_count + 1];

    {
        // Prepare a null-terminated array of C-strings
        for (size_t i = 0; i < arg_count; ++i)
        {
            arg_strings[i] = config.args[i];
            args[i] = const_cast<char*>(arg_strings[i].c_str());
        }

        // null-terminate!
        args[arg_count] = NULL;
    }

    // Setup environment
    char** env = nullptr;
    std::string* env_strings = nullptr;

    if (config.environment.empty())
    {
        // Inherit the parent's evirnonment by default
        env = environ;
    }
    else
    {
        // We must allocate this on the heap because it will be used in the child process
        const size_t env_count = config.environment.size();
        env_strings = new std::string[env_count + 1];
        env = new char* [env_count + 1];

        // Build a vector of "KEY=VALUE" strings
        size_t i = 0;
        for (const auto& pair : config.environment)
        {
            env_strings[i] = pair.first + '=' + pair.second;
            env[i] = const_cast<char*>(env_strings[i].c_str());
        }

        // null-terminate!
        env[env_count] = NULL;
    }

    enum : int
    {
        read_pipe_index = 0,
        write_pipe_index = 1
    };

    struct stream_data
    {
        int type;
        io_option option;
        int pipes[2];
        file* redirect;

        int& read_pipe() noexcept { return pipes[read_pipe_index]; }
        int& write_pipe() noexcept { return pipes[write_pipe_index]; }

        int user_pipe_index() const noexcept { return (type == STDIN_FILENO) ? write_pipe_index : read_pipe_index; }
        int proc_pipe_index() const noexcept { return (type == STDIN_FILENO) ? read_pipe_index : write_pipe_index; }

        int& user_pipe() noexcept { return pipes[user_pipe_index()]; }
        int& proc_pipe() noexcept { return pipes[proc_pipe_index()]; }

        file::mode user_file_mode() const noexcept { return (type == STDIN_FILENO) ? file::mode::write : file::mode::read; }
        file::mode proc_file_mode() const noexcept { return (type == STDIN_FILENO) ? file::mode::read : file::mode::write; }

        int proc_posix_file_permissions() const noexcept { return (type == STDIN_FILENO) ? O_RDONLY : O_WRONLY; }
        mode_t proc_posix_file_mode() const noexcept { return (type == STDIN_FILENO) ? 0 : 0644; }
    };

    stream_data streams[stream_count] = {
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

    if (::posix_spawnattr_init(&attr) != 0)
    {
        unix_::error_message("posix_spawnattr_init()");
        goto posix_spawn_fail_none;
    }

    if (::posix_spawn_file_actions_init(&fa) != 0)
    {
        unix_::error_message("posix_spawn_file_actions_init()");
        goto posix_spawn_fail_attr;
    }

    if (!config.working_directory.empty())
    {
#if defined(HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR)

        if (::posix_spawn_file_actions_addchdir(&fa, config.working_directory.c_str()) != 0)
        {
            unix_::error_message("posix_spawn_file_actions_addchdir()");
            goto posix_spawn_fail_all;
        }

#else

        err::set(err::system_error, "process::start(): setting the working directory not supported");
        goto posix_spawn_fail_all;

#endif
    }

    for (int i = 0; i < stream_count; ++i)
    {
        stream_data& stream = streams[i];

        if (config.background && stream.option == io_option::inherit)
        {
            // If running in the background, redirect all streams to null
            stream.option = io_option::none;
        }

        switch (stream.option)
        {
            case io_option::none:
            {
                if (::posix_spawn_file_actions_addopen(
                    &fa,
                    stream.type,
                    "/dev/null",
                    stream.proc_posix_file_permissions(),
                    stream.proc_posix_file_mode()
                ) != 0)
                {
                    unix_::error_message("posix_spawn_file_actions_addopen()");
                    goto posix_spawn_fail_all;
                }

                break;
            }
            case io_option::create:
            {
                if (::pipe(stream.pipes) < 0)
                {
                    unix_::error_message("pipe()");
                    goto posix_spawn_fail_all;
                }

                // Make sure the pipe isn't accidentally inherited by another thread creating a process
                ::fcntl(stream.read_pipe(), F_SETFD, ::fcntl(stream.read_pipe(), F_GETFD) | FD_CLOEXEC);
                ::fcntl(stream.write_pipe(), F_SETFD, ::fcntl(stream.write_pipe(), F_GETFD) | FD_CLOEXEC);

                // Make sure we don't crash if we write when the pipe is closed
                ignore_signal(SIGPIPE);

                if (::posix_spawn_file_actions_adddup2(&fa, stream.proc_pipe(), stream.type) != 0)
                {
                    unix_::error_message("posix_spawn_file_actions_adddup2()");
                    goto posix_spawn_fail_all;
                }

                break;
            }
            case io_option::redirect:
            {
                if (!stream.redirect || !stream.redirect->is_open()) // no write?
                {
                    err::set(err::invalid_argument, "process::start(): redirect stream was null");
                    goto posix_spawn_fail_all;
                }

                if ((stream.proc_file_mode() == file::mode::read && !stream.redirect->can_read()) ||
                    (stream.proc_file_mode() == file::mode::write && !stream.redirect->can_write()))
                {
                    err::set(err::invalid_argument, "process::start(): redirect stream mode is incompatable with expected file mode");
                    goto posix_spawn_fail_all;
                }

                const int fd = _priv::file_impl::get_native_handle(*stream.redirect);
                if (::posix_spawn_file_actions_adddup2(&fa, fd, stream.type) != 0)
                {
                    unix_::error_message("posix_spawn_file_actions_adddup2()");
                    goto posix_spawn_fail_all;
                }

                break;
            }
            case io_option::inherit:
            default:
            {
                // Default behaviour
                break;
            }
        }
    }

    if (!add_file_descriptor_close_actions(&fa))
    {
        err::set(err::system_error, "process::start(): failed to add file descriptor close actions");
        goto posix_spawn_fail_all;
    }

    // Spawn the new process
    if (config.background)
    {
        int status = -1;

#if defined(VX_OS_APPLE)

        // Apple has vfork marked as deprecated and (as of macOS 10.12) is almost identical to calling fork() anyhow.
#       define fork_func_name "fork()"
        const pid_t pid = ::fork();

#else

#       define fork_func_name "vfork()"
        const pid_t pid = ::vfork();

#endif

        switch (pid)
        {
            case -1:
            {
                unix_::error_message(fork_func_name);
                goto posix_spawn_fail_all;
            }
            case 0:
            {
                // Detach from the terminal and launch the process, then exit
                ::setsid();
                if (::posix_spawnp(&m_pid, args[0], &fa, &attr, args, env) != 0)
                {
                    ::_exit(errno);
                }
                ::_exit(0);
            }
            default:
            {
                if (::waitpid(pid, &status, 0) < 0)
                {
                    unix_::error_message("waitpid()");
                    goto posix_spawn_fail_all;
                }
                if (status != 0)
                {
                    unix_::error_message("posix_spawnp()");
                    goto posix_spawn_fail_all;
                }
                break;
            }
        }
    }
    else
    {
        if (::posix_spawnp(&m_pid, args[0], &fa, &attr, args, env) != 0)
        {
            unix_::error_message("posix_spawnp()");
            goto posix_spawn_fail_all;
        }
    }

    // Setup user end of created streams
    for (int i = 0; i < stream_count; ++i)
    {
        if (streams[i].option == io_option::create)
        {
            // Set the file descriptor to non-blocking mode
            const int fd = streams[i].user_pipe();
            ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) | O_NONBLOCK);

            // Create file object from handle for the corresponding stream
            p->m_streams[i] = _priv::file_impl::from_native_handle(
                fd,
                streams[i].user_file_mode()
            );

            if (!p->m_streams[i].is_open())
            {
                goto posix_spawn_fail_all;
            }
        }
    }

    m_background = config.background;
    success = true;

    ///////////////////////////////////////////////////////////////////////////////

    posix_spawn_fail_all:
    {
        ::posix_spawn_file_actions_destroy(&fa);
    }

    posix_spawn_fail_attr:
    {
        ::posix_spawnattr_destroy(&attr);
    }

    posix_spawn_fail_none:
    {
        for (stream_data& s : streams)
        {
            // Only close proc_pipe() if:
            // - It was created (CREATE or NONE), not redirected or inherited
            // - We succeeded and the child has inherited it already
            // - OR we failed to create the process, in which case everything must be cleaned up
            const bool created_pipe = (s.option == io_option::create || s.option == io_option::none);

            if (s.proc_pipe() != -1)
            {
                if (!success || created_pipe)
                {
                    ::close(s.proc_pipe());
                }
            }

            // If we failed, we also need to close user_pipe (the parent-facing end of the pipe)
            if (!success && s.user_pipe() != -1)
            {
                ::close(s.user_pipe());
            }
        }
    }

    // Cleanup allocated memory
    {
        delete[] args;
        delete[] arg_strings;

        if (env_strings)
        {
            delete[] env;
            delete[] env_strings;
        }
    }

    return success;
}

#define assert_process_configured() VX_ASSERT_MESSAGE(is_valid(), "process not configured")

process::process_impl::wait_status process::process_impl::wait(
    bool block, int default_background_exit_code
) const
{
    if (m_complete)
    {
        return wait_status::complete;
    }

    if (m_background)
    {
        // We can't wait on the status, so we'll poll to see if it's alive
        while (::kill(m_pid, 0) == 0)
        {
            if (!block)
            {
                // just check once
                return wait_status::alive;
            }

            os::sleep(time::milliseconds(10));
        }

        // Process is no longer alive, but we can't retrieve exit status
        m_complete = true;
        m_exit_code = default_background_exit_code;
        return wait_status::complete;
    }

    int status = 0;
    const pid_t res = ::waitpid(m_pid, &status, block ? 0 : WNOHANG);

    if (res < 0)
    {
        unix_::error_message("waitpid()");
        return wait_status::failed;
    }
    else if (res == 0)
    {
        return wait_status::alive;
    }
    else // (res > 0)
    {
        m_complete = true;

        if (WIFEXITED(status))
        {
            m_exit_code = WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status))
        {
            m_exit_code = 128 + WTERMSIG(status);
        }
        else
        {
            m_exit_code = 1;
        }

        return wait_status::complete;
    }
}

bool process::process_impl::is_alive() const
{
    assert_process_configured();
    return wait(false, 0) == wait_status::alive;
}

bool process::process_impl::is_complete() const
{
    assert_process_configured();
    return wait(false, 0) == wait_status::complete;
}

bool process::process_impl::join()
{
    assert_process_configured();
    return wait(true, 0) == wait_status::complete;
}

bool process::process_impl::kill(bool force)
{
    assert_process_configured();

    if (::kill(m_pid, force ? SIGKILL : SIGTERM) != 0)
    {
        unix_::error_message("kill()");
        return false;
    }

    if (m_background)
    {
        // If we are running in the background, just check if the
        // process is still alive. If it is, the signal failed. If
        // it is not, consider it an exit failure (code 1).
        return (wait(true, 1) == wait_status::complete);
    }

    return join();
}

bool process::process_impl::get_exit_code(int* exit_code) const
{
    assert_process_configured();
    VX_ASSERT(exit_code != nullptr); // checked in parent

    // The owner class calls is_complete() before this,
    // so if we get here, m_complete must be true. We 
    // can just set the exit code.
    VX_ASSERT(m_complete);

    *exit_code = m_exit_code;
    return true;
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

    return _priv::file_impl::from_native_handle(
        dup_fd,
        (fd == STDIN_FILENO) ? io_stream::mode::read : io_stream::mode::write
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