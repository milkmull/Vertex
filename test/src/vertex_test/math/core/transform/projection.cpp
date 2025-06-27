#include "vertex_test/test.hpp"

#include "vertex/math/core/transform/projection.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(ortho)
{
    VX_SECTION("lh_zo")
    {
        const float left = -1.0f;
        const float right = 1.0f;
        const float bottom = -1.0f;
        const float top = 1.0f;
        const float znear = 0.0f;
        const float zfar = 10.0f;

        const mat4 proj = projection::ortho_lh_zo(left, right, top, bottom, znear, zfar);
        const mat4 model = mat4::identity();
        const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

        // Sanity: round-trip projection
        {
            const vec3 world_point(0.5f, -0.5f, 5.0f);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);
            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // Check: view volume corner -> NDC (-1, -1, 0)
        {
            const vec4 corner(left, bottom, znear, 1.0f);
            const vec4 ndc = proj * corner;
            VX_CHECK_EQ(ndc.x, -1.0f);
            VX_CHECK_EQ(ndc.y, -1.0f);
            VX_CHECK_EQ(ndc.z, 0.0f);
        }

        // Check: view volume corner -> NDC (+1, +1, 1)
        {
            const vec4 corner(right, top, zfar, 1.0f);
            const vec4 ndc = proj * corner;
            VX_CHECK_EQ(ndc.x, 1.0f);
            VX_CHECK_EQ(ndc.y, 1.0f);
            VX_CHECK_EQ(ndc.z, 1.0f);
        }

        // Check: center maps to (0, 0, 0.5)
        {
            const float cx = (left + right) * 0.5f;
            const float cy = (top + bottom) * 0.5f;
            const float cz = (znear + zfar) * 0.5f;
            const vec4 center(cx, cy, cz, 1.0f);
            const vec4 ndc = proj * center;
            VX_CHECK_EQ(ndc.x, 0.0f);
            VX_CHECK_EQ(ndc.y, 0.0f);
            VX_CHECK_EQ(ndc.z, 0.5f);
        }
    }

    VX_SECTION("lh_no")
    {
        const float left = -2.0f, right = 2.0f;
        const float bottom = -1.0f, top = 3.0f;
        const float znear = 1.0f, zfar = 5.0f;

        const mat4 proj = projection::ortho_lh_no(left, right, top, bottom, znear, zfar);
        const mat4 model = mat4::identity();
        const vec4 viewport(0.0f, 0.0f, 640.0f, 480.0f);

        // Round-trip
        {
            const vec3 world_point(1.0f, 1.0f, 3.0f);
            const vec3 screen_point = projection::project_no(world_point, model, proj, viewport);
            const vec3 world_back = projection::unproject_no(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // Corner -> NDC
        {
            const vec4 corner(left, bottom, znear, 1.0f);
            const vec4 ndc = proj * corner;
            VX_CHECK_EQ(ndc.x, -1.0f);
            VX_CHECK_EQ(ndc.y, -1.0f);
            VX_CHECK_EQ(ndc.z, -1.0f);
        }

        {
            const vec4 corner(right, top, zfar, 1.0f);
            const vec4 ndc = proj * corner;
            VX_CHECK_EQ(ndc.x, 1.0f);
            VX_CHECK_EQ(ndc.y, 1.0f);
            VX_CHECK_EQ(ndc.z, 1.0f);
        }

        // Center
        {
            const float cx = (left + right) * 0.5f;
            const float cy = (top + bottom) * 0.5f;
            const float cz = (znear + zfar) * 0.5f;
            const vec4 center(cx, cy, cz, 1.0f);
            const vec4 ndc = proj * center;
            VX_CHECK_EQ(ndc.x, 0.0f);
            VX_CHECK_EQ(ndc.y, 0.0f);
            VX_CHECK_EQ(ndc.z, 0.0f);
        }
    }

    VX_SECTION("rh_zo")
    {
        const float left = -4.0f, right = 4.0f;
        const float bottom = -2.0f, top = 2.0f;
        const float znear = 0.0f, zfar = 10.0f;

        const mat4 proj = projection::ortho_rh_zo(left, right, top, bottom, znear, zfar);
        const mat4 model = mat4::identity();
        const vec4 viewport(50.0f, 50.0f, 800.0f, 600.0f);

        // Round-trip
        {
            const vec3 world_point(2.0f, 0.0f, 10.0f);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);
            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // Corner: (left, bottom, znear) -> NDC (-1, -1, 0)
        {
            const vec4 corner(left, bottom, znear, 1.0f);
            const vec4 ndc = proj * corner;
            VX_CHECK_EQ(ndc.x, -1.0f);
            VX_CHECK_EQ(ndc.y, -1.0f);
            VX_CHECK_EQ(ndc.z, 0.0f);
        }

        // Corner: (right, top, zfar) -> NDC (+1, +1, -1)
        {
            const vec4 corner(right, top, zfar, 1.0f);
            const vec4 ndc = proj * corner;
            VX_CHECK_EQ(ndc.x, 1.0f);
            VX_CHECK_EQ(ndc.y, 1.0f);
            VX_CHECK_EQ(ndc.z, -1.0f);
        }

        // Center
        {
            const vec4 center(0.0f, 0.0f, (znear + zfar) * 0.5f, 1.0f);
            const vec4 ndc = proj * center;
            VX_CHECK_EQ(ndc.x, 0.0f);
            VX_CHECK_EQ(ndc.y, 0.0f);
            VX_CHECK_EQ(ndc.z, -0.5f);
        }
    }

    VX_SECTION("rh_no")
    {
        const float left = -10.0f, right = 10.0f;
        const float bottom = -5.0f, top = 5.0f;
        const float znear = -50.0f, zfar = 50.0f;

        const mat4 proj = projection::ortho_rh_no(left, right, top, bottom, znear, zfar);
        const mat4 model = mat4::identity();
        const vec4 viewport(0.0f, 0.0f, 1920.0f, 1080.0f);

        // Round-trip
        {
            const vec3 world_point(0.0f, 0.0f, 25.0f);
            const vec3 screen_point = projection::project_no(world_point, model, proj, viewport);
            const vec3 world_back = projection::unproject_no(screen_point, model, proj, viewport);
            VX_CHECK(equal_approx(world_point, world_back, 1e-3f));
        }

        // Corner -> NDC
        {
            const vec4 corner(left, bottom, znear, 1.0f);
            const vec4 ndc = proj * corner;
            VX_CHECK_EQ(ndc.x, -1.0f);
            VX_CHECK_EQ(ndc.y, -1.0f);
            VX_CHECK_EQ(ndc.z, +1.0f);
        }

        {
            const vec4 corner(right, top, zfar, 1.0f);
            const vec4 ndc = proj * corner;
            VX_CHECK_EQ(ndc.x, 1.0f);
            VX_CHECK_EQ(ndc.y, 1.0f);
            VX_CHECK_EQ(ndc.z, -1.0f);
        }

        // Center
        {
            const float cx = (left + right) * 0.5f;
            const float cy = (top + bottom) * 0.5f;
            const float cz = (znear + zfar) * 0.5f;
            const vec4 center(cx, cy, cz, 1.0f);
            const vec4 ndc = proj * center;
            VX_CHECK_EQ(ndc.x, 0.0f);
            VX_CHECK_EQ(ndc.y, 0.0f);
            VX_CHECK_EQ(ndc.z, 0.0f);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(frustum)
{
    VX_SECTION("lh_zo")
    {
        const float left = -1.0f, right = 1.0f;
        const float bottom = -1.0f, top = 1.0f;
        const float znear = 1.0f, zfar = 10.0f;

        const mat4 proj = projection::frustum_lh_zo(left, right, top, bottom, znear, zfar);
        const mat4 model = mat4::identity();
        const vec4 viewport(0.0f, 0.0f, 800.0f, 600.0f);

        // Round-trip
        {
            const vec3 world_point(0.5f, 0.5f, 5.0f);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);
            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK(equal_approx(world_point, world_back, 1e-4f));
        }

        // Corner: near lower-left -> NDC (-1, -1, 0)
        {
            const vec4 corner(left, bottom, znear, 1.0f);
            const vec4 clip = proj * corner;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.x, -1.0f);
            VX_CHECK_EQ(ndc.y, -1.0f);
            VX_CHECK_EQ(ndc.z, 0.0f);
        }

        // Corner: far upper-right -> NDC (1, 1, 1)
        {
            const vec4 corner(right * zfar / znear, top * zfar / znear, zfar, 1.0f);
            const vec4 clip = proj * corner;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.x, 1.0f);
            VX_CHECK_EQ(ndc.y, 1.0f);
            VX_CHECK_EQ(ndc.z, 1.0f);
        }

        // Center
        {
            const float cz = (znear + zfar) * 0.5f;
            const vec4 center(0.0f, 0.0f, cz, 1.0f);
            const vec4 clip = proj * center;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.x, 0.0f);
            VX_CHECK_EQ(ndc.y, 0.0f);
            VX_CHECK_EQ(ndc.z, 0.5f);
        }
    }

    VX_SECTION("lh_no")
    {
        const float left = -2.0f, right = 2.0f;
        const float bottom = -1.5f, top = 1.5f;
        const float znear = 2.0f, zfar = 6.0f;

        const mat4 proj = projection::frustum_lh_no(left, right, top, bottom, znear, zfar);
        const mat4 model = mat4::identity();
        const vec4 viewport(100.0f, 100.0f, 1280.0f, 720.0f);

        // Round-trip
        {
            const vec3 world_point(1.0f, 0.5f, 4.0f);
            const vec3 screen_point = projection::project_no(world_point, model, proj, viewport);
            const vec3 world_back = projection::unproject_no(screen_point, model, proj, viewport);
            VX_CHECK(equal_approx(world_point, world_back, 1e-4f));
        }

        // Corner: near bottom-left -> NDC (-1, -1, -1)
        {
            const vec4 corner(left, bottom, znear, 1.0f);
            const vec4 clip = proj * corner;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.x, -1.0f);
            VX_CHECK_EQ(ndc.y, -1.0f);
            VX_CHECK_EQ(ndc.z, -1.0f);
        }

        // Corner: far top-right -> NDC (+1, +1, +1)
        {
            const vec4 corner(right * zfar / znear, top * zfar / znear, zfar, 1.0f);
            const vec4 clip = proj * corner;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.x, 1.0f);
            VX_CHECK_EQ(ndc.y, 1.0f);
            VX_CHECK_EQ(ndc.z, 1.0f);
        }

        // Center
        {
            const vec4 center(0.0f, 0.0f, (znear + zfar) * 0.5f, 1.0f);
            const vec4 clip = proj * center;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.x, 0.0f);
            VX_CHECK_EQ(ndc.y, 0.0f);
            VX_CHECK_EQ(ndc.z, 0.0f);
        }
    }

    VX_SECTION("rh_zo")
    {
        const float left = -1.0f, right = 1.0f;
        const float bottom = -1.0f, top = 1.0f;
        const float znear = 1.0f, zfar = 11.0f;

        const mat4 proj = projection::frustum_rh_zo(left, right, top, bottom, znear, zfar);
        const mat4 model = mat4::identity();
        const vec4 viewport(0.0f, 0.0f, 1024.0f, 768.0f);

        // Round-trip
        {
            const vec3 world_point(0.25f, -0.25f, -6.0f);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);
            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK(equal_approx(world_point, world_back, 1e-4f));
        }

        // Corner: near bottom-left -> NDC (-1, -1, 1)
        {
            const vec4 corner(left, bottom, -znear, 1.0f);
            const vec4 clip = proj * corner;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.x, -1.0f);
            VX_CHECK_EQ(ndc.y, -1.0f);
            VX_CHECK_EQ(ndc.z, 1.0f);
        }

        // Corner: far top-right -> NDC (+1, +1, 0)
        {
            const vec4 corner(right * zfar / znear, top * zfar / znear, -zfar, 1.0f);
            const vec4 clip = proj * corner;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.x, 1.0f);
            VX_CHECK_EQ(ndc.y, 1.0f);
            VX_CHECK_EQ(ndc.z, 0.0f);
        }

        // Center
        {
            const vec4 center(0.0f, 0.0f, -0.5f * (znear + zfar), 1.0f);
            const vec4 clip = proj * center;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.z, 0.5f);
        }
    }

    VX_SECTION("rh_no")
    {
        const float left = -3.0f, right = 3.0f;
        const float bottom = -2.0f, top = 2.0f;
        const float znear = 2.0f, zfar = 6.0f;

        const mat4 proj = projection::frustum_rh_no(left, right, top, bottom, znear, zfar);
        const mat4 model = mat4::identity();
        const vec4 viewport(0.0f, 0.0f, 1600.0f, 900.0f);

        // Round-trip
        {
            const vec3 world_point(0.0f, 0.0f, -4.0f);
            const vec3 screen_point = projection::project_no(world_point, model, proj, viewport);
            const vec3 world_back = projection::unproject_no(screen_point, model, proj, viewport);
            VX_CHECK(equal_approx(world_point, world_back, 1e-4f));
        }

        // Corner: near bottom-left -> NDC (-1, -1, +1)
        {
            const vec4 corner(left, bottom, -znear, 1.0f);
            const vec4 clip = proj * corner;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.x, -1.0f);
            VX_CHECK_EQ(ndc.y, -1.0f);
            VX_CHECK_EQ(ndc.z, 1.0f);
        }

        // Corner: far top-right -> NDC (+1, +1, -1)
        {
            const vec4 corner(right * zfar / znear, top * zfar / znear, -zfar, 1.0f);
            const vec4 clip = proj * corner;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.x, 1.0f);
            VX_CHECK_EQ(ndc.y, 1.0f);
            VX_CHECK_EQ(ndc.z, -1.0f);
        }

        // Center
        {
            const vec4 center(0.0f, 0.0f, -0.5f * (znear + zfar), 1.0f);
            const vec4 clip = proj * center;
            const vec3 ndc = vec3(clip.x, clip.y, clip.z) / clip.w;
            VX_CHECK_EQ(ndc.z, 0.0f);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}