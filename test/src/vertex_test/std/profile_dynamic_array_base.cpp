#include "vertex_test/std/vector_profile_tools.hpp"

//=========================================================================

int main()
{
    // warmup
    run<vec2<trivial_type>>(static_cast<size_t>(NN * 0.1f), RR);

    VX_PROFILE_START("profile_dynamic_array.csv");

    run<vec2<trivial_type>>(NN, RR);
    run<vec2<non_trivial_type>>(NN, RR);

    VX_PROFILE_STOP();
    return 0;
}
