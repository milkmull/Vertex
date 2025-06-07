#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/interpolation.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_lerp)
{
    VX_SECTION("scalar")
    {
        constexpr f32 min = 2.0f;
        constexpr f32 max = 10.0f;
        constexpr f32 range = max - min;

        VX_STATIC_CHECK_EQ(lerp(min, max, 0.0f), min);
        VX_STATIC_CHECK_EQ(lerp(min, max, 1.0f), max);
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.5f), min + (range * 0.5f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.25f), min + (range * 0.25f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.75f), min + (range * 0.75f));

        // same x and y -> result should be constant
        VX_STATIC_CHECK_EQ(lerp(5.0f, 5.0f, 0.3f), 5.0f);

        // t < 0 and t > 1 -> extrapolation
        VX_STATIC_CHECK_EQ(lerp(min, max, -1.0f), -6.0f);
        VX_STATIC_CHECK_EQ(lerp(min, max, 2.0f), 18.0f);

        // clamped
        VX_STATIC_CHECK(lerp_clamped(min, max, -1.0f) == min);
        VX_STATIC_CHECK(lerp_clamped(min, max, 0.5f) == min + (range * 0.5f));
        VX_STATIC_CHECK(lerp_clamped(min, max, 2.0f) == max);
    }

    VX_SECTION("vec2 scalar t")
    {
        constexpr vec2 min(2.0f, -10.0f);
        constexpr vec2 max(10.0f, -2.0f);
        constexpr vec2 range = max - min;

        VX_STATIC_CHECK_EQ(lerp(min, max, 0.0f), min);
        VX_STATIC_CHECK_EQ(lerp(min, max, 1.0f), max);
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.5f), min + (range * 0.5f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.25f), min + (range * 0.25f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.75f), min + (range * 0.75f));

        // same x and y -> result should be constant
        VX_STATIC_CHECK_EQ(lerp(vec2(5.0f), vec2(5.0f), 0.3f), vec2(5.0f));

        // t < 0 and t > 1 -> extrapolation
        VX_STATIC_CHECK_EQ(lerp(min, max, -1.0f), vec2(-6.0f, -18.0f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 2.0f), vec2(18.0f, 6.0f));

        // clamped
        VX_STATIC_CHECK(lerp_clamped(min, max, -1.0f) == min);
        VX_STATIC_CHECK(lerp_clamped(min, max, 0.5f) == min + (range * 0.5f));
        VX_STATIC_CHECK(lerp_clamped(min, max, 2.0f) == max);
    }

    VX_SECTION("vec3 scalar t")
    {
        constexpr vec3 min(2.0f, -10.0f, 5.0f);
        constexpr vec3 max(10.0f, -2.0f, 13.0f);
        constexpr vec3 range = max - min;

        VX_STATIC_CHECK_EQ(lerp(min, max, 0.0f), min);
        VX_STATIC_CHECK_EQ(lerp(min, max, 1.0f), max);
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.5f), min + (range * 0.5f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.25f), min + (range * 0.25f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.75f), min + (range * 0.75f));

        // same x and y -> result should be constant
        VX_STATIC_CHECK_EQ(lerp(vec3(5.0f), vec3(5.0f), 0.3f), vec3(5.0f));

        // t < 0 and t > 1 -> extrapolation
        VX_STATIC_CHECK_EQ(lerp(min, max, -1.0f), vec3(-6.0f, -18.0f, -3.0f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 2.0f), vec3(18.0f, 6.0f, 21.0f));

        // clamped
        VX_STATIC_CHECK(lerp_clamped(min, max, -1.0f) == min);
        VX_STATIC_CHECK(lerp_clamped(min, max, 0.5f) == min + (range * 0.5f));
        VX_STATIC_CHECK(lerp_clamped(min, max, 2.0f) == max);
    }

    VX_SECTION("vec4 scalar t")
    {
        constexpr vec4 min(2.0f, -10.0f, 5.0f, 0.5f);
        constexpr vec4 max(10.0f, -2.0f, 13.0f, 8.5f);
        constexpr vec4 range = max - min;

        VX_STATIC_CHECK_EQ(lerp(min, max, 0.0f), min);
        VX_STATIC_CHECK_EQ(lerp(min, max, 1.0f), max);
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.5f), min + (range * 0.5f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.25f), min + (range * 0.25f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 0.75f), min + (range * 0.75f));

        // same x and y -> result should be constant
        VX_STATIC_CHECK_EQ(lerp(vec4(5.0f), vec4(5.0f), 0.3f), vec4(5.0f));

        // t < 0 and t > 1 -> extrapolation
        VX_STATIC_CHECK_EQ(lerp(min, max, -1.0f), vec4(-6.0f, -18.0f, -3.0f, -7.5f));
        VX_STATIC_CHECK_EQ(lerp(min, max, 2.0f), vec4(18.0f, 6.0f, 21.0f, 16.5f));

        // clamped
        VX_STATIC_CHECK(lerp_clamped(min, max, -1.0f) == min);
        VX_STATIC_CHECK(lerp_clamped(min, max, 0.5f) == min + (range * 0.5f));
        VX_STATIC_CHECK(lerp_clamped(min, max, 2.0f) == max);
    }

    VX_SECTION("vec2 vector t")
    {
        constexpr vec2 min(2.0f, -10.0f);
        constexpr vec2 max(10.0f, -2.0f);
        constexpr vec2 range = max - min;

        VX_STATIC_CHECK_EQ(lerp(min, max, vec2(0.0f)), min);
        VX_STATIC_CHECK_EQ(lerp(min, max, vec2(1.0f)), max);
        VX_STATIC_CHECK_EQ(lerp(min, max, vec2(0.5f)), vec2(6.0f, -6.0f));
        VX_STATIC_CHECK_EQ(lerp(min, max, vec2(0.25f, 0.75f)), vec2(4.0f, -4.0f));

        // same x and y -> result should be constant
        VX_STATIC_CHECK_EQ(lerp(vec2(5.0f), vec2(5.0f), vec2(0.3f, 0.7f)), vec2(5.0f));

        // t < 0 and t > 1 -> extrapolation
        VX_STATIC_CHECK_EQ(lerp(min, max, vec2(-1.0f, 2.0f)), vec2(-6.0f, 6.0f));

        // clamped
        VX_STATIC_CHECK(lerp_clamped(min, max, -1.0f) == min);
        VX_STATIC_CHECK(lerp_clamped(min, max, 0.5f) == min + (range * 0.5f));
        VX_STATIC_CHECK(lerp_clamped(min, max, 2.0f) == max);
    }

    VX_SECTION("vec3 vector t")
    {
        constexpr vec3 min(2.0f, -10.0f, 1.0f);
        constexpr vec3 max(10.0f, -2.0f, 9.0f);
        constexpr vec3 range = max - min;

        VX_STATIC_CHECK_EQ(lerp(min, max, vec3(0.0f)), min);
        VX_STATIC_CHECK_EQ(lerp(min, max, vec3(1.0f)), max);
        VX_STATIC_CHECK_EQ(lerp(min, max, vec3(0.5f)), vec3(6.0f, -6.0f, 5.0f));
        VX_STATIC_CHECK_EQ(lerp(min, max, vec3(0.25f, 0.75f, 0.1f)), vec3(4.0f, -4.0f, 1.8f));

        // same x and y -> result should be constant
        VX_STATIC_CHECK_EQ(lerp(vec3(5.0f), vec3(5.0f), vec3(0.3f, 0.7f, 0.1f)), vec3(5.0f));

        // t < 0 and t > 1 -> extrapolation
        VX_STATIC_CHECK_EQ(lerp(min, max, vec3(-1.0f, 2.0f, 1.5f)), vec3(-6.0f, 6.0f, 13.0f));

        // clamped
        VX_STATIC_CHECK(lerp_clamped(min, max, -1.0f) == min);
        VX_STATIC_CHECK(lerp_clamped(min, max, 0.5f) == min + (range * 0.5f));
        VX_STATIC_CHECK(lerp_clamped(min, max, 2.0f) == max);
    }

    VX_SECTION("vec4 vector t")
    {
        constexpr vec4 min(2.0f, -10.0f, 1.0f, -3.0f);
        constexpr vec4 max(10.0f, -2.0f, 9.0f, 5.0f);
        constexpr vec4 range = max - min;

        VX_STATIC_CHECK_EQ(lerp(min, max, vec4(0.0f)), min);
        VX_STATIC_CHECK_EQ(lerp(min, max, vec4(1.0f)), max);
        VX_STATIC_CHECK_EQ(lerp(min, max, vec4(0.5f)), vec4(6.0f, -6.0f, 5.0f, 1.0f));
        VX_STATIC_CHECK_EQ(lerp(min, max, vec4(0.25f, 0.75f, 0.1f, 0.9f)), vec4(4.0f, -4.0f, 1.8f, 4.2f));

        // same x and y -> result should be constant
        VX_STATIC_CHECK_EQ(lerp(vec4(5.0f), vec4(5.0f), vec4(0.3f, 0.7f, 0.1f, 0.5f)), vec4(5.0f));

        // t < 0 and t > 1 -> extrapolation
        VX_STATIC_CHECK_EQ(lerp(min, max, vec4(-1.0f, 2.0f, 1.5f, -0.5f)), vec4(-6.0f, 6.0f, 13.0f, -7.0f));

        // clamped
        VX_STATIC_CHECK(lerp_clamped(min, max, -1.0f) == min);
        VX_STATIC_CHECK(lerp_clamped(min, max, 0.5f) == min + (range * 0.5f));
        VX_STATIC_CHECK(lerp_clamped(min, max, 2.0f) == max);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_step)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK_EQ(step(0.0f, -1.0f), 0.0f);
        VX_STATIC_CHECK_EQ(step(0.0f, 0.0f), 1.0f);
        VX_STATIC_CHECK_EQ(step(0.0f, 1.0f), 1.0f);
        VX_STATIC_CHECK_EQ(step(5.0f, 2.0f), 0.0f);
        VX_STATIC_CHECK_EQ(step(5.0f, 5.0f), 1.0f);
        VX_STATIC_CHECK_EQ(step(5.0f, 10.0f), 1.0f);
    }

    VX_SECTION("vec2 scalar edge")
    {
        VX_STATIC_CHECK_EQ(step(0.0f, vec2(-1.0f, 2.0f)), vec2(0.0f, 1.0f));
        VX_STATIC_CHECK_EQ(step(1.0f, vec2(0.5f, 1.5f)), vec2(0.0f, 1.0f));
        VX_STATIC_CHECK_EQ(step(-2.0f, vec2(-3.0f, -1.0f)), vec2(0.0f, 1.0f));
    }

    VX_SECTION("vec3 scalar edge")
    {
        VX_STATIC_CHECK_EQ(step(0.0f, vec3(-1.0f, 0.0f, 1.0f)), vec3(0.0f, 1.0f, 1.0f));
        VX_STATIC_CHECK_EQ(step(2.0f, vec3(1.0f, 2.0f, 3.0f)), vec3(0.0f, 1.0f, 1.0f));
        VX_STATIC_CHECK_EQ(step(-1.0f, vec3(-2.0f, -1.0f, 0.0f)), vec3(0.0f, 1.0f, 1.0f));
    }

    VX_SECTION("vec4 scalar edge")
    {
        VX_STATIC_CHECK_EQ(
            step(0.0f, vec4(-1.0f, 0.0f, 1.0f, 2.0f)),
            vec4(0.0f, 1.0f, 1.0f, 1.0f)
        );

        VX_STATIC_CHECK_EQ(
            step(2.0f, vec4(1.0f, 2.0f, 3.0f, 4.0f)),
            vec4(0.0f, 1.0f, 1.0f, 1.0f)
        );

        VX_STATIC_CHECK_EQ(
            step(1.0f, vec4(0.0f, 1.0f, 2.0f, 0.5f)),
            vec4(0.0f, 1.0f, 1.0f, 0.0f)
        );
    }

    VX_SECTION("vec2 vector edge")
    {
        VX_STATIC_CHECK_EQ(
            step(vec2(0.0f, 1.0f), vec2(-1.0f, 2.0f)),
            vec2(0.0f, 1.0f)
        );

        VX_STATIC_CHECK_EQ(
            step(vec2(1.0f, 1.0f), vec2(0.5f, 1.5f)),
            vec2(0.0f, 1.0f)
        );

        VX_STATIC_CHECK_EQ(
            step(vec2(-3.0f, -2.0f), vec2(-2.0f, -1.0f)),
            vec2(1.0f, 1.0f)
        );
    }


    VX_SECTION("vec3 vector edge")
    {
        VX_STATIC_CHECK_EQ(
            step(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 1.0f)),
            vec3(0.0f, 1.0f, 1.0f)
        );

        VX_STATIC_CHECK_EQ(
            step(vec3(2.0f, 2.0f, 2.0f), vec3(1.0f, 2.0f, 3.0f)),
            vec3(0.0f, 1.0f, 1.0f)
        );

        VX_STATIC_CHECK_EQ(
            step(vec3(-2.0f, -1.0f, 0.0f), vec3(-2.0f, -1.0f, 0.0f)),
            vec3(1.0f, 1.0f, 1.0f)
        );
    }

    VX_SECTION("vec4 vector edge")
    {
        VX_STATIC_CHECK_EQ(
            step(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(-1.0f, 0.0f, 1.0f, 2.0f)),
            vec4(0.0f, 1.0f, 1.0f, 1.0f)
        );

        VX_STATIC_CHECK_EQ(
            step(vec4(2.0f), vec4(1.0f, 2.0f, 3.0f, 4.0f)),
            vec4(0.0f, 1.0f, 1.0f, 1.0f)
        );

        VX_STATIC_CHECK_EQ(
            step(vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(0.0f, 1.0f, 2.0f, 0.5f)),
            vec4(0.0f, 1.0f, 1.0f, 0.0f)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_smoothstep)
{
    VX_SECTION("scalar")
    {
        constexpr f32 edge0 = 0.0f;
        constexpr f32 edge1 = 2.0f;

        // In-bounds values
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, 0.0f), 0.0f);
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, 0.5f), 0.15625f);
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, 1.0f), 0.5f);
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, 1.5f), 0.84375f);
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, 2.0f), 1.0f);

        // Out-of-bounds clamping
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, -1.0f), 0.0f);
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, 3.0f), 1.0f);
    }

    VX_SECTION("vec2 scalar edge")
    {
        constexpr f32 edge0 = 0.0f;
        constexpr f32 edge1 = 2.0f;

        // In-bounds values
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec2(0.0f, 0.5f)), vec2(0.0f, 0.15625f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec2(1.0f, 1.5f)), vec2(0.5f, 0.84375f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec2(1.5f, 2.0f)), vec2(0.84375f, 1.0f));

        // Out-of-bounds clamping
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec2(-1.0f, 3.0f)), vec2(0.0f, 1.0f));
    }

    VX_SECTION("vec3 scalar edge")
    {
        constexpr f32 edge0 = 0.0f;
        constexpr f32 edge1 = 2.0f;

        // In-bounds values
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec3(0.0f, 0.5f, 1.0f)), vec3(0.0f, 0.15625f, 0.5f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec3(1.0f, 1.5f, 2.0f)), vec3(0.5f, 0.84375f, 1.0f));

        // Out-of-bounds clamping
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec3(-1.0f, 3.0f, 5.0f)), vec3(0.0f, 1.0f, 1.0f));
    }

    VX_SECTION("vec4 scalar edge")
    {
        constexpr f32 edge0 = 0.0f;
        constexpr f32 edge1 = 2.0f;

        // In-bounds values
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec4(0.0f, 0.5f, 1.0f, 1.5f)), vec4(0.0f, 0.15625f, 0.5f, 0.84375f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec4(0.5f, 1.0f, 1.5f, 2.0f)), vec4(0.15625f, 0.5f, 0.84375f, 1.0f));

        // Out-of-bounds clamping
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec4(-3.0f, -1.0f, 3.0f, 5.0f)), vec4(0.0f, 0.0f, 1.0f, 1.0f));
    }

    VX_SECTION("vec2 vector edge")
    {
        constexpr vec2 edge0 = vec2(0.0f, 1.0f);
        constexpr vec2 edge1 = vec2(2.0f, 3.0f);

        // In-bounds values
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec2(0.0f, 1.0f)), vec2(0.0f, 0.0f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec2(1.0f, 2.0f)), vec2(0.5f, 0.5f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec2(2.0f, 3.0f)), vec2(1.0f, 1.0f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec2(0.5f, 1.5f)), vec2(0.15625f, 0.15625f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec2(1.5f, 2.5f)), vec2(0.84375f, 0.84375f));

        // Out-of-bounds clamping
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec2(-1.0f, 5.0f)), vec2(0.0f, 1.0f));
    }

    VX_SECTION("vec3 vector edge")
    {
        constexpr vec3 edge0 = vec3(0.0f, 1.0f, 2.0f);
        constexpr vec3 edge1 = vec3(2.0f, 3.0f, 4.0f);

        // In-bounds values
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec3(0.0f, 1.0f, 2.0f)), vec3(0.0f, 0.0f, 0.0f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec3(1.0f, 2.0f, 3.0f)), vec3(0.5f, 0.5f, 0.5f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec3(2.0f, 3.0f, 4.0f)), vec3(1.0f, 1.0f, 1.0f));

        // Intermediate values
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec3(0.5f, 1.5f, 2.5f)), vec3(0.15625f, 0.15625f, 0.15625f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec3(1.5f, 2.5f, 3.5f)), vec3(0.84375f, 0.84375f, 0.84375f));

        // Out-of-bounds clamping
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec3(-1.0f, 5.0f, 1.0f)), vec3(0.0f, 1.0f, 0.0f));
    }

    VX_SECTION("vec4 vector edge")
    {
        constexpr vec4 edge0 = vec4(0.0f, 1.0f, 2.0f, 3.0f);
        constexpr vec4 edge1 = vec4(2.0f, 3.0f, 4.0f, 5.0f);

        // In-bounds values
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec4(0.0f, 1.0f, 2.0f, 3.0f)), vec4(0.0f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec4(1.0f, 2.0f, 3.0f, 4.0f)), vec4(0.5f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec4(2.0f, 3.0f, 4.0f, 5.0f)), vec4(1.0f));

        // Intermediate values
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec4(0.5f, 1.5f, 2.5f, 3.5f)), vec4(0.15625f));
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec4(1.5f, 2.5f, 3.5f, 4.5f)), vec4(0.84375f));

        // Out-of-bounds clamping
        VX_STATIC_CHECK_EQ(smoothstep(edge0, edge1, vec4(-1.0f, 5.0f, 1.0f, 10.0f)), vec4(0.0f, 1.0f, 0.0f, 1.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_slerp)
{
    VX_SECTION("vec2")
    {
        // 0 degrees (same vector): should return same vector for any t
        {
            constexpr vec2 a(1.0f, 0.0f);

            VX_CHECK_EQ(slerp(a, a, 0.9f), a);
            VX_CHECK_EQ(slerp(a, a, 0.5f), a);
            VX_CHECK_EQ(slerp(a, a, 1.0f), a);
        }

        // 90 degrees: halfway should be normalized (1,1)
        {
            constexpr vec2 a(1.0f, 0.0f);
            constexpr vec2 b(0.0f, 1.0f);
            const vec2 mid = direction(radians(45.0f));

            VX_CHECK_EQ(slerp(a, b, 0.0f), a);
            VX_CHECK_EQ(slerp(a, b, 0.5f), mid);
            VX_CHECK_EQ(slerp(a, b, 1.0f), b);

            VX_CHECK_EQ(slerp(-a, -b, 0.0f), -a);
            VX_CHECK_EQ(slerp(-a, -b, 0.5f), -mid);
            VX_CHECK_EQ(slerp(-a, -b, 1.0f), -b);
        }

        // 45 degrees: halfway should point 22.5 off
        {
            constexpr vec2 a(1.0f, 0.0f);
            const vec2 b = normalize(vec2(1.0f, 1.0f));
            const vec2 mid = direction(radians(22.5f));

            VX_CHECK_EQ(slerp(a, b, 0.5f), mid);
        }

        // t = 0 and t = 1 always return endpoints
        {
            constexpr vec2 a(0.0f, 1.0f);
            const vec2 b = -direction(radians(45.0f));

            VX_CHECK_EQ(slerp(a, b, 0.0f), a);
            VX_CHECK_EQ(slerp(a, b, 1.0f), b);
        }
    }

    VX_SECTION("vec3")
    {
        // 0 degrees (same vector)
        {
            constexpr vec3 a(1.0f, 0.0f, 0.0f);

            VX_CHECK_EQ(slerp(a, a, 0.9f), a);
            VX_CHECK_EQ(slerp(a, a, 0.5f), a);
            VX_CHECK_EQ(slerp(a, a, 1.0f), a);
        }

        // 90 degrees in XY plane
        {
            constexpr vec3 a(1.0f, 0.0f, 0.0f);
            constexpr vec3 b(0.0f, 1.0f, 0.0f);
            const vec3 mid = normalize(vec3(1.0f, 1.0f, 0.0f));

            VX_CHECK_EQ(slerp(a, b, 0.0f), a);
            VX_CHECK_EQ(slerp(a, b, 0.5f), mid);
            VX_CHECK_EQ(slerp(a, b, 1.0f), b);
        }

        // 90 degrees in XZ plane
        {
            constexpr vec3 a(1.0f, 0.0f, 0.0f);
            constexpr vec3 b(0.0f, 0.0f, 1.0f);
            const vec3 mid = normalize(vec3(1.0f, 0.0f, 1.0f));

            VX_CHECK_EQ(slerp(a, b, 0.5f), mid);
        }

        // t = 0 and t = 1 always return endpoints
        {
            const vec3 a = normalize(vec3(1.0f, 2.0f, 3.0f));
            const vec3 b = normalize(vec3(-1.0f, -1.0f, 0.0f));

            VX_CHECK_EQ(slerp(a, b, 0.0f), a);
            VX_CHECK_EQ(slerp(a, b, 1.0f), b);
        }
    }

    VX_SECTION("vec4")
    {
        // 0 degrees (same vector)
        {
            constexpr vec4 a(1.0f, 0.0f, 0.0f, 0.0f);

            VX_CHECK_EQ(slerp(a, a, 0.5f), a);
        }

        // 90 degrees in 4D between orthogonal axes
        {
            constexpr vec4 a(1.0f, 0.0f, 0.0f, 0.0f);
            constexpr vec4 b(0.0f, 1.0f, 0.0f, 0.0f);
            const vec4 mid = normalize(vec4(1.0f, 1.0f, 0.0f, 0.0f));

            VX_CHECK_EQ(slerp(a, b, 0.5f), mid);
        }

        // t = 0 and t = 1 always return endpoints
        {
            const vec4 a = normalize(vec4(1.0f, -2.0f, 0.5f, 1.5f));
            const vec4 b = normalize(vec4(-1.0f, 0.5f, 0.5f, -2.0f));

            VX_CHECK_EQ(slerp(a, b, 0.0f), a);
            VX_CHECK_EQ(slerp(a, b, 1.0f), b);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}