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

process::process_impl::process_impl()
{
    m_process_information.hProcess = INVALID_HANDLE_VALUE;
    m_process_information.hThread = INVALID_HANDLE_VALUE;
}

process::process_impl::~process_impl()
{
    if (m_process_information.hProcess != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_process_information.hProcess);
        m_process_information.hProcess = INVALID_HANDLE_VALUE;
    }
    if (m_process_information.hThread != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_process_information.hThread);
        m_process_information.hThread = INVALID_HANDLE_VALUE;
    }
}

// https://github.com/libsdl-org/SDL/blob/f79083d9bb7726b2592e31f68dc25f3a6f337320/src/process/windows/SDL_windowsprocess.c#L93

static bool is_batch_file_path(const std::string& arg)
{
    const size_t size = arg.size();

    if (size < 4 || arg[size - 4] != '.')
    {
        return false;
    }

    return (arg.compare(size - 3, 3, "bat") == 0 || arg.compare(size - 3, 3, "BAT") == 0)
        || (arg.compare(size - 3, 3, "cmd") == 0 || arg.compare(size - 3, 3, "CMD") == 0);
}

static std::string join_arguments(const std::vector<std::string>& args)
{
    VX_ASSERT(!args.empty());

    const size_t count = args.size();
    const bool is_batch_file = is_batch_file_path(args.front());

    std::vector<bool> need_quotes(count, false);
    for (size_t i = 0; i < count; ++i)
    {
        if ((args[i].find_first_of(" \t\"()=!") != std::string::npos) ||
            (is_batch_file && args[i].find_first_of("^&|<>%") != std::string::npos))
        {
            need_quotes[i] = true;
        }
    }

    std::string out;

    for (size_t i = 0; i < count; ++i)
    {
        if (need_quotes[i])
        {
            out.push_back('"');
        }

        bool last_was_back_slash = false;
        for (const char c : args[i])
        {
            if (last_was_back_slash && c == '"')
            {
                // Only escape backslashes that precede a double quote
                out.push_back('\\');
            }

            last_was_back_slash = (c == '\\');
            out.push_back(c);
        }

        if (need_quotes[i])
        {
            if (last_was_back_slash)
            {
                // Only escape backslashes that precede a double quote
                out.push_back('\\');
            }

            out.push_back('"');
        }

        if (i < count - 1)
        {
            out.push_back(' ');
        }
    }

    return out;
}

static std::string join_environment(const process::environment& environment)
{
    std::string out;

    for (const auto& pair : environment)
    {
        out.append(pair.first);
        out.push_back('=');
        out.append(pair.second);
        out.push_back('\0');
    }
    out.push_back('\0');

    return out;
}

// https://github.com/libsdl-org/SDL/blob/f79083d9bb7726b2592e31f68dc25f3a6f337320/src/process/windows/SDL_windowsprocess.c#L93

