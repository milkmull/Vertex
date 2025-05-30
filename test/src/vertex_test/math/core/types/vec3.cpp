#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

#define check_each(vec, vx, vy, vz) VX_STATIC_CHECK(vec.x == vx && vec.y == vy && vec.z == vz)

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec3b)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec3b::scalar_type, bool>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec3b) == 3);
        VX_STATIC_CHECK(vec3b::size == 3);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec3b(), false, false, false);
        check_each(vec3b(vec3b(true, false, true)), true, false, true);

        // explicit constructors
        check_each(vec3b(true), true, true, true);
        check_each(vec3b(true, false, true), true, false, true);

        // conversion constructors
        check_each(vec3b(1), true, true, true);
        check_each(vec3b(1u), true, true, true);
        check_each(vec3b(1.0f), true, true, true);
        check_each(vec3b(1.0), true, true, true);

        // mixed
        check_each(vec3b(true, 1, 1.0f), true, true, true);
        check_each(vec3b(vec2i(0, 1), 0u), false, true, false);
        check_each(vec3b(1.0, vec2f(1, 0)), true, true, false);

        // vec3
        check_each(static_cast<vec3b>(vec3i(0, 1, 0)), false, true, false);
        check_each(static_cast<vec3b>(vec3u(0u, 1u, 0u)), false, true, false);
        check_each(static_cast<vec3b>(vec3f(0.0f, 1.0f, 0.0f)), false, true, false);
        check_each(static_cast<vec3b>(vec3d(0.0, 1.0, 0.0)), false, true, false);

        // vec4
        check_each(static_cast<vec3b>(vec4b(true, false, true, false)), true, false, true);
        check_each(static_cast<vec3b>(vec4i(0, 1, 0, 1)), false, true, false);
        check_each(static_cast<vec3b>(vec4u(0u, 1u, 0u, 1u)), false, true, false);
        check_each(static_cast<vec3b>(vec4f(0.0f, 1.0f, 0.0f, 1.0f)), false, true, false);
        check_each(static_cast<vec3b>(vec4d(0.0, 1.0, 0.0, 1.0)), false, true, false);
    }

    VX_SECTION("assignment")
    {
        {
            vec3b v1(false, true, false);
            vec3b v2(true, false, true);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
            VX_CHECK(v1.z == true);
        }

        {
            vec3b v1(false, true, false);
            vec3i v2(1, 0, 1);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
            VX_CHECK(v1.z == true);
        }

        {
            vec3b v1(false, true, false);
            vec3u v2(1u, 0u, 1u);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
            VX_CHECK(v1.z == true);
        }

        {
            vec3b v1(false, true, false);
            vec3f v2(1.0f, 0.0f, 1.0f);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
            VX_CHECK(v1.z == true);
        }

        {
            vec3b v1(false, true, false);
            vec3d v2(1.0, 0.0, 1.0);

            v1 = v2;
            VX_CHECK(v1.x == true);
            VX_CHECK(v1.y == false);
            VX_CHECK(v1.z == true);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec3b(false, true, false)[0] == false);
        VX_STATIC_CHECK(vec3b(false, true, false)[1] == true);
        VX_STATIC_CHECK(vec3b(false, true, false)[2] == false);

        vec3b v;

        v[0] = true;
        VX_CHECK(v[0] == true);
        v[1] = true;
        VX_CHECK(v[1] == true);
        v[2] = true;
        VX_CHECK(v[2] == true);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec3b() == vec3b());
        VX_STATIC_CHECK(vec3b() != vec3b(true));
    }

    VX_SECTION("boolean operators")
    {
        constexpr vec3b v_true(true, true, true);
        constexpr vec3b v_false(false, false, false);
        constexpr vec3b v_mixed(true, false, true);

        // && with scalar
        VX_STATIC_CHECK((v_true && true) == vec3b(true, true, true));
        VX_STATIC_CHECK((v_true && false) == vec3b(false, false, false));
        VX_STATIC_CHECK((v_false && true) == vec3b(false, false, false));
        VX_STATIC_CHECK((v_mixed && true) == vec3b(true, false, true));
        VX_STATIC_CHECK((true && v_mixed) == vec3b(true, false, true));
        VX_STATIC_CHECK((false && v_mixed) == vec3b(false, false, false));

        // && with vector
        VX_STATIC_CHECK((v_mixed && v_true) == vec3b(true, false, true));
        VX_STATIC_CHECK((v_mixed && v_false) == vec3b(false, false, false));
        VX_STATIC_CHECK((v_true && v_true) == vec3b(true, true, true));

        // || with scalar
        VX_STATIC_CHECK((v_false || false) == vec3b(false, false, false));
        VX_STATIC_CHECK((v_false || true) == vec3b(true, true, true));
        VX_STATIC_CHECK((v_mixed || false) == vec3b(true, false, true));
        VX_STATIC_CHECK((v_mixed || true) == vec3b(true, true, true));
        VX_STATIC_CHECK((true || v_false) == vec3b(true, true, true));
        VX_STATIC_CHECK((false || v_mixed) == vec3b(true, false, true));

        // || with vector
        VX_STATIC_CHECK((v_mixed || v_false) == vec3b(true, false, true));
        VX_STATIC_CHECK((v_mixed || v_true) == vec3b(true, true, true));
        VX_STATIC_CHECK((v_false || v_false) == vec3b(false, false, false));

        // ! operator
        VX_STATIC_CHECK((!v_true) == vec3b(false, false, false));
        VX_STATIC_CHECK((!v_false) == vec3b(true, true, true));
        VX_STATIC_CHECK((!v_mixed) == vec3b(false, true, false));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec3i)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec3i::scalar_type, i32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec3i) == 12);
        VX_STATIC_CHECK(vec3i::size == 3);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec3i(), 0, 0, 0);
        check_each(vec3i(vec3i(1, 2, 3)), 1, 2, 3);

        // explicit constructors
        check_each(vec3i(1), 1, 1, 1);
        check_each(vec3i(1, 2, 3), 1, 2, 3);

        // conversion constructors
        check_each(vec3i(true), 1, 1, 1);
        check_each(vec3i(5u), 5, 5, 5);
        check_each(vec3i(5.0f), 5, 5, 5);
        check_each(vec3i(5.0), 5, 5, 5);

        // mixed
        check_each(vec3i(true, -5, 100.0), 1, -5, 100);
        check_each(vec3i(vec2b(false, true), 24u), 0, 1, 24);
        check_each(vec3i(1.0f, vec2d(-37.5, 1.23)), 1, -37, 1);

        // vec3
        check_each(static_cast<vec3i>(vec3b(false, true, false)), 0, 1, 0);
        check_each(static_cast<vec3i>(vec3u(1u, 2u, 3u)), 1, 2, 3);
        check_each(static_cast<vec3i>(vec3f(1.5f, 2.5f, 3.5f)), 1, 2, 3);
        check_each(static_cast<vec3i>(vec3d(0.5, 1.5, 2.5)), 0, 1, 2);

        // vec4
        check_each(static_cast<vec3i>(vec4b(true, false, true, false)), 1, 0, 1);
        check_each(static_cast<vec3i>(vec4i(0, 1, 2, 3)), 0, 1, 2);
        check_each(static_cast<vec3i>(vec4u(0u, 1u, 2u, 3u)), 0, 1, 2);
        check_each(static_cast<vec3i>(vec4f(0.0f, 1.0f, 2.0f, 3.0f)), 0, 1, 2);
        check_each(static_cast<vec3i>(vec4d(0.0, 1.0, 2.0, 3.0)), 0, 1, 2);
    }

    VX_SECTION("assignment")
    {
        {
            vec3i v1(0, 1, 2);
            vec3b v2(true, false, true);

            v1 = v2;
            VX_CHECK(v1.x == 1);
            VX_CHECK(v1.y == 0);
            VX_CHECK(v1.z == 1);
        }

        {
            vec3i v1(1, 2, 3);
            vec3i v2(3, 2, 1);

            v1 = v2;
            VX_CHECK(v1.x == 3);
            VX_CHECK(v1.y == 2);
            VX_CHECK(v1.z == 1);
        }

        {
            vec3i v1(1, 2, 2);
            vec3u v2(3u, 2u, 1u);

            v1 = v2;
            VX_CHECK(v1.x == 3);
            VX_CHECK(v1.y == 2);
            VX_CHECK(v1.z == 1);
        }

        {
            vec3i v1(1, 2, 3);
            vec3f v2(3.0f, 2.0f, 1.0f);

            v1 = v2;
            VX_CHECK(v1.x == 3);
            VX_CHECK(v1.y == 2);
            VX_CHECK(v1.z == 1);
        }

        {
            vec3i v1(1, 2, 3);
            vec3d v2(3.0, 2.0, 1.0);

            v1 = v2;
            VX_CHECK(v1.x == 3);
            VX_CHECK(v1.y == 2);
            VX_CHECK(v1.z == 1);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec3i(1, 2, 3)[0] == 1);
        VX_STATIC_CHECK(vec3i(1, 2, 3)[1] == 2);
        VX_STATIC_CHECK(vec3i(1, 2, 3)[2] == 3);

        vec3i v;

        v[0] = 1;
        VX_CHECK(v[0] == 1);
        v[1] = 2;
        VX_CHECK(v[1] == 2);
        v[2] = 3;
        VX_CHECK(v[2] == 3);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec3i(1) == vec3i(1));
        VX_STATIC_CHECK(vec3i(1) != vec3i(2));
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
        VX_STATIC_CHECK(++vec3i(0) == vec3i(1));
        VX_STATIC_CHECK(--vec3i(0) == vec3i(-1));

        vec3i v(0);

        VX_CHECK(v++ == vec3i(0));
        VX_CHECK(v == vec3i(1));
        VX_CHECK(v-- == vec3i(1));
        VX_CHECK(v == vec3i(0));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec3i(1, 2, 3) + 5 == vec3i(6, 7, 8));
        VX_STATIC_CHECK(5 + vec3i(1, 2, 3) == vec3i(6, 7, 8));
        VX_STATIC_CHECK(vec3i(5) + vec3i(1, 2, 3) == vec3i(6, 7, 8));
        VX_STATIC_CHECK(vec3i(1, 2, 3) + vec3i(5) == vec3i(6, 7, 8));

        // subtraction
        VX_STATIC_CHECK(vec3i(1, 2, 3) - 5 == vec3i(-4, -3, -2));
        VX_STATIC_CHECK(5 - vec3i(1, 2, 3) == vec3i(4, 3, 2));
        VX_STATIC_CHECK(vec3i(5) - vec3i(1, 2, 3) == vec3i(4, 3, 2));
        VX_STATIC_CHECK(vec3i(1, 2, 3) - vec3i(5) == vec3i(-4, -3, -2));

        // multiplication
        VX_STATIC_CHECK(vec3i(1, 2, 3) * 5 == vec3i(5, 10, 15));
        VX_STATIC_CHECK(5 * vec3i(1, 2, 3) == vec3i(5, 10, 15));
        VX_STATIC_CHECK(vec3i(5) * vec3i(1, 2, 3) == vec3i(5, 10, 15));
        VX_STATIC_CHECK(vec3i(1, 2, 3) * vec3i(5) == vec3i(5, 10, 15));

        // division
        VX_STATIC_CHECK(vec3i(5, 10, 15) / 5 == vec3i(1, 2, 3));
        VX_STATIC_CHECK(15 / vec3i(15, 5, 3) == vec3i(1, 3, 5));
        VX_STATIC_CHECK(vec3i(15) / vec3i(15, 5, 3) == vec3i(1, 3, 5));
        VX_STATIC_CHECK(vec3i(15, 10, 5) / vec3i(5, 2, 5) == vec3i(3, 5, 1));

        // modulo
        VX_STATIC_CHECK(vec3i(5, 10, 15) % 4 == vec3i(1, 2, 3));
        VX_STATIC_CHECK(10 % vec3i(3, 4, 7) == vec3i(1, 2, 3));
        VX_STATIC_CHECK(vec3i(10) % vec3i(3, 4, 7) == vec3i(1, 2, 3));
        VX_STATIC_CHECK(vec3i(10, 5, 11) % vec3i(4, 3, 5) == vec3i(2, 2, 1));
    }

    VX_SECTION("binary bit operators")
    {
        // and
        VX_STATIC_CHECK((vec3i(5, 6, 7) & 3) == vec3i(1, 2, 3));
        VX_STATIC_CHECK((3 & vec3i(5, 6, 7)) == vec3i(1, 2, 3));
        VX_STATIC_CHECK((vec3i(5, 6, 7) & vec3i(3, 3, 3)) == vec3i(1, 2, 3));

        // or
        VX_STATIC_CHECK((vec3i(5, 6, 7) | 3) == vec3i(7, 7, 7));
        VX_STATIC_CHECK((3 | vec3i(5, 6, 7)) == vec3i(7, 7, 7));
        VX_STATIC_CHECK((vec3i(5, 6, 7) | vec3i(3, 3, 3)) == vec3i(7, 7, 7));

        // xor
        VX_STATIC_CHECK((vec3i(5, 6, 7) ^ 3) == vec3i(6, 5, 4));
        VX_STATIC_CHECK((3 ^ vec3i(5, 6, 7)) == vec3i(6, 5, 4));
        VX_STATIC_CHECK((vec3i(5, 6, 7) ^ vec3i(3, 3, 3)) == vec3i(6, 5, 4));

        // left shift
        VX_STATIC_CHECK((vec3i(1, 2, 3) << 1) == vec3i(2, 4, 6));
        VX_STATIC_CHECK((1 << vec3i(1, 2, 3)) == vec3i(2, 4, 8));
        VX_STATIC_CHECK((vec3i(1, 2, 3) << vec3i(1, 2, 3)) == vec3i(2, 8, 24));

        // right shift
        VX_STATIC_CHECK((vec3i(4, 8, 16) >> 1) == vec3i(2, 4, 8));
        VX_STATIC_CHECK((16 >> vec3i(2, 3, 4)) == vec3i(4, 2, 1));
        VX_STATIC_CHECK((vec3i(8, 16, 32) >> vec3i(2, 3, 4)) == vec3i(2, 2, 2));

        // not
        VX_STATIC_CHECK((~vec3i(0, -1, 42)) == vec3i(~0, ~(-1), ~42));
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec3i v;
            v += 5;
            VX_CHECK(v == vec3i(5, 5, 5));
            v += vec3i(1, 2, 3);
            VX_CHECK(v == vec3i(6, 7, 8));
        }

        // subtraction
        {
            vec3i v(10, 20, 30);
            v -= 5;
            VX_CHECK(v == vec3i(5, 15, 25));
            v -= vec3i(1, 2, 3);
            VX_CHECK(v == vec3i(4, 13, 22));
        }

        // multiplication
        {
            vec3i v(1, 2, 3);
            v *= 5;
            VX_CHECK(v == vec3i(5, 10, 15));
            v *= vec3i(2, 3, 4);
            VX_CHECK(v == vec3i(10, 30, 60));
        }

        // division
        {
            vec3i v(100, 50, 25);
            v /= 5;
            VX_CHECK(v == vec3i(20, 10, 5));
            v /= vec3i(2, 5, 5);
            VX_CHECK(v == vec3i(10, 2, 1));
        }

        // modulo
        {
            vec3i v(10, 20, 30);
            v %= 4;
            VX_CHECK(v == vec3i(2, 0, 2));
            v %= vec3i(2, 3, 4);
            VX_CHECK(v == vec3i(0, 0, 2));
        }
    }

    VX_SECTION("unary bit operators")
    {
        // and
        {
            vec3i v(7, 14, 15);
            v &= 3;
            VX_CHECK(v == vec3i(3, 2, 3));
            v &= vec3i(1, 2, 3);
            VX_CHECK(v == vec3i(1, 2, 3));
        }

        // or
        {
            vec3i v(1, 2, 3);
            v |= 4;
            VX_CHECK(v == vec3i(5, 6, 7));
            v |= vec3i(2, 1, 0);
            VX_CHECK(v == vec3i(7, 7, 7));
        }

        // xor
        {
            vec3i v(5, 6, 7);
            v ^= 3;
            VX_CHECK(v == vec3i(6, 5, 4));
            v ^= vec3i(1, 1, 1);
            VX_CHECK(v == vec3i(7, 4, 5));
        }

        // left shift
        {
            vec3i v(1, 2, 3);
            v <<= 1;
            VX_CHECK(v == vec3i(2, 4, 6));
            v <<= vec3i(1, 2, 3);
            VX_CHECK(v == vec3i(4, 16, 48));
        }

        // right shift
        {
            vec3i v(8, 16, 32);
            v >>= 1;
            VX_CHECK(v == vec3i(4, 8, 16));
            v >>= vec3i(2, 3, 4);
            VX_CHECK(v == vec3i(1, 1, 1));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec3i(1, 0, 3) && vec3i(1, 2, 0)) == vec3b(true, false, false));
        VX_STATIC_CHECK((vec3i(0, 0, 0) && vec3i(1, 1, 1)) == vec3b(false, false, false));

        // or
        VX_STATIC_CHECK((vec3i(1, 0, 0) || vec3i(0, 1, 1)) == vec3b(true, true, true));
        VX_STATIC_CHECK((vec3i(0, 0, 0) || vec3i(0, 1, 0)) == vec3b(false, true, false));

        // not
        VX_STATIC_CHECK((!vec3i(1, 0, 3)) == vec3b(false, true, false));
        VX_STATIC_CHECK((!vec3i(0, 0, 0)) == vec3b(true, true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec3i::zero() == vec3i(0, 0, 0));
        VX_STATIC_CHECK(vec3i::one() == vec3i(1, 1, 1));

        VX_STATIC_CHECK(vec3i::right() == vec3i(1, 0, 0));
        VX_STATIC_CHECK(vec3i::left() == vec3i(-1, 0, 0));

        VX_STATIC_CHECK(vec3i::up() == vec3i(0, 1, 0));
        VX_STATIC_CHECK(vec3i::down() == vec3i(0, -1, 0));

#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_RH_BIT)
        VX_STATIC_CHECK(vec3i::forward() == vec3i(0, 0, -1));
        VX_STATIC_CHECK(vec3i::back() == vec3i(0, 0, 1));
#else
        VX_STATIC_CHECK(vec3i::forward() == vec3i(0, 0, 1));
        VX_STATIC_CHECK(vec3i::back() == vec3i(0, 0, -1));
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec3u)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec3u::scalar_type, u32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec3u) == 12);
        VX_STATIC_CHECK(vec3u::size == 3);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec3u(), 0u, 0u, 0u);
        check_each(vec3u(vec3u(1u, 2u, 3u)), 1u, 2u, 3u);

        // explicit constructors
        check_each(vec3u(1u), 1u, 1u, 1u);
        check_each(vec3u(1u, 2u, 3u), 1u, 2u, 3u);

        // conversion constructors
        check_each(vec3u(true), 1u, 1u, 1u);
        check_each(vec3u(5), 5u, 5u, 5u);
        check_each(vec3u(5.0f), 5u, 5u, 5u);
        check_each(vec3u(5.0), 5u, 5u, 5u);

        // mixed
        check_each(vec3u(true, -5, 100.0), 1u, 4294967291u, 100u);
        check_each(vec3u(vec2b(false, true), 24), 0u, 1u, 24u);
        check_each(vec3u(1.0f, vec2(37.5, 1.23)), 1u, 37u, 1u);

        // vec3
        check_each(static_cast<vec3u>(vec3b(false, true, false)), 0u, 1u, 0u);
        check_each(static_cast<vec3u>(vec3i(1, 2, 3)), 1u, 2u, 3u);
        check_each(static_cast<vec3u>(vec3f(1.5f, 2.5f, 3.5f)), 1u, 2u, 3u);
        check_each(static_cast<vec3u>(vec3d(0.5, 1.5, 2.5)), 0u, 1u, 2u);

        // vec4
        check_each(static_cast<vec3u>(vec4b(true, false, true, false)), 1u, 0u, 1u);
        check_each(static_cast<vec3u>(vec4i(0, 1, 2, 3)), 0u, 1u, 2u);
        check_each(static_cast<vec3u>(vec4u(0u, 1u, 2u, 3u)), 0u, 1u, 2u);
        check_each(static_cast<vec3u>(vec4f(0.0f, 1.0f, 2.0f, 3.0f)), 0u, 1u, 2u);
        check_each(static_cast<vec3u>(vec4d(0.0, 1.0, 2.0, 3.0)), 0u, 1u, 2u);
    }

    VX_SECTION("assignment")
    {
        {
            vec3u v1(0u, 1u, 2u);
            vec3b v2(true, false, true);

            v1 = v2;
            VX_CHECK(v1.x == 1u);
            VX_CHECK(v1.y == 0u);
            VX_CHECK(v1.z == 1u);
        }

        {
            vec3u v1(1u, 2u, 3u);
            vec3i v2(3, 2, 1);

            v1 = v2;
            VX_CHECK(v1.x == 3u);
            VX_CHECK(v1.y == 2u);
            VX_CHECK(v1.z == 1u);
        }

        {
            vec3u v1(1u, 2u, 3u);
            vec3u v2(3, 2u, 1u);

            v1 = v2;
            VX_CHECK(v1.x == 3u);
            VX_CHECK(v1.y == 2u);
            VX_CHECK(v1.z == 1u);
        }

        {
            vec3u v1(1u, 2u, 3u);
            vec3f v2(3.0f, 2.0f, 1.0f);

            v1 = v2;
            VX_CHECK(v1.x == 3u);
            VX_CHECK(v1.y == 2u);
            VX_CHECK(v1.z == 1u);
        }

        {
            vec3u v1(1u, 2u, 3u);
            vec3d v2(3.0, 2.0, 1.0);

            v1 = v2;
            VX_CHECK(v1.x == 3u);
            VX_CHECK(v1.y == 2u);
            VX_CHECK(v1.z == 1u);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec3u(1, 2, 3)[0] == 1u);
        VX_STATIC_CHECK(vec3u(1, 2, 3)[1] == 2u);
        VX_STATIC_CHECK(vec3u(1, 2, 3)[2] == 3u);

        vec3u v;

        v[0] = 1u;
        VX_CHECK(v[0] == 1u);
        v[1] = 2u;
        VX_CHECK(v[1] == 2u);
        v[2] = 3u;
        VX_CHECK(v[2] == 3u);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec3u(1u) == vec3u(1u));
        VX_STATIC_CHECK(vec3u(1u) != vec3u(2u));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec3u(1u) == vec3u(1u));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec3u(0u) == vec3u(1u));
        VX_STATIC_CHECK(--vec3u(1u) == vec3u(0u));

        vec3u v(0u);

        VX_CHECK(v++ == vec3u(0u));
        VX_CHECK(v == vec3u(1u));
        VX_CHECK(v-- == vec3u(1u));
        VX_CHECK(v == vec3u(0u));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec3u(1u, 2u, 3u) + 5u == vec3u(6u, 7u, 8u));
        VX_STATIC_CHECK(5u + vec3u(1u, 2u, 3u) == vec3u(6u, 7u, 8u));
        VX_STATIC_CHECK(vec3u(5u) + vec3u(1u, 2u, 3u) == vec3u(6u, 7u, 8u));
        VX_STATIC_CHECK(vec3u(1u, 2u, 3u) + vec3u(5u) == vec3u(6u, 7u, 8u));

        // subtraction (vec3u) - avoids underflow
        VX_STATIC_CHECK(vec3u(6u, 7u, 8u) - 5u == vec3u(1u, 2u, 3u));
        VX_STATIC_CHECK(10u - vec3u(6u, 7u, 8u) == vec3u(4u, 3u, 2u));
        VX_STATIC_CHECK(vec3u(10u) - vec3u(6u, 7u, 8u) == vec3u(4u, 3u, 2u));
        VX_STATIC_CHECK(vec3u(6u, 7u, 8u) - vec3u(5u) == vec3u(1u, 2u, 3u));

        // multiplication
        VX_STATIC_CHECK(vec3u(1u, 2u, 3u) * 5u == vec3u(5u, 10u, 15u));
        VX_STATIC_CHECK(5u * vec3u(1u, 2u, 3u) == vec3u(5u, 10u, 15u));
        VX_STATIC_CHECK(vec3u(5u) * vec3u(1u, 2u, 3u) == vec3u(5u, 10u, 15u));
        VX_STATIC_CHECK(vec3u(1u, 2u, 3u) * vec3u(5u) == vec3u(5u, 10u, 15u));

        // division
        VX_STATIC_CHECK(vec3u(5u, 10u, 15u) / 5u == vec3u(1u, 2u, 3u));
        VX_STATIC_CHECK(15u / vec3u(15u, 5u, 3u) == vec3u(1u, 3u, 5u));
        VX_STATIC_CHECK(vec3u(15u) / vec3u(15u, 5u, 3u) == vec3u(1u, 3u, 5u));
        VX_STATIC_CHECK(vec3u(15u, 10u, 6u) / vec3u(5u, 10u, 3u) == vec3u(3u, 1u, 2u));

        // modulo
        VX_STATIC_CHECK(vec3u(5u, 10u, 15u) % 4u == vec3u(1u, 2u, 3u));
        VX_STATIC_CHECK(10u % vec3u(3u, 4u, 7u) == vec3u(1u, 2u, 3u));
        VX_STATIC_CHECK(vec3u(10u) % vec3u(3u, 4u, 7u) == vec3u(1u, 2u, 3u));
        VX_STATIC_CHECK(vec3u(10u, 5u, 9u) % vec3u(4u, 3u, 7u) == vec3u(2u, 2u, 2u));
    }

    VX_SECTION("binary bit operators")
    {
        // and
        VX_STATIC_CHECK((vec3u(5u, 6u, 7u) & 3u) == vec3u(1u, 2u, 3u));
        VX_STATIC_CHECK((3u & vec3u(5u, 6u, 7u)) == vec3u(1u, 2u, 3u));
        VX_STATIC_CHECK((vec3u(5u, 6u, 7u) & vec3u(3u)) == vec3u(1u, 2u, 3u));

        // or
        VX_STATIC_CHECK((vec3u(5u, 6u, 7u) | 3u) == vec3u(7u, 7u, 7u));
        VX_STATIC_CHECK((3u | vec3u(5u, 6u, 7u)) == vec3u(7u, 7u, 7u));
        VX_STATIC_CHECK((vec3u(5u, 6u, 7u) | vec3u(3u)) == vec3u(7u, 7u, 7u));

        // xor
        VX_STATIC_CHECK((vec3u(5u, 6u, 7u) ^ 3u) == vec3u(6u, 5u, 4u));
        VX_STATIC_CHECK((3u ^ vec3u(5u, 6u, 7u)) == vec3u(6u, 5u, 4u));
        VX_STATIC_CHECK((vec3u(5u, 6u, 7u) ^ vec3u(3u)) == vec3u(6u, 5u, 4u));

        // left shift
        VX_STATIC_CHECK((vec3u(1u, 2u, 3u) << 1u) == vec3u(2u, 4u, 6u));
        VX_STATIC_CHECK((1u << vec3u(1u, 2u, 3u)) == vec3u(2u, 4u, 8u));
        VX_STATIC_CHECK((vec3u(1u, 2u, 3u) << vec3u(1u, 2u, 3u)) == vec3u(2u, 8u, 24u));

        // right shift
        VX_STATIC_CHECK((vec3u(4u, 8u, 16u) >> 1u) == vec3u(2u, 4u, 8u));
        VX_STATIC_CHECK((16u >> vec3u(2u, 3u, 4u)) == vec3u(4u, 2u, 1u));
        VX_STATIC_CHECK((vec3u(16u, 32u, 64u) >> vec3u(2u, 3u, 4u)) == vec3u(4u, 4u, 4u));

        // not
        VX_STATIC_CHECK((~vec3u(0u, 0xFFFFFFFFu, 0x12345678u)) == vec3u(0xFFFFFFFFu, 0u, ~0x12345678u));
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec3u v;
            v += 5u;
            VX_CHECK(v == vec3u(5u, 5u, 5u));
            v += vec3u(1u, 2u, 3u);
            VX_CHECK(v == vec3u(6u, 7u, 8u));
        }

        // subtraction
        {
            vec3u v(10u, 20u, 30u);
            v -= 5u;
            VX_CHECK(v == vec3u(5u, 15u, 25u));
            v -= vec3u(1u, 2u, 3u);
            VX_CHECK(v == vec3u(4u, 13u, 22u));
        }

        // multiplication
        {
            vec3u v(1u, 2u, 3u);
            v *= 5u;
            VX_CHECK(v == vec3u(5u, 10u, 15u));
            v *= vec3u(2u, 3u, 4u);
            VX_CHECK(v == vec3u(10u, 30u, 60u));
        }

        // division
        {
            vec3u v(100u, 50u, 25u);
            v /= 5u;
            VX_CHECK(v == vec3u(20u, 10u, 5u));
            v /= vec3u(2u, 5u, 5u);
            VX_CHECK(v == vec3u(10u, 2u, 1u));
        }

        // modulo
        {
            vec3u v(10u, 20u, 30u);
            v %= 4u;
            VX_CHECK(v == vec3u(2u, 0u, 2u));
            v %= vec3u(2u, 3u, 5u);
            VX_CHECK(v == vec3u(0u, 0u, 2u));
        }

    }

    VX_SECTION("unary bit operators")
    {
        // and
        {
            vec3u v(7u, 14u, 0xFFu);
            v &= 3u;
            VX_CHECK(v == vec3u(3u, 2u, 3u));
            v &= vec3u(1u, 2u, 0x0Fu);
            VX_CHECK(v == vec3u(1u, 2u, 3u));
        }

        // or
        {
            vec3u v(1u, 2u, 3u);
            v |= 4u;
            VX_CHECK(v == vec3u(5u, 6u, 7u));
            v |= vec3u(2u, 1u, 8u);
            VX_CHECK(v == vec3u(7u, 7u, 15u));
        }

        // xor
        {
            vec3u v(5u, 6u, 7u);
            v ^= 3u;
            VX_CHECK(v == vec3u(6u, 5u, 4u));
            v ^= vec3u(1u, 1u, 1u);
            VX_CHECK(v == vec3u(7u, 4u, 5u));
        }

        // left shift
        {
            vec3u v(1u, 2u, 3u);
            v <<= 1u;
            VX_CHECK(v == vec3u(2u, 4u, 6u));
            v <<= vec3u(1u, 2u, 3u);
            VX_CHECK(v == vec3u(4u, 16u, 48u));
        }

        // right shift
        {
            vec3u v(8u, 16u, 64u);
            v >>= 1u;
            VX_CHECK(v == vec3u(4u, 8u, 32u));
            v >>= vec3u(2u, 3u, 4u);
            VX_CHECK(v == vec3u(1u, 1u, 2u));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec3u(1u, 0u, 3u) && vec3u(1u, 2u, 0u)) == vec3b(true, false, false));
        VX_STATIC_CHECK((vec3u(0u, 0u, 0u) && vec3u(1u, 1u, 1u)) == vec3b(false, false, false));

        // or
        VX_STATIC_CHECK((vec3u(1u, 0u, 0u) || vec3u(0u, 1u, 2u)) == vec3b(true, true, true));
        VX_STATIC_CHECK((vec3u(0u, 0u, 0u) || vec3u(0u, 1u, 0u)) == vec3b(false, true, false));

        // not
        VX_STATIC_CHECK((!vec3u(1u, 0u, 2u)) == vec3b(false, true, false));
        VX_STATIC_CHECK((!vec3u(0u, 0u, 0u)) == vec3b(true, true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec3u::zero() == vec3u(0u, 0u, 0u));
        VX_STATIC_CHECK(vec3u::one() == vec3u(1u, 1u, 1u));

        VX_STATIC_CHECK(vec3u::right() == vec3u(1u, 0u, 0u));
        VX_STATIC_CHECK(vec3u::up() == vec3u(0u, 1u, 0u));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec3f)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec3f::scalar_type, f32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec3f) == 12);
        VX_STATIC_CHECK(vec3f::size == 3);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec3f(), 0.0f, 0.0f, 0.0f);
        check_each(vec3f(vec3f(1.0f, 2.0f, 3.0f)), 1.0f, 2.0f, 3.0f);

        // explicit constructors
        check_each(vec3f(1.0f), 1.0f, 1.0f, 1.0f);
        check_each(vec3f(1.0f, 2.0f, 3.0f), 1.0f, 2.0f, 3.0f);

        // conversion constructors
        check_each(vec3f(true), 1.0f, 1.0f, 1.0f);
        check_each(vec3f(5), 5.0f, 5.0f, 5.0f);
        check_each(vec3f(5u), 5.0f, 5.0f, 5.0f);
        check_each(vec3f(5.0), 5.0f, 5.0f, 5.0f);

        // mixed
        check_each(vec3f(true, -5, 100.0), 1.0f, -5.0f, 100.0f);
        check_each(vec3f(vec2b(false, true), 24u), 0.0f, 1.0f, 24.0f);
        check_each(vec3f(1.0f, vec2d(-37.5, 1.23)), 1.0f, -37.5f, 1.23f);

        // vec3
        check_each(static_cast<vec3f>(vec3b(false, true, false)), 0.0f, 1.0f, 0.0f);
        check_each(static_cast<vec3f>(vec3i(1, -2, 3)), 1.0f, -2.0f, 3.0f);
        check_each(static_cast<vec3f>(vec3u(1u, 2u, 3u)), 1.0f, 2.0f, 3.0f);
        check_each(static_cast<vec3f>(vec3d(0.5, 1.5, 2.5)), 0.5f, 1.5f, 2.5f);

        // vec4
        check_each(static_cast<vec3f>(vec4b(true, false, true, false)), 1.0f, 0.0f, 1.0f);
        check_each(static_cast<vec3f>(vec4i(0, 1, 2, 3)), 0.0f, 1.0f, 2.0f);
        check_each(static_cast<vec3f>(vec4u(0u, 1u, 2u, 3u)), 0.0f, 1.0f, 2.0f);
        check_each(static_cast<vec3f>(vec4f(0.0f, 1.0f, 2.0f, 3.0f)), 0.0f, 1.0f, 2.0f);
        check_each(static_cast<vec3f>(vec4d(0.0, 1.0, 2.0, 3.0)), 0.0f, 1.0f, 2.0f);
    }

    VX_SECTION("assignment")
    {
        {
            vec3f v1(0.0f, 1.0f, 2.0f);
            vec3b v2(true, false, true);

            v1 = v2;
            VX_CHECK(v1.x == 1.0f);
            VX_CHECK(v1.y == 0.0f);
            VX_CHECK(v1.z == 1.0f);
        }

        {
            vec3f v1(1.0f, 2.0f, 3.0f);
            vec3i v2(3, 2, 1);

            v1 = v2;
            VX_CHECK(v1.x == 3.0f);
            VX_CHECK(v1.y == 2.0f);
            VX_CHECK(v1.z == 1.0f);
        }

        {
            vec3f v1(1.0f, 2.0f, 3.0f);
            vec3u v2(3u, 2u, 1u);

            v1 = v2;
            VX_CHECK(v1.x == 3.0f);
            VX_CHECK(v1.y == 2.0f);
            VX_CHECK(v1.z == 1.0f);
        }

        {
            vec3f v1(1.0f, 2.0f, 3.0f);
            vec3f v2(3.0f, 2.0f, 1.0f);

            v1 = v2;
            VX_CHECK(v1.x == 3.0f);
            VX_CHECK(v1.y == 2.0f);
            VX_CHECK(v1.z == 1.0f);
        }

        {
            vec3f v1(1.0f, 2.0f, 3.0f);
            vec3d v2(3.0, 2.0, 1.0);

            v1 = v2;
            VX_CHECK(v1.x == 3.0f);
            VX_CHECK(v1.y == 2.0f);
            VX_CHECK(v1.z == 1.0f);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec3f(1.5f, 2.75f, -1.23f)[0] == 1.5f);
        VX_STATIC_CHECK(vec3f(1.5f, 2.75f, -1.23f)[1] == 2.75f);
        VX_STATIC_CHECK(vec3f(1.5f, 2.75f, -1.23f)[2] == -1.23f);

        vec3f v;

        v[0] = 1.5f;
        VX_CHECK(v[0] == 1.5f);
        v[1] = 2.75f;
        VX_CHECK(v[1] == 2.75f);
        v[2] = -1.23f;
        VX_CHECK(v[2] == -1.23f);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec3f(1.5f, -2.75f, -1.23f) == vec3f(1.5f, -2.75f, -1.23f));
        VX_STATIC_CHECK(vec3f(1.5f, 2.75f, -1.23f) != vec3f(-1.5f, -2.75f, -1.23f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec3f(1.0f, -1.5f, 2.0f) == vec3f(1.0f, -1.5f, 2.0f));
        VX_STATIC_CHECK(+vec3f(-1.5f, 2.5f, -3.0f) == vec3f(-1.5f, 2.5f, -3.0f));

        VX_STATIC_CHECK(-vec3f(1.5f, -2.5f, 3.0f) == vec3f(-1.5f, 2.5f, -3.0f));
        VX_STATIC_CHECK(-vec3f(-1.5f, 2.5f, -3.0f) == vec3f(1.5f, -2.5f, 3.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec3f(0.5f, -1.0f, 2.0f) == vec3f(1.5f, 0.0f, 3.0f));
        VX_STATIC_CHECK(--vec3f(0.5f, -1.0f, 2.0f) == vec3f(-0.5f, -2.0f, 1.0f));

        vec3f v(0.5f, -1.0f, 2.0f);

        VX_CHECK(v++ == vec3f(0.5f, -1.0f, 2.0f));
        VX_CHECK(v == vec3f(1.5f, 0.0f, 3.0f));
        VX_CHECK(v-- == vec3f(1.5f, 0.0f, 3.0f));
        VX_CHECK(v == vec3f(0.5f, -1.0f, 2.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec3f(1.5f, -2.75f, 3.0f) + 5.0f == vec3f(6.5f, 2.25f, 8.0f));
        VX_STATIC_CHECK(5.0f + vec3f(1.5f, -2.75f, 3.0f) == vec3f(6.5f, 2.25f, 8.0f));
        VX_STATIC_CHECK(vec3f(5.5f) + vec3f(1.5f, -2.75f, 3.0f) == vec3f(7.0f, 2.75f, 8.5f));
        VX_STATIC_CHECK(vec3f(1.5f, -2.75f, 3.0f) + vec3f(5.0f) == vec3f(6.5f, 2.25f, 8.0f));

        // subtraction
        VX_STATIC_CHECK(vec3f(1.5f, -2.75f, 3.0f) - 5.0f == vec3f(-3.5f, -7.75f, -2.0f));
        VX_STATIC_CHECK(5.0f - vec3f(1.5f, -2.75f, 3.0f) == vec3f(3.5f, 7.75f, 2.0f));
        VX_STATIC_CHECK(vec3f(5.5f) - vec3f(1.5f, -2.75f, 3.0f) == vec3f(4.0f, 8.25f, 2.5f));
        VX_STATIC_CHECK(vec3f(1.5f, -2.75f, 3.0f) - vec3f(5.0f) == vec3f(-3.5f, -7.75f, -2.0f));

        // multiplication
        VX_STATIC_CHECK(vec3f(1.5f, -2.75f, 3.0f) * 5.0f == vec3f(7.5f, -13.75f, 15.0f));
        VX_STATIC_CHECK(5.0f * vec3f(1.5f, -2.75f, 3.0f) == vec3f(7.5f, -13.75f, 15.0f));
        VX_STATIC_CHECK(vec3f(5.5f) * vec3f(1.5f, -2.75f, 3.0f) == vec3f(8.25f, -15.125f, 16.5f));
        VX_STATIC_CHECK(vec3f(1.5f, -2.75f, 3.0f) * vec3f(5.0f) == vec3f(7.5f, -13.75f, 15.0f));

        // division
        VX_STATIC_CHECK(vec3f(5.5f, -13.75f, 15.0f) / 5.0f == vec3f(1.1f, -2.75f, 3.0f));
        VX_STATIC_CHECK(10.0f / vec3f(10.0f, 5.0f, 2.0f) == vec3f(1.0f, 2.0f, 5.0f));
        VX_STATIC_CHECK(vec3f(10.5f) / vec3f(10.0f, 5.0f, 2.0f) == vec3f(1.05f, 2.1f, 5.25f));
        VX_STATIC_CHECK(vec3f(10.0f, 5.0f, 2.0f) / vec3f(5.0f, 10.0f, 2.0f) == vec3f(2.0f, 0.5f, 1.0f));

    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec3f v;
            v += 5.5f;
            VX_CHECK(v == vec3f(5.5f, 5.5f, 5.5f));
            v += vec3f(1.5f, -2.75f, 3.0f);
            VX_CHECK(v == vec3f(7.0f, 2.75f, 8.5f));
        }

        // subtraction
        {
            vec3f v(10.5f, 20.75f, 30.0f);
            v -= 5.0f;
            VX_CHECK(v == vec3f(5.5f, 15.75f, 25.0f));
            v -= vec3f(1.5f, -2.75f, 3.0f);
            VX_CHECK(v == vec3f(4.0f, 18.5f, 22.0f));
        }

        // multiplication
        {
            vec3f v(1.5f, -2.75f, 3.0f);
            v *= 5.0f;
            VX_CHECK(v == vec3f(7.5f, -13.75f, 15.0f));
            v *= vec3f(2.0f, -3.0f, 4.0f);
            VX_CHECK(v == vec3f(15.0f, 41.25f, 60.0f));
        }

        // division
        {
            vec3f v(100.5f, 50.0f, 75.0f);
            v /= 5.0f;
            VX_CHECK(v == vec3f(20.1f, 10.0f, 15.0f));
            v /= vec3f(2.0f, 5.0f, 3.0f);
            VX_CHECK(v == vec3f(10.05f, 2.0f, 5.0f));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec3f(1.5f, 0.0f, 3.0f) && vec3f(1.5f, 2.75f, 0.0f)) == vec3b(true, false, false));
        VX_STATIC_CHECK((vec3f(0.0f, 0.0f, 0.0f) && vec3f(1.5f, 1.5f, 1.5f)) == vec3b(false, false, false));

        // or
        VX_STATIC_CHECK((vec3f(1.5f, 0.0f, 3.0f) || vec3f(0.0f, 1.5f, 0.0f)) == vec3b(true, true, true));
        VX_STATIC_CHECK((vec3f(0.0f, 0.0f, 0.0f) || vec3f(0.0f, 1.5f, 0.0f)) == vec3b(false, true, false));

        // not
        VX_STATIC_CHECK((!vec3f(1.5f, 0.0f, 3.0f)) == vec3b(false, true, false));
        VX_STATIC_CHECK((!vec3f(0.0f, 0.0f, 0.0f)) == vec3b(true, true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec3f::zero() == vec3f(0.0f, 0.0f, 0.0f));
        VX_STATIC_CHECK(vec3f::one() == vec3f(1.0f, 1.0f, 1.0f));

        VX_STATIC_CHECK(vec3f::right() == vec3f(1.0f, 0.0f, 0.0f));
        VX_STATIC_CHECK(vec3f::left() == vec3f(-1.0f, 0.0f, 0.0f));

        VX_STATIC_CHECK(vec3f::up() == vec3f(0.0f, 1.0f, 0.0f));
        VX_STATIC_CHECK(vec3f::down() == vec3f(0.0f, -1.0f, 0.0f));

#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_RH_BIT)
        VX_STATIC_CHECK(vec3f::forward() == vec3f(0.0f, 0.0f, -1.0f));
        VX_STATIC_CHECK(vec3f::back() == vec3f(0.0f, 0.0f, 1.0f));
#else
        VX_STATIC_CHECK(vec3f::forward() == vec3f(0.0f, 0.0f, 1.0f));
        VX_STATIC_CHECK(vec3f::back() == vec3f(0.0f, 0.0f, -1.0f));
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_vec3d)
{
    VX_SECTION("scalar type")
    {
        VX_STATIC_CHECK((is_same<typename vec3d::scalar_type, f64>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(vec3d) == 24);
        VX_STATIC_CHECK(vec3d::size == 3);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        check_each(vec3d(), 0.0, 0.0, 0.0);
        check_each(vec3d(vec3d(1.0, 2.0, 3.0)), 1.0, 2.0, 3.0);

        // explicit constructors
        check_each(vec3d(1.0), 1.0, 1.0, 1.0);
        check_each(vec3d(1.0, 2.0, 3.0), 1.0, 2.0, 3.0);

        // conversion constructors
        check_each(vec3d(true), 1.0, 1.0, 1.0);
        check_each(vec3d(5), 5.0, 5.0, 5.0);
        check_each(vec3d(5u), 5.0, 5.0, 5.0);
        check_each(vec3d(5.0f), 5.0, 5.0, 5.0);

        // mixed
        check_each(vec3d(true, -5, 100.0), 1.0, -5.0, 100.0);
        check_each(vec3d(vec2b(false, true), 24u), 0.0, 1.0, 24.0);
        check_each(vec3d(1.0f, vec2d(-37.5, 1.23)), 1.0, -37.5, 1.23);

        // vec3
        check_each(static_cast<vec3d>(vec3b(false, true, false)), 0.0, 1.0, 0.0);
        check_each(static_cast<vec3d>(vec3i(1, -2, 3)), 1.0, -2.0, 3.0);
        check_each(static_cast<vec3d>(vec3u(1u, 2u, 3u)), 1.0, 2.0, 3.0);
        check_each(static_cast<vec3d>(vec3f(1.5f, 2.5f, 3.5f)), 1.5, 2.5, 3.5);

        // vec4
        check_each(static_cast<vec3d>(vec4b(true, false, true, false)), 1.0, 0.0, 1.0);
        check_each(static_cast<vec3d>(vec4i(0, 1, 2, 3)), 0.0, 1.0, 2.0);
        check_each(static_cast<vec3d>(vec4u(0u, 1u, 2u, 3u)), 0.0, 1.0, 2.0);
        check_each(static_cast<vec3d>(vec4f(0.0f, 1.0f, 2.0f, 3.0f)), 0.0, 1.0, 2.0);
        check_each(static_cast<vec3d>(vec4d(0.0, 1.0, 2.0, 3.0)), 0.0, 1.0, 2.0);
    }

    VX_SECTION("assignment")
    {
        {
            vec3d v1(0.0, 1.0, 2.0);
            vec3b v2(true, false, true);

            v1 = v2;
            VX_CHECK(v1.x == 1.0);
            VX_CHECK(v1.y == 0.0);
            VX_CHECK(v1.z == 1.0);
        }

        {
            vec3d v1(0.0, 1.0, 2.0);
            vec3i v2(3, 2, 1);

            v1 = v2;
            VX_CHECK(v1.x == 3.0);
            VX_CHECK(v1.y == 2.0);
            VX_CHECK(v1.z == 1.0);
        }

        {
            vec3d v1(0.0, 1.0, 2.0);
            vec3u v2(3u, 2u, 1u);

            v1 = v2;
            VX_CHECK(v1.x == 3.0);
            VX_CHECK(v1.y == 2.0);
            VX_CHECK(v1.z == 1.0);
        }

        {
            vec3d v1(0.0, 1.0, 2.0);
            vec3d v2(3.0f, 2.0f, 1.0f);

            v1 = v2;
            VX_CHECK(v1.x == 3.0);
            VX_CHECK(v1.y == 2.0);
            VX_CHECK(v1.z == 1.0);
        }

        {
            vec3d v1(0.0, 1.0, 2.0);
            vec3d v2(3.0, 2.0, 1.0);

            v1 = v2;
            VX_CHECK(v1.x == 3.0);
            VX_CHECK(v1.y == 2.0);
            VX_CHECK(v1.z == 1.0);
        }
    }

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(vec3d(1.5, 2.75, -1.23)[0] == 1.5);
        VX_STATIC_CHECK(vec3d(1.5, 2.75, -1.23)[1] == 2.75);
        VX_STATIC_CHECK(vec3d(1.5, 2.75, -1.23)[2] == -1.23);

        vec3d v;

        v[0] = 1.5;
        VX_CHECK(v[0] == 1.5);
        v[1] = 2.75;
        VX_CHECK(v[1] == 2.75);
        v[2] = -1.23;
        VX_CHECK(v[2] == -1.23);
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(vec3d(1.5, -2.75, -1.23) == vec3d(1.5, -2.75, -1.23));
        VX_STATIC_CHECK(vec3d(1.5, 2.75, -1.23) != vec3d(-1.5, -2.75, -1.23));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+vec3d(1.0, -1.5, 2.0) == vec3d(1.0, -1.5, 2.0));
        VX_STATIC_CHECK(+vec3d(-1.5, 2.5, -3.0) == vec3d(-1.5, 2.5, -3.0));

        VX_STATIC_CHECK(-vec3d(1.5, -2.5, 3.0) == vec3d(-1.5, 2.5, -3.0));
        VX_STATIC_CHECK(-vec3d(-1.5, 2.5, -3.0) == vec3d(1.5, -2.5, 3.0));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++vec3d(0.5, -1.0, 2.0) == vec3d(1.5, 0.0, 3.0));
        VX_STATIC_CHECK(--vec3d(0.5, -1.0, 2.0) == vec3d(-0.5, -2.0, 1.0));

        vec3d v(0.5, -1.0, 2.0);

        VX_CHECK(v++ == vec3d(0.5, -1.0, 2.0));
        VX_CHECK(v == vec3d(1.5, 0.0, 3.0));
        VX_CHECK(v-- == vec3d(1.5, 0.0, 3.0));
        VX_CHECK(v == vec3d(0.5, -1.0, 2.0));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        VX_STATIC_CHECK(vec3d(1.5, -2.75, 3.0) + 5.0 == vec3d(6.5, 2.25, 8.0));
        VX_STATIC_CHECK(5.0 + vec3d(1.5, -2.75, 3.0) == vec3d(6.5, 2.25, 8.0));
        VX_STATIC_CHECK(vec3d(5.5) + vec3d(1.5, -2.75, 3.0) == vec3d(7.0, 2.75, 8.5));
        VX_STATIC_CHECK(vec3d(1.5, -2.75, 3.0) + vec3d(5.0) == vec3d(6.5, 2.25, 8.0));

        // subtraction
        VX_STATIC_CHECK(vec3d(1.5, -2.75, 3.0) - 5.0 == vec3d(-3.5, -7.75, -2.0));
        VX_STATIC_CHECK(5.0 - vec3d(1.5, -2.75, 3.0) == vec3d(3.5, 7.75, 2.0));
        VX_STATIC_CHECK(vec3d(5.5) - vec3d(1.5, -2.75, 3.0) == vec3d(4.0, 8.25, 2.5));
        VX_STATIC_CHECK(vec3d(1.5, -2.75, 3.0) - vec3d(5.0) == vec3d(-3.5, -7.75, -2.0));

        // multiplication
        VX_STATIC_CHECK(vec3d(1.5, -2.75, 3.0) * 5.0 == vec3d(7.5, -13.75, 15.0));
        VX_STATIC_CHECK(5.0 * vec3d(1.5, -2.75, 3.0) == vec3d(7.5, -13.75, 15.0));
        VX_STATIC_CHECK(vec3d(5.5) * vec3d(1.5, -2.75, 3.0) == vec3d(8.25, -15.125, 16.5));
        VX_STATIC_CHECK(vec3d(1.5, -2.75, 3.0) * vec3d(5.0) == vec3d(7.5, -13.75, 15.0));

        // division
        VX_STATIC_CHECK(vec3d(5.5, -13.75, 15.0) / 5.0 == vec3d(1.1, -2.75, 3.0));
        VX_STATIC_CHECK(10.0 / vec3d(10.0, 5.0, 2.0) == vec3d(1.0, 2.0, 5.0));
        VX_STATIC_CHECK(vec3d(10.5) / vec3d(10.0, 5.0, 2.0) == vec3d(1.05, 2.1, 5.25));
        VX_STATIC_CHECK(vec3d(10.0, 5.0, 2.0) / vec3d(5.0, 10.0, 2.0) == vec3d(2.0, 0.5, 1.0));
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            vec3d v;
            v += 5.5;
            VX_CHECK(v == vec3d(5.5, 5.5, 5.5));
            v += vec3d(1.5, -2.75, 3.0);
            VX_CHECK(v == vec3d(7.0, 2.75, 8.5));
        }

        // subtraction
        {
            vec3d v(10.5, 20.75, 30.0);
            v -= 5.0;
            VX_CHECK(v == vec3d(5.5, 15.75, 25.0));
            v -= vec3d(1.5, -2.75, 3.0);
            VX_CHECK(v == vec3d(4.0, 18.5, 22.0));
        }

        // multiplication
        {
            vec3d v(1.5, -2.75, 3.0);
            v *= 5.0;
            VX_CHECK(v == vec3d(7.5, -13.75, 15.0));
            v *= vec3d(2.0, -3.0, 4.0);
            VX_CHECK(v == vec3d(15.0, 41.25, 60.0));
        }

        // division
        {
            vec3d v(100.5, 50.0, 75.0);
            v /= 5.0;
            VX_CHECK(v == vec3d(20.1, 10.0, 15.0));
            v /= vec3d(2.0, 5.0, 3.0);
            VX_CHECK(v == vec3d(10.05, 2.0, 5.0));
        }
    }

    VX_SECTION("boolean operators")
    {
        // and
        VX_STATIC_CHECK((vec3d(1.5, 0.0, 3.0) && vec3d(1.5, 2.75, 0.0)) == vec3b(true, false, false));
        VX_STATIC_CHECK((vec3d(0.0, 0.0, 0.0) && vec3d(1.5, 1.5, 1.5)) == vec3b(false, false, false));

        // or
        VX_STATIC_CHECK((vec3d(1.5, 0.0, 3.0) || vec3d(0.0, 1.5, 0.0)) == vec3b(true, true, true));
        VX_STATIC_CHECK((vec3d(0.0, 0.0, 0.0) || vec3d(0.0, 1.5, 0.0)) == vec3b(false, true, false));

        // not
        VX_STATIC_CHECK((!vec3d(1.5, 0.0, 3.0)) == vec3b(false, true, false));
        VX_STATIC_CHECK((!vec3d(0.0, 0.0, 0.0)) == vec3b(true, true, true));
    }

    VX_SECTION("static constructors")
    {
        VX_STATIC_CHECK(vec3d::zero() == vec3d(0.0, 0.0, 0.0));
        VX_STATIC_CHECK(vec3d::one() == vec3d(1.0, 1.0, 1.0));

        VX_STATIC_CHECK(vec3d::right() == vec3d(1.0, 0.0, 0.0));
        VX_STATIC_CHECK(vec3d::left() == vec3d(-1.0, 0.0, 0.0));

        VX_STATIC_CHECK(vec3d::up() == vec3d(0.0, 1.0, 0.0));
        VX_STATIC_CHECK(vec3d::down() == vec3d(0.0, -1.0, 0.0));

#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_RH_BIT)
        VX_STATIC_CHECK(vec3d::forward() == vec3d(0.0, 0.0, -1.0));
        VX_STATIC_CHECK(vec3d::back() == vec3d(0.0, 0.0, 1.0));
#else
        VX_STATIC_CHECK(vec3d::forward() == vec3d(0.0, 0.0, 1.0));
        VX_STATIC_CHECK(vec3d::back() == vec3d(0.0, 0.0, -1.0));
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}