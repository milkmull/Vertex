#include <iostream>

#include "vertex/math/math.h"
#include "vertex/math/math/vec3.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    quaternion q;

    std::cout << math::slerp(q, q, 0.1f).to_string();

    return 0;
}

#endif // VX_WINDOWS