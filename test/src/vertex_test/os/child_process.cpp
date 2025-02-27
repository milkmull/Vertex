#include "vertex/os/process.hpp"

using namespace vx;

enum class command
{
    NONE,
    HELLO_WORLD,
    STDIN_TO_STDOUT
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
    }

    ++i;

    switch (cmd)
    {
        case command::HELLO_WORLD:
        {
            std::cout << "Hello World!\n";
            break;
        }
        case command::STDIN_TO_STDOUT:
        {
            os::io_stream stdin = os::this_process::get_stdin();
            os::io_stream stdout = os::this_process::get_stdout();

            std::string line;
            while (stdin.read_line(line))
            {
                stdout.write(line);
                if (line == "EOF")
                {
                    break;
                }
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