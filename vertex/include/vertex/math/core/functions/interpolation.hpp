#pragma once

#include "./geometric.hpp"

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

//////////////////////////////////////////////////////////////////////////////
// step
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T>
VX_FORCE_INLINE constexpr T step(T edge, T x) noexcept
{
    return static_cast<T>(x >= edge);
}

// scalar vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> step(
    T edge,
    const vec<2, T>& x
) noexcept
{
    return vec<2, T>(
        step(edge, x.x),
        step(edge, x.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> step(
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

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> step(
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

template <typename T>
VX_FORCE_INLINE constexpr vec<2, T> step(
    const vec<2, T>& edge,
    const vec<2, T>& x
) noexcept
{
    return vec<2, T>(
        step(edge.x, x.x),
        step(edge.y, x.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> step(
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

template <typename T>
VX_FORCE_INLINE constexpr vec<4, T> step(
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

    const T alpha = acos(cos_alpha);
    const T sin_alpha = sin(alpha);
    const T inv_sin_alpha = static_cast<T>(1) / sin_alpha;

    const T t1 = sin((static_cast<T>(1) - t) * alpha) * inv_sin_alpha;
    const T t2 = sin(t * alpha) * inv_sin_alpha;

    return (x * t1) + (y * t2);
}

///////////////////////////////////////////////////////////////////////////////
// move_toward
///////////////////////////////////////////////////////////////////////////////

// scalar

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> move_toward(T from, T to, T delta) noexcept
{
    const T d = to - from;
    return (d <= delta) ? to : (from + delta);
}

// vec scalar

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<L, T> move_toward(
    const vec<L, T>& from,
    const vec<L, T>& to,
    T delta
) noexcept
{
    const vec<L, T> vd = to - from;
    const T d = length(vd);
    return (d <= delta) ? to : (from + (vd / d * delta));
}

// vec trinary

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> move_toward(
    const vec<2, T>& from,
    const vec<2, T>& to,
    const vec<2, T>& delta
) noexcept
{
    return vec<2, T>(
        move_toward(from.x, to.x, delta.x),
        move_toward(from.y, to.y, delta.y)
    );
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> move_toward(
    const vec<3, T>& from,
    const vec<3, T>& to,
    const vec<3, T>& delta
) noexcept
{
    return vec<3, T>(
        move_toward(from.x, to.x, delta.x),
        move_toward(from.y, to.y, delta.y),
        move_toward(from.z, to.z, delta.z)
    );
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> move_toward(
    const vec<4, T>& from,
    const vec<4, T>& to,
    const vec<4, T>& delta
) noexcept
{
    return vec<4, T>(
        move_toward(from.x, to.x, delta.x),
        move_toward(from.y, to.y, delta.y),
        move_toward(from.z, to.z, delta.z),
        move_toward(from.w, to.w, delta.w)
    );
}

} // namespace math
} // namespace vx