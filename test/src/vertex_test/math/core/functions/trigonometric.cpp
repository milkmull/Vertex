#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/trigonometric.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b, 1e-4f))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b, 1e-4f))

///////////////////////////////////////////////////////////////////////////////

static constexpr float pi_over_six          = 0.523598775598299f;   // pi / 6
static constexpr float quarter_pi           = 0.785398163397448f;   // pi / 4
static constexpr float pi_over_three        = 1.047197551196598f;   // pi / 3
static constexpr float half_pi              = 1.570796326794896f;   // pi / 2
static constexpr float two_pi_over_three    = 2.094395102393195f;   // 2 * pi / 3
static constexpr float three_quarters_pi    = 2.356194490192345f;   // 3 * pi / 4
static constexpr float five_pi_over_six     = 2.617993877991494f;   // 5 * pi / 6
static constexpr float pi                   = 3.141592653589793f;
static constexpr float seven_pi_over_six    = 3.665191429188092f;   // 7 * pi / 6
static constexpr float five_pi_over_four    = 3.926990816987241f;   // 5 * pi / 4
static constexpr float three_pi_over_two    = 4.712388980384690f;   // 3 * pi / 2
static constexpr float seven_pi_over_four   = 5.497787143782138f;   // 7 * pi / 4
static constexpr float eleven_pi_over_six   = 5.759586531581287f;   // 11 * pi / 6
static constexpr float two_pi               = 6.283185307179586f;   // 2 * pi

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_radians)
{
    VX_STATIC_CHECK_EQ(radians(-180.0f), -pi);
    VX_STATIC_CHECK_EQ(radians(-135.0f), -three_quarters_pi);
    VX_STATIC_CHECK_EQ(radians(-90.0f),  -half_pi);
    VX_STATIC_CHECK_EQ(radians(-45.0f),  -quarter_pi);
    VX_STATIC_CHECK_EQ(radians(0.0f),     0.0f);
    VX_STATIC_CHECK_EQ(radians(45.0f),    quarter_pi);
    VX_STATIC_CHECK_EQ(radians(90.0f),    half_pi);
    VX_STATIC_CHECK_EQ(radians(135.0f),   three_quarters_pi);
    VX_STATIC_CHECK_EQ(radians(180.0f),   pi);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_degrees)
{
    VX_CHECK_EQ(degrees(-pi),                 -180.0f);
    VX_CHECK_EQ(degrees(-three_quarters_pi),  -135.0f);
    VX_CHECK_EQ(degrees(-half_pi),             -90.0f);
    VX_CHECK_EQ(degrees(-quarter_pi),          -45.0f);
    VX_CHECK_EQ(degrees(0.0f),                   0.0f);
    VX_CHECK_EQ(degrees(quarter_pi),            45.0f);
    VX_CHECK_EQ(degrees(half_pi),               90.0f);
    VX_CHECK_EQ(degrees(three_quarters_pi),    135.0f);
    VX_CHECK_EQ(degrees(pi),                   180.0f);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_equivalent_angle)
{
    // Basic rotational equivalence
    VX_CHECK(equivalent_angle(radians(0), radians(0)));
    VX_CHECK(equivalent_angle(radians(0), radians(360)));
    VX_CHECK(equivalent_angle(radians(360), radians(-360)));
    VX_CHECK(equivalent_angle(radians(180), radians(-180)));
    VX_CHECK(equivalent_angle(radians(90), radians(-270)));
    VX_CHECK(equivalent_angle(radians(270), radians(-90)));

    // Symmetry: equivalent_angle(a, b) == equivalent_angle(b, a)
    VX_CHECK(equivalent_angle(radians(-270), radians(90)));
    VX_CHECK(equivalent_angle(radians(90), radians(-270)));

    // Large rotations: multiple full turns
    VX_CHECK(equivalent_angle(radians(1080), radians(0)));       // 3 full turns
    VX_CHECK(equivalent_angle(radians(-1080), radians(0)));
    VX_CHECK(equivalent_angle(radians(1440), radians(360)));     // 4 turns

    // Should not be equivalent (true mismatches)
    VX_CHECK(!equivalent_angle(radians(0), radians(180)));
    VX_CHECK(!equivalent_angle(radians(90), radians(-90)));
    VX_CHECK(!equivalent_angle(radians(180), radians(360)));
    VX_CHECK(!equivalent_angle(radians(180.1f), radians(-180.0f)));
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_atan2)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::atan2(0.0f, 1.0f),         0.0f);
        VX_CHECK_EQ(vx::math::atan2(1.0f, 1.0f),         quarter_pi);
        VX_CHECK_EQ(vx::math::atan2(1.0f, 0.0f),         half_pi);
        VX_CHECK_EQ(vx::math::atan2(1.0f, -1.0f),        three_quarters_pi);
        VX_CHECK_EQ(vx::math::atan2(0.0f, -1.0f),        pi);
        VX_CHECK_EQ(vx::math::atan2(-1.0f, -1.0f),      -three_quarters_pi);
        VX_CHECK_EQ(vx::math::atan2(-1.0f, 0.0f),       -half_pi);
        VX_CHECK_EQ(vx::math::atan2(-1.0f, 1.0f),       -quarter_pi);
    }

    VX_SECTION("vec2")
    {
        const vec2f y(0.0f, 1.0f);
        const vec2f x(1.0f, -1.0f);
        const vec2f r = atan2(y, x);
        VX_CHECK_EQ(r, vec2f(0.0f, three_quarters_pi));
    }

    VX_SECTION("vec3")
    {
        const vec3f y(0.0f, 1.0f, -1.0f);
        const vec3f x(1.0f, -1.0f, 0.0f);
        const vec3f r = atan2(y, x);
        VX_CHECK_EQ(r, vec3f(0.0f, three_quarters_pi, -half_pi));
    }

    VX_SECTION("vec4")
    {
        const vec4f y(0.0f, 1.0f, -1.0f, -1.0f);
        const vec4f x(1.0f, -1.0f, 0.0f, 1.0f);
        const vec4f r = atan2(y, x);
        VX_CHECK_EQ(r, vec4f(0.0f, three_quarters_pi, -half_pi, -quarter_pi));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_sin)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::sin(-pi),                0.0f);
        VX_CHECK_EQ(vx::math::sin(-three_quarters_pi), -0.70710678f);
        VX_CHECK_EQ(vx::math::sin(-half_pi),           -1.0f);
        VX_CHECK_EQ(vx::math::sin(-quarter_pi),        -0.70710678f);
        VX_CHECK_EQ(vx::math::sin(0.0f),                0.0f);
        VX_CHECK_EQ(vx::math::sin(quarter_pi),          0.70710678f);
        VX_CHECK_EQ(vx::math::sin(pi_over_three),       0.86602540f);
        VX_CHECK_EQ(vx::math::sin(pi_over_six),         0.5f);
        VX_CHECK_EQ(vx::math::sin(half_pi),             1.0f);
        VX_CHECK_EQ(vx::math::sin(two_pi_over_three),   0.86602540f);
        VX_CHECK_EQ(vx::math::sin(three_quarters_pi),   0.70710678f);
        VX_CHECK_EQ(vx::math::sin(five_pi_over_six),    0.5f);
        VX_CHECK_EQ(vx::math::sin(pi),                  0.0f);
        VX_CHECK_EQ(vx::math::sin(seven_pi_over_six),  -0.5f);
        VX_CHECK_EQ(vx::math::sin(five_pi_over_four),  -0.70710678f);
        VX_CHECK_EQ(vx::math::sin(three_pi_over_two), -1.0f);
        VX_CHECK_EQ(vx::math::sin(seven_pi_over_four), -0.70710678f);
        VX_CHECK_EQ(vx::math::sin(eleven_pi_over_six), -0.5f);
        VX_CHECK_EQ(vx::math::sin(two_pi),              0.0f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, quarter_pi);
        const vec2f b = sin(a);
        VX_CHECK_EQ(b, vec2f(0.0f, 0.70710678f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, quarter_pi, half_pi);
        const vec3f b = sin(a);
        VX_CHECK_EQ(b, vec3f(0.0f, 0.70710678f, 1.0f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, quarter_pi, half_pi, pi);
        const vec4f b = sin(a);
        VX_CHECK_EQ(b, vec4f(0.0f, 0.70710678f, 1.0f, 0.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cos)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::cos(-pi),               -1.0f);
        VX_CHECK_EQ(vx::math::cos(-three_quarters_pi),-0.70710678f);
        VX_CHECK_EQ(vx::math::cos(-half_pi),           0.0f);
        VX_CHECK_EQ(vx::math::cos(-quarter_pi),        0.70710678f);
        VX_CHECK_EQ(vx::math::cos(0.0f),               1.0f);
        VX_CHECK_EQ(vx::math::cos(quarter_pi),         0.70710678f);
        VX_CHECK_EQ(vx::math::cos(pi_over_three),      0.5f);
        VX_CHECK_EQ(vx::math::cos(pi_over_six),        0.86602540f);
        VX_CHECK_EQ(vx::math::cos(half_pi),            0.0f);
        VX_CHECK_EQ(vx::math::cos(two_pi_over_three), -0.5f);
        VX_CHECK_EQ(vx::math::cos(three_quarters_pi), -0.70710678f);
        VX_CHECK_EQ(vx::math::cos(five_pi_over_six),  -0.86602540f);
        VX_CHECK_EQ(vx::math::cos(pi),                -1.0f);
        VX_CHECK_EQ(vx::math::cos(seven_pi_over_six), -0.86602540f);
        VX_CHECK_EQ(vx::math::cos(five_pi_over_four), -0.70710678f);
        VX_CHECK_EQ(vx::math::cos(three_pi_over_two),  0.0f);
        VX_CHECK_EQ(vx::math::cos(seven_pi_over_four), 0.70710678f);
        VX_CHECK_EQ(vx::math::cos(eleven_pi_over_six), 0.86602540f);
        VX_CHECK_EQ(vx::math::cos(two_pi),             1.0f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, quarter_pi);
        const vec2f b = cos(a);
        VX_CHECK_EQ(b, vec2f(1.0f, 0.70710678f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, quarter_pi, half_pi);
        const vec3f b = cos(a);
        VX_CHECK_EQ(b, vec3f(1.0f, 0.70710678f, 0.0f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, quarter_pi, half_pi, pi);
        const vec4f b = cos(a);
        VX_CHECK_EQ(b, vec4f(1.0f, 0.70710678f, 0.0f, -1.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_tan)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::tan(-pi),                 0.0f);
        VX_CHECK_EQ(vx::math::tan(-three_quarters_pi),  1.0f);
        // tan(-half_pi) is undefined — skip
        VX_CHECK_EQ(vx::math::tan(-quarter_pi),        -1.0f);
        VX_CHECK_EQ(vx::math::tan(0.0f),                0.0f);
        VX_CHECK_EQ(vx::math::tan(quarter_pi),          1.0f);
        VX_CHECK_EQ(vx::math::tan(pi_over_three),       1.7320508f);
        VX_CHECK_EQ(vx::math::tan(pi_over_six),         0.57735027f);
        // tan(half_pi) is undefined — skip
        VX_CHECK_EQ(vx::math::tan(two_pi_over_three),  -1.7320508f);
        VX_CHECK_EQ(vx::math::tan(three_quarters_pi),  -1.0f);
        VX_CHECK_EQ(vx::math::tan(five_pi_over_six),   -0.57735027f);
        VX_CHECK_EQ(vx::math::tan(pi),                  0.0f);
        VX_CHECK_EQ(vx::math::tan(seven_pi_over_six),   0.57735027f);
        VX_CHECK_EQ(vx::math::tan(five_pi_over_four),   1.0f);
        // tan(three_pi_over_two) is undefined — skip
        VX_CHECK_EQ(vx::math::tan(seven_pi_over_four), -1.0f);
        VX_CHECK_EQ(vx::math::tan(eleven_pi_over_six), -0.57735027f);
        VX_CHECK_EQ(vx::math::tan(two_pi),              0.0f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, pi_over_three);
        const vec2f b = tan(a);
        VX_CHECK_EQ(b, vec2f(0.0f, 1.7320508f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, pi_over_three, two_pi_over_three);
        const vec3f b = tan(a);
        VX_CHECK_EQ(b, vec3f(0.0f, 1.7320508f, -1.7320508f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, pi_over_three, two_pi_over_three, pi);
        const vec4f b = tan(a);
        VX_CHECK_EQ(b, vec4f(0.0f, 1.7320508f, -1.7320508f, 0.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_asin)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::asin(1.0f), half_pi);
        VX_CHECK_EQ(vx::math::asin(0.86602540f), pi_over_three);
        VX_CHECK_EQ(vx::math::asin(0.70710678f), quarter_pi);
        VX_CHECK_EQ(vx::math::asin(0.5f), pi_over_six);
        VX_CHECK_EQ(vx::math::asin(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::asin(-0.5f), -pi_over_six);
        VX_CHECK_EQ(vx::math::asin(-0.70710678f), -quarter_pi);
        VX_CHECK_EQ(vx::math::asin(-0.86602540f), -pi_over_three);
        VX_CHECK_EQ(vx::math::asin(-1.0f), -half_pi);
    }

    VX_SECTION("asin_clamped")
    {
        VX_CHECK_EQ(vx::math::asin_clamped(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::asin_clamped(1.0f), half_pi);
        VX_CHECK_EQ(vx::math::asin_clamped(-1.0f), -half_pi);

        // Out-of-range inputs — should clamp to asin(1.0) = pi/2
        VX_CHECK_EQ(vx::math::asin_clamped(1.0001f), half_pi);
        VX_CHECK_EQ(vx::math::asin_clamped(2.0f), half_pi);
        VX_CHECK_EQ(vx::math::asin_clamped(100.0f), half_pi);

        // Out-of-range inputs — should clamp to asin(-1.0) = -pi/2
        VX_CHECK_EQ(vx::math::asin_clamped(-1.0001f), -half_pi);
        VX_CHECK_EQ(vx::math::asin_clamped(-2.0f), -half_pi);
        VX_CHECK_EQ(vx::math::asin_clamped(-100.0f), -half_pi);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 0.70710678f);
        const vec2f b = asin(a);
        VX_CHECK_EQ(b, vec2f(0.0f, quarter_pi));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 0.70710678f, 1.0f);
        const vec3f b = asin(a);
        VX_CHECK_EQ(b, vec3f(0.0f, quarter_pi, half_pi));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 0.70710678f, 1.0f, -1.0f);
        const vec4f b = asin(a);
        VX_CHECK_EQ(b, vec4f(0.0f, quarter_pi, half_pi, -half_pi));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_acos)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::acos(1.0f), 0.0f);
        VX_CHECK_EQ(vx::math::acos(0.86602540f), pi_over_six);
        VX_CHECK_EQ(vx::math::acos(0.70710678f), quarter_pi);
        VX_CHECK_EQ(vx::math::acos(0.5f), pi_over_three);
        VX_CHECK_EQ(vx::math::acos(0.0f), half_pi);
        VX_CHECK_EQ(vx::math::acos(-0.5f), two_pi_over_three);
        VX_CHECK_EQ(vx::math::acos(-0.70710678f), three_quarters_pi);
        VX_CHECK_EQ(vx::math::acos(-0.86602540f), five_pi_over_six);
        VX_CHECK_EQ(vx::math::acos(-1.0f), pi);
    }

    VX_SECTION("acos_clamped")
    {
        VX_CHECK_EQ(vx::math::acos_clamped(1.0f), 0.0f);
        VX_CHECK_EQ(vx::math::acos_clamped(0.0f), half_pi);
        VX_CHECK_EQ(vx::math::acos_clamped(-1.0f), pi);

        // Out-of-range inputs — should clamp to acos(1.0) = 0.0
        VX_CHECK_EQ(vx::math::acos_clamped(1.0001f), 0.0f);
        VX_CHECK_EQ(vx::math::acos_clamped(2.0f), 0.0f);
        VX_CHECK_EQ(vx::math::acos_clamped(100.0f), 0.0f);

        // Out-of-range inputs — should clamp to acos(-1.0) = pi
        VX_CHECK_EQ(vx::math::acos_clamped(-1.0001f), pi);
        VX_CHECK_EQ(vx::math::acos_clamped(-2.0f), pi);
        VX_CHECK_EQ(vx::math::acos_clamped(-100.0f), pi);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.0f, 0.70710678f);
        const vec2f b = acos(a);
        VX_CHECK_EQ(b, vec2f(0.0f, quarter_pi));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 0.70710678f, 0.0f);
        const vec3f b = acos(a);
        VX_CHECK_EQ(b, vec3f(0.0f, quarter_pi, half_pi));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(1.0f, 0.70710678f, 0.0f, -1.0f);
        const vec4f b = acos(a);
        VX_CHECK_EQ(b, vec4f(0.0f, quarter_pi, half_pi, pi));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_atan)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::atan(0.0f), 0.0f);
        VX_CHECK_EQ(vx::math::atan(0.57735027f), pi_over_six);
        VX_CHECK_EQ(vx::math::atan(1.0f), quarter_pi);
        VX_CHECK_EQ(vx::math::atan(1.7320508f), pi_over_three);
        VX_CHECK_EQ(vx::math::atan(-0.57735027f), -pi_over_six);
        VX_CHECK_EQ(vx::math::atan(-1.0f), -quarter_pi);
        VX_CHECK_EQ(vx::math::atan(-1.7320508f), -pi_over_three);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 1.7320508f);
        const vec2f b = atan(a);
        VX_CHECK_EQ(b, vec2f(0.0f, pi_over_three));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 1.7320508f, -1.7320508f);
        const vec3f b = atan(a);
        VX_CHECK_EQ(b, vec3f(0.0f, pi_over_three, -pi_over_three));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 1.7320508f, -1.7320508f, 0.0f);
        const vec4f b = atan(a);
        VX_CHECK_EQ(b, vec4f(0.0f, pi_over_three, -pi_over_three, 0.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_sinh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::sinh(-pi),               -11.54873936f);
        VX_CHECK_EQ(vx::math::sinh(-three_quarters_pi), -5.22797192f);
        VX_CHECK_EQ(vx::math::sinh(-half_pi),           -2.30129890f);
        VX_CHECK_EQ(vx::math::sinh(-quarter_pi),        -0.86867096f);
        VX_CHECK_EQ(vx::math::sinh(0.0f),                0.0f);
        VX_CHECK_EQ(vx::math::sinh(quarter_pi),          0.86867096f);
        VX_CHECK_EQ(vx::math::sinh(pi_over_three),       1.24936705f);
        VX_CHECK_EQ(vx::math::sinh(pi_over_six),         0.54785347f);
        VX_CHECK_EQ(vx::math::sinh(half_pi),             2.30129890f);
        VX_CHECK_EQ(vx::math::sinh(two_pi_over_three),   3.99869134f);
        VX_CHECK_EQ(vx::math::sinh(three_quarters_pi),   5.22797192f);
        VX_CHECK_EQ(vx::math::sinh(five_pi_over_six),    6.81762330f);
        VX_CHECK_EQ(vx::math::sinh(pi),                  11.54873936f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, quarter_pi);
        const vec2f b = sinh(a);
        VX_CHECK_EQ(b, vec2f(0.0f, 0.86867096f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, quarter_pi, half_pi);
        const vec3f b = sinh(a);
        VX_CHECK_EQ(b, vec3f(0.0f, 0.86867096f, 2.30129890f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, quarter_pi, half_pi, pi);
        const vec4f b = sinh(a);
        VX_CHECK_EQ(b, vec4f(0.0f, 0.86867096f, 2.30129890f, 11.54873936f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cosh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::cosh(-pi),                11.59195328f);
        VX_CHECK_EQ(vx::math::cosh(-three_quarters_pi), 5.32275214f);
        VX_CHECK_EQ(vx::math::cosh(-half_pi),           2.50917847f);
        VX_CHECK_EQ(vx::math::cosh(-quarter_pi),        1.32460908f);
        VX_CHECK_EQ(vx::math::cosh(0.0f),               1.0f);
        VX_CHECK_EQ(vx::math::cosh(quarter_pi),         1.32460908f);
        VX_CHECK_EQ(vx::math::cosh(pi_over_three),      1.60028685f);
        VX_CHECK_EQ(vx::math::cosh(pi_over_six),        1.14023832f);
        VX_CHECK_EQ(vx::math::cosh(half_pi),            2.50917847f);
        VX_CHECK_EQ(vx::math::cosh(two_pi_over_three),  4.12183605f);
        VX_CHECK_EQ(vx::math::cosh(three_quarters_pi),  5.32275214f);
        VX_CHECK_EQ(vx::math::cosh(five_pi_over_six),   6.89057236f);
        VX_CHECK_EQ(vx::math::cosh(pi),                 11.59195328f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, quarter_pi);
        const vec2f b = cosh(a);
        VX_CHECK_EQ(b, vec2f(1.0f, 1.32460909f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, quarter_pi, half_pi);
        const vec3f b = cosh(a);
        VX_CHECK_EQ(b, vec3f(1.0f, 1.32460909f, 2.50917848f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, quarter_pi, half_pi, pi);
        const vec4f b = cosh(a);
        VX_CHECK_EQ(b, vec4f(1.0f, 1.32460909f, 2.50917848f, 11.59195328f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_tanh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::tanh(-pi),                -0.9962720f);
        VX_CHECK_EQ(vx::math::tanh(-three_quarters_pi), -0.9821933f);
        VX_CHECK_EQ(vx::math::tanh(-half_pi),           -0.9171523f);
        VX_CHECK_EQ(vx::math::tanh(-quarter_pi),        -0.6557944f);
        VX_CHECK_EQ(vx::math::tanh(0.0f),                0.0f);
        VX_CHECK_EQ(vx::math::tanh(quarter_pi),          0.6557944f);
        VX_CHECK_EQ(vx::math::tanh(pi_over_three),       0.780714f);
        VX_CHECK_EQ(vx::math::tanh(pi_over_six),         0.480472f);
        VX_CHECK_EQ(vx::math::tanh(half_pi),             0.9171523f);
        VX_CHECK_EQ(vx::math::tanh(two_pi_over_three),   0.9701238f);
        VX_CHECK_EQ(vx::math::tanh(three_quarters_pi),   0.9821933f);
        VX_CHECK_EQ(vx::math::tanh(five_pi_over_six),    0.9894132f);
        VX_CHECK_EQ(vx::math::tanh(pi),                  0.9962720f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, pi_over_three);
        const vec2f b = tanh(a);
        VX_CHECK_EQ(b, vec2f(0.0f, 0.780714f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, pi_over_three, two_pi_over_three);
        const vec3f b = tanh(a);
        VX_CHECK_EQ(b, vec3f(0.0f, 0.780714f, 0.9701238f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, pi_over_three, two_pi_over_three, pi);
        const vec4f b = tanh(a);
        VX_CHECK_EQ(b, vec4f(0.0f, 0.780714f, 0.9701238f, 0.9962720f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_asinh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::asinh(-11.54873936f),    -pi);
        VX_CHECK_EQ(vx::math::asinh(-5.22797192f),     -three_quarters_pi);
        VX_CHECK_EQ(vx::math::asinh(-2.30129890f),     -half_pi);
        VX_CHECK_EQ(vx::math::asinh(-0.86867096f),     -quarter_pi);
        VX_CHECK_EQ(vx::math::asinh(0.0f),              0.0f);
        VX_CHECK_EQ(vx::math::asinh(0.86867096f),       quarter_pi);
        VX_CHECK_EQ(vx::math::asinh(1.24936705f),       pi_over_three);
        VX_CHECK_EQ(vx::math::asinh(0.54785347f),       pi_over_six);
        VX_CHECK_EQ(vx::math::asinh(2.30129890f),       half_pi);
        VX_CHECK_EQ(vx::math::asinh(3.99869134f),       two_pi_over_three);
        VX_CHECK_EQ(vx::math::asinh(5.22797192f),       three_quarters_pi);
        VX_CHECK_EQ(vx::math::asinh(6.81762330f),       five_pi_over_six);
        VX_CHECK_EQ(vx::math::asinh(11.54873936f),      pi);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 0.86867096f);
        const vec2f b = asinh(a);
        VX_CHECK_EQ(b, vec2f(0.0f, quarter_pi));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 0.86867096f, 2.30129890f);
        const vec3f b = asinh(a);
        VX_CHECK_EQ(b, vec3f(0.0f, quarter_pi, half_pi));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 0.86867096f, 2.30129890f, 11.54873936f);
        const vec4f b = asinh(a);
        VX_CHECK_EQ(b, vec4f(0.0f, quarter_pi, half_pi, pi));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_acosh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::acosh(11.59195328f),      pi);
        VX_CHECK_EQ(vx::math::acosh(5.32275214f),       three_quarters_pi);
        VX_CHECK_EQ(vx::math::acosh(2.50917847f),       half_pi);
        VX_CHECK_EQ(vx::math::acosh(1.32460908f),       quarter_pi);
        VX_CHECK_EQ(vx::math::acosh(1.0f),              0.0f);
        VX_CHECK_EQ(vx::math::acosh(1.32460908f),       quarter_pi);
        VX_CHECK_EQ(vx::math::acosh(1.60028685f),       pi_over_three);
        VX_CHECK_EQ(vx::math::acosh(1.14023832f),       pi_over_six);
        VX_CHECK_EQ(vx::math::acosh(2.50917847f),       half_pi);
        VX_CHECK_EQ(vx::math::acosh(4.12183605f),       two_pi_over_three);
        VX_CHECK_EQ(vx::math::acosh(5.32275214f),       three_quarters_pi);
        VX_CHECK_EQ(vx::math::acosh(6.89057236f),       five_pi_over_six);
        VX_CHECK_EQ(vx::math::acosh(11.59195328f),      pi);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.0f, 1.32460909f);
        const vec2f b = acosh(a);
        VX_CHECK_EQ(b, vec2f(0.0f, quarter_pi));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 1.32460909f, 2.50917848f);
        const vec3f b = acosh(a);
        VX_CHECK_EQ(b, vec3f(0.0f, quarter_pi, half_pi));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(1.0f, 1.32460909f, 2.50917848f, 11.59195328f);
        const vec4f b = acosh(a);
        VX_CHECK_EQ(b, vec4f(0.0f, quarter_pi, half_pi, pi));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_atanh)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::atanh(-0.9962720f),       -pi);
        VX_CHECK_EQ(vx::math::atanh(-0.9821933f),       -three_quarters_pi);
        VX_CHECK_EQ(vx::math::atanh(-0.9171523f),       -half_pi);
        VX_CHECK_EQ(vx::math::atanh(-0.6557944f),       -quarter_pi);
        VX_CHECK_EQ(vx::math::atanh(0.0f),               0.0f);
        VX_CHECK_EQ(vx::math::atanh(0.6557944f),         quarter_pi);
        VX_CHECK_EQ(vx::math::atanh(0.780714f),          pi_over_three);
        VX_CHECK_EQ(vx::math::atanh(0.480472f),          pi_over_six);
        VX_CHECK_EQ(vx::math::atanh(0.9171523f),         half_pi);
        VX_CHECK_EQ(vx::math::atanh(0.9701238f),         two_pi_over_three);
        VX_CHECK_EQ(vx::math::atanh(0.9821933f),         three_quarters_pi);
        VX_CHECK_EQ(vx::math::atanh(0.9894132f),         five_pi_over_six);
        VX_CHECK_EQ(vx::math::atanh(0.9962720f),         pi);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 0.780714f);
        const vec2f b = atanh(a);
        VX_CHECK_EQ(b, vec2f(0.0f, pi_over_three));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 0.780714f, 0.9701238f);
        const vec3f b = atanh(a);
        VX_CHECK_EQ(b, vec3f(0.0f, pi_over_three, two_pi_over_three));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 0.780714f, 0.9701238f, 0.9962720f);
        const vec4f b = atanh(a);
        VX_CHECK_EQ(b, vec4f(0.0f, pi_over_three, two_pi_over_three, pi));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}