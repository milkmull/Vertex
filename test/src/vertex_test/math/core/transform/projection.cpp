#include "vertex_test/test.hpp"

#include "vertex/math/core/transform/projection.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(ortho)
{
    VX_SECTION("lh")
    {
        const float left = -1.0f;
        const float right = 1.0f;
        const float bottom = -1.0f;
        const float top = 1.0f;
        const float znear = 0.0f;
        const float zfar = 10.0f;

        const mat4 proj = projection::ortho_lh_zo(left, right, top, bottom, znear, zfar);
        const mat4 model = mat4::identity();

        const vec4 viewport(0.0f, 0.0f, 800.0f, 600.0f);

        const vec3 world_point(0.5f, -0.5f, 5.0f);

        const vec3 screen_point = projection::project(world_point, model, proj, viewport);
        const vec3 world_back = projection::unproject(screen_point, model, proj, viewport);

        VX_CHECK_EQ(world_point, world_back);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}