bool process::process_impl::start(process* p, const config& config)
{
    VX_ASSERT_MESSAGE(!is_valid(), "process already configured");

    m_process_information.hProcess = INVALID_HANDLE_VALUE;
    m_process_information.hThread = INVALID_HANDLE_VALUE;

    bool success = false;
    DWORD creation_flags = CREATE_UNICODE_ENVIRONMENT;

    STARTUPINFOW startup_info{ sizeof(STARTUPINFOW) };
    startup_info.dwFlags |= STARTF_USESTDHANDLES;
    startup_info.hStdInput = INVALID_HANDLE_VALUE;
    startup_info.hStdOutput = INVALID_HANDLE_VALUE;
    startup_info.hStdError = INVALID_HANDLE_VALUE;

    SECURITY_ATTRIBUTES security_attributes{ sizeof(SECURITY_ATTRIBUTES) };
    security_attributes.bInheritHandle = TRUE;
    security_attributes.lpSecurityDescriptor = NULL;

    std::wstring wargs;
    LPWSTR command_line = NULL;

    if (!config.args.empty())
    {
        wargs = str::string_cast<wchar_t>(join_arguments(config.args));
        command_line = wargs.data();
    }

    std::wstring wenv;
    LPVOID environment = NULL;

    if (!config.environment.empty())
    {
        wenv = str::string_cast<wchar_t>(join_environment(config.environment));
        environment = wenv.data();
    }

    enum : int
    {
        READ_PIPE  = 0,
        WRITE_PIPE = 1
    };

    struct stream_data
    {
        DWORD type;
        io_option option;
        HANDLE pipes[2];
        file* redirect;

        HANDLE& read_pipe() { return pipes[READ_PIPE]; }
        HANDLE& write_pipe() { return pipes[WRITE_PIPE]; }

        int user_pipe_index() const { return (type == STD_INPUT_HANDLE) ? WRITE_PIPE : READ_PIPE; }
        int proc_pipe_index() const { return (type == STD_INPUT_HANDLE) ? READ_PIPE : WRITE_PIPE; }

        HANDLE& user_pipe() { return pipes[user_pipe_index()]; }
        HANDLE& proc_pipe() { return pipes[proc_pipe_index()]; }

        file::mode user_file_mode() const { return (type == STD_INPUT_HANDLE) ? file::mode::WRITE : file::mode::READ; }
        file::mode proc_file_mode() const { return (type == STD_INPUT_HANDLE) ? file::mode::READ : file::mode::WRITE; }
    };

    stream_data streams[STREAM_COUNT] = {
        // stdin
        { 
            STD_INPUT_HANDLE,
            config.stdin_option,
            { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE },
            config.stdin_redirect
        },
        // stdout
        {
            STD_OUTPUT_HANDLE,
            config.stdout_option,
            { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE },
            config.stdout_redirect
        },
        // stderr
        {
            STD_ERROR_HANDLE,
            config.stderr_option,
            { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE },
            config.stderr_redirect
        }
    };

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
                // Redirect to a null device if the stream is null
                stream.proc_pipe() = CreateFileW(
                    VX_NULL_DEVICE,
                    GENERIC_ALL,
                    0,
                    &security_attributes,
                    OPEN_EXISTING,
                    0,
                    NULL
                );

                if (stream.proc_pipe() == INVALID_HANDLE_VALUE)
                {
                    windows::error_message("CreateFileW()");
                    goto cleanup;
                }

                break;
            }
            case io_option::CREATE:
            {
                // Create a pipe for communication between parent and child process
                if (!CreatePipe(&stream.read_pipe(), &stream.write_pipe(), &security_attributes, 0))
                {
                    windows::error_message("CreatePipe()");
                    goto cleanup;
                }
                // Set the pipe to non-blocking mode
                DWORD pipe_mode = PIPE_NOWAIT;
                if (!SetNamedPipeHandleState(stream.user_pipe(), &pipe_mode, NULL, NULL))
                {
                    windows::error_message("SetNamedPipeHandleState()");
                    goto cleanup;
                }
                // Ensure the user pipe handle is not inherited by the child process
                if (!SetHandleInformation(stream.user_pipe(), HANDLE_FLAG_INHERIT, 0))
                {
                    windows::error_message("SetHandleInformation()");
                    goto cleanup;
                }

                break;
            }
            case io_option::REDIRECT:
            {
                if (!stream.redirect || !stream.redirect->is_open()) // no write?
                {
                    VX_ERR(err::INVALID_ARGUMENT) << "redirect stream was null";
                    goto cleanup;
                }

                if ((stream.proc_file_mode() == file::mode::READ && !stream.redirect->can_read()) ||
                    (stream.proc_file_mode() == file::mode::WRITE && !stream.redirect->can_write()))
                {
                    VX_ERR(err::INVALID_ARGUMENT) << "redirect stream mode is incompatable with expected file mode";
                    goto cleanup;
                }

                if (!DuplicateHandle(
                    GetCurrentProcess(),
                    __detail::file_impl::get_handle(*stream.redirect).get(),
                    GetCurrentProcess(),
                    &stream.proc_pipe(),
                    0,
                    TRUE,
                    DUPLICATE_SAME_ACCESS))
                {
                    stream.proc_pipe() = INVALID_HANDLE_VALUE;
                    windows::error_message("DuplicateHandle()");
                    goto cleanup;
                }

                if (GetFileType(stream.proc_pipe()) == FILE_TYPE_PIPE)
                {
                    // PIPE_WAIT ensures that read and write operations on the named pipe are blocking.
                    // This means the process will wait (instead of returning immediately) 
                    // until data is available for reading or until a write operation completes.

                    DWORD wait_mode = PIPE_WAIT;
                    if (!SetNamedPipeHandleState(stream.proc_pipe(), &wait_mode, NULL, NULL))
                    {
                        windows::error_message("SetNamedPipeHandleState()");
                        goto cleanup;
                    }
                }

                break;
            }
            case io_option::INHERIT:
            default:
            {
                // Duplicate the handle from the parent process
                if (!DuplicateHandle(
                    GetCurrentProcess(),
                    GetStdHandle(stream.type),
                    GetCurrentProcess(),
                    &stream.proc_pipe(),
                    0,
                    TRUE,
                    DUPLICATE_SAME_ACCESS))
                {
                    stream.proc_pipe() = INVALID_HANDLE_VALUE;
                    windows::error_message("DuplicateHandle()");
                    goto cleanup;
                }

                break;
            }
        }
    }

    // Assign the handles to the STARTUPINFO structure
    startup_info.hStdInput  = streams[STDIN ].proc_pipe();
    startup_info.hStdOutput = streams[STDOUT].proc_pipe();
    startup_info.hStdError  = streams[STDERR].proc_pipe();

    if (!CreateProcessW(
        NULL,                       // Executable path (null if using command line args)
        command_line,               // Command line arguments
        NULL,                       // Process attributes
        NULL,                       // Thread attributes
        TRUE,                       // Inherit handles
        creation_flags,             // Process creation flags
        environment,                // Environment variables (null if inheriting)
        NULL,                       // Current directory (null if default)
        &startup_info,              // Startup information (with redirected stdin/stdout/stderr)
        &m_process_information))    // Process information
    {
        windows::error_message("CreateProcess()");
        goto cleanup;
    }

    for (int i = 0; i < STREAM_COUNT; ++i)
    {
        if (streams[i].option == io_option::CREATE)
        {
            // Create file object from handle for the corresponding stream
            p->m_streams[i] = __detail::file_impl::from_handle(
                streams[i].user_pipe(),
                streams[i].user_file_mode()
            );

            if (!p->m_streams[i].is_open())
            {
                goto cleanup;
            }
        }
    }

    success = true;

    cleanup:
    {
        // Ensure we properly close handles
        if (startup_info.hStdInput != INVALID_HANDLE_VALUE &&
            startup_info.hStdInput != streams[STDIN].proc_pipe())
        {
            CloseHandle(startup_info.hStdInput);
        }
        if (startup_info.hStdOutput != INVALID_HANDLE_VALUE &&
            startup_info.hStdOutput != streams[STDOUT].proc_pipe())
        {
            CloseHandle(startup_info.hStdOutput);
        }
        if (startup_info.hStdError != INVALID_HANDLE_VALUE &&
            startup_info.hStdError != streams[STDERR].proc_pipe())
        {
            CloseHandle(startup_info.hStdError);
        }

        for (int i = 0; i < STREAM_COUNT; ++i)
        {
            if (streams[i].proc_pipe() != INVALID_HANDLE_VALUE)
            {
                CloseHandle(streams[i].proc_pipe());
            }
            if (!success && streams[i].user_pipe() != INVALID_HANDLE_VALUE)
            {
                CloseHandle(streams[i].user_pipe());
            }
        }
    }

    return success;
}

