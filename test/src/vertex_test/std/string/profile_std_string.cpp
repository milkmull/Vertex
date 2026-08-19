#include "vertex_test/std/string/string_profile_tools.hpp"

//=========================================================================

int main()
{
    run_combined<str1, str2>(NN, static_cast<size_t>(RR * 0.1f));

    VX_PROFILE_START("profile_string.csv");

    run_combined<str1, str2>(NN, RR);

    VX_PROFILE_STOP();
    return 0;
}
