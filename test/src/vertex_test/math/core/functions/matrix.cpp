#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/matrix.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_transpose)
{
    VX_SECTION("mat2x2")
    {
        const mat2x2 m(
            1.0f, 2.0f,
            3.0f, 4.0f
        );

        const mat2x2 t(
            1.0f, 3.0f,
            2.0f, 4.0f
        );

        VX_CHECK_EQ(transpose(m), t);
    }

    VX_SECTION("mat2x3")
    {
        const mat2x3 m(
            1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f
        );

        const mat3x2 t(
            1.0f, 4.0f,
            2.0f, 5.0f,
            3.0f, 6.0f
        );

        VX_CHECK_EQ(transpose(m), t);
    }

    VX_SECTION("mat2x4")
    {
        const mat2x4 m(
            1.0f, 2.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f
        );

        const mat4x2 t(
            1.0f, 5.0f,
            2.0f, 6.0f,
            3.0f, 7.0f,
            4.0f, 8.0f
        );

        VX_CHECK_EQ(transpose(m), t);
    }

    VX_SECTION("mat3x2")
    {
        const mat3x2 m(
            1.0f, 2.0f,
            3.0f, 4.0f,
            5.0f, 6.0f
        );

        const mat2x3 t(
            1.0f, 3.0f, 5.0f,
            2.0f, 4.0f, 6.0f
        );

        VX_CHECK_EQ(transpose(m), t);
    }

    VX_SECTION("mat3x3")
    {
        const mat3x3 m(
            1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f,
            7.0f, 8.0f, 9.0f
        );

        const mat3x3 t(
            1.0f, 4.0f, 7.0f,
            2.0f, 5.0f, 8.0f,
            3.0f, 6.0f, 9.0f
        );

        VX_CHECK_EQ(transpose(m), t);
    }

    VX_SECTION("mat3x4")
    {
        const mat3x4 m(
            1.0f, 2.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f,
            9.0f, 10.0f, 11.0f, 12.0f
        );

        const mat4x3 t(
            1.0f, 5.0f, 9.0f,
            2.0f, 6.0f, 10.0f,
            3.0f, 7.0f, 11.0f,
            4.0f, 8.0f, 12.0f
        );

        VX_CHECK_EQ(transpose(m), t);
    }

    VX_SECTION("mat4x2")
    {
        const mat4x2 m(
            1.0f, 2.0f,
            3.0f, 4.0f,
            5.0f, 6.0f,
            7.0f, 8.0f
        );

        const mat2x4 t(
            1.0f, 3.0f, 5.0f, 7.0f,
            2.0f, 4.0f, 6.0f, 8.0f
        );

        VX_CHECK_EQ(transpose(m), t);
    }

    VX_SECTION("mat4x3")
    {
        const mat4x3 m(
            1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f,
            7.0f, 8.0f, 9.0f,
            10.0f, 11.0f, 12.0f
        );

        const mat3x4 t(
            1.0f, 4.0f, 7.0f, 10.0f,
            2.0f, 5.0f, 8.0f, 11.0f,
            3.0f, 6.0f, 9.0f, 12.0f
        );

        VX_CHECK_EQ(transpose(m), t);
    }

    VX_SECTION("mat4x4")
    {
        const mat4x4 m(
            1.0f, 2.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f,
            9.0f, 10.0f, 11.0f, 12.0f,
            13.0f, 14.0f, 15.0f, 16.0f
        );

        const mat4x4 t(
            1.0f, 5.0f, 9.0f, 13.0f,
            2.0f, 6.0f, 10.0f, 14.0f,
            3.0f, 7.0f, 11.0f, 15.0f,
            4.0f, 8.0f, 12.0f, 16.0f
        );

        VX_CHECK_EQ(transpose(m), t);
    }
}

///////////////////////////////////////////////////////////////////////////////

// // http://www.euclideanspace.com/maths/algebra/matrix/functions/determinant/twoD/index.htm

