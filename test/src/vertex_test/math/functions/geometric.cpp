#include "vertex_test/test.hpp"

#include "vertex/math/functions/geometric.hpp"

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
        vec2 a{ 3.0f, -4.0f };
        VX_CHECK_EQ(length_squared(a), 25.0f);
        VX_CHECK_EQ(length(a), 5.0f);

        vec2 zero{ 0.0f, 0.0f };
        VX_CHECK_EQ(length_squared(zero), 0.0f);
        VX_CHECK_EQ(length(zero), 0.0f);
    }

    VX_SECTION("vec3")
    {
        vec3 a{ 1.0f, -2.0f, 3.0f };
        VX_CHECK_EQ(length_squared(a), 14.0f);
        VX_CHECK_EQ(length(a), 3.74165738677f);

        vec3 zero{ 0.0f, 0.0f, 0.0f };
        VX_CHECK_EQ(length_squared(zero), 0.0f);
        VX_CHECK_EQ(length(zero), 0.0f);
    }

    VX_SECTION("vec4")
    {
        vec4 a{ 1.0f, -2.0f, 3.0f, -4.0f };
        VX_CHECK_EQ(length_squared(a), 30.0f);
        VX_CHECK_EQ(length(a), 5.47722557505f);

        vec4 zero{ 0.0f, 0.0f, 0.0f, 0.0f };
        VX_CHECK_EQ(length_squared(zero), 0.0f);
        VX_CHECK_EQ(length(zero), 0.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_distanced)
{
    VX_SECTION("vec2")
    {
        vec2 p0{ 0.0f, 0.0f };
        vec2 p1{ 3.0f, 4.0f };
        VX_CHECK_EQ(distance_squared(p0, p1), 25.0f);
        VX_CHECK_EQ(distance(p0, p1), 5.0f);

        vec2 p2{ 3.0f, 4.0f };
        VX_CHECK_EQ(distance_squared(p2, p2), 0.0f);
        VX_CHECK_EQ(distance(p2, p2), 0.0f);

        vec2 p3{ -1.0f, -1.0f };
        vec2 p4{ 1.0f, 1.0f };
        VX_CHECK_EQ(distance_squared(p3, p4), 8.0f);
        VX_CHECK_EQ(distance(p3, p4), 2.82842712475f);
    }

    VX_SECTION("vec3")
    {
        vec3 p0{ 0.0f, 0.0f, 0.0f };
        vec3 p1{ 1.0f, 2.0f, 2.0f };
        VX_CHECK_EQ(distance_squared(p0, p1), 9.0f);
        VX_CHECK_EQ(distance(p0, p1), 3.0f);

        vec3 p2{ 1.0f, 1.0f, 1.0f };
        VX_CHECK_EQ(distance_squared(p2, p2), 0.0f);
        VX_CHECK_EQ(distance(p2, p2), 0.0f);

        vec3 p3{ -1.0f, -1.0f, -1.0f };
        vec3 p4{ 1.0f, 1.0f, 1.0f };
        VX_CHECK_EQ(distance_squared(p3, p4), 12.0f);
        VX_CHECK_EQ(distance(p3, p4), 3.46410161514f);
    }

    VX_SECTION("vec4")
    {
        vec4 p0{ 1.0f, 2.0f, 3.0f, 4.0f };
        vec4 p1{ 5.0f, 6.0f, 7.0f, 8.0f };
        VX_CHECK_EQ(distance_squared(p0, p1), 64.0f);
        VX_CHECK_EQ(distance(p0, p1), 8.0f);

        vec4 p2{ 0.0f, 0.0f, 0.0f, 0.0f };
        VX_CHECK_EQ(distance_squared(p2, p2), 0.0f);
        VX_CHECK_EQ(distance(p2, p2), 0.0f);

        vec4 p3{ -1.0f, -1.0f, -1.0f, -1.0f };
        vec4 p4{ 1.0f, 1.0f, 1.0f, 1.0f };
        VX_CHECK_EQ(distance_squared(p3, p4), 16.0f);
        VX_CHECK_EQ(distance(p3, p4), 4.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_normalize)
{
    VX_SECTION("vec2")
    {
        vec2 v{ 3.0f, 4.0f };
        vec2 n = normalize(v);
        VX_CHECK_EQ(length(n), 1.0f);
        VX_CHECK(is_normalized(n));

        vec2 nf = normalize_fast(v);
        VX_CHECK_EQ(length(nf), 1.0f);
        VX_CHECK(is_normalized(nf));

        vec2 zero{ 0.0f, 0.0f };
        vec2 n_zero = normalize(zero);
        VX_CHECK(is_zero_approx(zero));
        VX_CHECK(!is_normalized(zero));
    }

    VX_SECTION("vec3")
    {
        vec3 v{ 1.0f, 2.0f, 2.0f };
        vec3 n = normalize(v);
        VX_CHECK_EQ(length(n), 1.0f);
        VX_CHECK(is_normalized(n));

        vec3 nf = normalize_fast(v);
        VX_CHECK_EQ(length(nf), 1.0f);
        VX_CHECK(is_normalized(nf));

        vec3 zero{ 0.0f, 0.0f, 0.0f };
        vec3 n_zero = normalize(zero);
        VX_CHECK(is_zero_approx(zero));
        VX_CHECK(!is_normalized(n_zero));
    }

    VX_SECTION("vec4")
    {
        vec4 v{ 1.0f, 2.0f, 2.0f, 3.0f };
        vec4 n = normalize(v);
        VX_CHECK_EQ(length(n), 1.0f);
        VX_CHECK(is_normalized(n));

        vec4 nf = normalize_fast(v);
        VX_CHECK_EQ(length(nf), 1.0f);
        VX_CHECK(is_normalized(nf));

        vec4 zero{ 0.0f, 0.0f, 0.0f, 0.0f };
        vec4 n_zero = normalize(zero);
        VX_CHECK(is_zero_approx(zero));
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
        vec2 a{ 1.0f, 0.0f };
        vec2 b{ 2.0f, 0.0f };
        VX_CHECK_EQ(normalized_dot(a, b), 1.0f);

        // Opposite direction: dot = -1
        vec2 c{ 1.0f, 0.0f };
        vec2 d{ -1.0f, 0.0f };
        VX_CHECK_EQ(normalized_dot(c, d), -1.0f);

        // Orthogonal: dot = 0
        vec2 e{ 0.0f, 1.0f };
        VX_CHECK_EQ(normalized_dot(a, e), 0.0f);

        // Diagonal 45: dot = cos(45) = sqrt(2) / 2
        vec2 diag{ 1.0f, 1.0f };
        VX_CHECK_EQ(normalized_dot(a, diag), 0.70710678118f);
    }

    VX_SECTION("vec3")
    {
        vec3 x{ 1.0f, 0.0f, 0.0f };
        vec3 y{ 0.0f, 1.0f, 0.0f };
        vec3 z{ -1.0f, 0.0f, 0.0f };

        VX_CHECK_EQ(normalized_dot(x, x), 1.0f);
        VX_CHECK_EQ(normalized_dot(x, z), -1.0f);
        VX_CHECK_EQ(normalized_dot(x, y), 0.0f);
    }

    VX_SECTION("vec4")
    {
        vec4 a{ 1.0f, 1.0f, 1.0f, 1.0f };
        vec4 b{ -1.0f, -1.0f, -1.0f, -1.0f };
        vec4 c{ 1.0f, -1.0f, 1.0f, -1.0f };

        VX_CHECK_EQ(normalized_dot(a, b), -1.0f);
        VX_CHECK_EQ(normalized_dot(a, a), 1.0f);
        VX_CHECK_EQ(normalized_dot(a, c), 0.0f);
    }

    VX_SECTION("zero vectors")
    {
        vec3 zero{ 0.0f, 0.0f, 0.0f };
        vec3 v{ 1.0f, 2.0f, 3.0f };

        auto r1 = normalized_dot(zero, v);
        auto r2 = normalized_dot(v, zero);
        auto r3 = normalized_dot(zero, zero);

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
        vec2 a = set_length(v, len * 2);
        VX_CHECK_EQ(length(a), len * 2);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // same
        vec2 b = set_length(v, len);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // shrink
        vec2 c = set_length(v, 1.0f);
        VX_CHECK_EQ(length(c), 1.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);

        // negate
        vec2 d = set_length(v, -len);
        VX_CHECK_EQ(length(d), len);
        VX_CHECK_EQ(normalized_dot(v, d), -1.0f);

        // zero
        vec2 zero(0.0f, 0.0f);
        vec2 e = set_length(zero, len);
        VX_CHECK(is_zero_approx(e));
    }

    VX_SECTION("vec3")
    {
        const vec3 v(2.0f, 3.0f, 6.0f);
        constexpr float len = 7.0f;

        // grow
        vec3 a = set_length(v, len * 2);
        VX_CHECK_EQ(length(a), len * 2);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // same
        vec3 b = set_length(v, len);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // shrink
        vec3 c = set_length(v, 1.0f);
        VX_CHECK_EQ(length(c), 1.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);

        // negate
        vec3 d = set_length(v, -len);
        VX_CHECK_EQ(length(d), len);
        VX_CHECK_EQ(normalized_dot(v, d), -1.0f);

        // zero
        vec3 zero(0.0f, 0.0f, 0.0f);
        vec3 e = set_length(zero, len);
        VX_CHECK(is_zero_approx(e));
    }

    VX_SECTION("vec4")
    {
        const vec4 v(1.0f, 2.0f, 2.0f, 4.0f);
        constexpr float len = 5.0f;

        // grow
        vec4 a = set_length(v, len * 2);
        VX_CHECK_EQ(length(a), len * 2);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // same
        vec4 b = set_length(v, len);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // shrink
        vec4 c = set_length(v, 1.0f);
        VX_CHECK_EQ(length(c), 1.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);

        // negate
        vec4 d = set_length(v, -len);
        VX_CHECK_EQ(length(d), len);
        VX_CHECK_EQ(normalized_dot(v, d), -1.0f);

        // zero
        vec4 zero(0.0f, 0.0f, 0.0f, 0.0f);
        vec4 e = set_length(zero, len);
        VX_CHECK(is_zero_approx(e));
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
        vec2 a = clamp_length(v, 7.0f, 10.0f);
        VX_CHECK_EQ(length(a), 7.0f);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // in
        vec2 b = clamp_length(v, 1.0f, 7.0f);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // above
        vec2 c = clamp_length(v, 1.0f, 4.0f);
        VX_CHECK_EQ(length(c), 4.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);

        // zero
        vec2 zero(0.0f, 0.0f);
        vec2 d = clamp_length(zero, 1.0f, 2.0f);
        VX_CHECK(is_zero_approx(d));
    }

    VX_SECTION("vec3")
    {
        const vec3 v(2.0f, 3.0f, 6.0f);
        constexpr float len = 7.0f;

        // below
        vec3 a = clamp_length(v, 10.0f, 15.0f);
        VX_CHECK_EQ(length(a), 10.0f);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // in
        vec3 b = clamp_length(v, 1.0f, 10.0f);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // above
        vec3 c = clamp_length(v, 1.0f, 4.0f);
        VX_CHECK_EQ(length(c), 4.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);

        // zero
        vec3 zero(0.0f, 0.0f, 0.0f);
        vec3 d = clamp_length(zero, 1.0f, 2.0f);
        VX_CHECK(is_zero_approx(d));
    }

    VX_SECTION("vec4")
    {
        const vec4 v(1.0f, 2.0f, 2.0f, 4.0f);
        constexpr float len = 5.0f;

        // below
        vec4 a = clamp_length(v, 10.0f, 15.0f);
        VX_CHECK_EQ(length(a), 10.0f);
        VX_CHECK_EQ(normalized_dot(v, a), 1.0f);

        // in
        vec4 b = clamp_length(v, 1.0f, 10.0f);
        VX_CHECK_EQ(length(b), len);
        VX_CHECK_EQ(normalized_dot(v, b), 1.0f);

        // above
        vec4 c = clamp_length(v, 1.0f, 4.0f);
        VX_CHECK_EQ(length(c), 4.0f);
        VX_CHECK_EQ(normalized_dot(v, c), 1.0f);

        // zero
        vec4 zero(0.0f, 0.0f, 0.0f, 0.0f);
        vec4 d = clamp_length(zero, 1.0f, 2.0f);
        VX_CHECK(is_zero_approx(d));
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
        VX_CHECK_EQ(signed_angle(vec2{ 1.0f, 0.0f }, vec2{ 0.0f, 1.0f }), half_pi);     // CCW
        VX_CHECK_EQ(signed_angle(vec2{ 0.0f, 1.0f }, vec2{ 1.0f, 0.0f }), -half_pi);    // CW
        VX_CHECK_EQ(signed_angle(vec2{ 1.0f, 0.0f }, vec2{ -1.0f, 0.0f }), pi);         // 180
        VX_CHECK_EQ(signed_angle(vec2{ 1.0f, 0.0f }, vec2{ 1.0f, 1.0f }), quarter_pi);  // 45
    }

    VX_SECTION("vec3")
    {
        const vec3 x_axis{ 1, 0, 0 };
        const vec3 y_axis{ 0, 1, 0 };
        const vec3 z_axis{ 0, 0, 1 };
        const vec3 neg_z_axis{ 0, 0, -1 };

        VX_CHECK(equal_approx(signed_angle(x_axis, y_axis, z_axis), half_pi));          // 90-degree CCW around Z
        VX_CHECK(equal_approx(signed_angle(x_axis, y_axis, neg_z_axis), -half_pi));     // 90-degree CW around Z
        VX_CHECK(equal_approx(signed_angle(x_axis, x_axis, z_axis), 0.0f));             // 0-degree
        VX_CHECK(equal_approx(signed_angle(x_axis, -x_axis, z_axis), pi));              // 180-degree rotation
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

int main()
{
    VX_RUN_TESTS();
    return 0;
}