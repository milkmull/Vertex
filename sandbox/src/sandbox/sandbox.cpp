#include <iostream>

#include "vertex/math/math/vec4.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    vx::vec4i vi(1, 2, 3, 4);
    vx::vec4f vf(1, 2, 3, 4);
    std::cout << vi.magnitude() << std::endl;
    return 0;
}

#endif // VX_WINDOWS