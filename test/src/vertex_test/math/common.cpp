#include "vertex_test/test.hpp"

#include "vertex/math/fn/common.hpp"
#include "vertex/math/fn/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(all_equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(all_equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_min)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK_EQ(min(-5, -3), -5);
        VX_STATIC_CHECK_EQ(min(-2.5f, 7.1f), -2.5f);
    }

    VX_SECTION("scalar multiple args")
    {
        VX_STATIC_CHECK_EQ(min(-9, -4, -7, -2), -9);
        VX_STATIC_CHECK_EQ(min(-3.14, 1.61, -2.72), -3.14);
    }

    VX_SECTION("vec2")
    {
        constexpr vec2i a(-5, 2);
        constexpr vec2i b(3, -4);

        constexpr vec2i r1 = min(a, b);
        VX_STATIC_CHECK_EQ(r1, vec2i(-5, -4));

        constexpr vec2i r2 = min(a, -3);
        VX_STATIC_CHECK_EQ(r2, vec2i(-5, -3));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a(-2.5f, 7.0f, -1.5f);
        constexpr vec3f b(3.0f, -4.0f, 5.0f);

        constexpr vec3f r1 = min(a, b);
        VX_STATIC_CHECK_EQ(r1, vec3f(-2.5f, -4.0f, -1.5f));

        constexpr vec3f r2 = min(a, 0.0f);
        VX_STATIC_CHECK_EQ(r2, vec3f(-2.5f, 0.0f, -1.5f));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(-9, 4, -6, 2);
        constexpr vec4i b(3, -5, 7, -8);

        constexpr vec4i r1 = min(a, b);
        VX_STATIC_CHECK_EQ(r1, vec4i(-9, -5, -6, -8));

        constexpr vec4i r2 = min(a, -6);
        VX_STATIC_CHECK_EQ(r2, vec4i(-9, -6, -6, -6));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_max)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK_EQ(max(-5, -3), -3);
        VX_STATIC_CHECK_EQ(max(-2.5f, 7.1f), 7.1f);
    }

    VX_SECTION("scalar multiple args")
    {
        VX_STATIC_CHECK_EQ(max(-9, -4, -7, -2), -2);
        VX_STATIC_CHECK_EQ(max(-3.14, 1.61, -2.72), 1.61);
    }

    VX_SECTION("vec2")
    {
        constexpr vec2i a(-5, 2);
        constexpr vec2i b(3, -4);

        constexpr vec2i r1 = max(a, b);
        VX_STATIC_CHECK_EQ(r1, vec2i(3, 2));

        constexpr vec2i r2 = max(a, -3);
        VX_STATIC_CHECK_EQ(r2, vec2i(-3, 2));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a(-2.5f, 7.0f, -1.5f);
        constexpr vec3f b(3.0f, -4.0f, 5.0f);

        constexpr vec3f r1 = max(a, b);
        VX_STATIC_CHECK_EQ(r1, vec3f(3.0f, 7.0f, 5.0f));

        constexpr vec3f r2 = max(a, 0.0f);
        VX_STATIC_CHECK_EQ(r2, vec3f(0.0f, 7.0f, 0.0f));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(-9, 4, -6, 2);
        constexpr vec4i b(3, -5, 7, -8);

        constexpr vec4i r1 = max(a, b);
        VX_STATIC_CHECK_EQ(r1, vec4i(3, 4, 7, 2));

        constexpr vec4i r2 = max(a, -6);
        VX_STATIC_CHECK_EQ(r2, vec4i(-6, 4, -6, 2));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cmin)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a(-5, 2);
        constexpr i32 r1 = cmin(a);
        VX_STATIC_CHECK_EQ(r1, -5);

        constexpr vec2f b(-3.5f, 4.1f);
        constexpr f32 r2 = cmin(b);
        VX_STATIC_CHECK_EQ(r2, -3.5f);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3i a(-8, 3, -6);
        constexpr i32 r1 = cmin(a);
        VX_STATIC_CHECK_EQ(r1, -8);

        constexpr vec3f b(7.0f, -2.5f, 9.1f);
        constexpr f32 r2 = cmin(b);
        VX_STATIC_CHECK_EQ(r2, -2.5f);

        constexpr vec3d c(-3.14, 1.618, -2.718);
        constexpr f64 r3 = cmin(c);
        VX_STATIC_CHECK_EQ(r3, -3.14);
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(9, -4, 6, -2);
        constexpr i32 r1 = cmin(a);
        VX_STATIC_CHECK_EQ(r1, -4);

        constexpr vec4f b(-5.5f, 3.3f, -8.8f, 4.4f);
        constexpr f32 r2 = cmin(b);
        VX_STATIC_CHECK_EQ(r2, -8.8f);

        constexpr vec4d c(-1.0, 0.5, -2.0, 3.0);
        constexpr f64 r3 = cmin(c);
        VX_STATIC_CHECK_EQ(r3, -2.0);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cmax)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a(-5, 2);
        constexpr i32 r1 = cmax(a);
        VX_STATIC_CHECK_EQ(r1, 2);

        constexpr vec2f b(-3.5f, 4.1f);
        constexpr f32 r2 = cmax(b);
        VX_STATIC_CHECK_EQ(r2, 4.1f);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3i a(-8, 3, -6);
        constexpr i32 r1 = cmax(a);
        VX_STATIC_CHECK_EQ(r1, 3);

        constexpr vec3f b(7.0f, -2.5f, 9.1f);
        constexpr f32 r2 = cmax(b);
        VX_STATIC_CHECK_EQ(r2, 9.1f);

        constexpr vec3d c(-3.14, 1.618, -2.718);
        constexpr f64 r3 = cmax(c);
        VX_STATIC_CHECK_EQ(r3, 1.618);
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(9, -4, 6, -2);
        constexpr i32 r1 = cmax(a);
        VX_STATIC_CHECK_EQ(r1, 9);

        constexpr vec4f b(-5.5f, 3.3f, -8.8f, 4.4f);
        constexpr f32 r2 = cmax(b);
        VX_STATIC_CHECK_EQ(r2, 4.4f);

        constexpr vec4d c(-1.0, 0.5, -2.0, 3.0);
        constexpr f64 r3 = cmax(c);
        VX_STATIC_CHECK_EQ(r3, 3.0);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_csum)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a(-5, 2);
        constexpr i32 r1 = csum(a);
        VX_STATIC_CHECK_EQ(r1, -3);

        constexpr vec2f b(-3.5f, 4.1f);
        constexpr f32 r2 = csum(b);
        VX_STATIC_CHECK_EQ(r2, 0.6f);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3i a(-8, 3, -6);
        constexpr i32 r1 = csum(a);
        VX_STATIC_CHECK_EQ(r1, -11);

        constexpr vec3f b(7.0f, -2.5f, 9.1f);
        constexpr f32 r2 = csum(b);
        VX_STATIC_CHECK_EQ(r2, 13.6f);

        constexpr vec3d c(-3.14, 1.618, -2.718);
        constexpr f64 r3 = csum(c);
        VX_STATIC_CHECK_EQ(r3, -4.24);
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(9, -4, 6, -2);
        constexpr i32 r1 = csum(a);
        VX_STATIC_CHECK_EQ(r1, 9);

        constexpr vec4f b(-5.5f, 3.3f, -8.8f, 4.4f);
        constexpr f32 r2 = csum(b);
        VX_STATIC_CHECK_EQ(r2, -6.6f);

        constexpr vec4d c(-1.0, 0.5, -2.0, 3.0);
        constexpr f64 r3 = csum(c);
        VX_STATIC_CHECK_EQ(r3, 0.5);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_abs)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::abs(-5), 5);
        VX_CHECK_EQ(vx::math::abs(5), 5);

        VX_CHECK_EQ(vx::math::abs(5u), 5u);

        VX_CHECK_EQ(vx::math::abs(-3.14f), 3.14f);
        VX_CHECK_EQ(vx::math::abs(3.14f), 3.14f);

        VX_CHECK_EQ(vx::math::abs(-2.718), 2.718);
        VX_CHECK_EQ(vx::math::abs(2.718), 2.718);
    }

    VX_SECTION("vec2")
    {
        const vec2i a(-1, 2);
        const vec2i b = abs(a);
        VX_CHECK_EQ(b, vec2i(1, 2));

        const vec2f c(-3.0f, -4.5f);
        const vec2f d = abs(c);
        VX_CHECK_EQ(d, vec2f(3.0f, 4.5f));
    }

    VX_SECTION("vec3")
    {
        const vec3i a(-1, 0, 3);
        const vec3i b = abs(a);
        VX_CHECK_EQ(b, vec3i(1, 0, 3));

        const vec3f c(-1.5f, 2.0f, -3.5f);
        const vec3f d = abs(c);
        VX_CHECK_EQ(d, vec3f(1.5f, 2.0f, 3.5f));
    }

    VX_SECTION("vec4")
    {
        const vec4i a(-1, -2, -3, -4);
        const vec4i b = abs(a);
        VX_CHECK_EQ(b, vec4i(1, 2, 3, 4));

        const vec4d c(-1.0, 2.0, -3.0, 4.0);
        const vec4d d = abs(c);
        VX_CHECK_EQ(d, vec4d(1.0, 2.0, 3.0, 4.0));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_round)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::round(3.14f), 3.0f);
        VX_CHECK_EQ(vx::math::round(2.718), 3.0);
        VX_CHECK_EQ(vx::math::round(-4.6f), -5.0f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.1f, 2.5f);
        const vec2f b = round(a);
        VX_CHECK_EQ(b, vec2f(1.0f, 3.0f));

        const vec2f c(-3.7f, 4.3f);
        const vec2f d = round(c);
        VX_CHECK_EQ(d, vec2f(-4.0f, 4.0f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(3.14f, 2.718f, 1.618f);
        const vec3f b = round(a);
        VX_CHECK_EQ(b, vec3f(3.0f, 3.0f, 2.0f));

        const vec3f c(4.4f, -5.5f, 6.6f);
        const vec3f d = round(c);
        VX_CHECK_EQ(d, vec3f(4.0f, -6.0f, 7.0f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(2.3f, -2.7f, 5.5f, 9.1f);
        const vec4f b = round(a);
        VX_CHECK_EQ(b, vec4f(2.0f, -3.0f, 6.0f, 9.0f));

        const vec4f c(-3.8f, 1.1f, -2.5f, 4.9f);
        const vec4f d = round(c);
        VX_CHECK_EQ(d, vec4f(-4.0f, 1.0f, -3.0f, 5.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_trunc)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::trunc(3.14f), 3.0f);
        VX_CHECK_EQ(vx::math::trunc(2.718), 2.0);
        VX_CHECK_EQ(vx::math::trunc(-4.6f), -4.0f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.1f, 2.5f);
        const vec2f b = trunc(a);
        VX_CHECK_EQ(b, vec2f(1.0f, 2.0f));

        const vec2f c(-3.7f, 4.3f);
        const vec2f d = trunc(c);
        VX_CHECK_EQ(d, vec2f(-3.0f, 4.0f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(3.14f, 2.718f, 1.618f);
        const vec3f b = trunc(a);
        VX_CHECK_EQ(b, vec3f(3.0f, 2.0f, 1.0f));

        const vec3f c(4.4f, -5.5f, 6.6f);
        const vec3f d = trunc(c);
        VX_CHECK_EQ(d, vec3f(4.0f, -5.0f, 6.0f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(2.3f, -2.7f, 5.5f, 9.1f);
        const vec4f b = trunc(a);
        VX_CHECK_EQ(b, vec4f(2.0f, -2.0f, 5.0f, 9.0f));

        const vec4f c(-3.8f, 1.1f, -2.5f, 4.9f);
        const vec4f d = trunc(c);
        VX_CHECK_EQ(d, vec4f(-3.0f, 1.0f, -2.0f, 4.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_fract)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::fract(3.14f), 0.14f);
        VX_CHECK_EQ(vx::math::fract(2.718), 0.718);
        VX_CHECK_EQ(vx::math::fract(-4.6f), 0.4f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.1f, 2.5f);
        const vec2f b = fract(a);
        VX_CHECK_EQ(b, vec2f(0.1f, 0.5f));

        const vec2f c(-3.7f, 4.3f);
        const vec2f d = fract(c);
        VX_CHECK_EQ(d, vec2f(0.3f, 0.3f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(3.14f, 2.718f, 1.618f);
        const vec3f b = fract(a);
        VX_CHECK_EQ(b, vec3f(0.14f, 0.718f, 0.618f));

        const vec3f c(4.4f, -5.5f, 6.6f);
        const vec3f d = fract(c);
        VX_CHECK_EQ(d, vec3f(0.4f, 0.5f, 0.6f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(2.3f, -2.7f, 5.5f, 9.1f);
        const vec4f b = fract(a);
        VX_CHECK_EQ(b, vec4f(0.3f, 0.3f, 0.5f, 0.1f));

        const vec4f c(-3.8f, 1.1f, -2.5f, 4.9f);
        const vec4f d = fract(c);
        VX_CHECK_EQ(d, vec4f(0.2f, 0.1f, 0.5f, 0.9f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_floor)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::floor(3.14f), 3.0f);
        VX_CHECK_EQ(vx::math::floor(2.718), 2.0);
        VX_CHECK_EQ(vx::math::floor(-4.6f), -5.0f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.1f, 2.5f);
        const vec2f b = floor(a);
        VX_CHECK_EQ(b, vec2f(1.0f, 2.0f));

        const vec2f c(-3.7f, 4.3f);
        const vec2f d = floor(c);
        VX_CHECK_EQ(d, vec2f(-4.0f, 4.0f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(3.14f, 2.718f, 1.618f);
        const vec3f b = floor(a);
        VX_CHECK_EQ(b, vec3f(3.0f, 2.0f, 1.0f));

        const vec3f c(4.4f, -5.5f, 6.6f);
        const vec3f d = floor(c);
        VX_CHECK_EQ(d, vec3f(4.0f, -6.0f, 6.0f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(2.3f, -2.7f, 5.5f, 9.1f);
        const vec4f b = floor(a);
        VX_CHECK_EQ(b, vec4f(2.0f, -3.0f, 5.0f, 9.0f));

        const vec4f c(-3.8f, 1.1f, -2.5f, 4.9f);
        const vec4f d = floor(c);
        VX_CHECK_EQ(d, vec4f(-4.0f, 1.0f, -3.0f, 4.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_ceil)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::ceil(3.14f), 4.0f);
        VX_CHECK_EQ(vx::math::ceil(2.718), 3.0);
        VX_CHECK_EQ(vx::math::ceil(-4.6f), -4.0f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.1f, 2.5f);
        const vec2f b = ceil(a);
        VX_CHECK_EQ(b, vec2f(2.0f, 3.0f));

        const vec2f c(-3.7f, 4.3f);
        const vec2f d = ceil(c);
        VX_CHECK_EQ(d, vec2f(-3.0f, 5.0f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(3.14f, 2.718f, 1.618f);
        const vec3f b = ceil(a);
        VX_CHECK_EQ(b, vec3f(4.0f, 3.0f, 2.0f));

        const vec3f c(4.4f, -5.5f, 6.6f);
        const vec3f d = ceil(c);
        VX_CHECK_EQ(d, vec3f(5.0f, -5.0f, 7.0f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(2.3f, -2.7f, 5.5f, 9.1f);
        const vec4f b = ceil(a);
        VX_CHECK_EQ(b, vec4f(3.0f, -2.0f, 6.0f, 10.0f));

        const vec4f c(-3.8f, 1.1f, -2.5f, 4.9f);
        const vec4f d = ceil(c);
        VX_CHECK_EQ(d, vec4f(-3.0f, 2.0f, -2.0f, 5.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_sign)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK_EQ(sign(10.0f), 1.0f);
        VX_STATIC_CHECK_EQ(sign(-5.0f), -1.0f);
        VX_STATIC_CHECK_EQ(sign(0.0f), 0.0f);

        VX_STATIC_CHECK_EQ(sign(42), 1);
        VX_STATIC_CHECK_EQ(sign(-42), -1);
        VX_STATIC_CHECK_EQ(sign(0), 0);
    }

    VX_SECTION("vec2")
    {
        constexpr vec2f a(5.0f, -2.0f);
        constexpr vec2f b = sign(a);
        VX_STATIC_CHECK_EQ(b, vec2f(1.0f, -1.0f));

        constexpr vec2f c(0.0f, -0.0f);
        constexpr vec2f d = sign(c);
        VX_STATIC_CHECK_EQ(d, vec2f(0.0f, 0.0f));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a(1.5f, 0.0f, -3.3f);
        constexpr vec3f b = sign(a);
        VX_STATIC_CHECK_EQ(b, vec3f(1.0f, 0.0f, -1.0f));

        constexpr vec3f c(-0.0f, 2.2f, -2.2f);
        constexpr vec3f d = sign(c);
        VX_STATIC_CHECK_EQ(d, vec3f(0.0f, 1.0f, -1.0f));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a(7.1f, -7.1f, 0.0f, 3.0f);
        constexpr vec4f b = sign(a);
        VX_STATIC_CHECK_EQ(b, vec4f(1.0f, -1.0f, 0.0f, 1.0f));

        constexpr vec4f c(-1.0f, -0.0f, 0.0f, 1.0f);
        constexpr vec4f d = sign(c);
        VX_STATIC_CHECK_EQ(d, vec4f(-1.0f, 0.0f, 0.0f, 1.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_fmod)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::fmod(5.3f, 2.0f), 1.3f);
        VX_CHECK_EQ(vx::math::fmod(-5.3, 2.0), -1.3);
        VX_CHECK_EQ(vx::math::fmod(5.3f, -2.0f), 1.3f);
        VX_CHECK_EQ(vx::math::fmod(-5.3f, -2.0f), -1.3f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(5.3f, -5.3f);
        const vec2f b = fmod(a, 2.0f);
        VX_CHECK_EQ(b, vec2f(1.3f, -1.3f));

        const vec2f c(5.3f, -5.3f);
        const vec2f d = fmod(c, vec2f(-2.0f, -2.0f));
        VX_CHECK_EQ(d, vec2f(1.3f, -1.3f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(5.3f, -5.3f, 7.7f);
        const vec3f b = fmod(a, 2.0f);
        VX_CHECK_EQ(b, vec3f(1.3f, -1.3f, 1.7f));

        const vec3f c(5.3f, -5.3f, 7.7f);
        const vec3f d = fmod(c, vec3f(-2.0f, -2.0f, -2.0f));
        VX_CHECK_EQ(d, vec3f(1.3f, -1.3f, 1.7f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(5.3f, -5.3f, 7.7f, -7.7f);
        const vec4f b = fmod(a, 2.0f);
        VX_CHECK_EQ(b, vec4f(1.3f, -1.3f, 1.7f, -1.7f));

        const vec4f c(5.3f, -5.3f, 7.7f, -7.7f);
        const vec4f d = fmod(c, vec4f(-2.0f, -2.0f, -2.0f, -2.0f));
        VX_CHECK_EQ(d, vec4f(1.3f, -1.3f, 1.7f, -1.7f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mod)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::mod(5.3f, 2.0f), 1.3f);
        VX_CHECK_EQ(vx::math::mod(-5.3, 2.0), 0.7);
        VX_CHECK_EQ(vx::math::mod(5.3f, -2.0f), -0.7f);
        VX_CHECK_EQ(vx::math::mod(-5.3f, -2.0f), -1.3f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(5.3f, -5.3f);
        const vec2f b = mod(a, 2.0f);
        VX_CHECK_EQ(b, vec2f(1.3f, 0.7f));

        const vec2f c(5.3f, -5.3f);
        const vec2f d = mod(c, vec2f(-2.0f, -2.0f));
        VX_CHECK_EQ(d, vec2f(-0.7f, -1.3f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(5.3f, -5.3f, 7.7f);
        const vec3f b = mod(a, 2.0f);
        VX_CHECK_EQ(b, vec3f(1.3f, 0.7f, 1.7f));

        const vec3f c(5.3f, -5.3f, 7.7f);
        const vec3f d = mod(c, vec3f(-2.0f, -2.0f, -2.0f));
        VX_CHECK_EQ(d, vec3f(-0.7f, -1.3f, -0.3f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(5.3f, -5.3f, 7.7f, -7.7f);
        const vec4f b = mod(a, 2.0f);
        VX_CHECK_EQ(b, vec4f(1.3f, 0.7f, 1.7f, 0.3f));

        const vec4f c(5.3f, -5.3f, 7.7f, -7.7f);
        const vec4f d = mod(c, vec4f(-2.0f, -2.0f, -2.0f, -2.0f));
        VX_CHECK_EQ(d, vec4f(-0.7f, -1.3f, -0.3f, -1.7f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_modf)
{
    VX_SECTION("scalar")
    {
        f32 intpart1;
        VX_CHECK_EQ(vx::math::modf(3.14f, intpart1), 0.14f);
        VX_CHECK_EQ(intpart1, 3.0f);

        f64 intpart2;
        VX_CHECK_EQ(vx::math::modf(2.718, intpart2), 0.718);
        VX_CHECK_EQ(intpart2, 2.0);

        f32 intpart3;
        VX_CHECK_EQ(vx::math::modf(-4.6f, intpart3), -0.6f);
        VX_CHECK_EQ(intpart3, -4.0f);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(1.1f, 2.5f);
        vec2f intpart_a;
        const vec2f frac_a = modf(a, intpart_a);
        VX_CHECK_EQ(frac_a, vec2f(0.1f, 0.5f));
        VX_CHECK_EQ(intpart_a, vec2f(1.0f, 2.0f));

        const vec2f b(-3.7f, 4.3f);
        vec2f intpart_b;
        const vec2f frac_b = modf(b, intpart_b);
        VX_CHECK_EQ(frac_b, vec2f(-0.7f, 0.3f));
        VX_CHECK_EQ(intpart_b, vec2f(-3.0f, 4.0f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(3.14f, 2.718f, 1.618f);
        vec3f intpart_a;
        const vec3f frac_a = modf(a, intpart_a);
        VX_CHECK_EQ(frac_a, vec3f(0.14f, 0.718f, 0.618f));
        VX_CHECK_EQ(intpart_a, vec3f(3.0f, 2.0f, 1.0f));

        const vec3f b(4.4f, -5.5f, 6.6f);
        vec3f intpart_b;
        const vec3f frac_b = modf(b, intpart_b);
        VX_CHECK_EQ(frac_b, vec3f(0.4f, -0.5f, 0.6f));
        VX_CHECK_EQ(intpart_b, vec3f(4.0f, -5.0f, 6.0f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(2.3f, -2.7f, 5.5f, 9.1f);
        vec4f intpart_a;
        const vec4f frac_a = modf(a, intpart_a);
        VX_CHECK_EQ(frac_a, vec4f(0.3f, -0.7f, 0.5f, 0.1f));
        VX_CHECK_EQ(intpart_a, vec4f(2.0f, -2.0f, 5.0f, 9.0f));

        const vec4f b(-3.8f, 1.1f, -2.5f, 4.9f);
        vec4f intpart_b;
        const vec4f frac_b = modf(b, intpart_b);
        VX_CHECK_EQ(frac_b, vec4f(-0.8f, 0.1f, -0.5f, 0.9f));
        VX_CHECK_EQ(intpart_b, vec4f(-3.0f, 1.0f, -2.0f, 4.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_frexp)
{
    VX_SECTION("scalar")
    {
        i32 exp1;
        const f32 m1 = vx::math::frexp(8.0f, exp1);
        VX_CHECK_EQ(m1, 0.5f);
        VX_CHECK_EQ(exp1, 4);

        i32 exp2;
        const f64 m2 = vx::math::frexp(18.0, exp2);
        VX_CHECK_EQ(m2, 0.5625);
        VX_CHECK_EQ(exp2, 5);

        i32 exp3;
        const f32 m3 = vx::math::frexp(-0.75f, exp3);
        VX_CHECK_EQ(m3, -0.75f);
        VX_CHECK_EQ(exp3, 0);
    }

    VX_SECTION("vec2")
    {
        const vec2f a(6.0f, -12.0f);
        vec2i exp;
        const vec2f mantissa = frexp(a, exp);
        VX_CHECK_EQ(mantissa, vec2f(0.75f, -0.75f));
        VX_CHECK_EQ(exp, vec2i(3, 4));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 0.5f, 32.0f);
        vec3i exp;
        const vec3f mantissa = frexp(a, exp);
        VX_CHECK_EQ(mantissa, vec3f(0.5f, 0.5f, 0.5f));
        VX_CHECK_EQ(exp, vec3i(1, 0, 6));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(4.0f, -8.0f, 10.0f, -0.5f);
        vec4i exp;
        const vec4f mantissa = frexp(a, exp);
        VX_CHECK_EQ(mantissa, vec4f(0.5f, -0.5f, 0.625f, -0.5f));
        VX_CHECK_EQ(exp, vec4i(3, 4, 4, 0));
    }
}
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_ldexp)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::ldexp(0.5f, 4), 8.0f);
        VX_CHECK_EQ(vx::math::ldexp(0.5625, 5), 18.0);
        VX_CHECK_EQ(vx::math::ldexp(-0.75f, 0), -0.75f);
    }

    VX_SECTION("vec2")
    {
        const vec2f m(0.75f, -0.75f);
        const vec2i e(3, 4);
        const vec2f r = ldexp(m, e);
        VX_CHECK_EQ(r, vec2f(6.0f, -12.0f));
    }

    VX_SECTION("vec3")
    {
        const vec3f m(0.5f, 0.5f, 0.5f);
        const vec3i e(1, 0, 6);
        const vec3f r = ldexp(m, e);
        VX_CHECK_EQ(r, vec3f(1.0f, 0.5f, 32.0f));
    }

    VX_SECTION("vec4")
    {
        const vec4f m(0.5f, -0.5f, 0.625f, -0.5f);
        const vec4i e(3, 4, 4, 0);
        const vec4f r = ldexp(m, e);
        VX_CHECK_EQ(r, vec4f(4.0f, -8.0f, 10.0f, -0.5));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_fma)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::fma(2.0f, 3.0f, 1.0f), 7.0f);   // 2 * 3 + 1 = 7
        VX_CHECK_EQ(vx::math::fma(-4.0, 0.5, 2.0), 0.0);       // -4 * 0.5 + 2 = 0
        VX_CHECK_EQ(vx::math::fma(1.5f, -2.0f, -1.0f), -4.0f); // 1.5 * -2 + -1 = -4
    }

    VX_SECTION("vec2")
    {
        const vec2f a(2.0f, -1.0f);
        const vec2f b(3.0f, 4.0f);
        const vec2f c(1.0f, -2.0f);
        const vec2f r = fma(a, b, c); // (2*3 + 1), (-1*4 + -2)
        VX_CHECK_EQ(r, vec2f(7.0f, -6.0f));
    }

    VX_SECTION("vec3")
    {
        const vec3f a(1.0f, 2.0f, -1.5f);
        const vec3f b(2.0f, -0.5f, 4.0f);
        const vec3f c(0.0f, 1.0f, -2.0f);
        const vec3f r = fma(a, b, c); // (1*2+0), (2*-0.5+1), (-1.5*4+-2)
        VX_CHECK_EQ(r, vec3f(2.0f, 0.0f, -8.0f));
    }

    VX_SECTION("vec4")
    {
        const vec4f a(0.5f, -1.0f, 3.0f, 2.0f);
        const vec4f b(4.0f, 2.0f, -1.0f, 0.0f);
        const vec4f c(1.0f, -3.0f, 2.0f, 5.0f);
        const vec4f r = fma(a, b, c); // (0.5*4+1), (-1*2+-3), (3*-1+2), (2*0+5)
        VX_CHECK_EQ(r, vec4f(3.0f, -5.0f, -1.0f, 5.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_clamp)
{
    VX_SECTION("scalar")
    {
        VX_CHECK_EQ(vx::math::clamp(2.5f, 1.0f, 3.0f), 2.5f);  // Within range
        VX_CHECK_EQ(vx::math::clamp(0.5f, 1.0f, 3.0f), 1.0f);  // Below min
        VX_CHECK_EQ(vx::math::clamp(4.0f, 1.0f, 3.0f), 3.0f);  // Above max
    }

    VX_SECTION("vec2 (scalar)")
    {
        const vec2f a(2.5f, 0.5f);
        const vec2f b = clamp(a, 1.0f, 3.0f); // (2.5, 0.5) -> (2.5, 1.0)
        VX_CHECK_EQ(b, vec2f(2.5f, 1.0f));

        const vec2f c(4.0f, 2.5f);
        const vec2f d = clamp(c, 1.0f, 3.0f); // (4.0, 2.5) -> (3.0, 2.5)
        VX_CHECK_EQ(d, vec2f(3.0f, 2.5f));
    }

    VX_SECTION("vec3 (scalar)")
    {
        const vec3f a(1.5f, 2.0f, 3.5f);
        const vec3f b = clamp(a, 1.0f, 3.0f); // (1.5, 2.0, 3.5) -> (1.5, 2.0, 3.0)
        VX_CHECK_EQ(b, vec3f(1.5f, 2.0f, 3.0f));

        const vec3f c(0.5f, 4.0f, 3.0f);
        const vec3f d = clamp(c, 1.0f, 3.0f); // (0.5, 4.0, 3.0) -> (1.0, 3.0, 3.0)
        VX_CHECK_EQ(d, vec3f(1.0f, 3.0f, 3.0f));
    }

    VX_SECTION("vec4 (scalar)")
    {
        const vec4f a(3.5f, 2.5f, 4.0f, 0.5f);
        const vec4f b = clamp(a, 1.0f, 3.0f); // (3.5, 2.5, 4.0, 0.5) -> (3.0, 2.5, 3.0, 1.0)
        VX_CHECK_EQ(b, vec4f(3.0f, 2.5f, 3.0f, 1.0f));

        const vec4f c(0.5f, 2.0f, 1.5f, 4.5f);
        const vec4f d = clamp(c, 1.0f, 3.0f); // (0.5, 2.0, 1.5, 4.5) -> (1.0, 2.0, 1.5, 3.0)
        VX_CHECK_EQ(d, vec4f(1.0f, 2.0f, 1.5f, 3.0f));
    }

    VX_SECTION("vec2 (trinary)")
    {
        const vec2f a(2.5f, 0.5f);
        const vec2f min(1.0f, 1.0f);
        const vec2f max(3.0f, 3.0f);
        const vec2f b = clamp(a, min, max); // (2.5, 0.5) -> (2.5, 1.0)
        VX_CHECK_EQ(b, vec2f(2.5f, 1.0f));

        const vec2f c(4.0f, 2.5f);
        const vec2f d = clamp(c, min, max); // (4.0, 2.5) -> (3.0, 2.5)
        VX_CHECK_EQ(d, vec2f(3.0f, 2.5f));
    }

    VX_SECTION("vec3 (trinary)")
    {
        const vec3f a(1.5f, 2.0f, 3.5f);
        const vec3f min(1.0f, 1.0f, 2.0f);
        const vec3f max(3.0f, 3.0f, 4.0f);
        const vec3f b = clamp(a, min, max); // (1.5, 2.0, 3.5) -> (1.5, 2.0, 3.5)
        VX_CHECK_EQ(b, vec3f(1.5f, 2.0f, 3.5f));

        const vec3f c(0.5f, 4.0f, 3.0f);
        const vec3f d = clamp(c, min, max); // (0.5, 4.0, 3.0) -> (1.0, 3.0, 3.0)
        VX_CHECK_EQ(d, vec3f(1.0f, 3.0f, 3.0f));
    }

    VX_SECTION("vec4 (trinary)")
    {
        const vec4f a(3.5f, 2.5f, 4.0f, 0.5f);
        const vec4f min(1.0f, 1.0f, 2.0f, 0.0f);
        const vec4f max(3.0f, 3.0f, 3.0f, 2.0f);
        const vec4f b = clamp(a, min, max); // (3.5, 2.5, 4.0, 0.5) -> (3.0, 2.5, 3.0, 0.5)
        VX_CHECK_EQ(b, vec4f(3.0f, 2.5f, 3.0f, 0.5f));

        const vec4f c(0.5f, 2.0f, 1.5f, 4.5f);
        const vec4f d = clamp(c, min, max); // (0.5, 2.0, 1.5, 4.5) -> (1.0, 2.0, 2.0, 2.0)
        VX_CHECK_EQ(d, vec4f(1.0f, 2.0f, 2.0f, 2.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_gcd)
{
    VX_SECTION("scalar")
    {
        constexpr int gcd_48_18 = vx::math::gcd(48, 18);
        VX_STATIC_CHECK_EQ(gcd_48_18, 6); 

        constexpr int gcd_56_98 = vx::math::gcd(56, 98);
        VX_STATIC_CHECK_EQ(gcd_56_98, 14);

        constexpr int gcd_101_103 = vx::math::gcd(101, 103);
        VX_STATIC_CHECK_EQ(gcd_101_103, 1);

        constexpr int gcd_0_15 = vx::math::gcd(0, 15);
        VX_STATIC_CHECK_EQ(gcd_0_15, 15);

        constexpr int gcd_45_0 = vx::math::gcd(45, 0);
        VX_STATIC_CHECK_EQ(gcd_45_0, 45);
    }

    VX_SECTION("vec2 (scalar)")
    {
        constexpr vec2i a(48, 56);
        constexpr int b = 18;
        constexpr vec2i c = gcd(a, b);
        VX_STATIC_CHECK_EQ(c, vec2i(6, 2));

        constexpr vec2i d(101, 103);
        constexpr vec2i e = gcd(d, 1);
        VX_STATIC_CHECK_EQ(e, vec2i(1, 1));
    }

    VX_SECTION("vec3 (scalar)")
    {
        constexpr vec3i a(48, 56, 101);
        constexpr int b = 18;
        constexpr vec3i c = gcd(a, b);
        VX_STATIC_CHECK_EQ(c, vec3i(6, 2, 1));

        constexpr vec3i d(45, 0, 15);
        constexpr vec3i e = gcd(d, 5);
        VX_STATIC_CHECK_EQ(e, vec3i(5, 5, 5));
    }

    VX_SECTION("vec4 (scalar)")
    {
        constexpr vec4i a(48, 56, 101, 0);
        constexpr int b = 18;
        constexpr vec4i c = gcd(a, b);
        VX_STATIC_CHECK_EQ(c, vec4i(6, 2, 1, 18));

        constexpr vec4i d(45, 0, 15, 9);
        constexpr vec4i e = gcd(d, 5);
        VX_STATIC_CHECK_EQ(e, vec4i(5, 5, 5, 1));
    }

    VX_SECTION("vec2 (binary)")
    {
        constexpr vec2i a(48, 56);
        constexpr vec2i b(18, 98);
        constexpr vec2i c = gcd(a, b);
        VX_STATIC_CHECK_EQ(c, vec2i(6, 14));

        constexpr vec2i d(45, 15);
        constexpr vec2i e(9, 27);
        constexpr vec2i f = gcd(d, e);
        VX_STATIC_CHECK_EQ(f, vec2i(9, 3));
    }

    VX_SECTION("vec3 (binary)")
    {
        constexpr vec3i a(48, 56, 101);
        constexpr vec3i b(18, 98, 9);
        constexpr vec3i c = gcd(a, b);
        VX_STATIC_CHECK_EQ(c, vec3i(6, 14, 1));

        constexpr vec3i d(45, 0, 15);
        constexpr vec3i e(9, 27, 5);
        constexpr vec3i f = gcd(d, e);
        VX_STATIC_CHECK_EQ(f, vec3i(9, 27, 5));
    }

    VX_SECTION("vec4 (binary)")
    {
        constexpr vec4i a(48, 56, 101, 0);
        constexpr vec4i b(18, 98, 9, 15);
        constexpr vec4i c = gcd(a, b);
        VX_STATIC_CHECK_EQ(c, vec4i(6, 14, 1, 15));

        constexpr vec4i d(45, 0, 15, 9);
        constexpr vec4i e(9, 27, 5, 3);
        constexpr vec4i f = gcd(d, e);
        VX_STATIC_CHECK_EQ(f, vec4i(9, 27, 5, 3));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}