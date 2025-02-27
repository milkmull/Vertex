#include "vertex/os/process.hpp"
#include "vertex/os/time.hpp"

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
    command cmd = command::NONE;

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
            os::this_process::get_stdout().write_line("Hello World");
            break;
        }
        case command::EXIT_CODE:
        {
            const int exit_code = str::to_int32(argv[i]);
            return exit_code;
        }
        case command::SHOW_ENVIRONMENT:
        {
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
            while (true)
            {
                os::sleep(time::milliseconds(100));
            }

            break;
        }
        case command::STDIN_TO_STDOUT:
        {
            os::io_stream stdin = os::this_process::get_stdin();
            os::io_stream stdout = os::this_process::get_stdout();

            std::string line;
            while (stdin.read_line(line))
            {
                if (line == "EOF")
                {
                    break;
                }

                stdout.write_line(line);
            }

            break;
        }
        case command::STDIN_TO_STDERR:
        {
            os::io_stream stdin = os::this_process::get_stdin();
            os::io_stream stderr = os::this_process::get_stderr();

            std::string line;
            while (stdin.read_line(line))
            {
                if (line == "EOF")
                {
                    break;
                }

                stderr.write_line(line);
            }

            break;
        }
        default:
        {
            break;
        }
    }

    return 0;
}