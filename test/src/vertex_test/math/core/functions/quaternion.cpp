#include "vertex_test/test.hpp"

#include "vertex/math/core/functions/quaternion.hpp"
#include "vertex/math/core/functions/comparison.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

///////////////////////////////////////////////////////////////////////////////

static constexpr float pi_over_six = 0.523598775598299f;   // pi / 6
static constexpr float quarter_pi = 0.785398163397448f;   // pi / 4
static constexpr float pi_over_three = 1.047197551196598f;   // pi / 3
static constexpr float half_pi = 1.570796326794896f;   // pi / 2
static constexpr float two_pi_over_three = 2.094395102393195f;   // 2 * pi / 3
static constexpr float three_quarters_pi = 2.356194490192345f;   // 3 * pi / 4
static constexpr float five_pi_over_six = 2.617993877991494f;   // 5 * pi / 6
static constexpr float pi = 3.141592653589793f;
static constexpr float seven_pi_over_six = 3.665191429188092f;   // 7 * pi / 6
static constexpr float five_pi_over_four = 3.926990816987241f;   // 5 * pi / 4
static constexpr float three_pi_over_two = 4.712388980384690f;   // 3 * pi / 2
static constexpr float seven_pi_over_four = 5.497787143782138f;   // 7 * pi / 4
static constexpr float eleven_pi_over_six = 5.759586531581287f;   // 11 * pi / 6
static constexpr float two_pi = 6.283185307179586f;   // 2 * pi

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_dot)
{
    constexpr quat a(1.0f, -2.0f, 3.0f, -4.0f);
    constexpr quat b(-5.0f, 6.0f, -7.0f, 8.0f);
    VX_STATIC_CHECK_EQ(dot(a, b), -70.0f);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_length)
{
    const quat a{ 1.0f, -2.0f, 3.0f, -4.0f };
    VX_CHECK_EQ(length_squared(a), 30.0f);
    VX_CHECK_EQ(length(a), 5.47722557505f);

    const quat zero{ 0.0f, 0.0f, 0.0f, 0.0f };
    VX_CHECK_EQ(length_squared(zero), 0.0f);
    VX_CHECK_EQ(length(zero), 0.0f);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_normalize)
{
    const quat q{ 1.0f, 2.0f, 2.0f, 3.0f };
    const quat n = normalize(q);
    VX_CHECK_EQ(length(n), 1.0f);
    VX_CHECK(is_normalized(n));

    const quat nf = normalize_fast(q);
    VX_CHECK_EQ(length(nf), 1.0f);
    VX_CHECK(is_normalized(nf));

    const quat zero{ 0.0f, 0.0f, 0.0f, 0.0f };
    const quat n_zero = normalize(zero);
    VX_CHECK(is_identity_approx(n_zero));
    // 0 quaternion should become unit quaternion when normalized
    VX_CHECK(is_normalized(n_zero));
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_normalized_dot)
{
    {
        const quat a{ 1.0f, 1.0f, 1.0f, 1.0f };
        const quat b{ -1.0f, -1.0f, -1.0f, -1.0f };
        const quat c{ 1.0f, -1.0f, 1.0f, -1.0f };

        VX_CHECK_EQ(normalized_dot(a, b), -1.0f);
        VX_CHECK_EQ(normalized_dot(a, a), 1.0f);
        VX_CHECK_EQ(normalized_dot(a, c), 0.0f);
    }

    {
        const quat zero{ 0.0f, 0.0f, 0.0f, 0.0f };
        const quat q{ 1.0f, 2.0f, 3.0f, 4.0f };

        VX_CHECK_EQ(normalized_dot(zero, q), 0.182574183f);
        VX_CHECK_EQ(normalized_dot(q, zero), 0.182574183f);
        VX_CHECK_EQ(normalized_dot(zero, zero), 1.0f);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_conjugate)
{
    constexpr quat a{ 1.0f, 2.0f, 3.0f, 4.0f };
    constexpr quat b{ -1.0f, -2.0f, -3.0f, -4.0f };
    constexpr quat c{ 0.0f, 0.0f, 0.0f, 0.0f };

    VX_STATIC_CHECK_EQ(conjugate(a), quat(1.0f, -2.0f, -3.0f, -4.0f));
    VX_STATIC_CHECK_EQ(conjugate(b), quat(-1.0f, 2.0f, 3.0f, 4.0f));
    VX_STATIC_CHECK_EQ(conjugate(c), quat(0.0f, 0.0f, 0.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_inverse)
{
    {
        constexpr quat q{ 1.0f, 2.0f, 3.0f, 4.0f };
        VX_STATIC_CHECK_EQ(inverse(q), quat(0.033333f, -0.066667f, -0.10000f, -0.133333f));
        VX_STATIC_CHECK(is_identity_approx(inverse(q) * q));
    }

    {
        constexpr quat q{ -1.0f, -2.0f, -3.0f, -4.0f };
        VX_STATIC_CHECK_EQ(inverse(q), quat(-0.033333f, 0.066667f, 0.10000f, 0.133333f));
        VX_STATIC_CHECK(is_identity_approx(inverse(q) * q));
    }

    {
        constexpr quat q = quat::identity();
        VX_STATIC_CHECK_EQ(inverse(q), quat::identity());
        VX_STATIC_CHECK(is_identity_approx(inverse(q) * q));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_axis_angle)
{
    // 90 degrees around +Z
    {
        const vec3 q_axis(0.0f, 0.0f, 1.0f);
        const f32 q_angle = half_pi;
        const quat q = axis_angle(q_axis, q_angle);

        VX_CHECK_EQ(axis(q), q_axis);
        VX_CHECK_EQ(angle(q), q_angle);
    }

    // 180 degrees around +X
    {
        const vec3 q_axis(1.0f, 0.0f, 0.0f);
        const f32 q_angle = pi;
        const quat q = axis_angle(q_axis, q_angle);

        VX_CHECK_EQ(axis(q), q_axis);
        VX_CHECK_EQ(angle(q), q_angle);
    }

    // Identity rotation (angle = 0)
    {
        const vec3 q_axis(1.0f, 0.0f, 0.0f);
        const f32 q_angle = 0.0f;
        const quat q = axis_angle(q_axis, q_angle);

        VX_CHECK_EQ(axis(q), vec3::up());
        VX_CHECK_EQ(angle(q), q_angle);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_rotate)
{
    // Rotate (1, 0, 0) 90 degrees around Z-axis -> should yield (0, 1, 0)
    {
        const vec3 axis(0.0f, 0.0f, 1.0f); // z
        const f32 angle = half_pi;
        const quat q = axis_angle(axis, angle);

        const vec3 v1(1.0f, 0.0f, 0.0f); // x
        const vec3 v2(0.0f, 1.0f, 0.0f); // y

        VX_CHECK_EQ(rotate(v1, q), v2);
    }

    // Rotate (0, 2, 0) 180 degrees around X-axis -> should yield (0, -2, 0)
    {
        const vec3 axis(1.0f, 0.0f, 0.0f); // x
        const f32 angle = pi;
        const quat q = axis_angle(axis, angle);

        const vec3 v1(0.0f, 2.0f, 0.0f); // y
        const vec3 v2(0.0f, -2.0f, 0.0f); // -y

        VX_CHECK_EQ(rotate(v1, q), v2);
    }

    // Identity rotation (should return input unchanged)
    {
        const quat q = quat::identity();
        const vec3 v(3.0f, -2.0f, 1.0f);

        VX_CHECK_EQ(rotate(v, q), v);
    }

    // Rotate zero vector (should always return zero)
    {
        const vec3 axis(1.0f, 0.0f, 0.0f); // x
        const f32 angle = pi;
        const quat q = axis_angle(axis, angle);
        const vec3 v;

        VX_CHECK_EQ(rotate(v, q), v);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_equivalent_rotation)
{
    // Identity vs Identity
    {
        constexpr quat a = quat::identity();
        constexpr quat b = quat::identity();
        VX_CHECK(equivalent_rotation(a, b));
    }

    // q vs -q (same rotation, opposite signs)
    {
        const quat a(constants<f32>::inv_root_two, 0.0f, constants<f32>::inv_root_two, 0.0f); // 90 deg around Y
        const quat b = -a;
        VX_CHECK(equivalent_rotation(a, b));
    }

    // 90 deg vs 90 deg around same axis
    {
        const vec3 axis(0.0f, 1.0f, 0.0f);
        const quat a = axis_angle(axis, radians(90.0f));
        const quat b = axis_angle(axis, radians(90.0f));
        VX_CHECK(equivalent_rotation(a, b));
    }

    // 90 deg vs 270 deg -> should NOT be equivalent
    {
        const vec3 axis(0.0f, 1.0f, 0.0f);
        const quat a = axis_angle(axis, radians(90.0f));
        const quat b = axis_angle(axis, radians(270.0f));
        VX_CHECK(!equivalent_rotation(a, b));
    }

    // Random rotation vs same rotation normalized
    {
        const quat raw(0.1f, 0.2f, 0.3f, 0.4f);
        const quat a = normalize(raw);
        const quat b = -a; // same rotation
        VX_CHECK(equivalent_rotation(a, b));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_rotate_between)
{
    // Identity case: rotating a vector onto itself -> identity quaternion
    {
        const vec3 v(1.0f, 0.0f, 0.0f);
        const quat q = rotate_between(v, v);

        VX_CHECK_EQ(q, quat::identity());
    }

    // Opposite vectors: rotating (1, 0, 0) to (-1, 0, 0) -> 180 degrees around any orthogonal axis
    {
        const vec3 v1(1.0f, 0.0f, 0.0f);
        const vec3 v2(-1.0f, 0.0f, 0.0f);
        const quat q = rotate_between(v1, v2);
        const vec3 r = rotate(v1, q);

        VX_CHECK_EQ(r, v2);
    }

    // Opposite vectors: rotating (1, 0, 0) to (-1, 0, 0) -> 180 degrees around any orthogonal axis
    {
        const vec3 v1(1.0f, 0.0f, 0.0f);
        const vec3 v2(0.0f, 1.0f, 0.0f);
        const quat q = rotate_between(v1, v2);
        const vec3 r = rotate(v1, q);

        VX_CHECK_EQ(r, v2);
    }

    // 90 degree rotation around X: (0, 1, 0) -> (0, 0, 1)
    {
        const vec3 v1(0.0f, 1.0f, 0.0f);
        const vec3 v2(0.0f, 0.0f, 1.0f);
        const quat q = rotate_between(v1, v2);
        const vec3 r = rotate(v1, q);

        VX_CHECK_EQ(r, v2);
    }

    // Arbitrary non-orthogonal vectors
    {
        const vec3 v1 = normalize(vec3(1.0f, 1.0f, 0.0f));
        const vec3 v2 = normalize(vec3(-1.0f, 1.0f, 0.0f));
        const quat q = rotate_between(v1, v2);
        const vec3 r = rotate(v1, q);

        VX_CHECK_EQ(r, v2);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_slerp)
{
    // slerp from identity to identity should yield identity for any t
    {
        const quat q1 = quat::identity();
        const quat q2 = quat::identity();

        for (const f32 t : { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f })
        {
            VX_CHECK(is_identity_approx(slerp(q1, q2, t)));
        }
    }

    // SLERP at t = 0 and t = 1
    {
        const vec3 axis(0.0f, 1.0f, 0.0f);
        const quat q1 = axis_angle(axis, radians(0.0f));   // identity
        const quat q2 = axis_angle(axis, radians(90.0f));

        VX_CHECK_EQ(slerp(q1, q2, 0.0f), q1);  // Should be q1
        VX_CHECK_EQ(slerp(q1, q2, 1.0f), q2);  // Should be q2
    }

    // Halfway Between (t = 0.5)
    {
        const vec3 axis(0.0f, 0.0f, 1.0f);
        const quat q1 = axis_angle(axis, radians(0.0f));
        const quat q2 = axis_angle(axis, radians(90.0f));
        const quat q_half = slerp(q1, q2, 0.5f);

        // Expected: rotation of 45 degrees around Z
        const quat expected = axis_angle(axis, radians(45.0f));
        VX_CHECK_EQ(q_half, expected);
    }

    // Opposite Quaternions (180-degree difference)
    {
        const vec3 axis(0.0f, 1.0f, 0.0f);
        const quat q1 = axis_angle(axis, radians(0.0f));
        const quat q2 = axis_angle(axis, radians(180.0f));
        const quat q_half = slerp(q1, q2, 0.5f);

        // Expected: 90 degrees around Y
        const quat expected = axis_angle(axis, radians(90.0f));
        VX_CHECK_EQ(q_half, expected);
    }

    // Equal and opposite (same rotation)
    {
        const vec3 axis(1.0f, 0.0f, 0.0f);
        const quat q1 = axis_angle(axis, radians(90.0f));
        const quat q_half1 = slerp(q1, -q1, 0.5f);
        const quat q_half2 = slerp(-q1, q1, 0.5f);

        // q1 and -q1 represent the same rotation, so slerp between them
        // should produce something equivalent to q1 (or -q1)
        VX_CHECK(equivalent_rotation(q1, -q1));
        VX_CHECK(equivalent_rotation(q_half1, q1));
        VX_CHECK(equivalent_rotation(q_half2, q1));
        VX_CHECK(equivalent_rotation(q_half1, q_half2));
    }

    // Antipodal Quaternions (Short Path Handling)
    {
        const vec3 axis(1.0f, 0.0f, 0.0f);
        const quat q1 = axis_angle(axis, radians(0.0f));
        const quat q2 = axis_angle(axis, radians(350.0f)); // Almost a full circle (shortest path is -10 degrees)
        const quat q_half = slerp(q1, q2, 0.5f);

        // Expected result: halfway should be 175 degrees around Y,
        // but since quaternions wrap, the shortest path is actually -5 deg
        const quat expected = axis_angle(axis, radians(355.0f)); // -5 degrees from identity
        VX_CHECK_EQ(q_half, expected);
    }

    // Very Small Angle Between Quaternions (LERP fallback)
    {
        const vec3 axis(0.0f, 1.0f, 0.0f);
        const quat q1 = axis_angle(axis, radians(0.0f));
        const quat q2 = axis_angle(axis, radians(1e-6f));
        const quat result = slerp(q1, q2, 0.5f);

        // Should still be near 0.5 * 1e-6 radians around Y
        const quat expected = axis_angle(axis, radians(5e-7f));
        VX_CHECK_EQ(result, expected);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}