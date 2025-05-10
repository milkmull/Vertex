#pragma once

#include "fn_interpolation.h"
#include "vec_fn_geometric.h"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// lerp
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> lerp(
    const vec<L, T>& x,
    const vec<L, T>& y,
    T t
)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> lerp(
    const vec<L, T>& x,
    const vec<L, T>& y,
    const vec<L, T>& t
)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

///////////////////////////////////////////////////////////////////////////////
// mix
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> mix(
    const vec<L, T>& x,
    const vec<L, T>& y,
    T t
)
{
    return lerp(x, y, t);
}

template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> mix(
    const vec<L, T>& x,
    const vec<L, T>& y,
    const vec<L, T>& t
)
{
    return lerp(x, y, t);
}

///////////////////////////////////////////////////////////////////////////////
// step
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr auto step(
    T edge,
    const vec<2, T>& x
)
{
    return vec<2, T>(
        step(edge, x.x),
        step(edge, x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr auto step(
    T edge,
    const vec<3, T>& x
)
{
    return vec<3, T>(
        step(edge, x.x),
        step(edge, x.y),
        step(edge, x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr auto step(
    T edge,
    const vec<4, T>& x
)
{
    return vec<4, T>(
        step(edge, x.x),
        step(edge, x.y),
        step(edge, x.z),
        step(edge, x.w)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr auto step(
    const vec<2, T>& edge,
    const vec<2, T>& x
)
{
    return vec<2, T>(
        step(edge.x, x.x),
        step(edge.y, x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr auto step(
    const vec<3, T>& edge,
    const vec<3, T>& x
)
{
    return vec<3, T>(
        step(edge.x, x.x),
        step(edge.y, x.y),
        step(edge.z, x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr auto step(
    const vec<4, T>& edge,
    const vec<4, T>& x
)
{
    return vec<4, T>(
        step(edge.x, x.x),
        step(edge.y, x.y),
        step(edge.z, x.z),
        step(edge.w, x.w)
    );
}

///////////////////////////////////////////////////////////////////////////////
// smoothstep
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> smoothstep(
    T edge0,
    T edge1,
    const vec<L, T>& x)
{
    const vec<L, T> v = (x - edge0) / (edge1 - edge0);
    const vec<L, T> t = math::clamp(v, static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> smoothstep(
    const vec<L, T>& edge0,
    const vec<L, T>& edge1,
    const vec<L, T>& x
)
{
    const vec<L, T> v = (x - edge0) / (edge1 - edge0);
    const vec<L, T> t = math::clamp(v, static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

///////////////////////////////////////////////////////////////////////////////
// slerp
///////////////////////////////////////////////////////////////////////////////

// https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm

///////////////////////////////////////////////////////////////////////////////
/// @brief Performs spherical linear interpolation (SLERP) between two vectors.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param x The start vector.
/// @param y The end vector.
/// @param t The interpolation parameter, where t is in the range [0, 1].
/// 
/// @return The interpolated vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> slerp(
    const vec<L, T>& x,
    const vec<L, T>& y,
    T t
)
{
    const T cos_alpha = math::normalized_dot(x, y);

    if (cos_alpha > static_cast<T>(1) - epsilon<T>)
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

}
}