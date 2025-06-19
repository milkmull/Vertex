#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/quaternion.hpp"
#include "vertex/math/core/functions/euler_angles.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

template <typename T>
VX_FORCE_INLINE constexpr bool equivalent_euler_angles(
    const vec<3, T>& a,
    const vec<3, T>& b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equivalent_angle(a.x, b.x, epsilon)
        && equivalent_angle(a.y, b.y, epsilon)
        && equivalent_angle(a.z, b.z, epsilon);
}

///////////////////////////////////////////////////////////////////////////////
// quaternion
///////////////////////////////////////////////////////////////////////////////

static bool quat_euler_test(
    const euler_order order,
    const vec3& angles,
    const quat& q,
    const f32 epsilon = constants<f32>::epsilon)
{
    switch (order)
    {
        case euler_order::XYZ:
        {
            const quat q2 = quat_from_euler_angles_xyz(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_xyz(q2);
            if (!equivalent_euler_angles(angles, angles2, epsilon))
            {
                return false;
            }

            const quat q3 = quat_from_euler_angles_xyz(angles2.x, angles2.y, angles2.z);
            if (!equivalent_rotation(q2, q3))
            {
                return false;
            }

            break;
        }
        case euler_order::YXZ:
        {
            const quat q2 = quat_from_euler_angles_yxz(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_yxz(q2);
            if (!equivalent_euler_angles(angles, angles2, epsilon))
            {
                return false;
            }

            const quat q3 = quat_from_euler_angles_yxz(angles2.x, angles2.y, angles2.z);
            if (!equivalent_rotation(q2, q3))
            {
                return false;
            }

            break;
        }
        case euler_order::ZXY:
        {
            const quat q2 = quat_from_euler_angles_zxy(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_zxy(q2);
            if (!equivalent_euler_angles(angles, angles2, epsilon))
            {
                return false;
            }

            const quat q3 = quat_from_euler_angles_zxy(angles2.x, angles2.y, angles2.z);
            if (!equivalent_rotation(q2, q3))
            {
                return false;
            }

            break;
        }
        case euler_order::ZYX:
        {
            const quat q2 = quat_from_euler_angles_zyx(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_zyx(q2);
            if (!equivalent_euler_angles(angles, angles2, epsilon))
            {
                return false;
            }

            const quat q3 = quat_from_euler_angles_zyx(angles2.x, angles2.y, angles2.z);
            if (!equivalent_rotation(q2, q3))
            {
                return false;
            }

            break;
        }
        case euler_order::YZX:
        {
            const quat q2 = quat_from_euler_angles_yzx(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_yzx(q2);
            if (!equivalent_euler_angles(angles, angles2, epsilon))
            {
                return false;
            }

            const quat q3 = quat_from_euler_angles_yzx(angles2.x, angles2.y, angles2.z);
            if (!equivalent_rotation(q2, q3))
            {
                return false;
            }

            break;
        }
        case euler_order::XZY:
        {
            const quat q2 = quat_from_euler_angles_xzy(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_xzy(q2);
            if (!equivalent_euler_angles(angles, angles2, epsilon))
            {
                return false;
            }

            const quat q3 = quat_from_euler_angles_xzy(angles2.x, angles2.y, angles2.z);
            if (!equivalent_rotation(q2, q3))
            {
                return false;
            }

            break;
        }
    }

    return true;
}

VX_TEST_CASE(test_quat_euler_angles_conversion)
{
    // identity (zero rotation)
    {
        const vec3 angles(0.0f, 0.0f, 0.0f);

        VX_CHECK(quat_euler_test(euler_order::XYZ, angles, quat::identity()));
        VX_CHECK(quat_euler_test(euler_order::YXZ, angles, quat::identity()));
        VX_CHECK(quat_euler_test(euler_order::ZXY, angles, quat::identity()));
        VX_CHECK(quat_euler_test(euler_order::ZYX, angles, quat::identity()));
        VX_CHECK(quat_euler_test(euler_order::YZX, angles, quat::identity()));
        VX_CHECK(quat_euler_test(euler_order::XZY, angles, quat::identity()));
    }
    
    // symmetric positive rotation
    {
        const vec3 angles(0.5f, 0.5f, 0.5f);

        VX_CHECK(quat_euler_test(euler_order::XYZ, angles, quat(0.894463241f, 0.291566581f, 0.172954798f, 0.291566581f)));
        VX_CHECK(quat_euler_test(euler_order::YXZ, angles, quat(0.924749792f, 0.291566581f, 0.172954798f, 0.172954798f)));
        //VX_CHECK(quat_euler_test(euler_order::ZXY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZYX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::YZX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::XZY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
    }
    
    // symmetric negative rotation
    {
        const vec3 angles(-0.5f, -0.5f, -0.5f);

        VX_CHECK(quat_euler_test(euler_order::XYZ, angles, quat(0.924749792f, -0.172954798f, -0.291566581f, -0.172954798f)));
        VX_CHECK(quat_euler_test(euler_order::YXZ, angles, quat(0.894463241f, -0.172954798f, -0.291566581f, -0.291566581f)));
        //VX_CHECK(quat_euler_test(euler_order::ZXY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZYX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::YZX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::XZY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
    }
    
    // 90 about X
    {
        const vec3 angles(constants<f32>::half_pi, 0.0f, 0.0f);

        VX_CHECK(quat_euler_test(euler_order::XYZ, angles, quat(0.707106769f, 0.707106769f, 0.0f, 0.0f)));
        VX_CHECK(quat_euler_test(euler_order::YXZ, angles, quat(0.707106769f, 0.707106769f, 0.0f, 0.0f), 1e-3f)); // gimbal lock
        //VX_CHECK(quat_euler_test(euler_order::ZXY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZYX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::YZX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::XZY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
    }
    
    // 180 about Y
    {
        const vec3 angles(0.0f, constants<f32>::pi, 0.0f);

        VX_CHECK(quat_euler_test(euler_order::XYZ, angles, quat(0.0f, 0.0f, 1.0f, 0.0f)));
        VX_CHECK(quat_euler_test(euler_order::YXZ, angles, quat(0.0f, 1.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZXY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZYX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::YZX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::XZY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
    }

    // 180 about Z
    {
        const vec3 angles(0.0f, 0.0f, constants<f32>::pi);

        VX_CHECK(quat_euler_test(euler_order::XYZ, angles, quat(0.0f, 0.0f, 0.0f, 1.0f)));
        VX_CHECK(quat_euler_test(euler_order::YXZ, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZXY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZYX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::YZX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::XZY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
    }

    // This test handles the special case of a 180-degree rotation around the mid axis (pitch = pi).
    // Euler angles (0, pi, 0) produce a valid quaternion, but due to the non-uniqueness
    // of Euler angles, the reverse conversion may yield different angles, which represents the same rotation.
    // This test confirms that both Euler sets produce the same quaternion and thus the same rotation.

    // 180 about middle axis
    {
        const vec3 angles(0.0f, constants<f32>::pi, 0.0f);

        VX_CHECK(quat_euler_test(euler_order::XYZ, angles, quat(0.0f, 0.0f, 1.0f, 0.0f)));
        VX_CHECK(quat_euler_test(euler_order::YXZ, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZXY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZYX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::YZX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::XZY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
    }
    
    // near gimbal lock: north pole
    {
        const vec3 angles(constants<f32>::half_pi, constants<f32>::half_pi, 0.0f);

        VX_CHECK(quat_euler_test(euler_order::XYZ, angles, quat(0.5f, 0.5f, 0.5f, 0.5f)));
        VX_CHECK(quat_euler_test(euler_order::YXZ, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZXY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZYX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::YZX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::XZY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
    }
    
    // near gimbal lock: south pole
    {
        const vec3 angles(-constants<f32>::half_pi, constants<f32>::half_pi, 0.0f);

        VX_CHECK(quat_euler_test(euler_order::XYZ, angles, quat(0.5f, -0.5f, 0.5f, -0.5f)));
        VX_CHECK(quat_euler_test(euler_order::YXZ, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZXY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::ZYX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::YZX, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
        //VX_CHECK(quat_euler_test(euler_order::XZY, angles, quat(0.0f, 0.0f, 0.0f, 0.0f)));
    }

    // arbitrary normalized quaternion round-trip
    {
        const quat q = normalize(quat(0.3f, 0.5f, 0.1f, 0.8f));

        // xyz
        {
            const vec3 angles = extract_euler_angles_xyz(q);
            const quat q2 = quat_from_euler_angles_xyz(angles.x, angles.y, angles.z);
            VX_CHECK(equivalent_rotation(q, q2));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// matrix
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat3_euler_xyz_conversion)
{
    // Identity rotation
    {
        const vec3 angles(0.0f, 0.0f, 0.0f);
        const mat3 m = mat3_from_euler_angles_xyz(angles.x, angles.y, angles.z);
        VX_CHECK(is_identity_approx(m));

        const vec3 out = extract_euler_angles_xyz(m);
        VX_CHECK(equivalent_euler_angles(angles, out));
    }

    // 90 deg rotation about X
    {
        const vec3 angles(constants<f32>::half_pi, 0.0f, 0.0f);
        const mat3 m = mat3_from_euler_angles_xyz(angles.x, angles.y, angles.z);

        const mat3 expected = mat3(
            1, 0, 0,
            0, 0, 1,
            0, -1, 0
        );
        VX_CHECK_EQ(m, expected);

        const vec3 out = extract_euler_angles_xyz(m);
        VX_CHECK(equivalent_euler_angles(angles, out));
    }

    // This test handles the special case of a 180-degree rotation around the Y axis (pitch = pi).
    // Euler angles (0, pi, 0) produce a valid matrix, but due to the non-uniqueness
    // of Euler angles, the reverse conversion may yield (-pi, 0, -pi), which represents the same rotation.
    // This test confirms that both Euler sets produce the same quaternion and thus the same rotation.

    // 90 deg rotation about Y
    {
        const vec3 angles(0.0f, constants<f32>::half_pi, 0.0f);
        const mat3 m = mat3_from_euler_angles_xyz(angles.x, angles.y, angles.z);

        const mat3 expected = mat3(
            0, 0, -1,
            0, 1, 0,
            1, 0, 0
        );
        VX_CHECK_EQ(m, expected);

        const vec3 out = extract_euler_angles_xyz(m);
        VX_CHECK(equivalent_euler_angles(angles, out));
    }

    // 90 deg rotation about Z
    {
        const vec3 angles(0.0f, 0.0f, constants<f32>::half_pi);
        const mat3 m = mat3_from_euler_angles_xyz(angles.x, angles.y, angles.z);

        const mat3 expected = mat3(
            0, 1, 0,
            -1, 0, 0,
            0, 0, 1
        );
        VX_CHECK_EQ(m, expected);

        const vec3 out = extract_euler_angles_xyz(m);
        VX_CHECK(equivalent_euler_angles(angles, out));
    }

    // 180 deg pitch (gimbal edge case)
    {
        const vec3 angles(0.0f, constants<f32>::pi, 0.0f);
        const mat3 m = mat3_from_euler_angles_xyz(angles.x, angles.y, angles.z);

        const mat3 expected = mat3(
            -1, 0, 0,
            0, 1, 0,
            0, 0, -1
        );
        VX_CHECK_EQ(m, expected);

        const vec3 out = extract_euler_angles_xyz(m);
        const vec3 alt(-constants<f32>::pi, 0.0f, -constants<f32>::pi);
        VX_CHECK(equivalent_euler_angles(alt, out, 1e-3f));
    }

    // random non-singular rotation
    {
        const vec3 angles(0.5f, 0.5f, 0.5f);
        const mat3 m = mat3_from_euler_angles_xyz(angles.x, angles.y, angles.z);

        const vec3 out = extract_euler_angles_xyz(m);
        VX_CHECK(equivalent_euler_angles(angles, out));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}