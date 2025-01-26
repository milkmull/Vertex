#include "vertex/_test/test.hpp"
#include "vertex/util/bit/endian.hpp"

VX_TEST_CASE(endian)
{
    switch (vx::endian::order::NATIVE)
    {
        default:
        case vx::endian::order::LITTLE:
        {
            std::cout << "Order: LITTLE" << std::endl;
            break;
        }
        case vx::endian::order::BIG:
        {
            std::cout << "Order: BIG" << std::endl;
            break;
        }
    }
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}