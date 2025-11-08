#include "vertex_test/test.hpp"
#include "vertex/os/process.hpp"
#include "vertex/os/time.hpp"

#if defined(EOF)
#   undef EOF
#endif

using namespace vx;

#if defined (VX_OS_WINDOWS)
#   define EXE ".exe"
static const char* child_process = "os_child_process" EXE;
#else
#   define EXE ""
static const char* child_process = "./os_child_process" EXE;
#endif // VX_OS_WINDOWS

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
        VX_CHECK(os::this_process::get_environment_variable(var.first).empty());
    }

    VX_SECTION("stdio")
    {
        auto p_stdin = os::this_process::get_stdin();
        VX_CHECK(p_stdin.is_open());
        VX_CHECK(p_stdin.can_read());
        VX_CHECK(!p_stdin.can_write());

        auto p_stdout = os::this_process::get_stdout();
        VX_CHECK(p_stdout.is_open());
        VX_CHECK(!p_stdout.can_read());
        VX_CHECK(p_stdout.can_write());
        VX_CHECK(p_stdout.write_line("  testing stdout"));

        auto p_stderr = os::this_process::get_stderr();
        VX_CHECK(p_stderr.is_open());
        VX_CHECK(!p_stderr.can_read());
        VX_CHECK(p_stderr.can_write());
        VX_CHECK(p_stderr.write_line("  testing stderr"));
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
    int exit_code;
    VX_CHECK(!p.get_exit_code(&exit_code));

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
    cfg.stdout_option = os::process::io_option::inherit;
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

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_process_move_operators)
{
    os::process::config cfg;
    cfg.args = { child_process, "--stall" };

    os::process p1;
    VX_CHECK_AND_EXPECT_NO_ERROR(p1.start(cfg));

    VX_CHECK(p1.is_valid());
    VX_CHECK(p1.is_alive());
    const os::process::id pid = p1.get_pid();
    VX_CHECK(pid != 0);

    VX_DISABLE_MSVC_WARNING_PUSH();
    VX_DISABLE_MSVC_WARNING(26800); // disable use after move warning

    // Move construction
    os::process p2(std::move(p1));

    VX_CHECK(!p1.is_valid()); // Original process should be closed
    VX_CHECK(p1.get_pid() == 0);

    VX_CHECK(p2.is_valid());
    VX_CHECK(p2.is_alive());
    VX_CHECK(p2.get_pid() == pid);

    // Move assignment
    os::process p3;
    p3 = std::move(p2);

    VX_CHECK(!p2.is_valid()); // Original process should be closed
    VX_CHECK(p2.get_pid() == 0);

    VX_CHECK(p3.is_valid());
    VX_CHECK(p3.is_alive());
    VX_CHECK(p3.get_pid() == pid);

    VX_CHECK(p3.kill());
}

///////////////////////////////////////////////////////////////////////////////

static std::string echo_argument(const std::string& arg)
{
    os::process::config cfg;

    // create a new stdout stream
    cfg.stdout_option = os::process::io_option::create;

#if defined(VX_OS_WINDOWS)

    cfg.args = { "cmd.exe", "/C", "echo", arg };

#else

    cfg.args = { "/bin/echo", arg };

#endif // VX_OS_WINDOWS

    os::process p;
    VX_CHECK_AND_EXPECT_NO_ERROR(p.start(cfg));

    VX_CHECK(p.is_valid());
    VX_CHECK(p.join());
    VX_CHECK(p.is_complete());

    int code;
    VX_CHECK(p.get_exit_code(&code));
    VX_CHECK(code == 0);

    // read the value from the stream
    auto& p_stdout = p.get_stdout();

    std::string echo;
    VX_CHECK(p_stdout.read_line(echo));

    return echo;
}

