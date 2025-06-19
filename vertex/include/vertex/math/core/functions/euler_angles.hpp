#pragma once

#include "./trigonometric.hpp"

namespace vx {
namespace math {

#define m11_ static_cast<T>(1) - static_cast<T>(2) * (q.y * q.y + q.z * q.z)
#define m12_ static_cast<T>(2) * (q.x * q.y + q.w * q.z)
#define m13_ static_cast<T>(2) * (q.x * q.z - q.w * q.y)
#define m21_ static_cast<T>(2) * (q.x * q.y - q.w * q.z)
#define m22_ static_cast<T>(1) - static_cast<T>(2) * (q.x * q.x + q.z * q.z)
#define m23_ static_cast<T>(2) * (q.y * q.z + q.w * q.x)
#define m31_ static_cast<T>(2) * (q.x * q.z + q.w * q.y)
#define m32_ static_cast<T>(2) * (q.y * q.z - q.w * q.x)
#define m33_ static_cast<T>(1) - static_cast<T>(2) * (q.x * q.x + q.y * q.y)

///////////////////////////////////////////////////////////////////////////////
// euler_order
///////////////////////////////////////////////////////////////////////////////

enum class euler_order
{
    XYZ,
    YXZ,
    ZXY,
    ZYX,
    YZX,
    XZY
};

///////////////////////////////////////////////////////////////////////////////
// extract_euler_angles (quaternion)
///////////////////////////////////////////////////////////////////////////////

// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_(in_3-2-1_sequence)_conversion
// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
// https://github.com/mrdoob/three.js/blob/dev/src/math/Euler.js#L188

// xyz

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_xyz(const quat_t<T>& q) noexcept
{
    const T m31 = m31_;
    const T y = asin_clamped(m31);

    if (greater_or_equal_approx(abs(m31), static_cast<T>(1)))
    {
        // singularity

        const T x = static_cast<T>(2) * atan2(q.x, q.w);
        const T z = static_cast<T>(0);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // XYZ Tait–Bryan angle decomposition

        const T m32 = m32_;
        const T m33 = m33_;
        const T m21 = m21_;
        const T m11 = m11_;

        const T x = atan2(-m32, m33);
        const T z = atan2(-m21, m11);

        return vec<3, T>(x, y, z);
    }
}

// yxz

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_yxz(const quat_t<T>& q) noexcept
{
    const T m32 = m32_;
    const T x = asin_clamped(-m32);

    if (greater_or_equal_approx(abs(m32), static_cast<T>(1)))
    {
        // singularity

        const T y = static_cast<T>(2) * atan2(q.y, q.w);
        const T z = static_cast<T>(0);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // YXZ Tait–Bryan angle decomposition

        const T m31 = m31_;
        const T m33 = m33_;
        const T m12 = m12_;
        const T m22 = m22_;

        const T y = atan2(m31, m33);
        const T z = atan2(m12, m22);

        return vec<3, T>(x, y, z);
    }
}

// zxy

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_zxy(const quat_t<T>& q) noexcept
{
    const T m23 = m23_;
    const T x = asin_clamped(m23);

    if (greater_or_equal_approx(abs(m23), static_cast<T>(1)))
    {
        // singularity

        const T y = static_cast<T>(0);
        const T z = static_cast<T>(2) * atan2(q.z, q.w);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // ZXY Tait–Bryan angle decomposition

        const T m13 = m13_;
        const T m33 = m33_;
        const T m21 = m21_;
        const T m22 = m22_;

        const T y = atan2(-m13, m33);
        const T z = atan2(-m21, m22);

        return vec<3, T>(x, y, z);
    }
}

// zyx

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_zyx(const quat_t<T>& q) noexcept
{
    const T m13 = m13_;
    const T y = asin_clamped(-m13);

    if (greater_or_equal_approx(abs(m13), static_cast<T>(1)))
    {
        // singularity

        const T x = static_cast<T>(0);
        const T z = atan2(q.z, q.w);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // ZYX Tait–Bryan angle decomposition

        const T m23 = m23_;
        const T m33 = m33_;
        const T m12 = m12_;
        const T m11 = m11_;

        const T x = atan2(m23, m33);
        const T z = atan2(m12, m11);

        return vec<3, T>(x, y, z);
    }
}

// yzx

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_yzx(const quat_t<T>& q) noexcept
{
    const T m12 = m12_;
    const T z = asin_clamped(m12);

    if (greater_or_equal_approx(abs(m12), static_cast<T>(1)))
    {
        // singularity

        const T x = static_cast<T>(0);
        const T y = atan2(q.y, q.w);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // YZX Tait–Bryan angle decomposition

        const T m32 = m32_;
        const T m22 = m22_;
        const T m13 = m13_;
        const T m11 = m11_;

        const T x = atan2(-m32, m22);
        const T y = atan2(-m13, m11);

        return vec<3, T>(x, y, z);
    }
}

// xzy

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_xzy(const quat_t<T>& q) noexcept
{
    const T m21 = m21_;
    const T z = asin_clamped(-m21);

    if (greater_or_equal_approx(abs(m21), static_cast<T>(1)))
    {
        // singularity

        const T x = atan2(q.x, q.w);
        const T y = static_cast<T>(0);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // XZY Tait–Bryan angle decomposition

        const T m23 = m23_;
        const T m22 = m22_;
        const T m31 = m31_;
        const T m11 = m11_;

        const T x = atan2(m23, m22);
        const T y = atan2(m31, m11);

        return vec<3, T>(x, y, z);
    }
}

///////////////////////////////////////////////////////////////////////////////
// extract_euler_angles (matrix)
///////////////////////////////////////////////////////////////////////////////

// https://github.com/mrdoob/three.js/blob/dev/src/math/Euler.js#L188

// xyz

template <size_t M, typename T, VXM_REQ((M == 3 || M == 4) && is_float<T>::value)>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_xyz(const mat<M, M, T>& m) noexcept
{
    const T y = asin_clamped(m.columns[2].x);

    if (greater_or_equal_approx(abs(m.columns[2].x), static_cast<T>(1)))
    {
        // singularity

        const T x = atan2(m.columns[1].z, m.columns[1].y);
        const T z = static_cast<T>(0);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // XYZ Tait–Bryan angle decomposition

        const T x = atan2(-m.columns[2].y, m.columns[2].z);
        const T z = atan2(-m.columns[1].x, m.columns[0].x);

        return vec<3, T>(x, y, z);
    }
}

// yxz

template <size_t M, typename T, VXM_REQ((M == 3 || M == 4) && is_float<T>::value)>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_yxz(const mat<M, M, T>& m) noexcept
{
    const T x = asin_clamped(-m.columns[2].y);

    if (greater_or_equal_approx(abs(m.columns[2].y), static_cast<T>(1)))
    {
        // singularity

        const T y = atan2(-m.columns[0].z, m.columns[0].x);
        const T z = static_cast<T>(0);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // YXZ Tait–Bryan angle decomposition

        const T y = atan2(m.columns[2].x, m.columns[2].z);
        const T z = atan2(m.columns[0].y, m.columns[1].y);

        return vec<3, T>(x, y, z);
    }
}

// zxy

template <size_t M, typename T, VXM_REQ((M == 3 || M == 4) && is_float<T>::value)>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_zxy(const mat<M, M, T>& m) noexcept
{
    const T x = asin_clamped(m.columns[1].z);

    if (greater_or_equal_approx(abs(m.columns[1].z), static_cast<T>(1)))
    {
        // singularity

        const T y = static_cast<T>(0);
        const T z = atan2(m.columns[0].y, m.columns[0].x);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // ZXY Tait–Bryan angle decomposition

        const T y = atan2(-m.columns[0].z, m.columns[2].z);
        const T z = atan2(-m.columns[1].x, m.columns[1].y);

        return vec<3, T>(x, y, z);
    }
}

// zyx

template <size_t M, typename T, VXM_REQ((M == 3 || M == 4) && is_float<T>::value)>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_zyx(const mat<M, M, T>& m) noexcept
{
    const T y = asin_clamped(-m.columns[0].z);

    if (greater_or_equal_approx(abs(m.columns[0].z), static_cast<T>(1)))
    {
        // singularity

        const T x = static_cast<T>(0);
        const T z = atan2(-m.columns[1].x, m.columns[1].y);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // ZYX Tait–Bryan angle decomposition

        const T x = atan2(m.columns[1].z, m.columns[2].z);
        const T z = atan2(m.columns[0].y, m.columns[0].x);

        return vec<3, T>(x, y, z);
    }
}

// yzx

template <size_t M, typename T, VXM_REQ((M == 3 || M == 4) && is_float<T>::value)>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_yzx(const mat<M, M, T>& m) noexcept
{
    const T z = asin_clamped(m.columns[0].y);

    if (greater_or_equal_approx(abs(m.columns[0].y), static_cast<T>(1)))
    {
        // singularity

        const T x = static_cast<T>(0);
        const T y = atan2(m.columns[2].x, m.columns[2].z);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // YZX Tait–Bryan angle decomposition

        const T x = atan2(-m.columns[2].y, m.columns[1].y);
        const T y = atan2(-m.columns[0].z, m.columns[0].x);

        return vec<3, T>(x, y, z);
    }
}

// xzy

template <size_t M, typename T, VXM_REQ((M == 3 || M == 4) && is_float<T>::value)>
VX_FORCE_INLINE constexpr vec<3, T> extract_euler_angles_xzy(const mat<M, M, T>& m) noexcept
{
    const T z = asin_clamped(-m.columns[1].x);

    if (greater_or_equal_approx(abs(m.columns[1].x), static_cast<T>(1)))
    {
        // singularity

        const T x = atan2(-m.columns[2].y, m.columns[2].z);
        const T y = static_cast<T>(0);

        return vec<3, T>(x, y, z);
    }
    else
    {
        // XZY Tait–Bryan angle decomposition

        const T x = atan2(m.columns[1].z, m.columns[1].y);
        const T y = atan2(m.columns[2].x, m.columns[0].x);

        return vec<3, T>(x, y, z);
    }
}

///////////////////////////////////////////////////////////////////////////////
// quat_from_euler_angles
///////////////////////////////////////////////////////////////////////////////

// https://github.com/mrdoob/three.js/blob/dev/src/math/Quaternion.js#L309

// xyz

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> quat_from_euler_angles_xyz(T x, T y, T z) noexcept
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
        (cx * cy * cz) - (sx * sy * sz),
        (sx * cy * cz) + (sy * sz * cx),
        (sy * cx * cz) - (sx * sz * cy),
        (sx * sy * cz) + (sz * cx * cy)
    );
}

