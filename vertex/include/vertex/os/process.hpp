#pragma once

#include <unordered_map>

#include "vertex/stdlib/file/file.hpp"

#undef stdin
#undef stdout
#undef stderr

namespace vx {
namespace os {

using process_id = int;
using process_environment = std::unordered_map<std::string, std::string>;

///////////////////////////////////////////////////////////////////////////////
// this_process
///////////////////////////////////////////////////////////////////////////////

namespace this_process {

process_id get_pid();

process_environment get_environment();
std::string get_environment_variable(const std::string& name);
bool set_environment_variable(const std::string& name, const std::string& value);
bool clear_environment_variable(const std::string& name);

size_t get_memory_usage();
size_t get_cpu_usage();

} // namespace this_process

///////////////////////////////////////////////////////////////////////////////
// process
///////////////////////////////////////////////////////////////////////////////

class process
{
public:

    enum class io_option
    {
        NONE,
        CREATE,
        REDIRECT,
        INHERIT
    };

    struct config
    {
        std::vector<std::string> args;
        process_environment environment;

        bool background = true;

        io_option stdin_option  = io_option::NONE;
        io_option stdout_option = io_option::NONE;
        io_option stderr_option = io_option::NONE;

        file* stdin_redirect  = nullptr;
        file* stdout_redirect = nullptr;
        file* stderr_redirect = nullptr;
    };

private:

    enum : int
    {
        STDIN = 0,
        STDOUT = 1,
        STDERR = 2,
        STREAM_COUNT = 3
    };

public:

    process();
    ~process();

    process(const process&) = delete;
    process(process&&) noexcept;

    process& operator=(const process&) = delete;
    process& operator=(process&&) noexcept;

public:

    bool start(const config& config);

    bool is_initialized() const;
    process_id get_pid() const;

    bool is_alive() const;
    bool kill(bool force = false);
    bool wait(bool block, int* exit_code = nullptr);

    file& get_stdin();
    file& get_stdout();
    file& get_stderr();

private:

    file m_streams[STREAM_COUNT]{};

    class process_impl;
    std::unique_ptr<process_impl> m_impl;
};

} // namespace os
} // namespace vx