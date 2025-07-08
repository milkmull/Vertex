#include "vertex_test/test.hpp"

#include "vertex/math/color/types.hpp"
#include "vertex/math/color/functions/comparison.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

#define check_each(c, cr, cg, cb, ca) VX_STATIC_CHECK(c.r == cr && c.g == cg && c.b == cb && c.a == ca)
#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_color)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename color::scalar_type, f32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(color) == 16);
        VX_STATIC_CHECK(color::size == 4);
    }

    VX_SECTION("channels")
    {
        VX_STATIC_CHECK(color::min_channel_value == 0.0f);
        VX_STATIC_CHECK(color::max_channel_value == 1.0f);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(color(), 0.0f, 0.0f, 0.0f, 1.0f);
        check_each(color(color(1.0f, 2.0f, 3.0f, 4.0f)), 1.0f, 2.0f, 3.0f, 4.0f);

        // explicit constructors
        check_each(color(0.5f), 0.5f, 0.5f, 0.5f, 1.0f);
        check_each(color(0.25f, 0.25f, 0.25f), 0.25f, 0.25f, 0.25f, 1.0f);
        check_each(color(0.25f, 0.25f, 0.25f, 0.25f), 0.25f, 0.25f, 0.25f, 0.25f);

        // conversion constructors
        check_each(color(true), 1.0f, 1.0f, 1.0f, 1.0f);
        check_each(color(1, 2u, 3.0), 1.0f, 2.0f, 3.0f, 1.0f);
        check_each(color(vec3i(1, 2, 3)), 1.0f, 2.0f, 3.0f, 1.0f);
        check_each(color(vec3i(1, 2, 3), 4), 1.0f, 2.0f, 3.0f, 4.0f);
        check_each(color(vec4d(1.0, 2.0, 3.0, 4.0)), 1.0f, 2.0f, 3.0f, 4.0f);
    }

    VX_SECTION("conversions")
    {
        // color conversion
        {
            constexpr color cf(-1.0f, 0.0f, 0.5f, 2.0f);

            constexpr color8 c8(cf);
            VX_CHECK_EQ(c8, color8(0, 0, 127, 255));

            constexpr color cf2(c8);
            VX_CHECK(equal_approx(cf2, color(0.0f, 0.0f, 0.5f, 1.0f), 1e-2f));
        }

        // to vec4
        {
            const color c(1.0f, 2.0f, 3.0f, 4.0f);
            const vec4i v = static_cast<vec4i>(c);
            VX_CHECK_EQ(v, vec4i(1, 2, 3, 4));
        }
    }

    VX_SECTION("assignment")
    {
        {
            color c1(1.0f, 2.0f, 3.0f, 4.0f);
            color c2(3.0f, 2.0f, 1.0f, 0.0f);

            c1 = c2;
            VX_CHECK(c1.r == 3.0f);
            VX_CHECK(c1.g == 2.0f);
            VX_CHECK(c1.b == 1.0f);
            VX_CHECK(c1.a == 0.0f);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(color(0.0f, 0.25f, -1.23f, 4.56f)[0] == 0.0f);
        VX_STATIC_CHECK(color(0.0f, 0.25f, -1.23f, 4.56f)[1] == 0.25f);
        VX_STATIC_CHECK(color(0.0f, 0.25f, -1.23f, 4.56f)[2] == -1.23f);
        VX_STATIC_CHECK(color(0.0f, 0.25f, -1.23f, 4.56f)[3] == 4.56f);

        color c;

        c[0] = 0.0f;
        VX_CHECK(c[0] == 0.0f);
        c[1] = 0.25f;
        VX_CHECK(c[1] == 0.25f);
        c[2] = -1.23f;
        VX_CHECK(c[2] == -1.23f);
        c[3] = 4.56f;
        VX_CHECK(c[3] == 4.56f);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(color(1.5f, -2.75f, -1.23f, 4.56f) == color(1.5f, -2.75f, -1.23f, 4.56f));
        VX_STATIC_CHECK(color(1.5f, 2.75f, -1.23f, 4.56f) != color(-1.5f, -2.75f, -1.23f, 4.56f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+color(1.0f, -1.5f, 2.0f, 4.0f) == color(1.0f, -1.5f, 2.0f, 4.0f));
        VX_STATIC_CHECK(+color(-1.5f, 2.5f, -3.0f, 5.5f) == color(-1.5f, 2.5f, -3.0f, 5.5f));

        VX_STATIC_CHECK(-color(1.5f, -2.5f, 3.0f, -4.0f) == color(-1.5f, 2.5f, -3.0f, 4.0f));
        VX_STATIC_CHECK(-color(-1.5f, 2.5f, -3.0f, 4.0f) == color(1.5f, -2.5f, 3.0f, -4.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++color(0.5f, -1.0f, 2.0f, 3.0f) == color(1.5f, 0.0f, 3.0f, 4.0f));
        VX_STATIC_CHECK(--color(0.5f, -1.0f, 2.0f, 3.0f) == color(-0.5f, -2.0f, 1.0f, 2.0f));

        color v(0.5f, -1.0f, 2.0f, 3.0f);

        VX_CHECK(v++ == color(0.5f, -1.0f, 2.0f, 3.0f));
        VX_CHECK(v == color(1.5f, 0.0f, 3.0f, 4.0f));
        VX_CHECK(v-- == color(1.5f, 0.0f, 3.0f, 4.0f));
        VX_CHECK(v == color(0.5f, -1.0f, 2.0f, 3.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_CHECK(color(1.5f, -2.75f, 3.0f, 4.0f) + 5.0f == color(6.5f, 2.25f, 8.0f, 9.0f));
        VX_CHECK(5.0f + color(1.5f, -2.75f, 3.0f, 4.0f) == color(6.5f, 2.25f, 8.0f, 9.0f));
        VX_CHECK(color(5.5f) + color(1.5f, -2.75f, 3.0f, 4.0f) == color(7.0f, 2.75f, 8.5f, 5.0f));
        VX_CHECK(color(1.5f, -2.75f, 3.0f, 4.0f) + color(5.0f) == color(6.5f, 2.25f, 8.0f, 5.0f));

        // subtraction
        VX_CHECK(color(1.5f, -2.75f, 3.0f, 4.0f) - 5.0f == color(-3.5f, -7.75f, -2.0f, -1.0f));
        VX_CHECK(5.0f - color(1.5f, -2.75f, 3.0f, 4.0f) == color(3.5f, 7.75f, 2.0f, 1.0f));
        VX_CHECK(color(5.5f) - color(1.5f, -2.75f, 3.0f, 4.0f) == color(4.0f, 8.25f, 2.5f, -3.0f));
        VX_CHECK(color(1.5f, -2.75f, 3.0f, 4.0f) - color(5.0f) == color(-3.5f, -7.75f, -2.0f, 3.0f));

        // multiplication
        VX_CHECK(color(1.5f, -2.75f, 3.0f, 4.0f) * 5.0f == color(7.5f, -13.75f, 15.0f, 20.0f));
        VX_CHECK(5.0f * color(1.5f, -2.75f, 3.0f, 4.0f) == color(7.5f, -13.75f, 15.0f, 20.0f));
        VX_CHECK(color(5.5f) * color(1.5f, -2.75f, 3.0f, 4.0f) == color(8.25f, -15.125f, 16.5f, 4.0f));
        VX_CHECK(color(1.5f, -2.75f, 3.0f, 4.0f) * color(5.0f) == color(7.5f, -13.75f, 15.0f, 4.0f));

        // division
        VX_CHECK(color(5.5f, -13.75f, 15.0f, 20.0f) / 5.0f == color(1.1f, -2.75f, 3.0f, 4.0f));
        VX_CHECK(10.0f / color(10.0f, 5.0f, 2.0f, 1.0f) == color(1.0f, 2.0f, 5.0f, 10.0f));
        VX_CHECK(color(10.5f) / color(10.0f, 5.0f, 2.0f, 1.0f) == color(1.05f, 2.1f, 5.25f, 1.0f));
        VX_CHECK(color(10.0f, 5.0f, 2.0f, 1.0f) / color(5.0f, 10.0f, 2.0f, 1.0f) == color(2.0f, 0.5f, 1.0f, 1.0f));
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            color v;
            v += 5.5f;
            VX_CHECK(v == color(5.5f, 5.5f, 5.5f, 6.5f));
            v += color(1.5f, -2.75f, 3.0f, 4.0f);
            VX_CHECK(v == color(7.0f, 2.75f, 8.5f, 10.5f));
        }

        // subtraction
        {
            color v(10.5f, 20.75f, 30.0f, 40.0f);
            v -= 5.0f;
            VX_CHECK(v == color(5.5f, 15.75f, 25.0f, 35.0f));
            v -= color(1.5f, -2.75f, 3.0f, 4.0f);
            VX_CHECK(v == color(4.0f, 18.5f, 22.0f, 31.0f));
        }

        // multiplication
        {
            color v(1.5f, -2.75f, 3.0f, 4.0f);
            v *= 5.0f;
            VX_CHECK(v == color(7.5f, -13.75f, 15.0f, 20.0f));
            v *= color(2.0f, -3.0f, 4.0f, 5.0f);
            VX_CHECK(v == color(15.0f, 41.25f, 60.0f, 100.0f));
        }

        // division
        {
            color v(100.5f, 50.0f, 75.0f, 40.0f);
            v /= 5.0f;
            VX_CHECK(v == color(20.1f, 10.0f, 15.0f, 8.0f));
            v /= color(2.0f, 5.0f, 3.0f, 4.0f);
            VX_CHECK(v == color(10.05f, 2.0f, 5.0f, 2.0f));
        }
    }

    VX_SECTION("rgb")
    {
        constexpr color c(0.0f, 0.5f, 1.0f);
        VX_CHECK_EQ(c.rgb(), vec3(0.0f, 0.5f, 1.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_color8)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename color8::scalar_type, u8>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(color8) == 4);
        VX_STATIC_CHECK(color8::size == 4);
    }

    VX_SECTION("channels")
    {
        VX_STATIC_CHECK(color8::min_channel_value == 0);
        VX_STATIC_CHECK(color8::max_channel_value == 255);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(color8(), 0, 0, 0, 255);
        check_each(color8(color8(1, 2, 3, 4)), 1, 2, 3, 4);

        // explicit constructors
        check_each(color8(128), 128, 128, 128, 255);
        check_each(color8(64, 64, 64), 64, 64, 64, 255);
        check_each(color8(64, 64, 64, 64), 64, 64, 64, 64);

        // conversion constructors
        check_each(color8(true), 1, 1, 1, 255);
        check_each(color8(1, 2u, 3.0), 1, 2, 3, 255);
        check_each(color8(vec3i(1, 2, 3)), 1, 2, 3, 255);
        check_each(color8(vec3i(1, 2, 3), 4), 1, 2, 3, 4);
        check_each(color8(vec4(1.0f, 2.0f, 3.0f, 4.0f)), 1, 2, 3, 4);
    }

    VX_SECTION("conversions")
    {
        // to and from float color
        {
            constexpr color cf(-1.0f, 0.0f, 0.5f, 2.0f);
            constexpr color8 c8(cf);
            VX_CHECK_EQ(c8, color8(0, 0, 127, 255));

            constexpr color cf2(c8);
            VX_CHECK_EQ(cf2, color(0.0f, 0.0f, 127.0f / 255.0f, 1.0f));
        }

        // to vec4
        {
            const color8 c(1, 2, 3, 4);
            const vec4i v = static_cast<vec4i>(c);
            VX_CHECK_EQ(v, vec4i(1, 2, 3, 4));
        }
    }

    VX_SECTION("hex conversion")
    {
        // Construct from hex
        {
            constexpr uint32_t hex = 0xAABBCCDD;
            constexpr color8 c = color8::from_hex(hex);
            VX_STATIC_CHECK(c.r == 0xDD);
            VX_STATIC_CHECK(c.g == 0xCC);
            VX_STATIC_CHECK(c.b == 0xBB);
            VX_STATIC_CHECK(c.a == 0xAA);
        }

        // Convert to hex
        {
            constexpr color8 c(0x11, 0x22, 0x33, 0x44);
            constexpr uint32_t hex = c.to_hex();
            VX_STATIC_CHECK(hex == 0x44332211);
        }

        // Round trip
        {
            constexpr uint32_t hex = 0xDEADBEEF;
            constexpr color8 c = color8::from_hex(hex);
            constexpr uint32_t rt = c.to_hex();
            VX_STATIC_CHECK(rt == hex);
        }
    }

    VX_SECTION("assignment")
    {
        color8 c1(1, 2, 3, 4);
        color8 c2(5, 6, 7, 8);

        c1 = c2;
        VX_CHECK(c1.r == 5);
        VX_CHECK(c1.g == 6);
        VX_CHECK(c1.b == 7);
        VX_CHECK(c1.a == 8);
    }

    VX_SECTION("index")
    {
        color8 c(10, 20, 30, 40);
        VX_CHECK(c[0] == 10);
        VX_CHECK(c[1] == 20);
        VX_CHECK(c[2] == 30);
        VX_CHECK(c[3] == 40);

        c[0] = 1;
        VX_CHECK(c[0] == 1);
        c[1] = 2;
        VX_CHECK(c[1] == 2);
        c[2] = 3;
        VX_CHECK(c[2] == 3);
        c[3] = 4;
        VX_CHECK(c[3] == 4);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(color8(1, 2, 3, 4) == color8(1, 2, 3, 4));
        VX_STATIC_CHECK(color8(1, 2, 3, 4) != color8(4, 3, 2, 1));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+color8(10, 20, 30, 40) == color8(10, 20, 30, 40));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++color8(1, 2, 3, 4) == color8(2, 3, 4, 5));
        VX_STATIC_CHECK(--color8(1, 2, 3, 4) == color8(0, 1, 2, 3));

        color8 v(1, 2, 3, 4);
        VX_CHECK(v++ == color8(1, 2, 3, 4));
        VX_CHECK(v == color8(2, 3, 4, 5));
        VX_CHECK(v-- == color8(2, 3, 4, 5));
        VX_CHECK(v == color8(1, 2, 3, 4));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition with float
        VX_CHECK(color8(10, 20, 30, 40) + 5 == color8(15, 25, 35, 45));
        VX_CHECK(5 + color8(10, 20, 30, 40) == color8(15, 25, 35, 45));
        VX_CHECK(color8(5, 5, 5, 5) + color8(1, 2, 3, 4) == color8(6, 7, 8, 9));

        // subtraction with float
        VX_CHECK(color8(10, 20, 30, 40) - 5 == color8(5, 15, 25, 35));
        VX_CHECK(100 - color8(10, 20, 30, 40) == color8(90, 80, 70, 60));
        VX_CHECK(color8(5, 5, 5, 5) - color8(1, 2, 3, 4) == color8(4, 3, 2, 1));

        // multiplication with float
        VX_CHECK(color8(10, 20, 30, 40) * 0.5f == color8(5, 10, 15, 20));
        VX_CHECK(2.0f * color8(10, 20, 30, 40) == color8(20, 40, 60, 80));
        VX_CHECK(color8(2, 2, 2, 2) * color8(3, 4, 5, 6) == color8(6, 8, 10, 12));

        // division with float
        VX_CHECK(color8(100, 50, 25, 10) / 5.0f == color8(20, 10, 5, 2));
        VX_CHECK(200.0f / color8(100, 50, 25, 10) == color8(2, 4, 8, 20));
        VX_CHECK(color8(100, 100, 100, 100) / color8(2, 4, 5, 10) == color8(50, 25, 20, 10));
    }

    VX_SECTION("compound arithmetic operators")
    {
        // +=
        {
            color8 v(1, 2, 3, 4);
            v += 5;
            VX_CHECK(v == color8(6, 7, 8, 9));
            v += color8(10, 20, 30, 40);
            VX_CHECK(v == color8(16, 27, 38, 49));
        }

        // -=
        {
            color8 v(50, 60, 70, 80);
            v -= 10;
            VX_CHECK(v == color8(40, 50, 60, 70));
            v -= color8(5, 10, 15, 20);
            VX_CHECK(v == color8(35, 40, 45, 50));
        }

        // *=
        {
            color8 v(10, 20, 30, 40);
            v *= 0.5f;
            VX_CHECK(v == color8(5, 10, 15, 20));
            v *= color8(1, 2, 3, 4);
            VX_CHECK(v == color8(5, 20, 45, 80));
        }

        // /=
        {
            color8 v(100, 200, 250, 255);
            v /= 2.0f;
            VX_CHECK(v == color8(50, 100, 125, 127));
            v /= color8(1, 2, 5, 1);
            VX_CHECK(v == color8(50, 50, 25, 127));
        }
    }

    VX_SECTION("rgb")
    {
        constexpr color8 c(10, 20, 30, 255);
        VX_CHECK_EQ(c.rgb(), vec3i(10, 20, 30));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}