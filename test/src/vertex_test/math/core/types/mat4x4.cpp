#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat4x4)
{
    VX_SECTION("types")
    {
        VX_STATIC_CHECK((is_same<typename mat4x4::scalar_type, f32>::value));
        VX_STATIC_CHECK((is_same<typename mat4x4::row_type, vec4>::value));
        VX_STATIC_CHECK((is_same<typename mat4x4::col_type, vec4>::value));
        VX_STATIC_CHECK((is_same<typename mat4x4::transpose_type, mat4x4>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(mat4x4::width == 4);
        VX_STATIC_CHECK(mat4x4::height == 4);

        VX_STATIC_CHECK(sizeof(mat4x4) == 64);
        VX_STATIC_CHECK(mat4x4::size == 16);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        {
            {
                const mat4x4 m;
                VX_CHECK_EQ(m.columns[0], vec4(1.0f, 0.0f, 0.0f, 0.0f));
                VX_CHECK_EQ(m.columns[1], vec4(0.0f, 1.0f, 0.0f, 0.0f));
                VX_CHECK_EQ(m.columns[2], vec4(0.0f, 0.0f, 1.0f, 0.0f));
                VX_CHECK_EQ(m.columns[3], vec4(0.0f, 0.0f, 0.0f, 1.0f));
            }

            {
                const mat4x4 m(mat4x4(2.0f));
                VX_CHECK_EQ(m.columns[0], vec4(2.0f, 0.0f, 0.0f, 0.0f));
                VX_CHECK_EQ(m.columns[1], vec4(0.0f, 2.0f, 0.0f, 0.0f));
                VX_CHECK_EQ(m.columns[2], vec4(0.0f, 0.0f, 2.0f, 0.0f));
                VX_CHECK_EQ(m.columns[3], vec4(0.0f, 0.0f, 0.0f, 2.0f));
            }
        }

        // explicit constructors
        {
            const vec4 c1(1.0f, 2.0f, 3.0f, 4.0f);
            const vec4 c2(5.0f, 6.0f, 7.0f, 8.0f);
            const vec4 c3(9.0f, 10.0f, 11.0f, 12.0f);
            const vec4 c4(13.0f, 14.0f, 15.0f, 16.0f);

            {
                const mat4x4 m(
                    c1[0], c1[1], c1[2], c1[3],
                    c2[0], c2[1], c2[2], c2[3],
                    c3[0], c3[1], c3[2], c3[3],
                    c4[0], c4[1], c4[2], c4[3]
                );
                VX_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
                VX_CHECK_EQ(m.columns[2], vec4(9.0f, 10.0f, 11.0f, 12.0f));
                VX_CHECK_EQ(m.columns[3], vec4(13.0f, 14.0f, 15.0f, 16.0f));
            }

            {
                const mat4x4 m(c1, c2, c3, c4);
                VX_CHECK_EQ(m.columns[0], c1);
                VX_CHECK_EQ(m.columns[1], c2);
                VX_CHECK_EQ(m.columns[2], c3);
                VX_CHECK_EQ(m.columns[3], c4);
            }
        }

        // conversion constructors
        {
            {
                const mat4x4 m(mat4x4(2u));
                VX_CHECK_EQ(m.columns[0], vec4(2.0f, 0.0f, 0.0f, 0.0f));
                VX_CHECK_EQ(m.columns[1], vec4(0.0f, 2.0f, 0.0f, 0.0f));
                VX_CHECK_EQ(m.columns[2], vec4(0.0f, 0.0f, 2.0f, 0.0f));
                VX_CHECK_EQ(m.columns[3], vec4(0.0f, 0.0f, 0.0f, 2.0f));
            }

            {
                const mat4x4 m(true, 2u, 3.0, 4, 5.0f, 6u, 7, 8.0f, 9u, 10, 11.0f, 12u, 13.0, 14, 15.0f, 16u);
                VX_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
                VX_CHECK_EQ(m.columns[2], vec4(9.0f, 10.0f, 11.0f, 12.0f));
                VX_CHECK_EQ(m.columns[3], vec4(13.0f, 14.0f, 15.0f, 16.0f));
            }

            {
                const mat4x4 m(vec4i(1, 2, 3, 4), vec4u(5u, 6u, 7u, 8u), vec4(9.0f, 10.0f, 11.0f, 12.0f), vec4d(13.0, 14.0, 15.0, 16.0));
                VX_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
                VX_CHECK_EQ(m.columns[2], vec4(9.0f, 10.0f, 11.0f, 12.0f));
                VX_CHECK_EQ(m.columns[3], vec4(13.0f, 14.0f, 15.0f, 16.0f));
            }
        }
    }

    VX_SECTION("assignment")
    {
        {
            mat4x4 m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
            mat4x4 m2(17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f, 30.0f, 31.0f, 32.0f);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec4(17.0f, 18.0f, 19.0f, 20.0f));
            VX_CHECK_EQ(m1.columns[1], vec4(21.0f, 22.0f, 23.0f, 24.0f));
            VX_CHECK_EQ(m1.columns[2], vec4(25.0f, 26.0f, 27.0f, 28.0f));
            VX_CHECK_EQ(m1.columns[3], vec4(29.0f, 30.0f, 31.0f, 32.0f));
        }

        {
            mat4x4f m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
            mat4x4i m2(17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec4(17.0f, 18.0f, 19.0f, 20.0f));
            VX_CHECK_EQ(m1.columns[1], vec4(21.0f, 22.0f, 23.0f, 24.0f));
            VX_CHECK_EQ(m1.columns[2], vec4(25.0f, 26.0f, 27.0f, 28.0f));
            VX_CHECK_EQ(m1.columns[3], vec4(29.0f, 30.0f, 31.0f, 32.0f));
        }
    }

    const mat4x4 m44(
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    );

    VX_SECTION("index")
    {
        VX_CHECK(m44[0] == vec4(1.0f, 2.0f, 3.0f, 4.0f));
        VX_CHECK(m44[1] == vec4(5.0f, 6.0f, 7.0f, 8.0f));
        VX_CHECK(m44[2] == vec4(9.0f, 10.0f, 11.0f, 12.0f));
        VX_CHECK(m44[3] == vec4(13.0f, 14.0f, 15.0f, 16.0f));

        mat4x4 m;

        m[0] = vec4(1.0f, 2.0f, 3.0f, 4.0f);
        VX_CHECK_EQ(m[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
        m[1] = vec4(5.0f, 6.0f, 7.0f, 8.0f);
        VX_CHECK_EQ(m[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
        m[2] = vec4(9.0f, 10.0f, 11.0f, 12.0f);
        VX_CHECK_EQ(m[2], vec4(9.0f, 10.0f, 11.0f, 12.0f));
        m[3] = vec4(13.0f, 14.0f, 15.0f, 16.0f);
        VX_CHECK_EQ(m[3], vec4(13.0f, 14.0f, 15.0f, 16.0f));
    }

    VX_SECTION("comparison")
    {
        VX_CHECK(m44 == m44);
        VX_CHECK(m44 != mat4x4(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f, -10.0f, 11.0f, -12.0f, 13.0f, -14.0f, 15.0f, -16.0f));
    }

    VX_SECTION("unary const operators")
    {
        VX_CHECK(+mat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f) ==
            mat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f));

        VX_CHECK(+mat4x4(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f, -10.0f, 11.0f, -12.0f, 13.0f, -14.0f, 15.0f, -16.0f) ==
            mat4x4(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f, -10.0f, 11.0f, -12.0f, 13.0f, -14.0f, 15.0f, -16.0f));

        VX_CHECK(-mat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f) ==
            mat4x4(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f, -10.0f, -11.0f, -12.0f, -13.0f, -14.0f, -15.0f, -16.0f));

        VX_CHECK(-mat4x4(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f, -10.0f, -11.0f, -12.0f, -13.0f, -14.0f, -15.0f, -16.0f) ==
            mat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_CHECK(++mat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f) ==
            mat4x4(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));

        VX_CHECK(--mat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f) ==
            mat4x4(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));

        mat4x4 m = mat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

        VX_CHECK(m++ == mat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f));
        VX_CHECK(m == mat4x4(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));
        VX_CHECK(m-- == mat4x4(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));
        VX_CHECK(m == mat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        {
            VX_CHECK(m44 + 5.0f == mat4x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f));
            VX_CHECK(5.0f + m44 == mat4x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f));
            VX_CHECK(m44 + mat4x4(5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f) == mat4x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f));
            VX_CHECK(mat4x4(5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f) + m44 == mat4x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f));
        }

        // subtraction
        {
            VX_CHECK(mat4x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f) - 5.0f == m44);
            VX_CHECK(5.0f - mat4x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f) == -m44);
            VX_CHECK(mat4x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f) - mat4x4(5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f) == m44);
            VX_CHECK(mat4x4(5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f) - mat4x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f) == -m44);
        }

        // multiplication
        {
            VX_CHECK(m44 * 5.0f == mat4x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f, 65.0f, 70.0f, 75.0f, 80.0f));
            VX_CHECK(5.0f * m44 == mat4x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f, 65.0f, 70.0f, 75.0f, 80.0f));

            // 4x1 * 4x4
            const vec4 m41(1.0f, 2.0f, 3.0f, 4.0f);
            VX_CHECK((m41 * m44) == vec4(30.0f, 70.0f, 110.0f, 150.0f));

            // 4x4 * 1x4
            const vec4 m14(1.0f, 2.0f, 3.0f, 4.0f);
            VX_CHECK((m44 * m14) == vec4(90.0f, 100.0f, 110.0f, 120.0f));

            // 4x4 * 2x4
            const mat2x4 m24(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
            VX_CHECK((m44 * m24) == mat2x4(90.0f, 100.0f, 110.0f, 120.0f, 202.0f, 228.0f, 254.0f, 280.0f));

            // 4x4 * 3x4
            const mat3x4 m34(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
            VX_CHECK((m44 * m34) == mat3x4(90.0f, 100.0f, 110.0f, 120.0f, 202.0f, 228.0f, 254.0f, 280.0f, 314.0f, 356.0f, 398.0f, 440.0f));

            // 4x4 * 4x4
            VX_CHECK((m44 * m44) == mat4x4(90.0f, 100.0f, 110.0f, 120.0f, 202.0f, 228.0f, 254.0f, 280.0f, 314.0f, 356.0f, 398.0f, 440.0f, 426.0f, 484.0f, 542.0f, 600.0f));
        }

        // division
        {
            VX_CHECK(mat4x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f, 65.0f, 70.0f, 75.0f, 80.0f) / 5.0f == m44);
            VX_CHECK(720720.0f / mat4x4(720720.0f, 360360.0f, 240240.0f, 180180.0f, 144144.0f, 120120.0f, 102960.0f, 90090.0f, 80080.0f, 72072.0f, 65520.0f, 60060.0f, 55440.0f, 51480.0f, 48048.0f, 45045.0f) == m44);
        }
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            mat4x4 m = m44;
            m += 5.0f;
            VX_CHECK(m == mat4x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f));

            m += m44;
            VX_CHECK(m == mat4x4(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f, 23.0f, 25.0f, 27.0f, 29.0f, 31.0f, 33.0f, 35.0f, 37.0f));
        }

        // subtraction
        {
            mat4x4 m(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f, 23.0f, 25.0f, 27.0f, 29.0f, 31.0f, 33.0f, 35.0f, 37.0f);
            m -= m44;
            VX_CHECK(m == mat4x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f));

            m -= 5.0f;
            VX_CHECK(m == m44);
        }

        // multiplication
        {
            mat4x4 m = m44;
            m *= 5.0f;
            VX_CHECK(m == mat4x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f, 65.0f, 70.0f, 75.0f, 80.0f));

            m *= m44;
            VX_CHECK(m == mat4x4(450.0f, 500.0f, 550.0f, 600.0f, 1010.0f, 1140.0f, 1270.0f, 1400.0f, 1570.0f, 1780.0f, 1990.0f, 2200.0f, 2130.0f, 2420.0f, 2710.0f, 3000.0f));
        }

        // division
        {
            mat4x4 m(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f, 65.0f, 70.0f, 75.0f, 80.0f);
            m /= 5.0f;
            VX_CHECK(m == m44);
        }
    }

    VX_SECTION("static constructors")
    {
        // identity
        {
            const mat4x4 i = mat4x4::identity();
            VX_CHECK(i.columns[0] == vec4(1.0f, 0.0f, 0.0f, 0.0f));
            VX_CHECK(i.columns[1] == vec4(0.0f, 1.0f, 0.0f, 0.0f));
            VX_CHECK(i.columns[2] == vec4(0.0f, 0.0f, 1.0f, 0.0f));
            VX_CHECK(i.columns[3] == vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        // zero
        {
            const mat4x4 i = mat4x4::zero();
            VX_CHECK(i.columns[0] == vec4(0.0f, 0.0f, 0.0f, 0.0f));
            VX_CHECK(i.columns[1] == vec4(0.0f, 0.0f, 0.0f, 0.0f));
            VX_CHECK(i.columns[2] == vec4(0.0f, 0.0f, 0.0f, 0.0f));
            VX_CHECK(i.columns[3] == vec4(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}