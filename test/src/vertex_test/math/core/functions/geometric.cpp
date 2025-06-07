#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/geometric.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

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

VX_TEST_CASE(test_dot)
{
    VX_SECTION("vec2")
    {
        constexpr vec2 a(1.0f, -2.0f);
        constexpr vec2 b(-3.0f, 4.0f);
        VX_STATIC_CHECK_EQ(dot(a, b), -11.0f);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3 a(1.0f, -2.0f, 3.0f);
        constexpr vec3 b(-4.0f, 5.0f, -6.0f);
        VX_STATIC_CHECK_EQ(dot(a, b), -32.0f);
    }

    VX_SECTION("vec4")
    {
        constexpr vec4 a(1.0f, -2.0f, 3.0f, -4.0f);
        constexpr vec4 b(-5.0f, 6.0f, -7.0f, 8.0f);
        VX_STATIC_CHECK_EQ(dot(a, b), -70.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_orthogonal)
{
    constexpr vec2 a{ 1.0f, 0.0f };
    constexpr vec2 ortho_a = orthogonal(a);
    VX_STATIC_CHECK_EQ(dot(a, ortho_a), 0.0f);

    constexpr vec2 b{ 3.0f, 4.0f };
    constexpr vec2 ortho_b = orthogonal(b);
    VX_STATIC_CHECK_EQ(dot(b, ortho_b), 0.0f);

    constexpr vec2 c{ -2.0f, 5.0f };
    constexpr vec2 ortho_c = orthogonal(c);
    VX_STATIC_CHECK_EQ(dot(c, ortho_c), 0.0f);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cross)
{
    VX_SECTION("vec2")
    {
        constexpr vec2 a{ 1.0f, 0.0f };
        constexpr vec2 b{ 0.0f, 1.0f };
        VX_STATIC_CHECK_EQ(cross(a, b), 1.0f);

        constexpr vec2 c{ 3.0f, 4.0f };
        constexpr vec2 d{ 4.0f, 3.0f };
        VX_STATIC_CHECK_EQ(cross(c, d), -7.0f);

        constexpr vec2 e{ 0.0f, 0.0f };
        constexpr vec2 f{ 5.0f, 5.0f };
        VX_STATIC_CHECK_EQ(cross(e, f), 0.0f);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3 a{ 1.0f, 0.0f, 0.0f };
        constexpr vec3 b{ 0.0f, 1.0f, 0.0f };
        VX_STATIC_CHECK_EQ(cross(a, b), vec3(0.0f, 0.0f, 1.0f));
        // Opposite order should negate the result
        VX_STATIC_CHECK_EQ(cross(b, a), vec3(0.0f, 0.0f, -1.0f));

        // Parallel vectors should result in zero vector
        constexpr vec3 c{ 2.0f, 2.0f, 2.0f };
        constexpr vec3 d{ 4.0f, 4.0f, 4.0f };
        VX_STATIC_CHECK_EQ(cross(c, d), vec3(0.0f, 0.0f, 0.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_length)
{
    VX_SECTION("vec2")
    {
        const vec2 a{ 3.0f, -4.0f };
        VX_CHECK_EQ(length_squared(a), 25.0f);
        VX_CHECK_EQ(length(a), 5.0f);

        const vec2 zero{ 0.0f, 0.0f };
        VX_CHECK_EQ(length_squared(zero), 0.0f);
        VX_CHECK_EQ(length(zero), 0.0f);
    }

    VX_SECTION("vec3")
    {
        const vec3 a{ 1.0f, -2.0f, 3.0f };
        VX_CHECK_EQ(length_squared(a), 14.0f);
        VX_CHECK_EQ(length(a), 3.74165738677f);

        const vec3 zero{ 0.0f, 0.0f, 0.0f };
        VX_CHECK_EQ(length_squared(zero), 0.0f);
        VX_CHECK_EQ(length(zero), 0.0f);
    }

    VX_SECTION("vec4")
    {
        const vec4 a{ 1.0f, -2.0f, 3.0f, -4.0f };
        VX_CHECK_EQ(length_squared(a), 30.0f);
        VX_CHECK_EQ(length(a), 5.47722557505f);

        const vec4 zero{ 0.0f, 0.0f, 0.0f, 0.0f };
        VX_CHECK_EQ(length_squared(zero), 0.0f);
        VX_CHECK_EQ(length(zero), 0.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_distanced)
{
    VX_SECTION("vec2")
    {
        const vec2 p0{ 0.0f, 0.0f };
        const vec2 p1{ 3.0f, 4.0f };
        VX_CHECK_EQ(distance_squared(p0, p1), 25.0f);
        VX_CHECK_EQ(distance(p0, p1), 5.0f);

        const vec2 p2{ 3.0f, 4.0f };
        VX_CHECK_EQ(distance_squared(p2, p2), 0.0f);
        VX_CHECK_EQ(distance(p2, p2), 0.0f);

        const vec2 p3{ -1.0f, -1.0f };
        const vec2 p4{ 1.0f, 1.0f };
        VX_CHECK_EQ(distance_squared(p3, p4), 8.0f);
        VX_CHECK_EQ(distance(p3, p4), 2.82842712475f);
    }

    VX_SECTION("vec3")
    {
        const vec3 p0{ 0.0f, 0.0f, 0.0f };
        const vec3 p1{ 1.0f, 2.0f, 2.0f };
        VX_CHECK_EQ(distance_squared(p0, p1), 9.0f);
        VX_CHECK_EQ(distance(p0, p1), 3.0f);

        const vec3 p2{ 1.0f, 1.0f, 1.0f };
        VX_CHECK_EQ(distance_squared(p2, p2), 0.0f);
        VX_CHECK_EQ(distance(p2, p2), 0.0f);

        const vec3 p3{ -1.0f, -1.0f, -1.0f };
        const vec3 p4{ 1.0f, 1.0f, 1.0f };
        VX_CHECK_EQ(distance_squared(p3, p4), 12.0f);
        VX_CHECK_EQ(distance(p3, p4), 3.46410161514f);
    }

    VX_SECTION("vec4")
    {
        const vec4 p0{ 1.0f, 2.0f, 3.0f, 4.0f };
        const vec4 p1{ 5.0f, 6.0f, 7.0f, 8.0f };
        VX_CHECK_EQ(distance_squared(p0, p1), 64.0f);
        VX_CHECK_EQ(distance(p0, p1), 8.0f);

        const vec4 p2{ 0.0f, 0.0f, 0.0f, 0.0f };
        VX_CHECK_EQ(distance_squared(p2, p2), 0.0f);
        VX_CHECK_EQ(distance(p2, p2), 0.0f);

        const vec4 p3{ -1.0f, -1.0f, -1.0f, -1.0f };
        const vec4 p4{ 1.0f, 1.0f, 1.0f, 1.0f };
        VX_CHECK_EQ(distance_squared(p3, p4), 16.0f);
        VX_CHECK_EQ(distance(p3, p4), 4.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_normalize)
{
    VX_SECTION("vec2")
    {
        const vec2 v{ 3.0f, 4.0f };
        const vec2 n = normalize(v);
        VX_CHECK_EQ(length(n), 1.0f);
        VX_CHECK(is_normalized(n));

        const vec2 nf = normalize_fast(v);
        VX_CHECK_EQ(length(nf), 1.0f);
        VX_CHECK(is_normalized(nf));

        const vec2 zero{ 0.0f, 0.0f };
        const vec2 n_zero = normalize(zero);
        VX_CHECK(is_zero_approx(n_zero));
        VX_CHECK(!is_normalized(n_zero));
    }

    VX_SECTION("vec3")
    {
        const vec3 v{ 1.0f, 2.0f, 2.0f };
        const vec3 n = normalize(v);
        VX_CHECK_EQ(length(n), 1.0f);
        VX_CHECK(is_normalized(n));

        const vec3 nf = normalize_fast(v);
        VX_CHECK_EQ(length(nf), 1.0f);
        VX_CHECK(is_normalized(nf));

        const vec3 zero{ 0.0f, 0.0f, 0.0f };
        const vec3 n_zero = normalize(zero);
        VX_CHECK(is_zero_approx(n_zero));
        VX_CHECK(!is_normalized(n_zero));
    }

    VX_SECTION("vec4")
    {
        const vec4 v{ 1.0f, 2.0f, 2.0f, 3.0f };
        const vec4 n = normalize(v);
        VX_CHECK_EQ(length(n), 1.0f);
        VX_CHECK(is_normalized(n));

        const vec4 nf = normalize_fast(v);
        VX_CHECK_EQ(length(nf), 1.0f);
        VX_CHECK(is_normalized(nf));

        const vec4 zero{ 0.0f, 0.0f, 0.0f, 0.0f };
        const vec4 n_zero = normalize(zero);
        VX_CHECK(is_zero_approx(n_zero));
        VX_CHECK(!is_normalized(n_zero));
    }
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_normalized_dot)
{
    VX_SECTION("vec2")
    {
        // Same direction: dot = 1
        const vec2 a{ 1.0f, 0.0f };
        const vec2 b{ 2.0f, 0.0f };
        VX_CHECK_EQ(normalized_dot(a, b), 1.0f);

        // Opposite direction: dot = -1
        const vec2 c{ 1.0f, 0.0f };
        const vec2 d{ -1.0f, 0.0f };
        VX_CHECK_EQ(normalized_dot(c, d), -1.0f);

        // Orthogonal: dot = 0
        const vec2 e{ 0.0f, 1.0f };
        VX_CHECK_EQ(normalized_dot(a, e), 0.0f);

        // Diagonal 45: dot = cos(45) = sqrt(2) / 2
        const vec2 diag{ 1.0f, 1.0f };
        VX_CHECK_EQ(normalized_dot(a, diag), 0.70710678118f);
    }

    VX_SECTION("vec3")
    {
        const vec3 x{ 1.0f, 0.0f, 0.0f };
        const vec3 y{ 0.0f, 1.0f, 0.0f };
        const vec3 z{ -1.0f, 0.0f, 0.0f };

        VX_CHECK_EQ(normalized_dot(x, x), 1.0f);
        VX_CHECK_EQ(normalized_dot(x, z), -1.0f);
        VX_CHECK_EQ(normalized_dot(x, y), 0.0f);
    }

    VX_SECTION("vec4")
    {
        const vec4 a{ 1.0f, 1.0f, 1.0f, 1.0f };
        const vec4 b{ -1.0f, -1.0f, -1.0f, -1.0f };
        const vec4 c{ 1.0f, -1.0f, 1.0f, -1.0f };

        VX_CHECK_EQ(normalized_dot(a, b), -1.0f);
        VX_CHECK_EQ(normalized_dot(a, a), 1.0f);
        VX_CHECK_EQ(normalized_dot(a, c), 0.0f);
    }

    VX_SECTION("zero vectors")
    {
        const vec3 zero{ 0.0f, 0.0f, 0.0f };
        const vec3 v{ 1.0f, 2.0f, 3.0f };

        const float r1 = normalized_dot(zero, v);
        const float r2 = normalized_dot(v, zero);
        const float r3 = normalized_dot(zero, zero);

        VX_CHECK_EQ(r1, 0.0f);
        VX_CHECK_EQ(r2, 0.0f);
        VX_CHECK_EQ(r3, 0.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_set_length)
{
    VX_SECTION("vec2")
    {
        const vec2 v(3.0f, 4.0f);
        constexpr float len = 5.0f;

        // grow
        const vec2 a = set_length(v, len * 2);
        VX_CHECK_EQ(length(a), len * 2);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // same
        const vec2 b = set_length(v, len);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // shrink
        const vec2 c = set_length(v, 1.0f);
        VX_CHECK_EQ(length(c), 1.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);

        // negate
        const vec2 d = set_length(v, -len);
        VX_CHECK_EQ(length(d), len);
        VX_CHECK_EQ(normalized_dot(v, d), -1.0f);
    }

    VX_SECTION("vec3")
    {
        const vec3 v(2.0f, 3.0f, 6.0f);
        constexpr float len = 7.0f;

        // grow
        const vec3 a = set_length(v, len * 2);
        VX_CHECK_EQ(length(a), len * 2);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // same
        const vec3 b = set_length(v, len);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // shrink
        const vec3 c = set_length(v, 1.0f);
        VX_CHECK_EQ(length(c), 1.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);

        // negate
        const vec3 d = set_length(v, -len);
        VX_CHECK_EQ(length(d), len);
        VX_CHECK_EQ(normalized_dot(v, d), -1.0f);
    }

    VX_SECTION("vec4")
    {
        const vec4 v(1.0f, 2.0f, 2.0f, 4.0f);
        constexpr float len = 5.0f;

        // grow
        const vec4 a = set_length(v, len * 2);
        VX_CHECK_EQ(length(a), len * 2);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // same
        const vec4 b = set_length(v, len);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // shrink
        const vec4 c = set_length(v, 1.0f);
        VX_CHECK_EQ(length(c), 1.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);

        // negate
        const vec4 d = set_length(v, -len);
        VX_CHECK_EQ(length(d), len);
        VX_CHECK_EQ(normalized_dot(v, d), -1.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_clamp_lenth)
{
    VX_SECTION("vec2")
    {
        const vec2 v(3.0f, 4.0f);
        constexpr float len = 5.0f;

        // below
        const vec2 a = clamp_length(v, 7.0f, 10.0f);
        VX_CHECK_EQ(length(a), 7.0f);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // in
        const vec2 b = clamp_length(v, 1.0f, 7.0f);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // above
        const vec2 c = clamp_length(v, 1.0f, 4.0f);
        VX_CHECK_EQ(length(c), 4.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);
    }

    VX_SECTION("vec3")
    {
        const vec3 v(2.0f, 3.0f, 6.0f);
        constexpr float len = 7.0f;

        // below
        const vec3 a = clamp_length(v, 10.0f, 15.0f);
        VX_CHECK_EQ(length(a), 10.0f);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // in
        const vec3 b = clamp_length(v, 1.0f, 10.0f);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // above
        const vec3 c = clamp_length(v, 1.0f, 4.0f);
        VX_CHECK_EQ(length(c), 4.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);
    }

    VX_SECTION("vec4")
    {
        const vec4 v(1.0f, 2.0f, 2.0f, 4.0f);
        constexpr float len = 5.0f;

        // below
        const vec4 a = clamp_length(v, 10.0f, 15.0f);
        VX_CHECK_EQ(length(a), 10.0f);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // in
        const vec4 b = clamp_length(v, 1.0f, 10.0f);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // above
        const vec4 c = clamp_length(v, 1.0f, 4.0f);
        VX_CHECK_EQ(length(c), 4.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_aspect)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK_EQ(aspect(4.0f, 2.0f), 2.0f);
        VX_STATIC_CHECK_EQ(aspect(10.0f, 5.0f), 2.0f);
        VX_STATIC_CHECK_EQ(aspect(5.0f, 10.0f), 0.5f);

        // division by 0
        VX_STATIC_CHECK_EQ(aspect(1.0f, 0.0f), 0.0f);
        VX_STATIC_CHECK(is_zero_approx(aspect(1.0f, 1e-9f)));
    }

    VX_SECTION("vec2")
    {
        VX_STATIC_CHECK_EQ(aspect(vec2(4.0f, 2.0f)), 2.0f);
        VX_STATIC_CHECK_EQ(aspect(vec2(10.0f, 5.0f)), 2.0f);
        VX_STATIC_CHECK_EQ(aspect(vec2(5.0f, 10.0f)), 0.5f);

        // division by 0
        VX_STATIC_CHECK_EQ(aspect(vec2(1.0f, 0.0f)), 0.0f);
        VX_STATIC_CHECK(is_zero_approx(aspect(vec2(1.0f, 1e-9f))));
    }

    VX_SECTION("vec2i")
    {
        VX_STATIC_CHECK_EQ(aspect(vec2i(4, 2)), 2.0);
        VX_STATIC_CHECK_EQ(aspect(vec2i(10, 5)), 2.0);
        VX_STATIC_CHECK_EQ(aspect(vec2i(5, 10)), 0.5);

        // division by 0
        VX_STATIC_CHECK_EQ(aspect(vec2i(1, 0)), 0.0);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_area)
{
    constexpr vec2 a{ 4.0f, 5.0f };
    VX_STATIC_CHECK_EQ(area(a), 20.0f);

    constexpr vec2 b{ -3.0f, 7.0f };
    VX_STATIC_CHECK_EQ(area(b), -21.0f);

    constexpr vec2 c{ 0.0f, 5.0f };
    VX_STATIC_CHECK_EQ(area(c), 0.0f);

    constexpr vec2 d{ 0.0f, 0.0f };
    VX_STATIC_CHECK_EQ(area(d), 0.0f);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_volume)
{
    constexpr vec3 a{ 2.0f, 3.0f, 4.0f };
    VX_STATIC_CHECK_EQ(volume(a), 24.0f);

    constexpr vec3 b{ 0.0f, 5.0f, 9.0f };
    VX_STATIC_CHECK_EQ(volume(b), 0.0f);

    constexpr vec3 c{ -1.0f, 2.0f, -3.0f };
    VX_STATIC_CHECK_EQ(volume(c), 6.0f);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_angle)
{
    VX_SECTION("vec2")
    {
        VX_CHECK_EQ(angle(vec2{ 1.0f, 0.0f }), 0.0f);                // 0
        VX_CHECK_EQ(angle(vec2{ 0.0f, 1.0f }), half_pi);             // 90
        VX_CHECK_EQ(angle(vec2{ -1.0f, 0.0f }), pi);                 // 180
        VX_CHECK_EQ(angle(vec2{ 0.0f, -1.0f }), -half_pi);           // -90
        VX_CHECK_EQ(angle(vec2{ 1.0f, 1.0f }), quarter_pi);          // 45
    }

    VX_SECTION("vec2 angle between")
    {
        VX_CHECK_EQ(angle(vec2{ 1.0f, 0.0f }, vec2{ 1.0f, 0.0f }), 0.0f);
        VX_CHECK_EQ(angle(vec2{ 1.0f, 0.0f }, vec2{ 0.0f, 1.0f }), half_pi);
        VX_CHECK_EQ(angle(vec2{ 1.0f, 0.0f }, vec2{ -1.0f, 0.0f }), pi);
        VX_CHECK_EQ(angle(vec2{ 1.0f, 0.0f }, vec2{ 1.0f, 1.0f }), quarter_pi);
    }

    VX_SECTION("vec3 angle between")
    {
        VX_CHECK_EQ(angle(vec3{ 1.0f, 0.0f, 0.0f }, vec3{ 1.0f, 0.0f, 0.0f }), 0.0f);
        VX_CHECK_EQ(angle(vec3{ 1.0f, 0.0f, 0.0f }, vec3{ 0.0f, 1.0f, 0.0f }), half_pi);
        VX_CHECK_EQ(angle(vec3{ 1.0f, 0.0f, 0.0f }, vec3{ -1.0f, 0.0f, 0.0f }), pi);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_signed_angle)
{
    VX_SECTION("vec2")
    {
        VX_CHECK_EQ(signed_angle(vec2{ 1.0f, 0.0f }, vec2{ 0.0f, 1.0f }), half_pi);     // ccw
        VX_CHECK_EQ(signed_angle(vec2{ 0.0f, 1.0f }, vec2{ 1.0f, 0.0f }), -half_pi);    // cw
        VX_CHECK_EQ(signed_angle(vec2{ 1.0f, 0.0f }, vec2{ -1.0f, 0.0f }), pi);         // 180
        VX_CHECK_EQ(signed_angle(vec2{ 1.0f, 0.0f }, vec2{ 1.0f, 1.0f }), quarter_pi);  // 45
    }

    VX_SECTION("vec3")
    {
        const vec3 x_axis{ 1.0f, 0.0f, 0.0f };
        const vec3 y_axis{ 0.0f, 1.0f, 0.0f };
        const vec3 z_axis{ 0.0f, 0.0f, 1.0f };
        const vec3 neg_z_axis{ 0.0f, 0.0f, -1.0f };

        VX_CHECK(equal_approx(signed_angle(x_axis, y_axis, z_axis), half_pi));          // 90 ccw around Z
        VX_CHECK(equal_approx(signed_angle(x_axis, y_axis, neg_z_axis), -half_pi));     // 90 cw around Z
        VX_CHECK(equal_approx(signed_angle(x_axis, x_axis, z_axis), 0.0f));             // 0
        VX_CHECK(equal_approx(signed_angle(x_axis, -x_axis, z_axis), pi));              // 180 around Z
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_direction)
{
    VX_SECTION("direction from angle (radians to unit vector)")
    {
        VX_CHECK_EQ(direction(0.0f), vec2(1.0f, 0.0f));
        VX_CHECK_EQ(direction(half_pi), vec2(0.0f, 1.0f));
        VX_CHECK_EQ(direction(pi), vec2(-1.0f, 0.0f));
        VX_CHECK_EQ(direction(three_pi_over_two), vec2(0.0f, -1.0f));
        VX_CHECK_EQ(direction(two_pi), vec2(1.0f, 0.0f));

        // diagonals
        VX_CHECK_EQ(direction(quarter_pi), normalize(vec2(1.0f, 1.0f)));
        VX_CHECK_EQ(direction(three_quarters_pi), normalize(vec2(-1.0f, 1.0f)));
        VX_CHECK_EQ(direction(-quarter_pi), normalize(vec2(1.0f, -1.0f)));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_rotate)
{
    VX_SECTION("vec2")
    {
        const vec2 v{ 1.0f, 0.0f };

        VX_CHECK_EQ(rotate(v, half_pi), vec2(0.0f, 1.0f));               // 90 ccw
        VX_CHECK_EQ(rotate(v, pi), vec2(-1.0f, 0.0f));                   // 180
        VX_CHECK_EQ(rotate(v, -half_pi), vec2(0.0f, -1.0f));             // 90 cw (-90)
        VX_CHECK_EQ(rotate(v, two_pi), vec2(1.0f, 0.0f));                // 360
        VX_CHECK_EQ(rotate(v, quarter_pi), normalize(vec2(1.0f, 1.0f))); // 45
    }

    VX_SECTION("vec3")
    {
        const vec3 v{ 1.0f, 0.0f, 0.0f };
        const vec3 axis_x{ 1.0f, 0.0f, 0.0f };
        const vec3 axis_y{ 0.0f, 1.0f, 0.0f };
        const vec3 axis_z{ 0.0f, 0.0f, 1.0f };

        VX_CHECK_EQ(rotate(v, axis_z, half_pi), vec3(0.0f, 1.0f, 0.0f));                                // 90 around Z
        VX_CHECK_EQ(rotate(v, axis_z, pi), vec3(-1.0f, 0.0f, 0.0f));                                    // 180 around Z
        VX_CHECK_EQ(rotate(vec3(0.0f, 0.0f, 1.0f), axis_y, half_pi), vec3(1.0f, 0.0f, 0.0f));           // 90 around Y
        VX_CHECK_EQ(rotate(vec3(0.0f, 0.0f, 1.0f), axis_y, pi), vec3(0.0f, 0.0f, -1.0f));               // 180 around Y
        VX_CHECK_EQ(rotate(vec3(0.0f, 1.0f, 0.0f), axis_x, half_pi), vec3(0.0f, 0.0f, 1.0f));           // 90 around X

        // Rotating along same direction should give same vector
        VX_CHECK_EQ(rotate(v, v, half_pi), v);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_project)
{
    VX_SECTION("vec2")
    {
        const vec2 i{ 2.0f, 2.0f };
        const vec2 x_axis{ 1.0f, 0.0f };
        const vec2 y_axis{ 0.0f, 1.0f };
        const vec2 xy{ 1.0f, 1.0f };

        VX_CHECK_EQ(project(i, x_axis), vec2(2.0f, 0.0f));
        VX_CHECK_EQ(project(i, y_axis), vec2(0.0f, 2.0f));
        VX_CHECK_EQ(project(i, xy), i);
        VX_CHECK_EQ(project(i, -i), i);
    }

    VX_SECTION("vec3")
    {
        const vec3 i{ 3.0f, 3.0f, 3.0f };
        const vec3 x_axis{ 1.0f, 0.0f, 0.0f };
        const vec3 y_axis{ 0.0f, 1.0f, 0.0f };
        const vec3 z_axis{ 0.0f, 0.0f, 1.0f };
        const vec3 xyz{ 1.0f, 1.0f, 1.0f };
    
        VX_CHECK_EQ(project(i, x_axis), vec3(3.0f, 0.0f, 0.0f));
        VX_CHECK_EQ(project(i, y_axis), vec3(0.0f, 3.0f, 0.0f));
        VX_CHECK_EQ(project(i, z_axis), vec3(0.0f, 0.0f, 3.0f));
        VX_CHECK_EQ(project(i, xyz), i);
        VX_CHECK_EQ(project(i, -i), i);
    }
   
    VX_SECTION("vec4")
    {
        const vec4 i{ 4.0f, 4.0f, 4.0f, 4.0f };
        const vec4 x_axis{ 1.0f, 0.0f, 0.0f, 0.0f };
        const vec4 y_axis{ 0.0f, 1.0f, 0.0f, 0.0f };
        const vec4 z_axis{ 0.0f, 0.0f, 1.0f, 0.0f };
        const vec4 w_axis{ 0.0f, 0.0f, 0.0f, 1.0f };
        const vec4 xyzw{ 1.0f, 1.0f, 1.0f, 1.0f };

        VX_CHECK_EQ(project(i, x_axis), vec4(4.0f, 0.0f, 0.0f, 0.0f));
        VX_CHECK_EQ(project(i, y_axis), vec4(0.0f, 4.0f, 0.0f, 0.0f));
        VX_CHECK_EQ(project(i, z_axis), vec4(0.0f, 0.0f, 4.0f, 0.0f));
        VX_CHECK_EQ(project(i, w_axis), vec4(0.0f, 0.0f, 0.0f, 4.0f));
        VX_CHECK_EQ(project(i, xyzw), i);
        VX_CHECK_EQ(project(i, -i), i);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_reflect_bounce)
{
    VX_SECTION("vec2")
    {
        // reflect across horizontal surface (y-axis normal)
        {
            const vec2 i{ 1.0f, -1.0f };
            const vec2 n{ 0.0f, 1.0f };
            const vec2 r{ 1.0f, 1.0f };

            VX_CHECK_EQ(reflect(i, n), r);
            VX_CHECK_EQ(bounce(i, n), -r);
        }

        // reflect across vertical surface (x-axis normal)
        {
            const vec2 i{ -1.0f, 1.0f };
            const vec2 n{ 1.0f, 0.0f };
            const vec2 r{ 1.0f, 1.0f };

            VX_CHECK_EQ(reflect(i, n), r);
            VX_CHECK_EQ(bounce(i, n), -r);
        }

        // reflect across 45-degree surface
        {
            const vec2 i{ 1.0f, 0.0f };
            const vec2 n = normalize(vec2{ 1.0f, 1.0f });
            const vec2 r = normalize(vec2{ 0.0f, -1.0f });

            VX_CHECK_EQ(reflect(i, n), r);
            VX_CHECK_EQ(bounce(i, n), -r);
        }

        // reflect incoming vector opposite normal
        {
            const vec2 i = normalize(vec2{ -1.0f, 0.0f });
            const vec2 n = normalize(vec2{ -1.0f, 0.0f });
            const vec2 r = normalize(vec2{ 1.0f, 0.0f });

            VX_CHECK_EQ(reflect(i, n), r);
            VX_CHECK_EQ(bounce(i, n), -r);
        }
    }

    VX_SECTION("vec3")
    {
        // reflect across XY plane
        {
            const vec3 i{ 1.0f, -1.0f, 1.0f };
            const vec3 n{ 0.0f, 1.0f, 0.0f };
            const vec3 r{ 1.0f, 1.0f, 1.0f };

            VX_CHECK_EQ(reflect(i, n), r);
            VX_CHECK_EQ(bounce(i, n), -r);
        }

        // reflect across XZ plane
        {
            const vec3 i{ 0.0f, 1.0f, -1.0f };
            const vec3 n{ 0.0f, 0.0f, 1.0f };
            const vec3 r{ 0.0f, 1.0f, 1.0f };

            VX_CHECK_EQ(reflect(i, n), r);
            VX_CHECK_EQ(bounce(i, n), -r);
        }

        // reflect across YZ plane
        {
            const vec3 i{ -1.0f, 1.0f, 1.0f };
            const vec3 n{ 1.0f, 0.0f, 0.0f };
            const vec3 r{ 1.0f, 1.0f, 1.0f };

            VX_CHECK_EQ(reflect(i, n), r);
            VX_CHECK_EQ(bounce(i, n), -r);
        }

        // reflect with arbitrary normal, non-axis aligned
        {
            const vec3 i = normalize(vec3{ 1.0f, 2.0f, 3.0f });
            const vec3 n = normalize(vec3{ 1.0f, 1.0f, 0.0f });
            const vec3 r{ -0.534522414f, -0.267261207f, 0.801783681f };

            VX_CHECK_EQ(reflect(i, n), r);
            VX_CHECK_EQ(bounce(i, n), -r);
        }
    }

    VX_SECTION("vec4")
    {
        // reflect across W-axis surface (normal in w-direction)
        {
            const vec4 i{ 1.0f, 1.0f, 1.0f, -1.0f };
            const vec4 n{ 0.0f, 0.0f, 0.0f, 1.0f };
            const vec4 r{ 1.0f, 1.0f, 1.0f, 1.0f };

            VX_CHECK_EQ(reflect(i, normalize(n)), r);
            VX_CHECK_EQ(bounce(i, normalize(n)), -r);
        }

        // reflect across arbitrary 4D plane
        {
            const vec4 i = normalize(vec4{ 1.0f, -2.0f, 3.0f, -4.0f });
            const vec4 n = normalize(vec4{ 2.0f, 1.0f, 0.0f, 3.0f });
            const vec4 r{ 0.808542728f, -0.0521640778f, 0.547722578f, 0.208656073f };

            VX_CHECK_EQ(reflect(i, n), r);
            VX_CHECK_EQ(bounce(i, n), -r);
        }

        // reflect incoming vector exactly opposite to normal
        {
            const vec4 i = normalize(vec4{ -1.0f, -1.0f, -1.0f, -1.0f });
            const vec4 n = normalize(vec4{ -1.0f, -1.0f, -1.0f, -1.0f });
            const vec4 r = normalize(vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

            VX_CHECK_EQ(reflect(i, n), r);
            VX_CHECK_EQ(bounce(i, n), -r);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_refract)
{
    VX_SECTION("vec2")
    {
        // Rare to Dense: air (1.0) to water (1.33)
        {
            const vec2 i = normalize(vec2{ 1.0f, -1.0f });
            const vec2 n = normalize(vec2{ 0.0f, 1.0f });
            const float eta = 1.0f / 1.33f;

            const vec2 r = refract(i, n, eta);
            VX_CHECK_EQ(r, vec2(0.531659245f, -0.846958339f));
        }

        // Dense to Rare: water (1.33) to air (1.0)
        {
            const vec2 i = normalize(vec2{ 1.0f, -1.0f });
            const vec2 n = normalize(vec2{ 0.0f, 1.0f });
            const float eta = 1.33f;

            const vec2 r = refract(i, n, eta);
            VX_CHECK_EQ(r, vec2(0.940452039f, -0.339926362f));
        }

        // No Refraction: incoming aligned with normal
        {
            const vec2 i = normalize(vec2{ 0.0f, -1.0f });
            const vec2 n = normalize(vec2{ 0.0f, 1.0f });
            const float eta = 1.0f / 1.33f;

            const vec2 r = refract(i, n, eta);
            VX_CHECK_EQ(r, i);
        }

        // Total Internal Reflection
        {
            const vec2 i = normalize(vec2{ 1.0f, -0.01f });
            const vec2 n = normalize(vec2{ 0.0f, 1.0f });
            const float eta = 1.33f;

            const vec2 r = refract(i, n, eta);
            VX_CHECK_EQ(r, vec2(0.0f, 0.0f));

        }
    }

    VX_SECTION("vec3")
    {
        // Rare to Dense: air (1.0) to water (1.33)
        {
            const vec3 i = normalize(vec3{ 0.8f, -0.6f, 0.2f });
            const vec3 n = normalize(vec3{ 0.0f, 1.0f, 0.0f });
            const float eta = 1.0f / 1.33f;

            const vec3 r = refract(i, n, eta);
            VX_CHECK_EQ(r, vec3(0.589822948f, -0.793955743f, 0.147455737f));
        }

        // Dense to Rare: water (1.33) to air (1.0)
        {
            const vec3 i = normalize(vec3{ 0.2f, -1.0f, 0.2f });
            const vec3 n = normalize(vec3{ 0.0f, 1.0f, 0.0f });
            const float eta = 1.33f;

            const vec3 r = refract(i, n, eta);
            VX_CHECK_EQ(r, vec3(0.255958617f, -0.932185829f, 0.255958617f));
        }

        // No Refraction: incoming aligned with normal
        {
            const vec3 i = normalize(vec3{ 0.0f, -1.0f, 0.0f });
            const vec3 n = normalize(vec3{ 0.0f, 1.0f, 0.0f });
            const float eta = 1.0f / 1.33f;

            const vec3 r = refract(i, n, eta);
            VX_CHECK_EQ(r, i);
        }

        // Total Internal Reflection
        {
            const vec3 i = normalize(vec3{ 0.99f, -0.01f, 0.1f });
            const vec3 n = normalize(vec3{ 0.0f, 1.0f, 0.0f });
            const float eta = 1.33f;

            const vec3 r = refract(i, n, eta);
            VX_CHECK_EQ(r, vec3(0.0f, 0.0f, 0.0f));
        }
    }

    VX_SECTION("vec4")
    {
        // Rare to Dense: air (1.0) to water (1.33)
        {
            const vec4 i = normalize(vec4{ 0.6f, -0.5f, 0.3f, 0.2f });
            const vec4 n = normalize(vec4{ 0.0f, 1.0f, 0.0f, 0.0f });
            const float eta = 1.0f / 1.33f;

            const vec4 r = refract(i, n, eta);
            VX_CHECK_EQ(r, vec4(0.524425387f, -0.790989578f, 0.262212694f, 0.174808472f));
        }

        // Dense to Rare: water (1.33) to air (1.0)
        {
            const vec4 i = normalize(vec4{ 0.3f, -1.0f, 0.4f, 0.1f });
            const vec4 n = normalize(vec4{ 0.0f, 1.0f, 0.0f, 0.0f });
            const float eta = 1.33f;

            const vec4 r = refract(i, n, eta);
            VX_CHECK_EQ(r, vec4(0.355457485f, -0.796861887f, 0.473943293f, 0.118485823f));
        }

        // No Refraction: incoming aligned with normal
        {
            const vec4 i = normalize(vec4{ 0.0f, -1.0f, 0.0f, 0.0f });
            const vec4 n = normalize(vec4{ 0.0f, 1.0f, 0.0f, 0.0f });
            const float eta = 1.0f / 1.33f;

            const vec4 r = refract(i, n, eta);
            VX_CHECK_EQ(r, i);
        }

        // Total Internal Reflection
        {
            const vec4 i = normalize(vec4{ 0.95f, -0.01f, 0.25f, 0.2f });
            const vec4 n = normalize(vec4{ 0.0f, 1.0f, 0.0f, 0.0f });
            const float eta = 1.33f;

            const vec4 r = refract(i, n, eta);
            VX_CHECK_EQ(r, vec4(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_face_forward)
{
    VX_SECTION("vec2")
    {
        // as is
        {
            const vec2 n{ 0.0f, 1.0f };
            const vec2 i{ 0.0f, -1.0f };
            const vec2 nref{ 0.0f, 1.0f };
            VX_CHECK_EQ(face_forward(n, i, nref), n);
        }

        // reverse
        {
            const vec2 n{ 0.0f, 1.0f };
            const vec2 i{ 0.0f, 1.0f };
            const vec2 nref{ 0.0f, 1.0f };
            VX_CHECK_EQ(face_forward(n, i, nref), -n);
        }
    }

    VX_SECTION("vec3")
    {
        // as is
        {
            const vec3 n{ 0.0f, 0.0f, 1.0f };
            const vec3 i{ 0.0f, 0.0f, -1.0f };
            const vec3 nref{ 0.0f, 0.0f, 1.0f };
            VX_CHECK_EQ(face_forward(n, i, nref), n);
        }

        // reverse
        {
            const vec3 n{ 0.0f, 0.0f, 1.0f };
            const vec3 i{ 0.0f, 0.0f, 1.0f };
            const vec3 nref{ 0.0f, 0.0f, 1.0f };
            VX_CHECK_EQ(face_forward(n, i, nref), -n);
        }
    }

    VX_SECTION("vec4")
    {
        // as is
        {
            const vec4 n{ 0.0f, 0.0f, 0.0f, 1.0f };
            const vec4 i{ 0.0f, 0.0f, 0.0f, -1.0f };
            const vec4 nref{ 0.0f, 0.0f, 0.0f, 1.0f };
            VX_CHECK_EQ(face_forward(n, i, nref), n);
        }

        // reverse
        {
            const vec4 n{ 0.0f, 0.0f, 0.0f, 1.0f };
            const vec4 i{ 0.0f, 0.0f, 0.0f, 1.0f };
            const vec4 nref{ 0.0f, 0.0f, 0.0f, 1.0f };
            VX_CHECK_EQ(face_forward(n, i, nref), -n);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}