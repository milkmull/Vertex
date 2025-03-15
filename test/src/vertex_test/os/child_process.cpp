#include "vertex/os/process.hpp"
#include "vertex/os/time.hpp"
#include "vertex/system/log.hpp"

using namespace vx;

enum class command
{
    NONE,
    HELLO_WORLD,
    EXIT_CODE,
    SHOW_ENVIRONMENT,
    STALL,
    STDIN_TO_STDOUT,
    STDIN_TO_STDERR
};

int main(int argc, char* argv[])
{
    VX_PRINT_ERRORS(true);
    log::start("child_process_log.txt");

    command cmd = command::NONE;

    VX_LOG_INFO << "starting child process";

    int i = 0;
    for (; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--hello-world") == 0)
        {
            cmd = command::HELLO_WORLD;
            break;
        }
        if (std::strcmp(argv[i], "--exit-code") == 0)
        {
            cmd = command::EXIT_CODE;
            break;
        }
        if (std::strcmp(argv[i], "--show-environment") == 0)
        {
            cmd = command::SHOW_ENVIRONMENT;
            break;
        }
        if (std::strcmp(argv[i], "--stall") == 0)
        {
            cmd = command::STALL;
            break;
        }
        if (std::strcmp(argv[i], "--stdin-to-stdout") == 0)
        {
            cmd = command::STDIN_TO_STDOUT;
            break;
        }
        if (std::strcmp(argv[i], "--stdin-to-stderr") == 0)
        {
            cmd = command::STDIN_TO_STDERR;
            break;
        }
    }

    ++i;

    switch (cmd)
    {
        case command::HELLO_WORLD:
        {
            VX_LOG_INFO << "running --hello-world";

            os::this_process::get_stdout().write_line("Hello World");
            break;
        }
        case command::EXIT_CODE:
        {
            VX_LOG_INFO << "running --exit-code" << argv[i];

            const int exit_code = str::to_int32(argv[i]);
            return exit_code;
        }
        case command::SHOW_ENVIRONMENT:
        {
            VX_LOG_INFO << "running --show-environment";

            const os::process::environment env = os::this_process::get_environment();
            std::vector<char> data;

            for (const auto& pair : env)
            {
                data.insert(data.end(), pair.first.begin(), pair.first.end());
                data.push_back('=');
                data.insert(data.end(), pair.second.begin(), pair.second.end());
                data.push_back('\0');
            }
            data.push_back('\0');

            os::this_process::get_stdout().write(data.data(), data.size());
            break;
        }
        case command::STALL:
        {
            VX_LOG_INFO << "running --stall" << argv[i];

            while (true)
            {
                os::sleep(time::milliseconds(100));
            }

            break;
        }
        case command::STDIN_TO_STDOUT:
        {
            VX_LOG_INFO << "running --stdin-to-stdout";

            os::io_stream p_stdin = os::this_process::get_stdin();
            os::io_stream p_stdout = os::this_process::get_stdout();

            std::string line;
            while (p_stdin.read_line(line))
            {
                if (line == "EOF")
                {
                    VX_LOG_INFO << "EOF";
                    break;
                }

                p_stdout.write_line(line);
                VX_LOG_INFO << "writing line " << line;
            }

            VX_LOG_INFO << "finished stdout";
            break;
        }
        case command::STDIN_TO_STDERR:
        {
            VX_LOG_INFO << "running --stdin-to-stderr";

            os::io_stream p_stdin = os::this_process::get_stdin();
            os::io_stream p_stderr = os::this_process::get_stderr();

            std::string line;
            while (p_stdin.read_line(line))
            {
                if (line == "EOF")
                {
                    break;
                }

                p_stderr.write_line(line);
                VX_LOG_INFO << "writing line " << line;
            }

            break;
        }
        default:
        {
            break;
        }
    }

    VX_LOG_INFO << "child process finished";

    return 0;
}