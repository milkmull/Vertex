#include <iostream>
#include <map>

#include "vertex/math/math.h"
#include "vertex/math/random/rng.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    math::rng rng;

    vec2 v(rng.randf(), rng.randf());
    vec2i vi;

    math::frexp(v, vi);

    std::cout << vi.to_string();

    return 0;
}

#endif // VX_WINDOWS