// yxz

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> quat_from_euler_angles_yxz(T x, T y, T z) noexcept
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
        (cx * cy * cz) + (sx * sy * sz),
        (sx * cy * cz) + (cx * sy * sz),
        (cx * sy * cz) - (sx * cy * sz),
        (cx * cy * sz) - (sx * sy * cz)
    );
}

// zxy

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> quat_from_euler_angles_zxy(T x, T y, T z) noexcept
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
		(cx * cy * cz) - (sx * sy * sz),
		(sx * cy * cz) - (cx * sy * sz),
		(cx * sy * cz) + (sx * cy * sz),
		(cx * cy * sz) + (sx * sy * cz)
    );
}

// zyx

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> quat_from_euler_angles_zyx(T x, T y, T z) noexcept
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
		(cx * cy * cz) + (sx * sy * sz),
		(sx * cy * cz) - (cx * sy * sz),
		(cx * sy * cz) + (sx * cy * sz),
		(cx * cy * sz) - (sx * sy * cz)
    );
}

// yzx

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> quat_from_euler_angles_yzx(T x, T y, T z) noexcept
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
		(cx * cy * cz) - (sx * sy * sz),
		(sx * cy * cz) + (cx * sy * sz),
		(cx * sy * cz) + (sx * cy * sz),
		(cx * cy * sz) - (sx * sy * cz)
    );
}

