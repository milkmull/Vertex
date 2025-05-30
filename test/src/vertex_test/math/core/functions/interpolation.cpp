#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/interpolation.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_pow)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::pow(2.0f, 3.0f), 8.0f);   // 2^3 = 8
        VX_CHECK_EQ(vx::math::pow(4.0, 0.5), 2.0);      // sqrt(4) = 2
        VX_CHECK_EQ(vx::math::pow(5.0f, 0.0f), 1.0f);   // x^0 = 1
        VX_CHECK_EQ(vx::math::pow(9.0, -0.5), 1.0 / 3.0); // 9^-0.5 = 1/sqrt(9) = 1/3
    }

    VX_SECTION("vec2 (scalar)")
    {
        const vec2f a(2.0f, 3.0f);
        const vec2f b = pow(a, 2.0f);
        VX_CHECK_EQ(b, vec2f(4.0f, 9.0f));

        const vec2f c(16.0f, 25.0f);
        const vec2f d = pow(c, 0.5f);
        VX_CHECK_EQ(d, vec2f(4.0f, 5.0f));
    }

    VX_SECTION("vec3 (scalar)")
    {
        const vec3f a(1.0f, 2.0f, 4.0f);
        const vec3f b = pow(a, 3.0f);
        VX_CHECK_EQ(b, vec3f(1.0f, 8.0f, 64.0f));

        const vec3f c(9.0f, 27.0f, 81.0f);
        const vec3f d = pow(c, 1.0f / 3.0f);
        VX_CHECK_CLOSE(d, vec3f(2.0801f, 3.0f, 4.3267f), 1e-3f); // cube roots approx
    }

    VX_SECTION("vec4 (scalar)")
    {
        const vec4f a(2.0f, 3.0f, 4.0f, 5.0f);
        const vec4f b = pow(a, 2.0f);
        VX_CHECK_EQ(b, vec4f(4.0f, 9.0f, 16.0f, 25.0f));

        const vec4f c(16.0f, 81.0f, 625.0f, 0.25f);
        const vec4f d = pow(c, 0.5f);
        VX_CHECK_EQ(d, vec4f(4.0f, 9.0f, 25.0f, 0.5f));
    }

    VX_SECTION("vec2 (binary)")
    {
        const vec2f a(2.0f, 4.0f);
        const vec2f b(3.0f, 0.5f);
        const vec2f c = pow(a, b);
        VX_CHECK_EQ(c, vec2f(8.0f, 2.0f));
    }

    VX_SECTION("vec3 (binary)")
    {
        const vec3f a(3.0f, 9.0f, 27.0f);
        const vec3f b(2.0f, 0.5f, 1.0f / 3.0f);
        const vec3f c = pow(a, b);
        VX_CHECK_EQ(c, vec3f(9.0f, 3.0f, 3.0f));
    }

    VX_SECTION("vec4 (binary)")
    {
        const vec4f a(1.0f, 2.0f, 3.0f, 4.0f);
        const vec4f b(0.0f, 1.0f, 2.0f, 3.0f);
        const vec4f c = pow(a, b);
        VX_CHECK_EQ(c, vec4f(1.0f, 2.0f, 9.0f, 64.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}