VX_TEST_CASE(test_arguments)
{
    VX_CHECK(echo_argument("hello") == "hello");

#if defined(VX_OS_WINDOWS)

    VX_CHECK(echo_argument(" ") == "\" \"");
    VX_CHECK(echo_argument("a b c") == "\"a b c\"");
    VX_CHECK(echo_argument("a\tb\tc\t") == "\"a\tb\tc\t\"");
    VX_CHECK(echo_argument("(something)") == "\"(something)\"");
    VX_CHECK(echo_argument("\"quoted argument\"") == "\"\"quoted argument\"\"");
    VX_CHECK(echo_argument("C:\\Path\\to\\file\\") == "C:\\Path\\to\\file\\");
    VX_CHECK(echo_argument("C:\\Folder\\\"Test\"") == "\"C:\\Folder\\\\\"Test\"\"");

#else

    VX_CHECK(echo_argument(" ") == " ");
    VX_CHECK(echo_argument("a b c") == "a b c");
    VX_CHECK(echo_argument("a\tb\tc\t") == "a\tb\tc\t");
    VX_CHECK(echo_argument("(something)") == "(something)");
    VX_CHECK(echo_argument("\"quoted argument\"") == "\"quoted argument\"");
    VX_CHECK(echo_argument("/Path/to/file/") == "/Path/to/file/");
    VX_CHECK(echo_argument("/Folder/\"Test\"") == "/Folder/\"Test\"");

#endif // VX_OS_WINDOWS
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(text_exit_code)
{
    const int exit_codes[] = { 0, 1, 13, 31, 127, 255 };

    for (auto ec : exit_codes)
    {
        // Configure the process
        os::process::config config;
        config.args = { child_process, "--exit-code", std::to_string(ec) };
        // on unix we need the process to be a child to get the exit code
        config.background = false;

        os::process p;
        VX_CHECK(p.start(config));
        VX_CHECK(p.join());

        int exit_code;
        VX_CHECK(p.get_exit_code(&exit_code));
        VX_CHECK(exit_code == ec);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_environment)
{
    VX_SECTION("inherited")
    {
        // Configure the process
        os::process::config config;
        config.args = { child_process, "--show-environment" };
        config.stdout_option = os::process::io_option::create;

        // Start the process
        os::process p;
        VX_CHECK(p.start(config));

        // make sure the child process finishes
        VX_CHECK(p.join());
        VX_CHECK(p.is_complete());

        int exit_code;
        VX_CHECK(p.get_exit_code(&exit_code));
        VX_CHECK(exit_code == 0);

        // build the environment
        os::process::environment child_env;
        auto& p_stdout = p.get_stdout();

        std::string line;
        while (p_stdout.read_line(line))
        {
            const auto sep = line.find('=');
            child_env[line.substr(0, sep)] = line.substr(sep + 1);
        }

        VX_CHECK(!child_env.empty());
        VX_CHECK(p_stdout.eof());

        // environments should match
        VX_CHECK(os::this_process::get_environment() == child_env);
    }

    VX_SECTION("new")
    {
        // Configure the process
        os::process::config config;
        config.args = { child_process, "--show-environment" };
        config.stdout_option = os::process::io_option::create;
        config.environment["test_new_environment"] = "true";

        // Start the process
        os::process p;
        VX_CHECK(p.start(config));

        // make sure the child process finishes
        VX_CHECK(p.join());
        VX_CHECK(p.is_complete());

        int exit_code;
        VX_CHECK(p.get_exit_code(&exit_code));
        VX_CHECK(exit_code == 0);

        // build the environment
        os::process::environment child_env;
        auto& p_stdout = p.get_stdout();

        std::string line;
        while (p_stdout.read_line(line))
        {
            const auto sep = line.find('=');
            child_env[line.substr(0, sep)] = line.substr(sep + 1);
        }

        VX_CHECK(!child_env.empty());
        VX_CHECK(p_stdout.eof());

        // environments should match
        VX_CHECK(config.environment == child_env);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_kill)
{
    // Configure the process
    os::process::config config;
    config.args = { child_process, "--stall" };
    config.stdout_option = os::process::io_option::create;

    // Start the process
    os::process p;
    VX_CHECK(p.start(config));

    // process should be stalling
    VX_CHECK(!p.is_complete());

    // kill the process
    VX_CHECK(p.kill());
    VX_CHECK(p.join());
    VX_CHECK(p.is_complete());

    // should have exit code of 1
    int exit_code;
    VX_CHECK(p.get_exit_code(&exit_code));
    VX_CHECK(exit_code == 1);
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
    config.stdin_option = os::process::io_option::create;
    config.stdout_option = os::process::io_option::create;

    // Start the process
    os::process p;
    VX_CHECK(p.start(config));

    // Get stdin pipe
    auto& p_stdin = p.get_stdin();
    VX_CHECK(p_stdin.is_open());
    VX_CHECK(p_stdin.can_write());

    // write each line
    for (auto line : lines)
    {
        VX_CHECK(p_stdin.write_line(line));
    }

    // finish the process
    VX_CHECK(p_stdin.write_line("EOF"));
    VX_CHECK(p.join());
    VX_CHECK(p.is_complete());

    // Get stdout pipe
    auto& p_stdout = p.get_stdout();
    VX_CHECK(p_stdout.is_open());
    VX_CHECK(p_stdout.can_read());

    // read the lines back
    std::string read_line;
    for (auto line : lines)
    {
        VX_CHECK(p_stdout.read_line(read_line));
        VX_CHECK(read_line == line);
    }

    VX_CHECK(p_stdout.eof());

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
    config.stdin_option = os::process::io_option::create;
    config.stderr_option = os::process::io_option::create;

    // Start the process
    os::process p;
    VX_CHECK(p.start(config));

    // Get stdin pipe
    auto& p_stdin = p.get_stdin();
    VX_CHECK(p_stdin.is_open());
    VX_CHECK(p_stdin.can_write());

    // write each line
    for (auto line : lines)
    {
        VX_CHECK(p_stdin.write_line(line));
    }

    // finish the process
    VX_CHECK(p_stdin.write_line("EOF"));
    VX_CHECK(p.join());
    VX_CHECK(p.is_complete());

    // Get stderr pipe
    auto& p_stderr = p.get_stderr();
    VX_CHECK(p_stderr.is_open());
    VX_CHECK(p_stderr.can_read());

    // read the lines back
    std::string read_line;
    for (auto line : lines)
    {
        VX_CHECK(p_stderr.read_line(read_line));
        VX_CHECK(read_line == line);
    }

    VX_CHECK(p_stderr.eof());

    // Ensure the process exits correctly
    int exit_code;
    VX_CHECK(p.get_exit_code(&exit_code));
    VX_CHECK(exit_code == 0);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_multiprocess_stdin_to_stdout)
{
    const char* lines[] = {
        "Tests whether we can write to stdin and read from stdout accross 2 child processes",
        "{'success': true, 'message': 'Success!'}",
        "Yippie ka yee",
        "EOFF"
    };

    os::process::config config;
    config.args = { child_process, "--stdin-to-stdout" };

    // Configure and start the first process
    config.stdin_option = os::process::io_option::create;
    config.stdout_option = os::process::io_option::create;
    os::process p1;
    VX_CHECK(p1.start(config));

    // Configure and start the second process
    config.stdin_option = os::process::io_option::redirect;
    config.stdin_redirect = &p1.get_stdout();
    config.stdout_option = os::process::io_option::create;
    os::process p2;
    VX_CHECK(p2.start(config));

    // Get stdin pipe from first process
    auto& p_stdin = p1.get_stdin();
    VX_CHECK(p_stdin.is_open());
    VX_CHECK(p_stdin.can_write());

    // write each line
    for (auto line : lines)
    {
        VX_CHECK(p_stdin.write_line(line));
    }

    // finish the first process
    VX_CHECK(p1.join());
    VX_CHECK(p1.is_complete());

    int exit_code;
    VX_CHECK(p1.get_exit_code(&exit_code));
    VX_CHECK(exit_code == 0);

    // finish the second process
    VX_CHECK(p2.join());
    VX_CHECK(p2.is_complete());

    VX_CHECK(p2.get_exit_code(&exit_code));
    VX_CHECK(exit_code == 0);

    // Get stdout pipe from second process
    auto& p_stdout = p2.get_stdout();
    VX_CHECK(p_stdout.is_open());
    VX_CHECK(p_stdout.can_read());
    
    // read the lines back
    std::string read_line;
    for (auto line : lines)
    {
        VX_CHECK(p_stdout.read_line(read_line));
        VX_CHECK(read_line == line);
    }

    VX_CHECK(p_stdout.eof());
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_write_to_completed_process)
{
    os::process p;

    os::process::config cfg;
    cfg.args = { child_process, "--hello-world" };
    cfg.stdin_option = os::process::io_option::create;
    VX_CHECK_AND_EXPECT_NO_ERROR(p.start(cfg));

    VX_CHECK(p.is_valid());
    VX_CHECK(p.get_pid() != 0);
    VX_CHECK(p.join());
    VX_CHECK(!p.is_alive());
    VX_CHECK(p.is_complete());

    int code;
    VX_CHECK(p.get_exit_code(&code));
    VX_CHECK(code == 0);

    // the pipe should be closed, writing should fail
    VX_CHECK_AND_EXPECT_ERROR(!p.get_stdin().write_line("hello"));
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_nonexisting_executable)
{
    os::process::config cfg;
    cfg.args = { "fake_exe" EXE };

    os::process p;
    VX_CHECK_AND_EXPECT_ERROR(!p.start(cfg));
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_stream_redirection)
{
    const os::path stdin_path = "stdin.txt";
    // create the parent process input side of the pipe
    os::io_stream stream;
    VX_CHECK(stream.open(stdin_path, os::file::mode::write));

    // create the child process input side of the pipe
    os::file p_stdin;
    VX_CHECK(p_stdin.open(stdin_path, os::file::mode::read));

    const os::path stdout_path = "stdout.txt";
    os::file p_stdout;
    VX_CHECK(p_stdout.open(stdout_path, os::file::mode::write));

    const os::path stderr_path = "stderr.txt";
    os::file p_stderr;
    VX_CHECK(p_stderr.open(stderr_path, os::file::mode::write));

    VX_SECTION("stdin to stdout redirection")
    {
        const char* stdin_to_stdout_redirect_text = "testing stdin to stdout redirection";

        os::process::config cfg;
        cfg.args = { child_process, "--stdin-to-stdout" };

        cfg.stdin_option = os::process::io_option::redirect;
        cfg.stdin_redirect = &p_stdin;

        cfg.stdout_option = os::process::io_option::redirect;
        cfg.stdout_redirect = &p_stdout;

        // Start the process
        os::process p;
        VX_CHECK(p.start(cfg));

        // write each line
        VX_CHECK(stream.write_line(stdin_to_stdout_redirect_text));

        // finish the process
        VX_CHECK(stream.write_line("EOF"));
        VX_CHECK(p.join());
        VX_CHECK(p.is_complete());

        // Ensure the process exits correctly
        int exit_code;
        VX_CHECK(p.get_exit_code(&exit_code));
        VX_CHECK(exit_code == 0);

        os::io_stream stdout_stream;
        VX_CHECK(stdout_stream.open(stdout_path, os::file::mode::read));

        // read the lines back
        std::string read_line;
        VX_CHECK(stdout_stream.read_line(read_line));
        VX_CHECK(read_line == stdin_to_stdout_redirect_text);
        VX_CHECK(stdout_stream.eof());
    }

    VX_SECTION("stdin to stderr redirection")
    {
        const char* stdin_to_stderr_redirect_text = "testing stdin to stderr redirection";

        os::process::config cfg;
        cfg.args = { child_process, "--stdin-to-stderr" };

        cfg.stdin_option = os::process::io_option::redirect;
        cfg.stdin_redirect = &p_stdin;

        cfg.stderr_option = os::process::io_option::redirect;
        cfg.stderr_redirect = &p_stderr;

        // Start the process
        os::process p;
        VX_CHECK(p.start(cfg));

        // write each line
        VX_CHECK(stream.write_line(stdin_to_stderr_redirect_text));

        // finish the process
        VX_CHECK(stream.write_line("EOF"));
        VX_CHECK(p.join());
        VX_CHECK(p.is_complete());

        // Ensure the process exits correctly
        int exit_code;
        VX_CHECK(p.get_exit_code(&exit_code));
        VX_CHECK(exit_code == 0);

        os::io_stream stderr_stream;
        VX_CHECK(stderr_stream.open(stderr_path, os::file::mode::read));

        // read the lines back
        std::string read_line;
        VX_CHECK(stderr_stream.read_line(read_line));
        VX_CHECK(read_line == stdin_to_stderr_redirect_text);
        VX_CHECK(stderr_stream.eof());
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}