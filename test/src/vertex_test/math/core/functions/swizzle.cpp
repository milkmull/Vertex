#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/swizzle.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b, 1e-4f))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b, 1e-4f))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_swizzle)
{
    VX_SECTION("vec2 compile time")
    {
        constexpr vec2 v(1, 2);

        constexpr vec2 v2 = swizzle<1, 0>(v);
        VX_STATIC_CHECK_EQ(v2, vec2(2, 1));

        constexpr vec3 v3 = swizzle<1, 0, 1>(v);
        VX_STATIC_CHECK_EQ(v3, vec3(2, 1, 2));

        constexpr vec4 v4 = swizzle<1, 0, 1, 0>(v);
        VX_STATIC_CHECK_EQ(v4, vec4(2, 1, 2, 1));
    }

    VX_SECTION("vec2 run time")
    {
        constexpr vec2 v(1, 2);
        
        constexpr vec2 v2 = swizzle(v, 1, 0);
        VX_STATIC_CHECK_EQ(v2, vec2(2, 1));

        constexpr vec3 v3 = swizzle(v, 1, 0, 1);
        VX_STATIC_CHECK_EQ(v3, vec3(2, 1, 2));

        constexpr vec4 v4 = swizzle(v, 1, 0, 1, 0);
        VX_STATIC_CHECK_EQ(v4, vec4(2, 1, 2, 1));
    }

    VX_SECTION("vec3 compile time")
    {
        constexpr vec3 v(1, 2, 3);

        constexpr vec2 v2 = swizzle<2, 1>(v);
        VX_STATIC_CHECK_EQ(v2, vec2(3, 2));

        constexpr vec3 v3 = swizzle<2, 1, 0>(v);
        VX_STATIC_CHECK_EQ(v3, vec3(3, 2, 1));

        constexpr vec4 v4 = swizzle<2, 1, 0, 2>(v);
        VX_STATIC_CHECK_EQ(v4, vec4(3, 2, 1, 3));
    }

    VX_SECTION("vec3 run time")
    {
        constexpr vec3 v(1, 2, 3);

        constexpr vec2 v2 = swizzle(v, 2, 1);
        VX_STATIC_CHECK_EQ(v2, vec2(3, 2));

        constexpr vec3 v3 = swizzle(v, 2, 1, 0);
        VX_STATIC_CHECK_EQ(v3, vec3(3, 2, 1));

        constexpr vec4 v4 = swizzle(v, 2, 1, 0, 2);
        VX_STATIC_CHECK_EQ(v4, vec4(3, 2, 1, 3));
    }

    VX_SECTION("vec4 compile time")
    {
        constexpr vec4 v(1, 2, 3, 4);

        constexpr vec2 v2 = swizzle<3, 2>(v);
        VX_STATIC_CHECK_EQ(v2, vec2(4, 3));

        constexpr vec3 v3 = swizzle<3, 2, 1>(v);
        VX_STATIC_CHECK_EQ(v3, vec3(4, 3, 2));

        constexpr vec4 v4 = swizzle<3, 2, 1, 0>(v);
        VX_STATIC_CHECK_EQ(v4, vec4(4, 3, 2, 1));
    }

    VX_SECTION("vec4 run time")
    {
        constexpr vec4 v(1, 2, 3, 4);

        constexpr vec2 v2 = swizzle(v, 3, 2);
        VX_STATIC_CHECK_EQ(v2, vec2(4, 3));

        constexpr vec3 v3 = swizzle(v, 3, 2, 1);
        VX_STATIC_CHECK_EQ(v3, vec3(4, 3, 2));

        constexpr vec4 v4 = swizzle(v, 3, 2, 1, 0);
        VX_STATIC_CHECK_EQ(v4, vec4(4, 3, 2, 1));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}