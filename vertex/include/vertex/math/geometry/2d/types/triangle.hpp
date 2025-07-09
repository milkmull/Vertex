#pragma once

#include "./point.hpp"

namespace vx {
namespace math {
namespace geometry2d {

template <typename T>
struct triangle_t
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = triangle_t<scalar_type>;
    using point_type = point_t<scalar_type>;
    using line_type = line_t<scalar_type>;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    point_type a;
    point_type b;
    point_type c;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr triangle_t(const type& t) noexcept
        : a(t.a), b(t.b), c(t.c) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr triangle_t(
        scalar_type ax, scalar_type ay,
        scalar_type bx, scalar_type by,
        scalar_type cx, scalar_type cy
    ) noexcept
        : a(ax, ay), b(bx, by), c(cx, cy) {}

    VX_FORCE_INLINE constexpr triangle_t(
        const point_type& ta,
        const point_type& tb,
        const point_type& tc
    ) noexcept
        : a(ta), b(tb), c(tc) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <
        typename AX, typename AY,
        typename BX, typename BY,
        typename CX, typename CY
    >
    VX_FORCE_INLINE constexpr triangle_t(
        AX ax, AY ay,
        BX bx, BY by,
        CX cx, CY cy
    ) noexcept
        : a(ax, ay), b(bx, by), c(cx, cy) {}

    template <typename A, typename B, typename C>
    VX_FORCE_INLINE constexpr triangle_t(
        const point<A>& ta,
        const point<B>& tb,
        const point<C>& tc
    ) noexcept
        : a(ta), b(tb), c(tc) {}

    template <typename U>
    VX_FORCE_INLINE constexpr triangle_t(const triangle_t<U>& t) noexcept
        : a(t.a), b(t.b), c(t.c) {}

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& t) noexcept
    {
        a = t.a;
        b = t.b;
        c = t.c;
        return *this;
    }

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const triangle_t<U>& t) noexcept
    {
        a = t.a;
        b = t.b;
        c = t.c;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& t1, const type& t2) noexcept
    {
        return t1.a == t2.a && t1.b == t2.b && t1.c == t2.c;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& t1, const type& t2) noexcept
    {
        return t1.a != t2.a || t1.b != t2.b || t1.c != t2.c;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // attributes
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr size_t side_count() noexcept { return 3; }
    static VX_FORCE_INLINE constexpr size_t vertex_count() noexcept { return 3; }

    VX_FORCE_INLINE constexpr line_type get_side(size_t i) const noexcept
    {
        switch (i % side_count())
        {
            default:
            case 0:     return line_type(a, b);
            case 1:     return line_type(b, c);
            case 2:     return line_type(c, a);
        }
    }

    VX_FORCE_INLINE constexpr scalar_type area() const noexcept
    {
        return static_cast<scalar_type>(0.5) * abs(
            (a.x * (b.y - c.y)) +
            (b.x * (c.y - a.y)) +
            (c.x * (a.y - b.y))
        );
    }

    VX_FORCE_INLINE constexpr scalar_type perimeter() const noexcept
    {
        return length(b - a) + length(c - b) + length(a - c);
    }
};

} // namespace geometry2d
} // namespace math
} // namespace vx