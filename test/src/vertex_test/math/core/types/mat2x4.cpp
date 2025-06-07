#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat2x4)
{
    VX_SECTION("types")
    {
        VX_STATIC_CHECK((is_same<typename mat2x4::scalar_type, f32>::value));
        VX_STATIC_CHECK((is_same<typename mat2x4::row_type, vec2>::value));
        VX_STATIC_CHECK((is_same<typename mat2x4::col_type, vec4>::value));
        VX_STATIC_CHECK((is_same<typename mat2x4::transpose_type, mat4x2>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(mat2x4::width == 2);
        VX_STATIC_CHECK(mat2x4::height == 4);

        VX_STATIC_CHECK(sizeof(mat2x4) == 32);
        VX_STATIC_CHECK(mat2x4::size == 8);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        {
            {
                constexpr mat2x4 m;
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(0.0f, 0.0f, 0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(0.0f, 0.0f, 0.0f, 0.0f));
            }

            {
                constexpr mat2x4 m(mat2x4(2.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(2.0f, 2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(2.0f, 2.0f, 2.0f, 2.0f));
            }
        }

        // explicit constructors
        {
            constexpr vec4 c1(1.0f, 2.0f, 3.0f, 4.0f);
            constexpr vec4 c2(5.0f, 6.0f, 7.0f, 8.0f);

            {
                constexpr mat2x4 m(c1[0], c1[1], c1[2], c1[3], c2[0], c2[1], c2[2], c2[3]);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
            }

            {
                constexpr mat2x4 m(c1, c2);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
            }
        }

        // conversion constructors
        {
            {
                constexpr mat2x4 m(mat2x4(2u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(2.0f, 2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(2.0f, 2.0f, 2.0f, 2.0f));
            }

            {
                constexpr mat2x4 m(true, 2.0f, 3u, 4.0, 5, 6.0f, 7.0f, 8u);
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
            }

            {
                constexpr mat2x4 m(vec4i(1, 2, 3, 4), vec4u(5u, 6u, 7u, 8u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
            }
        }

        // from larger
        {
            {
                constexpr mat3x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                constexpr mat2x4 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
            }

            {
                constexpr mat4x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
                constexpr mat2x4 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
            }
        }
    }

    VX_SECTION("assignment")
    {
        {
            mat2x4 m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
            mat2x4 m2(9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec4(9.0f, 10.0f, 11.0f, 12.0f));
            VX_CHECK_EQ(m1.columns[1], vec4(13.0f, 14.0f, 15.0f, 16.0f));
        }

        {
            mat2x4f m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
            mat2x4i m2(9, 10, 11, 12, 13, 14, 15, 16);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec4(9.0f, 10.0f, 11.0f, 12.0f));
            VX_CHECK_EQ(m1.columns[1], vec4(13.0f, 14.0f, 15.0f, 16.0f));
        }
    }

    constexpr mat2x4 m24(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(m24[0] == vec4(1.0f, 2.0f, 3.0f, 4.0f));
        VX_STATIC_CHECK(m24[1] == vec4(5.0f, 6.0f, 7.0f, 8.0f));

        mat2x4 m;

        m[0] = vec4(1.0f, 2.0f, 3.0f, 4.0f);
        VX_CHECK_EQ(m[0], vec4(1.0f, 2.0f, 3.0f, 4.0f));
        m[1] = vec4(5.0f, 6.0f, 7.0f, 8.0f);
        VX_CHECK_EQ(m[1], vec4(5.0f, 6.0f, 7.0f, 8.0f));
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(m24 == m24);
        VX_STATIC_CHECK(m24 != mat2x4(-1.0f, 2.0f, -3.0f, 4.0f, 5.0f, -6.0f, 7.0f, -8.0f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+mat2x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f) == mat2x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f));
        VX_STATIC_CHECK(-mat2x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f) == mat2x4f(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f));

        VX_STATIC_CHECK(-mat2x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f) == mat2x4f(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f));
        VX_STATIC_CHECK(-mat2x4f(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f) == mat2x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++mat2x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f) == mat2x4(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));
        VX_STATIC_CHECK(--mat2x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f) == mat2x4(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f));

        mat2x4 m = m24;

        VX_CHECK(m++ == mat2x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f));
        VX_CHECK(m == mat2x4(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));
        VX_CHECK(m-- == mat2x4(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));
        VX_CHECK(m == mat2x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        {
            VX_STATIC_CHECK(m24 + 5.0f == mat2x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
            VX_STATIC_CHECK(5.0f + m24 == mat2x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
            VX_STATIC_CHECK(m24 + mat2x4(5.0f) == mat2x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
            VX_STATIC_CHECK(mat2x4(5.0f) + m24 == mat2x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));
        }

        // subtraction
        {
            VX_STATIC_CHECK(mat2x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f) - 5.0f == m24);
            VX_STATIC_CHECK(5.0f - mat2x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f) == -m24);
            VX_STATIC_CHECK(mat2x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f) - mat2x4(5.0f) == m24);
            VX_STATIC_CHECK(mat2x4(5.0f) - mat2x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f) == -m24);
        }

        // multiplication
        {
            VX_STATIC_CHECK(m24 * 5.0f == mat2x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f));
            VX_STATIC_CHECK(5.0f * m24 == mat2x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f));

            // 2x4 * 2x2
            constexpr mat2x2 m22(1.0f, 2.0f, 3.0f, 4.0f);
            VX_STATIC_CHECK((m24 * m22) == mat2x4(11.0f, 14.0f, 17.0f, 20.0f, 23.0f, 30.0f, 37.0f, 44.0f));

            // 2x4 * 3x2
            constexpr mat3x2 m32(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            VX_STATIC_CHECK((m24 * m32) == mat3x4(11.0f, 14.0f, 17.0f, 20.0f, 23.0f, 30.0f, 37.0f, 44.0f, 35.0f, 46.0f, 57.0f, 68.0f));

            // 2x4 * 4x2
            constexpr mat4x2 m42(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
            VX_STATIC_CHECK((m24 * m42) == mat4x4(11.0f, 14.0f, 17.0f, 20.0f, 23.0f, 30.0f, 37.0f, 44.0f, 35.0f, 46.0f, 57.0f, 68.0f, 47.0f, 62.0f, 77.0f, 92.0f));
        }

        // division
        {
            VX_STATIC_CHECK(mat2x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f) / 5.0f == m24);
            VX_STATIC_CHECK(840.0f / mat2x4(840.0f, 420.0f, 280.0f, 210.0f, 168.0f, 140.0f, 120.0f, 105.0f) == m24);
        }
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            mat2x4 m = m24;
            m += 5.0f;
            VX_CHECK(m == mat2x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));

            m += m24;
            VX_CHECK(m == mat2x4(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f));
        }

        // subtraction
        {
            mat2x4 m(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f);
            m -= m24;
            VX_CHECK(m == mat2x4(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f));

            m -= 5.0f;
            VX_CHECK(m == m24);
        }

        // multiplication
        {
            mat2x4 m = m24;
            m *= 5.0f;
            VX_CHECK(m == mat2x4(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f));

            constexpr mat2x2 m22(1.0f, 2.0f, 3.0f, 4.0f);
            m *= m22;
            VX_CHECK(m == mat2x4(55.0f, 70.0f, 85.0f, 100.0f, 115.0f, 150.0f, 185.0f, 220.0f));
        }

        // division
        {
            mat2x4 m(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f);
            m /= 5.0f;
            VX_CHECK(m == m24);
        }
    }

    VX_SECTION("static constructors")
    {
        // zero
        {
            constexpr mat2x4 i = mat2x4::zero();
            VX_STATIC_CHECK(i.columns[0] == vec4(0.0f, 0.0f, 0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[1] == vec4(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}