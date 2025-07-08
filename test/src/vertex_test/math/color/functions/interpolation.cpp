#include "vertex_test/test.hpp"

#include "vertex/math/color/functions/interpolation.hpp"
#include "vertex/math/color/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_lerp)
{
    VX_SECTION("color scalar t")
    {
        const color min(2.0f, -10.0f, 5.0f, 0.5f);
        const color max(10.0f, -2.0f, 13.0f, 8.5f);
        const color range = max - min;

        VX_CHECK_EQ(lerp(min, max, 0.0f), min);
        VX_CHECK_EQ(lerp(min, max, 1.0f), max);
        VX_CHECK_EQ(lerp(min, max, 0.5f), min + (range * 0.5f));
        VX_CHECK_EQ(lerp(min, max, 0.25f), min + (range * 0.25f));
        VX_CHECK_EQ(lerp(min, max, 0.75f), min + (range * 0.75f));

        // same x and y -> result should be constant
        VX_CHECK_EQ(lerp(color(5.0f), color(5.0f), 0.3f), color(5.0f));

        // t < 0 and t > 1 -> extrapolation
        VX_CHECK_EQ(lerp(min, max, -1.0f), color(-6.0f, -18.0f, -3.0f, -7.5f));
        VX_CHECK_EQ(lerp(min, max, 2.0f), color(18.0f, 6.0f, 21.0f, 16.5f));

        // clamped
        VX_CHECK(lerp_clamped(min, max, -1.0f) == min);
        VX_CHECK(lerp_clamped(min, max, 0.5f) == min + (range * 0.5f));
        VX_CHECK(lerp_clamped(min, max, 2.0f) == max);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_step)
{
    VX_SECTION("color scalar edge")
    {
        VX_STATIC_CHECK_EQ(
            step(0.0f, color(-1.0f, 0.0f, 1.0f, 2.0f)),
            color(0.0f, 1.0f, 1.0f, 1.0f)
        );

        VX_STATIC_CHECK_EQ(
            step(2.0f, color(1.0f, 2.0f, 3.0f, 4.0f)),
            color(0.0f, 1.0f, 1.0f, 1.0f)
        );

        VX_STATIC_CHECK_EQ(
            step(1.0f, color(0.0f, 1.0f, 2.0f, 0.5f)),
            color(0.0f, 1.0f, 1.0f, 0.0f)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_smoothstep)
{
    VX_SECTION("color scalar edge")
    {
        constexpr f32 edge0 = 0.0f;
        constexpr f32 edge1 = 2.0f;

        // In-bounds values
        VX_CHECK_EQ(smoothstep(edge0, edge1, color(0.0f, 0.5f, 1.0f, 1.5f)), color(0.0f, 0.15625f, 0.5f, 0.84375f));
        VX_CHECK_EQ(smoothstep(edge0, edge1, color(0.5f, 1.0f, 1.5f, 2.0f)), color(0.15625f, 0.5f, 0.84375f, 1.0f));

        // Out-of-bounds clamping
        VX_CHECK_EQ(smoothstep(edge0, edge1, color(-3.0f, -1.0f, 3.0f, 5.0f)), color(0.0f, 0.0f, 1.0f, 1.0f));
    }
}

/////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}