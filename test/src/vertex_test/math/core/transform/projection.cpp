#include "vertex_test/test.hpp"

#include "vertex/math/core/transform/projection.hpp"
#include "vertex/math/core/functions/comparison.hpp"
#include "vertex/math/core/util/to_string.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

static bool in_frustum_zo(const mat4& proj, const vec3& p)
{
    const vec3 ndc = projection::project(proj, p);

    return (ndc.x >= -1 && ndc.x <= 1)
        && (ndc.y >= -1 && ndc.y <= 1)
        && (ndc.z >= +0 && ndc.z <= 1);
}

static bool in_frustum_no(const mat4& proj, const vec3& p)
{
    const vec3 ndc = projection::project(proj, p);

    return (ndc.x >= -1 && ndc.x <= 1)
        && (ndc.y >= -1 && ndc.y <= 1)
        && (ndc.z >= -1 && ndc.z <= 1);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(ortho)
{
    VX_SECTION("lh_zo")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = (znear + zfar) * 0.5f;

        constexpr mat4 proj = projection::ortho_lh_zo(left, right, top, bottom, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f / 99.0f, 0.0f },
                { -0.0f, -0.0f, -1.0f / 99.0f, 1.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.5f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_zo(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, (znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_zo(proj, { left + eps, bottom + eps, znear + eps }));
            VX_CHECK(in_frustum_zo(proj, { right - eps, top - eps, zfar - eps }));

            // On near/far planes
            VX_CHECK(in_frustum_zo(proj, { 0, 0, znear }));
            VX_CHECK(in_frustum_zo(proj, { 0, 0, zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_zo(proj, { left - out, bottom, znear + eps }));
            VX_CHECK(!in_frustum_zo(proj, { right + out, top, znear + eps }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, znear - eps }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, zfar + out }));
        }
    }

    VX_SECTION("lh_no")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = (znear + zfar) * 0.5f;

        constexpr mat4 proj = projection::ortho_lh_no(left, right, top, bottom, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 2.0f / 99.0f, 0.0f },
                { -0.0f, -0.0f, -101.0f / 99.0f, 1.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.0f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_no(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, (znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_no(proj, { left + eps, bottom + eps, znear + eps }));
            VX_CHECK(in_frustum_no(proj, { right - eps, top - eps, zfar - eps }));

            // On near/far planes
            VX_CHECK(in_frustum_no(proj, { 0, 0, znear }));
            VX_CHECK(in_frustum_no(proj, { 0, 0, zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_no(proj, { left - out, bottom, znear + eps }));
            VX_CHECK(!in_frustum_no(proj, { right + out, top, znear + eps }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, znear - eps }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, zfar + out }));
        }
    }

    VX_SECTION("rh_zo")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = -(znear + zfar) * 0.5f;

        constexpr mat4 proj = projection::ortho_rh_zo(left, right, top, bottom, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, -1.0f / 99.0f, 0.0f },
                { -0.0f, -0.0f, -1.0f / 99.0f, 1.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.5f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_zo(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, -(znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_zo(proj, { left + eps, bottom + eps, -(znear + eps) }));
            VX_CHECK(in_frustum_zo(proj, { right - eps, top - eps, -(zfar - eps) }));

            // On near/far planes
            VX_CHECK(in_frustum_zo(proj, { 0, 0, -znear }));
            VX_CHECK(in_frustum_zo(proj, { 0, 0, -zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_zo(proj, { left - out, bottom, -(znear + eps) }));
            VX_CHECK(!in_frustum_zo(proj, { right + out, top, -(znear + eps) }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, -(znear - eps) }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, -(zfar + out) }));
        }
    }

    VX_SECTION("rh_no")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = -(znear + zfar) * 0.5f;

        constexpr mat4 proj = projection::ortho_rh_no(left, right, top, bottom, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, -2.0f / 99.0f, 0.0f },
                { -0.0f, -0.0f, -101.0f / 99.0f, 1.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.0f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_no(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, -(znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_no(proj, { left + eps, bottom + eps, -(znear + eps) }));
            VX_CHECK(in_frustum_no(proj, { right - eps, top - eps, -(zfar - eps) }));

            // On near/far planes
            VX_CHECK(in_frustum_no(proj, { 0, 0, -znear }));
            VX_CHECK(in_frustum_no(proj, { 0, 0, -zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_no(proj, { left - out, bottom, -(znear + eps) }));
            VX_CHECK(!in_frustum_no(proj, { right + out, top, -(znear + eps) }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, -(znear - eps) }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, -(zfar + out) }));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(frustum)
{
    VX_SECTION("lh_zo")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float a = zfar / (zfar - znear);
        const float b = -(zfar * znear) / (zfar - znear);

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = -b / (a - 0.5f);

        constexpr mat4 proj = projection::frustum_lh_zo(left, right, top, bottom, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 100.0f / 99.0f, 1.0f },
                { -0.0f, -0.0f, -100.0f / 99.0f, 0.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.5f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_zo(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, (znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_zo(proj, { left + eps, bottom + eps, znear + eps }));
            VX_CHECK(in_frustum_zo(proj, { right - eps, top - eps, zfar - eps }));

            // On near/far planes
            VX_CHECK(in_frustum_zo(proj, { 0, 0, znear }));
            VX_CHECK(in_frustum_zo(proj, { 0, 0, zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_zo(proj, { left - out, bottom, znear + eps }));
            VX_CHECK(!in_frustum_zo(proj, { right + out, top, znear + eps }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, znear - eps }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, zfar + out }));
        }
    }

    VX_SECTION("lh_no")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float a = (zfar + znear) / (zfar - znear);
        const float b = -(2.0f * zfar * znear) / (zfar - znear);

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = -b / a;

        constexpr mat4 proj = projection::frustum_lh_no(left, right, top, bottom, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 101.0f / 99.0f, 1.0f },
                { -0.0f, -0.0f, -200.0f / 99.0f, 0.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.0f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_no(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, (znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_no(proj, { left + eps, bottom + eps, znear + eps }));
            VX_CHECK(in_frustum_no(proj, { right - eps, top - eps, zfar - eps }));

            // On near/far planes
            VX_CHECK(in_frustum_no(proj, { 0, 0, znear }));
            VX_CHECK(in_frustum_no(proj, { 0, 0, zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_no(proj, { left - out, bottom, znear + eps }));
            VX_CHECK(!in_frustum_no(proj, { right + out, top, znear + eps }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, znear - eps }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, zfar + out }));
        }
    }

    VX_SECTION("rh_zo")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float a = -zfar / (zfar - znear);
        const float b = -(zfar * znear) / (zfar - znear);

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = -b / (a + 0.5f);

        constexpr mat4 proj = projection::frustum_rh_zo(left, right, top, bottom, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, -100.0f / 99.0f, -1.0f },
                { -0.0f, -0.0f, -100.0f / 99.0f, 0.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.5f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_zo(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, -(znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_zo(proj, { left + eps, bottom + eps, -(znear + eps) }));
            VX_CHECK(in_frustum_zo(proj, { right - eps, top - eps, -(zfar - eps) }));

            // On near/far planes
            VX_CHECK(in_frustum_zo(proj, { 0, 0, -znear }));
            VX_CHECK(in_frustum_zo(proj, { 0, 0, -zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_zo(proj, { left - out, bottom, -(znear + eps) }));
            VX_CHECK(!in_frustum_zo(proj, { right + out, top, -(znear + eps) }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, -(znear - eps) }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, -(zfar + out) }));
        }
    }

    VX_SECTION("rh_no")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float a = -(zfar + znear) / (zfar - znear);
        const float b = -(2.0f * zfar * znear) / (zfar - znear);

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = -b / a;

        constexpr mat4 proj = projection::frustum_rh_no(left, right, top, bottom, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, -101.0f / 99.0f, -1.0f },
                { -0.0f, -0.0f, -200.0f / 99.0f, 0.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.0f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_no(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, -(znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_no(proj, { left + eps, bottom + eps, -(znear + eps) }));
            VX_CHECK(in_frustum_no(proj, { right - eps, top - eps, -(zfar - eps) }));

            // On near/far planes
            VX_CHECK(in_frustum_no(proj, { 0, 0, -znear }));
            VX_CHECK(in_frustum_no(proj, { 0, 0, -zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_no(proj, { left - out, bottom, -(znear + eps) }));
            VX_CHECK(!in_frustum_no(proj, { right + out, top, -(znear + eps) }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, -(znear - eps) }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, -(zfar + out) }));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(perspective)
{
    VX_SECTION("lh_zo")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float fovy = 2.0f * atan(top / znear);
        const float aspect = (right - left) / (top - bottom);

        const float a = zfar / (zfar - znear);
        const float b = -(zfar * znear) / (zfar - znear);

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = -b / (a - 0.5f);

        const mat4 proj = projection::perspective_lh_zo(fovy, aspect, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 100.0f / 99.0f, 1.0f },
                { -0.0f, -0.0f, -100.0f / 99.0f, 0.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.5f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_zo(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, (znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_zo(proj, { left + eps, bottom + eps, znear + eps }));
            VX_CHECK(in_frustum_zo(proj, { right - eps, top - eps, zfar - eps }));

            // On near/far planes
            VX_CHECK(in_frustum_zo(proj, { 0, 0, znear }));
            VX_CHECK(in_frustum_zo(proj, { 0, 0, zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_zo(proj, { left - out, bottom, znear + eps }));
            VX_CHECK(!in_frustum_zo(proj, { right + out, top, znear + eps }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, znear - eps }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, zfar + out }));
        }
    }

    VX_SECTION("lh_no")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float fovy = 2.0f * atan(top / znear);
        const float aspect = (right - left) / (top - bottom);

        const float a = (zfar + znear) / (zfar - znear);
        const float b = -(2.0f * zfar * znear) / (zfar - znear);

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = -b / a;

        const mat4 proj = projection::perspective_lh_no(fovy, aspect, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 101.0f / 99.0f, 1.0f },
                { -0.0f, -0.0f, -200.0f / 99.0f, 0.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.0f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_no(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, (znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_no(proj, { left + eps, bottom + eps, znear + eps }));
            VX_CHECK(in_frustum_no(proj, { right - eps, top - eps, zfar - eps }));

            // On near/far planes
            VX_CHECK(in_frustum_no(proj, { 0, 0, znear }));
            VX_CHECK(in_frustum_no(proj, { 0, 0, zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_no(proj, { left - out, bottom, znear + eps }));
            VX_CHECK(!in_frustum_no(proj, { right + out, top, znear + eps }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, znear - eps }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, zfar + out }));
        }
    }

    VX_SECTION("rh_zo")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float fovy = 2.0f * atan(top / znear);
        const float aspect = (right - left) / (top - bottom);

        const float a = -zfar / (zfar - znear);
        const float b = -(zfar * znear) / (zfar - znear);

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = -b / (a + 0.5f);

        const mat4 proj = projection::perspective_rh_zo(fovy, aspect, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, -100.0f / 99.0f, -1.0f },
                { -0.0f, -0.0f, -100.0f / 99.0f, 0.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.5f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_zo(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, -(znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_zo(proj, { left + eps, bottom + eps, -(znear + eps) }));
            VX_CHECK(in_frustum_zo(proj, { right - eps, top - eps, -(zfar - eps) }));

            // On near/far planes
            VX_CHECK(in_frustum_zo(proj, { 0, 0, -znear }));
            VX_CHECK(in_frustum_zo(proj, { 0, 0, -zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_zo(proj, { left - out, bottom, -(znear + eps) }));
            VX_CHECK(!in_frustum_zo(proj, { right + out, top, -(znear + eps) }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, -(znear - eps) }));
            VX_CHECK(!in_frustum_zo(proj, { 0, 0, -(zfar + out) }));
        }
    }

    VX_SECTION("rh_no")
    {
        constexpr float left = -1.0f;
        constexpr float right = 1.0f;
        constexpr float bottom = -1.0f;
        constexpr float top = 1.0f;
        constexpr float znear = 1.0f;
        constexpr float zfar = 100.0f;

        const float fovy = 2.0f * atan(top / znear);
        const float aspect = (right - left) / (top - bottom);

        const float a = -zfar / (zfar - znear);
        const float b = -(zfar * znear) / (zfar - znear);

        const float cx = (left + right) * 0.5f;
        const float cy = (top + bottom) * 0.5f;
        const float cz = -b / (a + 0.5f);

        const mat4 proj = projection::perspective_rh_no(fovy, aspect, znear, zfar);

        {
            const mat4 expected(
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, -101.0f / 99.0f, -1.0f },
                { -0.0f, -0.0f, -200.0f / 99.0f, 0.0f }
            );

            VX_CHECK_EQ(proj, expected);
        }

        // Sanity: round-trip projection
        {
            const mat4 model = mat4::identity();
            const vec4 viewport(0.0f, 0.0f, 100.0f, 100.0f);

            const vec3 world_point(cx, cy, cz);
            const vec3 screen_point = projection::project_zo(world_point, model, proj, viewport);

            const vec3 expected_screen_point(50.0f, 50.0f, 0.0f);
            VX_CHECK_EQ(screen_point, expected_screen_point);

            const vec3 world_back = projection::unproject_zo(screen_point, model, proj, viewport);
            VX_CHECK_EQ(world_point, world_back);
        }

        // point tests
        {
            constexpr float eps = 0.001f;
            constexpr float out = 0.1f;

            // Inside bounds
            VX_CHECK(in_frustum_no(proj, { (left + right) * 0.5f, (top + bottom) * 0.5f, -(znear + zfar) * 0.5f }));
            VX_CHECK(in_frustum_no(proj, { left + eps, bottom + eps, -(znear + eps) }));
            VX_CHECK(in_frustum_no(proj, { right - eps, top - eps, -(zfar - eps) }));

            // On near/far planes
            VX_CHECK(in_frustum_no(proj, { 0, 0, -znear }));
            VX_CHECK(in_frustum_no(proj, { 0, 0, -zfar }));

            // Slightly outside bounds
            VX_CHECK(!in_frustum_no(proj, { left - out, bottom, -(znear + eps) }));
            VX_CHECK(!in_frustum_no(proj, { right + out, top, -(znear + eps) }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, -(znear - eps) }));
            VX_CHECK(!in_frustum_no(proj, { 0, 0, -(zfar + out) }));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}