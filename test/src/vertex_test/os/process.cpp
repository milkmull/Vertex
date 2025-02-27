#include "vertex_test/test.hpp"
#include "vertex/os/process.hpp"
#include "vertex/os/time.hpp"
#include "vertex/util/random/rng.hpp"

#if defined(EOF)
#   undef EOF
#endif

using namespace vx;

#if defined (VX_PLATFORM_WINDOWS)
#   define EXE ".exe"
#else
#   define EXE ""
#endif // VX_PLATFORM_WINDOWS

static const char* child_process = "child_process" EXE;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_traits)
{
    VX_STATIC_CHECK(!std::is_copy_constructible<os::process>::value);
    VX_STATIC_CHECK(!std::is_copy_assignable<os::process>::value);

    VX_STATIC_CHECK(std::is_move_constructible<os::process>::value);
    VX_STATIC_CHECK(std::is_move_assignable<os::process>::value);
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_this_process)
{
    VX_SECTION("pid")
    {
        VX_CHECK_AND_EXPECT_NO_ERROR(os::this_process::get_pid() != 0);
    }

    VX_SECTION("environment")
    {
        // get the current environment
        os::process::environment env1;
        VX_EXPECT_NO_ERROR(env1 = os::this_process::get_environment());
        VX_CHECK(!env1.empty());

        // add a new environment variable
        const std::pair<std::string, std::string> var{ "test_variable", "hello world" };
        VX_CHECK_AND_EXPECT_NO_ERROR(os::this_process::set_environment_variable(var.first, var.second));

        // make sure it was added
        os::process::environment env2;
        VX_EXPECT_NO_ERROR(env2 = os::this_process::get_environment());
        VX_CHECK(!env2.empty());
        VX_CHECK(env2.count(var.first) != 0);
        VX_CHECK(env2[var.first] == var.second);

        // update the value of the added variable
        std::string value;
        VX_EXPECT_NO_ERROR(value = os::this_process::get_environment_variable(var.first));
        VX_CHECK(!value.empty());
        VX_CHECK(value == var.second);

        // clear it
        VX_CHECK_AND_EXPECT_NO_ERROR(os::this_process::clear_environment_variable(var.first));
        VX_CHECK_AND_EXPECT_ERROR(os::this_process::get_environment_variable(var.first).empty());
    }
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_process_initial_state)
{
    os::process p;

    VX_CHECK(!p.is_valid());
    VX_CHECK(p.get_pid() == 0);
    VX_CHECK(!p.is_alive());
    VX_CHECK(!p.is_complete());
    VX_CHECK(!p.join());
    VX_CHECK(!p.kill());
    VX_CHECK(!p.get_exit_code(nullptr));

    VX_CHECK(!p.get_stdin().is_open());
    VX_CHECK(!p.get_stdout().is_open());
    VX_CHECK(!p.get_stderr().is_open());
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_basic_process)
{
    os::process p;

    os::process::config cfg;
    cfg.args = { child_process, "--hello-world" };
    cfg.stdout_option = os::process::io_option::INHERIT;
    VX_CHECK_AND_EXPECT_NO_ERROR(p.start(cfg));

    VX_CHECK(p.is_valid());
    VX_CHECK(p.get_pid() != 0);
    VX_CHECK(p.join());
    VX_CHECK(!p.is_alive());
    VX_CHECK(p.is_complete());

    int code;
    VX_CHECK(p.get_exit_code(&code));
    VX_CHECK(code == 0);

    VX_CHECK_AND_EXPECT_ERROR(!p.start(cfg));
}

///////////////////////////////////////////////////////////////////////////////

static std::string echo_argument(const std::string& arg)
{
    os::process::config cfg;

    // create a new stdout stream
    cfg.stdout_option = os::process::io_option::CREATE;

#if defined(VX_PLATFORM_WINDOWS)

    cfg.args = { "cmd.exe", "/C", "echo", arg };

#else

    cfg.args = { "/bin/echo", arg };

#endif // VX_PLATFORM_WINDOWS

    os::process p;
    VX_CHECK_AND_EXPECT_NO_ERROR(p.start(cfg));

    VX_CHECK(p.is_valid());
    VX_CHECK(p.join());
    VX_CHECK(p.is_complete());

    int code;
    VX_CHECK(p.get_exit_code(&code));
    VX_CHECK(code == 0);

    // read the value from the stream
    auto& stdout = p.get_stdout();
    size_t size = stdout.size();

#if defined(VX_PLATFORM_WINDOWS)

    // on windows we want to strip the /r/n
    VX_ASSERT(size >= 2);
    size -= 2;

#endif // VX_PLATFORM_WINDOWS

    std::string echo(size, 0);
    VX_CHECK(stdout.read(echo.data(), size) == size);

    return echo;
}

