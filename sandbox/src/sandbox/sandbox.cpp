#include <iostream>

#include "vertex/math/math.h"
#include "vertex/math/math/vec3.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    vec3 v3(2, 0, 0);
    vec3 axis = vec3(0, 0, 1.5);

    std::cout << math::is_equal_approx(v3, v3)

    return 0;
}

#endif // VX_WINDOWS