VX_TEST_CASE(test_determinant)
{
    VX_SECTION("mat2x2")
    {
        VX_CHECK_EQ(determinant(mat2x2::identity()), 1.0f);

        const mat2x2 m1(
            1.0f, 2.0f,
            3.0f, 4.0f
        );

        VX_CHECK_EQ(determinant(m1), -2.0f);
    }

    VX_SECTION("mat3x3")
    {
        VX_CHECK_EQ(determinant(mat3x3::identity()), 1.0f);

        const mat3x3 m1(
            2.0f, 3.0f, 4.0f,
            5.0f, 13.0f, 7.0f,
            8.0f, 9.0f, 11.0f
        );

        VX_CHECK_EQ(determinant(m1), -73.0f);
    }

    VX_SECTION("mat4x4")
    {
        VX_CHECK_EQ(determinant(mat4x4::identity()), 1.0f);

        const mat4x4 m1(
             2.0f,  3.0f,   4.0f,   5.0f,
            -1.0f, -21.0f, -3.0f,  -4.0f,
             6.0f,  7.0f,   8.0f,   10.0f,
            -8.0f, -9.0f,  -10.0f, -12.0f
        );

        VX_CHECK_EQ(determinant(m1), 76.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////

// http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/twoD/index.htm

VX_TEST_CASE(test_inverse)
{
    VX_SECTION("mat2x2")
    {
        VX_CHECK_EQ(inverse(mat2x2::identity()), mat2x2::identity());

        const mat2x2 m(
            1.0f, 2.0f,
            3.0f, 4.0f
        );

        const mat2x2 mi(
            -2.0f, 1.0f,
            1.5f, -0.5f
        );

        VX_CHECK_EQ(inverse(m), mi);
        VX_CHECK(is_identity_approx(inverse(m) * m));
    }

    VX_SECTION("mat3x3")
    {
        VX_CHECK_EQ(inverse(mat3x3::identity()), mat3x3::identity());

        const mat3x3 m(
            2.0f, 3.0f,  4.0f,
            5.0f, 13.0f, 7.0f,
            8.0f, 9.0f,  11.0f
        );

        const mat3x3 mi(
            -1.0958904f, -0.0410958f,  0.4246575f,
            -0.0136986f,  0.1369863f, -0.0821917f,
             0.8082191f, -0.0821917f, -0.1506849f
        );

        VX_CHECK_EQ(inverse(m), mi);
        VX_CHECK(is_identity_approx(inverse(m) * m));
    }

    VX_SECTION("mat4x4")
    {
        VX_CHECK_EQ(inverse(mat4x4::identity()), mat4x4::identity());

        const mat4x4 m(
             2.0f,  3.0f,   4.0f,   5.0f,
            -1.0f, -21.0f, -3.0f,  -4.0f,
             6.0f,  7.0f,   8.0f,   10.0f,
            -8.0f, -9.0f,  -10.0f, -12.0f
        );

        const mat4x4 mi(
            -0.9736842f,  0.0263157f,  0.5131578f,  0.0131578f,
            -0.0526315f, -0.0526315f, -0.0263157f, -0.0263157f,
             2.0263157f,  0.0263157f, -3.9868421f, -2.4868421f,
            -1.0000000f,  0.0000000f,  3.0000000f,  2.0000000f
        );

        VX_CHECK_EQ(inverse(m), mi);
        VX_CHECK(is_identity_approx(inverse(m) * m, 1e-5f));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_matrix_comp_mult)
{
    VX_SECTION("mat2x2")
    {
        const mat2x2 m(0.0f, 1.0f, 2.0f, 3.0f);
        const mat2x2 r(0.0f, 1.0f, 4.0f, 9.0f);
        VX_CHECK_EQ(matrix_comp_mult(m, m), r);
    }

    VX_SECTION("mat2x3")
    {
        const mat2x3 m(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f);
        const mat2x3 r(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f);
        VX_CHECK_EQ(matrix_comp_mult(m, m), r);
    }

    VX_SECTION("mat2x4")
    {
        const mat2x4 m(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);
        const mat2x4 r(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f);
        VX_CHECK_EQ(matrix_comp_mult(m, m), r);
    }

    VX_SECTION("mat3x2")
    {
        const mat3x2 m(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f);
        const mat3x2 r(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f);
        VX_CHECK_EQ(matrix_comp_mult(m, m), r);
    }

    VX_SECTION("mat3x3")
    {
        const mat3x3 m(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
        const mat3x3 r(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f, 64.0f);
        VX_CHECK_EQ(matrix_comp_mult(m, m), r);
    }

    VX_SECTION("mat3x4")
    {
        const mat3x4 m(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f);
        const mat3x4 r(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f, 64.0f, 81.0f, 100.0f, 121.0f);
        VX_CHECK_EQ(matrix_comp_mult(m, m), r);
    }

    VX_SECTION("mat4x2")
    {
        const mat4x2 m(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);
        const mat4x2 r(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f);
        VX_CHECK_EQ(matrix_comp_mult(m, m), r);
    }

    VX_SECTION("mat4x3")
    {
        const mat4x3 m(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f);
        const mat4x3 r(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f, 64.0f, 81.0f, 100.0f, 121.0f);
        VX_CHECK_EQ(matrix_comp_mult(m, m), r);
    }

    VX_SECTION("mat4x4")
    {
        const mat4x4 m(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f);
        const mat4x4 r(0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f, 64.0f, 81.0f, 100.0f, 121.0f, 144.0f, 169.0f, 196.0f, 225.0f);
        VX_CHECK_EQ(matrix_comp_mult(m, m), r);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_outer_product)
{
    VX_SECTION("mat2x2")
    {
        const vec2 r(1.0f, 2.0f);
        const vec2 c(3.0f, 4.0f);
        const mat2x2 m(3.0f, 4.0f, 6.0f, 8.0f);

        VX_CHECK_EQ(outer_product(c, r), m);
    }

    VX_SECTION("mat2x3")
    {
        const vec2 r(1.0f, 2.0f);
        const vec3 c(3.0f, 4.0f, 5.0f);
        const mat2x3 m(3.0f, 4.0f, 5.0f, 6.0f, 8.0f, 10.0f);

        VX_CHECK_EQ(outer_product(c, r), m);
    }

    VX_SECTION("mat2x4")
    {
        const vec2 r(1.0f, 2.0f);
        const vec4 c(3.0f, 4.0f, 5.0f, 6.0f);
        const mat2x4 m(3.0f, 4.0f, 5.0f, 6.0f, 6.0f, 8.0f, 10.0f, 12.0f);

        VX_CHECK_EQ(outer_product(c, r), m);
    }

    VX_SECTION("mat3x2")
    {
        const vec3 r(1.0f, 2.0f, 3.0f);
        const vec2 c(4.0f, 5.0f);
        const mat3x2 m(4.0f, 5.0f, 8.0f, 10.0f, 12.0f, 15.0f);

        VX_CHECK_EQ(outer_product(c, r), m);
    }

    VX_SECTION("mat3x3")
    {
        const vec3 r(1.0f, 2.0f, 3.0f);
        const vec3 c(4.0f, 5.0f, 6.0f);
        const mat3x3 m(4.0f, 5.0f, 6.0f, 8.0f, 10.0f, 12.0f, 12.0f, 15.0f, 18.0f);

        VX_CHECK_EQ(outer_product(c, r), m);
    }

    VX_SECTION("mat3x4")
    {
        const vec3 r(1.0f, 2.0f, 3.0f);
        const vec4 c(4.0f, 5.0f, 6.0f, 7.0f);
        const mat3x4 m(4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 10.0f, 12.0f, 14.0f, 12.0f, 15.0f, 18.0f, 21.0f);

        VX_CHECK_EQ(outer_product(c, r), m);
    }

    VX_SECTION("mat4x2")
    {
        const vec4 r(1.0f, 2.0f, 3.0f, 4.0f);
        const vec2 c(5.0f, 6.0f);
        const mat4x2 m(5.0f, 6.0f, 10.0f, 12.0f, 15.0f, 18.0f, 20.0f, 24.0f);

        VX_CHECK_EQ(outer_product(c, r), m);
    }

    VX_SECTION("mat4x3")
    {
        const vec4 r(1.0f, 2.0f, 3.0f, 4.0f);
        const vec3 c(5.0f, 6.0f, 7.0f);
        const mat4x3 m(5.0f, 6.0f, 7.0f, 10.0f, 12.0f, 14.0f, 15.0f, 18.0f, 21.0f, 20.0f, 24.0f, 28.0f);

        VX_CHECK_EQ(outer_product(c, r), m);
    }

    VX_SECTION("mat4x4")
    {
        const vec4 r(1.0f, 2.0f, 3.0f, 4.0f);
        const vec4 c(5.0f, 6.0f, 7.0f, 8.0f);
        const mat4x4 m(5.0f, 6.0f, 7.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 15.0f, 18.0f, 21.0f, 24.0f, 20.0f, 24.0f, 28.0f, 32.0f);

        VX_CHECK_EQ(outer_product(c, r), m);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}