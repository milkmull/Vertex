#include "vertex_test/test.hpp"

#include "vertex/math/core/transform/transform2d.hpp"

using namespace vx::math;

#define VX_CHECK_EQ(a, b) VX_CHECK(equal_approx(a, b))
#define VX_STATIC_CHECK_EQ(a, b) VX_STATIC_CHECK(equal_approx(a, b))

template <typename T, VXM_REQ_FLOAT(T)>
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

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_translate)
{
    {
        const vec2 translation(0.0f, 0.0f);
        const mat3 expected(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        const mat3 t = transform2d::translate(translation);
        VX_CHECK_EQ(t, expected);

        const vec2 extracted = transform2d::extract_translation(t);
        VX_CHECK_EQ(extracted, translation);
    }

    {
        const vec2 translation(5.0f, 10.0f);
        const mat3 expected(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 5.0f, 10.0f, 1.0f);

        const mat3 t = transform2d::translate(translation);
        VX_CHECK_EQ(t, expected);

        const vec2 extracted = transform2d::extract_translation(t);
        VX_CHECK_EQ(extracted, translation);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_rotate)
{
    {
        const f32 rotation = 0.0f;
        const mat3 expected(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        const mat3 r = transform2d::rotate(rotation);
        VX_CHECK_EQ(r, expected);

        const f32 extracted = transform2d::extract_rotation(r);
        VX_CHECK(equivalent_angle(extracted, rotation));
    }

    {
        const f32 rotation = constants<f32>::quarter_pi;
        const mat3 expected(
            0.707106769, 0.707106769, 0.0f,
            -0.707106769, 0.707106769, 0.0f,
            0.0f, 0.0f, 1.0f
        );

        const mat3 r = transform2d::rotate(rotation);
        VX_CHECK_EQ(r, expected);

        const f32 extracted = transform2d::extract_rotation(r);
        VX_CHECK(equivalent_angle(extracted, rotation));
    }

    {
        const f32 rotation = constants<f32>::half_pi;
        const mat3 expected(
            0.0f, 1.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        );

        const mat3 r = transform2d::rotate(rotation);
        VX_CHECK_EQ(r, expected);

        const f32 extracted = transform2d::extract_rotation(r);
        VX_CHECK(equivalent_angle(extracted, rotation));
    }

    {
        const f32 rotation = constants<f32>::half_pi;
        const mat3 expected(
            0.0f, 1.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        );

        const mat3 r = transform2d::rotate(rotation);
        VX_CHECK_EQ(r, expected);

        const f32 extracted = transform2d::extract_rotation(r);
        VX_CHECK(equivalent_angle(extracted, rotation));
    }

    {
        const f32 rotation = -constants<f32>::half_pi;
        const mat3 expected(
            0.0f, -1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        );

        const mat3 r = transform2d::rotate(rotation);
        VX_CHECK_EQ(r, expected);

        const f32 extracted = transform2d::extract_rotation(r);
        VX_CHECK(equivalent_angle(extracted, rotation));
    }

    {
        const f32 rotation = constants<f32>::pi;
        const mat3 expected(
            -1.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        );

        const mat3 r = transform2d::rotate(rotation);
        VX_CHECK_EQ(r, expected);

        const f32 extracted = transform2d::extract_rotation(r);
        VX_CHECK(equivalent_angle(extracted, rotation));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_scale)
{
    {
        const vec2 scale(0.0f, 0.0f);
        const mat3 expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        const mat3 s = transform2d::scale(scale);
        VX_CHECK_EQ(s, expected);

        const vec2 extracted = transform2d::extract_scale(s);
        VX_CHECK_EQ(extracted, scale);
    }

    {
        const vec2 scale(1.0f, 1.0f);
        const mat3 expected(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        const mat3 s = transform2d::scale(scale);
        VX_CHECK_EQ(s, expected);

        const vec2 extracted = transform2d::extract_scale(s);
        VX_CHECK_EQ(extracted, scale);
    }

    {
        const vec2 scale(2.0f, 3.0f);
        const mat3 expected(2.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        const mat3 s = transform2d::scale(scale);
        VX_CHECK_EQ(s, expected);

        const vec2 extracted = transform2d::extract_scale(s);
        VX_CHECK_EQ(extracted, scale);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_trs)
{
    const vec2 t1(1.0f, 2.0f);
    const f32 r1 = -constants<f32>::half_pi;
    const vec2 s1(2.0f, 4.0f);

    const mat3 trs = transform2d::trs(t1, r1, s1);
    
    {
        const vec2 t2 = transform2d::extract_translation(trs);
        VX_CHECK_EQ(t1, t2);

        const f32 r2 = transform2d::extract_rotation(trs);
        VX_CHECK(equivalent_angle(r1, r2));

        const vec2 s2 = transform2d::extract_scale(trs);
        VX_CHECK_EQ(s1, s2);
    }

    {
        const mat3 trs2 = (
            transform2d::translate(t1) *
            transform2d::rotate(r1) *
            transform2d::scale(s1)
        );

        VX_CHECK_EQ(trs, trs2);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_transform)
{
    const vec2 translation(2.0f, -1.0f);
    const f32 rotation = constants<f32>::half_pi;
    const vec2 scale(2.0f, 3.0f);

    // translation only
    {
        const mat3 t = transform2d::translate(translation);
        
        const vec2 p1(1.0f, 1.0f);
        const vec2 expected(3.0f, 0.0f);

        const vec2 p2 = transform2d::transform(t, p1);
        VX_CHECK_EQ(p2, expected);
    }

    // rotation only
    {
        const mat3 r = transform2d::rotate(rotation);

        const vec2 p1(1.0f, 0.0f);
        const vec2 expected(0.0f, 1.0f);

        const vec2 p2 = transform2d::transform(r, p1);
        VX_CHECK_EQ(p2, expected);
    }

    // scale only
    {
        const mat3 s = transform2d::scale(scale);

        const vec2 p1(2.0f, 2.0f);
        const vec2 expected(4.0f, 6.0f);

        const vec2 p2 = transform2d::transform(s, p1);
        VX_CHECK_EQ(p2, expected);
    }

    // trs
    {
        const mat3 trs = transform2d::trs(translation, rotation, scale);

        const vec2 p1(1.0f, 1.0f);
        const vec2 expected(-1.0f, 1.0f);

        const vec2 p2 = transform2d::transform(trs, p1);
        VX_CHECK_EQ(p2, expected);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_inverse)
{
    const vec2 translation(2.0f, -1.0f);
    const f32 rotation = constants<f32>::half_pi;
    const vec2 scale(2.0f, 3.0f);

    // translation only
    {
        const mat3 tn = transform2d::translate(translation);
        const mat3 ti = transform2d::inverse(tn);

        const vec2 p1(1.0f, 1.0f);
        const vec2 p2 = transform2d::transform(tn, p1);
        const vec2 p3 = transform2d::transform(ti, p2);

        VX_CHECK_EQ(p1, p3);
    }

    // rotation only
    {
        const mat3 rn = transform2d::rotate(rotation);
        const mat3 ri = transform2d::inverse(rn);

        const vec2 p1(1.0f, 0.0f);
        const vec2 p2 = transform2d::transform(rn, p1);
        const vec2 p3 = transform2d::transform(ri, p2);

        VX_CHECK_EQ(p1, p3);
    }

    // scale only
    {
        const mat3 sn = transform2d::scale(scale);
        const mat3 si = transform2d::inverse(sn);

        const vec2 p1(2.0f, 3.0f);
        const vec2 p2 = transform2d::transform(sn, p1);
        const vec2 p3 = transform2d::transform(si, p2);

        VX_CHECK_EQ(p1, p3);
    }

    // trs
    {
        const mat3 trsn = transform2d::trs(translation, rotation, scale);
        const mat3 trsi = transform2d::inverse(trsn);

        const vec2 p1(1.0f, 1.0f);
        const vec2 p2 = transform2d::transform(trsn, p1);
        const vec2 p3 = transform2d::transform(trsi, p2);

        VX_CHECK_EQ(p1, p3);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}