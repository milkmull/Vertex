#pragma once

#include "../math.h"

namespace vx {
namespace math {
namespace transform3d {
namespace matrix3 {

// =============== mat3 pure rotation ===============

// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_rotation(const vec<3, T>& axis, T angle)
{
    const vec<3, T> naxis = math::normalize(axis);

    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);
    const vec<3, T> t = (static_cast<T>(1) - cosa) * naxis;
    const vec<3, T> sina_naxis = sina * naxis;

    return mat<3, 3, T>(
        (t.x * naxis.x) + cosa,
        (t.x * naxis.y) + sina_naxis.z,
        (t.x * naxis.z) - sina_naxis.y,

        (t.y * naxis.x) - sina_naxis.z,
        (t.y * naxis.y) + cosa,
        (t.y * naxis.z) + sina_naxis.x,

        (t.z * naxis.x) + sina_naxis.y,
        (t.z * naxis.y) - sina_naxis.x,
        (t.z * naxis.z) + cosa
    );
}

// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_rotation(const quat_t<T>& rotation)
{
    const quat_t<T> qn = math::normalize(q);

    const T qxx = qn.x * qn.x;
    const T qyy = qn.y * qn.y;
    const T qzz = qn.z * qn.z;
    const T qxz = qn.x * qn.z;
    const T qxy = qn.x * qn.y;
    const T qyz = qn.y * qn.z;
    const T qwx = qn.w * qn.x;
    const T qwy = qn.w * qn.y;
    const T qwz = qn.w * qn.z;

    return mat<3, 3, T>(
        static_cast<T>(1) - static_cast<T>(2) * (qyy + qzz),
        static_cast<T>(2) * (qxy + qwz),
        static_cast<T>(2) * (qxz - qwy),

        static_cast<T>(2) * (qxy - qwz),
        static_cast<T>(1) - static_cast<T>(2) * (qxx + qzz),
        static_cast<T>(2) * (qyz + qwx),

        static_cast<T>(2) * (qxz + qwy),
        static_cast<T>(2) * (qyz - qwx),
        static_cast<T>(1) - static_cast<T>(2) * (qxx + qyy)
    );
}

// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_rotation(T x, T y, T z)
{
    const T ct1 = math::cos(x);
    const T ct2 = math::cos(y);
    const T ct3 = math::cos(z);

    const T st1 = math::sin(x);
    const T st2 = math::sin(y);
    const T st3 = math::sin(z);

    return mat<3, 3, T>(
        +(ct2 * ct3),
        +(st1 * st2 * ct3) + (ct1 * st3),
        -(ct1 * st2 * ct3) + (st1 * st3),

        -(ct2 * st3),
        -(st1 * st2 * st3) + (ct1 * ct3),
        +(ct1 * st2 * st3) + (st1 * ct3),

        +(st2),
        -(st1 * ct2),
        +(ct1 * ct2)
    );
}

// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> get_rotation(const mat<3, 3, T>& m)
{
    const T trace = m.columns[0].x + m.columns[1].y + m.columns[2].z;

    if (trace > static_cast<T>(0))
    {
        const T s = math::sqrt(static_cast<T>(1) + trace) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            s * static_cast<T>(0.25),
            (m.columns[2].y - m.columns[1].z) * invs,
            (m.columns[0].z - m.columns[2].x) * invs,
            (m.columns[1].x - m.columns[0].y) * invs
        );
    }
    else if (m.columns[0].x > m.columns[1].y && m.columns[0].x > m.columns[2].z)
    {
        const T s = math::sqrt(static_cast<T>(1) + m.columns[0].x - m.columns[1].y - m.columns[2].z) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[2].y - m.columns[1].z) * invs,
            s * static_cast<T>(0.25),
            (m.columns[0].y + m.columns[1].x) * invs,
            (m.columns[0].z + m.columns[2].x) * invs
        );
    }
    else if (m.columns[1].y > m.columns[2].z)
    {
        const T s = math::sqrt(static_cast<T>(1) + m.columns[1].y - m.columns[0].x - m.columns[2].z) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[2].x - m.columns[0].z) * invs,
            (m.columns[0].y + m.columns[1].x) * invs,
            s * static_cast<T>(0.25),
            (m.columns[1].z + m.columns[2].y) * invs
        );
    }
    else
    {
        const T s = math::sqrt(static_cast<T>(1) + m.columns[2].z - m.columns[0].x - m.columns[1].y) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[1].x - m.columns[0].y) * invs,
            (m.columns[0].z + m.columns[2].x) * invs,
            (m.columns[1].z + m.columns[2].y) * invs,
            s * static_cast<T>(0.25)
        );
    }
}

// =============== mat3 pure scale ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_scale(const vec<3, T>& scale)
{
    return mat<3, 3, T>(
        scale.x,
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        scale.y,
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        scale.z
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> get_scale(const mat<3, 3, T>& m)
{
    return vec<3, T>(
        math::length(m.columns[0]),
        math::length(m.columns[1]),
        math::length(m.columns[2])
    );
}

// =============== look at ===============

// https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_look_at_lh(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::UP()
)
{
    const vec<3, T> z = math::normalize(target - eye);
    const vec<3, T> x = math::normalize(math::cross(math::normalize(up), z));
    const vec<3, T> y = math::cross(z, x);

    return mat<3, 3, T>(x, y, z);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_look_at_rh(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::UP()
)
{
    const vec<3, T> z = math::normalize(eye - target);
    const vec<3, T> x = math::normalize(math::cross(math::normalize(up), z));
    const vec<3, T> y = math::cross(z, x);

    return mat<3, 3, T>(x, y, z);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_look_at(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::UP()
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return make_look_at_lh(eye, target, up);
#else
    return make_look_at_rh(eye, target, up);
#endif
}

}
}
}
}