#include "sandbox/sandbox.h"

#include "vertex/math/math/transform2d.h"
#include "vertex/math/math/transform3d.h"

int main()
{
    using namespace vx;

    math::mat2 r3 = math::mat2_make_rotation(0.0f);
    math::mat3 r = math::mat3_make_rotation(0.2f, 3.95f, -0.3f);
    math::mat4 r2 = math::make_rotation_3d(0.2f, 3.95f, -0.3f);

    auto x = math::mat3_get_rotation(r);

    return 0;
}