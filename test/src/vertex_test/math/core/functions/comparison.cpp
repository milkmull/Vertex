#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_equal)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK(!equal(true, false));
        VX_STATIC_CHECK(equal(1, 1));
        VX_STATIC_CHECK(!equal(1u, 2u));
        VX_STATIC_CHECK(equal(1.5f, 1.5f));
        VX_STATIC_CHECK(!equal(-1.5, 1.5));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_equal)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 1, 2 };
        constexpr vec2i b{ 1, 2 };
        constexpr vec2i c{ 1, 3 };

        VX_STATIC_CHECK(each_equal(a, b) == vec2b(true, true));
        VX_STATIC_CHECK(each_equal(a, c) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 2.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 3.0f };
        constexpr vec3f c{ 1.0f, 2.0f, 4.0f };

        VX_STATIC_CHECK(each_equal(a, b) == vec3b(true, true, true));
        VX_STATIC_CHECK(each_equal(a, c) == vec3b(true, true, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d c{ 1.0, 2.0, 0.0, 4.0 };

        VX_STATIC_CHECK(each_equal(a, b) == vec4b(true, true, true, true));
        VX_STATIC_CHECK(each_equal(a, c) == vec4b(true, true, false, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_not_equal)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK(not_equal(true, false));
        VX_STATIC_CHECK(!not_equal(1, 1));
        VX_STATIC_CHECK(not_equal(1u, 2u));
        VX_STATIC_CHECK(!not_equal(1.5f, 1.5f));
        VX_STATIC_CHECK(not_equal(-1.5, 1.5));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_not_equal)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 1, 2 };
        constexpr vec2i b{ 1, 2 };
        constexpr vec2i c{ 1, 3 };

        VX_STATIC_CHECK(each_not_equal(a, b) == vec2b(false, false));
        VX_STATIC_CHECK(each_not_equal(a, c) == vec2b(false, true));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 2.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 3.0f };
        constexpr vec3f c{ 1.0f, 2.0f, 4.0f };

        VX_STATIC_CHECK(each_not_equal(a, b) == vec3b(false, false, false));
        VX_STATIC_CHECK(each_not_equal(a, c) == vec3b(false, false, true));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d c{ 1.0, 2.0, 0.0, 4.0 };

        VX_STATIC_CHECK(each_not_equal(a, b) == vec4b(false, false, false, false));
        VX_STATIC_CHECK(each_not_equal(a, c) == vec4b(false, false, true, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_greater)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK(!greater(1, 1));
        VX_STATIC_CHECK(greater(2, 1));
        VX_STATIC_CHECK(!greater(1u, 2u));
        VX_STATIC_CHECK(greater(2.5f, 2.0f));
        VX_STATIC_CHECK(!greater(-1.5, 1.5));
        VX_STATIC_CHECK(greater(0, -1));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_greater)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 2, 3 };
        constexpr vec2i b{ 1, 3 };
        constexpr vec2i c{ 0, 4 };

        VX_STATIC_CHECK(each_greater(a, b) == vec2b(true, false));
        VX_STATIC_CHECK(each_greater(a, c) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 4.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_STATIC_CHECK(each_greater(a, b) == vec3b(false, true, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 0.0, 2.0, 2.0, 5.0 };

        VX_STATIC_CHECK(each_greater(a, b) == vec4b(true, false, true, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_less)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK(!less(1, 1));
        VX_STATIC_CHECK(!less(2, 1));
        VX_STATIC_CHECK(less(1u, 2u));
        VX_STATIC_CHECK(!less(2.5f, 2.0f));
        VX_STATIC_CHECK(less(-1.5, 1.5));
        VX_STATIC_CHECK(!less(0, -1));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_less)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 2, 3 };
        constexpr vec2i b{ 1, 3 };
        constexpr vec2i c{ 0, 4 };

        VX_STATIC_CHECK(each_less(a, b) == vec2b(false, false));
        VX_STATIC_CHECK(each_less(a, c) == vec2b(false, true));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 4.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_STATIC_CHECK(each_less(a, b) == vec3b(false, false, true));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 0.0, 2.0, 2.0, 5.0 };

        VX_STATIC_CHECK(each_less(a, b) == vec4b(false, false, false, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_greater_or_equal)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK(greater_or_equal(1, 1));
        VX_STATIC_CHECK(greater_or_equal(2, 1));
        VX_STATIC_CHECK(!greater_or_equal(1u, 2u));
        VX_STATIC_CHECK(greater_or_equal(2.5f, 2.0f));
        VX_STATIC_CHECK(!greater_or_equal(-1.5, 1.5));
        VX_STATIC_CHECK(greater_or_equal(0, -1));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_greater_or_equal)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 2, 3 };
        constexpr vec2i b{ 1, 3 };
        constexpr vec2i c{ 2, 4 };

        VX_STATIC_CHECK(each_greater_or_equal(a, b) == vec2b(true, true));
        VX_STATIC_CHECK(each_greater_or_equal(a, c) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 4.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_STATIC_CHECK(each_greater_or_equal(a, b) == vec3b(true, true, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 0.0, 2.0, 2.0, 4.0 };

        VX_STATIC_CHECK(each_greater_or_equal(a, b) == vec4b(true, true, true, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_less_or_equal)
{
    VX_SECTION("scalar")
    {
        VX_STATIC_CHECK(less_or_equal(1, 1));
        VX_STATIC_CHECK(!less_or_equal(2, 1));
        VX_STATIC_CHECK(less_or_equal(1u, 2u));
        VX_STATIC_CHECK(!less_or_equal(2.5f, 2.0f));
        VX_STATIC_CHECK(less_or_equal(-1.5, 1.5));
        VX_STATIC_CHECK(!less_or_equal(0, -1));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_less_or_equal)
{
    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 2, 3 };
        constexpr vec2i b{ 1, 3 };
        constexpr vec2i c{ 2, 4 };

        VX_STATIC_CHECK(each_less_or_equal(a, b) == vec2b(false, true));
        VX_STATIC_CHECK(each_less_or_equal(a, c) == vec2b(true, true));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 4.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_STATIC_CHECK(each_less_or_equal(a, b) == vec3b(true, false, true));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 0.0, 2.0, 2.0, 4.0 };

        VX_STATIC_CHECK(each_less_or_equal(a, b) == vec4b(false, true, false, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_inf)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(is_inf(constants<f32>::infinity));
        VX_STATIC_CHECK(is_inf(-constants<f32>::infinity));
        VX_STATIC_CHECK(!is_inf(0.0f));
        VX_STATIC_CHECK(!is_inf(1.0));
        VX_STATIC_CHECK(!is_inf(constants<f32>::nan)); // NaN is not infinite

        // int
        VX_STATIC_CHECK(!is_inf(0));
        VX_STATIC_CHECK(!is_inf(123));
        VX_STATIC_CHECK(!is_inf(-456));
    }

    VX_SECTION("vec2")
    {
        constexpr vec2f a{ constants<f32>::infinity, 1.0f };
        constexpr vec2f b{ -constants<f32>::infinity, constants<f32>::nan };
        constexpr vec2f c{ 0.0f, 42.0f };

        VX_STATIC_CHECK(is_inf(a));
        VX_STATIC_CHECK(is_inf(b));
        VX_STATIC_CHECK(!is_inf(c));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3d a{ constants<f64>::infinity, 2.0, 3.0 };
        constexpr vec3d b{ 1.0, 2.0, constants<f64>::nan };
        constexpr vec3d c{ 0.0, 42.0, -100.0 };

        VX_STATIC_CHECK(is_inf(a));
        VX_STATIC_CHECK(!is_inf(b));
        VX_STATIC_CHECK(!is_inf(c));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 1.0f, constants<f32>::infinity, 0.0f, 4.0f };
        constexpr vec4f b{ -constants<f32>::infinity, 2.0f, constants<f32>::nan, 3.0f };
        constexpr vec4f c{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_STATIC_CHECK(is_inf(a));
        VX_STATIC_CHECK(is_inf(b));
        VX_STATIC_CHECK(!is_inf(c));
    }

    VX_SECTION("int vec")
    {
        constexpr vec2i vi{ 1, 2 };
        constexpr vec3i vi3{ 0, -1, 123 };
        constexpr vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_STATIC_CHECK(!is_inf(vi));
        VX_STATIC_CHECK(!is_inf(vi3));
        VX_STATIC_CHECK(!is_inf(vu4));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_is_inf)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ constants<f32>::infinity, 1.0f };
        constexpr vec2f b{ -constants<f32>::infinity, constants<f32>::infinity };
        constexpr vec2f c{ 0.0f, 42.0f };

        VX_STATIC_CHECK(each_is_inf(a) == vec2b(true, false));
        VX_STATIC_CHECK(each_is_inf(b) == vec2b(true, true));
        VX_STATIC_CHECK(each_is_inf(c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3d a{ 0.0, constants<f64>::infinity, -constants<f64>::infinity };
        constexpr vec3d b{ 1.0, 2.0, 3.0 };

        VX_STATIC_CHECK(each_is_inf(a) == vec3b(false, true, true));
        VX_STATIC_CHECK(each_is_inf(b) == vec3b(false, false, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 0.0f, constants<f32>::infinity, 1.0f, -constants<f32>::infinity };
        constexpr vec4f b{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_STATIC_CHECK(each_is_inf(a) == vec4b(false, true, false, true));
        VX_STATIC_CHECK(each_is_inf(b) == vec4b(false, false, false, false));
    }

    VX_SECTION("int vec")
    {
        constexpr vec2i vi{ 1, 2 };
        constexpr vec3i vi3{ 0, -1, 123 };
        constexpr vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_STATIC_CHECK(each_is_inf(vi) == vec2b(false, false));
        VX_STATIC_CHECK(each_is_inf(vi3) == vec3b(false, false, false));
        VX_STATIC_CHECK(each_is_inf(vu4) == vec4b(false, false, false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_nan)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(is_nan(constants<f32>::nan));
        VX_STATIC_CHECK(!is_nan(constants<f32>::infinity));
        VX_STATIC_CHECK(!is_nan(-constants<f32>::infinity));
        VX_STATIC_CHECK(!is_nan(0.0f));
        VX_STATIC_CHECK(!is_nan(1.0f));

        // int
        VX_STATIC_CHECK(!is_nan(0));
        VX_STATIC_CHECK(!is_nan(123));
        VX_STATIC_CHECK(!is_nan(-456));
    }

    VX_SECTION("vec2")
    {
        constexpr vec2f a{ constants<f32>::nan, 1.0f };
        constexpr vec2f b{ -constants<f32>::infinity, constants<f32>::nan };
        constexpr vec2f c{ 0.0f, 42.0f };

        VX_STATIC_CHECK(is_nan(a));
        VX_STATIC_CHECK(is_nan(b));
        VX_STATIC_CHECK(!is_nan(c));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3d a{ constants<f64>::nan, 2.0, 3.0 };
        constexpr vec3d b{ 1.0, 2.0, constants<f64>::infinity };
        constexpr vec3d c{ 0.0, 42.0, -100.0 };

        VX_STATIC_CHECK(is_nan(a));
        VX_STATIC_CHECK(!is_nan(b));
        VX_STATIC_CHECK(!is_nan(c));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 1.0f, constants<f32>::nan, 0.0f, 4.0f };
        constexpr vec4f b{ -constants<f32>::infinity, 2.0f, constants<f32>::nan, 3.0f };
        constexpr vec4f c{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_STATIC_CHECK(is_nan(a));
        VX_STATIC_CHECK(is_nan(b));
        VX_STATIC_CHECK(!is_nan(c));
    }

    VX_SECTION("int vec")
    {
        constexpr vec2i vi{ 1, 2 };
        constexpr vec3i vi3{ 0, -1, 123 };
        constexpr vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_STATIC_CHECK(!is_nan(vi));
        VX_STATIC_CHECK(!is_nan(vi3));
        VX_STATIC_CHECK(!is_nan(vu4));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_is_nan)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ constants<f32>::nan, 1.0f };
        constexpr vec2f b{ -constants<f32>::infinity, constants<f32>::nan };
        constexpr vec2f c{ 0.0f, 42.0f };

        VX_STATIC_CHECK(each_is_nan(a) == vec2b(true, false));
        VX_STATIC_CHECK(each_is_nan(b) == vec2b(false, true));
        VX_STATIC_CHECK(each_is_nan(c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3d a{ 0.0, constants<f64>::nan, -constants<f64>::infinity };
        constexpr vec3d b{ 1.0, 2.0, 3.0 };

        VX_STATIC_CHECK(each_is_nan(a) == vec3b(false, true, false));
        VX_STATIC_CHECK(each_is_nan(b) == vec3b(false, false, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 0.0f, constants<f32>::nan, 1.0f, -constants<f32>::infinity };
        constexpr vec4f b{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_STATIC_CHECK(each_is_nan(a) == vec4b(false, true, false, false));
        VX_STATIC_CHECK(each_is_nan(b) == vec4b(false, false, false, false));
    }

    VX_SECTION("int vec")
    {
        constexpr vec2i vi{ 1, 2 };
        constexpr vec3i vi3{ 0, -1, 123 };
        constexpr vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_STATIC_CHECK(each_is_nan(vi) == vec2b(false, false));
        VX_STATIC_CHECK(each_is_nan(vi3) == vec3b(false, false, false));
        VX_STATIC_CHECK(each_is_nan(vu4) == vec4b(false, false, false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_zero_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(is_zero_approx(0.0f));
        VX_STATIC_CHECK(is_zero_approx(0.0000009f));
        VX_STATIC_CHECK(!is_zero_approx(1.0f));
        VX_STATIC_CHECK(is_zero_approx(constants<f32>::epsilon));

        // int
        VX_STATIC_CHECK(is_zero_approx(0));
        VX_STATIC_CHECK(!is_zero_approx(123));
        VX_STATIC_CHECK(!is_zero_approx(-456));
    }

    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 0.0f, 0.0f };       // Exactly zero
        constexpr vec2f b{ 0.0000001f, 0.0f }; // Within epsilon
        constexpr vec2f c{ 1.0f, 0.0f };       // Not all zero approx

        VX_STATIC_CHECK(is_zero_approx(a));
        VX_STATIC_CHECK(is_zero_approx(b));
        VX_STATIC_CHECK(!is_zero_approx(c));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 0.0f, 0.0f, 0.0f };              // Exactly zero
        constexpr vec3f b{ 0.0000001f, 0.0f, 0.0f };        // Approx zero
        constexpr vec3f c{ 1.0f, 0.0f, 0.0f };              // Not all zero approx
        constexpr vec3f d{ 0.0000009f, 0.0000009f, 0.0f };   // Approx zero for all

        VX_STATIC_CHECK(is_zero_approx(a));
        VX_STATIC_CHECK(is_zero_approx(b));
        VX_STATIC_CHECK(!is_zero_approx(c));
        VX_STATIC_CHECK(is_zero_approx(d));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 0.0f, 0.0f, 0.0f, 0.0f };                    // Exactly zero
        constexpr vec4f b{ 0.0000001f, 0.0f, 0.0f, 0.0f };              // Approx zero
        constexpr vec4f c{ 1.0f, 0.0f, 0.0f, 0.0f };                    // Not all zero approx
        constexpr vec4f d{ 0.0000009f, 0.0000009f, 0.0000009f, 0.0f };  // Approx zero for all

        VX_STATIC_CHECK(is_zero_approx(a));
        VX_STATIC_CHECK(is_zero_approx(b));
        VX_STATIC_CHECK(!is_zero_approx(c));
        VX_STATIC_CHECK(is_zero_approx(d));
    }

    VX_SECTION("custom epsilon")
    {
        constexpr float tight = 1e-8f;
        constexpr float loose = 1e-3f;
        VX_STATIC_CHECK(!is_zero_approx(1e-6f, tight));
        VX_STATIC_CHECK(is_zero_approx(1e-6f, loose));

        constexpr vec3f a{ 1e-6f, 0.0f, 0.0f };
        VX_STATIC_CHECK(!is_zero_approx(a, tight));
        VX_STATIC_CHECK(is_zero_approx(a, loose));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_is_zero_approx)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 0.0f, 1.0f };
        constexpr vec2f b{ 0.0000001f, 0.0f };
        constexpr vec2f c{ 1.0f, -1.0f };

        VX_STATIC_CHECK(each_is_zero_approx(a) == vec2b(true, false));
        VX_STATIC_CHECK(each_is_zero_approx(b) == vec2b(true, true));
        VX_STATIC_CHECK(each_is_zero_approx(c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3d a{ 0.0, 0.000000001, 1.0 };
        constexpr vec3d b{ -0.00000005, 2.0, -2.0 };
        constexpr vec3d c{ 0.1, 0.2, 0.3 };

        VX_STATIC_CHECK(each_is_zero_approx(a) == vec3b(true, true, false));
        VX_STATIC_CHECK(each_is_zero_approx(b) == vec3b(true, false, false));
        VX_STATIC_CHECK(each_is_zero_approx(c) == vec3b(false, false, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 0.0f, 0.0000000001f, -0.0000000001f, 1.0f };
        constexpr vec4f b{ 0.0000001f, 2.0f, 0.0f, 4.0f };
        constexpr vec4f c{ 0.1f, 0.2f, 0.3f, 0.4f };

        VX_STATIC_CHECK(each_is_zero_approx(a) == vec4b(true, true, true, false));
        VX_STATIC_CHECK(each_is_zero_approx(b) == vec4b(true, false, true, false));
        VX_STATIC_CHECK(each_is_zero_approx(c) == vec4b(false, false, false, false));
    }

    VX_SECTION("int vec")
    {
        constexpr vec2i vi{ 0, 0 };
        constexpr vec3i vi3{ 0, 123, -1 };
        constexpr vec4u vu4{ 0u, 0u, 0u, 0u };

        VX_STATIC_CHECK(each_is_zero_approx(vi) == vec2b(true, true));
        VX_STATIC_CHECK(each_is_zero_approx(vi3) == vec3b(true, false, false));
        VX_STATIC_CHECK(each_is_zero_approx(vu4) == vec4b(true, true, true, true));
    }

    VX_SECTION("custom epsilon")
    {
        constexpr float tight = 1e-8f;
        constexpr float loose = 1e-3f;
        constexpr vec2f v{ 1e-6f, 0.0f };

        VX_STATIC_CHECK(each_is_zero_approx(v, tight) == vec2b(false, true));
        VX_STATIC_CHECK(each_is_zero_approx(v, loose) == vec2b(true, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_equal_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(equal_approx(1.0f, 1.0f));
        VX_STATIC_CHECK(equal_approx(1.0f, 1.0000009f));
        VX_STATIC_CHECK(!equal_approx(1.0f, 1.00001f));
        VX_STATIC_CHECK(equal_approx(0.0f, constants<f32>::epsilon));

        // int
        VX_STATIC_CHECK(equal_approx(3, 3));
        VX_STATIC_CHECK(!equal_approx(3, 4));
    }

    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 1.0f, 2.0f };
        constexpr vec2f b{ 1.0f, 2.0f };
        constexpr vec2f c{ 1.0f, 2.0000001f };
        constexpr vec2f d{ 1.0f, 3.0f };
        constexpr vec2f e{ 1.0000001f, 2.0000001f };

        VX_STATIC_CHECK(equal_approx(a, b));
        VX_STATIC_CHECK(equal_approx(a, c));
        VX_STATIC_CHECK(!equal_approx(a, d));
        VX_STATIC_CHECK(equal_approx(a, e));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ -1.0f, 0.0f, 1.0f };
        constexpr vec3f b{ -1.0f, 0.0f, 1.0f };
        constexpr vec3f c{ -1.0000001f, 0.0f, 1.0000001f };
        constexpr vec3f d{ -1.0f, 0.0f, 2.0f };

        VX_STATIC_CHECK(equal_approx(a, b));
        VX_STATIC_CHECK(equal_approx(a, c));
        VX_STATIC_CHECK(!equal_approx(a, d));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 5.0f, 6.0f, 7.0f, 8.0f };
        constexpr vec4f b{ 5.0f, 6.0f, 7.0f, 8.0f };
        constexpr vec4f c{ 5.0000001f, 6.0f, 7.0000001f, 8.0f };
        constexpr vec4f d{ 5.0f, 6.0f, 7.0f, 9.0f };

        VX_STATIC_CHECK(equal_approx(a, b));
        VX_STATIC_CHECK(equal_approx(a, c));
        VX_STATIC_CHECK(!equal_approx(a, d));
    }

    VX_SECTION("custom epsilon")
    {
        constexpr float tight = 1e-8f;
        constexpr float loose = 1e-3f;

        // vec3 with small difference
        constexpr vec3f a{ 1.0f, 2.0f, 3.0f };
        constexpr vec3f b{ 1.00001f, 2.00001f, 3.00001f };

        VX_STATIC_CHECK(!equal_approx(a, b, tight));
        VX_STATIC_CHECK(equal_approx(a, b, loose));

        // Scalar double with tight epsilon
        constexpr double x = 5.0;
        constexpr double y = 5.0000000001;

        VX_STATIC_CHECK(!equal_approx(x, y, 1e-12));
        VX_STATIC_CHECK(equal_approx(x, y, 1e-9));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_equal_approx)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 1.0f, 2.0f };
        constexpr vec2f b{ 1.0000009f, 2.0f };
        constexpr vec2f c{ 1.00001f, 2.00001f };

        VX_STATIC_CHECK(each_equal_approx(a, b) == vec2b(true, true));
        VX_STATIC_CHECK(each_equal_approx(a, c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 0.0f, 1.0f, 2.0f };
        constexpr vec3f b{ 0.0f, 1.0000009f, 2.0f };
        constexpr vec3f c{ 0.0f, 1.00001f, 2.0f };

        VX_STATIC_CHECK(each_equal_approx(a, b) == vec3b(true, true, true));
        VX_STATIC_CHECK(each_equal_approx(a, c) == vec3b(true, false, true));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 0.0, 1.0, 2.0, 3.0 };
        constexpr vec4d b{ 0.0, 1.0000009, 2.0, 3.0000009 };
        constexpr vec4d c{ 0.0, 1.00001, 2.0, 3.0 };

        VX_STATIC_CHECK(each_equal_approx(a, b) == vec4b(true, true, true, true));
        VX_STATIC_CHECK(each_equal_approx(a, c) == vec4b(true, false, true, true));
    }

    VX_SECTION("int vec")
    {
        constexpr vec2i vi1{ 1, 2 };
        constexpr vec2i vi2{ 1, 2 };
        constexpr vec2i vi3{ 1, 3 };

        VX_STATIC_CHECK(each_equal_approx(vi1, vi2) == vec2b(true, true));
        VX_STATIC_CHECK(each_equal_approx(vi1, vi3) == vec2b(true, false));
    }

    VX_SECTION("custom epsilon")
    {
        constexpr float eps_strict = 1e-8f;
        constexpr float eps_loose = 1e-3f;

        // vec2 with small difference
        constexpr vec2f a{ 1.0f, 2.0f };
        constexpr vec2f b{ 1.0000005f, 2.0000005f };

        VX_STATIC_CHECK(each_equal_approx(a, b, eps_strict) == vec2b(false, false));
        VX_STATIC_CHECK(each_equal_approx(a, b) == vec2b(true, true));
        VX_STATIC_CHECK(each_equal_approx(a, b, eps_loose) == vec2b(true, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_greater_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(greater_approx(1.00001f, 1.0f));
        VX_STATIC_CHECK(!greater_approx(1.0000001f, 1.0f));
        VX_STATIC_CHECK(!greater_approx(1.0f, 1.0f));
        VX_STATIC_CHECK(!greater_approx(0.99999f, 1.0f));
        VX_STATIC_CHECK(!greater_approx(0.0f, constants<f32>::epsilon));

        // int
        VX_STATIC_CHECK(greater_approx(2, 1));
        VX_STATIC_CHECK(!greater_approx(1, 1));
        VX_STATIC_CHECK(!greater_approx(0, 1));
    }

    VX_SECTION("custom epsilon")
    {
        constexpr float tight = 1e-8f;
        constexpr float loose = 1e-2f;

        // scalar
        VX_STATIC_CHECK(greater_approx(1.001f, 1.0f, tight));
        VX_STATIC_CHECK(!greater_approx(1.001f, 1.0f, loose));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_greater_approx)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 1.00001f, 0.5f };
        constexpr vec2f b{ 1.0f, 0.5f };
        VX_STATIC_CHECK(each_greater_approx(a, b) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.00001f, 2.0f, 2.999f };
        constexpr vec3f b{ 1.0f, 2.0f, 3.0f };
        VX_STATIC_CHECK(each_greater_approx(a, b) == vec3b(true, false, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 1.0f, 2.00001f, 2.0f, 3.0001f };
        constexpr vec4f b{ 1.0f, 2.0f, 2.0f, 3.0f };
        VX_STATIC_CHECK(each_greater_approx(a, b) == vec4b(false, true, false, true));
    }

    VX_SECTION("custom epsilon")
    {
        constexpr float tight = 1e-8f;
        constexpr float loose = 1e-2f;

        constexpr vec2f a{ 1.001f, 1.0f };
        constexpr vec2f b{ 1.0f, 1.0f };

        VX_STATIC_CHECK(each_greater_approx(a, b, tight) == vec2b(true, false));
        VX_STATIC_CHECK(each_greater_approx(a, b, loose) == vec2b(false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_less_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(less_approx(0.99999f, 1.0f));
        VX_STATIC_CHECK(!less_approx(0.9999999f, 1.0f));
        VX_STATIC_CHECK(!less_approx(1.0f, 1.0f));
        VX_STATIC_CHECK(!less_approx(1.00001f, 1.0f));
        VX_STATIC_CHECK(!less_approx(0.0f, constants<f32>::epsilon));

        // int
        VX_STATIC_CHECK(less_approx(0, 1));
        VX_STATIC_CHECK(!less_approx(1, 1));
        VX_STATIC_CHECK(!less_approx(2, 1));
    }

    VX_SECTION("custom epsilon")
    {
        constexpr float tight = 1e-8f;
        constexpr float loose = 1e-2f;

        VX_STATIC_CHECK(less_approx(0.999f, 1.0f, tight));
        VX_STATIC_CHECK(!less_approx(0.999f, 1.0f, loose));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_less_approx)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 0.99999f, 0.5f };
        constexpr vec2f b{ 1.0f, 0.5f };
        VX_STATIC_CHECK(each_less_approx(a, b) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 0.99999f, 2.0f, 2.999f };
        constexpr vec3f b{ 1.0f, 2.0f, 3.0f };
        VX_STATIC_CHECK(each_less_approx(a, b) == vec3b(true, false, true));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 1.0f, 1.99999f, 2.0f, 2.999f };
        constexpr vec4f b{ 1.0f, 2.0f, 2.0f, 3.0f };
        VX_STATIC_CHECK(each_less_approx(a, b) == vec4b(false, true, false, true));
    }

    VX_SECTION("custom epsilon")
    {
        constexpr float tight = 1e-8f;
        constexpr float loose = 1e-2f;

        constexpr vec2f a{ 0.999f, 1.0f };
        constexpr vec2f b{ 1.0f, 1.0f };

        VX_STATIC_CHECK(each_less_approx(a, b, tight) == vec2b(true, false));
        VX_STATIC_CHECK(each_less_approx(a, b, loose) == vec2b(false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_greater_or_equal_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(greater_or_equal_approx(1.000001f, 1.0f));
        VX_STATIC_CHECK(greater_or_equal_approx(1.0f, 1.0f));
        VX_STATIC_CHECK(!greater_or_equal_approx(-0.9999991f, 1.0f));
        VX_STATIC_CHECK(greater_or_equal_approx(1.0000001f, 1.0f));
        VX_STATIC_CHECK(greater_or_equal_approx(0.0f, constants<f32>::epsilon));


        // int
        VX_STATIC_CHECK(greater_or_equal_approx(2, 1));
        VX_STATIC_CHECK(greater_or_equal_approx(1, 1));
        VX_STATIC_CHECK(!greater_or_equal_approx(0, 1));
    }

    VX_SECTION("custom epsilon")
    {
        constexpr float loose = 1e-2f;
        constexpr float tight = 1e-8f;

        VX_STATIC_CHECK(greater_or_equal_approx(0.99f, 1.0f, loose));
        VX_STATIC_CHECK(!greater_or_equal_approx(0.99f, 1.0f, tight));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_greater_or_equal_approx)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 1.00001f, 0.999998f };
        constexpr vec2f b{ 1.0f, 1.0f };
        VX_STATIC_CHECK(each_greater_or_equal_approx(a, b) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 1.0f, 1.0000001f };
        constexpr vec3f b{ 1.0f, 1.1f, 1.0f };
        VX_STATIC_CHECK(each_greater_or_equal_approx(a, b) == vec3b(true, false, true));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 1.0f, 2.0f, 2.0f, 3.0f };
        constexpr vec4f b{ 0.9999f, 2.0f, 2.1f, 3.000002f };
        VX_STATIC_CHECK(each_greater_or_equal_approx(a, b) == vec4b(true, true, false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_less_or_equal_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(less_or_equal_approx(1.0f, 1.0f));
        VX_STATIC_CHECK(less_or_equal_approx(0.9999991f, 1.0f)); // inside epsilon
        VX_STATIC_CHECK(!less_or_equal_approx(1.00001f, 1.0f));  // outside epsilon
        VX_STATIC_CHECK(less_or_equal_approx(0.999f, 1.0f));
        VX_STATIC_CHECK(less_or_equal_approx(0.0f, constants<f32>::epsilon));      // On epsilon

        // int
        VX_STATIC_CHECK(less_or_equal_approx(0, 1));
        VX_STATIC_CHECK(less_or_equal_approx(1, 1));
        VX_STATIC_CHECK(!less_or_equal_approx(2, 1));
    }

    VX_SECTION("custom epsilon")
    {
        constexpr float loose = 1e-2f;
        constexpr float tight = 1e-8f;

        VX_STATIC_CHECK(less_or_equal_approx(1.01f, 1.0f, loose));
        VX_STATIC_CHECK(!less_or_equal_approx(1.01f, 1.0f, tight));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_less_or_equal_approx)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 0.9999991f, 1.0f };
        constexpr vec2f b{ 1.0f, 0.999f };
        VX_STATIC_CHECK(each_less_or_equal_approx(a, b) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 0.9f, 1.0f, 1.0000001f };
        constexpr vec3f b{ 1.0f, 1.0f, 1.0f };
        VX_STATIC_CHECK(each_less_or_equal_approx(a, b) == vec3b(true, true, true));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 1.0f, 2.0f, 2.0f, 3.0f };
        constexpr vec4f b{ 1.00001f, 2.0f, 1.9f, 3.0f };
        VX_STATIC_CHECK(each_less_or_equal_approx(a, b) == vec4b(true, true, false, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_any)
{
    VX_SECTION("vec2")
    {
        // Both components are zero
        constexpr vec2f a{ 0.0f, 0.0f };
        VX_STATIC_CHECK(any(a) == false);

        // One component is non-zero
        constexpr vec2f b{ 1.0f, 0.0f };
        VX_STATIC_CHECK(any(b) == true);

        constexpr vec2f c{ 0.0f, 1.0f };
        VX_STATIC_CHECK(any(c) == true);
    }

    VX_SECTION("vec3")
    {
        // All components are zero
        constexpr vec3f a{ 0.0f, 0.0f, 0.0f };
        VX_STATIC_CHECK(any(a) == false);

        // One component is non-zero
        constexpr vec3f b{ 1.0f, 0.0f, 0.0f };
        VX_STATIC_CHECK(any(b) == true);

        constexpr vec3f c{ 0.0f, 1.0f, 0.0f };
        VX_STATIC_CHECK(any(c) == true);

        constexpr vec3f d{ 0.0f, 0.0f, 1.0f };
        VX_STATIC_CHECK(any(d) == true);
    }

    VX_SECTION("vec4")
    {
        // All components are zero
        constexpr vec4f a{ 0.0f, 0.0f, 0.0f, 0.0f };
        VX_STATIC_CHECK(any(a) == false);

        // One component is non-zero
        constexpr vec4f b{ 1.0f, 0.0f, 0.0f, 0.0f };
        VX_STATIC_CHECK(any(b) == true);

        // Multiple components are non-zero
        constexpr vec4f c{ 1.0f, 1.0f, 0.0f, 0.0f };
        VX_STATIC_CHECK(any(c) == true);

        constexpr vec4f d{ 0.0f, 0.0f, 1.0f, 1.0f };
        VX_STATIC_CHECK(any(d) == true);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_all)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 1.0f, 1.0f };
        VX_STATIC_CHECK(all(a) == true);

        constexpr vec2f b{ 0.0f, 1.0f };
        VX_STATIC_CHECK(all(b) == false);

        constexpr vec2f c{ 1.0f, 0.0f };
        VX_STATIC_CHECK(all(c) == false);

        constexpr vec2f d{ 0.0f, 0.0f };
        VX_STATIC_CHECK(all(d) == false);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 2.0f, 3.0f };
        VX_STATIC_CHECK(all(a) == true);

        constexpr vec3f b{ 0.0f, 1.0f, 2.0f };
        VX_STATIC_CHECK(all(b) == false);

        constexpr vec3f c{ 1.0f, 0.0f, 2.0f };
        VX_STATIC_CHECK(all(c) == false);

        constexpr vec3f d{ 1.0f, 2.0f, 0.0f };
        VX_STATIC_CHECK(all(d) == false);
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 1.0f, 2.0f, 3.0f, 4.0f };
        VX_STATIC_CHECK(all(a) == true);

        constexpr vec4f b{ 0.0f, 2.0f, 3.0f, 4.0f };
        VX_STATIC_CHECK(all(b) == false);

        constexpr vec4f c{ 1.0f, 0.0f, 3.0f, 4.0f };
        VX_STATIC_CHECK(all(c) == false);

        constexpr vec4f d{ 1.0f, 2.0f, 3.0f, 0.0f };
        VX_STATIC_CHECK(all(d) == false);
    }

}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_none)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 0.0f, 0.0f };
        VX_STATIC_CHECK(none(a) == true);

        constexpr vec2f b{ 1.0f, 0.0f };
        VX_STATIC_CHECK(none(b) == false);

        constexpr vec2f c{ 0.0f, 1.0f };
        VX_STATIC_CHECK(none(c) == false);

        constexpr vec2f d{ 1.0f, 1.0f };
        VX_STATIC_CHECK(none(d) == false);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 0.0f, 0.0f, 0.0f };
        VX_STATIC_CHECK(none(a) == true);

        constexpr vec3f b{ 1.0f, 0.0f, 0.0f };
        VX_STATIC_CHECK(none(b) == false);

        constexpr vec3f c{ 0.0f, 1.0f, 0.0f };
        VX_STATIC_CHECK(none(c) == false);

        constexpr vec3f d{ 0.0f, 0.0f, 1.0f };
        VX_STATIC_CHECK(none(d) == false);
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 0.0f, 0.0f, 0.0f, 0.0f };
        VX_STATIC_CHECK(none(a) == true);

        constexpr vec4f b{ 1.0f, 0.0f, 0.0f, 0.0f };
        VX_STATIC_CHECK(none(b) == false);

        constexpr vec4f c{ 0.0f, 1.0f, 0.0f, 0.0f };
        VX_STATIC_CHECK(none(c) == false);

        constexpr vec4f d{ 0.0f, 0.0f, 0.0f, 1.0f };
        VX_STATIC_CHECK(none(d) == false);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}