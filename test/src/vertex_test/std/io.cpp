#include "vertex/std/io.hpp"
#include "vertex_test/test.hpp"

using namespace vx;

//=============================================================================

VX_TEST_CASE(io_print_char)
{
    print('A', '\n');
    print(u8'A', '\n');
    print(L'A', '\n');
    print(u'A', '\n');
    print(U'A', '\n');
}

VX_TEST_CASE(io_print_string)
{
    print("vx print string\n");
#if defined(__cpp_char8_t)
    print(u8"vx print string\n");
#endif
    print(L"vx print string\n");
    print(u"vx print string\n");
    print(U"vx print string\n");
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
#if defined(__cpp_char8_t)
    println(u8"vx print line");
#endif
    println(L"vx print line");
    println(u"vx print line");
    println(U"vx print line");
}

VX_TEST_CASE(io_println_multiple_arguments)
{
    println(
        "value=",
        "42");
}

VX_TEST_CASE(io_print_mixed_character_types)
{
    print(
        "char=",
        'A',
        " wide=",
        L'B',
        "\n");
}

//=============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
