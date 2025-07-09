#pragma once

#include "./point.hpp"

namespace vx {
namespace math {
namespace geometry2d {

template <typename T>
struct circle_t
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = circle_t<scalar_type>;
    using point_type = point_t<scalar_type>;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    point_type center;
    scalar_type radius = static_cast<scalar_type>(0);

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr circle_t(const type& c) noexcept
        : center(c.center), radius(c.radius) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr circle_t(
        scalar_type x, scalar_type y,
        scalar_type r
    ) noexcept
        : center(x, y), radius(r) {}

    VX_FORCE_INLINE constexpr circle_t(
        const point_type& ccenter,
        scalar_type cradius
    ) noexcept
        : center(ccenter), radius(cradius) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename X, typename Y, typename R>
    VX_FORCE_INLINE constexpr circle_t(X x, Y y, R r) noexcept
        : center(x, y), radius(r) {}

    template <typename C, typename R>
    VX_FORCE_INLINE constexpr circle_t(
        const point<C>& ccenter,
        R cradius
    ) noexcept
        : center(ccenter), radius(cradius) {}

    template <typename U>
    VX_FORCE_INLINE constexpr circle_t(const circle_t<U>& c) noexcept
        : center(c.center), radius(c.radius) {}

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& c) noexcept
    {
        center = c.center;
        radius = c.radius;
        return *this;
    }

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const circle_t<U>& c) noexcept
    {
        center = c.center;
        radius = c.radius;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& c1, const type& c2) noexcept
    {
        return c1.center == c2.center && c1.radius == c2.radius;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& c1, const type& c2) noexcept
    {
        return c1.center != c2.center || c1.radius != c2.radius;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // attributes
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr scalar_type area() const noexcept { return constants<scalar_type>::pi * radius * radius; }
    VX_FORCE_INLINE constexpr bool empty() const noexcept { return radius == static_cast<scalar_type>(0); }

    VX_FORCE_INLINE constexpr bool diameter() const noexcept { return static_cast<T>(2) * radius; }
    VX_FORCE_INLINE constexpr scalar_type perimeter() const noexcept { return static_cast<T>(2) * constants<scalar_type>::pi * radius; }
    VX_FORCE_INLINE constexpr scalar_type circumference() const noexcept { return perimeter(); }

};

} // namespace geometry2d
} // namespace math
} // namespace vx