#pragma once

#include <vector>
#include <iostream>
#include <functional>
#include <sstream>

#include "vertex/system/error.hpp"

namespace vx {
namespace test {

// Central test runner
class test_runner
{
public:

    struct TestCase
    {
        std::string name;
        std::function<void()> func;
    };

    static test_runner& instance()
    {
        static test_runner runner;
        return runner;
    }

    void add_test(const std::string& name, std::function<void()> func)
    {
        tests.push_back({ name, func });
    }

    void run()
    {
        size_t failed_count = 0;

        for (const auto& test : tests)
        {
            try
            {
                std::cout << "Running test: " << test.name << "..." << std::endl;
                test.func();
                std::cout << "  [PASS] " << test.name << std::endl;
            }
            catch (const std::exception& e)
            {
                ++failed_count;
                std::cerr << "  [FAIL] " << test.name << " - " << e.what() << std::endl;
            }

            std::cout << '\n';
        }

        std::cout << "\nTest Summary: "
            << tests.size() - failed_count << " passed, "
            << failed_count << " failed, "
            << tests.size() << " total.\n";
    }

private:

    test_runner() = default;
    std::vector<TestCase> tests;
};

// Macros for the test library
#define VX_TEST_CASE(name) \
    void name(); \
    static struct name##_registrar \
    { \
        name##_registrar() \
        { \
            ::vx::test::test_runner::instance().add_test(#name, name); \
        } \
    } name##_instance; \
    void name()

#define VX_SECTION(name) std::cout << "  [SECTION] " << name << std::endl;

#define VX_CHECK(condition) \
    do \
    { \
        if (!(condition)) \
        { \
            std::ostringstream oss; \
            oss << "Check failed: " #condition \
                << " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while (0)

#define VX_STATIC_CHECK(condition) static_assert((condition), "Static check failed: " #condition)

#define VX_CHECK_ERROR(condition, error_code) (condition); VX_CHECK(err::get().code == (error_code))

#define VX_RUN_TESTS() ::vx::test::test_runner::instance().run()

namespace __detail {

    template <typename... Args>
    void message(Args&&... args)
    {
        (std::cout << ... << args) << std::endl;
    }

} // namespace __detail

#define VX_MESSAGE(...) ::test::__detail::message(__VA_ARGS__)

}
}