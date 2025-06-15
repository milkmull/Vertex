#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/quaternion.hpp"
#include "vertex/math/core/functions/rotation_convert.hpp"
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

VX_TEST_CASE(test_quat_euler_xyz_conversion)
{
    // identity (zero rotation)
    {
        const vec3 xyz(0.0f, 0.0f, 0.0f);
        const quat q = euler_xyz_to_quat(xyz.x, xyz.y, xyz.z);
        VX_CHECK(is_identity_approx(q));
    
        const vec3 angles = quat_to_euler_xyz(q);
        VX_CHECK(equivalent_euler_angles(angles, xyz));
    
        const quat q2 = euler_xyz_to_quat(angles.x, angles.y, angles.z);
        VX_CHECK(equivalent_rotation(q, q2));
    }
    
    // symmetric positive rotation
    {
        const vec3 xyz(0.5f, 0.5f, 0.5f);
        const quat q = euler_xyz_to_quat(xyz.x, xyz.y, xyz.z);
        const quat expected(0.894463241f, 0.291566581f, 0.172954798f, 0.291566581f);
        VX_CHECK_EQ(q, expected);
    
        const vec3 angles = quat_to_euler_xyz(q);
        VX_CHECK(equivalent_euler_angles(angles, xyz));
    
        const quat q2 = euler_xyz_to_quat(angles.x, angles.y, angles.z);
        VX_CHECK(equivalent_rotation(q, q2));
    }
    
    // symmetric negative rotation
    {
        const vec3 xyz(-0.5f, -0.5f, -0.5f);
        const quat q = euler_xyz_to_quat(xyz.x, xyz.y, xyz.z);
        const quat expected(0.924749792f, -0.172954798f, -0.291566581f, -0.172954798f);
        VX_CHECK_EQ(q, expected);
    
        const vec3 angles = quat_to_euler_xyz(q);
        VX_CHECK(equivalent_euler_angles(angles, xyz));
    
        const quat q2 = euler_xyz_to_quat(angles.x, angles.y, angles.z);
        VX_CHECK(equivalent_rotation(q, q2));
    }
    
    // roll = 90 deg
    {
        const vec3 xyz(constants<f32>::half_pi, 0.0f, 0.0f);
        const quat q = euler_xyz_to_quat(xyz.x, xyz.y, xyz.z);
        const quat expected(0.707106769f, 0.707106769f, 0.0f, 0.0f);
        VX_CHECK_EQ(q, expected);
    
        const vec3 angles = quat_to_euler_xyz(q);
        VX_CHECK(equivalent_euler_angles(angles, xyz));
    
        const quat q2 = euler_xyz_to_quat(angles.x, angles.y, angles.z);
        VX_CHECK(equivalent_rotation(q, q2));
    }
    
    // roll = 180 deg
    {
        const vec3 xyz(constants<f32>::pi, 0.0f, 0.0f);
        const quat q = euler_xyz_to_quat(xyz.x, xyz.y, xyz.z);
        const quat expected(0.0f, 1.0f, 0.0f, 0.0f);
        VX_CHECK_EQ(q, expected);
    
        const vec3 angles = quat_to_euler_xyz(q);
        VX_CHECK(equivalent_euler_angles(angles, xyz));
    
        const quat q2 = euler_xyz_to_quat(angles.x, angles.y, angles.z);
        VX_CHECK(equivalent_rotation(q, q2));
    }
    
    // near gimbal lock: north pole
    {
        const vec3 xyz(constants<f32>::half_pi, constants<f32>::half_pi, 0.0f);
        const quat q = euler_xyz_to_quat(xyz.x, xyz.y, xyz.z);
        const quat expected(0.5f, 0.5f, 0.5f, 0.5f);
        VX_CHECK_EQ(q, expected);
    
        const vec3 angles = quat_to_euler_xyz(q);
        VX_CHECK(equivalent_euler_angles(angles, xyz, 1e-3f));
    
        const quat q2 = euler_xyz_to_quat(angles.x, angles.y, angles.z);
        VX_CHECK(equivalent_rotation(q, q2));
    }
    
    // near gimbal lock: south pole
    {
        const vec3 xyz(-constants<f32>::half_pi, constants<f32>::half_pi, 0.0f);
        const quat q = euler_xyz_to_quat(xyz.x, xyz.y, xyz.z);
        const quat expected(0.5f, -0.5f, 0.5f, -0.5f);
        VX_CHECK_EQ(q, expected);
    
        const vec3 angles = quat_to_euler_xyz(q);
        VX_CHECK(equivalent_euler_angles(angles, xyz, 1e-3f));
    
        const quat q2 = euler_xyz_to_quat(angles.x, angles.y, angles.z);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // This test handles the special case of a 180-degree rotation around the Y axis (pitch = pi).
    // Euler angles (0, pi, 0) produce a valid quaternion (0, 0, 1, 0), but due to the non-uniqueness
    // of Euler angles, the reverse conversion may yield (-pi, 0, -pi), which represents the same rotation.
    // This test confirms that both Euler sets produce the same quaternion and thus the same rotation.

    // full rotation: pitch = 180 deg
    {
        const vec3 xyz(0.0f, constants<f32>::pi, 0.0f);
        const quat q = euler_xyz_to_quat(xyz.x, xyz.y, xyz.z);
        const quat expected(0.0f, 0.0f, 1.0f, 0.0f);
        VX_CHECK_EQ(q, expected);

        const vec3 angles = quat_to_euler_xyz(q);
        const vec3 flipped(-constants<f32>::pi, 0.0f, -constants<f32>::pi);
        VX_CHECK_EQ(angles, flipped);

        const quat q2 = euler_xyz_to_quat(angles.x, angles.y, angles.z);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // full rotation: yaw = 180 deg
    {
        const vec3 xyz(0.0f, 0.0f, constants<f32>::pi);
        const quat q = euler_xyz_to_quat(xyz.x, xyz.y, xyz.z);
        const quat expected(0.0f, 0.0f, 0.0f, 1.0f);
        VX_CHECK_EQ(q, expected);

        const vec3 angles = quat_to_euler_xyz(q);
        const vec3 flipped(0.0f, 0.0f, -constants<f32>::pi);
        VX_CHECK(equivalent_euler_angles(angles, flipped));

        const quat q2 = euler_xyz_to_quat(angles.x, angles.y, angles.z);
        VX_CHECK(equivalent_rotation(q, q2));
    }

    // arbitrary normalized quaternion round-trip
    {
        const quat q = normalize(quat(0.3f, 0.5f, 0.1f, 0.8f));
        const vec3 angles = quat_to_euler_xyz(q);
        const quat q2 = euler_xyz_to_quat(angles.x, angles.y, angles.z);
        VX_CHECK(equivalent_rotation(q, q2));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}