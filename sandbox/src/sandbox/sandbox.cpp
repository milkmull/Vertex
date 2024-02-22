#include "sandbox/sandbox.h"

#include "vertex/math/math/transform/matrix3.h"
#include "vertex/math/math/transform/transform3d.h"

int main()
{
    using namespace vx;

    math::mat3 r = math::transform3d::matrix3::make_rotation(0.2f, 3.95f, -0.3f);
    math::mat4 r2 = math::transform3d::make_rotation(0.2f, 3.95f, -0.3f);

    return 0;
}