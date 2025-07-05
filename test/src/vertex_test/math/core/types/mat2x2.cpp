#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat2x2)
{
    VX_SECTION("types")
    {
        VX_STATIC_CHECK((is_same<typename mat2x2::scalar_type, f32>::value));
        VX_STATIC_CHECK((is_same<typename mat2x2::row_type, vec2>::value));
        VX_STATIC_CHECK((is_same<typename mat2x2::col_type, vec2>::value));
        VX_STATIC_CHECK((is_same<typename mat2x2::transpose_type, mat2x2>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(mat2x2::width == 2);
        VX_STATIC_CHECK(mat2x2::height == 2);

        VX_STATIC_CHECK(sizeof(mat2x2) == 16);
        VX_STATIC_CHECK(mat2x2::size == 4);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        {
            {
                constexpr mat2x2 m;
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(0.0f, 1.0f));
            }

            {
                constexpr mat2x2 m(mat2x2(2.0f));
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(2.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(0.0f, 2.0f));
            }
        }

        // explicit constructors
        {
            constexpr vec2 c1(1.0f, 2.0f);
            constexpr vec2 c2(3.0f, 4.0f);

            {
                constexpr mat2x2 m(c1[0], c1[1], c2[0], c2[1]);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
            }

            {
                constexpr mat2x2 m(c1, c2);
                VX_STATIC_CHECK_EQ(m.columns[0], c1);
                VX_STATIC_CHECK_EQ(m.columns[1], c2);
            }
        }

        // conversion constructors
        {
            {
                constexpr mat2x2 m(mat2x2(2u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(2.0f, 0.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(0.0f, 2.0f));
            }

            {
                constexpr mat2x2 m(true, 2.0f, 3u, 4.0);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
            }

            {
                constexpr mat2x2 m(vec2i(1, 2), vec2u(3u, 4u));
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
            }
        }

        // from larger
        {
            {
                constexpr mat2x3 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
                constexpr mat2x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(4.0f, 5.0f));
            }

            {
                constexpr mat2x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
                constexpr mat2x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(5.0f, 6.0f));
            }

            {
                constexpr mat3x2 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
                constexpr mat2x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
            }

            {
                constexpr mat3x3 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
                constexpr mat2x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(4.0f, 5.0f));
            }

            {
                constexpr mat3x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                constexpr mat2x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(5.0f, 6.0f));
            }

            {
                constexpr mat4x2 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
                constexpr mat2x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(3.0f, 4.0f));
            }

            {
                constexpr mat4x3 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
                constexpr mat2x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(4.0f, 5.0f));
            }

            {
                constexpr mat4x4 mx(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
                constexpr mat2x2 m(mx);
                VX_STATIC_CHECK_EQ(m.columns[0], vec2(1.0f, 2.0f));
                VX_STATIC_CHECK_EQ(m.columns[1], vec2(5.0f, 6.0f));
            }
        }
    }

    VX_SECTION("assignment")
    {
        {
            mat2x2 m1(1.0f, 2.0f, 3.0f, 4.0f);
            mat2x2 m2(5.0f, 6.0f, 7.0f, 8.0f);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec2(5.0f, 6.0f));
            VX_CHECK_EQ(m1.columns[1], vec2(7.0f, 8.0f));
        }

        {
            mat2f m1(1.0f, 2.0f, 3.0f, 4.0f);
            mat2i m2(5, 6, 7, 8);

            m1 = m2;
            VX_CHECK_EQ(m1.columns[0], vec2(5.0f, 6.0f));
            VX_CHECK_EQ(m1.columns[1], vec2(7.0f, 8.0f));
        }
    }

    constexpr mat2x2 m22(1.0f, 2.0f, 3.0f, 4.0f);

    VX_SECTION("index")
    {
        VX_STATIC_CHECK(m22[0] == vec2(1.0f, 2.0f));
        VX_STATIC_CHECK(m22[1] == vec2(3.0f, 4.0f));

        mat2x2 m;

        m[0] = vec2(1.0f, 2.0f);
        VX_CHECK_EQ(m[0], vec2(1.0f, 2.0f));
        m[1] = vec2(3.0f, 4.0f);
        VX_CHECK_EQ(m[1], vec2(3.0f, 4.0f));
    }

    VX_SECTION("comparison")
    {
        VX_STATIC_CHECK(m22 == m22);
        VX_STATIC_CHECK(m22 != mat2x2(1.0f, -2.0f, 3.0f, -4.0f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+mat2x2(1.0f, 2.0f, 3.0f, 4.0f) == mat2x2(1.0f, 2.0f, 3.0f, 4.0f));
        VX_STATIC_CHECK(+mat2x2(1.0f, -2.0f, 3.0f, -4.0f) == mat2x2(1.0f, -2.0f, 3.0f, -4.0f));

        VX_STATIC_CHECK(-mat2x2(1.0f, 2.0f, 3.0f, 4.0f) == mat2x2(-1.0f, -2.0f, -3.0f, -4.0f));
        VX_STATIC_CHECK(-mat2x2(-1.0f, -2.0f, -3.0f, -4.0f) == mat2x2(1.0f, 2.0f, 3.0f, 4.0f));
    }

    VX_SECTION("increment and decrement operators")
    {
        VX_STATIC_CHECK(++mat2x2(1.0f, 2.0f, 3.0f, 4.0f) == mat2x2(2.0f, 3.0f, 4.0f, 5.0f));
        VX_STATIC_CHECK(--mat2x2(1.0f, 2.0f, 3.0f, 4.0f) == mat2x2(0.0f, 1.0f, 2.0f, 3.0f));

        mat2x2 m = m22;

        VX_CHECK(m++ == mat2x2(1.0f, 2.0f, 3.0f, 4.0f));
        VX_CHECK(m == mat2x2(2.0f, 3.0f, 4.0f, 5.0f));
        VX_CHECK(m-- == mat2x2(2.0f, 3.0f, 4.0f, 5.0f));
        VX_CHECK(m == mat2x2(1.0f, 2.0f, 3.0f, 4.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        // addition
        {
            VX_STATIC_CHECK(m22 + 5.0f == mat2x2(6.0f, 7.0f, 8.0f, 9.0f));
            VX_STATIC_CHECK(5.0f + m22 == mat2x2(6.0f, 7.0f, 8.0f, 9.0f));
            VX_STATIC_CHECK(m22 + mat2x2(5.0f, 5.0f, 5.0f, 5.0f) == mat2x2(6.0f, 7.0f, 8.0f, 9.0f));
            VX_STATIC_CHECK(mat2x2(5.0f, 5.0f, 5.0f, 5.0f) + m22 == mat2x2(6.0f, 7.0f, 8.0f, 9.0f));
        }

        // subtraction
        {
            VX_STATIC_CHECK(mat2x2(6.0f, 7.0f, 8.0f, 9.0f) - 5.0f == m22);
            VX_STATIC_CHECK(5.0f - mat2x2(6.0f, 7.0f, 8.0f, 9.0f) == -m22);
            VX_STATIC_CHECK(mat2x2(6.0f, 7.0f, 8.0f, 9.0f) - mat2x2(5.0f, 5.0f, 5.0f, 5.0f) == m22);
            VX_STATIC_CHECK(mat2x2(5.0f, 5.0f, 5.0f, 5.0f) - mat2x2(6.0f, 7.0f, 8.0f, 9.0f) == -m22);
        }

        // multiplication
        {
            VX_STATIC_CHECK(m22 * 5.0f == mat2x2(5.0f, 10.0f, 15.0f, 20.0f));
            VX_STATIC_CHECK(5.0f * m22 == mat2x2(5.0f, 10.0f, 15.0f, 20.0f));

            // 2x1 * 2x2
            constexpr vec2 m21(1.0f, 2.0f);
            VX_STATIC_CHECK((m21 * m22) == vec2(5.0f, 11.0f));

            // 2x2 * 1x2
            constexpr vec2 m12(1.0f, 2.0f);
            VX_STATIC_CHECK((m22 * m12) == vec2(7.0f, 10.0f));

            // 2x2 * 2x2
            VX_STATIC_CHECK((m22 * m22) == mat2x2(7.0f, 10.0f, 15.0f, 22.0f));

            // 2x2 * 3x2
            constexpr mat3x2 m32(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
            VX_STATIC_CHECK((m22 * m32) == mat3x2(7.0f, 10.0f, 15.0f, 22.0f, 23.0f, 34.0f));

            // 2x2 * 4x2
            constexpr mat4x2 m42(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
            VX_STATIC_CHECK((m22 * m42) == mat4x2(7.0f, 10.0f, 15.0f, 22.0f, 23.0f, 34.0f, 31.0f, 46.0f));
        }
        
        // division
        {
            VX_STATIC_CHECK(mat2x2(5.0f, 10.0f, 15.0f, 20.0f) / 5.0f == m22);
            VX_STATIC_CHECK(12.0f / mat2x2(12.0f, 6.0f, 4.0f, 3.0f) == m22);
        }
    }

    VX_SECTION("unary arithmetic operators")
    {
        // addition
        {
            mat2x2 m = m22;
            m += 5.0f;
            VX_CHECK(m == mat2x2(6.0f, 7.0f, 8.0f, 9.0f));

            m += m22;
            VX_CHECK(m == mat2x2(7.0f, 9.0f, 11.0f, 13.0f));
        }
    
        // subtraction
        {
            mat2x2 m(7.0f, 9.0f, 11.0f, 13.0f);
            m -= m22;
            VX_CHECK(m == mat2x2(6.0f, 7.0f, 8.0f, 9.0f));

            m -= 5.0f;
            VX_CHECK(m == m22);
        }
    
        // multiplication
        {
            mat2x2 m = m22;
            m *= 5.0f;
            VX_CHECK(m == mat2x2(5.0f, 10.0f, 15.0f, 20.0f));

            m *= m22;
            VX_CHECK(m == mat2x2(35.0f, 50.0f, 75.0f, 110.0f));
        }
    
        // division
        {
            mat2x2 m(5.0f, 10.0f, 15.0f, 20.0f);
            m /= 5.0f;
            VX_CHECK(m == m22);
        }
    }

    VX_SECTION("static constructors")
    {
        // identity
        {
            constexpr mat2x2 i = mat2x2::identity();
            VX_STATIC_CHECK(i.columns[0] == vec2(1.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[1] == vec2(0.0f, 1.0f));
        }

        // zero
        {
            constexpr mat2x2 i = mat2x2::zero();
            VX_STATIC_CHECK(i.columns[0] == vec2(0.0f, 0.0f));
            VX_STATIC_CHECK(i.columns[1] == vec2(0.0f, 0.0f));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}