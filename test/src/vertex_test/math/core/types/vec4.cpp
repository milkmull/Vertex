#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

#define check_each(vec, vx, vy, vz, vw) VX_STATIC_CHECK(vec.x == vx && vec.y == vy && vec.z == vz && vec.w == vw)

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec4b)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec4b::scalar_type, bool>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec4b) == 4);
        VX_STATIC_CHECK(vec4b::size == 4);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec4b(), false, false, false, false);
        check_each(vec4b(vec4b(true, false, true, false)), true, false, true, false);

        // explicit constructors
        check_each(vec4b(true), true, true, true, true);
        check_each(vec4b(true, false, true, false), true, false, true, false);

        // conversion constructors
        check_each(vec4b(1), true, true, true, true);
        check_each(vec4b(1u), true, true, true, true);
        check_each(vec4b(1.0f), true, true, true, true);
        check_each(vec4b(1.0), true, true, true, true);

        // mixed
        check_each(vec4b(true, 1, 5u, 0.0), true, true, true, false);
        check_each(vec4b(vec2f(0.0f, 123.4f), 5u, 0.0), false, true, true, false);
        check_each(vec4b(0.0f, vec2u(1u, 123u), 0.0), false, true, true, false);
        check_each(vec4b(0.0f, 0.5, vec2i(1, 0)), false, true, true, false);
        check_each(vec4b(vec2i(0, 1), vec2u(0u, 1u)), false, true, false, true);
        check_each(vec4b(1.0, vec2f(1, 0), true), true, true, false, true);
        check_each(vec4b(vec3b(true, true, false), 1), true, true, false, true);
        check_each(vec4b(5u, vec3d(1e5, 0.0, -5e-10)), true, true, false, true);
        check_each(vec4b(vec2b(true, true), vec2b(false, true)), true, true, false, true);

        // vec4
        check_each(static_cast<vec4b>(vec4i(0, 1, 0, 2)), false, true, false, true);
        check_each(static_cast<vec4b>(vec4u(0u, 1u, 0u, 2u)), false, true, false, true);
        check_each(static_cast<vec4b>(vec4f(0.0f, 1.0f, 0.0f, 0.5f)), false, true, false, true);
        check_each(static_cast<vec4b>(vec4d(0.0, 1.0, 0.0, 2.0)), false, true, false, true);
    }

    VX_SECTION("assignment")
    {
        {
            vec4b v1(false, true, false, true);
            vec4b v2(true, false, true, false);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
            VX_CHECK(v1.z == true);
            VX_CHECK(v1.w == false);
        }

        {
            vec4b v1(false, true, false, true);
            vec4i v2(1, 0, 1, 0);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
            VX_CHECK(v1.z == true);
            VX_CHECK(v1.w == false);
        }

        {
            vec4b v1(false, true, false, true);
            vec4u v2(1u, 0u, 1u, 0u);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
            VX_CHECK(v1.z == true);
            VX_CHECK(v1.w == false);
        }

        {
            vec4b v1(false, true, false, true);
            vec4f v2(1.0f, 0.0f, 1.0f, 0.0f);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
            VX_CHECK(v1.z == true);
            VX_CHECK(v1.w == false);
        }

        {
            vec4b v1(false, true, false, true);
            vec4d v2(1.0, 0.0, 1.0, 0.0);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
            VX_CHECK(v1.z == true);
            VX_CHECK(v1.w == false);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec4b(false, true, false, true)[0] == false);
        VX_STATIC_CHECK(vec4b(false, true, false, true)[1] == true);
        VX_STATIC_CHECK(vec4b(false, true, false, true)[2] == false);
        VX_STATIC_CHECK(vec4b(false, true, false, true)[3] == true);

        vec4b v;

        v[0] = true;
        VX_CHECK(v[0] == true);
        v[1] = true;
        VX_CHECK(v[1] == true);
        v[2] = true;
        VX_CHECK(v[2] == true);
        v[3] = true;
        VX_CHECK(v[3] == true);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec4b() == vec4b());
        VX_STATIC_CHECK(vec4b() != vec4b(true));
    }

    VX_SECTION("boolean operators")
    {
        constexpr vec4b v_true(true, true, true, true);
        constexpr vec4b v_false(false, false, false, false);
        constexpr vec4b v_mixed(true, false, true, false);

        // && with scalar
        VX_STATIC_CHECK((v_true && true) == vec4b(true, true, true, true));
        VX_STATIC_CHECK((v_true && false) == vec4b(false, false, false, false));
        VX_STATIC_CHECK((v_false && true) == vec4b(false, false, false, false));
        VX_STATIC_CHECK((v_mixed && true) == vec4b(true, false, true, false));
        VX_STATIC_CHECK((true && v_mixed) == vec4b(true, false, true, false));
        VX_STATIC_CHECK((false && v_mixed) == vec4b(false, false, false, false));

        // && with vector
        VX_STATIC_CHECK((v_mixed && v_true) == vec4b(true, false, true, false));
        VX_STATIC_CHECK((v_mixed && v_false) == vec4b(false, false, false, false));
        VX_STATIC_CHECK((v_true && v_true) == vec4b(true, true, true, true));

        // || with scalar
        VX_STATIC_CHECK((v_false || false) == vec4b(false, false, false, false));
        VX_STATIC_CHECK((v_false || true) == vec4b(true, true, true, true));
        VX_STATIC_CHECK((v_mixed || false) == vec4b(true, false, true, false));
        VX_STATIC_CHECK((v_mixed || true) == vec4b(true, true, true, true));
        VX_STATIC_CHECK((true || v_false) == vec4b(true, true, true, true));
        VX_STATIC_CHECK((false || v_mixed) == vec4b(true, false, true, false));

        // || with vector
        VX_STATIC_CHECK((v_mixed || v_false) == vec4b(true, false, true, false));
        VX_STATIC_CHECK((v_mixed || v_true) == vec4b(true, true, true, true));
        VX_STATIC_CHECK((v_false || v_false) == vec4b(false, false, false, false));

        // ! operator
        VX_STATIC_CHECK((!v_true) == vec4b(false, false, false, false));
        VX_STATIC_CHECK((!v_false) == vec4b(true, true, true, true));
        VX_STATIC_CHECK((!v_mixed) == vec4b(false, true, false, true));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec4i)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec4i::scalar_type, i32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec4i) == 16);
        VX_STATIC_CHECK(vec4i::size == 4);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec4i(), 0, 0, 0, 0);
        check_each(vec4i(vec4i(1, 2, 3, 4)), 1, 2, 3, 4);

        // explicit constructors
        check_each(vec4i(7), 7, 7, 7, 7);
        check_each(vec4i(1, 2, 3, 4), 1, 2, 3, 4);

        // conversion constructors
        check_each(vec4i(true), 1, 1, 1, 1);
        check_each(vec4i(false), 0, 0, 0, 0);
        check_each(vec4i(1u), 1, 1, 1, 1);
        check_each(vec4i(1.0f), 1, 1, 1, 1);
        check_each(vec4i(1.0), 1, 1, 1, 1);

        // mixed
        check_each(vec4i(true, 5u, 1.2, -3), 1, 5, 1, -3);
        check_each(vec4i(vec2f(1.5f, 2.5f), 3u, -4.7), 1, 2, 3, -4);
        check_each(vec4i(1.1f, vec2u(2u, 3u), 4), 1, 2, 3, 4);
        check_each(vec4i(1.1f, 2.9, vec2i(3, 4)), 1, 2, 3, 4);
        check_each(vec4i(vec2i(5, 6), vec2u(7u, 8u)), 5, 6, 7, 8);
        check_each(vec4i(1.0, vec2f(2.0, 3.0), 4.0), 1, 2, 3, 4);
        check_each(vec4i(vec3b(true, false, true), 0), 1, 0, 1, 0);
        check_each(vec4i(5u, vec3d(1e5, 0.0, -5e-10)), 5, 100000, 0, 0);
        check_each(vec4i(vec2b(true, true), vec2b(false, true)), 1, 1, 0, 1);

        // vec4
        check_each(static_cast<vec4i>(vec4b(true, false, true, true)), 1, 0, 1, 1);
        check_each(static_cast<vec4i>(vec4u(0u, 1u, 2u, 3u)), 0, 1, 2, 3);
        check_each(static_cast<vec4i>(vec4f(0.0f, 1.0f, 2.0f, 3.5f)), 0, 1, 2, 3);
        check_each(static_cast<vec4i>(vec4d(0.0, 1.0, 2.0, 3.9)), 0, 1, 2, 3);
    }

    VX_SECTION("assignment")
    {
        {
            vec4i v1(0, 1, 2, 3);
            vec4b v2(true, false, true, false);

            v1 = v2;
            VX_CHECK(v1.x == 1);
            VX_CHECK(v1.y == 0);
            VX_CHECK(v1.z == 1);
            VX_CHECK(v1.w == 0);
        }

        {
            vec4i v1(1, 2, 3, 4);
            vec4i v2(4, 3, 2, 1);

            v1 = v2;
            VX_CHECK(v1.x == 4);
            VX_CHECK(v1.y == 3);
            VX_CHECK(v1.z == 2);
            VX_CHECK(v1.w == 1);
        }

        {
            vec4i v1(1, 2, 3, 4);
            vec4u v2(4u, 3u, 2u, 1u);

            v1 = v2;
            VX_CHECK(v1.x == 4);
            VX_CHECK(v1.y == 3);
            VX_CHECK(v1.z == 2);
            VX_CHECK(v1.w == 1);
        }

        {
            vec4i v1(1, 2, 3, 4);
            vec4f v2(4.0f, 3.0f, 2.0f, 1.0f);

            v1 = v2;
            VX_CHECK(v1.x == 4);
            VX_CHECK(v1.y == 3);
            VX_CHECK(v1.z == 2);
            VX_CHECK(v1.w == 1);
        }

        {
            vec4i v1(1, 2, 3, 4);
            vec4d v2(4.0, 3.0, 2.0, 1.0);

            v1 = v2;
            VX_CHECK(v1.x == 4);
            VX_CHECK(v1.y == 3);
            VX_CHECK(v1.z == 2);
            VX_CHECK(v1.w == 1);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec4i(1, 2, 3, 4)[0] == 1);
        VX_STATIC_CHECK(vec4i(1, 2, 3, 4)[1] == 2);
        VX_STATIC_CHECK(vec4i(1, 2, 3, 4)[2] == 3);
        VX_STATIC_CHECK(vec4i(1, 2, 3, 4)[3] == 4);

        vec4i v;

        v[0] = 1;
        VX_CHECK(v[0] == 1);
        v[1] = 2;
        VX_CHECK(v[1] == 2);
        v[2] = 3;
        VX_CHECK(v[2] == 3);
        v[3] = 4;
        VX_CHECK(v[3] == 4);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec4i(1) == vec4i(1));
        VX_STATIC_CHECK(vec4i(1) != vec4i(2));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec3i(1) == vec3i(1));
        VX_STATIC_CHECK(+vec3i(-1) == vec3i(-1));

        VX_STATIC_CHECK(-vec3i(1) == vec3i(-1));
        VX_STATIC_CHECK(-vec3i(-1) == vec3i(1));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec4i(0) == vec4i(1));
        VX_STATIC_CHECK(--vec4i(0) == vec4i(-1));

        vec4i v(0);

        VX_CHECK(v++ == vec4i(0));
        VX_CHECK(v == vec4i(1));
        VX_CHECK(v-- == vec4i(1));
        VX_CHECK(v == vec4i(0));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec4i(1, 2, 3, 4) + 5 == vec4i(6, 7, 8, 9));
        VX_STATIC_CHECK(5 + vec4i(1, 2, 3, 4) == vec4i(6, 7, 8, 9));
        VX_STATIC_CHECK(vec4i(5) + vec4i(1, 2, 3, 4) == vec4i(6, 7, 8, 9));
        VX_STATIC_CHECK(vec4i(1, 2, 3, 4) + vec4i(5) == vec4i(6, 7, 8, 9));

        // subtraction
        VX_STATIC_CHECK(vec4i(1, 2, 3, 4) - 5 == vec4i(-4, -3, -2, -1));
        VX_STATIC_CHECK(5 - vec4i(1, 2, 3, 4) == vec4i(4, 3, 2, 1));
        VX_STATIC_CHECK(vec4i(5) - vec4i(1, 2, 3, 4) == vec4i(4, 3, 2, 1));
        VX_STATIC_CHECK(vec4i(1, 2, 3, 4) - vec4i(5) == vec4i(-4, -3, -2, -1));

        // multiplication
        VX_STATIC_CHECK(vec4i(1, 2, 3, 4) * 5 == vec4i(5, 10, 15, 20));
        VX_STATIC_CHECK(5 * vec4i(1, 2, 3, 4) == vec4i(5, 10, 15, 20));
        VX_STATIC_CHECK(vec4i(5) * vec4i(1, 2, 3, 4) == vec4i(5, 10, 15, 20));
        VX_STATIC_CHECK(vec4i(1, 2, 3, 4) * vec4i(5) == vec4i(5, 10, 15, 20));

        // division
        VX_STATIC_CHECK(vec4i(5, 10, 15, 20) / 5 == vec4i(1, 2, 3, 4));
        VX_STATIC_CHECK(20 / vec4i(20, 10, 5, 4) == vec4i(1, 2, 4, 5));
        VX_STATIC_CHECK(vec4i(20) / vec4i(20, 10, 5, 4) == vec4i(1, 2, 4, 5));
        VX_STATIC_CHECK(vec4i(20, 10, 5, 16) / vec4i(5, 2, 5, 4) == vec4i(4, 5, 1, 4));

        // modulo
        VX_STATIC_CHECK(vec4i(5, 10, 15, 20) % 4 == vec4i(1, 2, 3, 0));
        VX_STATIC_CHECK(10 % vec4i(3, 4, 7, 6) == vec4i(1, 2, 3, 4));
        VX_STATIC_CHECK(vec4i(10) % vec4i(3, 4, 7, 6) == vec4i(1, 2, 3, 4));
        VX_STATIC_CHECK(vec4i(10, 5, 11, 13) % vec4i(4, 3, 5, 6) == vec4i(2, 2, 1, 1));
    }

    VX_SECTION("binary bit operators")
    {
        // and
        VX_STATIC_CHECK((vec4i(5, 6, 7, 8) & 3) == vec4i(1, 2, 3, 0));
        VX_STATIC_CHECK((3 & vec4i(5, 6, 7, 8)) == vec4i(1, 2, 3, 0));
        VX_STATIC_CHECK((vec4i(5, 6, 7, 8) & vec4i(3, 3, 3, 3)) == vec4i(1, 2, 3, 0));

        // or
        VX_STATIC_CHECK((vec4i(5, 6, 7, 8) | 3) == vec4i(7, 7, 7, 11));
        VX_STATIC_CHECK((3 | vec4i(5, 6, 7, 8)) == vec4i(7, 7, 7, 11));
        VX_STATIC_CHECK((vec4i(5, 6, 7, 8) | vec4i(3, 3, 3, 3)) == vec4i(7, 7, 7, 11));

        // xor
        VX_STATIC_CHECK((vec4i(5, 6, 7, 8) ^ 3) == vec4i(6, 5, 4, 11));
        VX_STATIC_CHECK((3 ^ vec4i(5, 6, 7, 8)) == vec4i(6, 5, 4, 11));
        VX_STATIC_CHECK((vec4i(5, 6, 7, 8) ^ vec4i(3, 3, 3, 3)) == vec4i(6, 5, 4, 11));

        // left shift
        VX_STATIC_CHECK((vec4i(1, 2, 3, 4) << 1) == vec4i(2, 4, 6, 8));
        VX_STATIC_CHECK((1 << vec4i(1, 2, 3, 4)) == vec4i(2, 4, 8, 16));
        VX_STATIC_CHECK((vec4i(1, 2, 3, 4) << vec4i(1, 2, 3, 4)) == vec4i(2, 8, 24, 64));

        // right shift
        VX_STATIC_CHECK((vec4i(4, 8, 16, 32) >> 1) == vec4i(2, 4, 8, 16));
        VX_STATIC_CHECK((32 >> vec4i(2, 3, 4, 5)) == vec4i(8, 4, 2, 1));
        VX_STATIC_CHECK((vec4i(8, 16, 32, 64) >> vec4i(2, 3, 4, 5)) == vec4i(2, 2, 2, 2));

        // not
        VX_STATIC_CHECK((~vec4i(0, -1, 42, 99)) == vec4i(~0, ~(-1), ~42, ~99));
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec4i v;
            v += 5;
            VX_CHECK(v == vec4i(5, 5, 5, 5));
            v += vec4i(1, 2, 3, 4);
            VX_CHECK(v == vec4i(6, 7, 8, 9));
        }

        // subtraction
        {
            vec4i v(10, 20, 30, 40);
            v -= 5;
            VX_CHECK(v == vec4i(5, 15, 25, 35));
            v -= vec4i(1, 2, 3, 4);
            VX_CHECK(v == vec4i(4, 13, 22, 31));
        }

        // multiplication
        {
            vec4i v(1, 2, 3, 4);
            v *= 5;
            VX_CHECK(v == vec4i(5, 10, 15, 20));
            v *= vec4i(2, 3, 4, 5);
            VX_CHECK(v == vec4i(10, 30, 60, 100));
        }

        // division
        {
            vec4i v(100, 50, 25, 10);
            v /= 5;
            VX_CHECK(v == vec4i(20, 10, 5, 2));
            v /= vec4i(2, 5, 5, 2);
            VX_CHECK(v == vec4i(10, 2, 1, 1));
        }

        // modulo
        {
            vec4i v(10, 20, 30, 40);
            v %= 4;
            VX_CHECK(v == vec4i(2, 0, 2, 0));
            v %= vec4i(2, 3, 4, 5);
            VX_CHECK(v == vec4i(0, 0, 2, 0));
        }
    }

    VX_SECTION("unary bit operators")
    {
        // and
        {
            vec4i v(7, 14, 15, 30);
            v &= 3;
            VX_CHECK(v == vec4i(3, 2, 3, 2));
            v &= vec4i(1, 2, 3, 0);
            VX_CHECK(v == vec4i(1, 2, 3, 0));
        }

        // or
        {
            vec4i v(1, 2, 3, 4);
            v |= 4;
            VX_CHECK(v == vec4i(5, 6, 7, 4));
            v |= vec4i(2, 1, 0, 5);
            VX_CHECK(v == vec4i(7, 7, 7, 5));
        }

        // xor
        {
            vec4i v(5, 6, 7, 8);
            v ^= 3;
            VX_CHECK(v == vec4i(6, 5, 4, 11));
            v ^= vec4i(1, 1, 1, 1);
            VX_CHECK(v == vec4i(7, 4, 5, 10));
        }

        // left shift
        {
            vec4i v(1, 2, 3, 4);
            v <<= 1;
            VX_CHECK(v == vec4i(2, 4, 6, 8));
            v <<= vec4i(1, 2, 3, 4);
            VX_CHECK(v == vec4i(4, 16, 48, 128));
        }

        // right shift
        {
            vec4i v(8, 16, 32, 64);
            v >>= 1;
            VX_CHECK(v == vec4i(4, 8, 16, 32));
            v >>= vec4i(2, 3, 4, 5);
            VX_CHECK(v == vec4i(1, 1, 1, 1));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec4i(1, 0, 3, 4) && vec4i(1, 2, 0, 1)) == vec4b(true, false, false, true));
        VX_STATIC_CHECK((vec4i(0, 0, 0, 0) && vec4i(1, 1, 1, 1)) == vec4b(false, false, false, false));

        // or
        VX_STATIC_CHECK((vec4i(1, 0, 0, 0) || vec4i(0, 1, 1, 1)) == vec4b(true, true, true, true));
        VX_STATIC_CHECK((vec4i(0, 0, 0, 0) || vec4i(0, 1, 0, 0)) == vec4b(false, true, false, false));

        // not
        VX_STATIC_CHECK((!vec4i(1, 0, 3, 4)) == vec4b(false, true, false, false));
        VX_STATIC_CHECK((!vec4i(0, 0, 0, 0)) == vec4b(true, true, true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec4i::zero() == vec4i(0, 0, 0, 0));
        VX_STATIC_CHECK(vec4i::one() == vec4i(1, 1, 1, 1));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec4u)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec4u::scalar_type, u32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec4u) == 16);
        VX_STATIC_CHECK(vec4u::size == 4);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec4u(), 0u, 0u, 0u, 0u);
        check_each(vec4u(vec4u(1u, 2u, 3u, 4u)), 1u, 2u, 3u, 4u);

        // explicit constructors
        check_each(vec4u(7u), 7u, 7u, 7u, 7u);
        check_each(vec4u(1u, 2u, 3u, 4u), 1u, 2u, 3u, 4u);

        // conversion constructors
        check_each(vec4u(true), 1u, 1u, 1u, 1u);
        check_each(vec4u(false), 0u, 0u, 0u, 0u);
        check_each(vec4u(1), 1u, 1u, 1u, 1u);
        check_each(vec4u(1.0f), 1u, 1u, 1u, 1u);
        check_each(vec4u(1.0), 1u, 1u, 1u, 1u);

        // mixed
        check_each(vec4u(true, 5, 1.2f, 3.7), 1u, 5u, 1u, 3u);
        check_each(vec4u(vec2f(1.5f, 2.5f), 3, 4.9), 1u, 2u, 3u, 4u);
        check_each(vec4u(1.1f, vec2i(2, 3), 4u), 1u, 2u, 3u, 4u);
        check_each(vec4u(1.1f, 2.9, vec2u(3u, 4u)), 1u, 2u, 3u, 4u);
        check_each(vec4u(vec2u(5u, 6u), vec2i(7, 8)), 5u, 6u, 7u, 8u);
        check_each(vec4u(1.0, vec2f(2.0f, 3.0f), 4.0), 1u, 2u, 3u, 4u);
        check_each(vec4u(vec3b(true, false, true), true), 1u, 0u, 1u, 1u);
        check_each(vec4u(5, vec3d(1e5, 0.0, 3.9)), 5u, 100000u, 0u, 3u);
        check_each(vec4u(vec2b(true, true), vec2b(false, true)), 1u, 1u, 0u, 1u);

        // vec4
        check_each(static_cast<vec4u>(vec4b(true, false, true, false)), 1u, 0u, 1u, 0u);
        check_each(static_cast<vec4u>(vec4i(0, 1, 2, 3)), 0u, 1u, 2u, 3u);
        check_each(static_cast<vec4u>(vec4f(0.0f, 1.0f, 2.0f, 3.5f)), 0u, 1u, 2u, 3u);
        check_each(static_cast<vec4u>(vec4d(0.0, 1.0, 2.0, 3.9)), 0u, 1u, 2u, 3u);
    }

    VX_SECTION("assignment")
    {
        {
            vec4u v1(0u, 1u, 2u, 3u);
            vec4b v2(true, false, true, false);

            v1 = v2;
            VX_CHECK(v1.x == 1u);
            VX_CHECK(v1.y == 0u);
            VX_CHECK(v1.z == 1u);
            VX_CHECK(v1.w == 0u);
        }

        {
            vec4u v1(1u, 2u, 3u, 4u);
            vec4i v2(3, 2, 1, 0);

            v1 = v2;
            VX_CHECK(v1.x == 3u);
            VX_CHECK(v1.y == 2u);
            VX_CHECK(v1.z == 1u);
            VX_CHECK(v1.w == 0u);
        }

        {
            vec4u v1(1u, 2u, 3u, 4u);
            vec4u v2(3u, 2u, 1u, 0u);

            v1 = v2;
            VX_CHECK(v1.x == 3u);
            VX_CHECK(v1.y == 2u);
            VX_CHECK(v1.z == 1u);
            VX_CHECK(v1.w == 0u);
        }

        {
            vec4u v1(1u, 2u, 3u, 4u);
            vec4f v2(3.0f, 2.0f, 1.0f, 0.0f);

            v1 = v2;
            VX_CHECK(v1.x == 3u);
            VX_CHECK(v1.y == 2u);
            VX_CHECK(v1.z == 1u);
            VX_CHECK(v1.w == 0u);
        }

        {
            vec4u v1(1u, 2u, 3u, 4u);
            vec4d v2(3.0, 2.0, 1.0, 0.0);

            v1 = v2;
            VX_CHECK(v1.x == 3u);
            VX_CHECK(v1.y == 2u);
            VX_CHECK(v1.z == 1u);
            VX_CHECK(v1.w == 0u);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec4u(1u, 2u, 3u, 4u)[0] == 1u);
        VX_STATIC_CHECK(vec4u(1u, 2u, 3u, 4u)[1] == 2u);
        VX_STATIC_CHECK(vec4u(1u, 2u, 3u, 4u)[2] == 3u);
        VX_STATIC_CHECK(vec4u(1u, 2u, 3u, 4u)[3] == 4u);

        vec4u v;

        v[0] = 1u;
        VX_CHECK(v[0] == 1u);
        v[1] = 2u;
        VX_CHECK(v[1] == 2u);
        v[2] = 3u;
        VX_CHECK(v[2] == 3u);
        v[3] = 4u;
        VX_CHECK(v[3] == 4u);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec4u(1u) == vec4u(1u));
        VX_STATIC_CHECK(vec4u(1u) != vec4u(2u));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec4u(1u) == vec4u(1u));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec4u(0u) == vec4u(1u));
        VX_STATIC_CHECK(--vec4u(1u) == vec4u(0u));

        vec4u v(0u);

        VX_CHECK(v++ == vec4u(0u));
        VX_CHECK(v == vec4u(1u));
        VX_CHECK(v-- == vec4u(1u));
        VX_CHECK(v == vec4u(0u));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec4u(1u, 2u, 3u, 4u) + 5u == vec4u(6u, 7u, 8u, 9u));
        VX_STATIC_CHECK(5u + vec4u(1u, 2u, 3u, 4u) == vec4u(6u, 7u, 8u, 9u));
        VX_STATIC_CHECK(vec4u(5u) + vec4u(1u, 2u, 3u, 4u) == vec4u(6u, 7u, 8u, 9u));
        VX_STATIC_CHECK(vec4u(1u, 2u, 3u, 4u) + vec4u(5u) == vec4u(6u, 7u, 8u, 9u));

        // subtraction
        VX_STATIC_CHECK(vec4u(6u, 7u, 8u, 9u) - 5u == vec4u(1u, 2u, 3u, 4u));
        VX_STATIC_CHECK(10u - vec4u(6u, 7u, 8u, 9u) == vec4u(4u, 3u, 2u, 1u));
        VX_STATIC_CHECK(vec4u(10u) - vec4u(6u, 7u, 8u, 9u) == vec4u(4u, 3u, 2u, 1u));
        VX_STATIC_CHECK(vec4u(6u, 7u, 8u, 9u) - vec4u(5u) == vec4u(1u, 2u, 3u, 4u));

        // multiplication
        VX_STATIC_CHECK(vec4u(1u, 2u, 3u, 4u) * 5u == vec4u(5u, 10u, 15u, 20u));
        VX_STATIC_CHECK(5u * vec4u(1u, 2u, 3u, 4u) == vec4u(5u, 10u, 15u, 20u));
        VX_STATIC_CHECK(vec4u(5u) * vec4u(1u, 2u, 3u, 4u) == vec4u(5u, 10u, 15u, 20u));
        VX_STATIC_CHECK(vec4u(1u, 2u, 3u, 4u) * vec4u(5u) == vec4u(5u, 10u, 15u, 20u));

        // division
        VX_STATIC_CHECK(vec4u(5u, 10u, 15u, 20u) / 5u == vec4u(1u, 2u, 3u, 4u));
        VX_STATIC_CHECK(20u / vec4u(15u, 5u, 3u, 2u) == vec4u(1u, 4u, 6u, 10u));
        VX_STATIC_CHECK(vec4u(15u) / vec4u(15u, 5u, 3u, 2u) == vec4u(1u, 3u, 5u, 7u));
        VX_STATIC_CHECK(vec4u(20u, 10u, 6u, 8u) / vec4u(5u, 10u, 3u, 4u) == vec4u(4u, 1u, 2u, 2u));

        // modulo
        VX_STATIC_CHECK(vec4u(5u, 10u, 15u, 20u) % 4u == vec4u(1u, 2u, 3u, 0u));
        VX_STATIC_CHECK(10u % vec4u(3u, 4u, 7u, 9u) == vec4u(1u, 2u, 3u, 1u));
        VX_STATIC_CHECK(vec4u(10u) % vec4u(3u, 4u, 7u, 9u) == vec4u(1u, 2u, 3u, 1u));
        VX_STATIC_CHECK(vec4u(10u, 5u, 9u, 11u) % vec4u(4u, 3u, 7u, 5u) == vec4u(2u, 2u, 2u, 1u));
    }

    VX_SECTION("binary bit operators")
    {
        // and
        VX_STATIC_CHECK((vec4u(5u, 6u, 7u, 8u) & 3u) == vec4u(1u, 2u, 3u, 0u));
        VX_STATIC_CHECK((3u & vec4u(5u, 6u, 7u, 8u)) == vec4u(1u, 2u, 3u, 0u));
        VX_STATIC_CHECK((vec4u(5u, 6u, 7u, 8u) & vec4u(3u)) == vec4u(1u, 2u, 3u, 0u));

        // or
        VX_STATIC_CHECK((vec4u(5u, 6u, 7u, 8u) | 3u) == vec4u(7u, 7u, 7u, 11u));
        VX_STATIC_CHECK((3u | vec4u(5u, 6u, 7u, 8u)) == vec4u(7u, 7u, 7u, 11u));
        VX_STATIC_CHECK((vec4u(5u, 6u, 7u, 8u) | vec4u(3u)) == vec4u(7u, 7u, 7u, 11u));

        // xor
        VX_STATIC_CHECK((vec4u(5u, 6u, 7u, 8u) ^ 3u) == vec4u(6u, 5u, 4u, 11u));
        VX_STATIC_CHECK((3u ^ vec4u(5u, 6u, 7u, 8u)) == vec4u(6u, 5u, 4u, 11u));
        VX_STATIC_CHECK((vec4u(5u, 6u, 7u, 8u) ^ vec4u(3u)) == vec4u(6u, 5u, 4u, 11u));

        // left shift
        VX_STATIC_CHECK((vec4u(1u, 2u, 3u, 4u) << 1u) == vec4u(2u, 4u, 6u, 8u));
        VX_STATIC_CHECK((1u << vec4u(1u, 2u, 3u, 4u)) == vec4u(2u, 4u, 8u, 16u));
        VX_STATIC_CHECK((vec4u(1u, 2u, 3u, 4u) << vec4u(1u, 2u, 3u, 4u)) == vec4u(2u, 8u, 24u, 64u));

        // right shift
        VX_STATIC_CHECK((vec4u(4u, 8u, 16u, 32u) >> 1u) == vec4u(2u, 4u, 8u, 16u));
        VX_STATIC_CHECK((32u >> vec4u(2u, 3u, 4u, 5u)) == vec4u(8u, 4u, 2u, 1u));
        VX_STATIC_CHECK((vec4u(16u, 32u, 64u, 128u) >> vec4u(2u, 3u, 4u, 5u)) == vec4u(4u, 4u, 4u, 4u));

        // not
        VX_STATIC_CHECK((~vec4u(0u, 0xFFFFFFFFu, 0x12345678u, 0x87654321u)) == vec4u(0xFFFFFFFFu, 0u, ~0x12345678u, ~0x87654321u));

    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec4u v;
            v += 5u;
            VX_CHECK(v == vec4u(5u, 5u, 5u, 5u));
            v += vec4u(1u, 2u, 3u, 4u);
            VX_CHECK(v == vec4u(6u, 7u, 8u, 9u));
        }

        // subtraction
        {
            vec4u v(10u, 20u, 30u, 40u);
            v -= 5u;
            VX_CHECK(v == vec4u(5u, 15u, 25u, 35u));
            v -= vec4u(1u, 2u, 3u, 4u);
            VX_CHECK(v == vec4u(4u, 13u, 22u, 31u));
        }

        // multiplication
        {
            vec4u v(1u, 2u, 3u, 4u);
            v *= 5u;
            VX_CHECK(v == vec4u(5u, 10u, 15u, 20u));
            v *= vec4u(2u, 3u, 4u, 5u);
            VX_CHECK(v == vec4u(10u, 30u, 60u, 100u));
        }

        // division
        {
            vec4u v(100u, 50u, 25u, 10u);
            v /= 5u;
            VX_CHECK(v == vec4u(20u, 10u, 5u, 2u));
            v /= vec4u(2u, 5u, 5u, 2u);
            VX_CHECK(v == vec4u(10u, 2u, 1u, 1u));
        }

        // modulo
        {
            vec4u v(10u, 20u, 30u, 40u);
            v %= 4u;
            VX_CHECK(v == vec4u(2u, 0u, 2u, 0u));
            v %= vec4u(2u, 3u, 5u, 6u);
            VX_CHECK(v == vec4u(0u, 0u, 2u, 0u));
        }
    }

    VX_SECTION("unary bit operators")
    {
        // and
        {
            vec4u v(7u, 14u, 0xFFu, 0xAAu);
            v &= 3u;
            VX_CHECK(v == vec4u(3u, 2u, 3u, 2u));
            v &= vec4u(1u, 2u, 0x0Fu, 0x0Au);
            VX_CHECK(v == vec4u(1u, 2u, 3u, 2u));
        }

        // or
        {
            vec4u v(1u, 2u, 3u, 4u);
            v |= 4u;
            VX_CHECK(v == vec4u(5u, 6u, 7u, 4u));
            v |= vec4u(2u, 1u, 8u, 4u);
            VX_CHECK(v == vec4u(7u, 7u, 15u, 4u));
        }

        // xor
        {
            vec4u v(5u, 6u, 7u, 8u);
            v ^= 3u;
            VX_CHECK(v == vec4u(6u, 5u, 4u, 11u));
            v ^= vec4u(1u, 1u, 1u, 1u);
            VX_CHECK(v == vec4u(7u, 4u, 5u, 10u));
        }

        // left shift
        {
            vec4u v(1u, 2u, 3u, 4u);
            v <<= 1u;
            VX_CHECK(v == vec4u(2u, 4u, 6u, 8u));
            v <<= vec4u(1u, 2u, 3u, 4u);
            VX_CHECK(v == vec4u(4u, 16u, 48u, 128u));
        }

        // right shift
        {
            vec4u v(8u, 16u, 64u, 128u);
            v >>= 1u;
            VX_CHECK(v == vec4u(4u, 8u, 32u, 64u));
            v >>= vec4u(2u, 3u, 4u, 5u);
            VX_CHECK(v == vec4u(1u, 1u, 2u, 2u));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec4u(1u, 0u, 3u, 5u) && vec4u(1u, 2u, 0u, 0u)) == vec4b(true, false, false, false));
        VX_STATIC_CHECK((vec4u(0u, 0u, 0u, 0u) && vec4u(1u, 1u, 1u, 1u)) == vec4b(false, false, false, false));

        // or
        VX_STATIC_CHECK((vec4u(1u, 0u, 0u, 0u) || vec4u(0u, 1u, 2u, 3u)) == vec4b(true, true, true, true));
        VX_STATIC_CHECK((vec4u(0u, 0u, 0u, 0u) || vec4u(0u, 1u, 0u, 0u)) == vec4b(false, true, false, false));

        // not
        VX_STATIC_CHECK((!vec4u(1u, 0u, 2u, 3u)) == vec4b(false, true, false, false));
        VX_STATIC_CHECK((!vec4u(0u, 0u, 0u, 0u)) == vec4b(true, true, true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec4u::zero() == vec4u(0u, 0u, 0u, 0u));
        VX_STATIC_CHECK(vec4u::one() == vec4u(1u, 1u, 1u, 1u));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec4f)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec4f::scalar_type, f32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec4f) == 16);
        VX_STATIC_CHECK(vec4f::size == 4);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec4f(), 0.0f, 0.0f, 0.0f, 0.0f);
        check_each(vec4f(vec4f(1.0f, 2.0f, 3.0f, 4.0f)), 1.0f, 2.0f, 3.0f, 4.0f);

        // explicit constructors
        check_each(vec4f(7.5f), 7.5f, 7.5f, 7.5f, 7.5f);
        check_each(vec4f(1.0f, 2.0f, 3.0f, 4.0f), 1.0f, 2.0f, 3.0f, 4.0f);

        // conversion constructors
        check_each(vec4f(true), 1.0f, 1.0f, 1.0f, 1.0f);
        check_each(vec4f(false), 0.0f, 0.0f, 0.0f, 0.0f);
        check_each(vec4f(1), 1.0f, 1.0f, 1.0f, 1.0f);
        check_each(vec4f(1u), 1.0f, 1.0f, 1.0f, 1.0f);
        check_each(vec4f(1.0), 1.0f, 1.0f, 1.0f, 1.0f);

        // mixed
        check_each(vec4f(true, 5, 1.2f, -3.75), 1.0f, 5.0f, 1.2f, -3.75f);
        check_each(vec4f(vec2f(1.5f, 2.5f), 3u, -4), 1.5f, 2.5f, 3.0f, -4.0f);
        check_each(vec4f(1.1f, vec2i(2, 3), 4u), 1.1f, 2.0f, 3.0f, 4.0f);
        check_each(vec4f(1.1f, 2.9, vec2u(3u, 4u)), 1.1f, 2.9f, 3.0f, 4.0f);
        check_each(vec4f(vec2u(5u, 6u), vec2i(7, 8)), 5.0f, 6.0f, 7.0f, 8.0f);
        check_each(vec4f(1.0, vec2f(2.0f, 3.0f), 4.0), 1.0f, 2.0f, 3.0f, 4.0f);
        check_each(vec4f(vec3b(true, false, true), true), 1.0f, 0.0f, 1.0f, 1.0f);
        check_each(vec4f(5u, vec3d(1e5, 0.0, -5e-10)), 5.0f, 100000.0f, 0.0f, -5e-10f);
        check_each(vec4f(vec2b(true, true), vec2b(false, true)), 1.0f, 1.0f, 0.0f, 1.0f);

        // vec4
        check_each(static_cast<vec4f>(vec4b(true, false, true, false)), 1.0f, 0.0f, 1.0f, 0.0f);
        check_each(static_cast<vec4f>(vec4i(0, 1, 2, 3)), 0.0f, 1.0f, 2.0f, 3.0f);
        check_each(static_cast<vec4f>(vec4u(0u, 1u, 2u, 3u)), 0.0f, 1.0f, 2.0f, 3.0f);
        check_each(static_cast<vec4f>(vec4d(0.0, 1.0, 2.0, 3.9)), 0.0f, 1.0f, 2.0f, 3.9f);
    }

    VX_SECTION("assignment")
    {
        {
            vec4f v1(0.0f, 1.0f, 2.0f, 3.0f);
            vec4b v2(true, false, true, false);

            v1 = v2;
            VX_CHECK(v1.x == 1.0f);
            VX_CHECK(v1.y == 0.0f);
            VX_CHECK(v1.z == 1.0f);
            VX_CHECK(v1.w == 0.0f);
        }

        {
            vec4f v1(1.0f, 2.0f, 3.0f, 4.0f);
            vec4i v2(3, 2, 1, 0);

            v1 = v2;
            VX_CHECK(v1.x == 3.0f);
            VX_CHECK(v1.y == 2.0f);
            VX_CHECK(v1.z == 1.0f);
            VX_CHECK(v1.w == 0.0f);
        }

        {
            vec4f v1(1.0f, 2.0f, 3.0f, 4.0f);
            vec4u v2(3u, 2u, 1u, 0u);

            v1 = v2;
            VX_CHECK(v1.x == 3.0f);
            VX_CHECK(v1.y == 2.0f);
            VX_CHECK(v1.z == 1.0f);
            VX_CHECK(v1.w == 0.0f);
        }

        {
            vec4f v1(1.0f, 2.0f, 3.0f, 4.0f);
            vec4f v2(3.0f, 2.0f, 1.0f, 0.0f);

            v1 = v2;
            VX_CHECK(v1.x == 3.0f);
            VX_CHECK(v1.y == 2.0f);
            VX_CHECK(v1.z == 1.0f);
            VX_CHECK(v1.w == 0.0f);
        }

        {
            vec4f v1(1.0f, 2.0f, 3.0f, 4.0f);
            vec4d v2(3.0, 2.0, 1.0, 0.0);

            v1 = v2;
            VX_CHECK(v1.x == 3.0f);
            VX_CHECK(v1.y == 2.0f);
            VX_CHECK(v1.z == 1.0f);
            VX_CHECK(v1.w == 0.0f);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec4f(1.5f, 2.75f, -1.23f, 4.56f)[0] == 1.5f);
        VX_STATIC_CHECK(vec4f(1.5f, 2.75f, -1.23f, 4.56f)[1] == 2.75f);
        VX_STATIC_CHECK(vec4f(1.5f, 2.75f, -1.23f, 4.56f)[2] == -1.23f);
        VX_STATIC_CHECK(vec4f(1.5f, 2.75f, -1.23f, 4.56f)[3] == 4.56f);

        vec4f v;

        v[0] = 1.5f;
        VX_CHECK(v[0] == 1.5f);
        v[1] = 2.75f;
        VX_CHECK(v[1] == 2.75f);
        v[2] = -1.23f;
        VX_CHECK(v[2] == -1.23f);
        v[3] = 4.56f;
        VX_CHECK(v[3] == 4.56f);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec4f(1.5f, -2.75f, -1.23f, 4.56f) == vec4f(1.5f, -2.75f, -1.23f, 4.56f));
        VX_STATIC_CHECK(vec4f(1.5f, 2.75f, -1.23f, 4.56f) != vec4f(-1.5f, -2.75f, -1.23f, 4.56f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec4f(1.0f, -1.5f, 2.0f, 4.0f) == vec4f(1.0f, -1.5f, 2.0f, 4.0f));
        VX_STATIC_CHECK(+vec4f(-1.5f, 2.5f, -3.0f, 5.5f) == vec4f(-1.5f, 2.5f, -3.0f, 5.5f));

        VX_STATIC_CHECK(-vec4f(1.5f, -2.5f, 3.0f, -4.0f) == vec4f(-1.5f, 2.5f, -3.0f, 4.0f));
        VX_STATIC_CHECK(-vec4f(-1.5f, 2.5f, -3.0f, 4.0f) == vec4f(1.5f, -2.5f, 3.0f, -4.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec4f(0.5f, -1.0f, 2.0f, 3.0f) == vec4f(1.5f, 0.0f, 3.0f, 4.0f));
        VX_STATIC_CHECK(--vec4f(0.5f, -1.0f, 2.0f, 3.0f) == vec4f(-0.5f, -2.0f, 1.0f, 2.0f));

        vec4f v(0.5f, -1.0f, 2.0f, 3.0f);

        VX_CHECK(v++ == vec4f(0.5f, -1.0f, 2.0f, 3.0f));
        VX_CHECK(v == vec4f(1.5f, 0.0f, 3.0f, 4.0f));
        VX_CHECK(v-- == vec4f(1.5f, 0.0f, 3.0f, 4.0f));
        VX_CHECK(v == vec4f(0.5f, -1.0f, 2.0f, 3.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec4f(1.5f, -2.75f, 3.0f, 4.0f) + 5.0f == vec4f(6.5f, 2.25f, 8.0f, 9.0f));
        VX_STATIC_CHECK(5.0f + vec4f(1.5f, -2.75f, 3.0f, 4.0f) == vec4f(6.5f, 2.25f, 8.0f, 9.0f));
        VX_STATIC_CHECK(vec4f(5.5f) + vec4f(1.5f, -2.75f, 3.0f, 4.0f) == vec4f(7.0f, 2.75f, 8.5f, 9.5f));
        VX_STATIC_CHECK(vec4f(1.5f, -2.75f, 3.0f, 4.0f) + vec4f(5.0f) == vec4f(6.5f, 2.25f, 8.0f, 9.0f));

        // subtraction
        VX_STATIC_CHECK(vec4f(1.5f, -2.75f, 3.0f, 4.0f) - 5.0f == vec4f(-3.5f, -7.75f, -2.0f, -1.0f));
        VX_STATIC_CHECK(5.0f - vec4f(1.5f, -2.75f, 3.0f, 4.0f) == vec4f(3.5f, 7.75f, 2.0f, 1.0f));
        VX_STATIC_CHECK(vec4f(5.5f) - vec4f(1.5f, -2.75f, 3.0f, 4.0f) == vec4f(4.0f, 8.25f, 2.5f, 1.5f));
        VX_STATIC_CHECK(vec4f(1.5f, -2.75f, 3.0f, 4.0f) - vec4f(5.0f) == vec4f(-3.5f, -7.75f, -2.0f, -1.0f));

        // multiplication
        VX_STATIC_CHECK(vec4f(1.5f, -2.75f, 3.0f, 4.0f) * 5.0f == vec4f(7.5f, -13.75f, 15.0f, 20.0f));
        VX_STATIC_CHECK(5.0f * vec4f(1.5f, -2.75f, 3.0f, 4.0f) == vec4f(7.5f, -13.75f, 15.0f, 20.0f));
        VX_STATIC_CHECK(vec4f(5.5f) * vec4f(1.5f, -2.75f, 3.0f, 4.0f) == vec4f(8.25f, -15.125f, 16.5f, 22.0f));
        VX_STATIC_CHECK(vec4f(1.5f, -2.75f, 3.0f, 4.0f) * vec4f(5.0f) == vec4f(7.5f, -13.75f, 15.0f, 20.0f));

        // division
        VX_STATIC_CHECK(vec4f(5.5f, -13.75f, 15.0f, 20.0f) / 5.0f == vec4f(1.1f, -2.75f, 3.0f, 4.0f));
        VX_STATIC_CHECK(10.0f / vec4f(10.0f, 5.0f, 2.0f, 1.0f) == vec4f(1.0f, 2.0f, 5.0f, 10.0f));
        VX_STATIC_CHECK(vec4f(10.5f) / vec4f(10.0f, 5.0f, 2.0f, 1.0f) == vec4f(1.05f, 2.1f, 5.25f, 10.5f));
        VX_STATIC_CHECK(vec4f(10.0f, 5.0f, 2.0f, 1.0f) / vec4f(5.0f, 10.0f, 2.0f, 1.0f) == vec4f(2.0f, 0.5f, 1.0f, 1.0f));

    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec4f v;
            v += 5.5f;
            VX_CHECK(v == vec4f(5.5f, 5.5f, 5.5f, 5.5f));
            v += vec4f(1.5f, -2.75f, 3.0f, 4.0f);
            VX_CHECK(v == vec4f(7.0f, 2.75f, 8.5f, 9.5f));
        }

        // subtraction
        {
            vec4f v(10.5f, 20.75f, 30.0f, 40.0f);
            v -= 5.0f;
            VX_CHECK(v == vec4f(5.5f, 15.75f, 25.0f, 35.0f));
            v -= vec4f(1.5f, -2.75f, 3.0f, 4.0f);
            VX_CHECK(v == vec4f(4.0f, 18.5f, 22.0f, 31.0f));
        }

        // multiplication
        {
            vec4f v(1.5f, -2.75f, 3.0f, 4.0f);
            v *= 5.0f;
            VX_CHECK(v == vec4f(7.5f, -13.75f, 15.0f, 20.0f));
            v *= vec4f(2.0f, -3.0f, 4.0f, 5.0f);
            VX_CHECK(v == vec4f(15.0f, 41.25f, 60.0f, 100.0f));
        }

        // division
        {
            vec4f v(100.5f, 50.0f, 75.0f, 40.0f);
            v /= 5.0f;
            VX_CHECK(v == vec4f(20.1f, 10.0f, 15.0f, 8.0f));
            v /= vec4f(2.0f, 5.0f, 3.0f, 4.0f);
            VX_CHECK(v == vec4f(10.05f, 2.0f, 5.0f, 2.0f));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec4f(1.5f, 0.0f, 3.0f, 2.0f) && vec4f(1.5f, 2.75f, 0.0f, 4.0f)) == vec4b(true, false, false, true));
        VX_STATIC_CHECK((vec4f(0.0f, 0.0f, 0.0f, 0.0f) && vec4f(1.5f, 1.5f, 1.5f, 1.5f)) == vec4b(false, false, false, false));

        // or
        VX_STATIC_CHECK((vec4f(1.5f, 0.0f, 3.0f, 2.0f) || vec4f(0.0f, 1.5f, 0.0f, 4.0f)) == vec4b(true, true, true, true));
        VX_STATIC_CHECK((vec4f(0.0f, 0.0f, 0.0f, 0.0f) || vec4f(0.0f, 1.5f, 0.0f, 0.0f)) == vec4b(false, true, false, false));

        // not
        VX_STATIC_CHECK((!vec4f(1.5f, 0.0f, 3.0f, 2.0f)) == vec4b(false, true, false, false));
        VX_STATIC_CHECK((!vec4f(0.0f, 0.0f, 0.0f, 0.0f)) == vec4b(true, true, true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec4f::zero() == vec4f(0.0f, 0.0f, 0.0f, 0.0f));
        VX_STATIC_CHECK(vec4f::one() == vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec4d)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec4d::scalar_type, f64>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec4d) == 32);
        VX_STATIC_CHECK(vec4d::size == 4);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec4d(), 0.0, 0.0, 0.0, 0.0);
        check_each(vec4d(vec4d(1.0, 2.0, 3.0, 4.0)), 1.0, 2.0, 3.0, 4.0);

        // explicit constructors
        check_each(vec4d(7.5), 7.5, 7.5, 7.5, 7.5);
        check_each(vec4d(1.0, 2.0, 3.0, 4.0), 1.0, 2.0, 3.0, 4.0);

        // conversion constructors
        check_each(vec4d(true), 1.0, 1.0, 1.0, 1.0);
        check_each(vec4d(false), 0.0, 0.0, 0.0, 0.0);
        check_each(vec4d(1), 1.0, 1.0, 1.0, 1.0);
        check_each(vec4d(1u), 1.0, 1.0, 1.0, 1.0);
        check_each(vec4d(1.0f), 1.0, 1.0, 1.0, 1.0);

        constexpr auto x = vec4d(5, vec3f(1e5f, 0.0f, -1.5));

        // mixed
        check_each(vec4d(true, 5, 1.5f, -3.75), 1.0, 5.0, 1.5, -3.75);
        check_each(vec4d(vec2f(1.5f, 2.5f), 3u, -4), 1.5, 2.5, 3.0, -4.0);
        check_each(vec4d(1.5f, vec2i(2, 3), 4u), 1.5, 2.0, 3.0, 4.0);
        check_each(vec4d(1.5f, 2.5f, vec2u(3u, 4u)), 1.5, 2.5, 3.0, 4.0);
        check_each(vec4d(vec2u(5u, 6u), vec2i(7, 8)), 5.0, 6.0, 7.0, 8.0);
        check_each(vec4d(1.0f, vec2f(2.0f, 3.0f), 4.0f), 1.0, 2.0, 3.0, 4.0);
        check_each(vec4d(vec3b(true, false, true), true), 1.0, 0.0, 1.0, 1.0);
        check_each(vec4d(5, vec3f(1e5f, 0.0f, -1.5)), 5.0, 100000.0, 0.0, -1.5);
        check_each(vec4d(vec2b(true, true), vec2b(false, true)), 1.0, 1.0, 0.0, 1.0);

        // vec4
        check_each(static_cast<vec4d>(vec4b(true, false, true, false)), 1.0, 0.0, 1.0, 0.0);
        check_each(static_cast<vec4d>(vec4i(0, 1, 2, 3)), 0.0, 1.0, 2.0, 3.0);
        check_each(static_cast<vec4d>(vec4u(0u, 1u, 2u, 3u)), 0.0, 1.0, 2.0, 3.0);
        check_each(static_cast<vec4d>(vec4f(0.0f, 1.0f, 2.0f, 3.5f)), 0.0, 1.0, 2.0, 3.5);
    }

    VX_SECTION("assignment")
    {
        {
            vec4d v1(0.0, 1.0, 2.0, 3.0);
            vec4b v2(true, false, true, false);

            v1 = v2;
            VX_CHECK(v1.x == 1.0);
            VX_CHECK(v1.y == 0.0);
            VX_CHECK(v1.z == 1.0);
            VX_CHECK(v1.w == 0.0);
        }

        {
            vec4d v1(0.0, 1.0, 2.0, 3.0);
            vec4i v2(3, 2, 1, 4);

            v1 = v2;
            VX_CHECK(v1.x == 3.0);
            VX_CHECK(v1.y == 2.0);
            VX_CHECK(v1.z == 1.0);
            VX_CHECK(v1.w == 4.0);
        }

        {
            vec4d v1(0.0, 1.0, 2.0, 3.0);
            vec4u v2(3u, 2u, 1u, 4u);

            v1 = v2;
            VX_CHECK(v1.x == 3.0);
            VX_CHECK(v1.y == 2.0);
            VX_CHECK(v1.z == 1.0);
            VX_CHECK(v1.w == 4.0);
        }

        {
            vec4d v1(0.0, 1.0, 2.0, 3.0);
            vec4f v2(3.0f, 2.0f, 1.0f, 4.0f);

            v1 = v2;
            VX_CHECK(v1.x == 3.0);
            VX_CHECK(v1.y == 2.0);
            VX_CHECK(v1.z == 1.0);
            VX_CHECK(v1.w == 4.0);
        }

        {
            vec4d v1(0.0, 1.0, 2.0, 3.0);
            vec4d v2(3.0, 2.0, 1.0, 4.0);

            v1 = v2;
            VX_CHECK(v1.x == 3.0);
            VX_CHECK(v1.y == 2.0);
            VX_CHECK(v1.z == 1.0);
            VX_CHECK(v1.w == 4.0);
        }
    }


    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec4d(1.5, 2.75, -1.23, 4.5)[0] == 1.5);
        VX_STATIC_CHECK(vec4d(1.5, 2.75, -1.23, 4.5)[1] == 2.75);
        VX_STATIC_CHECK(vec4d(1.5, 2.75, -1.23, 4.5)[2] == -1.23);
        VX_STATIC_CHECK(vec4d(1.5, 2.75, -1.23, 4.5)[3] == 4.5);

        vec4d v;

        v[0] = 1.5;
        VX_CHECK(v[0] == 1.5);
        v[1] = 2.75;
        VX_CHECK(v[1] == 2.75);
        v[2] = -1.23;
        VX_CHECK(v[2] == -1.23);
        v[3] = 4.5;
        VX_CHECK(v[3] == 4.5);
    }


    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec4d(1.5, -2.75, -1.23, 4.5) == vec4d(1.5, -2.75, -1.23, 4.5));
        VX_STATIC_CHECK(vec4d(1.5, 2.75, -1.23, 4.5) != vec4d(-1.5, -2.75, -1.23, 4.5));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec4d(1.0, -1.5, 2.0, 4.0) == vec4d(1.0, -1.5, 2.0, 4.0));
        VX_STATIC_CHECK(+vec4d(-1.5, 2.5, -3.0, 1.0) == vec4d(-1.5, 2.5, -3.0, 1.0));

        VX_STATIC_CHECK(-vec4d(1.5, -2.5, 3.0, -4.0) == vec4d(-1.5, 2.5, -3.0, 4.0));
        VX_STATIC_CHECK(-vec4d(-1.5, 2.5, -3.0, 4.0) == vec4d(1.5, -2.5, 3.0, -4.0));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec4d(0.5, -1.0, 2.0, 4.0) == vec4d(1.5, 0.0, 3.0, 5.0));
        VX_STATIC_CHECK(--vec4d(0.5, -1.0, 2.0, 4.0) == vec4d(-0.5, -2.0, 1.0, 3.0));

        vec4d v(0.5, -1.0, 2.0, 4.0);

        VX_CHECK(v++ == vec4d(0.5, -1.0, 2.0, 4.0));
        VX_CHECK(v == vec4d(1.5, 0.0, 3.0, 5.0));
        VX_CHECK(v-- == vec4d(1.5, 0.0, 3.0, 5.0));
        VX_CHECK(v == vec4d(0.5, -1.0, 2.0, 4.0));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec4d(1.5, -2.75, 3.0, 4.0) + 5.0 == vec4d(6.5, 2.25, 8.0, 9.0));
        VX_STATIC_CHECK(5.0 + vec4d(1.5, -2.75, 3.0, 4.0) == vec4d(6.5, 2.25, 8.0, 9.0));
        VX_STATIC_CHECK(vec4d(5.5) + vec4d(1.5, -2.75, 3.0, 4.0) == vec4d(7.0, 2.75, 8.5, 9.5));
        VX_STATIC_CHECK(vec4d(1.5, -2.75, 3.0, 4.0) + vec4d(5.0) == vec4d(6.5, 2.25, 8.0, 9.0));

        // subtraction
        VX_STATIC_CHECK(vec4d(1.5, -2.75, 3.0, 4.0) - 5.0 == vec4d(-3.5, -7.75, -2.0, -1.0));
        VX_STATIC_CHECK(5.0 - vec4d(1.5, -2.75, 3.0, 4.0) == vec4d(3.5, 7.75, 2.0, 1.0));
        VX_STATIC_CHECK(vec4d(5.5) - vec4d(1.5, -2.75, 3.0, 4.0) == vec4d(4.0, 8.25, 2.5, 1.5));
        VX_STATIC_CHECK(vec4d(1.5, -2.75, 3.0, 4.0) - vec4d(5.0) == vec4d(-3.5, -7.75, -2.0, -1.0));

        // multiplication
        VX_STATIC_CHECK(vec4d(1.5, -2.75, 3.0, 4.0) * 5.0 == vec4d(7.5, -13.75, 15.0, 20.0));
        VX_STATIC_CHECK(5.0 * vec4d(1.5, -2.75, 3.0, 4.0) == vec4d(7.5, -13.75, 15.0, 20.0));
        VX_STATIC_CHECK(vec4d(5.5) * vec4d(1.5, -2.75, 3.0, 4.0) == vec4d(8.25, -15.125, 16.5, 22.0));
        VX_STATIC_CHECK(vec4d(1.5, -2.75, 3.0, 4.0) * vec4d(5.0) == vec4d(7.5, -13.75, 15.0, 20.0));

        // division
        VX_STATIC_CHECK(vec4d(5.5, -13.75, 15.0, 20.0) / 5.0 == vec4d(1.1, -2.75, 3.0, 4.0));
        VX_STATIC_CHECK(10.0 / vec4d(10.0, 5.0, 2.0, 1.0) == vec4d(1.0, 2.0, 5.0, 10.0));
        VX_STATIC_CHECK(vec4d(10.5) / vec4d(10.0, 5.0, 2.0, 1.0) == vec4d(1.05, 2.1, 5.25, 10.5));
        VX_STATIC_CHECK(vec4d(10.0, 5.0, 2.0, 1.0) / vec4d(5.0, 10.0, 2.0, 1.0) == vec4d(2.0, 0.5, 1.0, 1.0));
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec4d v;
            v += 5.5;
            VX_CHECK(v == vec4d(5.5, 5.5, 5.5, 5.5));
            v += vec4d(1.5, -2.75, 3.0, -1.0);
            VX_CHECK(v == vec4d(7.0, 2.75, 8.5, 4.5));
        }

        // subtraction
        {
            vec4d v(10.5, 20.75, 30.0, 40.0);
            v -= 5.0;
            VX_CHECK(v == vec4d(5.5, 15.75, 25.0, 35.0));
            v -= vec4d(1.5, -2.75, 3.0, -4.0);
            VX_CHECK(v == vec4d(4.0, 18.5, 22.0, 39.0));
        }

        // multiplication
        {
            vec4d v(1.5, -2.75, 3.0, 4.0);
            v *= 5.0;
            VX_CHECK(v == vec4d(7.5, -13.75, 15.0, 20.0));
            v *= vec4d(2.0, -3.0, 4.0, 1.0);
            VX_CHECK(v == vec4d(15.0, 41.25, 60.0, 20.0));
        }

        // division
        {
            vec4d v(100.5, 50.0, 75.0, 120.0);
            v /= 5.0;
            VX_CHECK(v == vec4d(20.1, 10.0, 15.0, 24.0));
            v /= vec4d(2.0, 5.0, 3.0, 6.0);
            VX_CHECK(v == vec4d(10.05, 2.0, 5.0, 4.0));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec4d(1.5, 0.0, 3.0, 2.0) && vec4d(1.5, 2.75, 0.0, 4.0)) == vec4b(true, false, false, true));
        VX_STATIC_CHECK((vec4d(0.0, 0.0, 0.0, 0.0) && vec4d(1.5, 1.5, 1.5, 1.5)) == vec4b(false, false, false, false));

        // or
        VX_STATIC_CHECK((vec4d(1.5, 0.0, 3.0, 2.0) || vec4d(0.0, 1.5, 0.0, 0.0)) == vec4b(true, true, true, true));
        VX_STATIC_CHECK((vec4d(0.0, 0.0, 0.0, 0.0) || vec4d(0.0, 1.5, 0.0, 0.0)) == vec4b(false, true, false, false));

        // not
        VX_STATIC_CHECK((!vec4d(1.5, 0.0, 3.0, 2.0)) == vec4b(false, true, false, false));
        VX_STATIC_CHECK((!vec4d(0.0, 0.0, 0.0, 0.0)) == vec4b(true, true, true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec4d::zero() == vec4d(0.0, 0.0, 0.0, 0.0));
        VX_STATIC_CHECK(vec4d::one() == vec4d(1.0, 1.0, 1.0, 1.0));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}