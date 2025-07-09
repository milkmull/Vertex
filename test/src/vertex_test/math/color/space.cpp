#include "vertex_test/test.hpp"

#include "vertex/math/color/space/srgb.hpp"
#include "vertex/math/color/space/hsv.hpp"
#include "vertex/math/color/space/hsl.hpp"
#include "vertex/math/color/functions/comparison.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_srgb_conversion)
{
    VX_SECTION("Round-trip identity (sRGB -> linear -> sRGB)")
    {
        const color s1(0.0f, 0.5f, 1.0f, 1.0f);
        const color s2 = linear_to_srgb(srgb_to_linear(s1));
        VX_CHECK_EQ(s1, s2);
    }

    VX_SECTION("Round-trip identity (linear -> sRGB -> linear)")
    {
        const color l1(0.0f, 0.5f, 1.0f, 1.0f);
        const color l2 = srgb_to_linear(linear_to_srgb(l1));
        VX_CHECK_EQ(l1, l2);
    }

    VX_SECTION("Known edge values (sRGB -> linear)")
    {
        // Values from sRGB transfer function spec
        VX_CHECK_EQ(srgb_to_linear(color(0.0f)).r, 0.0f);
        VX_CHECK_EQ(srgb_to_linear(color(0.04045f)).r, 0.0031308f);
        VX_CHECK_EQ(srgb_to_linear(color(1.0f)).r, 1.0f);
    }

    VX_SECTION("Known edge values (linear -> sRGB)")
    {
        // Values from inverse transfer function spec
        VX_CHECK_EQ(linear_to_srgb(color(0.0f)).r, 0.0f);
        VX_CHECK_EQ(linear_to_srgb(color(0.0031308f)).r, 0.04045f);
        VX_CHECK_EQ(linear_to_srgb(color(1.0f)).r, 1.0f);
    }

    VX_SECTION("Clamp behavior")
    {
        const color c(-1.0f, 0.5f, 2.0f, 0.75f);
        const color clamped_srgb = linear_to_srgb(c);
        VX_CHECK(clamped_srgb.r >= 0.0f && clamped_srgb.r <= 1.0f);
        VX_CHECK(clamped_srgb.g >= 0.0f && clamped_srgb.g <= 1.0f);
        VX_CHECK(clamped_srgb.b >= 0.0f && clamped_srgb.b <= 1.0f);
        VX_CHECK(clamped_srgb.a == c.a); // Alpha should remain unchanged
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_hsv_rgb_conversion)
{
    VX_SECTION("HSV to RGB - Primary Colors")
    {
        // Red (H = 0)
        VX_CHECK_EQ(hsv_to_rgb(0.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f));

        // Green (H = 120)
        VX_CHECK_EQ(hsv_to_rgb(120.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));

        // Blue (H = 240)
        VX_CHECK_EQ(hsv_to_rgb(240.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));

        // Yellow (H = 60)
        VX_CHECK_EQ(hsv_to_rgb(60.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 0.0f));

        // Cyan (H = 180)
        VX_CHECK_EQ(hsv_to_rgb(180.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f));

        // Magenta (H = 300)
        VX_CHECK_EQ(hsv_to_rgb(300.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f));
    }

    VX_SECTION("RGB to HSV - Primary Colors")
    {
        // Red
        VX_CHECK_EQ(rgb_to_hsv(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f));

        // Green
        VX_CHECK_EQ(rgb_to_hsv(0.0f, 1.0f, 0.0f), vec3(120.0f, 1.0f, 1.0f));

        // Blue
        VX_CHECK_EQ(rgb_to_hsv(0.0f, 0.0f, 1.0f), vec3(240.0f, 1.0f, 1.0f));

        // White (no saturation)
        VX_CHECK_EQ(rgb_to_hsv(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));

        // Black (zero value)
        VX_CHECK_EQ(rgb_to_hsv(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));
    }

    VX_SECTION("Round-trip RGB -> HSV -> RGB")
    {
        const vec3 rgb_in(0.2f, 0.5f, 0.8f);
        const vec3 hsv = rgb_to_hsv(rgb_in.x, rgb_in.y, rgb_in.z);
        const vec3 rgb_out = hsv_to_rgb(hsv.x, hsv.y, hsv.z);
        VX_CHECK_EQ(rgb_in, rgb_out);
    }

    VX_SECTION("Round-trip HSV -> RGB -> HSV")
    {
        const vec3 hsv_in(280.0f, 0.5f, 0.75f);
        const vec3 rgb = hsv_to_rgb(hsv_in.x, hsv_in.y, hsv_in.z);
        const vec3 hsv_out = rgb_to_hsv(rgb.x, rgb.y, rgb.z);
        VX_CHECK(equal_approx(hsv_in, hsv_out, 1e-4f));
    }

    VX_SECTION("Saturation 0 (gray shades)")
    {
        const vec3 rgb = hsv_to_rgb(123.0f, 0.0f, 0.6f);
        VX_CHECK_EQ(rgb, vec3(0.6f));

        const vec3 hsv = rgb_to_hsv(0.6f, 0.6f, 0.6f);
        VX_CHECK_EQ(hsv, vec3(0.0f, 0.0f, 0.6f));
    }

    VX_SECTION("Clamp edge behavior")
    {
        const vec3 rgb = hsv_to_rgb(-30.0f, 2.0f, -1.0f); // Should clamp to (0°, 1.0, 0.0)
        VX_CHECK_EQ(rgb, vec3(0.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_hsl_rgb_conversion)
{
    VX_SECTION("HSL to RGB - Primary Hues")
    {
        VX_CHECK_EQ(hsl_to_rgb(0.0f, 1.0f, 0.5f), vec3(1.0f, 0.0f, 0.0f));    // Red
        VX_CHECK_EQ(hsl_to_rgb(120.0f, 1.0f, 0.5f), vec3(0.0f, 1.0f, 0.0f));  // Green
        VX_CHECK_EQ(hsl_to_rgb(240.0f, 1.0f, 0.5f), vec3(0.0f, 0.0f, 1.0f));  // Blue
    }

    VX_SECTION("RGB to HSL - Primary Hues")
    {
        VX_CHECK_EQ(rgb_to_hsl(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.5f));     // Red
        VX_CHECK_EQ(rgb_to_hsl(0.0f, 1.0f, 0.0f), vec3(120.0f, 1.0f, 0.5f));   // Green
        VX_CHECK_EQ(rgb_to_hsl(0.0f, 0.0f, 1.0f), vec3(240.0f, 1.0f, 0.5f));   // Blue
    }

    VX_SECTION("Desaturated (grayscale)")
    {
        VX_CHECK_EQ(hsl_to_rgb(60.0f, 0.0f, 0.5f), vec3(0.5f));  // Gray
        VX_CHECK_EQ(rgb_to_hsl(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.5f));  // Gray
    }

    VX_SECTION("Round-trip RGB -> HSL -> RGB")
    {
        const vec3 rgb_in(0.25f, 0.75f, 0.5f);
        const vec3 hsl = rgb_to_hsl(rgb_in.x, rgb_in.y, rgb_in.z);
        const vec3 rgb_out = hsl_to_rgb(hsl.x, hsl.y, hsl.z);
        VX_CHECK_EQ(rgb_in, rgb_out);
    }

    VX_SECTION("Round-trip HSL -> RGB -> HSL")
    {
        const vec3 hsl_in(300.0f, 0.5f, 0.6f);
        const vec3 rgb = hsl_to_rgb(hsl_in.x, hsl_in.y, hsl_in.z);
        const vec3 hsl_out = rgb_to_hsl(rgb.x, rgb.y, rgb.z);
        VX_CHECK(equal_approx(hsl_in, hsl_out, 1e-4f));
    }

    VX_SECTION("Clamp edge behavior")
    {
        const vec3 rgb = hsl_to_rgb(-60.0f, 5.0f, -1.0f);  // Should clamp to (0°, 1.0, 0.0)
        VX_CHECK_EQ(rgb, vec3(0.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}