#include <iostream>

#include "vertex/math/math/quaternion.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    quaternion q;

    std::cout << q.to_string() << std::endl;

    return 0;
}

#endif // VX_WINDOWS