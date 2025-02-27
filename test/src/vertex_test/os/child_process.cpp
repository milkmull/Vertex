#include "vertex/os/process.hpp"

using namespace vx;

enum class command
{
    NONE,
    HELLO_WORLD,
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
        else if (std::strcmp(argv[i], "--stdin-to-stdout") == 0)
        {
            cmd = command::STDIN_TO_STDOUT;
            break;
        }
        else if (std::strcmp(argv[i], "--stdin-to-stderr") == 0)
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