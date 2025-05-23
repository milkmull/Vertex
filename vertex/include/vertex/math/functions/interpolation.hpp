#pragma once

#include "vertex/math/functions/geometric.hpp"

namespace vx {
namespace math {

//////////////////////////////////////////////////////////////////////////////
// lerp
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T lerp(T x, T y, T t) noexcept
{
    return x * (static_cast<T>(1) - t) + y * t;
}

// vec scalar

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> lerp(
    const vec<L, T>& x,
    const vec<L, T>& y,
    T t
) noexcept
{
    return x * (static_cast<T>(1) - t) + y * t;
}

// vec trinary

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> lerp(
    const vec<L, T>& x,
    const vec<L, T>& y,
    const vec<L, T>& t
) noexcept
{
    return x * (static_cast<T>(1) - t) + y * t;
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> lerp(
    const quat_t<T>& x,
    const quat_t<T>& y,
    T t
) noexcept
{
    return x * (static_cast<T>(1) - t) + y * t;
}

//////////////////////////////////////////////////////////////////////////////
// mix
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T mix(T x, T y, T t) noexcept
{
    return lerp(x, y, t);
}

// vec scalar

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> mix(
    const vec<L, T>& x,
    const vec<L, T>& y,
    T t
) noexcept
{
    return lerp(x, y, t);
}

// vec trinary

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> mix(
    const vec<L, T>& x,
    const vec<L, T>& y,
    const vec<L, T>& t
) noexcept
{
    return lerp(x, y, t);
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> mix(
    const quat_t<T>& x,
    const quat_t<T>& y,
    T t
) noexcept
{
    return lerp(x, y, t);
}

//////////////////////////////////////////////////////////////////////////////
// step
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr T step(T edge, T x) noexcept
{
    return static_cast<T>(x >= edge);
}

// scalar vec

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr auto step(
    T edge,
    const vec<2, T>& x
) noexcept
{
    return vec<2, T>(
        step(edge, x.x),
        step(edge, x.y)
    );
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr auto step(
    T edge,
    const vec<3, T>& x
) noexcept
{
    return vec<3, T>(
        step(edge, x.x),
        step(edge, x.y),
        step(edge, x.z)
    );
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr auto step(
    T edge,
    const vec<4, T>& x
) noexcept
{
    return vec<4, T>(
        step(edge, x.x),
        step(edge, x.y),
        step(edge, x.z),
        step(edge, x.w)
    );
}

// vec binary

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr auto step(
    const vec<2, T>& edge,
    const vec<2, T>& x
) noexcept
{
    return vec<2, T>(
        step(edge.x, x.x),
        step(edge.y, x.y)
    );
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr auto step(
    const vec<3, T>& edge,
    const vec<3, T>& x
) noexcept
{
    return vec<3, T>(
        step(edge.x, x.x),
        step(edge.y, x.y),
        step(edge.z, x.z)
    );
}

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr auto step(
    const vec<4, T>& edge,
    const vec<4, T>& x
) noexcept
{
    return vec<4, T>(
        step(edge.x, x.x),
        step(edge.y, x.y),
        step(edge.z, x.z),
        step(edge.w, x.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// smoothstep
//////////////////////////////////////////////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/smoothstep.xhtml

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T smoothstep(T edge0, T edge1, T x) noexcept
{
    const T v = (x - edge0) / (edge1 - edge0);
    T t = v < static_cast<T>(0) ? static_cast<T>(0) : v;
    t = v > static_cast<T>(1) ? static_cast<T>(1) : v;
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

// vec scalar

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> smoothstep(
    T edge0,
    T edge1,
    const vec<L, T>& x)
{
    const vec<L, T> v = (x - edge0) / (edge1 - edge0);
    const vec<L, T> t = clamp(v, static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

// vec trinary

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> smoothstep(
    const vec<L, T>& edge0,
    const vec<L, T>& edge1,
    const vec<L, T>& x
)
{
    const vec<L, T> v = (x - edge0) / (edge1 - edge0);
    const vec<L, T> t = clamp(v, static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

///////////////////////////////////////////////////////////////////////////////
// slerp
///////////////////////////////////////////////////////////////////////////////

// https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
// https://en.wikipedia.org/wiki/Slerp

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> slerp(
    const vec<L, T>& x,
    const vec<L, T>& y,
    T t
) noexcept
{
    const T cos_alpha = normalized_dot(x, y);

    if (cos_alpha >= static_cast<T>(1) - constants<T>::epsilon)
    {
        // If the angle between the vectors is super small, we
        // can estimate with linear interpolation. This also helps
        // to avoid dividing by 0 later since acos(1) == sin(0) == 0.

        return lerp(x, y, t);
    }

    const T alpha = math::acos(cos_alpha);
    const T sin_alpha = math::sin(alpha);
    const T inv_sin_alpha = static_cast<T>(1) / sin_alpha;

    const T t1 = math::sin((static_cast<T>(1) - t) * alpha) * inv_sin_alpha;
    const T t2 = math::sin(t * alpha) * inv_sin_alpha;

    return (x * t1) + (y * t2);
}

// assuming qx and qy are normalized
// theta = arccos(dot(qx, qy))
// qm = { qx * sin((1 - t) * theta) + qy * sin(t * theta) } / sin(theta)

template <typename T>
inline constexpr quat_t<T> slerp(
    const quat_t<T>& x,
    const quat_t<T>& y,
    T t
)
{
    T cos_alpha = normalized_dot(x, y);
    T xsign = static_cast<T>(1);

    if (cos_alpha < static_cast<T>(0))
    {
        // Since q(w,x,y,z) and q(-w,-x,-y,-z) represent the same
        // rotation we should make sure the result is not sensitive 
        // to whether the positive or inverted form of the
        // quaternion is used.
        // 
        // acos will return an angle between:
        // 
        // 0-90 degrees for positive inputs
        // 90-180 degrees for negative inputs
        // 
        // To ensure acos returns the smallest angle (or shortest
        // path around the hypersphere), if cos_alpha is negative,
        // we make it positive. To account for this, we also have
        // to negate one of the input quaternions to make the new
        // angle accurate.
        // 
        // It doesn't matter which quaternion we invert as the
        // result will still be the same.

        xsign = static_cast<T>(-1);
        cos_alpha = -cos_alpha;
    }

    if (cos_alpha >= static_cast<T>(1) - constants<T>::epsilon)
    {
        // If the angle between the quaternions is super small, we
        // can estimate with linear interpolation. This also helps
        // to avoid dividing by 0 later since acos(1) == sin(0) == 0.

        return lerp(xsign * x, y, t);
    }

    const T alpha = math::acos(cos_alpha);
    const T sin_alpha = math::sin(alpha);
    const T inv_sin_alpha = static_cast<T>(1) / sin_alpha;

    const T t1 = math::sin((static_cast<T>(1) - t) * alpha) * inv_sin_alpha;
    const T t2 = math::sin(t * alpha) * inv_sin_alpha;

    return (xsign * x * t1) + (y * t2);
}

} // namespace math
} // namespace vx