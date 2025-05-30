#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

#define check_each(vec, vx, vy) VX_STATIC_CHECK(vec.x == vx && vec.y == vy)

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec2b)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec2b::scalar_type, bool>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec2b) == 2);
        VX_STATIC_CHECK(vec2b::size == 2);
    }

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

        // mixed
        check_each(vec2b(false, 1), false, true);
        check_each(vec2b(0.5f, 1u), true, true);
        check_each(vec2b(0, true), false, true);
        check_each(vec2b(0.0, 1), false, true);

        // vec2
        check_each(static_cast<vec2b>(vec2i(0, 1)), false, true);
        check_each(static_cast<vec2b>(vec2u(0u, 1u)), false, true);
        check_each(static_cast<vec2b>(vec2f(0.0f, 1.0f)), false, true);
        check_each(static_cast<vec2b>(vec2d(0.0, 1.0)), false, true);

        // vec3
        check_each(static_cast<vec2b>(vec3b(true, false, true)), true, false);
        check_each(static_cast<vec2b>(vec3i(0, 1, 0)), false, true);
        check_each(static_cast<vec2b>(vec3u(0u, 1u, 0u)), false, true);
        check_each(static_cast<vec2b>(vec3f(0.0f, 1.0f, 0.0f)), false, true);
        check_each(static_cast<vec2b>(vec3d(0.0, 1.0, 0.0)), false, true);

        // vec4
        check_each(static_cast<vec2b>(vec4b(true, false, true, false)), true, false);
        check_each(static_cast<vec2b>(vec4i(0, 1, 0, 1)), false, true);
        check_each(static_cast<vec2b>(vec4u(0u, 1u, 0u, 1u)), false, true);
        check_each(static_cast<vec2b>(vec4f(0.0f, 1.0f, 0.0f, 1.0f)), false, true);
        check_each(static_cast<vec2b>(vec4d(0.0, 1.0, 0.0, 1.0)), false, true);
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
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec2i)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec2i::scalar_type, i32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec2i) == 8);
        VX_STATIC_CHECK(vec2i::size == 2);
    }

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

        // mixed
        check_each(vec2i(false, 1), 0, 1);
        check_each(vec2i(7.5f, 1u), 7, 1);
        check_each(vec2i(100u, true), 100, 1);
        check_each(vec2i(-75.5, 1), -75, 1);

        // vec2
        check_each(static_cast<vec2i>(vec2b(false, true)), 0, 1);
        check_each(static_cast<vec2i>(vec2u(1u, 2u)), 1, 2);
        check_each(static_cast<vec2i>(vec2f(1.5f, 2.5f)), 1, 2);
        check_each(static_cast<vec2i>(vec2d(0.5, 1.5)), 0, 1);

        // vec3
        check_each(static_cast<vec2i>(vec3b(true, false, true)), 1, 0);
        check_each(static_cast<vec2i>(vec3i(0, 1, 2)), 0, 1);
        check_each(static_cast<vec2i>(vec3u(0u, 1u, 2u)), 0, 1);
        check_each(static_cast<vec2i>(vec3f(0.0f, 1.0f, 2.0f)), 0, 1);
        check_each(static_cast<vec2i>(vec3d(0.0, 1.0, 2.0)), 0, 1);

        // vec4
        check_each(static_cast<vec2i>(vec4b(true, false, true, false)), 1, 0);
        check_each(static_cast<vec2i>(vec4i(0, 1, 2, 3)), 0, 1);
        check_each(static_cast<vec2i>(vec4u(0u, 1u, 2u, 3u)), 0, 1);
        check_each(static_cast<vec2i>(vec4f(0.0f, 1.0f, 2.0f, 3.0f)), 0, 1);
        check_each(static_cast<vec2i>(vec4d(0.0, 1.0, 2.0, 3.0)), 0, 1);
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
        VX_STATIC_CHECK(vec2i::zero() == vec2i(0, 0));
        VX_STATIC_CHECK(vec2i::one() == vec2i(1, 1));

        VX_STATIC_CHECK(vec2i::right() == vec2i(1, 0));
        VX_STATIC_CHECK(vec2i::left() == vec2i(-1, 0));

        VX_STATIC_CHECK(vec2i::up() == vec2i(0, 1));
        VX_STATIC_CHECK(vec2i::down() == vec2i(0, -1));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec2u)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec2u::scalar_type, u32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec2u) == 8);
        VX_STATIC_CHECK(vec2u::size == 2);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec2u(), 0u, 0u);
        check_each(vec2u(vec2u(1u, 2u)), 1u, 2u);

        // explicit constructors
        check_each(vec2u(1u), 1u, 1u);
        check_each(vec2u(1u, 2u), 1u, 2u);

        // conversion constructors
        check_each(vec2u(true), 1u, 1u);
        check_each(vec2u(5), 5u, 5u);
        check_each(vec2u(-5), 4294967291u, 4294967291u);
        check_each(vec2u(5.0f), 5u, 5u);
        check_each(vec2u(5.0), 5u, 5u);

        // mixed
        check_each(vec2u(false, 1), 0u, 1u);
        check_each(vec2u(7.5f, 1u), 7u, 1u);
        check_each(vec2u(100u, true), 100u, 1u);
        check_each(vec2u(75.5, 7), 75u, 7u);

        // vec2
        check_each(static_cast<vec2u>(vec2b(false, true)), 0u, 1u);
        check_each(static_cast<vec2u>(vec2i(1, 2)), 1u, 2u);
        check_each(static_cast<vec2u>(vec2f(1.5f, 2.5f)), 1u, 2u);
        check_each(static_cast<vec2u>(vec2d(0.5, 1.5)), 0u, 1u);

        // vec3
        check_each(static_cast<vec2u>(vec3b(true, false, true)), 1u, 0u);
        check_each(static_cast<vec2u>(vec3i(0, 1, 2)), 0u, 1u);
        check_each(static_cast<vec2u>(vec3u(0u, 1u, 2u)), 0u, 1u);
        check_each(static_cast<vec2u>(vec3f(0.0f, 1.0f, 2.0f)), 0u, 1u);
        check_each(static_cast<vec2u>(vec3d(0.0, 1.0, 2.0)), 0u, 1u);

        // vec4
        check_each(static_cast<vec2u>(vec4b(true, false, true, false)), 1u, 0u);
        check_each(static_cast<vec2u>(vec4i(0, 1, 2, 3)), 0u, 1u);
        check_each(static_cast<vec2u>(vec4u(0u, 1u, 2u, 3u)), 0u, 1u);
        check_each(static_cast<vec2u>(vec4f(0.0f, 1.0f, 2.0f, 3.0f)), 0u, 1u);
        check_each(static_cast<vec2u>(vec4d(0.0, 1.0, 2.0, 3.0)), 0u, 1u);
    }

    VX_SECTION("assignment")
    {
        {
            vec2u v1(0u, 1u);
            vec2b v2(true, false);

            v1 = v2;
            VX_CHECK(v1.x == 1u);
            VX_CHECK(v1.y == 0u);
        }

        {
            vec2u v1(1u, 2u);
            vec2i v2(2, 1);

            v1 = v2;
            VX_CHECK(v1.x == 2u);
            VX_CHECK(v1.y == 1u);
        }

        {
            vec2u v1(1u, 2u);
            vec2u v2(2u, 1u);

            v1 = v2;
            VX_CHECK(v1.x == 2u);
            VX_CHECK(v1.y == 1u);
        }

        {
            vec2u v1(1u, 2u);
            vec2f v2(2.0f, 1.0f);

            v1 = v2;
            VX_CHECK(v1.x == 2u);
            VX_CHECK(v1.y == 1u);
        }

        {
            vec2u v1(1u, 2u);
            vec2d v2(2.0, 1.0);

            v1 = v2;
            VX_CHECK(v1.x == 2u);
            VX_CHECK(v1.y == 1u);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec2u(1, 2)[0] == 1u);
        VX_STATIC_CHECK(vec2u(1, 2)[1] == 2u);

        vec2u v;

        v[0] = 1u;
        VX_CHECK(v[0] == 1u);
        v[1] = 2u;
        VX_CHECK(v[1] == 2u);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec2u(1u) == vec2u(1u));
        VX_STATIC_CHECK(vec2u(1u) != vec2u(2u));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec2u(1u) == vec2u(1u));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec2u(0u) == vec2u(1u));
        VX_STATIC_CHECK(--vec2u(1u) == vec2u(0u));

        vec2u v(0u);

        VX_CHECK(v++ == vec2u(0u));
        VX_CHECK(v == vec2u(1u));
        VX_CHECK(v-- == vec2u(1u));
        VX_CHECK(v == vec2u(0u));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec2u(1u, 2u) + 5u == vec2u(6u, 7u));
        VX_STATIC_CHECK(5u + vec2u(1u, 2u) == vec2u(6u, 7u));
        VX_STATIC_CHECK(vec2u(5u) + vec2u(1u, 2u) == vec2u(6u, 7u));
        VX_STATIC_CHECK(vec2u(1u, 2u) + vec2u(5u) == vec2u(6u, 7u));

        // subtraction (vec2u) - avoids underflow
        VX_STATIC_CHECK(vec2u(6u, 7u) - 5u == vec2u(1u, 2u));
        VX_STATIC_CHECK(10u - vec2u(6u, 7u) == vec2u(4u, 3u));
        VX_STATIC_CHECK(vec2u(10u) - vec2u(6u, 7u) == vec2u(4u, 3u));
        VX_STATIC_CHECK(vec2u(6u, 7u) - vec2u(5u) == vec2u(1u, 2u));

        // multiplication
        VX_STATIC_CHECK(vec2u(1u, 2u) * 5u == vec2u(5u, 10u));
        VX_STATIC_CHECK(5u * vec2u(1u, 2u) == vec2u(5u, 10u));
        VX_STATIC_CHECK(vec2u(5u) * vec2u(1u, 2u) == vec2u(5u, 10u));
        VX_STATIC_CHECK(vec2u(1u, 2u) * vec2u(5u) == vec2u(5u, 10u));

        // division
        VX_STATIC_CHECK(vec2u(5u, 10u) / 5u == vec2u(1u, 2u));
        VX_STATIC_CHECK(10u / vec2u(10u, 5u) == vec2u(1u, 2u));
        VX_STATIC_CHECK(vec2u(10u) / vec2u(10u, 5u) == vec2u(1u, 2u));
        VX_STATIC_CHECK(vec2u(10u, 5u) / vec2u(5u, 10u) == vec2u(2u, 0u));

        // modulo
        VX_STATIC_CHECK(vec2u(5u, 10u) % 4u == vec2u(1u, 2u));
        VX_STATIC_CHECK(10u % vec2u(3u, 4u) == vec2u(1u, 2u));
        VX_STATIC_CHECK(vec2u(10u) % vec2u(3u, 4u) == vec2u(1u, 2u));
        VX_STATIC_CHECK(vec2u(10u, 5u) % vec2u(4u, 3u) == vec2u(2u, 2u));
    }

    VX_SECTION("binary bit operators")
    {
        // and
        VX_STATIC_CHECK((vec2u(5u, 6u) & 3u) == vec2u(1u, 2u));
        VX_STATIC_CHECK((3u & vec2u(5u, 6u)) == vec2u(1u, 2u));
        VX_STATIC_CHECK((vec2u(5u, 6u) & vec2u(3u, 3u)) == vec2u(1u, 2u));

        // or
        VX_STATIC_CHECK((vec2u(5u, 6u) | 3u) == vec2u(7u, 7u));
        VX_STATIC_CHECK((3u | vec2u(5u, 6u)) == vec2u(7u, 7u));
        VX_STATIC_CHECK((vec2u(5u, 6u) | vec2u(3u, 3u)) == vec2u(7u, 7u));

        // xor
        VX_STATIC_CHECK((vec2u(5u, 6u) ^ 3u) == vec2u(6u, 5u));
        VX_STATIC_CHECK((3u ^ vec2u(5u, 6u)) == vec2u(6u, 5u));
        VX_STATIC_CHECK((vec2u(5u, 6u) ^ vec2u(3u, 3u)) == vec2u(6u, 5u));

        // left shift
        VX_STATIC_CHECK((vec2u(1u, 2u) << 1u) == vec2u(2u, 4u));
        VX_STATIC_CHECK((1u << vec2u(1u, 2u)) == vec2u(2u, 4u));
        VX_STATIC_CHECK((vec2u(1u, 2u) << vec2u(1u, 2u)) == vec2u(2u, 8u));

        // right shift
        VX_STATIC_CHECK((vec2u(4u, 8u) >> 1u) == vec2u(2u, 4u));
        VX_STATIC_CHECK((8u >> vec2u(2u, 3u)) == vec2u(2u, 1u));
        VX_STATIC_CHECK((vec2u(8u, 16u) >> vec2u(2u, 3u)) == vec2u(2u, 2u));

        // not
        VX_STATIC_CHECK((~vec2u(0u, 0xFFFFFFFFu)) == vec2u(0xFFFFFFFFu, 0u));
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec2u v;
            v += 5u;
            VX_CHECK(v == vec2u(5u));
            v += vec2u(1u, 2u);
            VX_CHECK(v == vec2u(6u, 7u));
        }

        // subtraction
        {
            vec2u v(10u, 20u);
            v -= 5u;
            VX_CHECK(v == vec2u(5u, 15u));
            v -= vec2u(1u, 2u);
            VX_CHECK(v == vec2u(4u, 13u));
        }

        // multiplication
        {
            vec2u v(1u, 2u);
            v *= 5u;
            VX_CHECK(v == vec2u(5u, 10u));
            v *= vec2u(2u, 3u);
            VX_CHECK(v == vec2u(10u, 30u));
        }

        // division
        {
            vec2u v(100u, 50u);
            v /= 5u;
            VX_CHECK(v == vec2u(20u, 10u));
            v /= vec2u(2u, 5u);
            VX_CHECK(v == vec2u(10u, 2u));
        }

        // modulo
        {
            vec2u v(10u, 20u);
            v %= 4u;
            VX_CHECK(v == vec2u(2u, 0u));
            v %= vec2u(2u, 3u);
            VX_CHECK(v == vec2u(0u, 0u));
        }
    }

    VX_SECTION("unary bit operators")
    {
        // and
        {
            vec2u v(7u, 14u);
            v &= 3u;
            VX_CHECK(v == vec2u(3u, 2u));
            v &= vec2u(1u, 2u);
            VX_CHECK(v == vec2u(1u, 2u));
        }

        // or
        {
            vec2u v(1u, 2u);
            v |= 4u;
            VX_CHECK(v == vec2u(5u, 6u));
            v |= vec2u(2u, 1u);
            VX_CHECK(v == vec2u(7u, 7u));
        }

        // xor
        {
            vec2u v(5u, 6u);
            v ^= 3u;
            VX_CHECK(v == vec2u(6u, 5u));
            v ^= vec2u(1u, 1u);
            VX_CHECK(v == vec2u(7u, 4u));
        }

        // left shift
        {
            vec2u v(1u, 2u);
            v <<= 1u;
            VX_CHECK(v == vec2u(2u, 4u));
            v <<= vec2u(1u, 2u);
            VX_CHECK(v == vec2u(4u, 16u));
        }

        // right shift
        {
            vec2u v(8u, 16u);
            v >>= 1u;
            VX_CHECK(v == vec2u(4u, 8u));
            v >>= vec2u(2u, 3u);
            VX_CHECK(v == vec2u(1u, 1u));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec2u(1u, 0u) && vec2u(1u, 2u)) == vec2b(true, false));
        VX_STATIC_CHECK((vec2u(0u, 0u) && vec2u(1u, 1u)) == vec2b(false, false));

        // or
        VX_STATIC_CHECK((vec2u(1u, 0u) || vec2u(0u, 1u)) == vec2b(true, true));
        VX_STATIC_CHECK((vec2u(0u, 0u) || vec2u(0u, 1u)) == vec2b(false, true));

        // not
        VX_STATIC_CHECK((!vec2u(1u, 0u)) == vec2b(false, true));
        VX_STATIC_CHECK((!vec2u(0u, 0u)) == vec2b(true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec2u::zero() == vec2u(0u, 0u));
        VX_STATIC_CHECK(vec2u::one() == vec2u(1u, 1u));

        VX_STATIC_CHECK(vec2u::right() == vec2u(1u, 0u));
        VX_STATIC_CHECK(vec2u::up() == vec2u(0u, 1u));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec2f)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec2f::scalar_type, f32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec2f) == 8);
        VX_STATIC_CHECK(vec2f::size == 2);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec2f(), 0.0f, 0.0f);
        check_each(vec2f(vec2f(1.0f, 2.0f)), 1.0f, 2.0f);

        // explicit constructors
        check_each(vec2f(1.0f), 1.0f, 1.0f);
        check_each(vec2f(1.0f, 2.0f), 1.0f, 2.0f);

        // conversion constructors
        check_each(vec2f(true), 1.0f, 1.0f);
        check_each(vec2f(5), 5.0f, 5.0f);
        check_each(vec2f(5u), 5.0f, 5.0f);
        check_each(vec2f(5.0), 5.0f, 5.0f);

        // mixed
        check_each(vec2f(false, -1), 0.0f, -1.0f);
        check_each(vec2f(7.5f, 1u), 7.5f, 1.0f);
        check_each(vec2f(100u, true), 100.0f, 1.0f);
        check_each(vec2f(-75.5, 7), -75.5f, 7.0f);

        // vec2
        check_each(static_cast<vec2f>(vec2b(false, true)), 0.0f, 1.0f);
        check_each(static_cast<vec2f>(vec2i(1, -2)), 1.0f, -2.0f);
        check_each(static_cast<vec2f>(vec2u(1u, 2u)), 1.0f, 2.0f);
        check_each(static_cast<vec2f>(vec2d(0.5, 1.5)), 0.5f, 1.5f);

        // vec3
        check_each(static_cast<vec2f>(vec3b(true, false, true)), 1.0f, 0.0f);
        check_each(static_cast<vec2f>(vec3i(0, 1, 2)), 0.0f, 1.0f);
        check_each(static_cast<vec2f>(vec3u(0u, 1u, 2u)), 0.0f, 1.0f);
        check_each(static_cast<vec2f>(vec3f(0.0f, 1.0f, 2.0f)), 0.0f, 1.0f);
        check_each(static_cast<vec2f>(vec3d(0.0, 1.0, 2.0)), 0.0f, 1.0f);

        // vec4
        check_each(static_cast<vec2f>(vec4b(true, false, true, false)), 1.0f, 0.0f);
        check_each(static_cast<vec2f>(vec4i(0, 1, 2, 3)), 0.0f, 1.0f);
        check_each(static_cast<vec2f>(vec4u(0u, 1u, 2u, 3u)), 0.0f, 1.0f);
        check_each(static_cast<vec2f>(vec4f(0.0f, 1.0f, 2.0f, 3.0f)), 0.0f, 1.0f);
        check_each(static_cast<vec2f>(vec4d(0.0, 1.0, 2.0, 3.0)), 0.0f, 1.0f);
    }

    VX_SECTION("assignment")
    {
        {
            vec2f v1(0.0f, 1.0f);
            vec2b v2(true, false);

            v1 = v2;
            VX_CHECK(v1.x == 1.0f);
            VX_CHECK(v1.y == 0.0f);
        }

        {
            vec2f v1(1.0f, 2.0f);
            vec2i v2(2, 1);

            v1 = v2;
            VX_CHECK(v1.x == 2.0f);
            VX_CHECK(v1.y == 1.0f);
        }

        {
            vec2f v1(1.0f, 2.0f);
            vec2u v2(2u, 1u);

            v1 = v2;
            VX_CHECK(v1.x == 2.0f);
            VX_CHECK(v1.y == 1.0f);
        }

        {
            vec2f v1(1.0f, 2.0f);
            vec2f v2(2.0f, 1.0f);

            v1 = v2;
            VX_CHECK(v1.x == 2.0f);
            VX_CHECK(v1.y == 1.0f);
        }

        {
            vec2f v1(1.0f, 2.0f);
            vec2d v2(2.0, 1.0);

            v1 = v2;
            VX_CHECK(v1.x == 2.0f);
            VX_CHECK(v1.y == 1.0f);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec2f(1.5f, 2.75f)[0] == 1.5f);
        VX_STATIC_CHECK(vec2f(1.5f, 2.75f)[1] == 2.75f);

        vec2f v;

        v[0] = 1.5f;
        VX_CHECK(v[0] == 1.5f);
        v[1] = 2.75f;
        VX_CHECK(v[1] == 2.75f);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec2f(1.5f, -2.75f) == vec2f(1.5f, -2.75f));
        VX_STATIC_CHECK(vec2f(1.5f, 2.75f) != vec2f(-1.5f, -2.75f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec2f(1.0f, -1.5f) == vec2f(1.0f, -1.5f));
        VX_STATIC_CHECK(+vec2f(-1.5f, 2.5f) == vec2f(-1.5f, 2.5f));

        VX_STATIC_CHECK(-vec2f(1.5f, -2.5f) == vec2f(-1.5f, 2.5f));
        VX_STATIC_CHECK(-vec2f(-1.5f, 2.5f) == vec2f(1.5f, -2.5f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec2f(0.5f, -1.0f) == vec2f(1.5f, 0.0f));
        VX_STATIC_CHECK(--vec2f(0.5f, -1.0f) == vec2f(-0.5f, -2.0f));

        vec2f v(0.5f, -1.0f);

        VX_CHECK(v++ == vec2f(0.5f, -1.0f));
        VX_CHECK(v == vec2f(1.5f, 0.0f));
        VX_CHECK(v-- == vec2f(1.5f, 0.0f));
        VX_CHECK(v == vec2f(0.5f, -1.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec2f(1.5f, -2.75f) + 5.0f == vec2f(6.5f, 2.25f));
        VX_STATIC_CHECK(5.0f + vec2f(1.5f, -2.75f) == vec2f(6.5f, 2.25f));
        VX_STATIC_CHECK(vec2f(5.5f) + vec2f(1.5f, -2.75f) == vec2f(7.0f, 2.75f));
        VX_STATIC_CHECK(vec2f(1.5f, -2.75f) + vec2f(5.0f) == vec2f(6.5f, 2.25f));

        // subtraction
        VX_STATIC_CHECK(vec2f(1.5f, -2.75f) - 5.0f == vec2f(-3.5f, -7.75f));
        VX_STATIC_CHECK(5.0f - vec2f(1.5f, -2.75f) == vec2f(3.5f, 7.75f));
        VX_STATIC_CHECK(vec2f(5.5f) - vec2f(1.5, -2.75) == vec2f(4.0f, 8.25f));
        VX_STATIC_CHECK(vec2f(1.5f, -2.75f) - vec2f(5.0f) == vec2f(-3.5f, -7.75f));

        // multiplication
        VX_STATIC_CHECK(vec2f(1.5f, -2.75f) * 5.0f == vec2f(7.5f, -13.75f));
        VX_STATIC_CHECK(5.0f * vec2f(1.5f, -2.75f) == vec2f(7.5f, -13.75f));
        VX_STATIC_CHECK(vec2f(5.5f) * vec2f(1.5f, -2.75f) == vec2f(8.25f, -15.125f));
        VX_STATIC_CHECK(vec2f(1.5f, -2.75f) * vec2f(5.0f) == vec2f(7.5f, -13.75f));

        // division
        VX_STATIC_CHECK(vec2f(5.5f, -13.75) / 5.0f == vec2f(1.1f, -2.75f));
        VX_STATIC_CHECK(10.0f / vec2f(10.0f, 5.0f) == vec2f(1.0f, 2.0f));
        VX_STATIC_CHECK(vec2f(10.5f) / vec2f(10.0f, 5.0f) == vec2f(1.05f, 2.1f));
        VX_STATIC_CHECK(vec2f(10.0f, 5.0f) / vec2f(5.0f, 10.0f) == vec2f(2.0f, 0.5f));
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec2f v;
            v += 5.5f;
            VX_CHECK(v == vec2f(5.5f, 5.5f));
            v += vec2f(1.5f, -2.75f);
            VX_CHECK(v == vec2f(7.0f, 2.75f));
        }

        // subtraction
        {
            vec2f v(10.5f, 20.75f);
            v -= 5.0f;
            VX_CHECK(v == vec2f(5.5f, 15.75f));
            v -= vec2f(1.5, -2.75);
            VX_CHECK(v == vec2f(4.0f, 18.5f));
        }

        // multiplication
        {
            vec2f v(1.5f, -2.75f);
            v *= 5.0f;
            VX_CHECK(v == vec2f(7.5f, -13.75f));
            v *= vec2f(2.0f, -3.0f);
            VX_CHECK(v == vec2f(15.0f, 41.25f));
        }

        // division
        {
            vec2f v(100.5f, 50.0f);
            v /= 5.0f;
            VX_CHECK(v == vec2f(20.1f, 10.0f));
            v /= vec2f(2.0f, 5.0f);
            VX_CHECK(v == vec2f(10.05f, 2.0f));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec2f(1.5f, 0.0f) && vec2f(1.5f, 2.75f)) == vec2b(true, false));
        VX_STATIC_CHECK((vec2f(0.0f, 0.0f) && vec2f(1.5f, 1.5f)) == vec2b(false, false));

        // or
        VX_STATIC_CHECK((vec2f(1.5f, 0.0f) || vec2f(0.0f, 1.5f)) == vec2b(true, true));
        VX_STATIC_CHECK((vec2f(0.0f, 0.0f) || vec2f(0.0f, 1.5f)) == vec2b(false, true));

        // not
        VX_STATIC_CHECK((!vec2f(1.5f, 0.0f)) == vec2b(false, true));
        VX_STATIC_CHECK((!vec2f(0.0f, 0.0f)) == vec2b(true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec2f::zero() == vec2f(0.0f, 0.0f));
        VX_STATIC_CHECK(vec2f::one() == vec2f(1.0f, 1.0f));

        VX_STATIC_CHECK(vec2f::right() == vec2f(1.0f, 0.0f));
        VX_STATIC_CHECK(vec2f::left() == vec2f(-1.0f, 0.0f));

        VX_STATIC_CHECK(vec2f::up() == vec2f(0.0f, 1.0f));
        VX_STATIC_CHECK(vec2f::down() == vec2f(0.0f, -1.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec2d)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec2d::scalar_type, f64>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec2d) == 16);
        VX_STATIC_CHECK(vec2d::size == 2);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec2d(), 0.0, 0.0);
        check_each(vec2d(vec2d(1.0, 2.0)), 1.0, 2.0);

        // explicit constructors
        check_each(vec2d(1.0), 1.0, 1.0);
        check_each(vec2d(1.0, 2.0), 1.0, 2.0);

        // conversion constructors
        check_each(vec2d(true), 1.0, 1.0);
        check_each(vec2d(5), 5.0, 5.0);
        check_each(vec2d(5u), 5.0, 5.0);
        check_each(vec2d(5.0f), 5.0, 5.0);

        // mixed
        check_each(vec2d(false, -1), 0.0, -1.0);
        check_each(vec2d(7.5f, 1u), 7.5, 1.0);
        check_each(vec2d(100u, true), 100.0, 1.0);
        check_each(vec2d(-75.5, 7), -75.5, 7.0);

        // vec2
        check_each(static_cast<vec2d>(vec2b(false, true)), 0.0, 1.0);
        check_each(static_cast<vec2d>(vec2i(1, -2)), 1.0, -2.0);
        check_each(static_cast<vec2d>(vec2u(1u, 2u)), 1.0, 2.0);
        check_each(static_cast<vec2d>(vec2f(1.5f, 2.5f)), 1.5, 2.5);

        // vec3
        check_each(static_cast<vec2d>(vec3b(true, false, true)), 1.0, 0.0);
        check_each(static_cast<vec2d>(vec3i(0, 1, 2)), 0.0, 1.0);
        check_each(static_cast<vec2d>(vec3u(0u, 1u, 2u)), 0.0, 1.0);
        check_each(static_cast<vec2d>(vec3f(0.0f, 1.0f, 2.0f)), 0.0, 1.0);
        check_each(static_cast<vec2d>(vec3d(0.0, 1.0, 2.0)), 0.0, 1.0);

        // vec4
        check_each(static_cast<vec2d>(vec4b(true, false, true, false)), 1.0, 0.0);
        check_each(static_cast<vec2d>(vec4i(0, 1, 2, 3)), 0.0, 1.0);
        check_each(static_cast<vec2d>(vec4u(0u, 1u, 2u, 3u)), 0.0, 1.0);
        check_each(static_cast<vec2d>(vec4f(0.0f, 1.0f, 2.0f, 3.0f)), 0.0, 1.0);
        check_each(static_cast<vec2d>(vec4d(0.0, 1.0, 2.0, 3.0)), 0.0, 1.0);
    }

    VX_SECTION("assignment")
    {
        {
            vec2d v1(0.0, 1.0);
            vec2b v2(true, false);

            v1 = v2;
            VX_CHECK(v1.x == 1.0);
            VX_CHECK(v1.y == 0.0);
        }

        {
            vec2d v1(1.0, 2.0);
            vec2i v2(2, 1);

            v1 = v2;
            VX_CHECK(v1.x == 2.0);
            VX_CHECK(v1.y == 1.0);
        }

        {
            vec2d v1(1.0, 2.0);
            vec2u v2(2u, 1u);

            v1 = v2;
            VX_CHECK(v1.x == 2.0);
            VX_CHECK(v1.y == 1.0);
        }

        {
            vec2d v1(1.0, 2.0);
            vec2f v2(2.0f, 1.0f);

            v1 = v2;
            VX_CHECK(v1.x == 2.0);
            VX_CHECK(v1.y == 1.0);
        }

        {
            vec2d v1(1.0, 2.0);
            vec2d v2(2.0, 1.0);

            v1 = v2;
            VX_CHECK(v1.x == 2.0);
            VX_CHECK(v1.y == 1.0);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec2d(1.5, 2.75)[0] == 1.5);
        VX_STATIC_CHECK(vec2d(1.5, 2.75)[1] == 2.75);

        vec2d v;

        v[0] = 1.5;
        VX_CHECK(v[0] == 1.5);
        v[1] = 2.75;
        VX_CHECK(v[1] == 2.75);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec2d(1.5, -2.75) == vec2d(1.5, -2.75));
        VX_STATIC_CHECK(vec2d(1.5, 2.75) != vec2d(-1.5, -2.75));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec2d(1.0, -1.5) == vec2d(1.0, -1.5));
        VX_STATIC_CHECK(+vec2d(-1.5, 2.5) == vec2d(-1.5, 2.5));

        VX_STATIC_CHECK(-vec2d(1.5, -2.5) == vec2d(-1.5, 2.5));
        VX_STATIC_CHECK(-vec2d(-1.5, 2.5) == vec2d(1.5, -2.5));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec2d(0.5, -1.0) == vec2d(1.5, 0.0));
        VX_STATIC_CHECK(--vec2d(0.5, -1.0) == vec2d(-0.5, -2.0));

        vec2d v(0.5, -1.0);

        VX_CHECK(v++ == vec2d(0.5, -1.0));
        VX_CHECK(v == vec2d(1.5, 0.0));
        VX_CHECK(v-- == vec2d(1.5, 0.0));
        VX_CHECK(v == vec2d(0.5, -1.0));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec2d(1.5, -2.75) + 5.0 == vec2d(6.5, 2.25));
        VX_STATIC_CHECK(5.0 + vec2d(1.5, -2.75) == vec2d(6.5, 2.25));
        VX_STATIC_CHECK(vec2d(5.5) + vec2d(1.5, -2.75) == vec2d(7.0, 2.75));
        VX_STATIC_CHECK(vec2d(1.5, -2.75) + vec2d(5.0) == vec2d(6.5, 2.25));

        // subtraction
        VX_STATIC_CHECK(vec2d(1.5, -2.75) - 5.0 == vec2d(-3.5, -7.75));
        VX_STATIC_CHECK(5.0 - vec2d(1.5, -2.75) == vec2d(3.5, 7.75));
        VX_STATIC_CHECK(vec2d(5.5) - vec2d(1.5, -2.75) == vec2d(4.0, 8.25));
        VX_STATIC_CHECK(vec2d(1.5, -2.75) - vec2d(5.0) == vec2d(-3.5, -7.75));

        // multiplication
        VX_STATIC_CHECK(vec2d(1.5, -2.75) * 5.0 == vec2d(7.5, -13.75));
        VX_STATIC_CHECK(5.0 * vec2d(1.5, -2.75) == vec2d(7.5, -13.75));
        VX_STATIC_CHECK(vec2d(5.5) * vec2d(1.5, -2.75) == vec2d(8.25, -15.125));
        VX_STATIC_CHECK(vec2d(1.5, -2.75) * vec2d(5.0) == vec2d(7.5, -13.75));

        // division
        VX_STATIC_CHECK(vec2d(5.5, -13.75) / 5.0 == vec2d(1.1, -2.75));
        VX_STATIC_CHECK(10.0 / vec2d(10.0, 5.0) == vec2d(1.0, 2.0));
        VX_STATIC_CHECK(vec2d(10.5) / vec2d(10.0, 5.0) == vec2d(1.05, 2.1));
        VX_STATIC_CHECK(vec2d(10.0, 5.0) / vec2d(5.0, 10.0) == vec2d(2.0, 0.5));
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec2d v;
            v += 5.5;
            VX_CHECK(v == vec2d(5.5, 5.5));
            v += vec2d(1.5, -2.75);
            VX_CHECK(v == vec2d(7.0, 2.75));
        }

        // subtraction
        {
            vec2d v(10.5, 20.75);
            v -= 5.0;
            VX_CHECK(v == vec2d(5.5, 15.75));
            v -= vec2d(1.5, -2.75);
            VX_CHECK(v == vec2d(4.0, 18.5));
        }

        // multiplication
        {
            vec2d v(1.5, -2.75);
            v *= 5.0;
            VX_CHECK(v == vec2d(7.5, -13.75));
            v *= vec2d(2.0, -3.0);
            VX_CHECK(v == vec2d(15.0, 41.25));
        }

        // division
        {
            vec2d v(100.5, 50.0);
            v /= 5.0;
            VX_CHECK(v == vec2d(20.1, 10.0));
            v /= vec2d(2.0, 5.0);
            VX_CHECK(v == vec2d(10.05, 2.0));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec2d(1.5, 0.0) && vec2d(1.5, 2.75)) == vec2b(true, false));
        VX_STATIC_CHECK((vec2d(0.0, 0.0) && vec2d(1.5, 1.5)) == vec2b(false, false));

        // or
        VX_STATIC_CHECK((vec2d(1.5, 0.0) || vec2d(0.0, 1.5)) == vec2b(true, true));
        VX_STATIC_CHECK((vec2d(0.0, 0.0) || vec2d(0.0, 1.5)) == vec2b(false, true));

        // not
        VX_STATIC_CHECK((!vec2d(1.5, 0.0)) == vec2b(false, true));
        VX_STATIC_CHECK((!vec2d(0.0, 0.0)) == vec2b(true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec2d::zero() == vec2d(0.0, 0.0));
        VX_STATIC_CHECK(vec2d::one() == vec2d(1.0, 1.0));

        VX_STATIC_CHECK(vec2d::right() == vec2d(1.0, 0.0));
        VX_STATIC_CHECK(vec2d::left() == vec2d(-1.0, 0.0));

        VX_STATIC_CHECK(vec2d::up() == vec2d(0.0, 1.0));
        VX_STATIC_CHECK(vec2d::down() == vec2d(0.0, -1.0));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}