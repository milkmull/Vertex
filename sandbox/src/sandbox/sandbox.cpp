#include <iostream>

#include "vertex/math/math.h"
#include "vertex/math/math/vec3.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    vec3 v3(2, 0, 0);
    vec3 axis = vec3(0, 0, 1.5);

    std::cout << math::rotate(v3, axis, (float)math::radians(90)).to_string(true);

    return 0;
}

#endif // VX_WINDOWS