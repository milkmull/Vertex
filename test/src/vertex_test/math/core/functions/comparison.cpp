#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_equal)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(!equal(true, false));
        VX_CHECK(equal(1, 1));
        VX_CHECK(!equal(1u, 2u));
        VX_CHECK(equal(1.5f, 1.5f));
        VX_CHECK(!equal(-1.5, 1.5));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_equal)
{
    VX_SECTION("vec2")
    {
        const vec2i a{ 1, 2 };
        const vec2i b{ 1, 2 };
        const vec2i c{ 1, 3 };

        VX_CHECK(each_equal(a, b) == vec2b(true, true));
        VX_CHECK(each_equal(a, c) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 1.0f, 2.0f, 3.0f };
        const vec3f b{ 1.0f, 2.0f, 3.0f };
        const vec3f c{ 1.0f, 2.0f, 4.0f };

        VX_CHECK(each_equal(a, b) == vec3b(true, true, true));
        VX_CHECK(each_equal(a, c) == vec3b(true, true, false));
    }

    VX_SECTION("vec4")
    {
        const vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        const vec4d b{ 1.0, 2.0, 3.0, 4.0 };
        const vec4d c{ 1.0, 2.0, 0.0, 4.0 };

        VX_CHECK(each_equal(a, b) == vec4b(true, true, true, true));
        VX_CHECK(each_equal(a, c) == vec4b(true, true, false, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_not_equal)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(not_equal(true, false));
        VX_CHECK(!not_equal(1, 1));
        VX_CHECK(not_equal(1u, 2u));
        VX_CHECK(!not_equal(1.5f, 1.5f));
        VX_CHECK(not_equal(-1.5, 1.5));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_not_equal)
{
    VX_SECTION("vec2")
    {
        const vec2i a{ 1, 2 };
        const vec2i b{ 1, 2 };
        const vec2i c{ 1, 3 };

        VX_CHECK(each_not_equal(a, b) == vec2b(false, false));
        VX_CHECK(each_not_equal(a, c) == vec2b(false, true));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 1.0f, 2.0f, 3.0f };
        const vec3f b{ 1.0f, 2.0f, 3.0f };
        const vec3f c{ 1.0f, 2.0f, 4.0f };

        VX_CHECK(each_not_equal(a, b) == vec3b(false, false, false));
        VX_CHECK(each_not_equal(a, c) == vec3b(false, false, true));
    }

    VX_SECTION("vec4")
    {
        const vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        const vec4d b{ 1.0, 2.0, 3.0, 4.0 };
        const vec4d c{ 1.0, 2.0, 0.0, 4.0 };

        VX_CHECK(each_not_equal(a, b) == vec4b(false, false, false, false));
        VX_CHECK(each_not_equal(a, c) == vec4b(false, false, true, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_greater)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(!greater(1, 1));
        VX_CHECK(greater(2, 1));
        VX_CHECK(!greater(1u, 2u));
        VX_CHECK(greater(2.5f, 2.0f));
        VX_CHECK(!greater(-1.5, 1.5));
        VX_CHECK(greater(0, -1));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_greater)
{
    VX_SECTION("vec2")
    {
        const vec2i a{ 2, 3 };
        const vec2i b{ 1, 3 };
        const vec2i c{ 0, 4 };

        VX_CHECK(each_greater(a, b) == vec2b(true, false));
        VX_CHECK(each_greater(a, c) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 1.0f, 4.0f, 3.0f };
        const vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_CHECK(each_greater(a, b) == vec3b(false, true, false));
    }

    VX_SECTION("vec4")
    {
        const vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        const vec4d b{ 0.0, 2.0, 2.0, 5.0 };

        VX_CHECK(each_greater(a, b) == vec4b(true, false, true, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_less)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(!less(1, 1));
        VX_CHECK(!less(2, 1));
        VX_CHECK(less(1u, 2u));
        VX_CHECK(!less(2.5f, 2.0f));
        VX_CHECK(less(-1.5, 1.5));
        VX_CHECK(!less(0, -1));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_less)
{
    VX_SECTION("vec2")
    {
        const vec2i a{ 2, 3 };
        const vec2i b{ 1, 3 };
        const vec2i c{ 0, 4 };

        VX_CHECK(each_less(a, b) == vec2b(false, false));
        VX_CHECK(each_less(a, c) == vec2b(false, true));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 1.0f, 4.0f, 3.0f };
        const vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_CHECK(each_less(a, b) == vec3b(false, false, true));
    }

    VX_SECTION("vec4")
    {
        const vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        const vec4d b{ 0.0, 2.0, 2.0, 5.0 };

        VX_CHECK(each_less(a, b) == vec4b(false, false, false, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_greater_or_equal)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(greater_or_equal(1, 1));
        VX_CHECK(greater_or_equal(2, 1));
        VX_CHECK(!greater_or_equal(1u, 2u));
        VX_CHECK(greater_or_equal(2.5f, 2.0f));
        VX_CHECK(!greater_or_equal(-1.5, 1.5));
        VX_CHECK(greater_or_equal(0, -1));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_greater_or_equal)
{
    VX_SECTION("vec2")
    {
        const vec2i a{ 2, 3 };
        const vec2i b{ 1, 3 };
        const vec2i c{ 2, 4 };

        VX_CHECK(each_greater_or_equal(a, b) == vec2b(true, true));
        VX_CHECK(each_greater_or_equal(a, c) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 1.0f, 4.0f, 3.0f };
        const vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_CHECK(each_greater_or_equal(a, b) == vec3b(true, true, false));
    }

    VX_SECTION("vec4")
    {
        const vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        const vec4d b{ 0.0, 2.0, 2.0, 4.0 };

        VX_CHECK(each_greater_or_equal(a, b) == vec4b(true, true, true, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_less_or_equal)
{
    VX_SECTION("scalar")
    {
        VX_CHECK(less_or_equal(1, 1));
        VX_CHECK(!less_or_equal(2, 1));
        VX_CHECK(less_or_equal(1u, 2u));
        VX_CHECK(!less_or_equal(2.5f, 2.0f));
        VX_CHECK(less_or_equal(-1.5, 1.5));
        VX_CHECK(!less_or_equal(0, -1));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_less_or_equal)
{
    VX_SECTION("vec2")
    {
        const vec2i a{ 2, 3 };
        const vec2i b{ 1, 3 };
        const vec2i c{ 2, 4 };

        VX_CHECK(each_less_or_equal(a, b) == vec2b(false, true));
        VX_CHECK(each_less_or_equal(a, c) == vec2b(true, true));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 1.0f, 4.0f, 3.0f };
        const vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_CHECK(each_less_or_equal(a, b) == vec3b(true, false, true));
    }

    VX_SECTION("vec4")
    {
        const vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        const vec4d b{ 0.0, 2.0, 2.0, 4.0 };

        VX_CHECK(each_less_or_equal(a, b) == vec4b(false, true, false, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_inf)
{
    VX_SECTION("scalar")
    {
        // float
        VX_CHECK(is_inf(constants<f32>::infinity));
        VX_CHECK(is_inf(-constants<f32>::infinity));
        VX_CHECK(!is_inf(0.0f));
        VX_CHECK(!is_inf(1.0));
        VX_CHECK(!is_inf(constants<f32>::nan)); // NaN is not infinite

        // int
        VX_CHECK(!is_inf(0));
        VX_CHECK(!is_inf(123));
        VX_CHECK(!is_inf(-456));
    }

    VX_SECTION("vec2")
    {
        const vec2f a{ constants<f32>::infinity, 1.0f };
        const vec2f b{ -constants<f32>::infinity, constants<f32>::nan };
        const vec2f c{ 0.0f, 42.0f };

        VX_CHECK(is_inf(a));
        VX_CHECK(is_inf(b));
        VX_CHECK(!is_inf(c));
    }

    VX_SECTION("vec3")
    {
        const vec3d a{ constants<f64>::infinity, 2.0, 3.0 };
        const vec3d b{ 1.0, 2.0, constants<f64>::nan };
        const vec3d c{ 0.0, 42.0, -100.0 };

        VX_CHECK(is_inf(a));
        VX_CHECK(!is_inf(b));
        VX_CHECK(!is_inf(c));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 1.0f, constants<f32>::infinity, 0.0f, 4.0f };
        const vec4f b{ -constants<f32>::infinity, 2.0f, constants<f32>::nan, 3.0f };
        const vec4f c{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_CHECK(is_inf(a));
        VX_CHECK(is_inf(b));
        VX_CHECK(!is_inf(c));
    }

    VX_SECTION("int vec")
    {
        const vec2i vi{ 1, 2 };
        const vec3i vi3{ 0, -1, 123 };
        const vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_CHECK(!is_inf(vi));
        VX_CHECK(!is_inf(vi3));
        VX_CHECK(!is_inf(vu4));
    }

    VX_SECTION("mat2x2")
    {
        VX_CHECK(!is_inf(mat2x2()));
        VX_CHECK(is_inf(mat2x2(constants<f32>::infinity)));
        VX_CHECK(is_inf(mat2x2(-constants<f32>::infinity)));
        VX_CHECK(!is_inf(mat2x2(-constants<f32>::nan)));
    }

    VX_SECTION("mat2x3")
    {
        VX_CHECK(!is_inf(mat2x3()));
        VX_CHECK(is_inf(mat2x3(constants<f32>::infinity)));
        VX_CHECK(is_inf(mat2x3(-constants<f32>::infinity)));
        VX_CHECK(!is_inf(mat2x3(-constants<f32>::nan)));
    }

    VX_SECTION("mat2x4")
    {
        VX_CHECK(!is_inf(mat2x4()));
        VX_CHECK(is_inf(mat2x4(constants<f32>::infinity)));
        VX_CHECK(is_inf(mat2x4(-constants<f32>::infinity)));
        VX_CHECK(!is_inf(mat2x4(-constants<f32>::nan)));
    }

    VX_SECTION("mat3x2")
    {
        VX_CHECK(!is_inf(mat3x2()));
        VX_CHECK(is_inf(mat3x2(constants<f32>::infinity)));
        VX_CHECK(is_inf(mat3x2(-constants<f32>::infinity)));
        VX_CHECK(!is_inf(mat3x2(-constants<f32>::nan)));
    }

    VX_SECTION("mat3x3")
    {
        VX_CHECK(!is_inf(mat3x3()));
        VX_CHECK(is_inf(mat3x3(constants<f32>::infinity)));
        VX_CHECK(is_inf(mat3x3(-constants<f32>::infinity)));
        VX_CHECK(!is_inf(mat3x3(-constants<f32>::nan)));
    }

    VX_SECTION("mat3x4")
    {
        VX_CHECK(!is_inf(mat3x4()));
        VX_CHECK(is_inf(mat3x4(constants<f32>::infinity)));
        VX_CHECK(is_inf(mat3x4(-constants<f32>::infinity)));
        VX_CHECK(!is_inf(mat3x4(-constants<f32>::nan)));
    }

    VX_SECTION("mat4x2")
    {
        VX_CHECK(!is_inf(mat4x2()));
        VX_CHECK(is_inf(mat4x2(constants<f32>::infinity)));
        VX_CHECK(is_inf(mat4x2(-constants<f32>::infinity)));
        VX_CHECK(!is_inf(mat4x2(-constants<f32>::nan)));
    }

    VX_SECTION("mat4x3")
    {
        VX_CHECK(!is_inf(mat4x3()));
        VX_CHECK(is_inf(mat4x3(constants<f32>::infinity)));
        VX_CHECK(is_inf(mat4x3(-constants<f32>::infinity)));
        VX_CHECK(!is_inf(mat4x3(-constants<f32>::nan)));
    }

    VX_SECTION("mat4x4")
    {
        VX_CHECK(!is_inf(mat4x4()));
        VX_CHECK(is_inf(mat4x4(constants<f32>::infinity)));
        VX_CHECK(is_inf(mat4x4(-constants<f32>::infinity)));
        VX_CHECK(!is_inf(mat4x4(-constants<f32>::nan)));
    }

    VX_SECTION("quat")
    {
        VX_CHECK(!is_inf(quat()));
        VX_CHECK(is_inf(quat(constants<f32>::infinity, 0.0f, 0.0f, 0.0f)));
        VX_CHECK(is_inf(quat(-constants<f32>::infinity, 0.0f, 0.0f, 0.0f)));
        VX_CHECK(!is_inf(quat(-constants<f32>::nan, 0.0f, 0.0f, 0.0f)));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_is_inf)
{
    VX_SECTION("vec2")
    {
        const vec2f a{ constants<f32>::infinity, 1.0f };
        const vec2f b{ -constants<f32>::infinity, constants<f32>::infinity };
        const vec2f c{ 0.0f, 42.0f };

        VX_CHECK(each_is_inf(a) == vec2b(true, false));
        VX_CHECK(each_is_inf(b) == vec2b(true, true));
        VX_CHECK(each_is_inf(c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        const vec3d a{ 0.0, constants<f64>::infinity, -constants<f64>::infinity };
        const vec3d b{ 1.0, 2.0, 3.0 };

        VX_CHECK(each_is_inf(a) == vec3b(false, true, true));
        VX_CHECK(each_is_inf(b) == vec3b(false, false, false));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 0.0f, constants<f32>::infinity, 1.0f, -constants<f32>::infinity };
        const vec4f b{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_CHECK(each_is_inf(a) == vec4b(false, true, false, true));
        VX_CHECK(each_is_inf(b) == vec4b(false, false, false, false));
    }

    VX_SECTION("int vec")
    {
        const vec2i vi{ 1, 2 };
        const vec3i vi3{ 0, -1, 123 };
        const vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_CHECK(each_is_inf(vi) == vec2b(false, false));
        VX_CHECK(each_is_inf(vi3) == vec3b(false, false, false));
        VX_CHECK(each_is_inf(vu4) == vec4b(false, false, false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_nan)
{
    VX_SECTION("scalar")
    {
        // float
        VX_CHECK(is_nan(constants<f32>::nan));
        VX_CHECK(!is_nan(constants<f32>::infinity));
        VX_CHECK(!is_nan(-constants<f32>::infinity));
        VX_CHECK(!is_nan(0.0f));
        VX_CHECK(!is_nan(1.0f));

        // int
        VX_CHECK(!is_nan(0));
        VX_CHECK(!is_nan(123));
        VX_CHECK(!is_nan(-456));
    }

    VX_SECTION("vec2")
    {
        const vec2f a{ constants<f32>::nan, 1.0f };
        const vec2f b{ -constants<f32>::infinity, constants<f32>::nan };
        const vec2f c{ 0.0f, 42.0f };

        VX_CHECK(is_nan(a));
        VX_CHECK(is_nan(b));
        VX_CHECK(!is_nan(c));
    }

    VX_SECTION("vec3")
    {
        const vec3d a{ constants<f64>::nan, 2.0, 3.0 };
        const vec3d b{ 1.0, 2.0, constants<f64>::infinity };
        const vec3d c{ 0.0, 42.0, -100.0 };

        VX_CHECK(is_nan(a));
        VX_CHECK(!is_nan(b));
        VX_CHECK(!is_nan(c));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 1.0f, constants<f32>::nan, 0.0f, 4.0f };
        const vec4f b{ -constants<f32>::infinity, 2.0f, constants<f32>::nan, 3.0f };
        const vec4f c{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_CHECK(is_nan(a));
        VX_CHECK(is_nan(b));
        VX_CHECK(!is_nan(c));
    }

    VX_SECTION("int vec")
    {
        const vec2i vi{ 1, 2 };
        const vec3i vi3{ 0, -1, 123 };
        const vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_CHECK(!is_nan(vi));
        VX_CHECK(!is_nan(vi3));
        VX_CHECK(!is_nan(vu4));
    }

    VX_SECTION("mat2x2")
    {
        VX_CHECK(!is_nan(mat2x2()));
        VX_CHECK(!is_nan(mat2x2(constants<f32>::infinity)));
        VX_CHECK(!is_nan(mat2x2(-constants<f32>::infinity)));
        VX_CHECK(is_nan(mat2x2(-constants<f32>::nan)));
    }

    VX_SECTION("mat2x3")
    {
        VX_CHECK(!is_nan(mat2x3()));
        VX_CHECK(!is_nan(mat2x3(constants<f32>::infinity)));
        VX_CHECK(!is_nan(mat2x3(-constants<f32>::infinity)));
        VX_CHECK(is_nan(mat2x3(-constants<f32>::nan)));
    }

    VX_SECTION("mat2x4")
    {
        VX_CHECK(!is_nan(mat2x4()));
        VX_CHECK(!is_nan(mat2x4(constants<f32>::infinity)));
        VX_CHECK(!is_nan(mat2x4(-constants<f32>::infinity)));
        VX_CHECK(is_nan(mat2x4(-constants<f32>::nan)));
    }

    VX_SECTION("mat3x2")
    {
        VX_CHECK(!is_nan(mat3x2()));
        VX_CHECK(!is_nan(mat3x2(constants<f32>::infinity)));
        VX_CHECK(!is_nan(mat3x2(-constants<f32>::infinity)));
        VX_CHECK(is_nan(mat3x2(-constants<f32>::nan)));
    }

    VX_SECTION("mat3x3")
    {
        VX_CHECK(!is_nan(mat3x3()));
        VX_CHECK(!is_nan(mat3x3(constants<f32>::infinity)));
        VX_CHECK(!is_nan(mat3x3(-constants<f32>::infinity)));
        VX_CHECK(is_nan(mat3x3(-constants<f32>::nan)));
    }

    VX_SECTION("mat3x4")
    {
        VX_CHECK(!is_nan(mat3x4()));
        VX_CHECK(!is_nan(mat3x4(constants<f32>::infinity)));
        VX_CHECK(!is_nan(mat3x4(-constants<f32>::infinity)));
        VX_CHECK(is_nan(mat3x4(-constants<f32>::nan)));
    }

    VX_SECTION("mat4x2")
    {
        VX_CHECK(!is_nan(mat4x2()));
        VX_CHECK(!is_nan(mat4x2(constants<f32>::infinity)));
        VX_CHECK(!is_nan(mat4x2(-constants<f32>::infinity)));
        VX_CHECK(is_nan(mat4x2(-constants<f32>::nan)));
    }

    VX_SECTION("mat4x3")
    {
        VX_CHECK(!is_nan(mat4x3()));
        VX_CHECK(!is_nan(mat4x3(constants<f32>::infinity)));
        VX_CHECK(!is_nan(mat4x3(-constants<f32>::infinity)));
        VX_CHECK(is_nan(mat4x3(-constants<f32>::nan)));
    }

    VX_SECTION("mat4x4")
    {
        VX_CHECK(!is_nan(mat4x4()));
        VX_CHECK(!is_nan(mat4x4(constants<f32>::infinity)));
        VX_CHECK(!is_nan(mat4x4(-constants<f32>::infinity)));
        VX_CHECK(is_nan(mat4x4(-constants<f32>::nan)));
    }

    VX_SECTION("quat")
    {
        VX_CHECK(!is_nan(quat()));
        VX_CHECK(!is_nan(quat(constants<f32>::infinity, 0.0f, 0.0f, 0.0f)));
        VX_CHECK(!is_nan(quat(-constants<f32>::infinity, 0.0f, 0.0f, 0.0f)));
        VX_CHECK(is_nan(quat(-constants<f32>::nan, 0.0f, 0.0f, 0.0f)));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_is_nan)
{
    VX_SECTION("vec2")
    {
        const vec2f a{ constants<f32>::nan, 1.0f };
        const vec2f b{ -constants<f32>::infinity, constants<f32>::nan };
        const vec2f c{ 0.0f, 42.0f };

        VX_CHECK(each_is_nan(a) == vec2b(true, false));
        VX_CHECK(each_is_nan(b) == vec2b(false, true));
        VX_CHECK(each_is_nan(c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        const vec3d a{ 0.0, constants<f64>::nan, -constants<f64>::infinity };
        const vec3d b{ 1.0, 2.0, 3.0 };

        VX_CHECK(each_is_nan(a) == vec3b(false, true, false));
        VX_CHECK(each_is_nan(b) == vec3b(false, false, false));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 0.0f, constants<f32>::nan, 1.0f, -constants<f32>::infinity };
        const vec4f b{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_CHECK(each_is_nan(a) == vec4b(false, true, false, false));
        VX_CHECK(each_is_nan(b) == vec4b(false, false, false, false));
    }

    VX_SECTION("int vec")
    {
        const vec2i vi{ 1, 2 };
        const vec3i vi3{ 0, -1, 123 };
        const vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_CHECK(each_is_nan(vi) == vec2b(false, false));
        VX_CHECK(each_is_nan(vi3) == vec3b(false, false, false));
        VX_CHECK(each_is_nan(vu4) == vec4b(false, false, false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_zero_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_CHECK(is_zero_approx(0.0f));
        VX_CHECK(is_zero_approx(0.0000009f));
        VX_CHECK(!is_zero_approx(1.0f));
        VX_CHECK(is_zero_approx(constants<f32>::epsilon));

        // int
        VX_CHECK(is_zero_approx(0));
        VX_CHECK(!is_zero_approx(123));
        VX_CHECK(!is_zero_approx(-456));
    }

    VX_SECTION("vec2")
    {
        const vec2f a{ 0.0f, 0.0f };       // Exactly zero
        const vec2f b{ 0.0000001f, 0.0f }; // Within epsilon
        const vec2f c{ 1.0f, 0.0f };       // Not all zero approx

        VX_CHECK(is_zero_approx(a));
        VX_CHECK(is_zero_approx(b));
        VX_CHECK(!is_zero_approx(c));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 0.0f, 0.0f, 0.0f };              // Exactly zero
        const vec3f b{ 0.0000001f, 0.0f, 0.0f };        // Approx zero
        const vec3f c{ 1.0f, 0.0f, 0.0f };              // Not all zero approx
        const vec3f d{ 0.0000009f, 0.0000009f, 0.0f };   // Approx zero for all

        VX_CHECK(is_zero_approx(a));
        VX_CHECK(is_zero_approx(b));
        VX_CHECK(!is_zero_approx(c));
        VX_CHECK(is_zero_approx(d));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 0.0f, 0.0f, 0.0f, 0.0f };                    // Exactly zero
        const vec4f b{ 0.0000001f, 0.0f, 0.0f, 0.0f };              // Approx zero
        const vec4f c{ 1.0f, 0.0f, 0.0f, 0.0f };                    // Not all zero approx
        const vec4f d{ 0.0000009f, 0.0000009f, 0.0000009f, 0.0f };  // Approx zero for all

        VX_CHECK(is_zero_approx(a));
        VX_CHECK(is_zero_approx(b));
        VX_CHECK(!is_zero_approx(c));
        VX_CHECK(is_zero_approx(d));
    }

    VX_SECTION("custom epsilon")
    {
        const float tight = 1e-8f;
        const float loose = 1e-3f;
        VX_CHECK(!is_zero_approx(1e-6f, tight));
        VX_CHECK(is_zero_approx(1e-6f, loose));

        const vec3f a{ 1e-6f, 0.0f, 0.0f };
        VX_CHECK(!is_zero_approx(a, tight));
        VX_CHECK(is_zero_approx(a, loose));
    }

    VX_SECTION("mat2x2")
    {
        VX_CHECK(is_zero_approx(mat2x2::zero()));
        VX_CHECK(!is_zero_approx(mat2x2(1.0f)));
        VX_CHECK(is_zero_approx(mat2x2(0.0000009f)));
    }

    VX_SECTION("mat2x3")
    {
        VX_CHECK(is_zero_approx(mat2x3::zero()));
        VX_CHECK(!is_zero_approx(mat2x3(1.0f)));
        VX_CHECK(is_zero_approx(mat2x3(0.0000009f)));
    }

    VX_SECTION("mat2x4")
    {
        VX_CHECK(is_zero_approx(mat2x4::zero()));
        VX_CHECK(!is_zero_approx(mat2x4(1.0f)));
        VX_CHECK(is_zero_approx(mat2x4(0.0000009f)));
    }

    VX_SECTION("mat3x2")
    {
        VX_CHECK(is_zero_approx(mat3x2::zero()));
        VX_CHECK(!is_zero_approx(mat3x2(1.0f)));
        VX_CHECK(is_zero_approx(mat3x2(0.0000009f)));
    }

    VX_SECTION("mat3x3")
    {
        VX_CHECK(is_zero_approx(mat3x3::zero()));
        VX_CHECK(!is_zero_approx(mat3x3(1.0f)));
        VX_CHECK(is_zero_approx(mat3x3(0.0000009f)));
    }

    VX_SECTION("mat3x4")
    {
        VX_CHECK(is_zero_approx(mat3x4::zero()));
        VX_CHECK(!is_zero_approx(mat3x4(1.0f)));
        VX_CHECK(is_zero_approx(mat3x4(0.0000009f)));
    }

    VX_SECTION("mat4x2")
    {
        VX_CHECK(is_zero_approx(mat4x2::zero()));
        VX_CHECK(!is_zero_approx(mat4x2(1.0f)));
        VX_CHECK(is_zero_approx(mat4x2(0.0000009f)));
    }

    VX_SECTION("mat4x3")
    {
        VX_CHECK(is_zero_approx(mat4x3::zero()));
        VX_CHECK(!is_zero_approx(mat4x3(1.0f)));
        VX_CHECK(is_zero_approx(mat4x3(0.0000009f)));
    }

    VX_SECTION("mat4x4")
    {
        VX_CHECK(is_zero_approx(mat4x4::zero()));
        VX_CHECK(!is_zero_approx(mat4x4(1.0f)));
        VX_CHECK(is_zero_approx(mat4x4(0.0000009f)));
    }

    VX_SECTION("quat")
    {
        VX_CHECK(is_zero_approx(quat::zero()));
        VX_CHECK(!is_zero_approx(quat::identity()));
        VX_CHECK(is_zero_approx(quat(-0.0000009f, 0.0f, 0.0f, 0.0f)));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_is_zero_approx)
{
    VX_SECTION("vec2")
    {
        const vec2f a{ 0.0f, 1.0f };
        const vec2f b{ 0.0000001f, 0.0f };
        const vec2f c{ 1.0f, -1.0f };

        VX_CHECK(each_is_zero_approx(a) == vec2b(true, false));
        VX_CHECK(each_is_zero_approx(b) == vec2b(true, true));
        VX_CHECK(each_is_zero_approx(c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        const vec3d a{ 0.0, 0.000000001, 1.0 };
        const vec3d b{ -0.00000005, 2.0, -2.0 };
        const vec3d c{ 0.1, 0.2, 0.3 };

        VX_CHECK(each_is_zero_approx(a) == vec3b(true, true, false));
        VX_CHECK(each_is_zero_approx(b) == vec3b(true, false, false));
        VX_CHECK(each_is_zero_approx(c) == vec3b(false, false, false));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 0.0f, 0.0000000001f, -0.0000000001f, 1.0f };
        const vec4f b{ 0.0000001f, 2.0f, 0.0f, 4.0f };
        const vec4f c{ 0.1f, 0.2f, 0.3f, 0.4f };

        VX_CHECK(each_is_zero_approx(a) == vec4b(true, true, true, false));
        VX_CHECK(each_is_zero_approx(b) == vec4b(true, false, true, false));
        VX_CHECK(each_is_zero_approx(c) == vec4b(false, false, false, false));
    }

    VX_SECTION("int vec")
    {
        const vec2i vi{ 0, 0 };
        const vec3i vi3{ 0, 123, -1 };
        const vec4u vu4{ 0u, 0u, 0u, 0u };

        VX_CHECK(each_is_zero_approx(vi) == vec2b(true, true));
        VX_CHECK(each_is_zero_approx(vi3) == vec3b(true, false, false));
        VX_CHECK(each_is_zero_approx(vu4) == vec4b(true, true, true, true));
    }

    VX_SECTION("custom epsilon")
    {
        const float tight = 1e-8f;
        const float loose = 1e-3f;
        const vec2f v{ 1e-6f, 0.0f };

        VX_CHECK(each_is_zero_approx(v, tight) == vec2b(false, true));
        VX_CHECK(each_is_zero_approx(v, loose) == vec2b(true, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_equal_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_CHECK(equal_approx(1.0f, 1.0f));
        VX_CHECK(equal_approx(1.0f, 1.0000009f));
        VX_CHECK(!equal_approx(1.0f, 1.00001f));
        VX_CHECK(equal_approx(0.0f, constants<f32>::epsilon));

        // int
        VX_CHECK(equal_approx(3, 3));
        VX_CHECK(!equal_approx(3, 4));
    }

    VX_SECTION("vec2")
    {
        const vec2f a{ 1.0f, 2.0f };
        const vec2f b{ 1.0f, 2.0f };
        const vec2f c{ 1.0f, 2.0000001f };
        const vec2f d{ 1.0f, 3.0f };
        const vec2f e{ 1.0000001f, 2.0000001f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(equal_approx(a, c));
        VX_CHECK(!equal_approx(a, d));
        VX_CHECK(equal_approx(a, e));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ -1.0f, 0.0f, 1.0f };
        const vec3f b{ -1.0f, 0.0f, 1.0f };
        const vec3f c{ -1.0000001f, 0.0f, 1.0000001f };
        const vec3f d{ -1.0f, 0.0f, 2.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(equal_approx(a, c));
        VX_CHECK(!equal_approx(a, d));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 5.0f, 6.0f, 7.0f, 8.0f };
        const vec4f b{ 5.0f, 6.0f, 7.0f, 8.0f };
        const vec4f c{ 5.0000001f, 6.0f, 7.0000001f, 8.0f };
        const vec4f d{ 5.0f, 6.0f, 7.0f, 9.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(equal_approx(a, c));
        VX_CHECK(!equal_approx(a, d));
    }

    VX_SECTION("custom epsilon")
    {
        const float tight = 1e-8f;
        const float loose = 1e-3f;

        // vec3 with small difference
        const vec3f a{ 1.0f, 2.0f, 3.0f };
        const vec3f b{ 1.00001f, 2.00001f, 3.00001f };

        VX_CHECK(!equal_approx(a, b, tight));
        VX_CHECK(equal_approx(a, b, loose));

        // Scalar double with tight epsilon
        const double x = 5.0;
        const double y = 5.0000000001;

        VX_CHECK(!equal_approx(x, y, 1e-12));
        VX_CHECK(equal_approx(x, y, 1e-9));
    }

    VX_SECTION("mat2x2")
    {
        const mat2x2 a{ 1.0f, 2.0f, 3.0f, 4.0f };
        const mat2x2 b{ 1.0f, 2.0000001f, 3.0f, 4.0f };
        const mat2x2 c{ 1.0f, 2.0f, 3.0f, 5.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(!equal_approx(a, c));
    }

    VX_SECTION("mat2x3")
    {
        const mat2x3 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
        const mat2x3 b{ 1.0f, 2.0f, 3.0000001f, 4.0f, 5.0f, 6.0f };
        const mat2x3 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 7.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(!equal_approx(a, c));
    }

    VX_SECTION("mat2x4")
    {
        const mat2x4 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        const mat2x4 b{ 1.0f, 2.0000001f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        const mat2x4 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 9.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(!equal_approx(a, c));
    }

    VX_SECTION("mat3x2")
    {
        const mat3x2 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
        const mat3x2 b{ 1.0000001f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
        const mat3x2 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 7.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(!equal_approx(a, c));
    }

    VX_SECTION("mat3x3")
    {
        const mat3x3 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        const mat3x3 b{ 1.0f, 2.0000001f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        const mat3x3 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 10.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(!equal_approx(a, c));
    }

    VX_SECTION("mat3x4")
    {
        const mat3x4 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f };
        const mat3x4 b{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0000001f };
        const mat3x4 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 13.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(!equal_approx(a, c));
    }


    VX_SECTION("mat4x2")
    {
        const mat4x2 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        const mat4x2 b{ 1.0f, 2.0000001f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        const mat4x2 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 9.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(!equal_approx(a, c));
    }

    VX_SECTION("mat4x3")
    {
        const mat4x3 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f };
        const mat4x3 b{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0000001f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f };
        const mat4x3 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 13.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(!equal_approx(a, c));
    }

    VX_SECTION("mat4x4")
    {
        const mat4x4 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
        const mat4x4 b{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0000001f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
        const mat4x4 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 17.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(!equal_approx(a, c));
    }

    VX_SECTION("quat")
    {
        const quat a{ 1.0f, 2.0f, 3.0f, 4.0f };
        const quat b{ 1.0000001f, 2.0f, 3.0f, 4.0f };
        const quat c{ 1.0f, 2.0f, 3.0f, 5.0f };

        VX_CHECK(equal_approx(a, b));
        VX_CHECK(!equal_approx(a, c));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_equal_approx)
{
    VX_SECTION("vec2")
    {
        const vec2f a{ 1.0f, 2.0f };
        const vec2f b{ 1.0000009f, 2.0f };
        const vec2f c{ 1.00001f, 2.00001f };

        VX_CHECK(each_equal_approx(a, b) == vec2b(true, true));
        VX_CHECK(each_equal_approx(a, c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 0.0f, 1.0f, 2.0f };
        const vec3f b{ 0.0f, 1.0000009f, 2.0f };
        const vec3f c{ 0.0f, 1.00001f, 2.0f };

        VX_CHECK(each_equal_approx(a, b) == vec3b(true, true, true));
        VX_CHECK(each_equal_approx(a, c) == vec3b(true, false, true));
    }

    VX_SECTION("vec4")
    {
        const vec4d a{ 0.0, 1.0, 2.0, 3.0 };
        const vec4d b{ 0.0, 1.0000009, 2.0, 3.0000009 };
        const vec4d c{ 0.0, 1.00001, 2.0, 3.0 };

        VX_CHECK(each_equal_approx(a, b) == vec4b(true, true, true, true));
        VX_CHECK(each_equal_approx(a, c) == vec4b(true, false, true, true));
    }

    VX_SECTION("int vec")
    {
        const vec2i vi1{ 1, 2 };
        const vec2i vi2{ 1, 2 };
        const vec2i vi3{ 1, 3 };

        VX_CHECK(each_equal_approx(vi1, vi2) == vec2b(true, true));
        VX_CHECK(each_equal_approx(vi1, vi3) == vec2b(true, false));
    }

    VX_SECTION("custom epsilon")
    {
        const float eps_strict = 1e-8f;
        const float eps_loose = 1e-3f;

        // vec2 with small difference
        const vec2f a{ 1.0f, 2.0f };
        const vec2f b{ 1.0000005f, 2.0000005f };

        VX_CHECK(each_equal_approx(a, b, eps_strict) == vec2b(false, false));
        VX_CHECK(each_equal_approx(a, b) == vec2b(true, true));
        VX_CHECK(each_equal_approx(a, b, eps_loose) == vec2b(true, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_not_equal_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_CHECK(!not_equal_approx(1.0f, 1.0f));
        VX_CHECK(!not_equal_approx(1.0f, 1.0000009f));
        VX_CHECK(not_equal_approx(1.0f, 1.00001f));
        VX_CHECK(!not_equal_approx(0.0f, constants<f32>::epsilon));

        // int
        VX_CHECK(!not_equal_approx(3, 3));
        VX_CHECK(not_equal_approx(3, 4));
    }

    VX_SECTION("vec2")
    {
        const vec2f a{ 1.0f, 2.0f };
        const vec2f b{ 1.0f, 2.0f };
        const vec2f c{ 1.0f, 2.0000001f };
        const vec2f d{ 1.0f, 3.0f };
        const vec2f e{ 1.0000001f, 2.0000001f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(!not_equal_approx(a, c));
        VX_CHECK(not_equal_approx(a, d));
        VX_CHECK(!not_equal_approx(a, e));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ -1.0f, 0.0f, 1.0f };
        const vec3f b{ -1.0f, 0.0f, 1.0f };
        const vec3f c{ -1.0000001f, 0.0f, 1.0000001f };
        const vec3f d{ -1.0f, 0.0f, 2.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(!not_equal_approx(a, c));
        VX_CHECK(not_equal_approx(a, d));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 5.0f, 6.0f, 7.0f, 8.0f };
        const vec4f b{ 5.0f, 6.0f, 7.0f, 8.0f };
        const vec4f c{ 5.0000001f, 6.0f, 7.0000001f, 8.0f };
        const vec4f d{ 5.0f, 6.0f, 7.0f, 9.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(!not_equal_approx(a, c));
        VX_CHECK(not_equal_approx(a, d));
    }

    VX_SECTION("custom epsilon")
    {
        const float tight = 1e-8f;
        const float loose = 1e-3f;

        // vec3 with small difference
        const vec3f a{ 1.0f, 2.0f, 3.0f };
        const vec3f b{ 1.00001f, 2.00001f, 3.00001f };

        VX_CHECK(not_equal_approx(a, b, tight));
        VX_CHECK(!not_equal_approx(a, b, loose));

        // Scalar double with tight epsilon
        const double x = 5.0;
        const double y = 5.0000000001;

        VX_CHECK(not_equal_approx(x, y, 1e-12));
        VX_CHECK(!not_equal_approx(x, y, 1e-9));
    }

    VX_SECTION("mat2x2")
    {
        const mat2x2 a{ 1.0f, 2.0f, 3.0f, 4.0f };
        const mat2x2 b{ 1.0f, 2.0000001f, 3.0f, 4.0f };
        const mat2x2 c{ 1.0f, 2.0f, 3.0f, 5.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(not_equal_approx(a, c));
    }

    VX_SECTION("mat2x3")
    {
        const mat2x3 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
        const mat2x3 b{ 1.0f, 2.0f, 3.0000001f, 4.0f, 5.0f, 6.0f };
        const mat2x3 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 7.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(not_equal_approx(a, c));
    }

    VX_SECTION("mat2x4")
    {
        const mat2x4 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        const mat2x4 b{ 1.0f, 2.0000001f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        const mat2x4 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 9.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(not_equal_approx(a, c));
    }

    VX_SECTION("mat3x2")
    {
        const mat3x2 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
        const mat3x2 b{ 1.0000001f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
        const mat3x2 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 7.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(not_equal_approx(a, c));
    }

    VX_SECTION("mat3x3")
    {
        const mat3x3 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        const mat3x3 b{ 1.0f, 2.0000001f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        const mat3x3 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 10.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(not_equal_approx(a, c));
    }

    VX_SECTION("mat3x4")
    {
        const mat3x4 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f };
        const mat3x4 b{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0000001f };
        const mat3x4 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 13.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(not_equal_approx(a, c));
    }


    VX_SECTION("mat4x2")
    {
        const mat4x2 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        const mat4x2 b{ 1.0f, 2.0000001f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        const mat4x2 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 9.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(not_equal_approx(a, c));
    }

    VX_SECTION("mat4x3")
    {
        const mat4x3 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f };
        const mat4x3 b{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0000001f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f };
        const mat4x3 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 13.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(not_equal_approx(a, c));
    }

    VX_SECTION("mat4x4")
    {
        const mat4x4 a{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
        const mat4x4 b{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0000001f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
        const mat4x4 c{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 17.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(not_equal_approx(a, c));
    }

    VX_SECTION("quat")
    {
        const quat a{ 1.0f, 2.0f, 3.0f, 4.0f };
        const quat b{ 1.0000001f, 2.0f, 3.0f, 4.0f };
        const quat c{ 1.0f, 2.0f, 3.0f, 5.0f };

        VX_CHECK(!not_equal_approx(a, b));
        VX_CHECK(not_equal_approx(a, c));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_greater_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_CHECK(greater_approx(1.00001f, 1.0f));
        VX_CHECK(!greater_approx(1.0000001f, 1.0f));
        VX_CHECK(!greater_approx(1.0f, 1.0f));
        VX_CHECK(!greater_approx(0.99999f, 1.0f));
        VX_CHECK(!greater_approx(0.0f, constants<f32>::epsilon));

        // int
        VX_CHECK(greater_approx(2, 1));
        VX_CHECK(!greater_approx(1, 1));
        VX_CHECK(!greater_approx(0, 1));
    }

    VX_SECTION("custom epsilon")
    {
        const float tight = 1e-8f;
        const float loose = 1e-2f;

        // scalar
        VX_CHECK(greater_approx(1.001f, 1.0f, tight));
        VX_CHECK(!greater_approx(1.001f, 1.0f, loose));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_greater_approx)
{
    VX_SECTION("vec2")
    {
        const vec2f a{ 1.00001f, 0.5f };
        const vec2f b{ 1.0f, 0.5f };
        VX_CHECK(each_greater_approx(a, b) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 1.00001f, 2.0f, 2.999f };
        const vec3f b{ 1.0f, 2.0f, 3.0f };
        VX_CHECK(each_greater_approx(a, b) == vec3b(true, false, false));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 1.0f, 2.00001f, 2.0f, 3.0001f };
        const vec4f b{ 1.0f, 2.0f, 2.0f, 3.0f };
        VX_CHECK(each_greater_approx(a, b) == vec4b(false, true, false, true));
    }

    VX_SECTION("custom epsilon")
    {
        const float tight = 1e-8f;
        const float loose = 1e-2f;

        const vec2f a{ 1.001f, 1.0f };
        const vec2f b{ 1.0f, 1.0f };

        VX_CHECK(each_greater_approx(a, b, tight) == vec2b(true, false));
        VX_CHECK(each_greater_approx(a, b, loose) == vec2b(false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_less_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_CHECK(less_approx(0.99999f, 1.0f));
        VX_CHECK(!less_approx(0.9999999f, 1.0f));
        VX_CHECK(!less_approx(1.0f, 1.0f));
        VX_CHECK(!less_approx(1.00001f, 1.0f));
        VX_CHECK(!less_approx(0.0f, constants<f32>::epsilon));

        // int
        VX_CHECK(less_approx(0, 1));
        VX_CHECK(!less_approx(1, 1));
        VX_CHECK(!less_approx(2, 1));
    }

    VX_SECTION("custom epsilon")
    {
        const float tight = 1e-8f;
        const float loose = 1e-2f;

        VX_CHECK(less_approx(0.999f, 1.0f, tight));
        VX_CHECK(!less_approx(0.999f, 1.0f, loose));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_less_approx)
{
    VX_SECTION("vec2")
    {
        const vec2f a{ 0.99999f, 0.5f };
        const vec2f b{ 1.0f, 0.5f };
        VX_CHECK(each_less_approx(a, b) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 0.99999f, 2.0f, 2.999f };
        const vec3f b{ 1.0f, 2.0f, 3.0f };
        VX_CHECK(each_less_approx(a, b) == vec3b(true, false, true));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 1.0f, 1.99999f, 2.0f, 2.999f };
        const vec4f b{ 1.0f, 2.0f, 2.0f, 3.0f };
        VX_CHECK(each_less_approx(a, b) == vec4b(false, true, false, true));
    }

    VX_SECTION("custom epsilon")
    {
        const float tight = 1e-8f;
        const float loose = 1e-2f;

        const vec2f a{ 0.999f, 1.0f };
        const vec2f b{ 1.0f, 1.0f };

        VX_CHECK(each_less_approx(a, b, tight) == vec2b(true, false));
        VX_CHECK(each_less_approx(a, b, loose) == vec2b(false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_greater_or_equal_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_CHECK(greater_or_equal_approx(1.000001f, 1.0f));
        VX_CHECK(greater_or_equal_approx(1.0f, 1.0f));
        VX_CHECK(!greater_or_equal_approx(-0.9999991f, 1.0f));
        VX_CHECK(greater_or_equal_approx(1.0000001f, 1.0f));
        VX_CHECK(greater_or_equal_approx(0.0f, constants<f32>::epsilon));


        // int
        VX_CHECK(greater_or_equal_approx(2, 1));
        VX_CHECK(greater_or_equal_approx(1, 1));
        VX_CHECK(!greater_or_equal_approx(0, 1));
    }

    VX_SECTION("custom epsilon")
    {
        const float loose = 1e-2f;
        const float tight = 1e-8f;

        VX_CHECK(greater_or_equal_approx(0.99f, 1.0f, loose));
        VX_CHECK(!greater_or_equal_approx(0.99f, 1.0f, tight));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_greater_or_equal_approx)
{
    VX_SECTION("vec2")
    {
        const vec2f a{ 1.00001f, 0.999998f };
        const vec2f b{ 1.0f, 1.0f };
        VX_CHECK(each_greater_or_equal_approx(a, b) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 1.0f, 1.0f, 1.0000001f };
        const vec3f b{ 1.0f, 1.1f, 1.0f };
        VX_CHECK(each_greater_or_equal_approx(a, b) == vec3b(true, false, true));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 1.0f, 2.0f, 2.0f, 3.0f };
        const vec4f b{ 0.9999f, 2.0f, 2.1f, 3.000002f };
        VX_CHECK(each_greater_or_equal_approx(a, b) == vec4b(true, true, false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_less_or_equal_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_CHECK(less_or_equal_approx(1.0f, 1.0f));
        VX_CHECK(less_or_equal_approx(0.9999991f, 1.0f)); // inside epsilon
        VX_CHECK(!less_or_equal_approx(1.00001f, 1.0f));  // outside epsilon
        VX_CHECK(less_or_equal_approx(0.999f, 1.0f));
        VX_CHECK(less_or_equal_approx(0.0f, constants<f32>::epsilon));      // On epsilon

        // int
        VX_CHECK(less_or_equal_approx(0, 1));
        VX_CHECK(less_or_equal_approx(1, 1));
        VX_CHECK(!less_or_equal_approx(2, 1));
    }

    VX_SECTION("custom epsilon")
    {
        const float loose = 1e-2f;
        const float tight = 1e-8f;

        VX_CHECK(less_or_equal_approx(1.01f, 1.0f, loose));
        VX_CHECK(!less_or_equal_approx(1.01f, 1.0f, tight));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_each_less_or_equal_approx)
{
    VX_SECTION("vec2")
    {
        const vec2f a{ 0.9999991f, 1.0f };
        const vec2f b{ 1.0f, 0.999f };
        VX_CHECK(each_less_or_equal_approx(a, b) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 0.9f, 1.0f, 1.0000001f };
        const vec3f b{ 1.0f, 1.0f, 1.0f };
        VX_CHECK(each_less_or_equal_approx(a, b) == vec3b(true, true, true));
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 1.0f, 2.0f, 2.0f, 3.0f };
        const vec4f b{ 1.00001f, 2.0f, 1.9f, 3.0f };
        VX_CHECK(each_less_or_equal_approx(a, b) == vec4b(true, true, false, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_identity_approx)
{
    VX_SECTION("mat2")
    {
        VX_CHECK(is_identity_approx(mat2::identity()));
        VX_CHECK(!is_identity_approx(mat2(1.00001f)));
        VX_CHECK(!is_identity_approx(mat2::zero()));
    }

    VX_SECTION("mat3")
    {
        VX_CHECK(is_identity_approx(mat3::identity()));
        VX_CHECK(!is_identity_approx(mat3(1.00001f)));
        VX_CHECK(!is_identity_approx(mat3::zero()));
    }

    VX_SECTION("mat4")
    {
        VX_CHECK(is_identity_approx(mat4::identity()));
        VX_CHECK(!is_identity_approx(mat4(1.00001f)));
        VX_CHECK(!is_identity_approx(mat4::zero()));
    }

    VX_SECTION("quat")
    {
        VX_CHECK(is_identity_approx(quat::identity()));
        VX_CHECK(!is_identity_approx(quat(1.00001f, 0.0f, 0.0f, 0.0f)));
        VX_CHECK(!is_identity_approx(quat::zero()));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_any)
{
    VX_SECTION("vec2")
    {
        // Both components are zero
        const vec2f a{ 0.0f, 0.0f };
        VX_CHECK(any(a) == false);

        // One component is non-zero
        const vec2f b{ 1.0f, 0.0f };
        VX_CHECK(any(b) == true);

        const vec2f c{ 0.0f, 1.0f };
        VX_CHECK(any(c) == true);
    }

    VX_SECTION("vec3")
    {
        // All components are zero
        const vec3f a{ 0.0f, 0.0f, 0.0f };
        VX_CHECK(any(a) == false);

        // One component is non-zero
        const vec3f b{ 1.0f, 0.0f, 0.0f };
        VX_CHECK(any(b) == true);

        const vec3f c{ 0.0f, 1.0f, 0.0f };
        VX_CHECK(any(c) == true);

        const vec3f d{ 0.0f, 0.0f, 1.0f };
        VX_CHECK(any(d) == true);
    }

    VX_SECTION("vec4")
    {
        // All components are zero
        const vec4f a{ 0.0f, 0.0f, 0.0f, 0.0f };
        VX_CHECK(any(a) == false);

        // One component is non-zero
        const vec4f b{ 1.0f, 0.0f, 0.0f, 0.0f };
        VX_CHECK(any(b) == true);

        // Multiple components are non-zero
        const vec4f c{ 1.0f, 1.0f, 0.0f, 0.0f };
        VX_CHECK(any(c) == true);

        const vec4f d{ 0.0f, 0.0f, 1.0f, 1.0f };
        VX_CHECK(any(d) == true);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_all)
{
    VX_SECTION("vec2")
    {
        const vec2f a{ 1.0f, 1.0f };
        VX_CHECK(all(a) == true);

        const vec2f b{ 0.0f, 1.0f };
        VX_CHECK(all(b) == false);

        const vec2f c{ 1.0f, 0.0f };
        VX_CHECK(all(c) == false);

        const vec2f d{ 0.0f, 0.0f };
        VX_CHECK(all(d) == false);
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 1.0f, 2.0f, 3.0f };
        VX_CHECK(all(a) == true);

        const vec3f b{ 0.0f, 1.0f, 2.0f };
        VX_CHECK(all(b) == false);

        const vec3f c{ 1.0f, 0.0f, 2.0f };
        VX_CHECK(all(c) == false);

        const vec3f d{ 1.0f, 2.0f, 0.0f };
        VX_CHECK(all(d) == false);
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 1.0f, 2.0f, 3.0f, 4.0f };
        VX_CHECK(all(a) == true);

        const vec4f b{ 0.0f, 2.0f, 3.0f, 4.0f };
        VX_CHECK(all(b) == false);

        const vec4f c{ 1.0f, 0.0f, 3.0f, 4.0f };
        VX_CHECK(all(c) == false);

        const vec4f d{ 1.0f, 2.0f, 3.0f, 0.0f };
        VX_CHECK(all(d) == false);
    }

}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_none)
{
    VX_SECTION("vec2")
    {
        const vec2f a{ 0.0f, 0.0f };
        VX_CHECK(none(a) == true);

        const vec2f b{ 1.0f, 0.0f };
        VX_CHECK(none(b) == false);

        const vec2f c{ 0.0f, 1.0f };
        VX_CHECK(none(c) == false);

        const vec2f d{ 1.0f, 1.0f };
        VX_CHECK(none(d) == false);
    }

    VX_SECTION("vec3")
    {
        const vec3f a{ 0.0f, 0.0f, 0.0f };
        VX_CHECK(none(a) == true);

        const vec3f b{ 1.0f, 0.0f, 0.0f };
        VX_CHECK(none(b) == false);

        const vec3f c{ 0.0f, 1.0f, 0.0f };
        VX_CHECK(none(c) == false);

        const vec3f d{ 0.0f, 0.0f, 1.0f };
        VX_CHECK(none(d) == false);
    }

    VX_SECTION("vec4")
    {
        const vec4f a{ 0.0f, 0.0f, 0.0f, 0.0f };
        VX_CHECK(none(a) == true);

        const vec4f b{ 1.0f, 0.0f, 0.0f, 0.0f };
        VX_CHECK(none(b) == false);

        const vec4f c{ 0.0f, 1.0f, 0.0f, 0.0f };
        VX_CHECK(none(c) == false);

        const vec4f d{ 0.0f, 0.0f, 0.0f, 1.0f };
        VX_CHECK(none(d) == false);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}