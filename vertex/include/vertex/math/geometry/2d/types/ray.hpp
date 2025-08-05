#pragma once

#include "vertex/math/geometry/2d/types/point.hpp"

namespace vx {
namespace math {
namespace g2 {

template <typename T>
struct ray_t
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = ray_t<scalar_type>;
    using point_type = point_t<scalar_type>;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    point_type origin;
    vec<2, scalar_type> direction;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr ray_t() noexcept = default;

    VX_FORCE_INLINE constexpr ray_t(const type& r) noexcept
        : origin(r.origin), direction(r.direction) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr ray_t(
        scalar_type ox, scalar_type oy,
        scalar_type dx, scalar_type dy
    ) noexcept
        : origin(ox, oy), directon(dx, dy) {}

    VX_FORCE_INLINE constexpr ray_t(
        const point_type& rorigin,
        const vec<2, scalar_type>& rdirection
    ) noexcept
        : origin(rorigin), direction(rdirection) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename OX, typename OY, typename DX, typename DY>
    VX_FORCE_INLINE constexpr ray_t(OX ox, OY oy, DX dx, DY dy) noexcept
        : origin(ox, oy), direction(dx, dy) {}

    template <typename O, typename D>
    VX_FORCE_INLINE constexpr ray_t(
        const point_t<O>& rorigin,
        const vec<2, D>& rdirection
    ) noexcept
        : origin(rorigin), direction(rdirection) {}

    template <typename U>
    VX_FORCE_INLINE constexpr ray_t(const ray_t<U>& r) noexcept
        : origin(r.origin), direction(r.direction) {}

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& r) noexcept
    {
        origin = r.origin;
        direction = r.direction;
        return *this;
    }

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const ray_t<U>& r) noexcept
    {
        origin = r.origin;
        direction = r.direction;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& r1, const type& r2) noexcept
    {
        return r1.origin == r2.origin && r1.direction == r2.direction;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& r1, const type& r2) noexcept
    {
        return r1.origin != r2.origin || r1.direction != r2.direction;
    }

};

} // namespace g2
} // namespace math
} // namespace vx