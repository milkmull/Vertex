#include "vertex_test/test.hpp"

#include "vertex/math/fn/comparison.hpp"
#include "vertex/math/vec.hpp"

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

    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 1, 2 };
        constexpr vec2i b{ 1, 2 };
        constexpr vec2i c{ 1, 3 };

        VX_STATIC_CHECK(equal(a, b) == vec2b(true, true));
        VX_STATIC_CHECK(equal(a, c) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 2.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 3.0f };
        constexpr vec3f c{ 1.0f, 2.0f, 4.0f };

        VX_STATIC_CHECK(equal(a, b) == vec3b(true, true, true));
        VX_STATIC_CHECK(equal(a, c) == vec3b(true, true, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d c{ 1.0, 2.0, 0.0, 4.0 };

        VX_STATIC_CHECK(equal(a, b) == vec4b(true, true, true, true));
        VX_STATIC_CHECK(equal(a, c) == vec4b(true, true, false, true));
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

    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 1, 2 };
        constexpr vec2i b{ 1, 2 };
        constexpr vec2i c{ 1, 3 };

        VX_STATIC_CHECK(not_equal(a, b) == vec2b(false, false));
        VX_STATIC_CHECK(not_equal(a, c) == vec2b(false, true));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 2.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 3.0f };
        constexpr vec3f c{ 1.0f, 2.0f, 4.0f };

        VX_STATIC_CHECK(not_equal(a, b) == vec3b(false, false, false));
        VX_STATIC_CHECK(not_equal(a, c) == vec3b(false, false, true));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d c{ 1.0, 2.0, 0.0, 4.0 };

        VX_STATIC_CHECK(not_equal(a, b) == vec4b(false, false, false, false));
        VX_STATIC_CHECK(not_equal(a, c) == vec4b(false, false, true, false));
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

    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 2, 3 };
        constexpr vec2i b{ 1, 3 };
        constexpr vec2i c{ 0, 4 };

        VX_STATIC_CHECK(greater(a, b) == vec2b(true, false));
        VX_STATIC_CHECK(greater(a, c) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 4.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_STATIC_CHECK(greater(a, b) == vec3b(false, true, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 0.0, 2.0, 2.0, 5.0 };

        VX_STATIC_CHECK(greater(a, b) == vec4b(true, false, true, false));
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

    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 2, 3 };
        constexpr vec2i b{ 1, 3 };
        constexpr vec2i c{ 0, 4 };

        VX_STATIC_CHECK(less(a, b) == vec2b(false, false));
        VX_STATIC_CHECK(less(a, c) == vec2b(false, true));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 4.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_STATIC_CHECK(less(a, b) == vec3b(false, false, true));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 0.0, 2.0, 2.0, 5.0 };

        VX_STATIC_CHECK(less(a, b) == vec4b(false, false, false, true));
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

    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 2, 3 };
        constexpr vec2i b{ 1, 3 };
        constexpr vec2i c{ 2, 4 };

        VX_STATIC_CHECK(greater_or_equal(a, b) == vec2b(true, true));
        VX_STATIC_CHECK(greater_or_equal(a, c) == vec2b(true, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 4.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_STATIC_CHECK(greater_or_equal(a, b) == vec3b(true, true, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 0.0, 2.0, 2.0, 4.0 };

        VX_STATIC_CHECK(greater_or_equal(a, b) == vec4b(true, true, true, true));
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

    VX_SECTION("vec2")
    {
        constexpr vec2i a{ 2, 3 };
        constexpr vec2i b{ 1, 3 };
        constexpr vec2i c{ 2, 4 };

        VX_STATIC_CHECK(less_or_equal(a, b) == vec2b(false, true));
        VX_STATIC_CHECK(less_or_equal(a, c) == vec2b(true, true));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 1.0f, 4.0f, 3.0f };
        constexpr vec3f b{ 1.0f, 2.0f, 5.0f };

        VX_STATIC_CHECK(less_or_equal(a, b) == vec3b(true, false, true));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4d a{ 1.0, 2.0, 3.0, 4.0 };
        constexpr vec4d b{ 0.0, 2.0, 2.0, 4.0 };

        VX_STATIC_CHECK(less_or_equal(a, b) == vec4b(false, true, false, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_inf)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(is_inf(const_f::infinity));
        VX_STATIC_CHECK(is_inf(-const_f::infinity));
        VX_STATIC_CHECK(!is_inf(0.0f));
        VX_STATIC_CHECK(!is_inf(1.0));
        VX_STATIC_CHECK(!is_inf(const_f::nan)); // NaN is not infinite

        // int
        VX_STATIC_CHECK(!is_inf(0));
        VX_STATIC_CHECK(!is_inf(123));
        VX_STATIC_CHECK(!is_inf(-456));
    }

    VX_SECTION("vec2")
    {
        constexpr vec2f a{ const_f::infinity, 1.0f };
        constexpr vec2f b{ -const_f::infinity, const_f::infinity };
        constexpr vec2f c{ 0.0f, 42.0f };

        VX_STATIC_CHECK(is_inf(a) == vec2b(true, false));
        VX_STATIC_CHECK(is_inf(b) == vec2b(true, true));
        VX_STATIC_CHECK(is_inf(c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3d a{ 0.0, const_d::infinity, -const_d::infinity };
        constexpr vec3d b{ 1.0, 2.0, 3.0 };

        VX_STATIC_CHECK(is_inf(a) == vec3b(false, true, true));
        VX_STATIC_CHECK(is_inf(b) == vec3b(false, false, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 0.0f, const_f::infinity, 1.0f, -const_f::infinity };
        constexpr vec4f b{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_STATIC_CHECK(is_inf(a) == vec4b(false, true, false, true));
        VX_STATIC_CHECK(is_inf(b) == vec4b(false, false, false, false));
    }

    VX_SECTION("vec int types")
    {
        constexpr vec2i vi{ 1, 2 };
        constexpr vec3i vi3{ 0, -1, 123 };
        constexpr vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_STATIC_CHECK(is_inf(vi) == vec2b(false, false));
        VX_STATIC_CHECK(is_inf(vi3) == vec3b(false, false, false));
        VX_STATIC_CHECK(is_inf(vu4) == vec4b(false, false, false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_any_inf)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ const_f::infinity, 1.0f };
        constexpr vec2f b{ -const_f::infinity, const_f::nan };
        constexpr vec2f c{ 0.0f, 42.0f };

        VX_STATIC_CHECK(any_inf(a) == true);
        VX_STATIC_CHECK(any_inf(b) == true);
        VX_STATIC_CHECK(any_inf(c) == false);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3d a{ const_d::infinity, 2.0, 3.0 };
        constexpr vec3d b{ 1.0, 2.0, const_d::nan };
        constexpr vec3d c{ 0.0, 42.0, -100.0 };

        VX_STATIC_CHECK(any_inf(a) == true);
        VX_STATIC_CHECK(any_inf(b) == false);
        VX_STATIC_CHECK(any_inf(c) == false);
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 1.0f, const_f::infinity, 0.0f, 4.0f };
        constexpr vec4f b{ -const_f::infinity, 2.0f, const_f::nan, 3.0f };
        constexpr vec4f c{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_STATIC_CHECK(any_inf(a) == true);
        VX_STATIC_CHECK(any_inf(b) == true);
        VX_STATIC_CHECK(any_inf(c) == false);
    }

    VX_SECTION("vec int types")
    {
        constexpr vec2i vi{ 1, 2 };
        constexpr vec3i vi3{ 0, -1, 123 };
        constexpr vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_STATIC_CHECK(any_inf(vi) == false);
        VX_STATIC_CHECK(any_inf(vi3) == false);
        VX_STATIC_CHECK(any_inf(vu4) == false);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_nan)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(is_nan(const_f::nan));
        VX_STATIC_CHECK(!is_nan(const_f::infinity));
        VX_STATIC_CHECK(!is_nan(-const_f::infinity));
        VX_STATIC_CHECK(!is_nan(0.0f));
        VX_STATIC_CHECK(!is_nan(1.0f));

        // int
        VX_STATIC_CHECK(!is_nan(0));
        VX_STATIC_CHECK(!is_nan(123));
        VX_STATIC_CHECK(!is_nan(-456));
    }

    VX_SECTION("vec2")
    {
        constexpr vec2f a{ const_f::nan, 1.0f };
        constexpr vec2f b{ -const_f::infinity, const_f::nan };
        constexpr vec2f c{ 0.0f, 42.0f };

        VX_STATIC_CHECK(is_nan(a) == vec2b(true, false));
        VX_STATIC_CHECK(is_nan(b) == vec2b(false, true));
        VX_STATIC_CHECK(is_nan(c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3d a{ 0.0, const_d::nan, -const_d::infinity };
        constexpr vec3d b{ 1.0, 2.0, 3.0 };

        VX_STATIC_CHECK(is_nan(a) == vec3b(false, true, false));
        VX_STATIC_CHECK(is_nan(b) == vec3b(false, false, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 0.0f, const_f::nan, 1.0f, -const_f::infinity };
        constexpr vec4f b{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_STATIC_CHECK(is_nan(a) == vec4b(false, true, false, false));
        VX_STATIC_CHECK(is_nan(b) == vec4b(false, false, false, false));
    }

    VX_SECTION("vec int types")
    {
        constexpr vec2i vi{ 1, 2 };
        constexpr vec3i vi3{ 0, -1, 123 };
        constexpr vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_STATIC_CHECK(is_nan(vi) == vec2b(false, false));
        VX_STATIC_CHECK(is_nan(vi3) == vec3b(false, false, false));
        VX_STATIC_CHECK(is_nan(vu4) == vec4b(false, false, false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_any_nan)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ const_f::nan, 1.0f };
        constexpr vec2f b{ -const_f::infinity, const_f::nan };
        constexpr vec2f c{ 0.0f, 42.0f };

        VX_STATIC_CHECK(any_nan(a) == true);
        VX_STATIC_CHECK(any_nan(b) == true);
        VX_STATIC_CHECK(any_nan(c) == false);
    }

    VX_SECTION("vec3")
    {
        constexpr vec3d a{ const_d::nan, 2.0, 3.0 };
        constexpr vec3d b{ 1.0, 2.0, const_d::infinity };
        constexpr vec3d c{ 0.0, 42.0, -100.0 };

        VX_STATIC_CHECK(any_nan(a) == true);
        VX_STATIC_CHECK(any_nan(b) == false);
        VX_STATIC_CHECK(any_nan(c) == false);
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 1.0f, const_f::nan, 0.0f, 4.0f };
        constexpr vec4f b{ -const_f::infinity, 2.0f, const_f::nan, 3.0f };
        constexpr vec4f c{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_STATIC_CHECK(any_nan(a) == true);
        VX_STATIC_CHECK(any_nan(b) == true);
        VX_STATIC_CHECK(any_nan(c) == false);
    }

    VX_SECTION("vec int types")
    {
        constexpr vec2i vi{ 1, 2 };
        constexpr vec3i vi3{ 0, -1, 123 };
        constexpr vec4u vu4{ 0u, 1u, 2u, 3u };

        VX_STATIC_CHECK(any_nan(vi) == false);
        VX_STATIC_CHECK(any_nan(vi3) == false);
        VX_STATIC_CHECK(any_nan(vu4) == false);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_is_zero_approx)
{
    VX_SECTION("scalar")
    {
        // float
        VX_STATIC_CHECK(is_zero_approx(0.0f));
        VX_STATIC_CHECK(is_zero_approx(0.0000009f));    // Approx zero within epsilon
        VX_STATIC_CHECK(!is_zero_approx(1.0f));         // Not zero
        VX_STATIC_CHECK(!is_zero_approx(-0.000001f));   // Not zero within epsilon

        // int
        VX_STATIC_CHECK(is_zero_approx(0));
        VX_STATIC_CHECK(!is_zero_approx(123));
        VX_STATIC_CHECK(!is_zero_approx(-456));
    }

    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 0.0f, 1.0f };
        constexpr vec2f b{ 0.0000001f, 0.0f };  // Within epsilon
        constexpr vec2f c{ 1.0f, -1.0f };

        VX_STATIC_CHECK(is_zero_approx(a) == vec2b(true, false));
        VX_STATIC_CHECK(is_zero_approx(b) == vec2b(true, true));
        VX_STATIC_CHECK(is_zero_approx(c) == vec2b(false, false));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3d a{ 0.0, 0.000000001, 1.0 };
        constexpr vec3d b{ -0.00000005, 2.0, -2.0 };
        constexpr vec3d c{ 0.1, 0.2, 0.3 };

        VX_STATIC_CHECK(is_zero_approx(a) == vec3b(true, true, false));
        VX_STATIC_CHECK(is_zero_approx(b) == vec3b(true, false, false));
        VX_STATIC_CHECK(is_zero_approx(c) == vec3b(false, false, false));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 0.0f, 0.0000000001f, -0.0000000001f, 1.0f };
        constexpr vec4f b{ 0.0000001f, 2.0f, 0.0f, 4.0f };
        constexpr vec4f c{ 0.1f, 0.2f, 0.3f, 0.4f };

        VX_STATIC_CHECK(is_zero_approx(a) == vec4b(true, true, true, false));
        VX_STATIC_CHECK(is_zero_approx(b) == vec4b(true, false, true, false));
        VX_STATIC_CHECK(is_zero_approx(c) == vec4b(false, false, false, false));
    }

    VX_SECTION("vec int types")
    {
        constexpr vec2i vi{ 0, 0 };
        constexpr vec3i vi3{ 0, 123, -1 };
        constexpr vec4u vu4{ 0u, 0u, 0u, 0u };

        VX_STATIC_CHECK(is_zero_approx(vi) == vec2b(true, true));
        VX_STATIC_CHECK(is_zero_approx(vi3) == vec3b(true, false, false));
        VX_STATIC_CHECK(is_zero_approx(vu4) == vec4b(true, true, true, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_all_zero_approx)
{
    VX_SECTION("vec2")
    {
        constexpr vec2f a{ 0.0f, 0.0f };       // Exactly zero
        constexpr vec2f b{ 0.0000001f, 0.0f }; // Within epsilon
        constexpr vec2f c{ 1.0f, 0.0f };       // Not all zero approx

        VX_STATIC_CHECK(all_zero_approx(a));
        VX_STATIC_CHECK(all_zero_approx(b));
        VX_STATIC_CHECK(!all_zero_approx(c));
    }

    VX_SECTION("vec3")
    {
        constexpr vec3f a{ 0.0f, 0.0f, 0.0f };              // Exactly zero
        constexpr vec3f b{ 0.0000001f, 0.0f, 0.0f };        // Approx zero
        constexpr vec3f c{ 1.0f, 0.0f, 0.0f };              // Not all zero approx
        constexpr vec3f d{ 0.0000009f, 0.0000009f, 0.0f };   // Approx zero for all

        VX_STATIC_CHECK(all_zero_approx(a));
        VX_STATIC_CHECK(all_zero_approx(b));
        VX_STATIC_CHECK(!all_zero_approx(c));
        VX_STATIC_CHECK(all_zero_approx(d));
    }

    VX_SECTION("vec4")
    {
        constexpr vec4f a{ 0.0f, 0.0f, 0.0f, 0.0f };                    // Exactly zero
        constexpr vec4f b{ 0.0000001f, 0.0f, 0.0f, 0.0f };              // Approx zero
        constexpr vec4f c{ 1.0f, 0.0f, 0.0f, 0.0f };                    // Not all zero approx
        constexpr vec4f d{ 0.0000009f, 0.0000009f, 0.0000009f, 0.0f };  // Approx zero for all

        VX_STATIC_CHECK(all_zero_approx(a));
        VX_STATIC_CHECK(all_zero_approx(b));
        VX_STATIC_CHECK(!all_zero_approx(c));
        VX_STATIC_CHECK(all_zero_approx(d));
    }
}


///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}