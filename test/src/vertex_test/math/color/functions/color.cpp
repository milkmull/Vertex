#include "vertex_test/test.hpp"

#include "vertex/math/color/functions/color.hpp"
#include "vertex/math/color/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_luminance)
{
    VX_STATIC_CHECK(luminance(color::black()) == 0.0f);
    VX_STATIC_CHECK(luminance(color::white()) == 1.0f);
    VX_STATIC_CHECK(luminance(color::red()) == 0.2126f);
    VX_STATIC_CHECK(luminance(color::green()) == 0.7152f);
    VX_STATIC_CHECK(luminance(color::blue()) == 0.0722f);
    VX_STATIC_CHECK_EQ(luminance(color::gray()), 0.5f);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_lighten_darken)
{
    constexpr color input(0.2f, 0.4f, 0.6f, 1.0f);

    // move halfway to white
    const color light50 = lighten(input, 0.5f);
    VX_CHECK_EQ(light50, color(
        0.2f + (1.0f - 0.2f) * 0.5f,
        0.4f + (1.0f - 0.4f) * 0.5f,
        0.6f + (1.0f - 0.6f) * 0.5f,
        1.0f
    ));

    // move halfway to black
    const color dark50 = darken(input, 0.5f);
    VX_CHECK_EQ(dark50, color(
        0.2f * 0.5f,
        0.4f * 0.5f,
        0.6f * 0.5f,
        1.0f
    ));

    VX_STATIC_CHECK_EQ(lighten(color(0.0f), 1.0f), color(1.0f));
    VX_STATIC_CHECK_EQ(darken(color(1.0f), 1.0f), color(0.0f));
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_invert)
{
    using vx::math::invert;

    // float version
    {
        constexpr color input(0.2f, 0.4f, 0.6f, 0.8f);
        constexpr color result = invert(input);  // subtract from 1.0
        VX_STATIC_CHECK_EQ(result, color(0.8f, 0.6f, 0.4f, 0.8f));
    }

    // uint8_t version
    {
        constexpr color8 input(10, 20, 30, 40);
        constexpr color8 result = invert(input);  // subtract from 255
        VX_STATIC_CHECK_EQ(result, color8(245, 235, 225, 40));
    }

    VX_STATIC_CHECK_EQ(invert(color(0.0f)), color(1.0f));
    VX_STATIC_CHECK_EQ(invert(color(1.0f)), color(0.0f));
    VX_STATIC_CHECK_EQ(invert(color8(0)), color8(255));
    VX_STATIC_CHECK_EQ(invert(color8(255)), color8(0));
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}