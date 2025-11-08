#include "vertex_test/test.hpp"
#include "vertex/util/bit/endian.hpp"

VX_TEST_CASE(endian)
{
    switch (vx::endian::order::native)
    {
        default:
        case vx::endian::order::little:
        {
            VX_MESSAGE("  Order: little");
            break;
        }
        case vx::endian::order::big:
        {
            VX_MESSAGE("  Order: big");
            break;
        }
    }
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}