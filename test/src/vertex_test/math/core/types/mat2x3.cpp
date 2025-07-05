#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat2x3)
{
    VX_SECTION("types")
    {
        VX_STATIC_CHECK((is_same<typename mat2x3::scalar_type, f32>::value));
        VX_STATIC_CHECK((is_same<typename mat2x3::row_type, vec2>::value));
        VX_STATIC_CHECK((is_same<typename mat2x3::col_type, vec3>::value));
        VX_STATIC_CHECK((is_same<typename mat2x3::transpose_type, mat3x2>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(mat2x3::width == 2);
        VX_STATIC_CHECK(mat2x3::height == 3);

        VX_STATIC_CHECK(sizeof(mat2x3) == 24);
        VX_STATIC_CHECK(mat2x3::size == 6);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        {
            {
                constexpr mat2x3 m;
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(0.0f, 0.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(0.0f, 0.0f, 0.0f));
            }

            {
                constexpr mat2x3 m(mat2x3(2.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(2.0f, 2.0f, 2.0f));
            }
        }

        // explicit constructors
        {
            constexpr vec3 c1(1.0f, 2.0f, 3.0f);
            constexpr vec3 c2(4.0f, 5.0f, 6.0f);

            {
                constexpr mat2x3 m(c1[0], c1[1], c1[2], c2[0], c2[1], c2[2]);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
            }

            {
                constexpr mat2x3 m(c1, c2);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
            }
        }

        // conversion constructors
        {
            {
                constexpr mat2x3 m(mat2x3(2u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(2.0f, 2.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(2.0f, 2.0f, 2.0f));
            }

            {
                constexpr mat2x3 m(true, 2.0f, 3u, 4.0, 5, 6.0f);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
            }

            {
                constexpr mat2x3 m(vec3i(1, 2, 3), vec3u(4u, 5u, 6u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
            }
        }

        // from larger
        {
            {
                constexpr mat2x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
                constexpr mat2x3 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(5.0f, 6.0f, 7.0f));
            }

            {
                constexpr mat3x3 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
                constexpr mat2x3 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
            }

            {
                constexpr mat3x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                constexpr mat2x3 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(5.0f, 6.0f, 7.0f));
            }

            {
                constexpr mat4x3 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                constexpr mat2x3 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(4.0f, 5.0f, 6.0f));
            }

            {
                constexpr mat4x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
                constexpr mat2x3 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec3(1.0f, 2.0f, 3.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec3(5.0f, 6.0f, 7.0f));
            }
        }
    }

    VX_SECTION("assignment")
    {
        {
            mat2x3 m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            mat2x3 m2(7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec3(7.0f, 8.0f, 9.0f));
            VX_CHECK_EQ(m1.columns[1], vec3(10.0f, 11.0f, 12.0f));
        }

        {
            mat2x3f m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            mat2x3i m2(7, 8, 9, 10, 11, 12);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec3(7.0f, 8.0f, 9.0f));
            VX_CHECK_EQ(m1.columns[1], vec3(10.0f, 11.0f, 12.0f));
        }
    }

    constexpr mat2x3 m23(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(m23[0] == vec3(1.0f, 2.0f, 3.0f));
        VX_STATIC_CHECK(m23[1] == vec3(4.0f, 5.0f, 6.0f));

        mat2x3 m;

        m[0] = vec3(1.0f, 2.0f, 3.0f);
        VX_CHECK_EQ(m[0], vec3(1.0f, 2.0f, 3.0f));
        m[1] = vec3(4.0f, 5.0f, 6.0f);
        VX_CHECK_EQ(m[1], vec3(4.0f, 5.0f, 6.0f));
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(m23 == m23);
        VX_STATIC_CHECK(m23 != mat2x3(-1.0f, 2.0f, -3.0f, 4.0f, -5.0f, 6.0f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+mat2x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f) == mat2x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f));
        VX_STATIC_CHECK(+mat2x3f(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f) == mat2x3f(1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f));

        VX_STATIC_CHECK(-mat2x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f) == mat2x3f(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f));
        VX_STATIC_CHECK(-mat2x3f(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f) == mat2x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++mat2x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f) == mat2x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f));
        VX_STATIC_CHECK(--mat2x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f) == mat2x3(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f));

        mat2x3 m = m23;

        VX_CHECK(m++ == mat2x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f));
        VX_CHECK(m == mat2x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f));
        VX_CHECK(m-- == mat2x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f));
        VX_CHECK(m == mat2x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        {
            VX_STATIC_CHECK(m23 + 5.0f == mat2x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));
            VX_STATIC_CHECK(5.0f + m23 == mat2x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));
            VX_STATIC_CHECK(m23 + mat2x3(5.0f) == mat2x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));
            VX_STATIC_CHECK(mat2x3(5.0f) + m23 == mat2x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));
        }

        // subtraction
        {
            VX_STATIC_CHECK(mat2x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f) - 5.0f == m23);
            VX_STATIC_CHECK(5.0f - mat2x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f) == -m23);
            VX_STATIC_CHECK(mat2x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f) - mat2x3(5.0f) == m23);
            VX_STATIC_CHECK(mat2x3(5.0f) - mat2x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f) == -m23);
        }

        // multiplication
        {
            VX_STATIC_CHECK(m23 * 5.0f == mat2x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f));
            VX_STATIC_CHECK(5.0f * m23 == mat2x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f));

            // 3x1 * 2x3
            constexpr vec3 m31(1.0f, 2.0f, 3.0f);
            VX_STATIC_CHECK((m31 * m23) == vec2(14.0f, 32.0f));

            // 2x3 * 1x2
            constexpr vec2 m12(1.0f, 2.0f);
            VX_STATIC_CHECK((m23 * m12) == vec3(9.0f, 12.0f, 15.0f));

            // 2x3 * 2x2
            constexpr mat2x2 m22(1.0f, 2.0f, 3.0f, 4.0f);
            VX_STATIC_CHECK((m23 * m22) == mat2x3(9.0f, 12.0f, 15.0f, 19.0f, 26.0f, 33.0f));

            // 2x3 * 3x2
            constexpr mat3x2 m32(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            VX_STATIC_CHECK((m23 * m32) == mat3x3(9.0f, 12.0f, 15.0f, 19.0f, 26.0f, 33.0f, 29.0f, 40.0f, 51.0f));

            // 2x3 * 4x2
            constexpr mat4x2 m42(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
            VX_STATIC_CHECK((m23 * m42) == mat4x3(9.0f, 12.0f, 15.0f, 19.0f, 26.0f, 33.0f, 29.0f, 40.0f, 51.0f, 39.0f, 54.0f, 69.0f));
        }

        // division
        {
            VX_STATIC_CHECK(mat2x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f) / 5.0f == m23);
            VX_STATIC_CHECK(60.0f / mat2x3(60.0f, 30.0f, 20.0f, 15.0f, 12.0f, 10.0f) == m23);
        }
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            mat2x3 m = m23;
            m += 5.0f;
            VX_CHECK(m == mat2x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));

            m += m23;
            VX_CHECK(m == mat2x3(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f));
        }

        // subtraction
        {
            mat2x3 m(7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f);
            m -= m23;
            VX_CHECK(m == mat2x3(6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f));

            m -= 5.0f;
            VX_CHECK(m == m23);
        }

        // multiplication
        {
            mat2x3 m = m23;
            m *= 5.0f;
            VX_CHECK(m == mat2x3(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f));

            constexpr mat2x2 m22(1.0f, 2.0f, 3.0f, 4.0f);
            m *= m22;
            VX_CHECK(m == mat2x3(45.0f, 60.0f, 75.0f, 95.0f, 130.0f, 165.0f));
        }

        // division
        {
            mat2x3 m(5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f);
            m /= 5.0f;
            VX_CHECK(m == m23);
        }
    }

    VX_SECTION("static constructors")
    {
        // zero
        {
            constexpr mat2x3 i = mat2x3::zero();
            VX_STATIC_CHECK(i.columns[0] == vec3(0.0f, 0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[1] == vec3(0.0f, 0.0f, 0.0f));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}