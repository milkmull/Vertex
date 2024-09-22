#pragma once

#include <vector>
#include <iostream>

#include "vertex/system/log.hpp"

namespace vx {
namespace test {

class test_case
{
public:
    virtual void run() = 0;
    virtual const char* get_name() const = 0; // Method to get the test name
    virtual const char* get_section() const = 0; // Method to get the section name
};

class test_suite
{
public:
    static test_suite& get_suite()
    {
        static test_suite suite;
        return suite;
    }

    void add_test(test_case* test)
    {
        tests.push_back(test);
    }

    void run_tests()
    {
        std::string current_section;
        for (auto test : tests)
        {
            const std::string section = test->get_section();
            if (section != current_section)
            {
                std::cout << "\nRunning tests in section: " << section << std::endl;
                current_section = section;
            }

            std::cout << "Running test: " << test->get_name() << "..." << std::endl;
            try {
                test->run();
                std::cout << "Test passed: " << test->get_name() << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Test failed: " << test->get_name() << " - " << e.what() << std::endl;
            }
        }
    }

private:
    std::vector<test_case*> tests;
};

class assert
{
public:
    static void that(bool condition, const std::string& message)
    {
        if (!condition)
        {
            throw std::runtime_error("Assertion failed: " + message);
        }
    }
};

#define VX_TEST(SECTION, NAME) class SECTION##_##NAME : public ::vx::test::test_case \
{                                                                                    \
public:                                                                              \
    SECTION##_##NAME() { ::vx::test::test_suite::get_suite().add_test(this); }       \
    void run() override;                                                             \
    const char* get_name() const override { return #NAME; }                          \
    const char* get_section() const override { return #SECTION; }                    \
};                                                                                   \
SECTION##_##NAME SECTION##NAME##_test;                                               \
void SECTION##_##NAME::run()

}
}
