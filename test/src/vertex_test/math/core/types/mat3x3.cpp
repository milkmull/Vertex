#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat3x3)
{
    VX_SECTION("types")
    {
        VX_STATIC_CHECK((is_same<typename mat3x3::scalar_type, f32>::value));
        VX_STATIC_CHECK((is_same<typename mat3x3::row_type, vec3>::value));
        VX_STATIC_CHECK((is_same<typename mat3x3::col_type, vec3>::value));
        VX_STATIC_CHECK((is_same<typename mat3x3::transpose_type, mat3x3>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(mat3x3::width == 3);
        VX_STATIC_CHECK(mat3x3::height == 3);

        VX_STATIC_CHECK(sizeof(mat3x3) == 36);
        VX_STATIC_CHECK(mat3x3::size == 9);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        {
            {
                constexpr mat3x3 m;
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(0.0f, 1.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(0.0f, 0.0f, 1.0f));
            }

            {
                constexpr mat3x3 m(mat3x3(2.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(2.0f, 0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(0.0f, 2.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(0.0f, 0.0f, 2.0f));
            }
        }

        // explicit constructors
        {
            constexpr vec3 c1(1.0f, 2.0f, 3.0f);
            constexpr vec3 c2(4.0f, 5.0f, 6.0f);
            constexpr vec3 c3(7.0f, 8.0f, 9.0f);

            {
                constexpr mat3x3 m(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(7.0f, 8.0f, 9.0f));
            }

            {
                constexpr mat3x3 m(c1, c2, c3);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
                VX_STATIC_CHECK_EQ(m.columns[2], c3);
            }
        }

        // conversion constructors
        {
            {
                constexpr mat3x3 m(mat3x3(2u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(2.0f, 0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(0.0f, 2.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(0.0f, 0.0f, 2.0f));
            }

            {
                constexpr mat3x3 m(true, 2u, 3.0, 4, 5.0f, 6u, 7, 8, 9);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(7.0f, 8.0f, 9.0f));
            }

            {
                constexpr mat3x3 m(vec3i(1, 2, 3), vec3u(4u, 5u, 6u), vec3(7.0f, 8.0f, 9.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(7.0f, 8.0f, 9.0f));
            }
        }

        // from larger
        {
            {
                constexpr mat3x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                constexpr mat3x3 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(5.0f, 6.0f, 7.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(9.0f, 10.0f, 11.0f));
            }

            {
                constexpr mat4x3 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                constexpr mat3x3 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(7.0f, 8.0f, 9.0f));
            }

            {
                constexpr mat4x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
                constexpr mat3x3 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(5.0f, 6.0f, 7.0f));
                VX_STATIC_CHECK_EQ(m.columns[2], vec3(9.0f, 10.0f, 11.0f));
            }
        }
    }

    VX_SECTION("assignment")
    {
        {
            mat3x3 m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
            mat3x3 m2(10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec3(10.0f, 11.0f, 12.0f));
            VX_CHECK_EQ(m1.columns[1], vec3(13.0f, 14.0f, 15.0f));
            VX_CHECK_EQ(m1.columns[2], vec3(16.0f, 17.0f, 18.0f));
        }

        {
            mat3x3f m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
            mat3x3i m2(10, 11, 12, 13, 14, 15, 16, 17, 18);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec3(10.0f, 11.0f, 12.0f));
            VX_CHECK_EQ(m1.columns[1], vec3(13.0f, 14.0f, 15.0f));
            VX_CHECK_EQ(m1.columns[2], vec3(16.0f, 17.0f, 18.0f));
        }
    }

    constexpr mat3x3 m33(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(m33[0] == vec3(1.0f, 2.0f, 3.0f));
        VX_STATIC_CHECK(m33[1] == vec3(4.0f, 5.0f, 6.0f));
        VX_STATIC_CHECK(m33[2] == vec3(7.0f, 8.0f, 9.0f));

        mat3x3 m;

        m[0] = vec3(1.0f, 2.0f, 3.0f);
        VX_CHECK_EQ(m[0], vec3(1.0f, 2.0f, 3.0f));
        m[1] = vec3(4.0f, 5.0f, 6.0f);
        VX_CHECK_EQ(m[1], vec3(4.0f, 5.0f, 6.0f));
        m[2] = vec3(7.0f, 8.0f, 9.0f);
        VX_CHECK_EQ(m[2], vec3(7.0f, 8.0f, 9.0f));
    }


    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(m33 == m33);
        VX_STATIC_CHECK(m33 != mat3x3(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+mat3x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f) == mat3x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));
        VX_STATIC_CHECK(+mat3x3(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f) == mat3x3(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f));

        VX_STATIC_CHECK(-mat3x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f) == mat3x3(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f));
        VX_STATIC_CHECK(-mat3x3(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f) == mat3x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++mat3x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f) == mat3x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f));
        VX_STATIC_CHECK(--mat3x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f) == mat3x3(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f));

        mat3x3 m = m33;

        VX_CHECK(m++ == mat3x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));
        VX_CHECK(m == mat3x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f));
        VX_CHECK(m-- == mat3x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f));
        VX_CHECK(m == mat3x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));
    }

    constexpr auto z = m33 + mat3x3(5.0f);

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        {
            VX_STATIC_CHECK(m33 + 5.0f == mat3x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f));
            VX_STATIC_CHECK(5.0f + m33 == mat3x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f));
            VX_STATIC_CHECK(m33 + mat3x3(5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f) == mat3x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f));
            VX_STATIC_CHECK(mat3x3(5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f) + m33 == mat3x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f));
        }

        // subtraction
        {
            VX_STATIC_CHECK(mat3x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f) - 5.0f == m33);
            VX_STATIC_CHECK(5.0f - mat3x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f) == -m33);
            VX_STATIC_CHECK(mat3x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f) - mat3x3(5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f) == m33);
            VX_STATIC_CHECK(mat3x3(5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f) - mat3x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f) == -m33);
        }

        // multiplication
        {
            VX_STATIC_CHECK(m33 * 5.0f == mat3x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f));
            VX_STATIC_CHECK(5.0f * m33 == mat3x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f));

            // 3x3 * 2x3
            constexpr mat2x3 m23(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            VX_STATIC_CHECK((m33 * m23) == mat2x3(30.0f, 36.0f, 42.0f, 66.0f, 81.0f, 96.0f));

            // 3x3 * 3x3
            constexpr mat3x3 m33(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
            VX_STATIC_CHECK((m33 * m33) == mat3x3(30.0f, 36.0f, 42.0f, 66.0f, 81.0f, 96.0f, 102.0f, 126.0f, 150.0f));

            // 3x3 * 4x3
            constexpr mat4x3 m43(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
            VX_STATIC_CHECK((m33 * m43) == mat4x3(30.0f, 36.0f, 42.0f, 66.0f, 81.0f, 96.0f, 102.0f, 126.0f, 150.0f, 138.0f, 171.0f, 204.0f));
        }

        // division
        {
            VX_STATIC_CHECK(mat3x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f) / 5.0f == m33);
            VX_STATIC_CHECK(2520.0f / mat3x3(2520.0f, 1260.0f, 840.0f, 630.0f, 504.0f, 420.0f, 360.0f, 315.0f, 280.0f) == m33);
        }
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            mat3x3 m = m33;
            m += 5.0f;
            VX_CHECK(m == mat3x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f));

            m += m33;
            VX_CHECK(m == mat3x3(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f, 23.0f));
        }

        // subtraction
        {
            mat3x3 m(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f, 23.0f);
            m -= m33;
            VX_CHECK(m == mat3x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f));

            m -= 5.0f;
            VX_CHECK(m == m33);
        }

        // multiplication
        {
            mat3x3 m = m33;
            m *= 5.0f;
            VX_CHECK(m == mat3x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f));

            constexpr mat3x3 m33(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
            m *= m33;
            VX_CHECK(m == mat3x3(150.0f, 180.0f, 210.0f, 330.0f, 405.0f, 480.0f, 510.0f, 630.0f, 750.0f));
        }

        // division
        {
            mat3x3 m(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f);
            m /= 5.0f;
            VX_CHECK(m == m33);
        }
    }

    VX_SECTION("static constructors")
    {
        // identity
        {
            constexpr mat3x3 i = mat3x3::identity();
            VX_STATIC_CHECK(i.columns[0] == vec3(1.0f, 0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[1] == vec3(0.0f, 1.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[2] == vec3(0.0f, 0.0f, 1.0f));
        }

        // zero
        {
            constexpr mat3x3 i = mat3x3::zero();
            VX_STATIC_CHECK(i.columns[0] == vec3(0.0f, 0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[1] == vec3(0.0f, 0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[2] == vec3(0.0f, 0.0f, 0.0f));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}