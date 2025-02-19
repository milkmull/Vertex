#include "vertex_test/test.hpp"
#include "vertex/os/process.hpp"

using namespace vx;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_traits)
{
    VX_STATIC_CHECK(!std::is_copy_constructible<os::process>::value);
    VX_STATIC_CHECK(!std::is_copy_assignable<os::process>::value);

    VX_STATIC_CHECK(std::is_move_constructible<os::process>::value);
    VX_STATIC_CHECK(std::is_move_assignable<os::process>::value);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_this_process_get_pid)
{
    VX_CHECK_AND_EXPECT_NO_ERROR(os::this_process::get_pid() != 0);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_this_process_environment)
{
    os::process::environment env1;
    VX_EXPECT_NO_ERROR(env1 = os::this_process::get_environment());
    VX_CHECK(!env1.empty());

    const std::pair<std::string, std::string> var{ "test_variable", "hello world" };
    VX_CHECK_AND_EXPECT_NO_ERROR(os::this_process::set_environment_variable(var.first, var.second));

    os::process::environment env2;
    VX_EXPECT_NO_ERROR(env2 = os::this_process::get_environment());
    VX_CHECK(!env2.empty());
    VX_CHECK(env2.count(var.first) != 0);
    VX_CHECK(env2[var.first] == var.second);

    std::string value;
    VX_EXPECT_NO_ERROR(value = os::this_process::get_environment_variable(var.first));
    VX_CHECK(!value.empty());
    VX_CHECK(value == var.second);

    VX_CHECK_AND_EXPECT_NO_ERROR(os::this_process::clear_environment_variable(var.first));
    VX_CHECK_AND_EXPECT_ERROR(os::this_process::get_environment_variable(var.first).empty());
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_process)
{
    os::process p;

    VX_SECTION("initial state")
    {
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

    VX_SECTION("echo")
    {
        os::process::config cfg;

#   if defined(VX_PLATFORM_WINDOWS)

        cfg.args = { "cmd.exe", "/C", "echo", "hello" };

#   else

        cfg.args = { "/bin/echo", "hello" };

#   endif // VX_PLATFORM_WINDOWS

        VX_CHECK_AND_EXPECT_NO_ERROR(p.start(cfg));

        VX_CHECK(p.is_valid());
        VX_CHECK(p.get_pid() != 0);
        VX_CHECK(!p.is_alive());
        VX_CHECK(p.is_complete());

        int code;
        VX_CHECK(p.get_exit_code(&code));
        VX_CHECK(code == 0);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}