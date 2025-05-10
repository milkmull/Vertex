#include "vertex_test/test.hpp"

#include "vertex/math/types/vec/vec2b.hpp"
#include "vertex/math/types/vec/vec2i.hpp"
#include "vertex/math/types/vec/vec2u.hpp"
#include "vertex/math/types/vec/vec2f.hpp"
#include "vertex/math/types/vec/vec2d.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

#define check_each(vec, vx, vy) VX_STATIC_CHECK(vec.x == vx && vec.y == vy)

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_traits)
{
    VX_SECTION("scalar types")
    {
        VX_STATIC_CHECK((is_same<typename vec2b::scalar_type, b>::value));
        VX_STATIC_CHECK((is_same<typename vec2i::scalar_type, i>::value));
        VX_STATIC_CHECK((is_same<typename vec2u::scalar_type, u>::value));
        VX_STATIC_CHECK((is_same<typename vec2f::scalar_type, f>::value));
        VX_STATIC_CHECK((is_same<typename vec2d::scalar_type, d>::value));
    }

    VX_SECTION("sizes")
    {
        VX_STATIC_CHECK(sizeof(vec2b) == 2);
        VX_STATIC_CHECK(sizeof(vec2i) == 8);
        VX_STATIC_CHECK(sizeof(vec2u) == 8);
        VX_STATIC_CHECK(sizeof(vec2f) == 8);
        VX_STATIC_CHECK(sizeof(vec2d) == 16);

        VX_STATIC_CHECK(vec2b::size == 2);
        VX_STATIC_CHECK(vec2i::size == 2);
        VX_STATIC_CHECK(vec2u::size == 2);
        VX_STATIC_CHECK(vec2f::size == 2);
        VX_STATIC_CHECK(vec2d::size == 2);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec2b)
{
    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec2b(), false, false);
        check_each(vec2b(vec2b(true, false)), true, false);

        // explicit constructors
        check_each(vec2b(true), true, true);
        check_each(vec2b(true, false), true, false);

        // conversion constructors
        check_each(vec2b(1), true, true);
        check_each(vec2b(1u), true, true);
        check_each(vec2b(1.0f), true, true);
        check_each(vec2b(1.0), true, true);

        check_each(vec2b(0, 1), false, true);
        check_each(vec2b(0u, 1u), false, true);
        check_each(vec2b(0.0f, 1.0f), false, true);
        check_each(vec2b(0.0, 1.0), false, true);

        check_each(static_cast<vec2b>(vec2i(0, 1)), false, true);
        check_each(static_cast<vec2b>(vec2u(0u, 1u)), false, true);
        check_each(static_cast<vec2b>(vec2f(0.0f, 1.0f)), false, true);
        check_each(static_cast<vec2b>(vec2d(0.0, 1.0)), false, true);
    }

    VX_SECTION("assignment")
    {
        {
            vec2b v1(false, true);
            vec2b v2(true, false);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
        }

        {
            vec2b v1(false, true);
            vec2i v2(1, 0);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
        }

        {
            vec2b v1(false, true);
            vec2u v2(1, 0);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
        }

        {
            vec2b v1(false, true);
            vec2f v2(1.0f, 0.0f);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
        }

        {
            vec2b v1(false, true);
            vec2d v2(1.0, 0.0);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec2b(false, true)[0] == false);
        VX_STATIC_CHECK(vec2b(false, true)[1] == true);

        vec2b v;

        v[0] = true;
        VX_CHECK(v[0] == true);
        v[1] = true;
        VX_CHECK(v[1] == true);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec2b() == vec2b());
        VX_STATIC_CHECK(vec2b() != vec2b(true));
    }

    VX_SECTION("boolean operators")
    {
        constexpr vec2b v_true(true, true);
        constexpr vec2b v_false(false, false);
        constexpr vec2b v_mixed(true, false);

        // && with scalar
        VX_STATIC_CHECK((v_true && true) == vec2b(true, true));
        VX_STATIC_CHECK((v_true && false) == vec2b(false, false));
        VX_STATIC_CHECK((v_false && true) == vec2b(false, false));
        VX_STATIC_CHECK((v_mixed && true) == vec2b(true, false));
        VX_STATIC_CHECK((true && v_mixed) == vec2b(true, false));
        VX_STATIC_CHECK((false && v_mixed) == vec2b(false, false));

        // && with vector
        VX_STATIC_CHECK((v_mixed && v_true) == vec2b(true, false));
        VX_STATIC_CHECK((v_mixed && v_false) == vec2b(false, false));
        VX_STATIC_CHECK((v_true && v_true) == vec2b(true, true));

        // || with scalar
        VX_STATIC_CHECK((v_false || false) == vec2b(false, false));
        VX_STATIC_CHECK((v_false || true) == vec2b(true, true));
        VX_STATIC_CHECK((v_mixed || false) == vec2b(true, false));
        VX_STATIC_CHECK((v_mixed || true) == vec2b(true, true));
        VX_STATIC_CHECK((true || v_false) == vec2b(true, true));
        VX_STATIC_CHECK((false || v_mixed) == vec2b(true, false));

        // || with vector
        VX_STATIC_CHECK((v_mixed || v_false) == vec2b(true, false));
        VX_STATIC_CHECK((v_mixed || v_true) == vec2b(true, true));
        VX_STATIC_CHECK((v_false || v_false) == vec2b(false, false));

        // ! operator
        VX_STATIC_CHECK((!v_true) == vec2b(false, false));
        VX_STATIC_CHECK((!v_false) == vec2b(true, true));
        VX_STATIC_CHECK((!v_mixed) == vec2b(false, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec2b::TRUE() == vec2b(true, true));
        VX_STATIC_CHECK(vec2b::FALSE() == vec2b(false, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}