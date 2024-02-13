#pragma once

#include "fn_interpolation.h"
#include "vec_fn_geometric.h"

namespace vx {
namespace math {

// =============== lerp ===============

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr vec<L, T> lerp(
    const vec<L, T>& x,
    const vec<L, T>& y,
    T t
)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr vec<L, T> lerp(
    const vec<L, T>& x,
    const vec<L, T>& y,
    const vec<L, T>& t
)
{
    return x * (static_cast<T>(1) - t) + y * t;
}

// =============== step ===============

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr auto step(
    T edge,
    const vec<2, T>& x
)
{
    return vec<2, T>(
        step(edge, x.x),
        step(edge, x.y)
    );
}

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr auto step(
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

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr auto step(
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

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr auto step(
    const vec<2, T>& edge,
    const vec<2, T>& x
)
{
    return vec<2, T>(
        step(edge.x, x.x),
        step(edge.y, x.y)
    );
}

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr auto step(
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

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr auto step(
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

// =============== smoothstep ===============

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr vec<L, T> smoothstep(
    T edge0,
    T edge1,
    const vec<L, T>& x)
{
    const T v = (x - edge0) / (edge1 - edge0);
    const T t = std::clamp(v, static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr vec<L, T> smoothstep(
    const vec<L, T>& edge0,
    const vec<L, T>& edge1,
    const vec<L, T>& x
)
{
    const T v = (x - edge0) / (edge1 - edge0);
    const T t = std::clamp(v, static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

// =============== slerp ===============

/**
 * @brief Performs spherical linear interpolation ('slerp') between two normalized vectors.
 *
 * This function calculates the slerp interpolation between two normalized vectors 'x' and 'y'
 * based on the interpolation parameter 't'.
 *
 * @param x The starting normalized vector.
 * @param y The target normalized vector.
 * @param t Interpolation parameter in the range [0, 1].
 * @return The slerp interpolated vector.
 *
 * @note If the angle between the vectors is very small, linear interpolation
 * is used as an optimization to avoid numerical instability.
 *
 * @note The vectors 'x' and 'y' are assumed to be normalized before calling this function.
 * 
 * @ref https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool> = true>
inline constexpr vec<L, T> slerp(
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