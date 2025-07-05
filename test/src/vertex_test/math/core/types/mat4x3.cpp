#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat4x3)
{
    VX_SECTION("types")
    {
        VX_STATIC_CHECK((is_same<typename mat4x3::scalar_type, f32>::value));
        VX_STATIC_CHECK((is_same<typename mat4x3::row_type, vec4>::value));
        VX_STATIC_CHECK((is_same<typename mat4x3::col_type, vec3>::value));
        VX_STATIC_CHECK((is_same<typename mat4x3::transpose_type, mat3x4>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(mat4x3::width == 4);
        VX_STATIC_CHECK(mat4x3::height == 3);

        VX_STATIC_CHECK(sizeof(mat4x3) == 48);
        VX_STATIC_CHECK(mat4x3::size == 12);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        {
            {
                constexpr mat4x3 m;
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(0.0f, 0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(0.0f, 0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(0.0f, 0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec3(0.0f, 0.0f, 0.0f));
            }

            {
                constexpr mat4x3 m(mat4x3(2.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec3(2.0f, 2.0f, 2.0f));
            }
        }

        // explicit constructors
        {
            constexpr vec3 c1(1.0f, 2.0f, 3.0f);
            constexpr vec3 c2(4.0f, 5.0f, 6.0f);
            constexpr vec3 c3(7.0f, 8.0f, 9.0f);
            constexpr vec3 c4(10.0f, 11.0f, 12.0f);

            {
                constexpr mat4x3 m(
                    c1[0], c1[1], c1[2],
                    c2[0], c2[1], c2[2],
                    c3[0], c3[1], c3[2],
                    c4[0], c4[1], c4[2]
                );
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(7.0f, 8.0f, 9.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec3(10.0f, 11.0f, 12.0f));
            }

            {
                constexpr mat4x3 m(c1, c2, c3, c4);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
                VX_STATIC_CHECK_EQ(m.columns[2], c3);
                VX_STATIC_CHECK_EQ(m.columns[3], c4);
            }
        }


        // conversion constructors
        {
            {
                constexpr mat4x3 m(mat4x3(2u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec3(2.0f, 2.0f, 2.0f));
            }

            {
                constexpr mat4x3 m(true, 2u, 3.0, 4, 5.0f, 6u, 7, 8.0f, 9u, 10, 11.0f, 12u);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(7.0f, 8.0f, 9.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec3(10.0f, 11.0f, 12.0f));
            }

            {
                constexpr mat4x3 m(vec3i(1, 2, 3), vec3u(4u, 5u, 6u), vec3(7.0f, 8.0f, 9.0f), vec3d(10.0, 11.0, 12.0));
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(7.0f, 8.0f, 9.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec3(10.0f, 11.0f, 12.0f));
            }
        }

        // from larger
        {
            {
                constexpr mat4x4 mx(
                    1.0f, 2.0f, 3.0f, 4.0f,
                    5.0f, 6.0f, 7.0f, 8.0f,
                    9.0f, 10.0f, 11.0f, 12.0f,
                    13.0f, 14.0f, 15.0f, 16.0f
                );
                constexpr mat4x3 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(5.0f, 6.0f, 7.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(9.0f, 10.0f, 11.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec3(13.0f, 14.0f, 15.0f));
            }
        }
    }

    VX_SECTION("assignment")
    {
        {
            mat4x3 m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
            mat4x3 m2(13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec3(13.0f, 14.0f, 15.0f));
            VX_CHECK_EQ(m1.columns[1], vec3(16.0f, 17.0f, 18.0f));
            VX_CHECK_EQ(m1.columns[2], vec3(19.0f, 20.0f, 21.0f));
            VX_CHECK_EQ(m1.columns[3], vec3(22.0f, 23.0f, 24.0f));
        }

        {
            mat4x3f m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
            mat4x3i m2(13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec3(13.0f, 14.0f, 15.0f));
            VX_CHECK_EQ(m1.columns[1], vec3(16.0f, 17.0f, 18.0f));
            VX_CHECK_EQ(m1.columns[2], vec3(19.0f, 20.0f, 21.0f));
            VX_CHECK_EQ(m1.columns[3], vec3(22.0f, 23.0f, 24.0f));
        }
    }


    constexpr mat4x3 m43(
        1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f,
        7.0f, 8.0f, 9.0f,
        10.0f, 11.0f, 12.0f
    );

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(m43[0] == vec3(1.0f, 2.0f, 3.0f));
        VX_STATIC_CHECK(m43[1] == vec3(4.0f, 5.0f, 6.0f));
        VX_STATIC_CHECK(m43[2] == vec3(7.0f, 8.0f, 9.0f));
        VX_STATIC_CHECK(m43[3] == vec3(10.0f, 11.0f, 12.0f));

        mat4x3 m;

        m[0] = vec3(1.0f, 2.0f, 3.0f);
        VX_CHECK_EQ(m[0], vec3(1.0f, 2.0f, 3.0f));
        m[1] = vec3(4.0f, 5.0f, 6.0f);
        VX_CHECK_EQ(m[1], vec3(4.0f, 5.0f, 6.0f));
        m[2] = vec3(7.0f, 8.0f, 9.0f);
        VX_CHECK_EQ(m[2], vec3(7.0f, 8.0f, 9.0f));
        m[3] = vec3(10.0f, 11.0f, 12.0f);
        VX_CHECK_EQ(m[3], vec3(10.0f, 11.0f, 12.0f));
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(m43 == m43);
        VX_STATIC_CHECK(m43 != mat4x3(1.0f, -2.0f, 3.0f, 4.0f, -5.0f, 6.0f, 7.0f, -8.0f, 9.0f, 10.0f, -11.0f, 12.0f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+mat4x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f) ==
            mat4x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f));

        VX_STATIC_CHECK(+mat4x3(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f, -10.0f, 11.0f, -12.0f) ==
            mat4x3(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f, -10.0f, 11.0f, -12.0f));

        VX_STATIC_CHECK(-mat4x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f) ==
            mat4x3(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f, -10.0f, -11.0f, -12.0f));

        VX_STATIC_CHECK(-mat4x3(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f, -10.0f, -11.0f, -12.0f) ==
            mat4x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++mat4x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f) == mat4x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
        VX_STATIC_CHECK(--mat4x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f) == mat4x3(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));

        mat4x3 m = mat4x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);

        VX_CHECK(m++ == mat4x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f));
        VX_CHECK(m == mat4x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
        VX_CHECK(m-- == mat4x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
        VX_CHECK(m == mat4x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        {
            VX_STATIC_CHECK(m43 + 5.0f == mat4x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));
            VX_STATIC_CHECK(5.0f + m43 == mat4x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));
            VX_STATIC_CHECK(m43 + mat4x3(5.0f) == mat4x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));
            VX_STATIC_CHECK(mat4x3(5.0f) + m43 == mat4x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));
        }

        // subtraction
        {
            VX_STATIC_CHECK(mat4x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f) - 5.0f == m43);
            VX_STATIC_CHECK(5.0f - mat4x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f) == -m43);
            VX_STATIC_CHECK(mat4x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f) - mat4x3(5.0f) == m43);
            VX_STATIC_CHECK(mat4x3(5.0f) - mat4x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f) == -m43);
        }

        // multiplication
        {
            VX_STATIC_CHECK(m43 * 5.0f == mat4x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f));
            VX_STATIC_CHECK(5.0f * m43 == mat4x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f));

            // 3x1 * 4x3
            constexpr vec3 m31(1.0f, 2.0f, 3.0f);
            VX_STATIC_CHECK((m31 * m43) == vec4(14.0f, 32.0f, 50.0f, 68.0f));

            // 4x3 * 1x4
            constexpr vec4 m14(1.0f, 2.0f, 3.0f, 4.0f);
            VX_STATIC_CHECK((m43 * m14) == vec3(70.0f, 80.0f, 90.0f));

            // 4x3 * 2x4
            constexpr mat2x4 m24(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
            VX_STATIC_CHECK((m43 * m24) == mat2x3(70.0f, 80.0f, 90.0f, 158.0f, 184.0f, 210.0f));

            // 4x3 * 3x4
            constexpr mat3x4 m34(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
            VX_STATIC_CHECK((m43 * m34) == mat3x3(70.0f, 80.0f, 90.0f, 158.0f, 184.0f, 210.0f, 246.0f, 288.0f, 330.0f));

            // 4x3 * 4x4
            constexpr mat4x4 m44(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
            VX_STATIC_CHECK((m43 * m44) == mat4x3(70.0f, 80.0f, 90.0f, 158.0f, 184.0f, 210.0f, 246.0f, 288.0f, 330.0f, 334.0f, 392.0f, 450.0f));
        }

        // division
        {
            VX_STATIC_CHECK(mat4x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f) / 5.0f == m43);
            VX_STATIC_CHECK(27720.0f / mat4x3(27720.0f, 13860.0f, 9240.0f, 6930.0f, 5544.0f, 4620.0f, 3960.0f, 3465.0f, 3080.0f, 2772.0f, 2520.0f, 2310.0f) == m43);
        }
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            mat4x3 m = m43;
            m += 5.0f;
            VX_CHECK(m == mat4x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));

            m += m43;
            VX_CHECK(m == mat4x3(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f, 23.0f, 25.0f, 27.0f, 29.0f));
        }

        // subtraction
        {
            mat4x3 m(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f, 23.0f, 25.0f, 27.0f, 29.0f);
            m -= m43;
            VX_CHECK(m == mat4x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f));

            m -= 5.0f;
            VX_CHECK(m == m43);
        }

        // multiplication
        {
            mat4x3 m = m43;
            m *= 5.0f;
            VX_CHECK(m == mat4x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f));

            constexpr mat4x4 m44(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
            m *= m44;
            VX_CHECK(m == mat4x3(350.0f, 400.0f, 450.0f, 790.0f, 920.0f, 1050.0f, 1230.0f, 1440.0f, 1650.0f, 1670.0f, 1960.0f, 2250.0f));
        }

        // division
        {
            mat4x3 m(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f);
            m /= 5.0f;
            VX_CHECK(m == m43);
        }
    }

    VX_SECTION("static constructors")
    {
        // zero
        {
            constexpr mat4x3 i = mat4x3::zero();
            VX_STATIC_CHECK(i.columns[0] == vec3(0.0f, 0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[1] == vec3(0.0f, 0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[2] == vec3(0.0f, 0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[3] == vec3(0.0f, 0.0f, 0.0f));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}