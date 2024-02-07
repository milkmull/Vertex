#pragma once

#include "math.h"

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
inline constexpr T sd_rounded_box(const detail::vecf<2, T>& p, const detail::vecf<2, T>& b, const detail::vecf<4, T>& r)
{
    detail::vecf<4, T> ir(r);

    if (p.x > static_cast<T>(0))
    {
        ir.x = ir.z;
        ir.y = ir.w;
    }
    if (p.y > static_cast<T>(0))
    {
        ir.x = ir.y;
    }

    const detail::vecf<2, T> q = abs(p) - b + ir.x;
    return min(max(q.x, q.y), static_cast<T>(0)) + length(max(q, static_cast<T>(0))) - ir.x;
}

template <typename T>
inline constexpr T sd_box(const detail::vecf<2, T>& p, const detail::vecf<2, T>& b)
{

    const detail::vecf<2, T> d = math::abs(p) - b;
    return length(max(d, static_cast<T>(0))) + min(max(d.x, d.y), static_cast<T>(0));
}

template <typename T>
inline constexpr T sd_segment(const detail::vecf<2, T>& p, const detail::vecf<2, T>& a, const detail::vecf<2, T>& b)
{
    const detail::vecf<2, T> pa = p - a, ba = b - a;
    const float h = clamp(dot(pa, ba) / dot(ba, ba), static_cast<T>(0), static_cast<T>(1));
    return length(pa - ba * h);
}

template <typename T>
T sd_equilateral_triangle(const detail::vecf<2, T>& p, T r)
{
    detail::vecf<2, T> ip(p);

    const float k = sqrt(static_cast<T>(3));

    ip.x = abs(ip.x) - r;
    ip.y = ip.y + r / k;

    if (ip.x + k * ip.y > static_cast<T>(0))
    {
        ip = detail::vecf<2, T>(ip.x - k * ip.y, -k * ip.x - ip.y) / static_cast<T>(2);
    }
    ip.x -= clamp(ip.x, static_cast<T>(-2) * r, static_cast<T>(0));

    return -length(ip) * sign(ip.y);
}

template <typename T>
T sd_isosceles_triangle(const detail::vecf<2, T>& p, const detail::vecf<2, T>& q)
{
    detail::vecf<2, T> ip(p);

    ip.x = abs(ip.x);

    const detail::vecf<2, T> a = ip - q * clamp(dot(ip, q) / dot(q, q), static_cast<T>(0), static_cast<T>(1));
    const detail::vecf<2, T> b = ip - q * detail::vecf<2, T>(clamp(ip.x / q.x, static_cast<T>(0), static_cast<T>(1)), static_cast<T>(1));

    const T s = -sign(q.y);
    const detail::vecf<2, T> d = min(
        detail::vecf<2, T>(dot(a, a), s * (ip.x * q.y - ip.y * q.x)),
        detail::vecf<2, T>(dot(b, b), s * (ip.y - q.y))
    );

    return -sqrt(d.x) * sign(d.y);
}

template <typename T>
float sd_triangle(
    const detail::vecf<2, T>& p,
    const detail::vecf<2, T>& p0,
    const detail::vecf<2, T>& p1,
    const detail::vecf<2, T>& p2
)
{
    const detail::vecf<2, T> e0 = p1 - p0, e1 = p2 - p1, e2 = p0 - p2;
    const detail::vecf<2, T> v0 = p  - p0, v1 = p  - p1, v2 = p  - p2;

    const detail::vecf<2, T> pq0 = v0 - e0 * clamp(dot(v0, e0) / dot(e0, e0), static_cast<T>(0), static_cast<T>(1));
    const detail::vecf<2, T> pq1 = v1 - e1 * clamp(dot(v1, e1) / dot(e1, e1), static_cast<T>(0), static_cast<T>(1));
    const detail::vecf<2, T> pq2 = v2 - e2 * clamp(dot(v2, e2) / dot(e2, e2), static_cast<T>(0), static_cast<T>(1));

    const T s = sign(e0.x * e2.y - e0.y * e2.x);

    const detail::vecf<2, T> d = min({
        detail::vecf<2, T>(dot(pq0, pq0), s * (v0.x * e0.y - v0.y * e0.x)),
        detail::vecf<2, T>(dot(pq1, pq1), s * (v1.x * e1.y - v1.y * e1.x)),
        detail::vecf<2, T>(dot(pq2, pq2), s * (v2.x * e2.y - v2.y * e2.x))
    });

    return -sqrt(d.x) * sign(d.y);
}

// sc is the sin/cos of the arc's aperture

template <typename T>
T sd_pie(const detail::vecf<2, T>& p, const detail::vecf<2, T>& sc, T r) // use a 2x2 rotation matrix to rotate p to get rotated version
{
    detail::vecf<2, T> ip(p);

    ip.x = abs(ip.x);
    const float l = length(ip) - r;
    const float m = length(ip - sc * clamp(dot(ip, sc), static_cast<T>(0), r)); // sc is the sin/cos of the arc's aperture
    return max(l, m * sign(sc.y * ip.x - sc.x * ip.y));
}

template <typename T>
T sd_arc(const detail::vecf<2, T>& p, const detail::vecf<2, T>& sc, T ra, T rb) // sc is the sin/cos of the arc's aperture
{
    detail::vecf<2, T> ip(p);
    
    ip.x = abs(ip.x);
    return ((sc.y * ip.x > sc.x * ip.y) ? length(ip - sc * ra) : abs(length(ip) - ra)) - rb;
}

template <typename T>
T sd_ring(const detail::vecf<2, T>& p, const detail::vecf<2, T>& sc, T r, T t)
{
    detail::vecf<2, T> ip(p);

    ip.x = abs(ip.x);
    ip = detail::mat2x<T>(sc.x, sc.y, -sc.y, sc.x) * ip;

    return max(
        abs(length(ip) - r) - t * static_cast<T>(0.5),
        length(detail::vecf<2, T>(ip.x, max(static_cast<T>(0), abs(r - ip.y) - t * static_cast<T>(0.5)))) * sign(ip.x)
    );
}

}
}
}