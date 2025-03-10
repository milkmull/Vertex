#include "vertex_test/test.hpp"
#include "vertex/os/shared_library.hpp"

using namespace vx;

VX_TEST_CASE(test_shared_library)
{
    VX_SECTION("shared library")
    {
        os::shared_library lib;

        // Load the library
        VX_CHECK(lib.load("shared_library"));

        // Check that the library is loaded
        VX_CHECK(lib.is_loaded());

        // Check for symbols
        VX_CHECK(lib.has("get_message"));
        VX_CHECK(lib.has("add"));
        VX_CHECK(lib.has("print_message"));

        VX_DISABLE_MSVC_WARNING(6011); // dereferencing null ptr
        VX_DISABLE_MSVC_WARNING_PUSH();

        // Test calling functions
        using get_message_t = const char* (*)();
        auto get_message = lib.get<get_message_t>("get_message");
        VX_CHECK(get_message != nullptr);
        VX_CHECK(std::string(get_message()) == "Hello from the shared library!");

        using add_t = int(*)(int, int);
        auto add = lib.get<add_t>("add");
        VX_CHECK(add != nullptr);
        VX_CHECK(add(2, 3) == 5);

        using print_message_t = void(*)(const char*);
        auto print_message = lib.get<print_message_t>("print_message");
        VX_CHECK(print_message != nullptr);
        print_message("Test message");

        VX_DISABLE_MSVC_WARNING_POP();

        // Unload the library
        lib.free();
        VX_CHECK(!lib.is_loaded());
    }

    VX_SECTION("nonexistent shared library")
    {
        os::shared_library lib;

        VX_CHECK_AND_EXPECT_ERROR(!lib.load("fake_library"));
    }
}

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
