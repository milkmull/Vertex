#include "vertex_test/std/vector/vector_profile_tools.hpp"

//=========================================================================

int main()
{
    // single warmup covering BOTH implementations, so both start from the
    // same steady-state frequency before any measured sample is taken
    run_combined<vec1<trivial_type>, vec2<trivial_type>>(NN, static_cast<size_t>(RR * 0.1f));
    //run_combined<vec1<non_trivial_type>, vec2<non_trivial_type>>(NN, static_cast<size_t>(RR * 0.1f));

    VX_PROFILE_START("profile_vector.csv");

    run_combined<vec1<trivial_type>, vec2<trivial_type>>(NN, RR);
    //run_combined<vec1<non_trivial_type>, vec2<non_trivial_type>>(NN, RR);

    VX_PROFILE_STOP();
    return 0;
}
