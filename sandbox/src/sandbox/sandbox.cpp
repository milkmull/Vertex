#include <iostream>

#include "vertex/math/math/vec3.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    vx::vec3i vi(1, 2, 3);
    vx::vec3f vf(1, 2, 3);
    std::cout << vi.size() << std::endl;
    return 0;
}

#endif // VX_WINDOWS