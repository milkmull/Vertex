#include "vertex_test/test.hpp"

#include "vertex/math/color/functions/common.hpp"
#include "vertex/math/color/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_min)
{
    VX_SECTION("color")
    {
        constexpr color a(-9, 4, -6, 2);
        constexpr color b(3, -5, 7, -8);

        constexpr color r1 = min(a, b);
        VX_STATIC_CHECK_EQ(r1, color(-9, -5, -6, -8));

        constexpr color r2 = min(a, -6.0f);
        VX_STATIC_CHECK_EQ(r2, color(-9, -6, -6, -6));
    }

    VX_SECTION("color8")
    {
        constexpr color8 a(9, 4, 6, 2);
        constexpr color8 b(3, 5, 7, 8);

        constexpr color8 r1 = min(a, b);
        VX_STATIC_CHECK_EQ(r1, color8(3, 4, 6, 2));

        constexpr color8 r2 = min(a, static_cast<u8>(5));
        VX_STATIC_CHECK_EQ(r2, color8(5, 4, 5, 2));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_max)
{
    VX_SECTION("color")
    {
        constexpr color a(-9, 4, -6, 2);
        constexpr color b(3, -5, 7, -8);

        constexpr color r1 = max(a, b);
        VX_STATIC_CHECK_EQ(r1, color(3, 4, 7, 2));

        constexpr color r2 = max(a, -6.0f);
        VX_STATIC_CHECK_EQ(r2, color(-6, 4, -6, 2));
    }

    VX_SECTION("color8")
    {
        constexpr color8 a(9, 4, 6, 2);
        constexpr color8 b(3, 5, 7, 8);

        constexpr color8 r1 = max(a, b);
        VX_STATIC_CHECK_EQ(r1, color8(9, 5, 7, 8));

        constexpr color8 r2 = max(a, static_cast<u8>(5));
        VX_STATIC_CHECK_EQ(r2, color8(9, 5, 6, 5));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cmin)
{
    VX_SECTION("color")
    {
        constexpr color b(-5.5f, 3.3f, -8.8f, 4.4f);
        constexpr f32 r2 = cmin(b);
        VX_STATIC_CHECK_EQ(r2, -8.8f);
    }

    VX_SECTION("color8")
    {
        constexpr color8 c(5, 3, 8, 4);
        constexpr u8 r = cmin(c);
        VX_STATIC_CHECK_EQ(r, static_cast<u8>(3));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_cmax)
{
    VX_SECTION("color")
    {
        constexpr color b(-5.5f, 3.3f, -8.8f, 4.4f);
        constexpr f32 r2 = cmax(b);
        VX_STATIC_CHECK_EQ(r2, 3.3f);
    }

    VX_SECTION("color8")
    {
        constexpr color8 c(5, 3, 8, 4);
        constexpr u8 r = cmax(c);
        VX_STATIC_CHECK_EQ(r, static_cast<u8>(8));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_clamp)
{
    VX_SECTION("color (scalar)")
    {
        const color a(3.5f, 2.5f, 4.0f, 0.5f);
        const color b = clamp(a, 1.0f, 3.0f); // (3.5, 2.5, 4.0, 0.5) -> (3.0, 2.5, 3.0, 1.0)
        VX_CHECK_EQ(b, color(3.0f, 2.5f, 3.0f, 1.0f));

        const color c(0.5f, 2.0f, 1.5f, 4.5f);
        const color d = clamp(c, 1.0f, 3.0f); // (0.5, 2.0, 1.5, 4.5) -> (1.0, 2.0, 1.5, 3.0)
        VX_CHECK_EQ(d, color(1.0f, 2.0f, 1.5f, 3.0f));
    }

    VX_SECTION("color (trinary)")
    {
        const color a(3.5f, 2.5f, 4.0f, 0.5f);
        const color min(1.0f, 1.0f, 2.0f, 0.0f);
        const color max(3.0f, 3.0f, 3.0f, 2.0f);
        const color b = clamp(a, min, max); // (3.5, 2.5, 4.0, 0.5) -> (3.0, 2.5, 3.0, 0.5)
        VX_CHECK_EQ(b, color(3.0f, 2.5f, 3.0f, 0.5f));

        const color c(0.5f, 2.0f, 1.5f, 4.5f);
        const color d = clamp(c, min, max); // (0.5, 2.0, 1.5, 4.5) -> (1.0, 2.0, 2.0, 2.0)
        VX_CHECK_EQ(d, color(1.0f, 2.0f, 2.0f, 2.0f));
    }

    VX_SECTION("color8 (scalar)")
    {
        const color8 a(255, 100, 0, 128);
        const color8 b = clamp(a, static_cast<u8>(50), static_cast<u8>(200));
        VX_CHECK_EQ(b, color8(200, 100, 50, 128));

        const color8 c(10, 240, 130, 220);
        const color8 d = clamp(c, static_cast<u8>(50), static_cast<u8>(180));
        VX_CHECK_EQ(d, color8(50, 180, 130, 180));
    }

    VX_SECTION("color8 (trinary)")
    {
        const color8 a(200, 100, 0, 250);
        const color8 min(128, 64, 10, 32);
        const color8 max(180, 120, 40, 200);
        const color8 b = clamp(a, min, max); // -> (180, 100, 10, 200)
        VX_CHECK_EQ(b, color8(180, 100, 10, 200));

        const color8 c(10, 240, 130, 220);
        const color8 d = clamp(c, min, max); // -> (128, 120, 40, 200)
        VX_CHECK_EQ(d, color8(128, 120, 40, 200));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}