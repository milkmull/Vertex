#pragma once

#include "../functions/rotation_cast.hpp"
#include "../functions/matrix.hpp"

namespace vx {
namespace math {
namespace transform3d {

///////////////////////////////////////////////////////////////////////////////
// translation
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> translate(const vec<3, T>& translation) noexcept
{
    return mat<4, 4, T>(
        static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
        translation.x,     translation.y,     translation.z,     static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> extract_translation(const mat<4, 4, T>& m) noexcept
{
    return vec<3, T>(m.columns[3].x, m.columns[3].y, m.columns[3].z);
}

///////////////////////////////////////////////////////////////////////////////
// rotation
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> rotate(const vec<3, T>& axis, T angle) noexcept
{
    const T cosa = cos(angle);
    const T sina = sin(angle);

    const vec<3, T> t = (static_cast<T>(1) - cosa) * axis;
    const vec<3, T> sina_axis = sina * axis;

    return mat<4, 4, T>(
        (t.x * axis.x) + cosa,
        (t.x * axis.y) + sina_axis.z,
        (t.x * axis.z) - sina_axis.y,
        static_cast<T>(0),

        (t.y * axis.x) - sina_axis.z,
        (t.y * axis.y) + cosa,
        (t.y * axis.z) + sina_axis.x,
        static_cast<T>(0),

        (t.z * axis.x) + sina_axis.y,
        (t.z * axis.y) - sina_axis.x,
        (t.z * axis.z) + cosa,
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> rotate(const quat_t<T>& q) noexcept
{
    return rotation_cast<mat<4, 4, T>>(q);
}

///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> extract_rotation(const mat<4, 4, T>& m) noexcept
{
    const vec3 c0(m.columns[0]);
    const vec3 c1(m.columns[1]);
    const vec3 c2(m.columns[2]);

    const mat3 m3(
        normalize(c0),
        normalize(c1),
        normalize(c2)
    );

    return rotation_cast<quat_t<T>>(m3);
}

///////////////////////////////////////////////////////////////////////////////
// scale
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> scale(const vec<3, T>& scale) noexcept
{
    return mat<4, 4, T>(
        scale.x,
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        scale.y,
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        scale.z,
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> extract_scale(const mat<4, 4, T>& m) noexcept
{
    return vec<3, T>(
        length(m.columns[0]),
        length(m.columns[1]),
        length(m.columns[2])
    );
}

///////////////////////////////////////////////////////////////////////////////
// trs
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> trs(
    const vec<3, T>& translation,
    const vec<3, T>& axis, T angle,
    const vec<3, T>& scale
) noexcept
{
    const T cosa = cos(angle);
    const T sina = sin(angle);

    const vec<3, T> t = (static_cast<T>(1) - cosa) * axis;
    const vec<3, T> sina_axis = sina * axis;

    return mat<4, 4, T>(
        scale.x * ((t.x * axis.x) + cosa),
        scale.x * ((t.x * axis.y) + sina_axis.z),
        scale.x * ((t.x * axis.z) - sina_axis.y),
        static_cast<T>(0),

        scale.y * ((t.y * axis.x) - sina_axis.z),
        scale.y * ((t.y * axis.y) + cosa),
        scale.y * ((t.y * axis.z) + sina_axis.x),
        static_cast<T>(0),

        scale.z * ((t.z * axis.x) + sina_axis.y),
        scale.z * ((t.z * axis.y) - sina_axis.x),
        scale.z * ((t.z * axis.z) + cosa),
        static_cast<T>(0),

        translation.x,
        translation.y,
        translation.z,
        static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
static VX_FORCE_INLINE constexpr mat<4, 4, T> trs(
    const vec<3, T>& translation,
    const quat_t<T>& q,
    const vec<3, T>& scale
) noexcept
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

    return mat<4, 4, T>(
        scale.x * (static_cast<T>(1) - (static_cast<T>(2) * (qyy + qzz))),
        scale.x * (static_cast<T>(2) * (qxy + qwz)),
        scale.x * (static_cast<T>(2) * (qxz - qwy)),
        static_cast<T>(0),

        scale.y * (static_cast<T>(2) * (qxy - qwz)),
        scale.y * (static_cast<T>(1) - (static_cast<T>(2) * (qxx + qzz))),
        scale.y * (static_cast<T>(2) * (qyz + qwx)),
        static_cast<T>(0),

        scale.z * (static_cast<T>(2) * (qxz + qwy)),
        scale.z * (static_cast<T>(2) * (qyz - qwx)),
        scale.z * (static_cast<T>(1) - (static_cast<T>(2) * (qxx + qyy))),
        static_cast<T>(0),

        translation.x,
        translation.y,
        translation.z,
        static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////
// transform
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> transform(const mat<4, 4, T>& m, const vec<3, T>& v) noexcept
{
    return vec<3, T>(
        (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + (m.columns[2].x * v.z) + m.columns[3].x,
        (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z) + m.columns[3].y,
        (m.columns[0].z * v.x) + (m.columns[1].z * v.y) + (m.columns[2].z * v.z) + m.columns[3].z
    );
}

///////////////////////////////////////////////////////////////////////////////
// inverse
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> inverse(const mat<4, 4, T>& m) noexcept
{
    const mat<3, 3, T> ibasis = inverse(mat<3, 3, T>(m));

    return mat<4, 4, T>(
        vec<4, T>(ibasis.columns[0], static_cast<T>(0)),
        vec<4, T>(ibasis.columns[1], static_cast<T>(0)),
        vec<4, T>(ibasis.columns[2], static_cast<T>(0)),
        vec<4, T>(ibasis * vec<3, T>(-m.columns[3]), static_cast<T>(1))
    );
}

///////////////////////////////////////////////////////////////////////////////
// look_at (matrix)
///////////////////////////////////////////////////////////////////////////////

// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> look_at_lh(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::up()
) noexcept
{
    const vec<3, T> z = normalize(target - eye);
    const vec<3, T> x = normalize(cross(up, z));
    const vec<3, T> y = cross(z, x);

    return mat<4, 4, T>(
        x.x,
        y.x,
        z.x,
        static_cast<T>(0),

        x.y,
        y.y,
        z.y,
        static_cast<T>(0),

        x.z,
        y.z,
        z.z,
        static_cast<T>(0),

        -dot(x, eye),
        -dot(y, eye),
        -dot(z, eye),
        static_cast<T>(1)
    );
}

////////////////////////////////////////////////////////////////////////////////

// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatrh

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> look_at_rh(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::up()
) noexcept
{
    // In a right handed coordinate system, the camera is looking down the
    // -z axis, so to get a vector from the front of the camera to the
    // target, we reverse the subtraction.
    const vec<3, T> z = normalize(eye - target);
    const vec<3, T> x = normalize(cross(up, z));
    const vec<3, T> y = cross(z, x);

    return mat<4, 4, T>(
        x.x,
        y.x,
        z.x,
        static_cast<T>(0),

        x.y,
        y.y,
        z.y,
        static_cast<T>(0),

        x.z,
        y.z,
        z.z,
        static_cast<T>(0),

        -dot(x, eye),
        -dot(y, eye),
        -dot(z, eye),
        static_cast<T>(1)
    );
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
static VX_FORCE_INLINE constexpr mat<4, 4, T> look_at(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::up()
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return look_at_lh(eye, target, up);
#else
    return look_at_rh(eye, target, up);
#endif
}

} // namespace vx
} // namespace math
} // namespace transform3d