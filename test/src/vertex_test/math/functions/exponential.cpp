#include "vertex_test/test.hpp"

#include "vertex/math/functions/exponential.hpp"
#include "vertex/math/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_CHECK_CLOSE(a, b, epsilon) VX_CHECK(equal_approx(a, b, epsilon))
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

VX_TEST_CASE(test_exp)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_CLOSE(vx::math::exp(0.0f), 1.0f, 1e-6f);       // e^0 = 1
        VX_CHECK_CLOSE(vx::math::exp(1.0), 2.718281828459, 1e-12); // e^1 = e
        VX_CHECK_CLOSE(vx::math::exp(-1.0f), 1.0f / 2.71828f, 1e-6f); // e^-1 ~= 0.3679
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 1.0f);
        const vec2f b = exp(a);
        VX_CHECK_CLOSE(b, vec2f(1.0f, 2.71828f), 1e-5f);

        const vec2f c(-1.0f, 2.0f);
        const vec2f d = exp(c);
        VX_CHECK_CLOSE(d, vec2f(0.36788f, 7.38906f), 1e-5f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(0.0f, 1.0f, 2.0f);
        const vec3f b = exp(a);
        VX_CHECK_CLOSE(b, vec3f(1.0f, 2.71828f, 7.38906f), 1e-5f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(-1.0f, 0.0f, 1.0f, 2.0f);
        const vec4f b = exp(a);
        VX_CHECK_CLOSE(b, vec4f(0.36788f, 1.0f, 2.71828f, 7.38906f), 1e-5f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_exp2)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_CLOSE(vx::math::exp2(0.0f), 1.0f, 1e-6f);     // 2^0 = 1
        VX_CHECK_CLOSE(vx::math::exp2(1.0f), 2.0f, 1e-6f);     // 2^1 = 2
        VX_CHECK_CLOSE(vx::math::exp2(2.0f), 4.0f, 1e-6f);     // 2^2 = 4
        VX_CHECK_CLOSE(vx::math::exp2(-1.0f), 0.5f, 1e-6f);    // 2^-1 = 0.5
    }

    VX_SECTION("vec2")
    {
        const vec2f a(0.0f, 1.0f);
        const vec2f b = exp2(a);
        VX_CHECK_CLOSE(b, vec2f(1.0f, 2.0f), 1e-6f);

        const vec2f c(2.0f, -1.0f);
        const vec2f d = exp2(c);
        VX_CHECK_CLOSE(d, vec2f(4.0f, 0.5f), 1e-6f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(-2.0f, 0.0f, 3.0f);
        const vec3f b = exp2(a);
        VX_CHECK_CLOSE(b, vec3f(0.25f, 1.0f, 8.0f), 1e-6f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 1.0f, 2.0f, -2.0f);
        const vec4f b = exp2(a);
        VX_CHECK_CLOSE(b, vec4f(1.0f, 2.0f, 4.0f, 0.25f), 1e-6f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_log)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_CLOSE(vx::math::log(1.0f), 0.0f, 1e-6f);               // ln(1) = 0
        VX_CHECK_CLOSE(vx::math::log(constants<f32>::e), 1.0f, 1e-6f);  // ln(e) = 1
        VX_CHECK_CLOSE(vx::math::log(10.0f), 2.30258509299f, 1e-6f);    // ln(10)
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.0f, constants<f32>::e);
        const vec2f b = log(a);
        VX_CHECK_CLOSE(b, vec2f(0.0f, 1.0f), 1e-6f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 10.0f, constants<f32>::e);
        const vec3f b = log(a);
        VX_CHECK_CLOSE(b, vec3f(0.0f, 2.30258509299f, 1.0f), 1e-6f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(1.0f, 2.0f, 4.0f, 8.0f);
        const vec4f b = log(a);
        VX_CHECK_CLOSE(b, vec4f(0.0f, 0.69314718056f, 1.38629436112f, 2.07944154168), 1e-6f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_log2)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_CLOSE(vx::math::log2(1.0f), 0.0f, 1e-6f);  // log2(1) = 0
        VX_CHECK_CLOSE(vx::math::log2(2.0f), 1.0f, 1e-6f);  // log2(2) = 1
        VX_CHECK_CLOSE(vx::math::log2(8.0f), 3.0f, 1e-6f);  // log2(8) = 3
    }

    VX_SECTION("vec2")
    {
        const vec2f a(4.0f, 16.0f);
        const vec2f b = log2(a);
        VX_CHECK_CLOSE(b, vec2f(2.0f, 4.0f), 1e-6f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 2.0f, 32.0f);
        const vec3f b = log2(a);
        VX_CHECK_CLOSE(b, vec3f(0.0f, 1.0f, 5.0f), 1e-6f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(8.0f, 4.0f, 2.0f, 1.0f);
        const vec4f b = log2(a);
        VX_CHECK_CLOSE(b, vec4f(3.0f, 2.0f, 1.0f, 0.0f), 1e-6f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_sqrt)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_CLOSE(vx::math::sqrt(0.0f), 0.0f, 1e-6f);     // sqrt(0) = 0
        VX_CHECK_CLOSE(vx::math::sqrt(1.0f), 1.0f, 1e-6f);     // sqrt(1) = 1
        VX_CHECK_CLOSE(vx::math::sqrt(4.0f), 2.0f, 1e-6f);     // sqrt(4) = 2
        VX_CHECK_CLOSE(vx::math::sqrt(9.0f), 3.0f, 1e-6f);     // sqrt(9) = 3
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.0f, 4.0f);
        const vec2f b = sqrt(a);
        VX_CHECK_CLOSE(b, vec2f(1.0f, 2.0f), 1e-6f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(9.0f, 16.0f, 25.0f);
        const vec3f b = sqrt(a);
        VX_CHECK_CLOSE(b, vec3f(3.0f, 4.0f, 5.0f), 1e-6f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.0f, 1.0f, 2.25f, 6.25f);
        const vec4f b = sqrt(a);
        VX_CHECK_CLOSE(b, vec4f(0.0f, 1.0f, 1.5f, 2.5f), 1e-6f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_inverse)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_CLOSE(vx::math::inverse(1.0f), 1.0f, 1e-6f);       // 1 / 1 = 1
        VX_CHECK_CLOSE(vx::math::inverse(2.0f), 0.5f, 1e-6f);       // 1 / 2 = 0.5
        VX_CHECK_CLOSE(vx::math::inverse(0.5f), 2.0f, 1e-6f);       // 1 / 0.5 = 2
        VX_CHECK_CLOSE(vx::math::inverse(-4.0f), -0.25f, 1e-6f);    // 1 / -4 = -0.25
    }

    VX_SECTION("vec2")
    {
        const vec2f a(2.0f, 4.0f);
        const vec2f b = inverse(a);
        VX_CHECK_CLOSE(b, vec2f(0.5f, 0.25f), 1e-6f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 0.5f, 0.25f);
        const vec3f b = inverse(a);
        VX_CHECK_CLOSE(b, vec3f(1.0f, 2.0f, 4.0f), 1e-6f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(10.0f, -2.0f, 0.2f, -0.5f);
        const vec4f b = inverse(a);
        VX_CHECK_CLOSE(b, vec4f(0.1f, -0.5f, 5.0f, -2.0f), 1e-6f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_inverse_sqrt)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_CLOSE(vx::math::inverse_sqrt(1.0f), 1.0f, 1e-6f);     // 1 / sqrt(1) = 1
        VX_CHECK_CLOSE(vx::math::inverse_sqrt(4.0f), 0.5f, 1e-6f);     // 1 / sqrt(4) = 0.5
        VX_CHECK_CLOSE(vx::math::inverse_sqrt(9.0f), 1.0f / 3.0f, 1e-6f); // 1 / sqrt(9) = 1/3
        VX_CHECK_CLOSE(vx::math::inverse_sqrt(0.25f), 2.0f, 1e-6f);    // 1 / sqrt(0.25) = 2
    }

    VX_SECTION("vec2")
    {
        const vec2f a(4.0f, 16.0f);
        const vec2f b = inverse_sqrt(a);
        VX_CHECK_CLOSE(b, vec2f(0.5f, 0.25f), 1e-6f);
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 0.25f, 0.04f);
        const vec3f b = inverse_sqrt(a);
        VX_CHECK_CLOSE(b, vec3f(1.0f, 2.0f, 5.0f), 1e-6f);
    }

    VX_SECTION("vec4")
    {
        const vec4f a(100.0f, 1.0f, 0.01f, 25.0f);
        const vec4f b = inverse_sqrt(a);
        VX_CHECK_CLOSE(b, vec4f(0.1f, 1.0f, 10.0f, 0.2f), 1e-6f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_next_pow2)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(0), 1);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(1), 1);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(2), 2);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(3), 4);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(5), 8);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(9), 16);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(17), 32);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(33), 64);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(255), 256);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(256), 256);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(300), 512);
        VX_STATIC_CHECK_EQ(vx::math::next_pow2(1023), 1024);
    }

    VX_SECTION("vec2")
    {
        const vec2i a(3, 9);
        const vec2i b = next_pow2(a);
        VX_CHECK_EQ(b, vec2i(4, 16));
    }

    VX_SECTION("vec3")
    {
        const vec3i a(1, 5, 17);
        const vec3i b = next_pow2(a);
        VX_CHECK_EQ(b, vec3i(1, 8, 32));
    }

    VX_SECTION("vec4")
    {
        const vec4i a(0, 2, 33, 255);
        const vec4i b = next_pow2(a);
        VX_CHECK_EQ(b, vec4i(1, 2, 64, 256));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}