#include <cstring>

#include "vertex_test/test.hpp"
#include "vertex/std/string_convert.hpp"

using namespace vx;

VX_TEST_CASE(int_to_string)
{
    str::basic_string s(65, '.');
    long double value = 1234.5678;

    str::float_formatter fmt{};
    fmt.force_sign = true;
    fmt.format = str::float_format::hex;
    fmt.precision = 4;

    s = str::to_string(value, fmt);
    VX_MESSAGE(s);
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}
