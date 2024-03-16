#include "sandbox/sandbox.h"

#include "vertex/math/math/type/vec4.h"
#include "vertex/math/math/type/vec4i.h"

int main()
{
    using namespace vx;

    constexpr math::mat3 m(1, 1, 1, 1, 1, 1, 1, 1, 1);
    constexpr math::mat3 mi = math::invert(m);

    return 0;
}