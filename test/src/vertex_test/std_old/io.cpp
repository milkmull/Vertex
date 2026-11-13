#include "vertex/std/io.hpp"
#include "vertex_test/test.hpp"

using namespace vx;

//=============================================================================

VX_TEST_CASE(io_print_char)
{
    print('A', '\n');
#if defined(__cpp_char8_t)
    print(u8'A', '\n');
#endif
}

VX_TEST_CASE(io_print_string)
{
    print("vx print string\n");
#if defined(__cpp_char8_t)
    print(u8"vx print string\n");
#endif
}

VX_TEST_CASE(io_print_multiple_strings)
{
    print(
        "hello ",
        "world",
        "!\n");
}

VX_TEST_CASE(io_print_to_stderr)
{
    print(
        os::stream::err,
        "vx stderr print\n");
}

VX_TEST_CASE(io_println)
{
    println("vx print line");
}

VX_TEST_CASE(io_println_multiple_arguments)
{
    println(
        "value=",
        "42");
}

//=============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
