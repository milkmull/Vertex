#include "vertex_test/test.hpp"

#include "vertex/math/color/functions/comparison.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_inf)
{
    VX_SECTION("vec4")
    {
        const color a{ 1.0f, constants<f32>::infinity, 0.0f, 4.0f };
        const color b{ -constants<f32>::infinity, 2.0f, constants<f32>::nan, 3.0f };
        const color c{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_CHECK(is_inf(a));
        VX_CHECK(is_inf(b));
        VX_CHECK(!is_inf(c));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_nan)
{
    VX_SECTION("vec4")
    {
        const color a{ 1.0f, constants<f32>::nan, 0.0f, 4.0f };
        const color b{ -constants<f32>::infinity, 2.0f, constants<f32>::nan, 3.0f };
        const color c{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_CHECK(is_nan(a));
        VX_CHECK(is_nan(b));
        VX_CHECK(!is_nan(c));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_zero_approx)
{
    VX_SECTION("vec4")
    {
        const color a{ 0.0f, 0.0f, 0.0f, 0.0f };                    // Exactly zero
        const color b{ 0.0000001f, 0.0f, 0.0f, 0.0f };              // Approx zero
        const color c{ 1.0f, 0.0f, 0.0f, 0.0f };                    // Not all zero approx
        const color d{ 0.0000009f, 0.0000009f, 0.0000009f, 0.0f };  // Approx zero for all

        VX_CHECK(is_zero_approx(a));
        VX_CHECK(is_zero_approx(b));
        VX_CHECK(!is_zero_approx(c));
        VX_CHECK(is_zero_approx(d));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_equal_approx)
{
    VX_SECTION("vec4")
    {
        const color a{ 5.0f, 6.0f, 7.0f, 8.0f };
        const color b{ 5.0f, 6.0f, 7.0f, 8.0f };
        const color c{ 5.0000001f, 6.0f, 7.0000001f, 8.0f };
        const color d{ 5.0f, 6.0f, 7.0f, 9.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(equal_approx(a, c));
        VX_CHECK(!equal_approx(a, d));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_not_equal_approx)
{
    VX_SECTION("vec4")
    {
        const color a{ 5.0f, 6.0f, 7.0f, 8.0f };
        const color b{ 5.0f, 6.0f, 7.0f, 8.0f };
        const color c{ 5.0000001f, 6.0f, 7.0000001f, 8.0f };
        const color d{ 5.0f, 6.0f, 7.0f, 9.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(!not_equal_approx(a, c));
        VX_CHECK(not_equal_approx(a, d));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}