#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/quaternion.hpp"
#include "vertex/math/core/transform/transform3d.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_translate)
{
    {
        const vec3 translation(0.0f, 0.0f, 0.0f);
        const mat4 expected = mat4::identity();

        const mat4 t = transform3d::translate(translation);
        VX_CHECK_EQ(t, expected);

        const vec3 extracted = transform3d::extract_translation(t);
        VX_CHECK_EQ(extracted, translation);
    }

    {
        const vec3 translation(5.0f, 10.0f, 15.0f);
        const mat4 expected(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            5.0f, 10.f, 15.f, 1.0f
        );

        const mat4 t = transform3d::translate(translation);
        VX_CHECK_EQ(t, expected);

        const vec3 extracted = transform3d::extract_translation(t);
        VX_CHECK_EQ(extracted, translation);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_rotate)
{
    // No rotation
    {
        const vec3 axis(1.0f, 0.0f, 0.0f);
        const f32 angle = 0.0f;

        const quat q = axis_angle(axis, angle);

        const mat4 r_axis_angle = transform3d::rotate(axis, angle);
        const mat4 r_quat = transform3d::rotate(q);
        const mat4 expected = mat4::identity();

        VX_CHECK_EQ(r_axis_angle, r_quat);
        VX_CHECK_EQ(r_axis_angle, expected);

        const quat extracted = transform3d::extract_rotation(r_quat);
        VX_CHECK_EQ(q, extracted);
    }

    // 90 degrees around Z
    {
        const vec3 axis(0.0f, 0.0f, 1.0f);
        const f32 angle = constants<f32>::half_pi;

        const quat q = axis_angle(axis, angle);

        const mat4 r_axis_angle = transform3d::rotate(axis, angle);
        const mat4 r_quat = transform3d::rotate(q);
        const mat4 expected(
            0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        VX_CHECK_EQ(r_axis_angle, r_quat);
        VX_CHECK_EQ(r_axis_angle, expected);

        const quat extracted = transform3d::extract_rotation(r_quat);
        VX_CHECK_EQ(q, extracted);
    }

    // 180 degrees around Y
    {
        const vec3 axis(0.0f, 1.0f, 0.0f);
        const f32 angle = constants<f32>::pi;

        const quat q = axis_angle(axis, angle);

        const mat4 r_axis_angle = transform3d::rotate(axis, angle);
        const mat4 r_quat = transform3d::rotate(q);
        const mat4 expected(
            -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        VX_CHECK_EQ(r_axis_angle, r_quat);
        VX_CHECK_EQ(r_axis_angle, expected);

        const quat extracted = transform3d::extract_rotation(r_quat);
        VX_CHECK_EQ(q, extracted);
    }

    // 90 degrees around X
    {
        const vec3 axis(1.0f, 0.0f, 0.0f);
        const f32 angle = constants<f32>::half_pi;

        const quat q = axis_angle(axis, angle);

        const mat4 r_axis_angle = transform3d::rotate(axis, angle);
        const mat4 r_quat = transform3d::rotate(q);
        const mat4 expected(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        VX_CHECK_EQ(r_axis_angle, r_quat);
        VX_CHECK_EQ(r_axis_angle, expected);

        const quat extracted = transform3d::extract_rotation(r_quat);
        VX_CHECK_EQ(q, extracted);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_scale_3d)
{
    {
        const vec3 scale(0.0f, 0.0f, 0.0f);
        const mat4 expected(
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        const mat4 s = transform3d::scale(scale);
        VX_CHECK_EQ(s, expected);

        const vec3 extracted = transform3d::extract_scale(s);
        VX_CHECK_EQ(extracted, scale);
    }

    {
        const vec3 scale(1.0f, 1.0f, 1.0f);
        const mat4 expected(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        const mat4 s = transform3d::scale(scale);
        VX_CHECK_EQ(s, expected);

        const vec3 extracted = transform3d::extract_scale(s);
        VX_CHECK_EQ(extracted, scale);
    }

    {
        const vec3 scale(2.0f, 3.0f, 4.0f);
        const mat4 expected(
            2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 3.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 4.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        const mat4 s = transform3d::scale(scale);
        VX_CHECK_EQ(s, expected);

        const vec3 extracted = transform3d::extract_scale(s);
        VX_CHECK_EQ(extracted, scale);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_trs)
{
    const vec3 t1(1.0f, 2.0f, 3.0f);
    const vec3 axis(0.0f, 1.0f, 0.0f); // Y-axis
    const f32 angle = constants<f32>::half_pi;
    const quat r1 = axis_angle(axis, angle);
    const vec3 s1(2.0f, 4.0f, 6.0f);

    // Create TRS using quaternion
    const mat4 trs_quat = transform3d::trs(t1, r1, s1);

    // Create TRS using axis + angle
    const mat4 trs_axis_angle = transform3d::trs(t1, axis, angle, s1);

    // Both should produce the same matrix
    VX_CHECK_EQ(trs_quat, trs_axis_angle);

    {
        const vec3 t2 = transform3d::extract_translation(trs_quat);
        VX_CHECK_EQ(t1, t2);

        const quat r2 = transform3d::extract_rotation(trs_quat);
        VX_CHECK_EQ(r1, r2);

        const vec3 s2 = transform3d::extract_scale(trs_quat);
        VX_CHECK_EQ(s1, s2);
    }

    {
        const mat4 trs_manual = (
            transform3d::translate(t1) *
            transform3d::rotate(r1) *
            transform3d::scale(s1)
        );

        VX_CHECK_EQ(trs_quat, trs_manual);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_transform)
{
    const vec3 translation(2.0f, -1.0f, 3.0f);
    const vec3 axis(0.0f, 1.0f, 0.0f); // Y-axis
    const f32 angle = constants<f32>::half_pi;
    const quat rotation = axis_angle(axis, angle);
    const vec3 scale(2.0f, 3.0f, 4.0f);

    // translation only
    {
        const mat4 t = transform3d::translate(translation);

        const vec3 p1(1.0f, 1.0f, 1.0f);
        const vec3 expected(3.0f, 0.0f, 4.0f);

        const vec3 p2 = transform3d::transform(t, p1);
        VX_CHECK_EQ(p2, expected);
    }

    // rotation only
    {
        const mat4 r = transform3d::rotate(rotation);

        const vec3 p1(1.0f, 0.0f, 0.0f);
        const vec3 expected(0.0f, 0.0f, -1.0f); // 90 around Y

        const vec3 p2 = transform3d::transform(r, p1);
        VX_CHECK_EQ(p2, expected);
    }

    // scale only
    {
        const mat4 s = transform3d::scale(scale);

        const vec3 p1(2.0f, 2.0f, 2.0f);
        const vec3 expected(4.0f, 6.0f, 8.0f);

        const vec3 p2 = transform3d::transform(s, p1);
        VX_CHECK_EQ(p2, expected);
    }

    // trs (T * R * S)
    {
        const mat4 trs = transform3d::trs(translation, rotation, scale);

        const vec3 p1(1.0f, 1.0f, 1.0f);
        const vec3 expected(6.0f, 2.0f, 1.0f);

        const vec3 p2 = transform3d::transform(trs, p1);
        VX_CHECK_EQ(p2, expected);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_inverse_3d)
{
    const vec3 translation(2.0f, -1.0f, 3.0f);
    const vec3 axis(0.0f, 1.0f, 0.0f); // Rotate around Y
    const f32 angle = constants<f32>::half_pi;
    const quat rotation = axis_angle(axis, angle);
    const vec3 scale(2.0f, 3.0f, 4.0f);

    // translation only
    {
        const mat4 tn = transform3d::translate(translation);
        const mat4 ti = transform3d::inverse(tn);

        const vec3 p1(1.0f, 1.0f, 1.0f);
        const vec3 p2 = transform3d::transform(tn, p1);
        const vec3 p3 = transform3d::transform(ti, p2);

        VX_CHECK_EQ(p1, p3);
    }

    // rotation only
    {
        const mat4 rn = transform3d::rotate(rotation);
        const mat4 ri = transform3d::inverse(rn);

        const vec3 p1(1.0f, 0.0f, 0.0f);
        const vec3 p2 = transform3d::transform(rn, p1);
        const vec3 p3 = transform3d::transform(ri, p2);

        VX_CHECK_EQ(p1, p3);
    }

    // scale only
    {
        const mat4 sn = transform3d::scale(scale);
        const mat4 si = transform3d::inverse(sn);

        const vec3 p1(2.0f, 3.0f, 4.0f);
        const vec3 p2 = transform3d::transform(sn, p1);
        const vec3 p3 = transform3d::transform(si, p2);

        VX_CHECK_EQ(p1, p3);
    }

    // trs
    {
        const mat4 trsn = transform3d::trs(translation, rotation, scale);
        const mat4 trsi = transform3d::inverse(trsn);

        const vec3 p1(1.0f, 1.0f, 1.0f);
        const vec3 p2 = transform3d::transform(trsn, p1);
        const vec3 p3 = transform3d::transform(trsi, p2);

        VX_CHECK_EQ(p1, p3);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_look_at)
{
    VX_SECTION("lh")
    {
        const vec3 eye(1.0f, 2.0f, 3.0f);
        const vec3 target(4.0f, 6.0f, 5.0f);
        const vec3 up(0.0f, 1.0f, 0.0f);

        const mat4 view = transform3d::look_at_lh(eye, target, up);
        const mat4 expected(
             0.554700196f, -0.618031442f,  0.557085991f, 0.0f,
             0.0f,          0.669534028f,  0.742781341f, 0.0f,
            -0.832050204f, -0.412021011f,  0.371390671f, 0.0f,
             1.94145036f,   0.515026391f, -3.15682077f,  1.0f
        );

        VX_CHECK_EQ(view, expected);

        const vec3 p1 = eye;
        const vec3 p2 = transform3d::transform(view, p1);
        VX_CHECK(is_zero_approx(p2));

        {
            const vec3 forward_lh = target;
            const vec3 forward_lh_transformed = transform3d::transform(view, forward_lh);
            const f32 dist = length(eye - target);

            // Should land in front of the camera along +Z
            VX_CHECK_EQ(forward_lh_transformed.z, dist);
        }
    }

    VX_SECTION("rh")
    {
        const vec3 eye(1.0f, 2.0f, 3.0f);
        const vec3 target(4.0f, 6.0f, 5.0f);
        const vec3 up(0.0f, 1.0f, 0.0f);

        const mat4 view = transform3d::look_at_rh(eye, target, up);
        const mat4 expected(
            -0.554700196f, -0.618031442f, -0.557085991f, 0.0f,
             0.0f,          0.669534028f, -0.742781341f, 0.0f,
             0.832050204f, -0.412021011f, -0.371390671f, 0.0f,
            -1.94145036f,   0.515026391f,  3.15682077f,  1.0f
        );

        VX_CHECK_EQ(view, expected);

        const vec3 p1 = eye;
        const vec3 p2 = transform3d::transform(view, p1);
        VX_CHECK(is_zero_approx(p2));

        {
            const vec3 forward_rh = target;
            const vec3 forward_rh_transformed = transform3d::transform(view, forward_rh);
            const f32 dist = length(eye - target);

            // Should land in front of the camera along -Z
            VX_CHECK_EQ(forward_rh_transformed.z, -dist);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}