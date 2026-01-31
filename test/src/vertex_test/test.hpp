#pragma once

#include <vector>
#include <iostream>
#include <functional>
#include <sstream>

#include "vertex/config/platform.hpp"
#include "vertex/config/language_config.hpp"
#define VX_ERROR_PRINTING_AVAILABLE 1
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

inline void fail_test(const char* condition, const char* func, int line)
{
    std::ostringstream oss;
    oss << "Check failed: " << condition
        << " at " << func << ":" << line;
    throw std::runtime_error(oss.str());
}

// Macros for the test library
#define VX_TEST_CASE(name) \
    static void name(); \
    static struct name##_registrar \
    { \
        name##_registrar() \
        { \
            ::vx::test::test_runner::instance().add_test(#name, name); \
        } \
    } name##_instance; \
    static void name()

#define VX_SECTION(name) std::cout << "  [SECTION] " << name << std::endl;

#define VX_CHECK(condition) \
    do \
    { \
        if (!(condition)) \
        { \
            ::vx::test::fail_test(#condition, VX_FUNCTION, VX_LINE); \
        } \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_STATIC_CHECK(condition) static_assert((condition), "Static check failed: " #condition)

#define VX_EXPECT_ERROR(condition) \
    do \
    { \
        ::vx::err::clear(); \
        condition; \
        VX_CHECK(::vx::err::is_set()); \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_EXPECT_ERROR_CODE(condition, e) \
    do \
    { \
        ::vx::err::clear(); \
        condition; \
        VX_CHECK(::vx::err::is_set()); \
        VX_CHECK(::vx::err::get_code() == e); \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_EXPECT_NO_ERROR(condition) \
    do \
    { \
        ::vx::err::clear(); \
        condition; \
        VX_CHECK(!::vx::err::is_set()); \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_CHECK_AND_EXPECT_ERROR(condition) VX_EXPECT_ERROR(VX_CHECK(condition))
#define VX_CHECK_AND_EXPECT_ERROR_CODE(condition, e) VX_EXPECT_ERROR_CODE(VX_CHECK(condition), e)
#define VX_CHECK_AND_EXPECT_NO_ERROR(condition) VX_EXPECT_NO_ERROR(VX_CHECK(condition))

#define VX_RUN_TESTS() ::vx::test::test_runner::instance().run()

namespace _priv {

    template <typename... Args>
    void message(Args&&... args)
    {
        (std::cout << ... << args) << std::endl;
    }

} // namespace _priv

#define VX_MESSAGE(...) ::vx::test::_priv::message(__VA_ARGS__)

#define VX_WARNING(...) VX_MESSAGE("  [WARNING]: ", __VA_ARGS__)

}
}
