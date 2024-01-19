#include <iostream>

#include "vertex/math/math.h"
#include "vertex/math/random/rng.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    mat4 m;

    for (auto col : m)
    {
        for (auto v : col)
        {
            std::cout << v << std::endl;
        }
    }

    return 0;
}

#endif // VX_WINDOWS