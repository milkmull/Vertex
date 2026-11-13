#include "vertex_test/std/string_convert/profile_tools.hpp"

//=========================================================================

int main()
{
    // warmup
    size_t n = test_vx_from_string(static_cast<size_t>(RR * 0.1f));

    VX_PROFILE_START_APPEND("profile_from_string.csv");
    n += test_vx_from_string(RR);
    VX_PROFILE_STOP();

    return static_cast<int>(n);
}
