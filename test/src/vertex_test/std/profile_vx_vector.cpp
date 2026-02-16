#include "vertex_test/std/vector_profile_tools.hpp"

//=========================================================================

// Testing shows that allocating memory aligned to the element type is often faster than using no explicit alignment.
// Using an optimal alignment can provide additional speedups by taking advantage of vectorization. Any alignment
// introduces a small amount of wasted memory at the start of the allocation. In typical use cases the performance
// gains are negligible, but for large vectors aligned memory is beneficial, as it enables more effective SIMD
// vectorization.

int main()
{
    // warmup
    run<vec2<trivial_type>>(NN, static_cast<size_t>(RR * 0.1f));

    VX_PROFILE_START("profile_vector.csv");

    run<vec2<trivial_type>>(NN, RR);
    run<vec2<non_trivial_type>>(NN, RR);

    VX_PROFILE_STOP();
    return 0;
}
