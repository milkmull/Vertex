#pragma once

#include <unordered_map>

#include "vertex/core/compiler.hpp"
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

class process
{
public:

    using id = int;
    using environment = std::unordered_map<std::string, std::string>;

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
        environment environment;

        bool background = true;

        io_option stdin_option  = io_option::NONE;
        io_option stdout_option = io_option::NONE;
        io_option stderr_option = io_option::NONE;

        io_stream* stdin_redirect  = nullptr;
        io_stream* stdout_redirect = nullptr;
        io_stream* stderr_redirect = nullptr;
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

    friend void swap(process& lhs, process& rhs) noexcept;

public:

    bool start(const config& config);

    id get_pid() const;

    bool is_valid() const;
    bool is_alive() const;
    bool is_complete() const;

    bool join();
    bool kill(bool force = false);

    bool get_exit_code(int* exit_code) const;

    io_stream& get_stdin();
    io_stream& get_stdout();
    io_stream& get_stderr();

private:

    io_stream m_streams[STREAM_COUNT]{};

    class process_impl;
    std::unique_ptr<process_impl> m_impl;
};

///////////////////////////////////////////////////////////////////////////////
// this_process
///////////////////////////////////////////////////////////////////////////////

namespace this_process {

VX_API process::id get_pid();

VX_API process::environment get_environment();
VX_API std::string get_environment_variable(const std::string& name);
VX_API bool set_environment_variable(const std::string& name, const std::string& value);
VX_API bool clear_environment_variable(const std::string& name);

} // namespace this_process

} // namespace os
} // namespace vx