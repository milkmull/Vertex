#pragma once

#include <chrono>
#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "vertex/config/language_config.hpp"
#include "vertex/config/platform.hpp"
#include "vertex_test/test_util.hpp"

namespace vx {
namespace test {

// Thrown by a fatal check after the failure has already been recorded.
struct test_failure
{};

// Thrown by VX_SKIP to abort a test without failing it.
struct test_skipped
{
    std::string reason;
};

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
        m_tests.push_back({ name, std::move(func) });
    }

    void record_failure(const char* file, int line, const std::string& msg)
    {
        ++m_failures;
        std::cerr << file << ':' << line << ": error: " << msg;
        if (!m_section.empty())
        {
            std::cerr << "  (section: " << m_section << ')';
        }
        std::cerr << std::endl;
    }

    void set_section(const char* s)
    {
        m_section = s;
    }

    // Usage: run(argc, argv) -- supports `--filter <substring>`.
    int run(int argc = 0, char** argv = nullptr)
    {
        std::string filter;
        for (int i = 1; i + 1 < argc; ++i)
        {
            if (std::string(argv[i]) == "--filter")
            {
                filter = argv[i + 1];
            }
        }

        std::vector<std::string> failed;
        std::size_t ran = 0, skipped = 0;

        for (const auto& t : m_tests)
        {
            if (!filter.empty() && t.name.find(filter) == std::string::npos)
            {
                continue;
            }

            ++ran;
            m_failures = 0;
            m_section.clear();
            bool was_skipped = false;
            const auto start = std::chrono::steady_clock::now();
            std::cout << "Running test: " << t.name << "..." << std::endl;

            try
            {
                t.func();
            }
            catch (const test_failure&)
            {} // already recorded
            catch (const test_skipped& s)
            {
                was_skipped = true;
                std::cout << "  [SKIP] " << s.reason << '\n';
            }
            catch (const std::exception& e)
            {
                record_failure("<unknown>", 0, std::string("unexpected exception: ") + e.what());
            }
            catch (...)
            {
                record_failure("<unknown>", 0, "unexpected non-standard exception");
            }

            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start)
                                .count();

            const char* status = "[PASS]";
            if (was_skipped)
            {
                ++skipped;
                status = "[SKIP]";
            }
            else if (m_failures)
            {
                failed.push_back(t.name);
                status = "[FAIL]";
            }

            std::cout << "  " << status << ' ' << t.name << " (" << ms << " ms)\n\n";
        }

        if (ran == 0 && !filter.empty())
        {
            std::cout << "No tests matched filter \"" << filter << "\"\n";
        }

        std::cout << "Summary: " << ran - failed.size() - skipped << " passed, "
                  << failed.size() << " failed, " << skipped << " skipped\n";
        for (const auto& n : failed)
        {
            std::cout << "  failed: " << n << '\n'; // recap at the bottom
        }

        return failed.empty() ? 0 : 1;
    }

private:

    test_runner() = default;

    std::vector<TestCase> m_tests;
    std::string m_section;
    int m_failures = 0;
};

namespace _priv {

inline void check_failed(bool fatal, const char* file, int line, const std::string& msg)
{
    test_runner::instance().record_failure(file, line, msg);

    if (fatal)
    {
        throw test_failure{};
    }
}

template <typename... Args>
void message(Args&&... args)
{
    (std::cout << ... << args) << std::endl;
}

} // namespace _priv

struct registrar
{
    registrar(const char* n, void (*f)())
    {
        test_runner::instance().add_test(n, f);
    }
};

//=============================================================================
// test registration / structure
//=============================================================================

// Works at global or namespace scope.
#define VX_TEST_CASE(name) \
    static void name(); \
    namespace { \
    [[maybe_unused]] const ::vx::test::registrar name##_registrar_(#name, name); \
    } \
    static void name()

#define VX_SECTION(name) \
    do \
    { \
        ::vx::test::test_runner::instance().set_section(name); \
        std::cout << "  [SECTION] " << name << std::endl; \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

// e.g. VX_SKIP("platform without char8_t");
#define VX_SKIP(msg) \
    throw ::vx::test::test_skipped \
    { \
        msg \
    }

//=============================================================================
// checks
//=============================================================================

#define VX_DETAIL_CHECK(fatal, ...) \
    do \
    { \
        if (!(__VA_ARGS__)) \
            ::vx::test::_priv::check_failed(fatal, __FILE__, __LINE__, "Check failed: " #__VA_ARGS__); \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_CHECK(...)      VX_DETAIL_CHECK(true, __VA_ARGS__)  // fatal: aborts the test
#define VX_SOFT_CHECK(...) VX_DETAIL_CHECK(false, __VA_ARGS__) // records failure, keeps going

#define VX_STATIC_CHECK(...) static_assert((__VA_ARGS__), "Static check failed: " #__VA_ARGS__)

#define VX_CHECK_ERROR(cond, e) \
    do \
    { \
        VX_CHECK((cond) == (e)); \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_CHECK_EXPECTED_ERROR(cond, e) \
    do \
    { \
        VX_CHECK((cond).error() == (e)); \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

// The expression arguments are variadic so commas in template arguments don't
// split them into separate macro parameters.
#define VX_EXPECT_ERROR(...) \
    do \
    { \
        ::vx::err::clear(); \
        __VA_ARGS__; \
        VX_CHECK(::vx::err::is_set()); \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_EXPECT_ERROR_CODE(e, ...) \
    do \
    { \
        ::vx::err::clear(); \
        __VA_ARGS__; \
        VX_CHECK(::vx::err::is_set()); \
        VX_CHECK(::vx::err::get_code() == (e)); \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_EXPECT_NO_ERROR(...) \
    do \
    { \
        ::vx::err::clear(); \
        __VA_ARGS__; \
        VX_CHECK(!::vx::err::is_set()); \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_CHECK_AND_EXPECT_ERROR(...)         VX_EXPECT_ERROR(VX_CHECK(__VA_ARGS__))
#define VX_CHECK_AND_EXPECT_ERROR_CODE(e, ...) VX_EXPECT_ERROR_CODE(e, VX_CHECK(__VA_ARGS__))
#define VX_CHECK_AND_EXPECT_NO_ERROR(...)      VX_EXPECT_NO_ERROR(VX_CHECK(__VA_ARGS__))

//=============================================================================
// running / output
//=============================================================================

// VX_RUN_TESTS()           -> run all
// VX_RUN_TESTS(argc, argv) -> supports --filter <substring>
#define VX_RUN_TESTS(...) ::vx::test::test_runner::instance().run(__VA_ARGS__)

#define VX_MESSAGE(...)      ::vx::test::_priv::message(__VA_ARGS__)
#define VX_WARNING(...)      VX_MESSAGE("  [WARNING]: ", __VA_ARGS__)
#define VX_PRINT_ERRORS(...) ::vx::err::set_hook(::vx::err::print_error_hook)

#define VX_DISABLE_USE_AFTER_MOVE_WARNING() \
    VX_DISABLE_WARNING_PUSH(); \
    VX_DISABLE_WARNING("-Wuse-after-move", 26800)

} // namespace test
} // namespace vx
