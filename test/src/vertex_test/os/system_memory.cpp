#include "vertex_test/test.hpp"
#include "vertex/os/system_memory.hpp"

using namespace vx;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_system_memory)
{
    os::memory_info info{};
    VX_CHECK(os::get_memory_info(info));
    VX_CHECK(info.physical > 0);
    VX_CHECK(info.free > 0);
    VX_CHECK(info.available > 0);

    VX_MESSAGE("  physical: ", info.physical, " free: ", info.free, " available: ", info.available);
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}