process::id process::process_impl::get_pid() const
{
    return static_cast<id>(
        is_valid() ? m_process_information.dwProcessId : 0
    );
}

bool process::process_impl::is_valid() const
{
    return (m_process_information.hProcess != INVALID_HANDLE_VALUE);
}

#define assert_process_configured() VX_ASSERT_MESSAGE(is_valid(), "process not configured")

bool process::process_impl::is_alive() const
{
    assert_process_configured();

    DWORD exit_code;
    if (!GetExitCodeProcess(m_process_information.hProcess, &exit_code))
    {
        windows::error_message("GetExitCodeProcess()");
        return false;
    }

    return (exit_code == STILL_ACTIVE);
}

bool process::process_impl::is_complete() const
{
    assert_process_configured();

    const DWORD result = WaitForSingleObject(
        m_process_information.hProcess,
        0
    );

    if (result == WAIT_FAILED)
    {
        windows::error_message("WaitForSingleObject(): WAIT_FAILED");
        return false;
    }

    return (result == WAIT_OBJECT_0);
}

bool process::process_impl::join()
{
    assert_process_configured();

    const DWORD result = WaitForSingleObject(
        m_process_information.hProcess,
        INFINITE
    );

    if (result == WAIT_FAILED)
    {
        windows::error_message("WaitForSingleObject(): WAIT_FAILED");
        return false;
    }

    return (result == WAIT_OBJECT_0);
}

