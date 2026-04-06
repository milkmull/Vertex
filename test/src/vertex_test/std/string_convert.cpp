#include <cstring>

#include "vertex_test/test.hpp"
#include "vertex/std/string_convert.hpp"

using namespace vx;

VX_TEST_CASE(int_to_string)
{
    string s(65, '.');
    uint64_t value = UINT64_MAX;

    str::to_string(value, s.data(), s.size());
    VX_MESSAGE(s);

    s = "18446744073709551615";

    value = 0;
    str::from_string(s.data(), s.size(), value, true);
    VX_MESSAGE(value);
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}
