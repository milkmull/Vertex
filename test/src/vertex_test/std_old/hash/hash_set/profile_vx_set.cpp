#include "vertex_test/std/hash/hash_profile_tools.hpp"

//=========================================================================

// Testing shows that allocating memory aligned to the element type is often faster than using no explicit alignment.
// Using an optimal alignment can provide additional speedups by taking advantage of vectorization. Any alignment
// introduces a small amount of wasted memory at the start of the allocation. In typical use cases the performance
// gains are negligible, but for large vectors aligned memory is beneficial, as it enables more effective SIMD
// vectorization.

int main()
{
    // warmup
    run<uset_flat>(UN, static_cast<size_t>(UR * 0.1f));

    VX_PROFILE_START_APPEND("profile_set.csv");

    run<uset_flat>(UN, UR);

    VX_PROFILE_STOP();
    return 0;
}
