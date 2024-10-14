#pragma once

#include <unordered_map>

#include "vertex/stdlib/file/file.hpp"

#undef stdin
#undef stdout
#undef stderr

namespace vx {
namespace proc {

std::unordered_map<std::string, std::string> get_environment_variables();
std::string get_environment_variable(const std::string& name);
bool set_environment_variable(const std::string& name, const std::string& value);
bool clear_environment_variable(const std::string& name);

using process_id = int;

enum class process_io
{
    IO_NULL,
    IO_INHERIT,
    IO_CREATE
};

struct process_config
{
    std::vector<std::string> args;
    std::unordered_map<std::string, std::string> environment;

    bool background = true;

    process_io stdin = process_io::IO_NULL;
    process_io stdout = process_io::IO_NULL;
    process_io stderr = process_io::IO_NULL;
};

class process
{
public:

    process();
    ~process();

    process(const process&) = delete;
    process(process&&) noexcept;

    process& operator=(const process&) = delete;
    process& operator=(process&&) noexcept;

public:

    bool start(const process_config& config);

public:

    bool is_valid() const;
    process_id pid() const;

    bool get_input(file& f);
    bool get_output(file& f);
    bool get_error(file& f);

    std::string read() const;
    int exit_code() const;

    bool is_alive() const;
    bool kill(bool force = false);
    int wait();

private:

    process_id m_pid;

    class process_impl;
    std::unique_ptr<process_impl> m_impl;
};

} // namespace proc
} // namespace vx