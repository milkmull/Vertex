#include "sandbox/sandbox.h"

#include "vertex/math/math/type/vec2.h"
#include "vertex/math/math/type/vec3.h"
#include "vertex/math/math/type/vec4.h"
#include "vertex/math/math/type/mat2x4.h"
#include "vertex/math/math/type/mat4x2.h"

int main()
{
    using namespace vx;

    constexpr math::mat2x4 m1(1, 2, 3, 4, 5, 6, 7, 8);
    constexpr math::mat4x2 m2(1, 2, 3, 4, 5, 6, 7, 8);

    constexpr auto x = m2 * m1;

    return 0;
}