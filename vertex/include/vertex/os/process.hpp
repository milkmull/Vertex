#pragma once

#include <memory>
#include <unordered_map>

#include "vertex/config/language_config.hpp"
#include "vertex/os/file.hpp"

#undef stdin
#undef stdout
#undef stderr

namespace vx {
namespace os {

using io_stream = file;

///////////////////////////////////////////////////////////////////////////////
// process
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Represents a process that can be started, controlled, and monitored.
 *
 * This class provides an abstraction for working with processes. It allows for starting,
 * managing input/output streams, checking the status of a process, and getting the process's
 * exit code. The process can be run in the background and can be configured with specific
 * environment variables, arguments, and working directory.
 */
class process
{
public:

    using id = uint32_t;
    using environment = std::unordered_map<std::string, std::string>;

    enum class io_option
    {
        none,      // No I/O stream
        create,    // Create new I/O stream
        redirect,  // Redirect the I/O stream to a given file
        inherit    // Inherit the I/O stream from parent process
    };

    struct config
    {
        std::vector<std::string> args;              // Arguments passed to the process
        process::environment environment;           // Environment variables for the process

        bool background = true;                     // Whether the process runs in the background
        path working_directory;                     // Working directory of the process

        io_option stdin_option = io_option::none;   // I/O option for stdin
        io_option stdout_option = io_option::none;  // I/O option for stdout
        io_option stderr_option = io_option::none;  // I/O option for stderr

        io_stream* stdin_redirect = nullptr;        // Stream to redirect stdin
        io_stream* stdout_redirect = nullptr;       // Stream to redirect stdout
        io_stream* stderr_redirect = nullptr;       // Stream to redirect stderr
    };

private:

    enum : int
    {
        stdin_index = 0,
        stdout_index = 1,
        stderr_index = 2,
        stream_count = 3
    };

public:

    VX_API process();
    VX_API ~process();

    process(const process&) = delete;
    VX_API process(process&&) noexcept;

    process& operator=(const process&) = delete;
    VX_API process& operator=(process&&) noexcept;

    void swap(process& other) noexcept { std::swap(m_impl, other.m_impl); }

public:

    /**
     * @brief Start the process with the specified configuration.
     * @param cfg The configuration to use for starting the process.
     * @return `true` if the process started successfully, `false` otherwise.
     */
    VX_API bool start(const config& cfg);

    /**
     * @brief Get the process ID (PID).
     * @return The process ID.
     */
    VX_API id get_pid() const;

    /**
     * @brief Check if the process is valid.
     * @return `true` if the process is valid, `false` otherwise.
     */
    VX_API bool is_valid() const;

    /**
     * @brief Check if the process is alive.
     * @return `true` if the process is alive, `false` otherwise.
     */
    VX_API bool is_alive() const;

    /**
     * @brief Check if the process is complete.
     * @return `true` if the process has completed, `false` otherwise.
     */
    VX_API bool is_complete() const;

    /**
     * @brief Wait for the process to complete (join).
     * @return `true` if the process completed successfully, `false` otherwise.
     */
    VX_API bool join();

    /**
     * @brief Kill the process.
     * @param force If `true`, forcefully kill the process. Default is `false`.
     * @return `true` if the process was killed successfully, `false` otherwise.
     */
    VX_API bool kill(bool force = false);

    /**
     * @brief Get the exit code of the process.
     * @param[out] exit_code Pointer to store the exit code.
     * @return `true` if the exit code was retrieved successfully, `false` otherwise.
     */
    VX_API bool get_exit_code(int* exit_code) const;

    /**
     * @brief Get the stdin stream of the process.
     * @return A reference to the stdin stream.
     */
    VX_API io_stream& get_stdin();

    /**
     * @brief Get the stdout stream of the process.
     * @return A reference to the stdout stream.
     */
    VX_API io_stream& get_stdout();

    /**
     * @brief Get the stderr stream of the process.
     * @return A reference to the stderr stream.
     */
    VX_API io_stream& get_stderr();

private:

    io_stream m_streams[stream_count]{};

    class process_impl;
    std::unique_ptr<process_impl> m_impl;
};

///////////////////////////////////////////////////////////////////////////////
// this_process
///////////////////////////////////////////////////////////////////////////////

namespace this_process {

/**
 * @brief Get the process ID (PID) of the current process.
 * @return The process ID of the current process.
 */
VX_API process::id get_pid();

/**
 * @brief Get the environment variables of the current process.
 * @return A map containing the environment variables of the current process.
 */
VX_API process::environment get_environment();

/**
 * @brief Get the value of a specific environment variable.
 * @param name The name of the environment variable.
 * @return The value of the environment variable, or an empty string if the variable is not found.
 */
VX_API std::string get_environment_variable(const std::string& name);

/**
 * @brief Set the value of an environment variable for the current process.
 * @param name The name of the environment variable.
 * @param value The value to assign to the environment variable.
 * @return `true` if the environment variable was successfully set, `false` otherwise.
 */
VX_API bool set_environment_variable(const std::string& name, const std::string& value);

/**
 * @brief Clear an environment variable for the current process.
 * @param name The name of the environment variable to clear.
 * @return `true` if the environment variable was successfully cleared, `false` otherwise.
 */
VX_API bool clear_environment_variable(const std::string& name);

/**
 * @brief Get the standard input stream of the current process.
 * @return A reference to the stdin stream of the current process.
 */
VX_API io_stream get_stdin();

/**
 * @brief Get the standard output stream of the current process.
 * @return A reference to the stdout stream of the current process.
 */
VX_API io_stream get_stdout();

/**
 * @brief Get the standard error stream of the current process.
 * @return A reference to the stderr stream of the current process.
 */
VX_API io_stream get_stderr();

} // namespace this_process

} // namespace os
} // namespace vx