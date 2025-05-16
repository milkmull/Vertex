#include "vertex_test/test.hpp"

#include "vertex/math/fn/trigonometric.hpp"
#include "vertex/math/fn/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(all_equal_approx(a, b))
#define VX_CHECK_CLOSE(a, b, epsilon) VX_CHECK(all_equal_approx(a, epsilon))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(all_equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_radians)
{
    VX_SECTION("f32")
    {
        VX_STATIC_CHECK_EQ(radians(0.0f), 0.0f);
        VX_STATIC_CHECK_EQ(radians(180.0f), 3.141592653589793f);  // pi
        VX_STATIC_CHECK_EQ(radians(90.0f), constants<f32>::half_pi);  // constants<f32>::half_pi
        VX_STATIC_CHECK_EQ(radians(-90.0f), -constants<f32>::half_pi);  // -constants<f32>::half_pi
    }

    VX_SECTION("f64")
    {
        VX_STATIC_CHECK_EQ(radians(0.0), 0.0);
        VX_STATIC_CHECK_EQ(radians(180.0), 3.141592653589793);
        VX_STATIC_CHECK_EQ(radians(90.0), 1.5707963267948966);
        VX_STATIC_CHECK_EQ(radians(-90.0), -1.5707963267948966);
    }

    VX_SECTION("int")
    {
        VX_STATIC_CHECK_EQ(radians(0), 0.0);
        VX_STATIC_CHECK_EQ(radians(180), 3.141592653589793);
        VX_STATIC_CHECK_EQ(radians(90), 1.5707963267948966);
        VX_STATIC_CHECK_EQ(radians(-90), -1.5707963267948966);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_degrees)
{
    VX_SECTION("f32")
    {
        VX_STATIC_CHECK_EQ(degrees(0.0f), 0.0f);
        VX_STATIC_CHECK_EQ(degrees(3.141592653589793f), 180.0f);  // pi
        VX_STATIC_CHECK_EQ(degrees(constants<f32>::half_pi), 90.0f);  // constants<f32>::half_pi
        VX_STATIC_CHECK_EQ(degrees(-constants<f32>::half_pi), -90.0f);  // -constants<f32>::half_pi
    }

    VX_SECTION("f64")
    {
        VX_STATIC_CHECK_EQ(degrees(0.0), 0.0);
        VX_STATIC_CHECK_EQ(degrees(3.141592653589793), 180.0);
        VX_STATIC_CHECK_EQ(degrees(1.5707963267948966), 90.0);
        VX_STATIC_CHECK_EQ(degrees(-1.5707963267948966), -90.0);
    }

    VX_SECTION("int")
    {
        VX_STATIC_CHECK_EQ(degrees(0), 0.0);
        VX_STATIC_CHECK_EQ(degrees(3), 171.8873385);            // Approx value for 3 radians
        VX_STATIC_CHECK_EQ(degrees(1), 57.29577951308232);      // Approx value for 1 radian
        VX_STATIC_CHECK_EQ(degrees(-1), -57.29577951308232);    // Approx value for -1 radian
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_atan2)
{
    VX_SECTION("scalar f32")
    {
        VX_CHECK_EQ(vx::math::atan2(1.0f, 1.0f), constants<f32>::quarter_pi);  // constants<f32>::quarter_pi
        VX_CHECK_EQ(vx::math::atan2(1.0f, -1.0f), 2.356194490192345f);  // 3constants<f32>::quarter_pi
        VX_CHECK_EQ(vx::math::atan2(-1.0f, -1.0f), -2.356194490192345f);  // -3constants<f32>::quarter_pi
        VX_CHECK_EQ(vx::math::atan2(-1.0f, 1.0f), -constants<f32>::quarter_pi);  // -constants<f32>::quarter_pi
        VX_CHECK_EQ(vx::math::atan2(0.0f, 1.0f), 0.0f);
        VX_CHECK_EQ(vx::math::atan2(1.0f, 0.0f), constants<f32>::half_pi);  // constants<f32>::half_pi
        VX_CHECK_EQ(vx::math::atan2(-1.0f, 0.0f), -constants<f32>::half_pi);  // -constants<f32>::half_pi
    }

    VX_SECTION("scalar f64")
    {
        VX_CHECK_EQ(vx::math::atan2(1.0, 1.0), 0.7853981633974483);
        VX_CHECK_EQ(vx::math::atan2(1.0, -1.0), 2.356194490192345);
        VX_CHECK_EQ(vx::math::atan2(-1.0, -1.0), -2.356194490192345);
        VX_CHECK_EQ(vx::math::atan2(-1.0, 1.0), -0.7853981633974483);
        VX_CHECK_EQ(vx::math::atan2(0.0, 1.0), 0.0);
        VX_CHECK_EQ(vx::math::atan2(1.0, 0.0), 1.5707963267948966);
        VX_CHECK_EQ(vx::math::atan2(-1.0, 0.0), -1.5707963267948966);
    }

    VX_SECTION("vec2")
    {
        const vec2f y(1.0f, -1.0f);
        const vec2f x(1.0f, 1.0f);
        const vec2f r1 = atan2(y, x);
        VX_CHECK_EQ(r1, vec2f(constants<f32>::quarter_pi, -constants<f32>::quarter_pi));

        const vec2f y2(0.0f, 1.0f);
        const vec2f x2(1.0f, 0.0f);
        const vec2f r2 = atan2(y2, x2);
        VX_CHECK_EQ(r2, vec2f(0.0f, constants<f32>::half_pi));  // constants<f32>::half_pi
    }

    VX_SECTION("vec3")
    {
        const vec3f y(1.0f, -1.0f, 0.0f);
        const vec3f x(1.0f, 1.0f, 0.0f);
        const vec3f r1 = atan2(y, x);
        VX_CHECK_EQ(r1, vec3f(constants<f32>::quarter_pi, -constants<f32>::quarter_pi, 0.0f));

        const vec3f y2(0.0f, 1.0f, 0.0f);
        const vec3f x2(1.0f, 0.0f, 0.0f);
        const vec3f r2 = atan2(y2, x2);
        VX_CHECK_EQ(r2, vec3f(0.0f, constants<f32>::half_pi, 0.0f));  // constants<f32>::half_pi
    }

    VX_SECTION("vec4")
    {
        const vec4f y(1.0f, -1.0f, 0.0f, 1.0f);
        const vec4f x(1.0f, 1.0f, 0.0f, 1.0f);
        const vec4f r1 = atan2(y, x);
        VX_CHECK_EQ(r1, vec4f(constants<f32>::quarter_pi, -constants<f32>::quarter_pi, 0.0f, constants<f32>::quarter_pi));

        const vec4f y2(0.0f, 1.0f, 0.0f, 1.0f);
        const vec4f x2(1.0f, 0.0f, 0.0f, 1.0f);
        const vec4f r2 = atan2(y2, x2);
        VX_CHECK_EQ(r2, vec4f(0.0f, constants<f32>::half_pi, 0.0f, constants<f32>::half_pi));  // constants<f32>::half_pi
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_sin)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::sin(-constants<f32>::pi), 0.0f);
        VX_CHECK_EQ(vx::math::sin(-constants<f32>::three_pi_over_two), -constants<f32>::inv_root_two);
        VX_CHECK_EQ(vx::math::sin(-constants<f32>::half_pi), -1.0f);
        VX_CHECK_EQ(vx::math::sin(-constants<f32>::quarter_pi), -constants<f32>::inv_root_two);
        VX_CHECK_EQ(vx::math::sin(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::sin(constants<f32>::quarter_pi), constants<f32>::inv_root_two);
        VX_CHECK_EQ(vx::math::sin(constants<f32>::half_pi), 1.0f);
        VX_CHECK_EQ(vx::math::sin(constants<f32>::three_pi_over_two), constants<f32>::inv_root_two);
        VX_CHECK_EQ(vx::math::sin(constants<f32>::pi), 0.0f);

        VX_CHECK_EQ(vx::math::sin(1.0), 0.84147098);
        VX_CHECK_EQ(vx::math::sin(-1.0), -0.84147098);
        VX_CHECK_EQ(vx::math::sin(2.0), 0.90929743);
        VX_CHECK_EQ(vx::math::sin(-2.0), -0.90929743);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(constants<f32>::half_pi, 0.0f);
        const vec2f b = sin(a);
        VX_CHECK_EQ(b, vec2f(1.0f, 0.0f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 0.0f, constants<f32>::quarter_pi);
        const vec3f b = sin(a);
        VX_CHECK_EQ(b, vec3f(0.84147098f, 0.0f, 0.70710678f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(-1.0f, 2.0f, constants<f32>::half_pi, 0.0f);
        const vec4f b = sin(a);
        VX_CHECK_EQ(b, vec4f(-0.84147098f, 0.90929743f, 1.0f, 0.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cos)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::cos(-constants<f32>::pi), -1.0f);
        VX_CHECK_EQ(vx::math::cos(-constants<f32>::three_pi_over_two), -constants<f32>::inv_root_two);
        VX_CHECK_EQ(vx::math::cos(-constants<f32>::half_pi), 0.0f);
        VX_CHECK_EQ(vx::math::cos(-constants<f32>::quarter_pi), constants<f32>::inv_root_two);
        VX_CHECK_EQ(vx::math::cos(0.0f), 1.0f);
        VX_CHECK_EQ(vx::math::cos(constants<f32>::quarter_pi), constants<f32>::inv_root_two);
        VX_CHECK_EQ(vx::math::cos(constants<f32>::half_pi), 0.0f);
        VX_CHECK_EQ(vx::math::cos(constants<f32>::three_pi_over_two), -constants<f32>::inv_root_two);
        VX_CHECK_EQ(vx::math::cos(constants<f32>::pi), -1.0f);

        VX_CHECK_EQ(vx::math::cos(1.0f), 0.5403023f);
        VX_CHECK_EQ(vx::math::cos(-1.0f), 0.5403023f);
        VX_CHECK_EQ(vx::math::cos(3.0f), -0.9899925f);
        VX_CHECK_EQ(vx::math::cos(-2.0f), -0.4161468f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, constants<f32>::half_pi);
        const vec2f r = cos(a);
        VX_CHECK_CLOSE(r.x, 1.0f, 1e-6f);
        VX_CHECK_CLOSE(r.y, 0.0f, 1e-6f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 2.0f, 3.0f);
        const vec3f r = cos(a);
        VX_CHECK_CLOSE(r.x, 0.5403023f, 1e-6f);
        VX_CHECK_CLOSE(r.y, -0.4161468f, 1e-6f);
        VX_CHECK_CLOSE(r.z, -0.9899925f, 1e-6f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, constants<f32>::pi, 1.0f, -1.0f);
        const vec4f r = cos(a);
        VX_CHECK_CLOSE(r.x, 1.0f, 1e-6f);
        VX_CHECK_CLOSE(r.y, -1.0f, 1e-6f);
        VX_CHECK_CLOSE(r.z, 0.5403023f, 1e-6f);
        VX_CHECK_CLOSE(r.w, 0.5403023f, 1e-6f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_tan)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::tan(-constants<f32>::pi), 0.0f);
        VX_CHECK_EQ(vx::math::tan(-constants<f32>::three_pi_over_two), 1.0f);
        VX_CHECK_EQ(vx::math::tan(-constants<f32>::quarter_pi), -1.0f);
        VX_CHECK_EQ(vx::math::tan(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::tan(constants<f32>::quarter_pi), 1.0f);
        VX_CHECK_EQ(vx::math::tan(constants<f32>::three_pi_over_two), -1.0f);
        VX_CHECK_EQ(vx::math::tan(constants<f32>::pi), 0.0f);

        VX_CHECK_EQ(vx::math::tan(1.0f), 1.557408f);
        VX_CHECK_EQ(vx::math::tan(-1.0f), -1.557408f);
        VX_CHECK_EQ(vx::math::tan(0.5f), 0.5463025f);
        VX_CHECK_EQ(vx::math::tan(-0.5f), -0.5463025f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, constants<f32>::quarter_pi);
        const vec2f r = tan(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.0f, 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 0.5f, 1.0f);
        const vec3f r = tan(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.5463025f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 1.557408f, 1e-5f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(-1.0f, -0.5f, 0.5f, 1.0f);
        const vec4f r = tan(a);
        VX_CHECK_CLOSE(r.x, -1.557408f, 1e-5f);
        VX_CHECK_CLOSE(r.y, -0.5463025f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 0.5463025f, 1e-5f);
        VX_CHECK_CLOSE(r.w, 1.557408f, 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_asin)
{
    VX_SECTION("scalar")
    {
        // Regular asin tests
        VX_CHECK_EQ(vx::math::asin(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::asin(-constants<f32>::inv_root_two), -constants<f32>::quarter_pi);
        VX_CHECK_EQ(vx::math::asin(-1.0f), -constants<f32>::half_pi);
        VX_CHECK_EQ(vx::math::asin(-constants<f32>::inv_root_two), -constants<f32>::quarter_pi);
        VX_CHECK_EQ(vx::math::asin(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::asin(constants<f32>::inv_root_two), constants<f32>::quarter_pi);
        VX_CHECK_EQ(vx::math::asin(1.0f), constants<f32>::half_pi);
        VX_CHECK_EQ(vx::math::asin(constants<f32>::inv_root_two), constants<f32>::quarter_pi);
        VX_CHECK_EQ(vx::math::asin(0.0f), 0.0f);


        // Safe asin tests (clamping)
        VX_CHECK_EQ(vx::math::asin_safe(1.1f), constants<f32>::half_pi);
        VX_CHECK_EQ(vx::math::asin_safe(-1.1f), -constants<f32>::half_pi);
        VX_CHECK_EQ(vx::math::asin_safe(0.5f), 0.5235988f);
        VX_CHECK_EQ(vx::math::asin_safe(-0.5f), -0.5235988f);

        // Test for values slightly outside the domain of asin
        VX_CHECK_EQ(vx::math::asin_safe(1.0f), constants<f32>::half_pi);
        VX_CHECK_EQ(vx::math::asin_safe(-1.0f), -constants<f32>::half_pi);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 0.5f);
        const vec2f r = asin(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.5235988f, 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 0.5f, -0.5f);
        const vec3f r = asin(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.5235988f, 1e-5f);
        VX_CHECK_CLOSE(r.z, -0.5235988f, 1e-5f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 0.5f, -0.5f, 1.0f);
        const vec4f r = asin(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.5235988f, 1e-5f);
        VX_CHECK_CLOSE(r.z, -0.5235988f, 1e-5f);
        VX_CHECK_CLOSE(r.w, constants<f32>::half_pi, 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_acos)
{
    VX_SECTION("scalar")
    {
        // Regular acos tests
        VX_CHECK_EQ(vx::math::acos(-1.0f), constants<f32>::pi);
        VX_CHECK_EQ(vx::math::acos(constants<f32>::inv_root_two), constants<f32>::quarter_pi);
        VX_CHECK_EQ(vx::math::acos(0.0f), constants<f32>::half_pi);
        VX_CHECK_EQ(vx::math::acos(-constants<f32>::inv_root_two), constants<f32>::three_pi_over_two);
        VX_CHECK_EQ(vx::math::acos(1.0f), 0.0f);
        VX_CHECK_EQ(vx::math::acos(constants<f32>::inv_root_two), constants<f32>::quarter_pi);
        VX_CHECK_EQ(vx::math::acos(0.0f), constants<f32>::half_pi);
        VX_CHECK_EQ(vx::math::acos(-constants<f32>::inv_root_two), constants<f32>::three_pi_over_two);
        VX_CHECK_EQ(vx::math::acos(-1.0f), constants<f32>::pi);

        // Safe acos tests (clamping)
        VX_CHECK_EQ(vx::math::acos_safe(1.1f), 0.0f);
        VX_CHECK_EQ(vx::math::acos_safe(-1.1f), constants<f32>::pi);
        VX_CHECK_EQ(vx::math::acos_safe(0.5f), 1.0471975f);
        VX_CHECK_EQ(vx::math::acos_safe(-0.5f), 2.094395f);

        // Test for values slightly outside the domain of acos
        VX_CHECK_EQ(vx::math::acos_safe(1.0f), 0.0f);
        VX_CHECK_EQ(vx::math::acos_safe(-1.0f), constants<f32>::pi);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 0.5f);
        const vec2f r = acos(a);
        VX_CHECK_CLOSE(r.x, constants<f32>::half_pi, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.0471975f, 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 0.5f, -0.5f);
        const vec3f r = acos(a);
        VX_CHECK_CLOSE(r.x, constants<f32>::half_pi, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.0471975f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 2.094395f, 1e-5f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 0.5f, -0.5f, 1.0f);
        const vec4f r = acos(a);
        VX_CHECK_CLOSE(r.x, constants<f32>::half_pi, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.0471975f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 2.094395f, 1e-5f);
        VX_CHECK_CLOSE(r.w, 0.0f, 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_atan)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::atan(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::atan(1.0f), -constants<f32>::three_pi_over_two);
        VX_CHECK_EQ(vx::math::atan(-1.0f), -constants<f32>::quarter_pi);
        VX_CHECK_EQ(vx::math::atan(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::atan(1.0f), constants<f32>::quarter_pi);
        VX_CHECK_EQ(vx::math::atan(-1.0f), constants<f32>::three_pi_over_two);
        VX_CHECK_EQ(vx::math::atan(0.0f), 0.0f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 1.0f);
        const vec2f r = atan(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, constants<f32>::quarter_pi, 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 1.0f, -1.0f);
        const vec3f r = atan(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, constants<f32>::quarter_pi, 1e-5f);
        VX_CHECK_CLOSE(r.z, -constants<f32>::quarter_pi, 1e-5f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 1.0f, -1.0f, 0.5f);
        const vec4f r = atan(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, constants<f32>::quarter_pi, 1e-5f);
        VX_CHECK_CLOSE(r.z, -constants<f32>::quarter_pi, 1e-5f);
        VX_CHECK_CLOSE(r.w, 0.4636476f, 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_sinh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::sinh(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::sinh(constants<f32>::quarter_pi), 1.1752012f);
        VX_CHECK_EQ(vx::math::sinh(constants<f32>::half_pi), 2.3012989f);
        VX_CHECK_EQ(vx::math::sinh(constants<f32>::three_pi_over_two), 3.6634393f);
        VX_CHECK_EQ(vx::math::sinh(constants<f32>::two_pi), 3.6268604f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, constants<f32>::quarter_pi);
        const vec2f r = sinh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.1752012f, 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, constants<f32>::quarter_pi, constants<f32>::half_pi);
        const vec3f r = sinh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.1752012f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 2.3012989f, 1e-5f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, constants<f32>::quarter_pi, constants<f32>::half_pi, constants<f32>::three_pi_over_two);
        const vec4f r = sinh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.1752012f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 2.3012989f, 1e-5f);
        VX_CHECK_CLOSE(r.w, 3.6634393f, 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cosh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::cosh(0.0f), 1.0f);
        VX_CHECK_EQ(vx::math::cosh(constants<f32>::quarter_pi), 1.3246095f);
        VX_CHECK_EQ(vx::math::cosh(constants<f32>::half_pi), 2.5091780f);
        VX_CHECK_EQ(vx::math::cosh(constants<f32>::three_pi_over_two), 3.6268604f);
        VX_CHECK_EQ(vx::math::cosh(constants<f32>::two_pi), 3.7621957f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, constants<f32>::quarter_pi);
        const vec2f r = cosh(a);
        VX_CHECK_CLOSE(r.x, 1.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.3246095f, 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, constants<f32>::quarter_pi, constants<f32>::half_pi);
        const vec3f r = cosh(a);
        VX_CHECK_CLOSE(r.x, 1.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.3246095f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 2.5091780f, 1e-5f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, constants<f32>::quarter_pi, constants<f32>::half_pi, constants<f32>::three_pi_over_two);
        const vec4f r = cosh(a);
        VX_CHECK_CLOSE(r.x, 1.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.3246095f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 2.5091780f, 1e-5f);
        VX_CHECK_CLOSE(r.w, 3.6268604f, 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_tanh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::tanh(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::tanh(constants<f32>::quarter_pi), 0.9993293f);
        VX_CHECK_EQ(vx::math::tanh(constants<f32>::half_pi), 0.9171523f);
        VX_CHECK_EQ(vx::math::tanh(constants<f32>::three_pi_over_two), 0.9975370f);
        VX_CHECK_EQ(vx::math::tanh(constants<f32>::two_pi), 0.9999993f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, constants<f32>::quarter_pi);
        const vec2f r = tanh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.9993293f, 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, constants<f32>::quarter_pi, constants<f32>::half_pi);
        const vec3f r = tanh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.9993293f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 0.9171523f, 1e-5f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, constants<f32>::quarter_pi, constants<f32>::half_pi, constants<f32>::three_pi_over_two);
        const vec4f r = tanh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.9993293f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 0.9171523f, 1e-5f);
        VX_CHECK_CLOSE(r.w, 0.9975370f, 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_asinh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::asinh(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::asinh(1.0f), 0.8813736f);
        VX_CHECK_EQ(vx::math::asinh(-1.0f), -0.8813736f);
        VX_CHECK_EQ(vx::math::asinh(2.0f), 1.4436355f);
        VX_CHECK_EQ(vx::math::asinh(10.0f), 2.9932228f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 1.0f);
        const vec2f r = asinh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.8813736f, 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 1.0f, -1.0f);
        const vec3f r = asinh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.8813736f, 1e-5f);
        VX_CHECK_CLOSE(r.z, -0.8813736f, 1e-5f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 1.0f, -1.0f, 2.0f);
        const vec4f r = asinh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.8813736f, 1e-5f);
        VX_CHECK_CLOSE(r.z, -0.8813736f, 1e-5f);
        VX_CHECK_CLOSE(r.w, 1.4436355f, 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_acosh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::acosh(1.0f), 0.0f);
        VX_CHECK_EQ(vx::math::acosh(2.0f), 1.316957f);
        VX_CHECK_EQ(vx::math::acosh(10.0f), 2.9932228f);
        VX_CHECK_EQ(vx::math::acosh(100.0f), 4.6151205f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.0f, 2.0f);
        const vec2f r = acosh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.316957f, 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 2.0f, 10.0f);
        const vec3f r = acosh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.316957f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 2.9932228f, 1e-5f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(1.0f, 2.0f, 10.0f, 100.0f);
        const vec4f r = acosh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 1.316957f, 1e-5f);
        VX_CHECK_CLOSE(r.z, 2.9932228f, 1e-5f);
        VX_CHECK_CLOSE(r.w, 4.6151205f, 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_atanh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::atanh(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::atanh(0.5f), 0.5463025f);
        VX_CHECK_EQ(vx::math::atanh(1.0f), std::numeric_limits<f32>::infinity()); // atanh(1) -> infinity
        VX_CHECK_EQ(vx::math::atanh(-0.5f), -0.5463025f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 0.5f);
        const vec2f r = atanh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.5463025f, 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 0.5f, 1.0f);
        const vec3f r = atanh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.5463025f, 1e-5f);
        VX_CHECK_EQ(r.z, std::numeric_limits<f32>::infinity());
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 0.5f, 1.0f, -0.5f);
        const vec4f r = atanh(a);
        VX_CHECK_CLOSE(r.x, 0.0f, 1e-5f);
        VX_CHECK_CLOSE(r.y, 0.5463025f, 1e-5f);
        VX_CHECK_EQ(r.z, std::numeric_limits<f32>::infinity());
        VX_CHECK_CLOSE(r.w, -0.5463025f, 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}