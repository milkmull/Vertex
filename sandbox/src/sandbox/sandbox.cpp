#include <iostream>

#include "vertex/math/math.h"
#include "vertex/math/math/vec3.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    vec2 v2;
    vec2i v2i;

    std::cout << math::round(v2i);

    return 0;
}

#endif // VX_WINDOWS