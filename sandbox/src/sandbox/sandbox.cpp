#include <iostream>

#include "vertex/math/math/vec2.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    vx::vec2i vi(1, 2);
    vx::vec2f vf(1, 2);
    std::cout << vi.magnitude() << ' ' << vf.magnitude() << std::endl;
    return 0;
}

#endif // VX_WINDOWS