#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat3x4)
{
    VX_SECTION("types")
    {
        VX_STATIC_CHECK((is_same<typename mat3x4::scalar_type, f32>::value));
        VX_STATIC_CHECK((is_same<typename mat3x4::row_type, vec3>::value));
        VX_STATIC_CHECK((is_same<typename mat3x4::col_type, vec4>::value));
        VX_STATIC_CHECK((is_same<typename mat3x4::transpose_type, mat4x3>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(mat3x4::width == 3);
        VX_STATIC_CHECK(mat3x4::height == 4);

        VX_STATIC_CHECK(sizeof(mat3x4) == 48);
        VX_STATIC_CHECK(mat3x4::size == 12);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        {
            {
                constexpr mat3x4 m;
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(0.0f, 0.0f, 0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(0.0f, 0.0f, 0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec4(0.0f, 0.0f, 0.0f, 0.0f));
            }

            {
                constexpr mat3x4 m(mat3x4(2.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(2.0f, 2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(2.0f, 2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec4(2.0f, 2.0f, 2.0f, 2.0f));
            }
        }

        // explicit constructors
        {
            constexpr vec4 c1(1.0f, 2.0f, 3.0f, 4.0f);
            constexpr vec4 c2(5.0f, 6.0f, 7.0f, 8.0f);
            constexpr vec4 c3(9.0f, 10.0f, 11.0f, 12.0f);

            {
                constexpr mat3x4 m(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec4(9.0f, 10.0f, 11.0f, 12.0f));
            }

            {
                constexpr mat3x4 m(c1, c2, c3);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
                VX_STATIC_CHECK_EQ(m.columns[2], c3);
            }
        }

        // conversion constructors
        {
            {
                constexpr mat3x4 m(mat3x4(2u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(2.0f, 2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(2.0f, 2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec4(2.0f, 2.0f, 2.0f, 2.0f));
            }

            {
                constexpr mat3x4 m(true, 2u, 3.0, 4, 5.0f, 6u, 7, 8, 9, 10, 11, 12);
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec4(9.0f, 10.0f, 11.0f, 12.0f));
            }

            {
                constexpr mat3x4 m(vec4i(1, 2, 3, 4), vec4u(5u, 6u, 7u, 8u), vec4(9.0f, 10.0f, 11.0f, 12.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec4(9.0f, 10.0f, 11.0f, 12.0f));
            }
        }

        // from larger
        {
            {
                constexpr mat4x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
                constexpr mat3x4 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec4(9.0f, 10.0f, 11.0f, 12.0f));
            }
        }
    }

    VX_SECTION("assignment")
    {
        {
            mat3x4 m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
            mat3x4 m2(13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec4(13.0f, 14.0f, 15.0f, 16.0f));
            VX_CHECK_EQ(m1.columns[1], vec4(17.0f, 18.0f, 19.0f, 20.0f));
            VX_CHECK_EQ(m1.columns[2], vec4(21.0f, 22.0f, 23.0f, 24.0f));
        }

        {
            mat3x4f m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
            mat3x4i m2(13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec4(13.0f, 14.0f, 15.0f, 16.0f));
            VX_CHECK_EQ(m1.columns[1], vec4(17.0f, 18.0f, 19.0f, 20.0f));
            VX_CHECK_EQ(m1.columns[2], vec4(21.0f, 22.0f, 23.0f, 24.0f));
        }
    }

    constexpr mat3x4 m34(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(m34[0] == vec4(1.0f, 2.0f, 3.0f, 4.0f));
        VX_STATIC_CHECK(m34[1] == vec4(5.0f, 6.0f, 7.0f, 8.0f));
        VX_STATIC_CHECK(m34[2] == vec4(9.0f, 10.0f, 11.0f, 12.0f));

        mat3x4 m;

        m[0] = vec4(1.0f, 2.0f, 3.0f, 4.0f);
        VX_CHECK_EQ(m[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
        m[1] = vec4(5.0f, 6.0f, 7.0f, 8.0f);
        VX_CHECK_EQ(m[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
        m[2] = vec4(9.0f, 10.0f, 11.0f, 12.0f);
        VX_CHECK_EQ(m[2], vec4(9.0f, 10.0f, 11.0f, 12.0f));
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(m34 == m34);
        VX_STATIC_CHECK(m34 != mat3x4(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f, -10.0f, 11.0f, -12.0f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(
            +mat3x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f) ==
            mat3x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f)
        );

        VX_STATIC_CHECK(
            +mat3x4(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f, -10.0f, 11.0f, -12.0f) ==
            mat3x4(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f, -10.0f, 11.0f, -12.0f)
        );

        VX_STATIC_CHECK(
            -mat3x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f) ==
            mat3x4(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f, -10.0f, -11.0f, -12.0f)
        );

        VX_STATIC_CHECK(
            -mat3x4(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f, -10.0f, -11.0f, -12.0f) ==
            mat3x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f)
        );
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(
            ++mat3x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f) ==
            mat3x4(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f)
        );

        VX_STATIC_CHECK(
            --mat3x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f) ==
            mat3x4(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f)
        );

        mat3x4 m = mat3x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);

        VX_CHECK(m++ == m34);
        VX_CHECK(m == mat3x4(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
        VX_CHECK(m-- == mat3x4(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
        VX_CHECK(m == m34);
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        {
            VX_STATIC_CHECK(m34 + 5.0f == mat3x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));
            VX_STATIC_CHECK(5.0f + m34 == mat3x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));
            VX_STATIC_CHECK(m34 + mat3x4(5.0f) == mat3x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));
            VX_STATIC_CHECK(mat3x4(5.0f) + m34 == mat3x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));
        }

        // subtraction
        {
            VX_STATIC_CHECK(mat3x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f) - 5.0f == m34);
            VX_STATIC_CHECK(5.0f - mat3x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f) == -m34);
            VX_STATIC_CHECK(mat3x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f) - mat3x4(5.0f) == m34);
            VX_STATIC_CHECK(mat3x4(5.0f) - mat3x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f) == -m34);
        }

        // multiplication
        {
            VX_STATIC_CHECK(m34 * 5.0f == mat3x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f));
            VX_STATIC_CHECK(5.0f * m34 == mat3x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f));

            // 3x4 * 2x3
            constexpr mat2x3 m23(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            VX_STATIC_CHECK((m34 * m23) == mat2x4(38.0f, 44.0f, 50.0f, 56.0f, 83.0f, 98.0f, 113.0f, 128.0f));

            // 3x4 * 3x3
            constexpr mat3x3 m33(1.0f, 2.0f, 3.0f, 4.0f, 20.0f, 6.0f, 7.0f, 8.0f, 9.0f);
            VX_STATIC_CHECK((m34 * m33) == mat3x4(38.0f, 44.0f, 50.0f, 56.0f, 158.0f, 188.0f, 218.0f, 248.0f, 128.0f, 152.0f, 176.0f, 200.0f));

            // 3x4 * 4x3
            constexpr mat4x3 m43(1.0f, 2.0f, 3.0f, 4.0f, 20.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
            VX_STATIC_CHECK((m34 * m43) == mat4x4(38.0f, 44.0f, 50.0f, 56.0f, 158.0f, 188.0f, 218.0f, 248.0f, 128.0f, 152.0f, 176.0f, 200.0f, 173.0f, 206.0f, 239.0f, 272.0f));
        }

        // division
        {
            VX_STATIC_CHECK(mat3x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f) / 5.0f == m34);
            VX_STATIC_CHECK(27720.0f / mat3x4(27720.0f, 13860.0f, 9240.0f, 6930.0f, 5544.0f, 4620.0f, 3960.0f, 3465.0f, 3080.0f, 2772.0f, 2520.0f, 2310.0f) == m34);
        }
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            mat3x4 m = m34;
            m += 5.0f;
            VX_CHECK(m == mat3x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));

            m += m34;
            VX_CHECK(m == mat3x4(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f, 23.0f, 25.0f, 27.0f, 29.0f));
        }

        // subtraction
        {
            mat3x4 m(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f, 23.0f, 25.0f, 27.0f, 29.0f);
            m -= m34;
            VX_CHECK(m == mat3x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));

            m -= 5.0f;
            VX_CHECK(m == m34);
        }

        // multiplication
        {
            mat3x4 m = m34;
            m *= 5.0f;
            VX_CHECK(m == mat3x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f));

            constexpr mat3x3 m33(1.0f, 2.0f, 3.0f, 4.0f, 20.0f, 6.0f, 7.0f, 8.0f, 9.0f);
            m *= m33;
            VX_CHECK(m == mat3x4(190.0f, 220.0f, 250.0f, 280.0f, 790.0f, 940.0f, 1090.0f, 1240.0f, 640.0f, 760.0f, 880.0f, 1000.0f));
        }

        // division
        {
            mat3x4 m(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f);
            m /= 5.0f;
            VX_CHECK(m == m34);
        }
    }


    VX_SECTION("static constructors")
    {
        // zero
        {
            constexpr mat3x4 i = mat3x4::zero();
            VX_STATIC_CHECK(i.columns[0] == vec4(0.0f, 0.0f, 0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[1] == vec4(0.0f, 0.0f, 0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[2] == vec4(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}