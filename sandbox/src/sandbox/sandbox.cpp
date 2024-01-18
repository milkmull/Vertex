#include <iostream>

#include "vertex/math/math.h"
#include "vertex/math/math/vec3.h"

#ifdef VX_PLATFORM_WINDOWS

int main()
{
    using namespace vx;

    vec3f v3f(math::radians(30), math::radians(60), math::radians(90));
    vec3i v3i;
    vec3i v3i2(v3f);

    std::cout << math::degrees(v3i2).to_string() << std::endl;

    return 0;
}

#endif // VX_WINDOWS