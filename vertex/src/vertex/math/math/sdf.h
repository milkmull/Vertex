#pragma once

#include "detail/base_type_defs.h"
#include "fn_common.h"
#include "vec_fn_geometric.h"

namespace vx {
namespace math {
namespace sdf {

// https://iquilezles.org/articles/distfunctions2d/

// =============== operators ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T op_round(T d, T r)
{
    return d - r;
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T op_annularize(T d, T r)
{
    return abs(d) - r;
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T op_union(T d1, T d2)
{
    return min(d1, d2);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T op_smooth_union(T d1, T d2, T k)
{
    float h = clamp(static_cast<T>(0.5) + static_cast<T>(0.5) * (d2 - d1) / k, static_cast<T>(0), static_cast<T>(1));
    return lerp(d2, d1, h) - k * h * (static_cast<T>(1) - h);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T op_subtract(T dist1, T dist2)
{
    return max(-dist1, dist2);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T op_smooth_subtraction(T d1, T d2, T k)
{
    float h = clamp(static_cast<T>(0.5) - static_cast<T>(0.5) * (d2 + d1) / k, static_cast<T>(0), static_cast<T>(1));
    return lerp(d2, -d1, h) + k * h * (static_cast<T>(1) - h);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T op_intersection(T d1, T d2)
{
    return max(d1, d2);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T op_smooth_itersection(T d1, T d2, T k)
{
    float h = clamp(static_cast<T>(0.5) - static_cast<T>(0.5) * (d2 - d1) / k, static_cast<T>(0), static_cast<T>(1));
    return lerp(d2, d1, h) + k * h * (static_cast<T>(1) - h);
}

// =============== sdfs ===============

template <typename T>
inline constexpr T sd_circle(const detail::vecf<2, T>& p, float r)
{
    return length(p) - r;
}

template <typename T>
inline constexpr T sd_rounded_box(const detail::vecf<2, T>& p, const detail::vecf<2, T>& b, detail::vecf<4, T> r)
{
    if (p.x > static_cast<T>(0))
    {
        r.x = r.z;
        r.y = r.w;
    }
    if (p.y > static_cast<T>(0))
    {
        r.x = r.y;
    }

    const detail::vecf<2, T> q = abs(p) - b + r.x;
    return min(max(q.x, q.y), static_cast<T>(0)) + length(max(q, static_cast<T>(0))) - r.x;
}

template <typename T>
inline constexpr T sd_box(const detail::vecf<2, T>& p, const detail::vecf<2, T>& b)
{

    const detail::vecf<2, T> d = math::abs(p) - b;
    return length(max(d, static_cast<T>(0))) + min(max(d.x, d.y), static_cast<T>(0));
}

template <typename T>
inline constexpr T sd_orientated_box(const detail::vecf<2, T>& p, const detail::vecf<2, T>& a, const detail::vecf<2, T>& b, T th)
{
    float l = length(b - a);
    detail::vecf<2, T> d = (b - a) / l;
    detail::vecf<2, T> q = (p - (a + b) * static_cast<T>(0.5));
    q = detail::mat2x<T>(d.x, -d.y, d.y, d.x) * q;
    q = abs(q) - detail::vecf<2, T>(l, th) * static_cast<T>(0.5);
    return length(max(q, static_cast<T>(0))) + min(max(q.x, q.y), static_cast<T>(0));
}

}
}
}