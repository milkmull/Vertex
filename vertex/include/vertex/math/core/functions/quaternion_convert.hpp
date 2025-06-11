#pragma once

#include "./quaternion.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// euler angles
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> quat_from_euler_xyz(T x, T y, T z) noexcept
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
    const T qxy = q.x * q.y;
    const T qwz = q.w * q.z;
    const T test = qxy + qwz;

    if (test >= static_cast<T>(0.5) - constants<T>::epsilon)
    {
        // singularity at north pole
        return vec<3, T>(
            static_cast<T>(2) * atan2(q.x, q.w),
            constants<T>::half_pi,
            static_cast<T>(0)
        );
    }
    if (test <= static_cast<T>(-0.5) + constants<T>::epsilon)
    {
        // singularity at south pole
        return vec<3, T>(
            static_cast<T>(-2) * atan2(q.x, q.w),
            -constants<T>::half_pi,
            static_cast<T>(0)
        );
    }

    // XYZ Tait–Bryan angle decomposition

    const T t0 = static_cast<T>(2) * (q.w * q.x - q.y * q.z);
    const T t1 = static_cast<T>(1) - static_cast<T>(2) * (q.x * q.x + q.y * q.y);
    const T x = atan2(t0, t1);

    const T t2 = static_cast<T>(2) * (q.w * q.y + q.z * q.x);
    const T y = asin_clamped(t2);

    const T t3 = static_cast<T>(2) * (q.w * q.z - q.x * q.y);
    const T t4 = static_cast<T>(1) - static_cast<T>(2) * (q.y * q.y + q.z * q.z);
    const T z = atan2(t3, t4);

    return vec<3, T>(x, y, z);
}

///////////////////////////////////////////////////////////////////////////////
// mat3
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
VX_FORCE_INLINE constexpr quat_t<T> quat_from_mat3(const mat<3, 3, T>& m) noexcept
{
    const T trace = m.columns[0].x + m.columns[1].y + m.columns[2].z;

    if (trace > static_cast<T>(0))
    {
        const T s = sqrt(static_cast<T>(1) + trace) * static_cast<T>(2);
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
        const T s = sqrt(static_cast<T>(1) + m.columns[0].x - m.columns[1].y - m.columns[2].z) * static_cast<T>(2);
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
        const T s = sqrt(static_cast<T>(1) + m.columns[1].y - m.columns[0].x - m.columns[2].z) * static_cast<T>(2);
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
        const T s = sqrt(static_cast<T>(1) + m.columns[2].z - m.columns[0].x - m.columns[1].y) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[1].x - m.columns[0].y) * invs,
            (m.columns[0].z + m.columns[2].x) * invs,
            (m.columns[1].z + m.columns[2].y) * invs,
            s * static_cast<T>(0.25)
        );
    }
}

} // namespace math
} // namespace vx