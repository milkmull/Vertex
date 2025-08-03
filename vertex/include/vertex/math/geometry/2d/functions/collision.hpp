#pragma once

#include "vertex/math/geometry/2d/types.hpp"

namespace vx {
namespace math {
namespace geometry2d {

///////////////////////////////////////////////////////////////////////////////
// move
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr point_t<T> move(const point_t<T>& p, T dx, T dy) noexcept
{
    return point_t<T>(p.x + dx, p.y + dy);
}

template <typename T>
VX_FORCE_INLINE constexpr line_t<T> move(const line_t<T>& l, T dx, T dy) noexcept
{
    return line_t<T>(l.a.x + dx, l.a.y + dy, l.b.x + dx, l.b.y + dy);
}

template <typename T>
VX_FORCE_INLINE constexpr rect_t<T> move(const rect_t<T>& r, T dx, T dy) noexcept
{
    return rect_t<T>(r.position.x + dx, r.position.y + dy, r.size.x, r.size.y);
}

template <typename T>
VX_FORCE_INLINE constexpr circle_t<T> move(const circle_t<T>& c, T dx, T dy) noexcept
{
    return circle_t<T>(c.center.x + dx, c.center.y + dy, c.radius);
}

template <typename T>
VX_FORCE_INLINE constexpr triangle_t<T> move(const triangle_t<T>& t, T dx, T dy) noexcept
{
    return triangle_t<T>(
        t.a.x + dx, t.a.y + dy,
        t.b.x + dx, t.b.y + dy,
        t.c.x + dx, t.c.y + dy
    );
}

template <typename T>
VX_FORCE_INLINE constexpr ray_t<T> move(const ray_t<T>& r, T dx, T dy) noexcept
{
    return ray_t<T>(r.origin.x + dx, r.origin.y + dy, r.direction.x, r.direction.y);
}

///////////////////////////////////////////////////////////////////////////////
// normalize
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr rect_t<T> normalize(const rect_t<T>& r) noexcept
{
    rect_t<T> nr = r;

    if (r.size.x < static_cast_t<T>(0))
    {
        nr.position.x += r.size.x;
        nr.size.x = -r.size.x;
    }
    if (r.size.y < static_cast_t<T>(0))
    {
        nr.position.y += r.size.y;
        nr.size.y = -r.size.y;
    }

    return nr;
}

template <typename T>
VX_FORCE_INLINE constexpr ray_t<T> normalize(const ray_t<T>& r) noexcept
{
    return ray_t<T>(r.origin, normalize(r.direction));
}

///////////////////////////////////////////////////////////////////////////////
// scale
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr rect_t<T> scale(const rect_t<T>& r, T sx, T sy) noexcept
{
    return rect_t<T>(
        r.position.x,
        r.position.y,
        r.size.x * sx,
        r.size.y * sy
    );
}

///////////////////////////////////////////////////////////////////////////////
// inflate
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr rect_t<T> inflate(const rect_t<T>& r, T ix, T iy) noexcept
{
    return rect_t<T>(
        r.position.x - ix,
        r.position.y - iy,
        r.size.x + static_cast_t<T>(2) * ix,
        r.size.y + static_cast_t<T>(2) * iy
    );
}

///////////////////////////////////////////////////////////////////////////////
// grow_sides
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr rect_t<T> grow_sides(const rect_t<T>& r, T left, T right, T top, T bottom) noexcept
{
    return rect_t<T>(
        r.position.x - left,
        r.position.y - top,
        r.size.x + left + right,
        r.size.y + bottom + top
    );
}

///////////////////////////////////////////////////////////////////////////////
// crop
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr rect_t<T> crop(const rect_t<T>& bounds, const rect_t<T>& r) noexcept
{
    const T nl = clamp(r.position.x, bounds.position.x, bounds.right());
    const T nr = clamp(r.right(), bounds.position.x, bounds.right());

    const T nt = clamp(r.position.y, bounds.position.y, bounds.bottom());
    const T nb = clamp(r.bottom(), bounds.position.y, bounds.bottom());

    return rect_t<T>(
        nl,
        nt,
        nr - nl,
        nb - nt
    );
}

///////////////////////////////////////////////////////////////////////////////
// bounding_box
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr rect_t<T> bounding_box(const rect_t<T>& r1, const rect_t<T>& r2) noexcept
{
    const T nl = min(r1.position.x, r2.position.x);
    const T nr = max(r1.right(), r2.right());

    const T nt = min(r1.position.y, r2.position.y);
    const T nb = max(r1.bottom(), r2.bottom());

    return rect_t<T>(
        nl,
        nt,
        nr - nl,
        nb - nt
    );
}

template <typename T>
VX_FORCE_INLINE constexpr rect_t<T> bounding_box(const rect_t<T>& r, const point_t<T>& p) noexcept
{
    const T nl = min(r.position.x, p.x);
    const T nr = max(r.right(), p.x);

    const T nt = min(r.position.y, p.y);
    const T nb = max(r.bottom(), p.y);

    return rect_t<T>(
        nl,
        nt,
        nr - nl,
        nb - nt
    );
}

///////////////////////////////////////////////////////////////////////////////
// contains
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool contains(const rect_t<T>& r1, const rect_t<T>& r2) noexcept
{
    return r1.position.x <= r2.position.x
        && r1.position.x + r1.size.x > r2.position.x + r2.size.x
        && r1.position.y <= r2.position.y
        && r1.position.y + r1.size.y > r2.position.y + r2.size.y;
}

template <typename T>
VX_FORCE_INLINE constexpr bool contains(const rect_t<T>& r, const point_t<T>& p)
{
    return r.position.x <= p.x
        && r.position.x + r.size.x > p.x
        && r.position.y <= p.y
        && r.position.y + r.size.y > p.y;
}

///////////////////////////////////////////////////////////////////////////////
// overlaps
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool overlaps(const rect_t<T>& r1, const rect_t<T>& r2) noexcept
{
    return r1.position.x <= r2.position.x + r2.size.x
        && r1.position.x + r1.size.x > r2.position.x
        && r1.position.y <= r2.position.y + r2.size.y
        && r1.position.y + r1.size.y > r2.position.y;
}

template <typename T>
VX_FORCE_INLINE constexpr bool overlaps(const rect_t<T>& r, const vec<2, T>& p) noexcept
{
    return contains(r, p);
}

///////////////////////////////////////////////////////////////////////////////
// closest
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr point_t<T> closest(const rect_t<T>& r, const point_t<T>& p) noexcept
{
    return point_t<T>(
        clamp(p.x, r.left(), r.right()),
        clamp(p.y, r.top(), r.bottom())
    );
}

///////////////////////////////////////////////////////////////////////////////
// intersects
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// collision
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// reflect
///////////////////////////////////////////////////////////////////////////////

} // namespace geometry2d
} // namespace math
} // namespace vx