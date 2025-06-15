#pragma once

#include "./quaternion.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// quaternion to/from euler
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> euler_xyz_to_quat(T x, T y, T z) noexcept
{
    x *= static_cast<T>(0.5);
    y *= static_cast<T>(0.5);
    z *= static_cast<T>(0.5);

    const T cx = cos(x);
    const T cy = cos(y);
    const T cz = cos(z);

    const T sx = sin(x);
    const T sy = sin(y);
    const T sz = sin(z);

    return quat_t<T>(
        -(sx * sy * sz) + (cx * cy * cz),
        +(sx * cy * cz) + (sy * sz * cx),
        -(sx * sz * cy) + (sy * cx * cz),
        +(sx * sy * cz) + (sz * cx * cy)
    );
}

// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_(in_3-2-1_sequence)_conversion
// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> quat_to_euler_xyz(const quat_t<T>& q) noexcept
{
    const T m13 = static_cast<T>(2) * (q.x * q.z + q.w * q.y);
    const T y = asin_clamped(m13);

    if (greater_or_equal_approx(abs(m13), static_cast<T>(1)))
    {
        // singularity

        const T x = static_cast<T>(2) * atan2(q.x, q.w);
        const T z = static_cast<T>(0);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // XYZ Tait–Bryan angle decomposition

        const T t0 = static_cast<T>(2) * (q.w * q.x - q.y * q.z);
        const T t1 = static_cast<T>(1) - static_cast<T>(2) * (q.x * q.x + q.y * q.y);
        const T t2 = static_cast<T>(2) * (q.w * q.z - q.x * q.y);
        const T t3 = static_cast<T>(1) - static_cast<T>(2) * (q.y * q.y + q.z * q.z);

        const T x = atan2(t0, t1);
        const T z = atan2(t2, t3);

        return vec<3, T>(x, y, z);
    }
}

///////////////////////////////////////////////////////////////////////////////
// mat3 to/from quaternion
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr mat<3, 3, T> quat_to_mat3(const quat_t<T>& q) noexcept
{
    const T qxx = q.x * q.x;
    const T qyy = q.y * q.y;
    const T qzz = q.z * q.z;
    const T qxz = q.x * q.z;
    const T qxy = q.x * q.y;
    const T qyz = q.y * q.z;
    const T qwx = q.w * q.x;
    const T qwy = q.w * q.y;
    const T qwz = q.w * q.z;

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

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> mat3_to_quat(const mat<3, 3, T>& m) noexcept
{
    const T trace = m.columns[0].x + m.columns[1].y + m.columns[2].z;

    if (trace > static_cast<T>(0))
    {
        const T s = sqrt(static_cast<T>(1) + trace) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            s * static_cast<T>(0.25),
            (m.columns[1].z - m.columns[2].y) * invs,
            (m.columns[2].x - m.columns[0].z) * invs,
            (m.columns[0].y - m.columns[1].x) * invs
        );
    }
    else if (m.columns[0].x > m.columns[1].y && m.columns[0].x > m.columns[2].z)
    {
        const T s = sqrt(static_cast<T>(1) + m.columns[0].x - m.columns[1].y - m.columns[2].z) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[1].z - m.columns[2].y) * invs,
            s * static_cast<T>(0.25),
            (m.columns[1].x + m.columns[0].y) * invs,
            (m.columns[2].x + m.columns[0].z) * invs
        );
    }
    else if (m.columns[1].y > m.columns[2].z)
    {
        const T s = sqrt(static_cast<T>(1) + m.columns[1].y - m.columns[0].x - m.columns[2].z) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[0].z - m.columns[2].x) * invs,
            (m.columns[1].x + m.columns[0].y) * invs,
            s * static_cast<T>(0.25),
            (m.columns[2].y + m.columns[1].z) * invs
        );
    }
    else
    {
        const T s = sqrt(static_cast<T>(1) + m.columns[2].z - m.columns[0].x - m.columns[1].y) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[0].y - m.columns[1].x) * invs,
            (m.columns[2].x + m.columns[0].z) * invs,
            (m.columns[2].y + m.columns[1].z) * invs,
            s * static_cast<T>(0.25)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
// mat3 to/from euler
///////////////////////////////////////////////////////////////////////////////

// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<3, 3, T> euler_xyz_to_mat3(T x, T y, T z) noexcept
{
    const T cx = cos(x);
    const T cy = cos(y);
    const T cz = cos(z);

    const T sx = sin(x);
    const T sy = sin(y);
    const T sz = sin(z);

    return mat<3, 3, T>(
        +(cy * cz),
        +(sx * sy * cz) + (cx * sz),
        -(cx * sy * cz) + (sx * sz),

        -(cy * sz),
        -(sx * sy * sz) + (cx * cz),
        +(cx * sy * sz) + (sx * cz),

        +(sy),
        -(sx * cy),
        +(cx * cy)
    );
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> mat3_to_euler_xyz(const mat<3, 3, T>& m) noexcept
{
    const T x = atan2(m.columns[2].y, m.columns[2].z);
    const T c2 = sqrt(m.columns[0].x * m.columns[0].x + m.columns[1].x * m.columns[1].x);
    const T y = atan2(-m.columns[2].x, c2);
    const T s1 = sin(x);
    const T c1 = cos(x);
    const T z = atan2(s1 * m.columns[0].z - c1 * m.columns[0].y, c1 * m.columns[1].y - s1 * m.columns[1].z);

    return vec<3, T>(-x, -y, -z);
}

///////////////////////////////////////////////////////////////////////////////
// mat3 to/from axis angle
///////////////////////////////////////////////////////////////////////////////

// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<3, 3, T> axis_angle_mat3(const vec<3, T>& axis, T angle) noexcept
{
    const T cosa = cos(angle);
    const T sina = sin(angle);
    const vec<3, T> t = (static_cast<T>(1) - cosa) * axis;
    const vec<3, T> sina_axis = sina * axis;

    return mat<3, 3, T>(
        (t.x * axis.x) + cosa,
        (t.x * axis.y) + sina_axis.z,
        (t.x * axis.z) - sina_axis.y,

        (t.y * axis.x) - sina_axis.z,
        (t.y * axis.y) + cosa,
        (t.y * axis.z) + sina_axis.x,

        (t.z * axis.x) + sina_axis.y,
        (t.z * axis.y) - sina_axis.x,
        (t.z * axis.z) + cosa
    );
}

} // namespace math
} // namespace vx