bool process::process_impl::kill(bool force)
{
    assert_process_configured();

    if (!TerminateProcess(m_process_information.hProcess, 1))
    {
        windows::error_message("TerminateProcess()");
        return false;
    }

    return join();
}

bool process::process_impl::get_exit_code(int* exit_code) const
{
    assert_process_configured();

    if (exit_code)
    {
        DWORD rc;
        if (!GetExitCodeProcess(m_process_information.hProcess, &rc))
        {
            windows::error_message("GetExitCodeProcess()");
            return false;
        }

        *exit_code = static_cast<int>(rc);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// this_process
///////////////////////////////////////////////////////////////////////////////

process::id this_process::get_pid_impl() noexcept
{
    const DWORD pid = GetCurrentProcessId();
    return static_cast<process::id>(pid);
}

process::environment this_process::get_environment_impl()
{
    process::environment environment;

    LPWCH environment_strings = GetEnvironmentStringsW();
    if (!environment_strings)
    {
        windows::error_message("GetEnvironmentStringsW()");
        return environment;
    }

    LPCWSTR current = environment_strings;
    while (*current)
    {
        const std::wstring entry(current);

        size_t pos = entry.find(L'=');
        if (pos != std::wstring::npos)
        {
            const std::wstring wname = entry.substr(0, pos);
            const std::wstring wvalue = entry.substr(pos + 1);

            environment.emplace(
                str::string_cast<char>(wname),
                str::string_cast<char>(wvalue)
            );
        }

        current += entry.size() + 1;
    }

    FreeEnvironmentStringsW(environment_strings);
    return environment;
}

std::string this_process::get_environment_variable_impl(const std::string& name)
{
    const std::wstring wname(str::string_cast<wchar_t>(name));

    DWORD buffer_size = GetEnvironmentVariableW(wname.c_str(), NULL, 0);
    if (buffer_size == 0)
    {
        //windows::error_message("GetEnvironmentVariableW()");
        return std::string{};
    }

    std::wstring wvalue(buffer_size - 1, 0); // don't have to worry about null terminator
    GetEnvironmentVariableW(wname.c_str(), &wvalue[0], buffer_size);
    return str::string_cast<char>(wvalue);
}

bool this_process::set_environment_variable_impl(const std::string& name, const std::string& value)
{
    const std::wstring wname(str::string_cast<wchar_t>(name));
    const std::wstring wvalue(str::string_cast<wchar_t>(value));

    if (SetEnvironmentVariableW(wname.c_str(), wvalue.c_str()) == 0)
    {
        windows::error_message("SetEnvironmentVariableW()");
        return false;
    }
    return true;
}

bool this_process::clear_environment_variable_impl(const std::string& name)
{
    const std::wstring wname(str::string_cast<wchar_t>(name));
    SetEnvironmentVariableW(wname.c_str(), NULL);
    return true;
}

static io_stream get_stream_handle(DWORD nStdHandle)
{
    HANDLE h;

    // Duplicate the handle from the parent process
    if (!DuplicateHandle(
        GetCurrentProcess(),
        GetStdHandle(nStdHandle),
        GetCurrentProcess(),
        &h,
        0,
        TRUE,
        DUPLICATE_SAME_ACCESS))
    {
        windows::error_message("DuplicateHandle()");
    }

    return __detail::file_impl::from_handle(
        h,
        (nStdHandle == STD_INPUT_HANDLE) ? io_stream::mode::READ : io_stream::mode::WRITE
    );
}

io_stream this_process::get_stdin_impl()
{
    return get_stream_handle(STD_INPUT_HANDLE);
}

io_stream this_process::get_stdout_impl()
{
    return get_stream_handle(STD_OUTPUT_HANDLE);
}

io_stream this_process::get_stderr_impl()
{
    return get_stream_handle(STD_ERROR_HANDLE);
}

} // namespace os
} // namespace vx