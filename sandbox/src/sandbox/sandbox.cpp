#include <iostream>

#include "vertex/math/math.h"
#include "vertex/math/math/vec3.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    vec3 v3;
    vec3i v3i;

    std::cout << math::dot(v3, v3);

    return 0;
}

#endif // VX_WINDOWS