// xzy

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> quat_from_euler_angles_xzy(T x, T y, T z) noexcept
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
		(cx * cy * cz) + (sx * sy * sz),
		(sx * cy * cz) - (cx * sy * sz),
		(cx * sy * cz) - (sx * cy * sz),
		(cx * cy * sz) + (sx * sy * cz)
    );
}

///////////////////////////////////////////////////////////////////////////////
// mat3_from_euler_angles
///////////////////////////////////////////////////////////////////////////////

// xyz

// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<3, 3, T> mat3_from_euler_angles_xyz(T x, T y, T z) noexcept
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

///////////////////////////////////////////////////////////////////////////////
// mat4_from_euler_angles_xyz
///////////////////////////////////////////////////////////////////////////////

// xyz

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> mat4_from_euler_angles_xyz(T x, T y, T z) noexcept
{
    const T cx = cos(x);
    const T cy = cos(y);
    const T cz = cos(z);

    const T sx = sin(x);
    const T sy = sin(y);
    const T sz = sin(z);

    return mat<4, 4, T>(
        +(cy * cz),
        +(sx * sy * cz) + (cx * sz),
        -(cx * sy * cz) + (sx * sz),
        static_cast<T>(0),

        -(cy * sz),
        -(sx * sy * sz) + (cx * cz),
        +(cx * sy * sz) + (sx * cz),
        static_cast<T>(0),

        +(sy),
        -(sx * cy),
        +(cx * cy),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

} // namespace math
} // namespace vx