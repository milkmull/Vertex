#include <iostream>

#include "vertex/math/math/fn_trigonometric.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    std::cout << vx::math::sin(5.0) << std::endl;
    return 0;
}

#endif // VX_WINDOWS