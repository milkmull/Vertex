#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/quaternion.hpp"
#include "vertex/math/core/functions/rotation_cast.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_quat_mat3_cast)
{
    // identity rotation
    {
        const quat q = quat::identity();
        const mat3 m = rotation_cast<mat3>(q);
        VX_CHECK(is_identity_approx(m));

        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // 90 deg rotation about X
    {
        const quat q = axis_angle(vec3(1, 0, 0), constants<f32>::half_pi);
        const mat3 m = rotation_cast<mat3>(q);
    
        const mat3 expected = mat3(
            1, 0, 0,
            0, 0, 1,
            0, -1, 0
        );
        VX_CHECK_EQ(m, expected);
    
        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // 90 deg rotation about Y
    {
        const quat q = axis_angle(vec3(0, 1, 0), constants<f32>::half_pi);
        const mat3 m = rotation_cast<mat3>(q);

        const mat3 expected = mat3(
            0, 0, -1,
            0, 1, 0,
            1, 0, 0
        );
        VX_CHECK_EQ(m, expected);

        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // 90 deg rotation about Z
    {
        const quat q = axis_angle(vec3(0, 0, 1), constants<f32>::half_pi);
        const mat3 m = rotation_cast<mat3>(q);

        const mat3 expected = mat3(
            0, 1, 0,
            -1, 0, 0,
            0, 0, 1
        );
        VX_CHECK_EQ(m, expected);

        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // 180 deg rotation about arbitrary axis (e.g. Y)
    {
        const quat q = axis_angle(vec3(0, 1, 0), constants<f32>::pi);
        const mat3 m = rotation_cast<mat3>(q);

        const mat3 expected = mat3(
            -1, 0, 0,
            0, 1, 0,
            0, 0, -1
        );
        VX_CHECK_EQ(m, expected);

        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // random-ish composite rotation (0.5, 0.5, 0.5) Euler XYZ
    {
        const quat q(0.894463241f, 0.291566581f, 0.172954798f, 0.291566581f); // euler angles xyz = (0.5, 0.5, 0.5)
        const mat3 m = rotation_cast<mat3>(q);
    
        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_quat_mat4_cast)
{
    // identity rotation
    {
        const quat q = quat::identity();
        const mat4 m = rotation_cast<mat4>(q);
        VX_CHECK(is_identity_approx(m));

        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // 90 deg rotation about X
    {
        const quat q = axis_angle(vec3(1, 0, 0), constants<f32>::half_pi);
        const mat4 m = rotation_cast<mat4>(q);

        const mat4 expected = mat4(
            1, 0, 0, 0,
            0, 0, 1, 0,
            0, -1, 0, 0,
            0, 0, 0, 1
        );
        VX_CHECK_EQ(m, expected);

        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // 90 deg rotation about Y
    {
        const quat q = axis_angle(vec3(0, 1, 0), constants<f32>::half_pi);
        const mat4 m = rotation_cast<mat4>(q);

        const mat4 expected = mat4(
            0, 0, -1, 0,
            0, 1, 0, 0,
            1, 0, 0, 0,
            0, 0, 0, 1
        );
        VX_CHECK_EQ(m, expected);

        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // 90 deg rotation about Z
    {
        const quat q = axis_angle(vec3(0, 0, 1), constants<f32>::half_pi);
        const mat4 m = rotation_cast<mat4>(q);

        const mat4 expected = mat4(
            0, 1, 0, 0,
            -1, 0, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
        VX_CHECK_EQ(m, expected);

        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // 180 deg rotation about arbitrary axis (e.g. Y)
    {
        const quat q = axis_angle(vec3(0, 1, 0), constants<f32>::pi);
        const mat4 m = rotation_cast<mat4>(q);

        const mat4 expected = mat4(
            -1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, -1, 0,
            0, 0, 0, 1
        );
        VX_CHECK_EQ(m, expected);

        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // random-ish composite rotation (0.5, 0.5, 0.5) Euler XYZ
    {
        const quat q(0.894463241f, 0.291566581f, 0.172954798f, 0.291566581f); // euler angles xyz = (0.5, 0.5, 0.5)
        const mat4 m = rotation_cast<mat4>(q);

        const quat q2 = rotation_cast<quat>(m);
        VX_CHECK(equivalent_rotation(q, q2));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat_cast)
{
    const mat3 m3(
        1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f,
        7.0f, 8.0f, 9.0f
    );

    const mat4 m4 = rotation_cast<mat4>(m3);

    const mat4 expected(
        1.0f, 2.0f, 3.0f, 0.0f,
        4.0f, 5.0f, 6.0f, 0.0f,
        7.0f, 8.0f, 9.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    VX_CHECK_EQ(m4, expected);
    VX_CHECK_EQ(rotation_cast<mat3>(m4), m3);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_quat_cast)
{
    const quatf qf(0.0f, 1.0f, 2.0f, 3.0f);
    const quatd qd = rotation_cast<quatd>(qf);

    VX_CHECK_EQ(qd, quatd(0.0, 1.0, 2.0, 3.0));
    VX_CHECK_EQ(rotation_cast<quatf>(qd), qf);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat3_cast)
{
    const mat3f m3f(
        1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f,
        7.0f, 8.0f, 9.0f
    );

    const mat3d m3d = rotation_cast<mat3d>(m3f);

    const mat3d expected(
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    );

    VX_CHECK_EQ(m3d, expected);
    VX_CHECK_EQ(rotation_cast<mat3f>(m3d), m3f);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat4_cast)
{
    const mat4f m4f(
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    );

    const mat4d m4d = rotation_cast<mat4d>(m4f);

    const mat4d expected(
        1.0, 2.0, 3.0, 4.0,
        5.0, 6.0, 7.0, 8.0,
        9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0
    );

    VX_CHECK_EQ(m4d, expected);
    VX_CHECK_EQ(rotation_cast<mat4f>(m4d), m4f);
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}