#include "vertex/os/process.hpp"
#include "vertex/os/time.hpp"
#include "vertex/os/file.hpp"

using namespace vx;

enum class command
{
    none,
    hello_world,
    exit_code,
    show_environment,
    stall,
    stdin_to_stdout,
    stdin_to_stderr
};

int main(int argc, char* argv[])
{
    os::file log;
    log.open("child_process_log.txt", os::file::mode::write);

    command cmd = command::none;

    log.write_line("starting child process");

    int i = 0;
    for (; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--hello-world") == 0)
        {
            cmd = command::hello_world;
            break;
        }
        if (std::strcmp(argv[i], "--exit-code") == 0)
        {
            cmd = command::exit_code;
            break;
        }
        if (std::strcmp(argv[i], "--show-environment") == 0)
        {
            cmd = command::show_environment;
            break;
        }
        if (std::strcmp(argv[i], "--stall") == 0)
        {
            cmd = command::stall;
            break;
        }
        if (std::strcmp(argv[i], "--stdin-to-stdout") == 0)
        {
            cmd = command::stdin_to_stdout;
            break;
        }
        if (std::strcmp(argv[i], "--stdin-to-stderr") == 0)
        {
            cmd = command::stdin_to_stderr;
            break;
        }
    }

    ++i;

    switch (cmd)
    {
        case command::hello_world:
        {
            log.write_line("running --hello-world");

            os::this_process::get_stdout().write_line("Hello World");
            break;
        }
        case command::exit_code:
        {
            log.write("running --exit-code");
            log.write_line(argv[i]);

            const int exit_code = str::to_int32(argv[i]);
            return exit_code;
        }
        case command::show_environment:
        {
            log.write_line("running --show-environment");

            os::io_stream p_stdout = os::this_process::get_stdout();

            const os::process::environment env = os::this_process::get_environment();
            for (const auto& pair : env)
            {
                p_stdout.write(pair.first);
                p_stdout.write('=');
                p_stdout.write_line(pair.second);
            }

            break;
        }
        case command::stall:
        {
            log.write("running --stall");

            while (true)
            {
                os::sleep(time::milliseconds(100));
            }

            break;
        }
        case command::stdin_to_stdout:
        {
            log.write_line("running --stdin-to-stdout");

            os::io_stream p_stdin = os::this_process::get_stdin();
            os::io_stream p_stdout = os::this_process::get_stdout();

            std::string line;
            while (true)
            {
                if (!p_stdin.read_line(line))
                {
                    os::sleep(time::milliseconds(10));
                    continue;
                }

                if (line == "EOFF")
                {
                    // forward it
                    p_stdout.write_line(line);
                    break;
                }

                if (line == "EOF")
                {
                    log.write_line("EOF");
                    break;
                }

                p_stdout.write_line(line);
                log.write("writing line: ");
                log.write_line(line);
            }

            log.write_line("finished stdout");
            break;
        }
        case command::stdin_to_stderr:
        {
            log.write_line("running --stdin-to-stderr");

            os::io_stream p_stdin = os::this_process::get_stdin();
            os::io_stream p_stderr = os::this_process::get_stderr();

            std::string line;
            while (true)
            {
                if (!p_stdin.read_line(line))
                {
                    os::sleep(time::milliseconds(10));
                    continue;
                }

                if (line == "EOFF")
                {
                    // forward it
                    p_stderr.write_line(line);
                    break;
                }

                if (line == "EOF")
                {
                    log.write_line("EOF");
                    break;
                }

                p_stderr.write_line(line);
                log.write("writing line: ");
                log.write_line(line);
            }

            log.write_line("finished stdout");
            break;
        }
        default:
        {
            break;
        }
    }

    log.write_line("child process finished");
    return 0;
}