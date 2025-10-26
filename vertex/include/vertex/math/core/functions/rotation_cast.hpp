#pragma once

#include "vertex/math/core/functions/quaternion.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// quat to mat3
///////////////////////////////////////////////////////////////////////////////

template <typename To, typename T, VX_MATH_REQ(is_float_mat3<To>::value)>
VX_FORCE_INLINE constexpr To rotation_cast(const quat_t<T>& q) noexcept
{
    using mat_type = mat<3, 3, typename To::scalar_type>;

    const T qxx = q.x * q.x;
    const T qyy = q.y * q.y;
    const T qzz = q.z * q.z;
    const T qxz = q.x * q.z;
    const T qxy = q.x * q.y;
    const T qyz = q.y * q.z;
    const T qwx = q.w * q.x;
    const T qwy = q.w * q.y;
    const T qwz = q.w * q.z;

    return mat_type(
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

///////////////////////////////////////////////////////////////////////////////
// quat to mat4
///////////////////////////////////////////////////////////////////////////////

template <typename To, typename T, VX_MATH_REQ(is_float_mat4<To>::value)>
VX_FORCE_INLINE constexpr To rotation_cast(const quat_t<T>& q) noexcept
{
    using mat_type = mat<4, 4, typename To::scalar_type>;

    const T qxx = q.x * q.x;
    const T qyy = q.y * q.y;
    const T qzz = q.z * q.z;
    const T qxz = q.x * q.z;
    const T qxy = q.x * q.y;
    const T qyz = q.y * q.z;
    const T qwx = q.w * q.x;
    const T qwy = q.w * q.y;
    const T qwz = q.w * q.z;

    return mat_type(
        static_cast<T>(1) - static_cast<T>(2) * (qyy + qzz),
        static_cast<T>(2) * (qxy + qwz),
        static_cast<T>(2) * (qxz - qwy),
        static_cast<T>(0),

        static_cast<T>(2) * (qxy - qwz),
        static_cast<T>(1) - static_cast<T>(2) * (qxx + qzz),
        static_cast<T>(2) * (qyz + qwx),
        static_cast<T>(0),

        static_cast<T>(2) * (qxz + qwy),
        static_cast<T>(2) * (qyz - qwx),
        static_cast<T>(1) - static_cast<T>(2) * (qxx + qyy),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////
// mat3 or mat4 to quat
///////////////////////////////////////////////////////////////////////////////

template <typename To, size_t M, typename T, VX_MATH_REQ(is_quat<To>::value && (M == 3 || M == 4) && is_float<T>::value)>
VX_FORCE_INLINE constexpr To rotation_cast(const mat<M, M, T>& m) noexcept
{
    using quat_type = quat_t<typename To::scalar_type>;

    const T trace = m.columns[0].x + m.columns[1].y + m.columns[2].z;

    if (trace > static_cast<T>(0))
    {
        const T s = sqrt(static_cast<T>(1) + trace) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_type(
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

        return quat_type(
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

        return quat_type(
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

        return quat_type(
            (m.columns[0].y - m.columns[1].x) * invs,
            (m.columns[2].x + m.columns[0].z) * invs,
            (m.columns[2].y + m.columns[1].z) * invs,
            s * static_cast<T>(0.25)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
// mat3 to mat4
///////////////////////////////////////////////////////////////////////////////

template <typename To, typename T, VX_MATH_REQ(is_float_mat4<To>::value && is_float<T>::value)>
VX_FORCE_INLINE constexpr To rotation_cast(const mat<3, 3, T>& m) noexcept
{
    using mat_type = mat<4, 4, typename To::scalar_type>;

    return mat_type(
        m.columns[0].x,
        m.columns[0].y,
        m.columns[0].z,
        static_cast<T>(0),

        m.columns[1].x,
        m.columns[1].y,
        m.columns[1].z,
        static_cast<T>(0),

        m.columns[2].x,
        m.columns[2].y,
        m.columns[2].z,
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////
// mat4 to mat3
///////////////////////////////////////////////////////////////////////////////

template <typename To, typename T, VX_MATH_REQ(is_float_mat3<To>::value && is_float<T>::value)>
VX_FORCE_INLINE constexpr To rotation_cast(const mat<4, 4, T>& m) noexcept
{
    using mat_type = mat<3, 3, typename To::scalar_type>;
    return mat_type(m);
}

///////////////////////////////////////////////////////////////////////////////
// identity casts
///////////////////////////////////////////////////////////////////////////////

template <typename To, typename T, VX_MATH_REQ(is_quat<To>::value)>
VX_FORCE_INLINE constexpr To rotation_cast(const quat_t<T>& q) noexcept
{
    using quat_type = quat_t<typename To::scalar_type>;
    return quat_type(q);
}

template <typename To, typename T, VX_MATH_REQ(is_float_mat3<To>::value && is_float<T>::value)>
VX_FORCE_INLINE constexpr To rotation_cast(const mat<3, 3, T>& m) noexcept
{
    using mat3_type = mat<3, 3, typename To::scalar_type>;
    return mat3_type(m);
}

template <typename To, typename T, VX_MATH_REQ(is_float_mat4<To>::value&& is_float<T>::value)>
VX_FORCE_INLINE constexpr To rotation_cast(const mat<4, 4, T>& m) noexcept
{
    using mat4_type = mat<4, 4, typename To::scalar_type>;
    return mat4_type(m);
}

} // namespace math
} // namespace vx