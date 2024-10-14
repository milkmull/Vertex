#include "vertex_impl/stdlib/system/_win32/win32_process.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex/stdlib/string/string.hpp"

namespace vx {
namespace proc {

std::unordered_map<std::string, std::string> get_environment_variables()
{
    std::unordered_map<std::string, std::string> environment;

    LPWCH environment_strings = GetEnvironmentStringsW();
    if (!environment_strings)
    {
        WindowsErrorMessage("GetEnvironmentStringsW()");
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
                str::wstring_to_string(wname),
                str::wstring_to_string(wvalue)
            );
        }

        current += entry.size() + 1;
    }

    FreeEnvironmentStringsW(environment_strings);
    return environment;
}

std::string get_environment_variable(const std::string& name)
{
    const std::wstring wname(str::string_to_wstring(name));

    DWORD buffer_size = GetEnvironmentVariableW(wname.c_str(), NULL, 0);
    if (buffer_size == 0)
    {
        WindowsErrorMessage("GetEnvironmentVariableW()");
        return std::string();
    }

    std::wstring wvalue(buffer_size, 0);
    GetEnvironmentVariableW(wname.c_str(), &wvalue[0], buffer_size);
    return str::wstring_to_string(wvalue);
}

bool set_environment_variable(const std::string& name, const std::string& value)
{
    const std::wstring wname(str::string_to_wstring(name));
    const std::wstring wvalue(str::string_to_wstring(value));

    if (SetEnvironmentVariableW(wname.c_str(), wvalue.c_str()) == 0)
    {
        WindowsErrorMessage("SetEnvironmentVariableW()");
        return false;
    }
    return true;
}

bool clear_environment_variable(const std::string& name)
{
    const std::wstring wname(str::string_to_wstring(name));

    if (SetEnvironmentVariableW(wname.c_str(), NULL) == 0)
    {
        WindowsErrorMessage("SetEnvironmentVariableW()");
        return false;
    }
    return true;
}

static std::string join_arguments(const std::vector<std::string>& args)
{
    const size_t count = args.size();
    const bool batch_file = str::ends_with(args[0], ".bat") || str::ends_with(args[0], ".cmd");

    std::vector<bool> need_quotes(count, false);

    for (size_t i = 0; i < count; ++i)
    {
        if ((args[i].find_first_of(" \t\"()=!") != std::string::npos) ||
            (batch_file && args[i].find_first_of("^&|<>") != std::string::npos))
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

            for (const char* it = args[i].data(); *it; ++it)
            {
                // Only escape backslashes that precede a double quote
                if (*it == '"' || (*it == '\\' && (it[1] == '"' || it[1] == '\0')))
                {
                    out.push_back('\\');
                }

                out.push_back(*it);
            }

            out.push_back('"');
        }
        else
        {
            out.append(args[i]);
        }

        if (i < count - 1)
        {
            out.push_back(' ');
        }
    }

    return out;
}

static std::string join_environment(const std::unordered_map<std::string, std::string>& environment)
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

bool process::process_impl::start(process* p, const process_config& config)
{
    m_process_information.hProcess = INVALID_HANDLE_VALUE;
    m_process_information.hThread = INVALID_HANDLE_VALUE;
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
        wargs = str::string_to_wstring(join_arguments(config.args));
        command_line = wargs.data();
    }

    std::wstring wenv;
    LPVOID environment = NULL;

    if (!config.environment.empty())
    {
        wenv = str::string_to_wstring(join_environment(config.environment));
        environment = wenv.data();
    }

    process_io stdin  = config.stdin;
    process_io stdout = config.stdout;
    process_io stderr = config.stderr;

    if (config.background)
    {
        if (stdin == process_io::IO_INHERIT)
        {
            stdin = process_io::IO_NULL;
        }
        if (stdout == process_io::IO_INHERIT)
        {
            stdout = process_io::IO_NULL;
        }
        if (stderr == process_io::IO_INHERIT)
        {
            stderr = process_io::IO_NULL;
        }
    }

    switch (stdout)
    {
        default:
        {
            if (!DuplicateHandle(
                GetCurrentProcess(),
                GetStdHandle(STD_OUTPUT_HANDLE),
                GetCurrentProcess(),
                &startup_info.hStdOutput,
                0,
                TRUE,
                DUPLICATE_SAME_ACCESS))
            {
                startup_info.hStdOutput = INVALID_HANDLE_VALUE;
                WindowsErrorMessage("DuplicateHandle()");
                return false;
            }

            break;
        }
    }

    if (!CreateProcessW(
        // Path to the executable
        NULL,
        // Command line arguments
        command_line,
        // Process attributes
        NULL,
        // Thread attributes
        NULL,
        // Inherit handles
        TRUE,
        // Creation flags
        creation_flags,
        // Environment (same as calling)
        environment,
        // Directory (same as calling)
        NULL,

        &startup_info,
        &m_process_information))
    {
        WindowsErrorMessage("CreateProcess()");
        return false;
    }

    p->m_pid = m_process_information.dwProcessId;
    return true;
}

} // namespace proc
} // namespace vx

#endif