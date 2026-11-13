#include "vertex_test/std/string_convert/profile_tools.hpp"

//=========================================================================

int main()
{
    // warmup
    size_t n = test_std_to_chars(static_cast<size_t>(RR * 0.1f));

    VX_PROFILE_START_APPEND("profile_to_string.csv");
    n += test_std_to_chars(RR);
    VX_PROFILE_STOP();

    return static_cast<int>(n);
}
