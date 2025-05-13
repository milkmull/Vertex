#include "vertex_test/test.hpp"

#include "vertex/math/fn/common.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_min)
{
    VX_SECTION("scalar")
    {
        constexpr i32 a = 5;
        constexpr i32 b = 3;
        constexpr i32 c = min(a, b);
        VX_STATIC_CHECK(c == 3);

        constexpr f32 x = 2.5f;
        constexpr f32 y = 7.1f;
        constexpr f32 z = min(x, y);
        VX_STATIC_CHECK(z == 2.5f);
    }

    VX_SECTION("scalar multiple args")
    {
        constexpr i32 a = 9;
        constexpr i32 b = 4;
        constexpr i32 c = 7;
        constexpr i32 d = 2;
        constexpr i32 result = min(a, b, c, d);
        VX_STATIC_CHECK(result == 2);

        constexpr f64 u = 3.14, v = 1.61, w = 2.72;
        constexpr f64 r = min(u, v, w);
        VX_STATIC_CHECK(r == 1.61);
    }

    VX_SECTION("vec2")
    {
        constexpr vec2i a(5, 2);
        constexpr vec2i b(3, 4);
        constexpr vec2i r1 = min(a, b);
        VX_STATIC_CHECK(r1 == vec2i(3, 2));

        constexpr vec2i r2 = min(a, 3);
        VX_STATIC_CHECK(r2 == vec2i(3, 2));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a(2.5f, 7.0f, 1.5f);
        constexpr vec3f b(3.0f, 4.0f, 5.0f);
        constexpr vec3f r1 = min(a, b);
        VX_STATIC_CHECK(r1 == vec3f(2.5f, 4.0f, 1.5f));

        constexpr vec3f r2 = min(a, 3.0f);
        VX_STATIC_CHECK(r2 == vec3f(2.5f, 3.0f, 1.5f));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(9, 4, 6, 2);
        constexpr vec4i b(3, 5, 7, 8);
        constexpr vec4i r1 = min(a, b);
        VX_STATIC_CHECK(r1 == vec4i(3, 4, 6, 2));

        constexpr vec4i r2 = min(a, 6);
        VX_STATIC_CHECK(r2 == vec4i(6, 4, 6, 2));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_max)
{
    VX_SECTION("scalar")
    {
        constexpr i32 a = 5;
        constexpr i32 b = 3;
        constexpr i32 c = max(a, b);
        VX_STATIC_CHECK(c == 5);

        constexpr f32 x = 2.5f;
        constexpr f32 y = 7.1f;
        constexpr f32 z = max(x, y);
        VX_STATIC_CHECK(z == 7.1f);
    }

    VX_SECTION("scalar multiple args")
    {
        constexpr i32 a = 9;
        constexpr i32 b = 4;
        constexpr i32 c = 7;
        constexpr i32 d = 2;
        constexpr i32 result = max(a, b, c, d);
        VX_STATIC_CHECK(result == 9);

        constexpr f64 u = 3.14, v = 1.61, w = 2.72;
        constexpr f64 r = max(u, v, w);
        VX_STATIC_CHECK(r == 3.14);
    }

    VX_SECTION("vec2")
    {
        constexpr vec2i a(5, 2);
        constexpr vec2i b(3, 4);
        constexpr vec2i r1 = max(a, b);
        VX_STATIC_CHECK(r1 == vec2i(5, 4));

        constexpr vec2i r2 = max(a, 3);
        VX_STATIC_CHECK(r2 == vec2i(5, 3));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a(2.5f, 7.0f, 1.5f);
        constexpr vec3f b(3.0f, 4.0f, 5.0f);
        constexpr vec3f r1 = max(a, b);
        VX_STATIC_CHECK(r1 == vec3f(3.0f, 7.0f, 5.0f));

        constexpr vec3f r2 = max(a, 3.0f);
        VX_STATIC_CHECK(r2 == vec3f(3.0f, 7.0f, 3.0f));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(9, 4, 6, 2);
        constexpr vec4i b(3, 5, 7, 8);
        constexpr vec4i r1 = max(a, b);
        VX_STATIC_CHECK(r1 == vec4i(9, 5, 7, 8));

        constexpr vec4i r2 = max(a, 6);
        VX_STATIC_CHECK(r2 == vec4i(9, 6, 6, 6));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cmin)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a(5, 2);
        constexpr i32 result_i = cmin(a);
        VX_STATIC_CHECK(result_i == 2);

        constexpr vec2f b(3.5f, 4.1f);
        constexpr f32 result_f = cmin(b);
        VX_STATIC_CHECK(result_f == 3.5f);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3i a(8, 3, 6);
        constexpr i32 result_i = cmin(a);
        VX_STATIC_CHECK(result_i == 3);

        constexpr vec3f b(7.0f, 2.5f, 9.1f);
        constexpr f32 result_f = cmin(b);
        VX_STATIC_CHECK(result_f == 2.5f);

        constexpr vec3d c(3.14, 1.618, 2.718);
        constexpr f64 result_d = cmin(c);
        VX_STATIC_CHECK(result_d == 1.618);
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(9, 4, 6, 2);
        constexpr i32 result_i = cmin(a);
        VX_STATIC_CHECK(result_i == 2);

        constexpr vec4f b(5.5f, 3.3f, 8.8f, 4.4f);
        constexpr f32 result_f = cmin(b);
        VX_STATIC_CHECK(result_f == 3.3f);

        constexpr vec4d c(1.0, 0.5, 2.0, 3.0);
        constexpr f64 result_d = cmin(c);
        VX_STATIC_CHECK(result_d == 0.5);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cmax)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a(5, 2);
        constexpr i32 result_i = cmax(a);
        VX_STATIC_CHECK(result_i == 5);

        constexpr vec2f b(3.5f, 4.1f);
        constexpr f32 result_f = cmax(b);
        VX_STATIC_CHECK(result_f == 4.1f);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3i a(8, 3, 6);
        constexpr i32 result_i = cmax(a);
        VX_STATIC_CHECK(result_i == 8);

        constexpr vec3f b(7.0f, 2.5f, 9.1f);
        constexpr f32 result_f = cmax(b);
        VX_STATIC_CHECK(result_f == 9.1f);

        constexpr vec3d c(3.14, 1.618, 2.718);
        constexpr f64 result_d = cmax(c);
        VX_STATIC_CHECK(result_d == 3.14);
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(9, 4, 6, 2);
        constexpr i32 result_i = cmax(a);
        VX_STATIC_CHECK(result_i == 9);

        constexpr vec4f b(5.5f, 3.3f, 8.8f, 4.4f);
        constexpr f32 result_f = cmax(b);
        VX_STATIC_CHECK(result_f == 8.8f);

        constexpr vec4d c(1.0, 0.5, 2.0, 3.0);
        constexpr f64 result_d = cmax(c);
        VX_STATIC_CHECK(result_d == 3.0);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_csum)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a(5, 2);
        constexpr i32 result_i = csum(a);
        VX_STATIC_CHECK(result_i == 7);

        constexpr vec2f b(3.5f, 4.1f);
        constexpr f32 result_f = csum(b);
        VX_STATIC_CHECK(result_f == 7.6f);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3i a(8, 3, 6);
        constexpr i32 result_i = csum(a);
        VX_STATIC_CHECK(result_i == 17);

        constexpr vec3f b(7.0f, 2.5f, 9.1f);
        constexpr f32 result_f = csum(b);
        VX_STATIC_CHECK(result_f == 18.6f);

        constexpr vec3d c(3.14, 1.618, 2.718);
        constexpr f64 result_d = csum(c);
        VX_STATIC_CHECK(result_d == (3.14 + 1.618 + 2.718));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(9, 4, 6, 2);
        constexpr i32 result_i = csum(a);
        VX_STATIC_CHECK(result_i == 21);

        constexpr vec4f b(5.5f, 3.3f, 8.8f, 4.4f);
        constexpr f32 result_f = csum(b);
        VX_STATIC_CHECK(result_f == 22.0f);

        constexpr vec4d c(1.0, 0.5, 2.0, 3.0);
        constexpr f64 result_d = csum(c);
        VX_STATIC_CHECK(result_d == 6.5);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_abs)
{
    VX_SECTION("scalar")
    {
        constexpr i32 a = -5;
        i32 b = vx::math::abs(a);
        VX_CHECK(b == 5);

        constexpr f32 x = -3.14f;
        f32 y = vx::math::abs(x);
        VX_CHECK(y == 3.14f);

        constexpr f64 z = -2.718;
        f64 result = vx::math::abs(z);
        VX_CHECK(result == 2.718);
    }

    VX_SECTION("vec2")
    {
        constexpr vec2i a(-1, 2);
        vec2i b = abs(a);
        VX_CHECK(b == vec2i(1, 2));

        constexpr vec2f c(-3.0f, -4.5f);
        vec2f d = abs(c);
        VX_CHECK(d == vec2f(3.0f, 4.5f));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3i a(-1, 0, 3);
        vec3i b = abs(a);
        VX_CHECK(b == vec3i(1, 0, 3));

        constexpr vec3f c(-1.5f, 2.0f, -3.5f);
        vec3f d = abs(c);
        VX_CHECK(d == vec3f(1.5f, 2.0f, 3.5f));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a(-1, -2, -3, -4);
        vec4i b = abs(a);
        VX_CHECK(b == vec4i(1, 2, 3, 4));

        constexpr vec4d c(-1.0, 2.0, -3.0, 4.0);
        vec4d d = abs(c);
        VX_CHECK(d == vec4d(1.0, 2.0, 3.0, 4.0));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_round)
{
    VX_SECTION("scalar")
    {
        f32 a = 3.14f;
        f32 b = vx::math::round(a);
        VX_CHECK(b == 3.0f);

        f64 x = 2.718;
        f64 y = vx::math::round(x);
        VX_CHECK(y == 3.0);

        f32 z = -4.6f;
        f32 result = vx::math::round(z);
        VX_CHECK(result == -5.0f);
    }

    VX_SECTION("vec2")
    {
        vec2f a(1.1f, 2.5f);
        vec2f b = round(a);
        VX_CHECK(b == vec2f(1.0f, 3.0f));

        vec2f x(-3.7f, 4.3f);
        vec2f y = round(x);
        VX_CHECK(y == vec2f(-4.0f, 4.0f));
    }

    VX_SECTION("vec3")
    {
        vec3f a(3.14f, 2.718f, 1.618f);
        vec3f b = round(a);
        VX_CHECK(b == vec3f(3.0f, 3.0f, 2.0f));

        vec3f x(4.4f, -5.5f, 6.6f);
        vec3f y = round(x);
        VX_CHECK(y == vec3f(4.0f, -6.0f, 7.0f));
    }

    VX_SECTION("vec4")
    {
        vec4f a(2.3f, -2.7f, 5.5f, 9.1f);
        vec4f b = round(a);
        VX_CHECK(b == vec4f(2.0f, -3.0f, 6.0f, 9.0f));

        vec4f x(-3.8f, 1.1f, -2.5f, 4.9f);
        vec4f y = round(x);
        VX_CHECK(y == vec4f(-4.0f, 1.0f, -3.0f, 5.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_trunc)
{
    VX_SECTION("scalar")
    {
        f32 a = 3.14f;
        f32 b = vx::math::trunc(a);
        VX_CHECK(b == 3.0f);

        f64 x = 2.718;
        f64 y = vx::math::trunc(x);
        VX_CHECK(y == 2.0);

        f32 z = -4.6f;
        f32 result = vx::math::trunc(z);
        VX_CHECK(result == -4.0f);
    }

    VX_SECTION("vec2")
    {
        vec2f a(1.1f, 2.5f);
        vec2f b = trunc(a);
        VX_CHECK(b == vec2f(1.0f, 2.0f));

        vec2f x(-3.7f, 4.3f);
        vec2f y = trunc(x);
        VX_CHECK(y == vec2f(-3.0f, 4.0f));
    }

    VX_SECTION("vec3")
    {
        vec3f a(3.14f, 2.718f, 1.618f);
        vec3f b = trunc(a);
        VX_CHECK(b == vec3f(3.0f, 2.0f, 1.0f));

        vec3f x(4.4f, -5.5f, 6.6f);
        vec3f y = trunc(x);
        VX_CHECK(y == vec3f(4.0f, -5.0f, 6.0f));
    }

    VX_SECTION("vec4")
    {
        vec4f a(2.3f, -2.7f, 5.5f, 9.1f);
        vec4f b = trunc(a);
        VX_CHECK(b == vec4f(2.0f, -2.0f, 5.0f, 9.0f));

        vec4f x(-3.8f, 1.1f, -2.5f, 4.9f);
        vec4f y = trunc(x);
        VX_CHECK(y == vec4f(-3.0f, 1.0f, -2.0f, 4.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_floor)
{
    VX_SECTION("scalar")
    {
        f32 a = 3.14f;
        f32 b = vx::math::floor(a);
        VX_CHECK(b == 3.0f);

        f64 x = 2.718;
        f64 y = vx::math::floor(x);
        VX_CHECK(y == 2.0);

        f32 z = -4.6f;
        f32 result = vx::math::floor(z);
        VX_CHECK(result == -5.0f);
    }

    VX_SECTION("vec2")
    {
        vec2f a(1.1f, 2.5f);
        vec2f b = floor(a);
        VX_CHECK(b == vec2f(1.0f, 2.0f));

        vec2f x(-3.7f, 4.3f);
        vec2f y = floor(x);
        VX_CHECK(y == vec2f(-4.0f, 4.0f));
    }

    VX_SECTION("vec3")
    {
        vec3f a(3.14f, 2.718f, 1.618f);
        vec3f b = floor(a);
        VX_CHECK(b == vec3f(3.0f, 2.0f, 1.0f));

        vec3f x(4.4f, -5.5f, 6.6f);
        vec3f y = floor(x);
        VX_CHECK(y == vec3f(4.0f, -6.0f, 6.0f));
    }

    VX_SECTION("vec4")
    {
        vec4f a(2.3f, -2.7f, 5.5f, 9.1f);
        vec4f b = floor(a);
        VX_CHECK(b == vec4f(2.0f, -3.0f, 5.0f, 9.0f));

        vec4f x(-3.8f, 1.1f, -2.5f, 4.9f);
        vec4f y = floor(x);
        VX_CHECK(y == vec4f(-4.0f, 1.0f, -3.0f, 4.0f));
    }
}


///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_ceil)
{
    VX_SECTION("scalar")
    {
        f32 a = 3.14f;
        f32 b = vx::math::ceil(a);
        VX_CHECK(b == 4.0f);

        f64 x = 2.718;
        f64 y = vx::math::ceil(x);
        VX_CHECK(y == 3.0);

        f32 z = -4.6f;
        f32 result = vx::math::ceil(z);
        VX_CHECK(result == -4.0f);
    }

    VX_SECTION("vec2")
    {
        vec2f a(1.1f, 2.5f);
        vec2f b = ceil(a);
        VX_CHECK(b == vec2f(2.0f, 3.0f));

        vec2f x(-3.7f, 4.3f);
        vec2f y = ceil(x);
        VX_CHECK(y == vec2f(-3.0f, 5.0f));
    }

    VX_SECTION("vec3")
    {
        vec3f a(3.14f, 2.718f, 1.618f);
        vec3f b = ceil(a);
        VX_CHECK(b == vec3f(4.0f, 3.0f, 2.0f));

        vec3f x(4.4f, -5.5f, 6.6f);
        vec3f y = ceil(x);
        VX_CHECK(y == vec3f(5.0f, -5.0f, 7.0f));
    }

    VX_SECTION("vec4")
    {
        vec4f a(2.3f, -2.7f, 5.5f, 9.1f);
        vec4f b = ceil(a);
        VX_CHECK(b == vec4f(3.0f, -2.0f, 6.0f, 10.0f));

        vec4f x(-3.8f, 1.1f, -2.5f, 4.9f);
        vec4f y = ceil(x);
        VX_CHECK(y == vec4f(-3.0f, 2.0f, -2.0f, 5.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_sign)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK(sign(10.0f) == 1.0f);
        VX_STATIC_CHECK(sign(-5.0f) == -1.0f);
        VX_STATIC_CHECK(sign(0.0f) == 0.0f);

        VX_STATIC_CHECK(sign(42) == 1);
        VX_STATIC_CHECK(sign(-42) == -1);
        VX_STATIC_CHECK(sign(0) == 0);
    }

    VX_SECTION("vec2")
    {
        constexpr vec2f a(5.0f, -2.0f);
        constexpr vec2f b = sign(a);
        VX_STATIC_CHECK(b == vec2f(1.0f, -1.0f));

        constexpr vec2f x(0.0f, -0.0f);
        constexpr vec2f y = sign(x);
        VX_STATIC_CHECK(y == vec2f(0.0f, 0.0f));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a(1.5f, 0.0f, -3.3f);
        constexpr vec3f b = sign(a);
        VX_STATIC_CHECK(b == vec3f(1.0f, 0.0f, -1.0f));

        constexpr vec3f x(-0.0f, 2.2f, -2.2f);
        constexpr vec3f y = sign(x);
        VX_STATIC_CHECK(y == vec3f(0.0f, 1.0f, -1.0f));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a(7.1f, -7.1f, 0.0f, 3.0f);
        constexpr vec4f b = sign(a);
        VX_STATIC_CHECK(b == vec4f(1.0f, -1.0f, 0.0f, 1.0f));

        constexpr vec4f x(-1.0f, -0.0f, 0.0f, 1.0f);
        constexpr vec4f y = sign(x);
        VX_STATIC_CHECK(y == vec4f(-1.0f, 0.0f, 0.0f, 1.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_fmod)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(vx::math::fmod(5.3f, 2.0f) == 1.3f);
        VX_CHECK(vx::math::fmod(-5.3f, 2.0f) == -1.3f);
        VX_CHECK(vx::math::fmod(5.3f, -2.0f) == 1.3f);
        VX_CHECK(vx::math::fmod(-5.3f, -2.0f) == -1.3f);

        VX_CHECK(vx::math::fmod(7.0, 3.0) == 1.0);
        VX_CHECK(vx::math::fmod(-7.0, 3.0) == -1.0);
    }

    VX_SECTION("vec2 scalar")
    {
        const vec2f x(5.5f, -4.0f);
        const vec2f y = fmod(x, 2.0f);
        VX_CHECK(y == vec2f(1.5f, 0.0f));
    }

    VX_SECTION("vec3 scalar")
    {
        const vec3f x(9.0f, -7.5f, 3.3f);
        const vec3f y = fmod(x, 3.0f);
        VX_CHECK(y == vec3f(0.0f, -1.5f, 0.3f));
    }

    VX_SECTION("vec4 scalar")
    {
        const vec4f x(6.2f, -6.2f, 10.0f, -10.0f);
        const vec4f y = fmod(x, 4.0f);
        VX_CHECK(y == vec4f(2.2f, -2.2f, 2.0f, -2.0f));
    }

    VX_SECTION("vec2 binary")
    {
        const vec2f x(9.0f, -9.0f);
        const vec2f y(4.0f, 4.0f);
        const vec2f result = fmod(x, y);
        VX_CHECK(result == vec2f(1.0f, -1.0f));
    }

    VX_SECTION("vec3 binary")
    {
        const vec3f x(7.0f, 8.0f, 9.0f);
        const vec3f y(3.0f, 2.5f, 4.0f);
        const vec3f result = fmod(x, y);
        VX_CHECK(result == vec3f(1.0f, 0.5f, 1.0f));
    }

    VX_SECTION("vec4 binary")
    {
        const vec4f x(10.5f, -10.5f, 7.0f, -7.0f);
        const vec4f y(3.0f, 3.0f, 4.0f, 4.0f);
        const vec4f result = fmod(x, y);
        VX_CHECK(result == vec4f(1.5f, -1.5f, 3.0f, -3.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mod)
{
    VX_SECTION("scalar float")
    {
        VX_CHECK(vx::math::mod(5.3f, 2.0f) == 1.3f);
        VX_CHECK(vx::math::mod(-5.3f, 2.0f) == 0.7f);
        VX_CHECK(vx::math::mod(5.3f, -2.0f) == -1.3f);
        VX_CHECK(vx::math::mod(-5.3f, -2.0f) == -0.7f);

        VX_CHECK(vx::math::mod(7.0, 3.0) == 1.0);
        VX_CHECK(vx::math::mod(-7.0, 3.0) == 2.0);
    }

    VX_SECTION("scalar int")
    {
        VX_CHECK(mod(7, 3) == 1);
        VX_CHECK(mod(-7, 3) == 2);
        VX_CHECK(mod(7, -3) == -2);
        VX_CHECK(mod(-7, -3) == -1);
    }

    VX_SECTION("vec2 float scalar")
    {
        const vec2f x(5.5f, -4.0f);
        const vec2f y = mod(x, 2.0f);
        VX_CHECK(y == vec2f(1.5f, 0.0f));
    }

    VX_SECTION("vec3 float scalar")
    {
        const vec3f x(9.0f, -7.5f, 3.3f);
        const vec3f y = mod(x, 3.0f);
        VX_CHECK(y == vec3f(0.0f, 1.5f, 0.3f));
    }

    VX_SECTION("vec4 float scalar")
    {
        const vec4f x(6.2f, -6.2f, 10.0f, -10.0f);
        const vec4f y = mod(x, 4.0f);
        VX_CHECK(y == vec4f(2.2f, 1.8f, 2.0f, 2.0f));
    }

    VX_SECTION("vec2 float binary")
    {
        const vec2f x(9.0f, -9.0f);
        const vec2f y(4.0f, 4.0f);
        const vec2f result = mod(x, y);
        VX_CHECK(result == vec2f(1.0f, 3.0f));
    }

    VX_SECTION("vec3 float binary")
    {
        const vec3f x(7.0f, 8.0f, 9.0f);
        const vec3f y(3.0f, 2.5f, 4.0f);
        const vec3f result = mod(x, y);
        VX_CHECK(result == vec3f(1.0f, 0.5f, 1.0f));
    }

    VX_SECTION("vec4 float binary")
    {
        const vec4f x(10.5f, -10.5f, 7.0f, -7.0f);
        const vec4f y(3.0f, 3.0f, 4.0f, 4.0f);
        const vec4f result = mod(x, y);
        VX_CHECK(result == vec4f(1.5f, 2.5f, 3.0f, 1.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_fract)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(vx::math::fract(0.0f) == 0.0f);
        VX_CHECK(vx::math::fract(0.5f) == 0.5f);
        VX_CHECK(vx::math::fract(1.0f) == 0.0f);
        VX_CHECK(vx::math::fract(-0.5f) == 0.5f);
        VX_CHECK(vx::math::fract(-1.2f) == 0.8f);

        VX_CHECK(vx::math::fract(2.75) == 0.75);
        VX_CHECK(vx::math::fract(-3.25) == 0.75);
    }

    VX_SECTION("vec2")
    {
        const vec2f x(1.5f, -2.3f);
        const vec2f y = fract(x);
        VX_CHECK(y == vec2f(0.5f, 0.7f));
    }

    VX_SECTION("vec3")
    {
        const vec3f x(0.0f, 2.9f, -1.1f);
        const vec3f y = fract(x);
        VX_CHECK(y == vec3f(0.0f, 0.9f, 0.9f));
    }

    VX_SECTION("vec4")
    {
        const vec4f x(-3.75f, 4.125f, 0.0f, -0.875f);
        const vec4f y = fract(x);
        VX_CHECK(y == vec4f(0.25f, 0.125f, 0.0f, 0.125f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_modf)
{
    VX_SECTION("scalar")
    {
        f32 intpart = 0.0f;
        f32 frac = vx::math::modf(2.75f, intpart);
        VX_CHECK(frac == 0.75f);
        VX_CHECK(intpart == 2.0f);

        frac = vx::math::modf(-1.25f, intpart);
        VX_CHECK(frac == -0.25f);
        VX_CHECK(intpart == -1.0f);

        f64 intpart_d = 0.0;
        f64 frac_d = vx::math::modf(5.9, intpart_d);
        VX_CHECK(frac_d == 0.9);
        VX_CHECK(intpart_d == 5.0);
    }

    VX_SECTION("vec2")
    {
        vec2f intpart;
        vec2f result = modf(vec2f(1.5f, -2.25f), intpart);
        VX_CHECK(result == vec2f(0.5f, -0.25f));
        VX_CHECK(intpart == vec2f(1.0f, -2.0f));
    }

    VX_SECTION("vec3")
    {
        vec3f intpart;
        vec3f result = modf(vec3f(0.0f, 3.7f, -4.8f), intpart);
        VX_CHECK(result == vec3f(0.0f, 0.7f, -0.8f));
        VX_CHECK(intpart == vec3f(0.0f, 3.0f, -4.0f));
    }

    VX_SECTION("vec4")
    {
        vec4f intpart;
        vec4f result = modf(vec4f(-5.5f, 6.125f, 0.0f, -0.875f), intpart);
        VX_CHECK(result == vec4f(-0.5f, 0.125f, 0.0f, -0.875f));
        VX_CHECK(intpart == vec4f(-5.0f, 6.0f, 0.0f, 0.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_fma)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(vx::math::fma(2.0f, 3.0f, 4.0f) == 10.0f);  // 2 * 3 + 4
        VX_CHECK(vx::math::fma(-1.5f, 4.0f, 2.0f) == -4.0f); // -1.5 * 4 + 2
        VX_CHECK(vx::math::fma(0.0f, 3.0f, 5.0f) == 5.0f);   // 0 * 3 + 5
        VX_CHECK(vx::math::fma(2.0, 2.0, 2.0) == 6.0);       // double
    }

    VX_SECTION("vec2")
    {
        vec2f x(1.0f, -2.0f);
        vec2f y(4.0f, 3.0f);
        vec2f z(0.5f, 1.5f);
        VX_CHECK(fma(x, y, z) == vec2f(4.5f, -4.5f));  // 1*4+0.5, -2*3+1.5
    }

    VX_SECTION("vec3")
    {
        vec3f x(1.0f, 0.0f, -1.0f);
        vec3f y(2.0f, 3.0f, 4.0f);
        vec3f z(5.0f, 6.0f, 7.0f);
        VX_CHECK(fma(x, y, z) == vec3f(7.0f, 6.0f, 3.0f));
    }

    VX_SECTION("vec4")
    {
        vec4f x(1.0f, -1.0f, 0.5f, -0.5f);
        vec4f y(2.0f, 2.0f, 4.0f, 4.0f);
        vec4f z(0.0f, 1.0f, 0.0f, -1.0f);
        VX_CHECK(fma(x, y, z) == vec4f(2.0f, -1.0f, 2.0f, -3.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_frexp)
{
    VX_SECTION("scalar")
    {
        i32 exp = 0;
        f32 sig = vx::math::frexp(8.0f, exp);
        VX_CHECK(sig == 0.5f);
        VX_CHECK(exp == 4);

        exp = 0;
        sig = vx::math::frexp(-12.0f, exp);
        VX_CHECK(sig == -0.75f);
        VX_CHECK(exp == 4);
    }

    VX_SECTION("vec2")
    {
        vec2f input(16.0f, -5.0f);
        vec2i exp;
        vec2f sig = frexp(input, exp);
        VX_CHECK(sig == vec2f(0.5f, -0.625f));
        VX_CHECK(exp == vec2i(5, 3));
    }

    VX_SECTION("vec3")
    {
        vec3f input(0.0f, 1.0f, -2.0f);
        vec3i exp;
        vec3f sig = frexp(input, exp);
        VX_CHECK(sig == vec3f(0.0f, 0.5f, -0.5f));
        VX_CHECK(exp == vec3i(0, 1, 2));
    }

    VX_SECTION("vec4")
    {
        vec4f input(3.0f, 6.0f, 12.0f, 24.0f);
        vec4i exp;
        vec4f sig = frexp(input, exp);
        VX_CHECK(sig == vec4f(0.75f, 0.75f, 0.75f, 0.75f));
        VX_CHECK(exp == vec4i(2, 3, 4, 5));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_ldexp)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(vx::math::ldexp(0.5f, 4) == 8.0f);
        VX_CHECK(vx::math::ldexp(-0.75f, 3) == -6.0f);
    }

    VX_SECTION("vec2")
    {
        vec2f sig(0.5f, -0.625f);
        vec2i exp(5, 3);
        VX_CHECK(ldexp(sig, exp) == vec2f(16.0f, -5.0f));
    }

    VX_SECTION("vec3")
    {
        vec3f sig(0.0f, 0.5f, -0.5f);
        vec3i exp(0, 1, 2);
        VX_CHECK(ldexp(sig, exp) == vec3f(0.0f, 1.0f, -2.0f));
    }

    VX_SECTION("vec4")
    {
        vec4f sig(0.75f);
        vec4i exp(2, 3, 4, 5);
        VX_CHECK(ldexp(sig, exp) == vec4f(3.0f, 6.0f, 12.0f, 24.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_clamp)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(vx::math::clamp(5.0f, 1.0f, 10.0f) == 5.0f);
        VX_CHECK(vx::math::clamp(-5.0f, 0.0f, 10.0f) == 0.0f);
        VX_CHECK(vx::math::clamp(15.0f, 0.0f, 10.0f) == 10.0f);
    }

    VX_SECTION("vec2")
    {
        constexpr vec2f sig(0.5f, 12.0f);
        constexpr vec2f min(1.0f, 5.0f);
        constexpr vec2f max(10.0f, 15.0f);
        VX_STATIC_CHECK(clamp(sig, min, max) == vec2f(1.0f, 12.0f));

        constexpr vec2f sig2(0.0f, 20.0f);
        constexpr vec2f min2(2.0f, 3.0f);
        constexpr vec2f max2(5.0f, 18.0f);
        VX_STATIC_CHECK(clamp(sig2, min2, max2) == vec2f(2.0f, 18.0f));

        VX_STATIC_CHECK(clamp(sig, 1.0f, 10.0f) == vec2f(1.0f, 10.0f));
        VX_STATIC_CHECK(clamp(sig2, 2.0f, 18.0f) == vec2f(2.0f, 18.0f));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f sig(0.5f, 12.0f, -3.0f);
        constexpr vec3f min(1.0f, 5.0f, -2.0f);
        constexpr vec3f max(10.0f, 15.0f, 2.0f);
        VX_STATIC_CHECK(clamp(sig, min, max) == vec3f(1.0f, 12.0f, -2.0f));

        constexpr vec3f sig2(0.0f, 20.0f, -10.0f);
        constexpr vec3f min2(2.0f, 3.0f, -5.0f);
        constexpr vec3f max2(5.0f, 18.0f, 0.0f);
        VX_STATIC_CHECK(clamp(sig2, min2, max2) == vec3f(2.0f, 18.0f, -5.0f));

        VX_STATIC_CHECK(clamp(sig, 1.0f, 10.0f) == vec3f(1.0f, 10.0f, 1.0f));
        VX_STATIC_CHECK(clamp(sig2, 2.0f, 18.0f) == vec3f(2.0f, 18.0f, 2.0f));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f sig(0.75f, 12.0f, -3.0f, 6.0f);
        constexpr vec4f min(1.0f, 5.0f, -5.0f, 3.0f);
        constexpr vec4f max(10.0f, 15.0f, 2.0f, 8.0f);
        VX_STATIC_CHECK(clamp(sig, min, max) == vec4f(1.0f, 12.0f, -3.0f, 6.0f));

        constexpr vec4f sig2(0.0f, 20.0f, -10.0f, 15.0f);
        constexpr vec4f min2(2.0f, 3.0f, -5.0f, 10.0f);
        constexpr vec4f max2(5.0f, 18.0f, 0.0f, 14.0f);
        VX_STATIC_CHECK(clamp(sig2, min2, max2) == vec4f(2.0f, 18.0f, -5.0f, 14.0f));

        VX_STATIC_CHECK(clamp(sig, 1.0f, 10.0f) == vec4f(1.0f, 10.0f, 1.0f, 6.0f));
        VX_STATIC_CHECK(clamp(sig2, 2.0f, 18.0f) == vec4f(2.0f, 18.0f, 2.0f, 15.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_gcd)
{
    VX_SECTION("scalar")
    {
        constexpr i32 result1 = gcd(10, 5);  // gcd(10, 5) should be 5
        VX_STATIC_CHECK(result1 == 5);

        constexpr i32 result2 = gcd(13, 7);  // gcd(13, 7) should be 1 (they are coprime)
        VX_STATIC_CHECK(result2 == 1);

        constexpr i32 result3 = gcd(56, 98);  // gcd(56, 98) should be 14
        VX_STATIC_CHECK(result3 == 14);

        constexpr i32 result4 = gcd(0, 5);  // gcd(0, 5) should be 5
        VX_STATIC_CHECK(result4 == 5);

        constexpr i32 result5 = gcd(5, 0);  // gcd(5, 0) should be 5
        VX_STATIC_CHECK(result5 == 5);
    }

    VX_SECTION("vec2")
    {
        constexpr vec2i a1(10, 14), b1(5, 7);
        constexpr auto result1 = gcd(a1, b1);
        VX_STATIC_CHECK(result1.x == 5 && result1.y == 7);  // gcd(10, 5) = 5, gcd(14, 7) = 7

        constexpr vec2i a2(13, 9), b2(7, 3);
        constexpr auto result2 = gcd(a2, b2);
        VX_STATIC_CHECK(result2.x == 1 && result2.y == 3);  // gcd(13, 7) = 1, gcd(9, 3) = 3
    }

    VX_SECTION("vec3")
    {
        constexpr vec3i a1(10, 28, 12), b1(5, 7, 3);
        constexpr auto result1 = gcd(a1, b1);
        VX_STATIC_CHECK(result1.x == 5 && result1.y == 7 && result1.z == 3);  // gcd(10, 5) = 5, gcd(28, 7) = 7, gcd(12, 3) = 3

        constexpr vec3i a2(15, 9, 8), b2(5, 3, 4);
        constexpr auto result2 = gcd(a2, b2);
        VX_STATIC_CHECK(result2.x == 5 && result2.y == 3 && result2.z == 4);  // gcd(15, 5) = 5, gcd(9, 3) = 3, gcd(8, 4) = 4
    }

    VX_SECTION("vec4")
    {
        constexpr vec4i a1(10, 28, 35, 15), b1(5, 7, 5, 3);
        constexpr auto result1 = gcd(a1, b1);
        VX_STATIC_CHECK(result1.x == 5 && result1.y == 7 && result1.z == 5 && result1.w == 3);  // gcd(10, 5) = 5, gcd(28, 7) = 7, gcd(35, 5) = 5, gcd(15, 3) = 3
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}