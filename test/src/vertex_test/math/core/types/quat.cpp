#include "vertex_test/test.hpp"

#include "vertex/math/core/types.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_quat)
{
    VX_SECTION("types")
    {
        VX_STATIC_CHECK((is_same<typename quat::scalar_type, f32>::value));
    }

    VX_SECTION("size")
    {
        VX_STATIC_CHECK(sizeof(quat) == 16);
        VX_STATIC_CHECK(quat::size == 4);
    }

    VX_SECTION("constructors")
    {
        // implicit constructors
        {
            {
                constexpr quat q;
                VX_STATIC_CHECK_EQ(q, quat(1.0f, 0.0f, 0.0f, 0.0f));
            }

            {
                constexpr quat q(quat(1.0f, 2.0f, 3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(q, quat(1.0f, 2.0f, 3.0f, 4.0f));

                VX_STATIC_CHECK_EQ(q.w, 1.0f);
                VX_STATIC_CHECK_EQ(q.x, 2.0f);
                VX_STATIC_CHECK_EQ(q.y, 3.0f);
                VX_STATIC_CHECK_EQ(q.z, 4.0f);
            }
        }

        // explicit constructors
        {
            {
                constexpr quat q(1.0f, vec3(2.0f, 3.0f, 4.0f));
                VX_STATIC_CHECK_EQ(q, quat(1.0f, 2.0f, 3.0f, 4.0f));
            }

            {
                constexpr quat q(1.0f, 2.0f, 3.0f, 4.0f);
                VX_STATIC_CHECK_EQ(q, quat(1.0f, 2.0f, 3.0f, 4.0f));
            }

            {
                constexpr quat q(vec4(2.0f, 3.0f, 4.0f, 1.0f));
                VX_STATIC_CHECK_EQ(q, quat(1.0f, 2.0f, 3.0f, 4.0f));
            }
        }

        // conversion constructors
        {
            {
                constexpr quat q(true, vec3u(2u, 3u, 4u));
                VX_STATIC_CHECK_EQ(q, quat(1.0f, 2.0f, 3.0f, 4.0f));
            }

            {
                constexpr quat q(true, 2u, 3, 4.0);
                VX_STATIC_CHECK_EQ(q, quat(1.0f, 2.0f, 3.0f, 4.0f));
            }

            {
                constexpr quat q(quatd(1.0, 2.0, 3.0, 4.0));
                VX_STATIC_CHECK_EQ(q, quat(1.0f, 2.0f, 3.0f, 4.0f));
            }

            {
                constexpr quat q(vec4i(2, 3, 4, 1));
                VX_STATIC_CHECK_EQ(q, quat(1.0f, 2.0f, 3.0f, 4.0f));
            }
        }
    }

    VX_SECTION("assignment")
    {
        {
            quat q1(1.0f, 2.0f, 3.0f, 4.0f);
            quat q2(5.0f, 6.0f, 7.0f, 8.0f);

            q1 = q2;
            VX_CHECK_EQ(q1, quat(5.0f, 6.0f, 7.0f, 8.0f));
        }

        {
            quatf q1(1.0f, 2.0f, 3.0f, 4.0f);
            quatd q2(5.0, 6.0, 7.0, 8.0);

            q1 = q2;
            VX_CHECK_EQ(q1, quat(5.0f, 6.0f, 7.0f, 8.0f));
        }
    }

    VX_SECTION("index")
    {
        constexpr quat q1(1.0f, 2.0, 3.0f, 4.0f);

        VX_STATIC_CHECK_EQ(q1[0], 1.0f);
        VX_STATIC_CHECK_EQ(q1[1], 2.0f);
        VX_STATIC_CHECK_EQ(q1[2], 3.0f);
        VX_STATIC_CHECK_EQ(q1[3], 4.0f);

        quat q2(0.0f, 0.0f, 0.0f, 0.0f);
        VX_CHECK(is_zero_approx(q2));

        q2[0] = 1.0f;
        VX_CHECK_EQ(q2[0], 1.0f);
        q2[1] = 2.0f;
        VX_CHECK_EQ(q2[1], 2.0f);
        q2[2] = 3.0f;
        VX_CHECK_EQ(q2[2], 3.0f);
        q2[3] = 4.0f;
        VX_CHECK_EQ(q2[3], 4.0f);
    }

    VX_SECTION("conversion")
    {
        constexpr quat q(1.0f, 2.0f, 3.0f, 4.0f);
        constexpr vec4i v = q.operator vx::math::vec4i();
        VX_STATIC_CHECK_EQ(v, vec4i(2, 3, 4, 1));
    }

    VX_SECTION("comparison")
    {
        constexpr quat q(1.0f, 2.0f, 3.0f, 4.0f);

        VX_STATIC_CHECK(q == q);
        VX_STATIC_CHECK(q != quat(1.0f, -2.0f, 3.0f, -4.0f));
    }

    VX_SECTION("unary const operators")
    {
        VX_STATIC_CHECK(+quat(1.0f, 2.0f, 3.0f, 4.0f) == quat(1.0f, 2.0f, 3.0f, 4.0f));
        VX_STATIC_CHECK(+quat(1.0f, -2.0f, 3.0f, -4.0f) == quat(1.0f, -2.0f, 3.0f, -4.0f));

        VX_STATIC_CHECK(-quat(1.0f, 2.0f, 3.0f, 4.0f) == quat(-1.0f, -2.0f, -3.0f, -4.0f));
        VX_STATIC_CHECK(-quat(-1.0f, -2.0f, -3.0f, -4.0f) == quat(1.0f, 2.0f, 3.0f, 4.0f));
    }

    VX_SECTION("binary arithmetic operators")
    {
        constexpr quat q(1.0f, 2.0f, 3.0f, 4.0f);

        // addition
        {
            VX_STATIC_CHECK_EQ(q + quat(5.0f, 6.0f, 7.0f, 8.0f), quat(6.0f, 8.0f, 10.0f, 12.0f));
        }

        // subtraction
        {
            VX_STATIC_CHECK_EQ(quat(6.0f, 8.0f, 10.0f, 12.0f) - quat(5.0f, 6.0f, 7.0f, 8.0f), q);
        }

        // multiplication
        {
            VX_STATIC_CHECK_EQ(q * 5.0f, quat(5.0f, 10.0f, 15.0f, 20.0f));
            VX_STATIC_CHECK_EQ(5.0f * q, quat(5.0f, 10.0f, 15.0f, 20.0f));

            constexpr quat a(1.0f, 2.0f, 3.0f, 4.0f);
            constexpr quat b(5.0f, 6.0f, 7.0f, 8.0f);
            constexpr quat expected(-60.0f, 12.0f, 30.0f, 24.0f);

            VX_STATIC_CHECK_EQ(a * b, expected);
        }

        // division
        {
            VX_STATIC_CHECK_EQ(quat(5.0f, 10.0f, 15.0f, 20.0f) / 5.0f, q);
        }
    }

    VX_SECTION("unary arithmetic operators")
    {
        constexpr quat q(1.0f, 2.0f, 3.0f, 4.0f);

        // addition
        {
            quat q1(5.0f, 6.0f, 7.0f, 8.0f);
            q1 += q;
            VX_CHECK_EQ(q1, quat(6.0f, 8.0f, 10.0f, 12.0f));
        }

        // subtraction
        {
            quat q1(6.0f, 8.0f, 10.0f, 12.0f);
            q1 -= quat(5.0f, 6.0f, 7.0f, 8.0f);
            VX_CHECK_EQ(q1, q);
        }

        // multiplication
        {
            quat q1 = q;
            q1 *= 5.0f;
            VX_CHECK_EQ(q1, quat(5.0f, 10.0f, 15.0f, 20.0f));

            constexpr quat a(1.0f, 2.0f, 3.0f, 4.0f);
            constexpr quat b(5.0f, 6.0f, 7.0f, 8.0f);
            constexpr quat expected(-60.0f, 12.0f, 30.0f, 24.0f);

            q1 = a;
            VX_CHECK(equal_approx(q1, a));
            q1 *= b;
            VX_CHECK_EQ(q1, expected);
        }

        // division
        {
            quat q1(5.0f, 10.0f, 15.0f, 20.0f);
            q1 /= 5.0f;
            VX_CHECK_EQ(q1, q);
        }
    }

    VX_SECTION("vector")
    {
        constexpr quat q(1.0f, 2.0f, 3.0f, 4.0f);
        constexpr vec3 v = q.vector();

        VX_STATIC_CHECK_EQ(v.x, q.x);
        VX_STATIC_CHECK_EQ(v.y, q.y);
        VX_STATIC_CHECK_EQ(v.z, q.z);
    }

    VX_SECTION("static constructors")
    {
        // identity
        {
            constexpr quat i = quat::identity();
            VX_STATIC_CHECK_EQ(i[0], 1.0f);
            VX_STATIC_CHECK_EQ(i[1], 0.0f);
            VX_STATIC_CHECK_EQ(i[2], 0.0f);
            VX_STATIC_CHECK_EQ(i[3], 0.0f);
        }

        // zero
        {
            constexpr quat i = quat::zero();
            VX_STATIC_CHECK_EQ(i[0], 0.0f);
            VX_STATIC_CHECK_EQ(i[1], 0.0f);
            VX_STATIC_CHECK_EQ(i[2], 0.0f);
            VX_STATIC_CHECK_EQ(i[3], 0.0f);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}