VX_TEST_CASE(test_arguments)
{
    VX_CHECK(echo_argument("hello") == "hello");

#if defined(VX_PLATFORM_WINDOWS)

    VX_CHECK(echo_argument(" ") == "\" \"");
    VX_CHECK(echo_argument("a b c") == "\"a b c\"");
    VX_CHECK(echo_argument("a\tb\tc\t") == "\"a\tb\tc\t\"");
    VX_CHECK(echo_argument("(something)") == "\"(something)\"");
    VX_CHECK(echo_argument("\"quoted argument\"") == "\"\"quoted argument\"\"");
    VX_CHECK(echo_argument("C:\\Path\\to\\file\\") == "C:\\Path\\to\\file\\");
    VX_CHECK(echo_argument("C:\\Folder\\\"Test\"") == "\"C:\\Folder\\\\\"Test\"\"");

#endif // VX_PLATFORM_WINDOWS
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_stdin_to_stdout)
{
    const char* lines[] = {
        "Tests whether we can write to stdin and read from stdout",
        "{'success': true, 'message': 'Success!'}",
        "Yippie ka yee"
    };

    // Configure the process
    os::process::config config;
    config.args = { child_process, "--stdin-to-stdout" };
    config.stdin_option = os::process::io_option::CREATE;
    config.stdout_option = os::process::io_option::CREATE;

    // Start the process
    os::process p;
    VX_CHECK(p.start(config));

    // Get stdin pipe
    auto& stdin = p.get_stdin();
    VX_CHECK(stdin.is_open());
    VX_CHECK(stdin.can_write());

    // write each line
    for (auto line : lines)
    {
        VX_CHECK(stdin.write_line(line));
    }

    // finish the process
    VX_CHECK(stdin.write_line("EOF"));
    VX_CHECK(p.join());
    VX_CHECK(p.is_complete());

    // Get stdout pipe
    auto& stdout = p.get_stdout();
    VX_CHECK(stdout.is_open());
    VX_CHECK(stdout.can_read());

    // read the lines back
    std::string read_line;
    for (auto line : lines)
    {
        VX_CHECK(stdout.read_line(read_line) && read_line == line);
    }

    VX_CHECK(stdout.eof());

    // Ensure the process exits correctly
    int exit_code;
    VX_CHECK(p.get_exit_code(&exit_code));
    VX_CHECK(exit_code == 0);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_stdin_to_stderr)
{
    const char* lines[] = {
        "Tests whether we can write to stdin and read from stderr",
        "{'success': true, 'message': 'Success!'}",
        "Yippie ka yee"
    };

    // Configure the process
    os::process::config config;
    config.args = { child_process, "--stdin-to-stderr" };
    config.stdin_option = os::process::io_option::CREATE;
    config.stderr_option = os::process::io_option::CREATE;

    // Start the process
    os::process p;
    VX_CHECK(p.start(config));

    // Get stdin pipe
    auto& stdin = p.get_stdin();
    VX_CHECK(stdin.is_open());
    VX_CHECK(stdin.can_write());

    // write each line
    for (auto line : lines)
    {
        VX_CHECK(stdin.write_line(line));
    }

    // finish the process
    VX_CHECK(stdin.write_line("EOF"));
    VX_CHECK(p.join());
    VX_CHECK(p.is_complete());

    // Get stderr pipe
    auto& stderr = p.get_stderr();
    VX_CHECK(stderr.is_open());
    VX_CHECK(stderr.can_read());

    // read the lines back
    std::string read_line;
    for (auto line : lines)
    {
        VX_CHECK(stderr.read_line(read_line) && read_line == line);
    }

    VX_CHECK(stderr.eof());

    // Ensure the process exits correctly
    int exit_code;
    VX_CHECK(p.get_exit_code(&exit_code));
    VX_CHECK(exit_code == 0);
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}