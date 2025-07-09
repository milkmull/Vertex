#pragma once

#include "./point.hpp"

namespace vx {
namespace math {
namespace geometry2d {

template <typename T>
struct line_t
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = line_t<scalar_type>;
    using point_type = point_t<scalar_type>;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    point_type a;
    point_type b;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr line_t(const type& p) noexcept
        : a(p.a), b(p.b) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr line_t(
        scalar_type ax, scalar_type ay,
        scalar_type bx, scalar_type by
    ) noexcept
        : a(ax, ay), b(bx, by) {}

    VX_FORCE_INLINE constexpr line_t(
        const point_type& la,
        const point_type& lb
    ) noexcept
        : a(la), b(lb) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename AX, typename AY, typename BX, typename BY>
    VX_FORCE_INLINE constexpr line_t(AX ax, AY ay, BX bx, BY by) noexcept
        : a(ax, ay), b(bx, by) {}

    template <typename A, typename B>
    VX_FORCE_INLINE constexpr line_t(
        const point<A>& la,
        const point<B>& lb
    ) noexcept
        : a(la), b(lb) {}

    template <typename U>
    VX_FORCE_INLINE constexpr line_t(const line_t<U>& l) noexcept
        : a(l.a), b(l.b) {}

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& l) noexcept
    {
        a = l.a;
        b = l.b;
        return *this;
    }

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const line_t<U>& l) noexcept
    {
        a = l.a;
        b = l.b;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& l1, const type& l2) noexcept
    {
        return l1.a == l2.a && l1.b == l2.b;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& l1, const type& l2) noexcept
    {
        return l1.a != l2.a || l1.b != l2.b;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // get anchors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr point_type midpoint() const noexcept
    {
        return (a + b) * static_cast<scalar_type>(0.5);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // attributes
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr scalar_type length() const noexcept { return ::vx::math::length(b - a); }
    VX_FORCE_INLINE constexpr scalar_type length_squared() const noexcept { return ::vx::math::length_squared(b - a); }
    VX_FORCE_INLINE constexpr vec<2, T> vector() const noexcept { return vec<2, T>(b - a); }
};

} // namespace geometry2d
} // namespace math
} // namespace vx