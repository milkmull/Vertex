#include <iostream>

#include "vertex/math/math.h"
#include "vertex/math/geometry/rect.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    rect2 r1;
    rect2i r1i = rect2i::ZERO();
    rect2 r2(r1i);

    std::cout << math::is_equal_approx(r1, r2) << std::endl;

    return 0;
}

#endif // VX_WINDOWS