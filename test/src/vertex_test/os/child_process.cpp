#include "vertex/os/process.hpp"
#include "vertex/os/time.hpp"
#include "vertex/os/file.hpp"

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
    os::file log;
    log.open("child_process_log.txt", os::file::mode::WRITE);

    command cmd = command::NONE;

    log.write_line("starting child process");

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
            log.write_line("running --hello-world");

            os::this_process::get_stdout().write_line("Hello World");
            break;
        }
        case command::EXIT_CODE:
        {
            log.write("running --exit-code");
            log.write_line(argv[i]);

            const int exit_code = str::to_int32(argv[i]);
            return exit_code;
        }
        case command::SHOW_ENVIRONMENT:
        {
            log.write_line("running --show-environment");

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
            log.write("running --stall");
            log.write_line(argv[i]);

            while (true)
            {
                os::sleep(time::milliseconds(100));
            }

            break;
        }
        case command::STDIN_TO_STDOUT:
        {
            log.write_line("running --stdin-to-stdout");

            os::io_stream p_stdin = os::this_process::get_stdin();
            os::io_stream p_stdout = os::this_process::get_stdout();

            std::string line;
            while (p_stdin.read_line(line))
            {
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
        case command::STDIN_TO_STDERR:
        {
            log.write_line("running --stdin-to-stderr");

            os::io_stream p_stdin = os::this_process::get_stdin();
            os::io_stream p_stderr = os::this_process::get_stderr();

            std::string line;
            while (p_stdin.read_line(line))
            {
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

    // Give the parent process time to finish reading
    os::sleep(time::milliseconds(10));

    log.write_line("child process finished");
    return 0;
}