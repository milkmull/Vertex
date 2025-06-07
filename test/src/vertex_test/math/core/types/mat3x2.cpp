#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat3x2)
{
    VX_SECTION("types")
    {
        VX_STATIC_CHECK((is_same<typename mat3x2::scalar_type, f32>::value));
        VX_STATIC_CHECK((is_same<typename mat3x2::row_type, vec3>::value));
        VX_STATIC_CHECK((is_same<typename mat3x2::col_type, vec2>::value));
        VX_STATIC_CHECK((is_same<typename mat3x2::transpose_type, mat2x3>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(mat3x2::width == 3);
        VX_STATIC_CHECK(mat3x2::height == 2);

        VX_STATIC_CHECK(sizeof(mat3x2) == 24);
        VX_STATIC_CHECK(mat3x2::size == 6);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        {
            {
                constexpr mat3x2 m;
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(0.0f, 0.0f));
            }

            {
                constexpr mat3x2 m(mat3x2(2.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(2.0f, 2.0f));
            }
        }

        // explicit constructors
        {
            constexpr vec2 c1(1.0f, 2.0f);
            constexpr vec2 c2(3.0f, 4.0f);
            constexpr vec2 c3(5.0f, 6.0f);

            {
                constexpr mat3x2 m(c1[0], c1[1], c2[0], c2[1], c3[0], c3[1]);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(5.0f, 6.0f));
            }

            {
                constexpr mat3x2 m(c1, c2, c3);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
                VX_STATIC_CHECK_EQ(m.columns[2], c3);
            }
        }

        // conversion constructors
        {
            {
                constexpr mat3x2 m(mat3x2(2u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(2.0f, 2.0f));
            }

            {
                constexpr mat3x2 m(true, 2u, 3.0, 4, 5.0f, 6u);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(5.0f, 6.0f));
            }

            {
                constexpr mat3x2 m(vec2i(1, 2), vec2u(3u, 4u), vec2(5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(5.0f, 6.0f));
            }
        }

        // from larger
        {
            {
                constexpr mat3x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                constexpr mat3x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(9.0f, 10.0f));
            }

            {
                constexpr mat4x2 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
                constexpr mat3x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(5.0f, 6.0f));
            }

            {
                constexpr mat4x3 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                constexpr mat3x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(4.0f, 5.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(7.0f, 8.0f));
            }

            {
                constexpr mat4x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
                constexpr mat3x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(9.0f, 10.0f));
            }
        }
    }

    VX_SECTION("assignment")
    {
        {
            mat3x2 m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            mat3x2 m2(7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec2(7.0f, 8.0f));
            VX_CHECK_EQ(m1.columns[1], vec2(9.0f, 10.0f));
            VX_CHECK_EQ(m1.columns[2], vec2(11.0f, 12.0f));
        }

        {
            mat3x2f m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            mat3x2i m2(7, 8, 9, 10, 11, 12);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec2(7.0f, 8.0f));
            VX_CHECK_EQ(m1.columns[1], vec2(9.0f, 10.0f));
            VX_CHECK_EQ(m1.columns[2], vec2(11.0f, 12.0f));
        }
    }

    constexpr mat3x2 m32(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(m32[0] == vec2(1.0f, 2.0f));
        VX_STATIC_CHECK(m32[1] == vec2(3.0f, 4.0f));
        VX_STATIC_CHECK(m32[2] == vec2(5.0f, 6.0f));

        mat3x2 m;

        m[0] = vec2(1.0f, 2.0f);
        VX_CHECK_EQ(m[0], vec2(1.0f, 2.0f));
        m[1] = vec2(3.0f, 4.0f);
        VX_CHECK_EQ(m[1], vec2(3.0f, 4.0f));
        m[2] = vec2(5.0f, 6.0f);
        VX_CHECK_EQ(m[2], vec2(5.0f, 6.0f));
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(m32 == m32);
        VX_STATIC_CHECK(m32 != mat3x2(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+mat3x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f) == mat3x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f));
        VX_STATIC_CHECK(+mat3x2(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f) == mat3x2(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f));

        VX_STATIC_CHECK(-mat3x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f) == mat3x2(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f));
        VX_STATIC_CHECK(-mat3x2(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f) == mat3x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++mat3x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f) == mat3x2(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f));
        VX_STATIC_CHECK(--mat3x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f) == mat3x2(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f));

        mat3x2 m = m32;

        VX_CHECK(m++ == mat3x2(1.0f, 2.0f, 3.0f, 4.0f, 0.0f, 0.0f));
        VX_CHECK(m == mat3x2(2.0f, 3.0f, 4.0f, 5.0f, 1.0f, 1.0f));
        VX_CHECK(m-- == mat3x2(2.0f, 3.0f, 4.0f, 5.0f, 1.0f, 1.0f));
        VX_CHECK(m == mat3x2(1.0f, 2.0f, 3.0f, 4.0f, 0.0f, 0.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        {
            VX_STATIC_CHECK(m32 + 5.0f == mat3x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));
            VX_STATIC_CHECK(5.0f + m32 == mat3x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));
            VX_STATIC_CHECK(m32 + mat3x2(5.0f) == mat3x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));
            VX_STATIC_CHECK(mat3x2(5.0f) + m32 == mat3x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));
        }

        // subtraction
        {
            VX_STATIC_CHECK(mat3x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f) - 5.0f == m32);
            VX_STATIC_CHECK(5.0f - mat3x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f) == -m32);
            VX_STATIC_CHECK(mat3x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f) - mat3x2(5.0f) == m32);
            VX_STATIC_CHECK(mat3x2(5.0f) - mat3x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f) == -m32);
        }

        // multiplication
        {
            VX_STATIC_CHECK(m32 * 5.0f == mat3x2(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f));
            VX_STATIC_CHECK(5.0f * m32 == mat3x2(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f));

            // 3x2 * 2x3
            constexpr mat2x3 m23(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            VX_STATIC_CHECK((m32 * m23) == mat2x2(22.0f, 28.0f, 49.0f, 64.0f));

            // 3x2 * 3x3
            constexpr mat3x3 m33(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
            VX_STATIC_CHECK((m32 * m33) == mat3x2(22.0f, 28.0f, 49.0f, 64.0f, 76.0f, 100.0f));

            // 3x2 * 4x3
            constexpr mat4x3 m43(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
            VX_STATIC_CHECK((m32 * m43) == mat4x2(22.0f, 28.0f, 49.0f, 64.0f, 76.0f, 100.0f, 103.0f, 136.0f));
        }

        // division
        {
            VX_STATIC_CHECK(mat3x2(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f) / 5.0f == m32);
            VX_STATIC_CHECK(60.0f / mat3x2(60.0f, 30.0f, 20.0f, 15.0f, 12.0f, 10.0f) == m32);
        }
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            mat3x2 m = m32;
            m += 5.0f;
            VX_CHECK(m == mat3x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));

            m += m32;
            VX_CHECK(m == mat3x2(7.0f, 9.0f, 10.0f, 12.0f, 13.0f, 15.0f));
        }

        // subtraction
        {
            mat3x2 m(7.0f, 9.0f, 10.0f, 12.0f, 13.0f, 15.0f);
            m -= m32;
            VX_CHECK(m == mat3x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));

            m -= 5.0f;
            VX_CHECK(m == m32);
        }

        // multiplication
        {
            mat3x2 m = m32;
            m *= 5.0f;
            VX_CHECK(m == mat3x2(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f));

            constexpr mat3x3 m33(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
            m *= m33;
            VX_CHECK(m == mat3x2(110.0f, 140.0f, 245.0f, 320.0f, 380.0f, 500.0f));
        }

        // division
        {
            mat3x2 m(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f);
            m /= 5.0f;
            VX_CHECK(m == m32);
        }
    }

    VX_SECTION("static constructors")
    {
        // zero
        {
            constexpr mat3x2 i = mat3x2::zero();
            VX_STATIC_CHECK(i.columns[0] == vec2(0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[1] == vec2(0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[2] == vec2(0.0f, 0.0f));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}