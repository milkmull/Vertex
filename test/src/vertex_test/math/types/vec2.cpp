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
            vec2u v2(1u, 0u);

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

VX_TEST_CASE(test_vec2i)
{
    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec2i(), 0, 0);
        check_each(vec2i(vec2i(1, 2)), 1, 2);

        // explicit constructors
        check_each(vec2i(1), 1, 1);
        check_each(vec2i(1, 2), 1, 2);

        // conversion constructors
        check_each(vec2i(true), 1, 1);
        check_each(vec2i(5u), 5, 5);
        check_each(vec2i(5.0f), 5, 5);
        check_each(vec2i(5.0), 5, 5);

        check_each(vec2i(false, true), 0, 1);
        check_each(vec2i(1u, 2u), 1, 2);
        check_each(vec2i(1.5f, 2.5f), 1, 2);
        check_each(vec2i(0.5, 1.5), 0, 1);

        check_each(static_cast<vec2i>(vec2b(false, true)), 0, 1);
        check_each(static_cast<vec2i>(vec2u(1u, 2u)), 1, 2);
        check_each(static_cast<vec2i>(vec2f(1.5f, 2.5f)), 1, 2);
        check_each(static_cast<vec2i>(vec2d(0.5, 1.5)), 0, 1);
    }

    VX_SECTION("assignment")
    {
        {
            vec2i v1(0, 1);
            vec2b v2(true, false);

            v1 = v2;
            VX_CHECK(v1.x == 1);
            VX_CHECK(v1.y == 0);
        }

        {
            vec2i v1(1, 2);
            vec2i v2(2, 1);

            v1 = v2;
            VX_CHECK(v1.x == 2);
            VX_CHECK(v1.y == 1);
        }

        {
            vec2i v1(1, 2);
            vec2u v2(2u, 1u);

            v1 = v2;
            VX_CHECK(v1.x == 2);
            VX_CHECK(v1.y == 1);
        }

        {
            vec2i v1(1, 2);
            vec2f v2(2.0f, 1.0f);

            v1 = v2;
            VX_CHECK(v1.x == 2);
            VX_CHECK(v1.y == 1);
        }

        {
            vec2i v1(1, 2);
            vec2d v2(2.0, 1.0);

            v1 = v2;
            VX_CHECK(v1.x == 2);
            VX_CHECK(v1.y == 1);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec2i(1, 2)[0] == 1);
        VX_STATIC_CHECK(vec2i(1, 2)[1] == 2);

        vec2i v;

        v[0] = 1;
        VX_CHECK(v[0] == 1);
        v[1] = 2;
        VX_CHECK(v[1] == 2);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec2i(1) == vec2i(1));
        VX_STATIC_CHECK(vec2i(1) != vec2i(2));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec2i(1) == vec2i(1));
        VX_STATIC_CHECK(+vec2i(-1) == vec2i(-1));

        VX_STATIC_CHECK(-vec2i(1) == vec2i(-1));
        VX_STATIC_CHECK(-vec2i(-1) == vec2i(1));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec2i(0) == vec2i(1));
        VX_STATIC_CHECK(--vec2i(0) == vec2i(-1));

        vec2i v(0);

        VX_CHECK(v++ == vec2i(0));
        VX_CHECK(v == vec2i(1));
        VX_CHECK(v-- == vec2i(1));
        VX_CHECK(v == vec2i(0));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec2i(1, 2) + 5 == vec2i(6, 7));
        VX_STATIC_CHECK(5 + vec2i(1, 2) == vec2i(6, 7));
        VX_STATIC_CHECK(vec2i(5) + vec2i(1, 2) == vec2i(6, 7));
        VX_STATIC_CHECK(vec2i(1, 2) + vec2i(5) == vec2i(6, 7));

        // subtraction
        VX_STATIC_CHECK(vec2i(1, 2) - 5 == vec2i(-4, -3));
        VX_STATIC_CHECK(5 - vec2i(1, 2) == vec2i(4, 3));
        VX_STATIC_CHECK(vec2i(5) - vec2i(1, 2) == vec2i(4, 3));
        VX_STATIC_CHECK(vec2i(1, 2) - vec2i(5) == vec2i(-4, -3));

        // multiplication
        VX_STATIC_CHECK(vec2i(1, 2) * 5 == vec2i(5, 10));
        VX_STATIC_CHECK(5 * vec2i(1, 2) == vec2i(5, 10));
        VX_STATIC_CHECK(vec2i(5) * vec2i(1, 2) == vec2i(5, 10));
        VX_STATIC_CHECK(vec2i(1, 2) * vec2i(5) == vec2i(5, 10));

        // division
        VX_STATIC_CHECK(vec2i(5, 10) / 5 == vec2i(1, 2));
        VX_STATIC_CHECK(10 / vec2i(10, 5) == vec2i(1, 2));
        VX_STATIC_CHECK(vec2i(10) / vec2i(10, 5) == vec2i(1, 2));
        VX_STATIC_CHECK(vec2i(10, 5) / vec2i(5, 10) == vec2i(2, 0));

        // modulo
        VX_STATIC_CHECK(vec2i(5, 10) % 4 == vec2i(1, 2));
        VX_STATIC_CHECK(10 % vec2i(3, 4) == vec2i(1, 2));
        VX_STATIC_CHECK(vec2i(10) % vec2i(3, 4) == vec2i(1, 2));
        VX_STATIC_CHECK(vec2i(10, 5) % vec2i(4, 3) == vec2i(2, 2));
    }

    VX_SECTION("binary bit operators")
    {
        // and
        VX_STATIC_CHECK((vec2i(5, 6) & 3) == vec2i(1, 2));
        VX_STATIC_CHECK((3 & vec2i(5, 6)) == vec2i(1, 2));
        VX_STATIC_CHECK((vec2i(5, 6) & vec2i(3, 3)) == vec2i(1, 2));

        // or
        VX_STATIC_CHECK((vec2i(5, 6) | 3) == vec2i(7, 7));
        VX_STATIC_CHECK((3 | vec2i(5, 6)) == vec2i(7, 7));
        VX_STATIC_CHECK((vec2i(5, 6) | vec2i(3, 3)) == vec2i(7, 7));

        // xor
        VX_STATIC_CHECK((vec2i(5, 6) ^ 3) == vec2i(6, 5));
        VX_STATIC_CHECK((3 ^ vec2i(5, 6)) == vec2i(6, 5));
        VX_STATIC_CHECK((vec2i(5, 6) ^ vec2i(3, 3)) == vec2i(6, 5));

        // left shift
        VX_STATIC_CHECK((vec2i(1, 2) << 1) == vec2i(2, 4));
        VX_STATIC_CHECK((1 << vec2i(1, 2)) == vec2i(2, 4));
        VX_STATIC_CHECK((vec2i(1, 2) << vec2i(1, 2)) == vec2i(2, 8));

        // right shift
        VX_STATIC_CHECK((vec2i(4, 8) >> 1) == vec2i(2, 4));
        VX_STATIC_CHECK((8 >> vec2i(2, 3)) == vec2i(2, 1));
        VX_STATIC_CHECK((vec2i(8, 16) >> vec2i(2, 3)) == vec2i(2, 2));

        // not
        VX_STATIC_CHECK((~vec2i(0, -1)) == vec2i(~0, ~(-1)));
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec2i v;
            v += 5;
            VX_CHECK(v == vec2i(5));
            v += vec2i(1, 2);
            VX_CHECK(v == vec2i(6, 7));
        }

        // subtraction
        {
            vec2i v(10, 20);
            v -= 5;
            VX_CHECK(v == vec2i(5, 15));
            v -= vec2i(1, 2);
            VX_CHECK(v == vec2i(4, 13));
        }

        // multiplication
        {
            vec2i v(1, 2);
            v *= 5;
            VX_CHECK(v == vec2i(5, 10));
            v *= vec2i(2, 3);
            VX_CHECK(v == vec2i(10, 30));
        }

        // division
        {
            vec2i v(100, 50);
            v /= 5;
            VX_CHECK(v == vec2i(20, 10));
            v /= vec2i(2, 5);
            VX_CHECK(v == vec2i(10, 2));
        }

        // modulo
        {
            vec2i v(10, 20);
            v %= 4;
            VX_CHECK(v == vec2i(2, 0));
            v %= vec2i(2, 3);
            VX_CHECK(v == vec2i(0, 0));
        }
    }

    VX_SECTION("unary bit operators")
    {
        // and
        {
            vec2i v(7, 14);
            v &= 3;
            VX_CHECK(v == vec2i(3, 2));
            v &= vec2i(1, 2);
            VX_CHECK(v == vec2i(1, 2));
        }

        // or
        {
            vec2i v(1, 2);
            v |= 4;
            VX_CHECK(v == vec2i(5, 6));
            v |= vec2i(2, 1);
            VX_CHECK(v == vec2i(7, 7));
        }

        // xor
        {
            vec2i v(5, 6);
            v ^= 3;
            VX_CHECK(v == vec2i(6, 5));
            v ^= vec2i(1, 1);
            VX_CHECK(v == vec2i(7, 4));
        }

        // left shift
        {
            vec2i v(1, 2);
            v <<= 1;
            VX_CHECK(v == vec2i(2, 4));
            v <<= vec2i(1, 2);
            VX_CHECK(v == vec2i(4, 16));
        }

        // right shift
        {
            vec2i v(8, 16);
            v >>= 1;
            VX_CHECK(v == vec2i(4, 8));
            v >>= vec2i(2, 3);
            VX_CHECK(v == vec2i(1, 1));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec2i(1, 0) && vec2i(1, 2)) == vec2b(true, false));
        VX_STATIC_CHECK((vec2i(0, 0) && vec2i(1, 1)) == vec2b(false, false));

        // or
        VX_STATIC_CHECK((vec2i(1, 0) || vec2i(0, 1)) == vec2b(true, true));
        VX_STATIC_CHECK((vec2i(0, 0) || vec2i(0, 1)) == vec2b(false, true));

        // not
        VX_STATIC_CHECK((!vec2i(1, 0)) == vec2b(false, true));
        VX_STATIC_CHECK((!vec2i(0, 0)) == vec2b(true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec2i::ZERO() == vec2i(0, 0));
        VX_STATIC_CHECK(vec2i::ONE() == vec2i(1, 1));

        VX_STATIC_CHECK(vec2i::RIGHT() == vec2i(1, 0));
        VX_STATIC_CHECK(vec2i::LEFT() == vec2i(-1, 0));

        VX_STATIC_CHECK(vec2i::UP() == vec2i(0, 1));
        VX_STATIC_CHECK(vec2i::DOWN() == vec2i(0, -1));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}