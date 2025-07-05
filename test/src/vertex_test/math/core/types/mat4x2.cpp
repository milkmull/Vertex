#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat4x2)
{
    VX_SECTION("types")
    {
        VX_STATIC_CHECK((is_same<typename mat4x2::scalar_type, f32>::value));
        VX_STATIC_CHECK((is_same<typename mat4x2::row_type, vec4>::value));
        VX_STATIC_CHECK((is_same<typename mat4x2::col_type, vec2>::value));
        VX_STATIC_CHECK((is_same<typename mat4x2::transpose_type, mat2x4>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(mat4x2::width == 4);
        VX_STATIC_CHECK(mat4x2::height == 2);

        VX_STATIC_CHECK(sizeof(mat4x2) == 32);
        VX_STATIC_CHECK(mat4x2::size == 8);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        {
            {
                constexpr mat4x2 m;
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec2(0.0f, 0.0f));
            }

            {
                constexpr mat4x2 m(mat4x2(2.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec2(2.0f, 2.0f));
            }
        }

        // explicit constructors
        {
            constexpr vec2 c1(1.0f, 2.0f);
            constexpr vec2 c2(3.0f, 4.0f);
            constexpr vec2 c3(5.0f, 6.0f);
            constexpr vec2 c4(7.0f, 8.0f);

            {
                constexpr mat4x2 m(c1[0], c1[1], c2[0], c2[1], c3[0], c3[1], c4[0], c4[1]);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec2(7.0f, 8.0f));
            }

            {
                constexpr mat4x2 m(c1, c2, c3, c4);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
                VX_STATIC_CHECK_EQ(m.columns[2], c3);
                VX_STATIC_CHECK_EQ(m.columns[3], c4);
            }
        }

        // conversion constructors
        {
            {
                constexpr mat4x2 m(mat4x2(2u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec2(2.0f, 2.0f));
            }

            {
                constexpr mat4x2 m(true, 2u, 3.0, 4, 5.0f, 6u, 7, 8.0f);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec2(7.0f, 8.0f));
            }

            {
                constexpr mat4x2 m(vec2i(1, 2), vec2u(3u, 4u), vec2(5.0f, 6.0f), vec2d(7.0, 8.0));
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec2(7.0f, 8.0f));
            }
        }

        // from larger
        {
            {
                constexpr mat4x3 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                constexpr mat4x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(4.0f, 5.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(7.0f, 8.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec2(10.0f, 11.0f));
            }

            {
                constexpr mat4x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
                constexpr mat4x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec2(9.0f, 10.0f));
                VX_STATIC_CHECK_EQ(m.columns[3], vec2(13.0f, 14.0f));
            }
        }
    }

    VX_SECTION("assignment")
    {
        {
            mat4x2 m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
            mat4x2 m2(9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec2(9.0f, 10.0f));
            VX_CHECK_EQ(m1.columns[1], vec2(11.0f, 12.0f));
            VX_CHECK_EQ(m1.columns[2], vec2(13.0f, 14.0f));
            VX_CHECK_EQ(m1.columns[3], vec2(15.0f, 16.0f));
        }

        {
            mat4x2f m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
            mat4x2i m2(9, 10, 11, 12, 13, 14, 15, 16);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec2(9.0f, 10.0f));
            VX_CHECK_EQ(m1.columns[1], vec2(11.0f, 12.0f));
            VX_CHECK_EQ(m1.columns[2], vec2(13.0f, 14.0f));
            VX_CHECK_EQ(m1.columns[3], vec2(15.0f, 16.0f));
        }
    }


    constexpr mat4x2 m42(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(m42[0] == vec2(1.0f, 2.0f));
        VX_STATIC_CHECK(m42[1] == vec2(3.0f, 4.0f));
        VX_STATIC_CHECK(m42[2] == vec2(5.0f, 6.0f));
        VX_STATIC_CHECK(m42[3] == vec2(7.0f, 8.0f));

        mat4x2 m;

        m[0] = vec2(1.0f, 2.0f);
        VX_CHECK_EQ(m[0], vec2(1.0f, 2.0f));
        m[1] = vec2(3.0f, 4.0f);
        VX_CHECK_EQ(m[1], vec2(3.0f, 4.0f));
        m[2] = vec2(5.0f, 6.0f);
        VX_CHECK_EQ(m[2], vec2(5.0f, 6.0f));
        m[3] = vec2(7.0f, 8.0f);
        VX_CHECK_EQ(m[3], vec2(7.0f, 8.0f));
    }


    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(m42 == m42);
        VX_STATIC_CHECK(m42 != mat4x2(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+mat4x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f) ==
            mat4x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f));

        VX_STATIC_CHECK(+mat4x2(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f) ==
            mat4x2(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f));

        VX_STATIC_CHECK(-mat4x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f) ==
            mat4x2(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f));

        VX_STATIC_CHECK(-mat4x2(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f) ==
            mat4x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++mat4x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f) ==
            mat4x2(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));

        VX_STATIC_CHECK(--mat4x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f) ==
            mat4x2(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f));

        mat4x2 m = m42;

        VX_CHECK(m++ == mat4x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f));
        VX_CHECK(m == mat4x2(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));
        VX_CHECK(m-- == mat4x2(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));
        VX_CHECK(m == mat4x2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f));
    }


    VX_SECTION("binary arithmetic operators")
    {
        // addition
        {
            VX_STATIC_CHECK(m42 + 5.0f == mat4x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
            VX_STATIC_CHECK(5.0f + m42 == mat4x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
            VX_STATIC_CHECK(m42 + mat4x2(5.0f) == mat4x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
            VX_STATIC_CHECK(mat4x2(5.0f) + m42 == mat4x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
        }

        // subtraction
        {
            VX_STATIC_CHECK(mat4x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f) - 5.0f == m42);
            VX_STATIC_CHECK(5.0f - mat4x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f) == -m42);
            VX_STATIC_CHECK(mat4x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f) - mat4x2(5.0f) == m42);
            VX_STATIC_CHECK(mat4x2(5.0f) - mat4x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f) == -m42);
        }

        // multiplication
        {
            VX_STATIC_CHECK(m42 * 5.0f == mat4x2(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f));
            VX_STATIC_CHECK(5.0f * m42 == mat4x2(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f));

            // 2x1 * 4x2
            constexpr vec2 m21(1.0f, 2.0f);
            VX_STATIC_CHECK((m21 * m42) == vec4(5.0f, 11.0f, 17.0f, 23.0f));

            // 4x2 * 1x4
            constexpr vec4 m14(1.0f, 2.0f, 3.0f, 4.0f);
            VX_STATIC_CHECK((m42 * m14) == vec2(50.0f, 60.0f));

            // 4x2 * 2x4
            constexpr mat2x4 m24(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
            VX_STATIC_CHECK((m42 * m24) == mat2x2(50.0f, 60.0f, 114.0f, 140.0f));

            // 4x2 * 3x4
            constexpr mat3x4 m43(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
            VX_STATIC_CHECK((m42 * m43) == mat3x2(50.0f, 60.0f, 114.0f, 140.0f, 178.0f, 220.0f));

            // 4x2 * 4x4
            constexpr mat4x4 m44(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
            VX_STATIC_CHECK((m42 * m44) == mat4x2(50.0f, 60.0f, 114.0f, 140.0f, 178.0f, 220.0f, 242.0f, 300.0f));
        }

        // division
        {
            VX_STATIC_CHECK(mat4x2(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f) / 5.0f == m42);
            VX_STATIC_CHECK(840.0f / mat4x2(840.0f, 420.0f, 280.0f, 210.0f, 168.0f, 140.0f, 120.0f, 105.0f) == m42);
        }
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            mat4x2 m = m42;
            m += 5.0f;
            VX_CHECK(m == mat4x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));

            m += m42;
            VX_CHECK(m == mat4x2(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f));
        }

        // subtraction
        {
            mat4x2 m(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f);
            m -= m42;
            VX_CHECK(m == mat4x2(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));

            m -= 5.0f;
            VX_CHECK(m == m42);
        }

        // multiplication
        {
            mat4x2 m = m42;
            m *= 5.0f;
            VX_CHECK(m == mat4x2(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f));

            constexpr mat4x4 m44(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
            m *= m44;
            VX_CHECK(m == mat4x2(250.0f, 300.0f, 570.0f, 700.0f, 890.0f, 1100.0f, 1210.0f, 1500.0f));
        }

        // division
        {
            mat4x2 m(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f);
            m /= 5.0f;
            VX_CHECK(m == m42);
        }
    }

    VX_SECTION("static constructors")
    {
        // zero
        {
            constexpr mat4x2 i = mat4x2::zero();
            VX_STATIC_CHECK(i.columns[0] == vec2(0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[1] == vec2(0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[2] == vec2(0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[3] == vec2(0.0f, 0.0f));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}