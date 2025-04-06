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

class process
{
public:

    using id = uint32_t;
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
        process::environment environment;

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

    VX_API process();
    VX_API ~process();

    process(const process&) = delete;
    VX_API process(process&&) noexcept;

    process& operator=(const process&) = delete;
    VX_API process& operator=(process&&) noexcept;

    void swap(process& other) noexcept { std::swap(m_impl, other.m_impl); }

public:

    VX_API bool start(const config& cfg);

    VX_API id get_pid() const;

    VX_API bool is_valid() const;
    VX_API bool is_alive() const;
    VX_API bool is_complete() const;

    VX_API bool join();
    VX_API bool kill(bool force = false);

    VX_API bool get_exit_code(int* exit_code) const;

    VX_API io_stream& get_stdin();
    VX_API io_stream& get_stdout();
    VX_API io_stream& get_stderr();

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

VX_API io_stream get_stdin();
VX_API io_stream get_stdout();
VX_API io_stream get_stderr();

} // namespace this_process

} // namespace os
} // namespace vx