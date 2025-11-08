#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/quaternion.hpp"
#include "vertex/math/core/functions/euler_angles.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))

template <typename T, VX_MATH_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T equivalent_angle(
    T a,
    T b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    // avoid case where a or b are super close to zero but negative, resulting in floor returning -1 instead of 0
    const T az = is_zero_approx(a, epsilon) ? static_cast<T>(0) : a;
    const T bz = is_zero_approx(b, epsilon) ? static_cast<T>(0) : b;

    return equal_approx(
        abs(mod(az, constants<T>::two_pi)),
        abs(mod(bz, constants<T>::two_pi)),
        epsilon
    );
}

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
    const vec3& extracted_angles,
    const f32 epsilon = constants<f32>::epsilon
)
{
    switch (order)
    {
        case euler_order::xyz:
        {
            const quat q2 = quat_from_euler_angles_xyz(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_xyz(q2);
            if (!equal_approx(extracted_angles, angles2, epsilon))
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
        case euler_order::xzy:
        {
            const quat q2 = quat_from_euler_angles_xzy(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_xzy(q2);
            if (!equivalent_euler_angles(extracted_angles, angles2, epsilon))
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
        case euler_order::yxz:
        {
            const quat q2 = quat_from_euler_angles_yxz(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_yxz(q2);
            if (!equivalent_euler_angles(extracted_angles, angles2, epsilon))
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
        case euler_order::yzx:
        {
            const quat q2 = quat_from_euler_angles_yzx(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_yzx(q2);
            if (!equivalent_euler_angles(extracted_angles, angles2, epsilon))
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
        case euler_order::zxy:
        {
            const quat q2 = quat_from_euler_angles_zxy(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_zxy(q2);
            if (!equivalent_euler_angles(extracted_angles, angles2, epsilon))
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
        case euler_order::zyx:
        {
            const quat q2 = quat_from_euler_angles_zyx(angles.x, angles.y, angles.z);
            if (!equal_approx(q, q2))
            {
                return false;
            }

            const vec3 angles2 = extract_euler_angles_zyx(q2);
            if (!equivalent_euler_angles(extracted_angles, angles2, epsilon))
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
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_quat_euler_angles_conversion)
{
    // identity (zero rotation)
    {
        const vec3 angles(0.0f, 0.0f, 0.0f);
        const quat q = quat::identity();

        VX_CHECK(quat_euler_test(euler_order::xyz, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::xzy, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::yxz, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::yzx, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::zxy, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::zyx, angles, q, angles));
    }
    
    {
        const f32 a = 0.894463241f;
        const f32 b = 0.291566581f;
        const f32 c = 0.172954798f;
        const f32 d = 0.924749792f;

        // symmetric positive rotation
        {
            const vec3 angles(0.5f, 0.5f, 0.5f);

            VX_CHECK(quat_euler_test(euler_order::xyz, angles, quat(a, b, c, b), angles));
            VX_CHECK(quat_euler_test(euler_order::xzy, angles, quat(d, c, c, b), angles));
            VX_CHECK(quat_euler_test(euler_order::yxz, angles, quat(d, b, c, c), angles));
            VX_CHECK(quat_euler_test(euler_order::yzx, angles, quat(a, b, b, c), angles));
            VX_CHECK(quat_euler_test(euler_order::zxy, angles, quat(a, c, b, b), angles));
            VX_CHECK(quat_euler_test(euler_order::zyx, angles, quat(d, c, b, c), angles));
        }

        // symmetric negative rotation
        {
            const vec3 angles(-0.5f, -0.5f, -0.5f);

            VX_CHECK(quat_euler_test(euler_order::xyz, angles, quat(d, -c, -b, -c), angles));
            VX_CHECK(quat_euler_test(euler_order::xzy, angles, quat(a, -b, -b, -c), angles));
            VX_CHECK(quat_euler_test(euler_order::yxz, angles, quat(a, -c, -b, -b), angles));
            VX_CHECK(quat_euler_test(euler_order::yzx, angles, quat(d, -c, -c, -b), angles));
            VX_CHECK(quat_euler_test(euler_order::zxy, angles, quat(d, -b, -c, -c), angles));
            VX_CHECK(quat_euler_test(euler_order::zyx, angles, quat(a, -b, -c, -b), angles));
        }
    }
    
    // 90 about X
    {
        const vec3 angles(constants<f32>::half_pi, 0.0f, 0.0f);
        const quat q(0.707106769f, 0.707106769f, 0.0f, 0.0f);

        VX_CHECK(quat_euler_test(euler_order::xyz, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::xzy, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::yxz, angles, q, angles, 1e-3f)); // gimbal lock
        VX_CHECK(quat_euler_test(euler_order::yzx, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::zxy, angles, q, angles, 1e-3f)); // gimbal lock
        VX_CHECK(quat_euler_test(euler_order::zyx, angles, q, angles));
    }

    // 90 about Y
    {
        const vec3 angles(0.0f, constants<f32>::half_pi, 0.0f);
        const quat q(0.707106769f, 0.0f, 0.707106769f, 0.0f);

        VX_CHECK(quat_euler_test(euler_order::xyz, angles, q, angles, 1e-3f)); // gimbal lock
        VX_CHECK(quat_euler_test(euler_order::xzy, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::yxz, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::yzx, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::zxy, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::zyx, angles, q, angles, 1e-3f)); // gimbal lock
    }

    // 90 about Z
    {
        const vec3 angles(0.0f, 0.0f, constants<f32>::half_pi);
        const quat q(0.707106769f, 0.0f, 0.0f, 0.707106769f);

        VX_CHECK(quat_euler_test(euler_order::xyz, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::xzy, angles, q, angles, 1e-3f)); // gimbal lock
        VX_CHECK(quat_euler_test(euler_order::yxz, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::yzx, angles, q, angles, 1e-3f)); // gimbal lock
        VX_CHECK(quat_euler_test(euler_order::zxy, angles, q, angles));
        VX_CHECK(quat_euler_test(euler_order::zyx, angles, q, angles));
    }

    // 180 about X
    {
        const vec3 angles(constants<f32>::pi, 0.0f, 0.0f);
        const quat q(0.0f, 1.0f, 0.0f, 0.0f);

        const vec3 x_angles(-constants<f32>::pi, 0.0f, 0.0f);
        const vec3 mid_angles(0.0f, -constants<f32>::pi, -constants<f32>::pi);

        VX_CHECK(quat_euler_test(euler_order::xyz, angles, q, x_angles));
        VX_CHECK(quat_euler_test(euler_order::xzy, angles, q, x_angles));
        VX_CHECK(quat_euler_test(euler_order::yxz, angles, q, mid_angles));
        VX_CHECK(quat_euler_test(euler_order::yzx, angles, q, x_angles));
        VX_CHECK(quat_euler_test(euler_order::zxy, angles, q, mid_angles));
        VX_CHECK(quat_euler_test(euler_order::zyx, angles, q, x_angles));
    }
    
    // 180 about Y
    {
        const vec3 angles(0.0f, constants<f32>::pi, 0.0f);
        const quat q(0.0f, 0.0f, 1.0f, 0.0f);

        const vec3 y_angles(0.0f, -constants<f32>::pi, 0.0f);
        const vec3 mid_angles(-constants<f32>::pi, 0.0f, -constants<f32>::pi);

        VX_CHECK(quat_euler_test(euler_order::xyz, angles, q, mid_angles));
        VX_CHECK(quat_euler_test(euler_order::xzy, angles, q, y_angles));
        VX_CHECK(quat_euler_test(euler_order::yxz, angles, q, y_angles));
        VX_CHECK(quat_euler_test(euler_order::yzx, angles, q, y_angles));
        VX_CHECK(quat_euler_test(euler_order::zxy, angles, q, y_angles));
        VX_CHECK(quat_euler_test(euler_order::zyx, angles, q, mid_angles));
    }

    // 180 about Z
    {
        const vec3 angles(0.0f, 0.0f, constants<f32>::pi);
        const quat q(0.0f, 0.0f, 0.0f, 1.0f);

        const vec3 z_angles(0.0f, 0.0f, -constants<f32>::pi);
        const vec3 mid_angles(-constants<f32>::pi, -constants<f32>::pi, 0.0f);

        VX_CHECK(quat_euler_test(euler_order::xyz, angles, q, z_angles));
        VX_CHECK(quat_euler_test(euler_order::xzy, angles, q, mid_angles));
        VX_CHECK(quat_euler_test(euler_order::yxz, angles, q, z_angles));
        VX_CHECK(quat_euler_test(euler_order::yzx, angles, q, mid_angles));
        VX_CHECK(quat_euler_test(euler_order::zxy, angles, q, z_angles));
        VX_CHECK(quat_euler_test(euler_order::zyx, angles, q, z_angles));
    }
}

///////////////////////////////////////////////////////////////////////////////
// matrix
///////////////////////////////////////////////////////////////////////////////

static bool mat_euler_test(
    const euler_order order,
    const vec3& angles,
    const mat3& m3,
    const vec3& extracted_angles,
    const f32 epsilon = constants<f32>::epsilon
)
{
    const mat4 m4(
        m3.columns[0].x, m3.columns[0].y, m3.columns[0].z, 0.0f,
        m3.columns[1].x, m3.columns[1].y, m3.columns[1].z, 0.0f,
        m3.columns[2].x, m3.columns[2].y, m3.columns[2].z, 0.0f,
        0.0f,            0.0f,            0.0f,            1.0f
    );

    switch (order)
    {
        case euler_order::xyz:
        {
            const mat3 m32 = mat3_from_euler_angles_xyz(angles.x, angles.y, angles.z);
            const mat4 m42 = mat4_from_euler_angles_xyz(angles.x, angles.y, angles.z);

            if (!equal_approx(m3, m32))
            {
                return false;
            }

            if (!equal_approx(m4, m42))
            {
                return false;
            }

            const vec3 angles32 = extract_euler_angles_xyz(m32);
            const vec3 angles42 = extract_euler_angles_xyz(m42);

            if (!equal_approx(extracted_angles, angles32, epsilon))
            {
                return false;
            }

            if (!equal_approx(extracted_angles, angles42, epsilon))
            {
                return false;
            }

            const mat3 m33 = mat3_from_euler_angles_xyz(angles32.x, angles32.y, angles32.z);
            const mat4 m43 = mat4_from_euler_angles_xyz(angles42.x, angles42.y, angles42.z);

            if (!equal_approx(m32, m33))
            {
                return false;
            }

            if (!equal_approx(m42, m43))
            {
                return false;
            }

            break;
        }
        case euler_order::xzy:
        {
            const mat3 m32 = mat3_from_euler_angles_xzy(angles.x, angles.y, angles.z);
            const mat4 m42 = mat4_from_euler_angles_xzy(angles.x, angles.y, angles.z);

            if (!equal_approx(m3, m32))
            {
                return false;
            }

            if (!equal_approx(m4, m42))
            {
                return false;
            }

            const vec3 angles32 = extract_euler_angles_xzy(m32);
            const vec3 angles42 = extract_euler_angles_xzy(m42);

            if (!equal_approx(extracted_angles, angles32, epsilon))
            {
                return false;
            }

            if (!equal_approx(extracted_angles, angles42, epsilon))
            {
                return false;
            }

            const mat3 m33 = mat3_from_euler_angles_xzy(angles32.x, angles32.y, angles32.z);
            const mat4 m43 = mat4_from_euler_angles_xzy(angles42.x, angles42.y, angles42.z);

            if (!equal_approx(m32, m33))
            {
                return false;
            }

            if (!equal_approx(m42, m43))
            {
                return false;
            }

            break;
        }
        case euler_order::yxz:
        {
            const mat3 m32 = mat3_from_euler_angles_yxz(angles.x, angles.y, angles.z);
            const mat4 m42 = mat4_from_euler_angles_yxz(angles.x, angles.y, angles.z);

            if (!equal_approx(m3, m32))
            {
                return false;
            }

            if (!equal_approx(m4, m42))
            {
                return false;
            }

            const vec3 angles32 = extract_euler_angles_yxz(m32);
            const vec3 angles42 = extract_euler_angles_yxz(m42);

            if (!equal_approx(extracted_angles, angles32, epsilon))
            {
                return false;
            }

            if (!equal_approx(extracted_angles, angles42, epsilon))
            {
                return false;
            }

            const mat3 m33 = mat3_from_euler_angles_yxz(angles32.x, angles32.y, angles32.z);
            const mat4 m43 = mat4_from_euler_angles_yxz(angles42.x, angles42.y, angles42.z);

            if (!equal_approx(m32, m33))
            {
                return false;
            }

            if (!equal_approx(m42, m43))
            {
                return false;
            }

            break;
        }
        case euler_order::yzx:
        {
            const mat3 m32 = mat3_from_euler_angles_yzx(angles.x, angles.y, angles.z);
            const mat4 m42 = mat4_from_euler_angles_yzx(angles.x, angles.y, angles.z);

            if (!equal_approx(m3, m32))
            {
                return false;
            }

            if (!equal_approx(m4, m42))
            {
                return false;
            }

            const vec3 angles32 = extract_euler_angles_yzx(m32);
            const vec3 angles42 = extract_euler_angles_yzx(m42);

            if (!equal_approx(extracted_angles, angles32, epsilon))
            {
                return false;
            }

            if (!equal_approx(extracted_angles, angles42, epsilon))
            {
                return false;
            }

            const mat3 m33 = mat3_from_euler_angles_yzx(angles32.x, angles32.y, angles32.z);
            const mat4 m43 = mat4_from_euler_angles_yzx(angles42.x, angles42.y, angles42.z);

            if (!equal_approx(m32, m33))
            {
                return false;
            }

            if (!equal_approx(m42, m43))
            {
                return false;
            }

            break;
        }
        case euler_order::zxy:
        {
            const mat3 m32 = mat3_from_euler_angles_zxy(angles.x, angles.y, angles.z);
            const mat4 m42 = mat4_from_euler_angles_zxy(angles.x, angles.y, angles.z);

            if (!equal_approx(m3, m32))
            {
                return false;
            }

            if (!equal_approx(m4, m42))
            {
                return false;
            }

            const vec3 angles32 = extract_euler_angles_zxy(m32);
            const vec3 angles42 = extract_euler_angles_zxy(m42);

            if (!equal_approx(extracted_angles, angles32, epsilon))
            {
                return false;
            }

            if (!equal_approx(extracted_angles, angles42, epsilon))
            {
                return false;
            }

            const mat3 m33 = mat3_from_euler_angles_zxy(angles32.x, angles32.y, angles32.z);
            const mat4 m43 = mat4_from_euler_angles_zxy(angles42.x, angles42.y, angles42.z);

            if (!equal_approx(m32, m33))
            {
                return false;
            }

            if (!equal_approx(m42, m43))
            {
                return false;
            }

            break;
        }
        case euler_order::zyx:
        {
            const mat3 m32 = mat3_from_euler_angles_zyx(angles.x, angles.y, angles.z);
            const mat4 m42 = mat4_from_euler_angles_zyx(angles.x, angles.y, angles.z);

            if (!equal_approx(m3, m32))
            {
                return false;
            }

            if (!equal_approx(m4, m42))
            {
                return false;
            }

            const vec3 angles32 = extract_euler_angles_zyx(m32);
            const vec3 angles42 = extract_euler_angles_zyx(m42);

            if (!equal_approx(extracted_angles, angles32, epsilon))
            {
                return false;
            }

            if (!equal_approx(extracted_angles, angles42, epsilon))
            {
                return false;
            }

            const mat3 m33 = mat3_from_euler_angles_zyx(angles32.x, angles32.y, angles32.z);
            const mat4 m43 = mat4_from_euler_angles_zyx(angles42.x, angles42.y, angles42.z);

            if (!equal_approx(m32, m33))
            {
                return false;
            }

            if (!equal_approx(m42, m43))
            {
                return false;
            }

            break;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mat_euler_angles_conversion)
{
    // identity (zero rotation)
    {
        const vec3 angles(0.0f, 0.0f, 0.0f);
        const mat3 m = mat3::identity();

        VX_CHECK(mat_euler_test(euler_order::xyz, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::xzy, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::yxz, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::yzx, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::zxy, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::zyx, angles, m, angles));
    }

    {
        const f32 a = 0.770151138f;
        const f32 b = 0.622446895f;
        const f32 c = 0.139381289f;
        const f32 d = 0.420735508f;
        const f32 e = 0.659955740f;
        const f32 f = 0.479425550f;
        const f32 g = 0.599079013f;
        const f32 h = 0.219024152f;
        const f32 i = 0.880346537f;

        // symmetric positive rotation
        {
            const vec3 angles(0.5f, 0.5f, 0.5f);

            VX_CHECK(mat_euler_test(euler_order::xyz, angles, mat3(a, b, -c, -d, e, b, f, -d, a), angles));
            VX_CHECK(mat_euler_test(euler_order::xzy, angles, mat3(a, g, -h, -f, a, d, d, -h, i), angles));
            VX_CHECK(mat_euler_test(euler_order::yxz, angles, mat3(i, d, -h, -h, a, g, d, -f, a), angles));
            VX_CHECK(mat_euler_test(euler_order::yzx, angles, mat3(a, f, -d, -c, a, b, b, -d, e), angles));
            VX_CHECK(mat_euler_test(euler_order::zxy, angles, mat3(e, b, -d, -d, a, f, b, -c, a), angles));
            VX_CHECK(mat_euler_test(euler_order::zyx, angles, mat3(a, d, -f, -h, i, d, g, -h, a), angles));
        }

        // symmetric negative rotation
        {
            const vec3 angles(-0.5f, -0.5f, -0.5f);

            VX_CHECK(mat_euler_test(euler_order::xyz, angles, mat3(a, -h, g, d, i, -h, -f, d, a), angles));
            VX_CHECK(mat_euler_test(euler_order::xzy, angles, mat3(a, -c, b, f, a, -d, -d, b, e), angles));
            VX_CHECK(mat_euler_test(euler_order::yxz, angles, mat3(e, -d, b, b, a, -c, -d, f, a), angles));
            VX_CHECK(mat_euler_test(euler_order::yzx, angles, mat3(a, -f, d, g, a, -h, -h, d, i), angles));
            VX_CHECK(mat_euler_test(euler_order::zxy, angles, mat3(i, -h, d, d, a, -f, -h, g, a), angles));
            VX_CHECK(mat_euler_test(euler_order::zyx, angles, mat3(a, -d, f, b, e, -d, -c, b, a), angles));
        }
    }

    // 90 about X
    {
        const vec3 angles(constants<f32>::half_pi, 0.0f, 0.0f);
        const mat3 m(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f);

        VX_CHECK(mat_euler_test(euler_order::xyz, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::xzy, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::yxz, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::yzx, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::zxy, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::zyx, angles, m, angles));
    }

    // 90 about Y
    {
        const vec3 angles(0.0f, constants<f32>::half_pi, 0.0f);
        const mat3 m(0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);

        VX_CHECK(mat_euler_test(euler_order::xyz, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::xzy, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::yxz, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::yzx, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::zxy, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::zyx, angles, m, angles));
    }

    // 90 about Z
    {
        const vec3 angles(0.0f, 0.0f, constants<f32>::half_pi);
        const mat3 m(0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        VX_CHECK(mat_euler_test(euler_order::xyz, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::xzy, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::yxz, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::yzx, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::zxy, angles, m, angles));
        VX_CHECK(mat_euler_test(euler_order::zyx, angles, m, angles));
    }

    // 180 about X
    {
        const vec3 angles(constants<f32>::pi, 0.0f, 0.0f);
        const mat3 m(1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f);

        const vec3 x_angles(-constants<f32>::pi, 0.0f, 0.0f);
        const vec3 mid_angles(0.0f, -constants<f32>::pi, -constants<f32>::pi);

        VX_CHECK(mat_euler_test(euler_order::xyz, angles, m, x_angles));
        VX_CHECK(mat_euler_test(euler_order::xzy, angles, m, x_angles));
        VX_CHECK(mat_euler_test(euler_order::yxz, angles, m, mid_angles));
        VX_CHECK(mat_euler_test(euler_order::yzx, angles, m, x_angles));
        VX_CHECK(mat_euler_test(euler_order::zxy, angles, m, mid_angles));
        VX_CHECK(mat_euler_test(euler_order::zyx, angles, m, x_angles));
    }

    // 180 about Y
    {
        const vec3 angles(0.0f, constants<f32>::pi, 0.0f);
        const mat3 m(-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f);

        const vec3 y_angles(0.0f, -constants<f32>::pi, 0.0f);
        const vec3 mid_angles(-constants<f32>::pi, 0.0f, -constants<f32>::pi);

        VX_CHECK(mat_euler_test(euler_order::xyz, angles, m, mid_angles));
        VX_CHECK(mat_euler_test(euler_order::xzy, angles, m, y_angles));
        VX_CHECK(mat_euler_test(euler_order::yxz, angles, m, y_angles));
        VX_CHECK(mat_euler_test(euler_order::yzx, angles, m, y_angles));
        VX_CHECK(mat_euler_test(euler_order::zxy, angles, m, y_angles));
        VX_CHECK(mat_euler_test(euler_order::zyx, angles, m, mid_angles));
    }
    
    // 180 about Z
    {
        const vec3 angles(0.0f, 0.0f, constants<f32>::pi);
        const mat3 m(-1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        const vec3 z_angles(0.0f, 0.0f, -constants<f32>::pi);
        const vec3 mid_angles(-constants<f32>::pi, -constants<f32>::pi, 0.0f);

        VX_CHECK(mat_euler_test(euler_order::xyz, angles, m, z_angles));
        VX_CHECK(mat_euler_test(euler_order::xzy, angles, m, mid_angles));
        VX_CHECK(mat_euler_test(euler_order::yxz, angles, m, z_angles));
        VX_CHECK(mat_euler_test(euler_order::yzx, angles, m, mid_angles));
        VX_CHECK(mat_euler_test(euler_order::zxy, angles, m, z_angles));
        VX_CHECK(mat_euler_test(euler_order::zyx, angles, m, z_angles));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}