#include "vertex_test/std/string/string_profile_tools.hpp"

//=========================================================================

// Testing shows that allocating memory aligned to the element type is often faster than using no explicit alignment.
// Using an optimal alignment can provide additional speedups by taking advantage of stringization. Any alignment
// introduces a small amount of wasted memory at the start of the allocation. In typical use cases the performance
// gains are negligible, but for large strings aligned memory is beneficial, as it enables more effective SIMD
// stringization.

int main()
{
    // warmup
    run<str2>(NN, static_cast<size_t>(RR * 0.1f));

    VX_PROFILE_START_APPEND("profile_string.csv");

    run<str2>(NN, RR);
    run<str2>(NN, RR);

    VX_PROFILE_STOP();
    return 0;
}
