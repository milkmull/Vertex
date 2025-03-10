#include "vertex_test/test.hpp"
#include "vertex/util/bit/endian.hpp"

VX_TEST_CASE(endian)
{
    switch (vx::endian::order::NATIVE)
    {
        default:
        case vx::endian::order::LITTLE:
        {
            VX_MESSAGE("  Order: LITTLE");
            break;
        }
        case vx::endian::order::BIG:
        {
            VX_MESSAGE("  Order: BIG");
            break;
        }
    }
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}