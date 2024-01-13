#include <iostream>

#include "vertex/math/math.h"
#include "vertex/math/math/vec3.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    std::cout << math::snap(-1234.0, -25.0) << std::endl;
    std::cout << math::snap(-1234, -25) << std::endl;

    return 0;
}

#endif // VX_WINDOWS